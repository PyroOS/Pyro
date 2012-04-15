/* Journal - transaction and logging
**
** Initial version by Axel Dörfler, axeld@pinc-software.de
** This file may be used under the terms of the OpenBeOS License.
*/

#include "Utility.h"
#include "Journal.h"
#include "Inode.h"
#include "Debug.h"

#include <atheos/time.h>
#include "kernel_cpp.h"


Journal::Journal(Volume *volume)
	:
	fVolume(volume),
	fLock("bfs journal"),
	fOwner(NULL),
	fArray(volume->BlockSize()),
	fLogSize(volume->Log().length),
	fMaxTransactionSize(fLogSize / 4 - 5),
	fUsed(0),
	fTransactionsInEntry(0)
{
	if (fMaxTransactionSize > fLogSize / 2)
		fMaxTransactionSize = fLogSize / 2 - 5;
}


Journal::~Journal()
{
	FlushLogAndBlocks();
}


status_t
Journal::InitCheck()
{
	if (fVolume->LogStart() != fVolume->LogEnd()) {
		if (fVolume->SuperBlock().flags != SUPER_BLOCK_DISK_DIRTY)
			FATAL(("log_start and log_end differ, but disk is marked clean - trying to replay log...\n"));

		return ReplayLog();
	}

	return B_OK;
}


status_t
Journal::CheckLogEntry(int32 count, off_t *array)
{
	// ToDo: check log entry integrity (block numbers and entry size)
	PRINT(("Log entry has %ld entries (%Ld)\n", count, array[0]));
	return B_OK;
}


status_t
Journal::ReplayLogEntry(int32 *_start)
{
	PRINT(("ReplayLogEntry(start = %ld)\n", *_start));

	off_t logOffset = fVolume->ToBlock(fVolume->Log());
	off_t arrayBlock = (*_start % fLogSize) + fVolume->ToBlock(fVolume->Log());
	int32 blockSize = fVolume->BlockSize();
	int32 count = 1, valuesInBlock = blockSize / sizeof(off_t);
	int32 numArrayBlocks;
	off_t blockNumber = 0;
	bool first = true;

	CachedBlock cached(fVolume);
	while (count > 0) {
		off_t *array = (off_t *)cached.SetTo(arrayBlock);
		if (array == NULL)
			return B_IO_ERROR;

		int32 index = 0;
		if (first) {
			if (array[0] < 1 || array[0] >= fLogSize)
				return B_BAD_DATA;

			count = array[0];
			first = false;

			numArrayBlocks = ((count + 1) * sizeof(off_t) + blockSize - 1) / blockSize;
			blockNumber = (*_start + numArrayBlocks) % fLogSize;
				// first real block in this log entry
			*_start += count;
			index++;
				// the first entry in the first block is the number
				// of blocks in that log entry
		}
		(*_start)++;

		if (CheckLogEntry(count, array + 1) < B_OK)
			return B_BAD_DATA;

		CachedBlock cachedCopy(fVolume);
		for (; index < valuesInBlock && count-- > 0; index++) {
			PRINT(("replay block %Ld in log at %Ld!\n", array[index], blockNumber));

			uint8 *copy = cachedCopy.SetTo(logOffset + blockNumber);
			if (copy == NULL)
				RETURN_ERROR(B_IO_ERROR);

			ssize_t written = write_pos(fVolume->Device(),
						array[index] << fVolume->BlockShift(), copy, blockSize);
			if (written != blockSize)
				RETURN_ERROR(B_IO_ERROR);

			blockNumber = (blockNumber + 1) % fLogSize;
		}
		arrayBlock++;
		if (arrayBlock > fVolume->ToBlock(fVolume->Log()) + fLogSize)
			arrayBlock = fVolume->ToBlock(fVolume->Log());
	}
	return B_OK;
}


/**	Replays all log entries - this will put the disk into a
 *	consistent and clean state, if it was not correctly unmounted
 *	before.
 *	This method is called by Journal::InitCheck() if the log start
 *	and end pointer don't match.
 */

status_t
Journal::ReplayLog()
{
	INFORM(("Replay log, disk was not correctly unmounted...\n"));

	int32 start = fVolume->LogStart();
	int32 lastStart = -1;
	while (true) {
		// stop if the log is completely flushed
		if (start == fVolume->LogEnd())
			break;

		if (start == lastStart) {
			// strange, flushing the log hasn't changed the log_start pointer
			return B_ERROR;
		}
		lastStart = start;

		status_t status = ReplayLogEntry(&start);
		if (status < B_OK) {
			FATAL(("replaying log entry from %ld failed: %s\n", start, strerror(status)));
			return B_ERROR;
		}
		start = start % fLogSize;
	}
	
	PRINT(("replaying worked fine!\n"));
	fVolume->SuperBlock().log_start = fVolume->LogEnd();
	fVolume->LogStart() = fVolume->LogEnd();
	fVolume->SuperBlock().flags = SUPER_BLOCK_DISK_CLEAN;

	return fVolume->WriteSuperBlock();
}


