#ifndef JOURNAL_H
#define JOURNAL_H
/* Journal - transaction and logging
**
** Copyright 2001-2004, Axel Dörfler, axeld@pinc-software.de
** This file may be used under the terms of the OpenBeOS License.
*/



#include <atheos/kernel.h>

#ifdef USER
//#	include "myfs.h"
#	include <stdio.h>
#endif

#ifndef _IMPEXP_KERNEL
#	define _IMPEXP_KERNEL
#endif

#include "Volume.h"
#include "Chain.h"
#include "Utility.h"


struct log_entry : node<log_entry> {
	uint16		start;
	uint16		length;
	uint32		cached_blocks;
	Journal		*journal;
};


// Locking policy in BFS: if you need both, the volume lock and the
//	journal lock, you must lock the volume first - or else you will
//	end up in a deadlock.
//	That is, if you start a transaction, and will need to lock the
//	volume while the transaction is in progress (for the unsafe
//	get_vnode() call, for example), you must lock the volume before
//	starting the transaction.

class Journal {
	public:
		Journal(Volume *);
		~Journal();
		
		status_t InitCheck();

		status_t Lock(Transaction *owner);
		void Unlock(Transaction *owner, bool success);

		status_t CheckLogEntry(int32 count, off_t *array);
		status_t ReplayLogEntry(int32 *start);
		status_t ReplayLog();

		status_t WriteLogEntry();
		status_t LogBlocks(off_t blockNumber, const uint8 *buffer, size_t numBlocks);

		Transaction *CurrentTransaction();
		uint32 TransactionSize() const { return fArray.CountItems() + fArray.BlocksUsed(); }

		status_t FlushLogAndBlocks();
		Volume *GetVolume() const { return fVolume; }

		inline uint32 FreeLogBlocks() const;

	private:
		friend struct log_entry;

		static void blockNotify(off_t blockNumber, int numBlocks, void *arg);
		status_t TransactionDone(bool success);

		Volume		*fVolume;
		RecursiveLock	fLock;
		Transaction *fOwner;
		BlockArray	fArray;
		uint32		fLogSize, fMaxTransactionSize, fUsed;
		int32		fTransactionsInEntry;
		SimpleLock	fEntriesLock;
		list<log_entry>	fEntries;
		log_entry	*fCurrent;
		bool		fHasChangedBlocks;
		bigtime_t	fTimestamp;
};


inline uint32 
Journal::FreeLogBlocks() const
{
	return fVolume->LogStart() <= fVolume->LogEnd() ?
		fLogSize - fVolume->LogEnd() + fVolume->LogStart()
		: fVolume->LogStart() - fVolume->LogEnd();
}


// For now, that's only a dumb class that does more or less nothing
// else than writing the blocks directly to the real location.
// It doesn't yet use logging.

class Transaction {
	public:
		Transaction(Volume *volume, off_t refBlock)
			:
			fJournal(NULL)
		{
			Start(volume, refBlock);
		}

		Transaction(Volume *volume, block_run refRun)
			:
			fJournal(NULL)
		{
			Start(volume, volume->ToBlock(refRun));
		}

		Transaction()
			:
			fJournal(NULL)
		{
		}

		~Transaction()
		{
			if (fJournal)
				fJournal->Unlock(this, false);
		}

		status_t Start(Volume *volume, off_t refBlock);
		bool IsStarted() const { return fJournal != NULL; }

		void Done()
		{
			if (fJournal != NULL)
				fJournal->Unlock(this, true);
			fJournal = NULL;
		}

		status_t WriteBlocks(off_t blockNumber, const uint8 *buffer, size_t numBlocks = 1)
		{
			if (fJournal == NULL)
				return B_NO_INIT;

			return fJournal->LogBlocks(blockNumber, buffer, numBlocks);
		}

		Volume	*GetVolume() { return fJournal != NULL ? fJournal->GetVolume() : NULL; }

	private:
		Transaction(const Transaction &);
		Transaction &operator=(const Transaction &);
			// no implementation

		Journal	*fJournal;
};

#endif	/* JOURNAL_H */