/**	This is a callback function that is called by the cache, whenever
 *	a block is flushed to disk that was updated as part of a transaction.
 *	This is necessary to keep track of completed transactions, to be
 *	able to update the log start pointer.
 */

void
Journal::blockNotify(off_t blockNumber, int numBlocks, void *arg)
{
	log_entry *logEntry = (log_entry *)arg;
	//printk("block %i %i blocks to go\n", (int)blockNumber, (int)logEntry->cached_blocks );
	logEntry->cached_blocks -= numBlocks;
	if (logEntry->cached_blocks > 0) {
		// nothing to do yet...
		return;
	}
	

	Journal *journal = logEntry->journal;
	disk_super_block &superBlock = journal->fVolume->SuperBlock();
	bool update = false;

	// Set log_start pointer if possible...

	if (logEntry == journal->fEntries.head) {
		//printk("Update log!\n" );
		if (logEntry->Next() != NULL) {
			int32 length = logEntry->next->start - logEntry->start;
			superBlock.log_start = (superBlock.log_start + length) % journal->fLogSize;
		} else
			superBlock.log_start = journal->fVolume->LogEnd();

		update = true;
	}
	journal->fUsed -= logEntry->length;

	journal->fEntriesLock.Lock();
	logEntry->Remove();
	journal->fEntriesLock.Unlock();

	free(logEntry);

	// update the super block, and change the disk's state, if necessary

	if (update) {
		journal->fVolume->LogStart() = superBlock.log_start;

		if (superBlock.log_start == superBlock.log_end)
			superBlock.flags = SUPER_BLOCK_DISK_CLEAN;
		//printk("Write superblock %i %i\n",(int)journal->fVolume->LogStart(), (int)journal->fVolume->LogEnd() );
		journal->fVolume->WriteSuperBlock();
	}
}


status_t
Journal::WriteLogEntry()
{
	fTransactionsInEntry = 0;
	fHasChangedBlocks = false;
	
	sorted_array *array = fArray.Array();
	if (array == NULL || array->count == 0)
		return B_OK;
	
	// Make sure there is enough space in the log.
	// If that fails for whatever reason, panic!
	flush_device_cache(fVolume->Device(), true);
	flush_locked_device_cache(fVolume->Device(), true);
	int32 tries = fLogSize / 2 + 1;
	while (TransactionSize() > FreeLogBlocks() && tries-- > 0)
	{
		flush_device_cache(fVolume->Device(), true);
		flush_locked_device_cache(fVolume->Device(), true);
		//flush_inodes(fVolume->ID());
		printk( "BFS panic: Journal full -> trying to flush blocks...\n" );
	}

	if (tries <= 0) {
		fVolume->Panic();
		return B_BAD_DATA;
	}

	int32 blockShift = fVolume->BlockShift();
	off_t logOffset = fVolume->ToBlock(fVolume->Log()) << blockShift;
	off_t logStart = fVolume->LogEnd();
	off_t logPosition = logStart % fLogSize;

	// Write disk block array

	uint8 *arrayBlock = (uint8 *)array;

	for (int32 size = fArray.BlocksUsed(); size-- > 0;) {
		write_pos(fVolume->Device(), logOffset + (logPosition << blockShift),
			arrayBlock, fVolume->BlockSize());

		logPosition = (logPosition + 1) % fLogSize;
		arrayBlock += fVolume->BlockSize();
	}

	// Write logged blocks into the log

	CachedBlock cached(fVolume);
	for (int32 i = 0;i < array->count;i++) {
		// ToDo: combine blocks if possible (using iovecs)!
		uint8 *block = cached.SetTo(array->values[i]);
		if (block == NULL)
			return B_IO_ERROR;

		write_pos(fVolume->Device(), logOffset + (logPosition << blockShift),
			block, fVolume->BlockSize());
		logPosition = (logPosition + 1) % fLogSize;
	}

	log_entry *logEntry = (log_entry *)malloc(sizeof(log_entry));
	if (logEntry != NULL) {
		logEntry->start = logStart;
		logEntry->length = TransactionSize();
		logEntry->cached_blocks = array->count;
		logEntry->journal = this;

		fEntriesLock.Lock();
		fEntries.Add(logEntry);
		fEntriesLock.Unlock();

		fCurrent = logEntry;
		fUsed += logEntry->length;
		
		// On Syllable we need to lock the blocks before calling set_blocks_info
		//printk("Used inodes %i %i\n", get_cached_inodes(fVolume->ID(), true ), get_cached_inodes(fVolume->ID(), false ) );
		printk("Needed /Free %i %i\n", TransactionSize(), FreeLogBlocks() );
		for (int32 i = 0;i < array->count;i++)
		{
			void* block = get_cache_block(fVolume->Device(), array->values[i], fVolume->BlockSize());
			set_blocks_info(fVolume->Device(), &array->values[i], 1, false, blockNotify, logEntry);
			release_cache_block(fVolume->Device(), array->values[i] );
			//printk("Cache block %i\n", array->values[i] );
		}

	} else
		printk("Error: Could not allocate log block!\n" );

	// If the log goes to the next round (the log is written as a
	// circular buffer), all blocks will be flushed out which is
	// possible because we don't have any locked blocks at this
	// point.
	if (logPosition < logStart)
		fVolume->FlushDevice();

	// We need to flush the drives own cache here to ensure
	// disk consistency.
	// If that call fails, we can't do anything about it anyway
//	ioctl(fVolume->Device(), B_FLUSH_DRIVE_CACHE);

		
	fArray.MakeEmpty();
	
	// Update the log end pointer in the super block
	fVolume->SuperBlock().flags = SUPER_BLOCK_DISK_DIRTY;
	fVolume->SuperBlock().log_end = logPosition;
	fVolume->LogEnd() = logPosition;
	
	return fVolume->WriteSuperBlock();
}


status_t 
Journal::FlushLogAndBlocks()
{
	status_t status = Lock((Transaction *)this);
	if (status != B_OK)
		return status;

	// write the current log entry to disk
	
	if (TransactionSize() != 0) {
		status = WriteLogEntry();
		if (status < B_OK)
			FATAL(("writing current log entry failed: %s\n", strerror(status)));
	}
	flush_locked_device_cache(fVolume->Device(), true);
	status = fVolume->FlushDevice();

	Unlock((Transaction *)this, true);
	return status;
}


status_t
Journal::Lock(Transaction *owner)
{
	if (owner == fOwner)
		return B_OK;

	status_t status = fLock.Lock();
	if (status == B_OK)
		fOwner = owner;

	// if the last transaction is older than 2 secs, start a new one
	if (fTransactionsInEntry != 0 && get_system_time() - fTimestamp > 2000000L)
		WriteLogEntry();
	
	return B_OK;
}


void
Journal::Unlock(Transaction *owner, bool success)
{
	if (owner != fOwner)
		return;

	TransactionDone(success);

	fTimestamp = get_system_time();
	fOwner = NULL;
	fLock.Unlock();
}


/** If there is a current transaction that the current thread has
 *	started, this function will give you access to it.
 */

Transaction *
Journal::CurrentTransaction()
{
	if (fLock.LockWithTimeout(0) != B_OK)
		return NULL;

	Transaction *owner = fOwner;
	fLock.Unlock();

	return owner;
}


status_t
Journal::TransactionDone(bool success)
{
	if (!success && fTransactionsInEntry == 0) {
		// we can safely abort the transaction
		sorted_array *array = fArray.Array();
		if (array != NULL) {
			// release the lock for all blocks in the array (we don't need
			// to be notified when they are actually written to disk)
			for (int32 i = 0; i < array->count; i++)
				release_cache_block(fVolume->Device(), array->values[i]);
		}

		return B_OK;
	}

	// Up to a maximum size, we will just batch several
	// transactions together to improve speed
	if (TransactionSize() < fMaxTransactionSize) {
		fTransactionsInEntry++;
		fHasChangedBlocks = false;

		return B_OK;
	}

	return WriteLogEntry();
}


status_t
Journal::LogBlocks(off_t blockNumber, const uint8 *buffer, size_t numBlocks)
{
	// ToDo: that's for now - we should change the log file size here
	if (TransactionSize() + numBlocks + 1 > fLogSize)
		return B_DEVICE_FULL;

	fHasChangedBlocks = true;
	int32 blockSize = fVolume->BlockSize();

	for (;numBlocks-- > 0; blockNumber++, buffer += blockSize) {
		if (fArray.Find(blockNumber) >= 0) {
			// The block is already in the log, so just update its data
			// Note, this is only necessary if this method is called with a buffer
			// different from the cached block buffer - which is unlikely but
			// we'll make sure this way (costs one cache lookup, though).
			status_t status = cached_write(fVolume->Device(), blockNumber, buffer, 1, blockSize);
			if (status < B_OK)
				return status;

			continue;
		}

		// Insert the block into the transaction's array, and write the changes
		// back into the locked cache buffer
		fArray.Insert(blockNumber);
		status_t status = cached_write(fVolume->Device(), blockNumber, buffer, 1, blockSize);
		if (status < B_OK)
			return status;
	}

	// If necessary, flush the log, so that we have enough space for this transaction
	if (TransactionSize() > FreeLogBlocks())
	{
		flush_device_cache(fVolume->Device(), true);
		flush_locked_device_cache(fVolume->Device(), true);
	}

	return B_OK;
}


//	#pragma mark -


status_t 
Transaction::Start(Volume *volume, off_t refBlock)
{
	// has it already been started?
	if (fJournal != NULL)
		return B_OK;

	fJournal = volume->GetJournal(refBlock);
	if (fJournal != NULL && fJournal->Lock(this) == B_OK)
		return B_OK;

	fJournal = NULL;
	return B_ERROR;
}

