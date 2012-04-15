#include "transferthread.h"
#include "job.h"
#include "server.h"
#include "settings.h"
#include "messages.h"

#include <util/message.h>
#include <util/exceptions.h>

#include <unistd.h>
#include <sys/select.h>

using namespace os;

#define max( a, b ) (a > b ? a : b)

/** \brief TransferThread Constructor.
 * Initializes the TransferThread creating its pipes, locks and curl objects.
 * \param pcServer The server that this thread is working for.
 */
TransferThread::TransferThread( Server* pcServer ) : Thread( "transfer_thread" )
{
	m_pcServer = pcServer;
	
	m_pcLock = new Locker( "transfer_thread_lock" );
	
	int nTmp[2];
	int nResult;
	nResult = pipe( nTmp );		/* Create a pipe for communication between transfer_thread and others */
	if( nResult ) {
		fprintf( stderr, "Transferrer: ERROR: Unable to create pipe!\n" );
		throw( errno_exception( "TransferThread: Unable to create pipe!" ) );
	}
	m_nReadPipe = nTmp[0];
	m_nWritePipe = nTmp[1];
	
	/* Set the pipe to non-blocking IO */
	fcntl( m_nReadPipe, F_SETFL, (fcntl( m_nReadPipe, F_GETFL ) | O_NONBLOCK) );
	fcntl( m_nWritePipe, F_SETFL, (fcntl( m_nWritePipe, F_GETFL ) | O_NONBLOCK) );
	
	CURLM* pMultiHandle = curl_multi_init();
	if( pMultiHandle == NULL ) {
		fprintf( stderr, "Transferrer: ERROR: Could not create curl multi handle!\n" );
		throw( errno_exception( "Could not create curl multi handle!\n" ) );
	}
	m_pCurlMultiHandle = pMultiHandle;
	
	for( int i = 0; i < m_pcServer->m_pcSettings->GetMaxConnections(); i++ ) {
		CURL* pHandle = curl_easy_init();
		if( m_pcServer->_SetupCurlHandle( pHandle ) != CURLE_OK ) {
			fprintf( stderr, "Transferrer: ERROR: Could not setup curl handle!\n" );
			throw( errno_exception( "Could not setup curl handle!" ) );
		}
		CurlHandle_s* psTmp = new CurlHandle_s;		
		psTmp->m_pHandle = pHandle;
		psTmp->m_nStatus = HANDLE_IDLE;
		psTmp->m_pMultiHandle = m_pCurlMultiHandle;
		curl_easy_setopt( pHandle, CURLOPT_PRIVATE, (void*)psTmp );		/* Save the CurlHandle_s to the curl handle's private data */
		
		m_asHandles.push_back( psTmp );
	}
}

/** \brief TransferThread destructor.
 * All the cleanup is done in _Close(), so we don't need anything here.
 *
 */
TransferThread::~TransferThread()
{
}

/** \brief Lock the Thread.
 * This is used to lock the thread from further access until any operations
 * being performed are complete.
 */
int TransferThread::Lock()
{
	return( m_pcLock->Lock() );
}

/** \brief Unlock the Thread.
 * This unlocks the thread for further operations.
 */
int TransferThread::Unlock()
{
	return( m_pcLock->Unlock() );
}

/** \brief Wrapper for sending messages with numerical codes.
 * This creates a Message object using nCode as a parameter.
 *
 * \param nCode The code to send for the message.
 */
int TransferThread::SendMessage( int nCode )
{
	return( SendMessage( new Message( nCode ) ) );
}

/** \brief Send a message to the thread.
 * The message will be deleted by the thread once it is processed,
 * so it should be created via new Message( code ).
 */
int TransferThread::SendMessage( Message* pcMsg )
{
	Lock();
	m_apcMessages.push( pcMsg );
	Unlock();
	_Notify();
	return( 0 );
}

/** \brief Notify() writes a piece of data to the pipe to wake the transfer 
 * thread from its select() sleep. This is called from gui threads.
 */
void TransferThread::_Notify()
{
	if( get_thread_id( NULL ) == GetThreadId() ) {
//		DEBUG( "Warning: TransferThread::_Notify() called from transfer thread!\n" );
		return;
	}	
	write( m_nWritePipe, "x", 1 );		/* This will wake the transfer_thread from its select() sleep */
}

/** \brief _CleanupJob() removes a job from the list and deletes it.
 * The job should already have been detached from any CURL handles and should have
 * already safely finished its work (eg, flushed dirty data to disk).
 *
 * \param nJob The Job ID to remove.
 *
 */
void TransferThread::_CleanupJob( int nJob )
{
	m_pcServer->Lock();
	if( m_pcServer->m_apcQueue.count( nJob ) == 0 ) {
		/* Sanity checking - better safe than sorry */
		DEBUG( "TransferThread:_CleanupJob( %i ): job %i doesn't exist or has already been cleaned up!!\n", nJob, nJob );
		m_pcServer->Unlock();
		return;
	}
	Job* pcJob = m_pcServer->m_apcQueue[ nJob ];
	m_pcServer->m_apcQueue.erase( nJob );
	delete( pcJob );
	
	m_pcServer->Unlock();
}

/** \brief Clean up and stop transfer thread.
 *  Cleans up all transfers, frees curl handles, deletes the Server object and terminates the transfer thread.
 */
void TransferThread::_Close()
{
	/* TODO: Notify gui somewhere? */
	/* Clean up all transfers, free curl handles and delete the Server then ourself */
	DEBUG( "TransferThread::_Close(); performing cleanup\n" );

	m_pcServer->Lock();
	/* Delete all jobs from the queue */
	while( !m_pcServer->m_apcQueue.empty() )
	{
		_CleanupJob( m_pcServer->m_apcQueue.begin()->first );
	}
	m_pcServer->Unlock();

	/* Close curl handles and delete CurlHandle_s structs */
	for( int i = 0; i < m_asHandles.size(); i++ ) {
		CurlHandle_s* pcHandle = m_asHandles[i];
		curl_easy_cleanup( pcHandle->m_pHandle );
		delete( pcHandle );
	}
	curl_multi_cleanup( m_pCurlMultiHandle );

	/* Delete the Server object */
	delete( m_pcServer );

	/* Delete any remaining messages */
	m_pcLock->Lock();
	while( !m_apcMessages.empty() ) {
		Message* pcTmp = m_apcMessages.front();
		m_apcMessages.pop();
		delete( pcTmp );
	}
	m_pcLock->Unlock();
	delete( m_pcLock );

	/* Close the pipe */
	close( m_nReadPipe );
	close( m_nWritePipe );
	fflush( stdout );		/* Because we get some wierdness in the terminal when two threads are printf'ing at the same time */

	/* Delete ourself and quit the thread */
	delete( this );		/*	NOTE: I don't think this is strictly kosher with the current libsyllable.
							The os::Thread destructor calls Terminate() which calls kill( our thread, SIGKILL ).
							So the destructor never returns and the TransferThread object's memory isn't freed.
							So the next line currently isn't reached, and the TransferThread memory is leaked.
							This probably should be fixed in libsyllable. When it is, hopefully this code will be correct.
						*/
	exit_thread( 0 );
}


/** \brief Pauses the given transfer.
 *  Sets the given job's status to paused, and detaches it from a curl handle, if it was previously attached.
 */
void TransferThread::_PauseJob( int nID )
{
	m_pcServer->Lock();

	/* Find the job */
	std::map< int, Job* >::iterator i = m_pcServer->m_apcQueue.find( nID );
	if( i == m_pcServer->m_apcQueue.end() ) {
		DEBUG( "TransferThread: Got M_PAUSE for non-existent job %i!\n", nID );
		m_pcServer->Unlock();
		return;
	}
	Job* pcJob = i->second;

	if( !STATUS_IS_ACTIVE( pcJob->m_nStatus ) ) {
		DEBUG( "TransferThread: Got M_PAUSE for job %i which is not active!\n", nID );
		m_pcServer->Unlock();
		return;
	}

	pcJob->Pause();

	m_pcServer->Unlock();

	DEBUG( "TransferThread: Paused transfer %i (%x)\n", nID, pcJob );
}

/* \brief Resumes a paused transfer.
 *  Sets the given job's status to queued. It may then be attached to a curl handle by _Schedule().
 */
void TransferThread::_ResumeJob( int nID )
{
	m_pcServer->Lock();

	/* Find the job */
	std::map< int, Job* >::iterator i = m_pcServer->m_apcQueue.find( nID );
	if( i == m_pcServer->m_apcQueue.end() ) {
		DEBUG( "TransferThread: Got M_RESUME for non-existent job %i!\n", nID );
		m_pcServer->Unlock();
		return;
	}
	Job* pcJob = i->second;

	if( pcJob->m_nStatus != STATUS_PAUSED ) {
		DEBUG( "TransferThread: Got M_RESUME for job %i which is not paused!\n", nID );
		m_pcServer->Unlock();
		return;
	}

	pcJob->Resume();
	m_pcServer->Unlock();

	DEBUG( "TransferThread: Resumed transfer %i\n", pcJob->m_nID );
}

/** \brief Cancels the given queue job and deletes it.
 */
void TransferThread::_CancelJob( int nID )
{
	m_pcServer->Lock();

	/* Find the job */
	std::map< int, Job* >::iterator i = m_pcServer->m_apcQueue.find( nID );
	if( i == m_pcServer->m_apcQueue.end() ) {
		DEBUG( "TransferThread: Got M_CANCEL for non-existent job %i!\n", nID );
		m_pcServer->Unlock();
		return;
	}
	Job* pcJob = i->second;
	
	if( !STATUS_IS_ACTIVE( pcJob->m_nStatus ) && !STATUS_IS_SUSPENDED( pcJob->m_nStatus ) ) {
		DEBUG( "TransferThread: Got M_CANCEL for job %i which is not active or suspended!\n", nID );
		m_pcServer->Unlock();
		return;
	}

	pcJob->Cancel();
	m_pcServer->Unlock();

	DEBUG( "TransferThread: Cancelled transfer %i\n", nID );
}


/** \brief Assign queued jobs to idle curl handles.
 * Attaches a suitable job from the queue to each idle curl handle.
 */
void TransferThread::_Schedule()
{
//	DEBUG( "_Schedule()\n" );
	for( uint i = 0; i < m_asHandles.size(); i++ )
	{
		CurlHandle_s* psHandle = m_asHandles[i];
		if( psHandle->m_nStatus == HANDLE_IDLE )
		{
//			DEBUG( "_Schedule(): curl handle %x is free\n", psHandle->m_pHandle );
			/* This curl handle is idle. Find the next queued job from the queue and setup the curl handle */
			m_pcServer->Lock();
			std::map< int, Job* >::iterator j = m_pcServer->m_apcQueue.begin();
			for( ; j != m_pcServer->m_apcQueue.end(); j++ )
			{
				Job* pcJob = j->second;
				if( pcJob->m_nStatus == STATUS_QUEUED )
				{
					char* pzType;	/* Just for the next DEBUG message */
					switch( pcJob->m_nType ) {
						case JOB_DOWNLOAD: pzType = "download"; break;
						case JOB_UPLOAD: pzType = "upload"; break;
						case JOB_DIRLIST: pzType = "dirlist"; break;
						case JOB_COMMAND: pzType = "command"; break;
						default: pzType = "UNKNOWN";
					}
					DEBUG( " _Schedule(): assigning %s job %i to handle %x\n", pzType, pcJob->m_nID, psHandle->m_pHandle );
					int nResult;
					nResult = pcJob->AttachToHandle( psHandle );
					if( nResult != CURLE_OK ) {
						DEBUG( "_Schedule(): Job::AttachToHandle() failed for handle %x, job %i.\n", psHandle->m_pHandle, pcJob->m_nID );
						continue;
					}
					break;
				}
			}
			if( j == m_pcServer->m_apcQueue.end() ) DEBUG( " _Schedule(): handle %x is idle.\n", psHandle->m_pHandle );
			m_pcServer->Unlock();
		} else DEBUG( " _Schedule(): handle %x is busy %i\n", psHandle->m_pHandle, psHandle->m_nStatus );
	}
}


/** \brief Checks for messages from other threads and carries out the appropriate action.
 * Only called from the transfer thread.
 * \return Returns true if _Schedule() needs to be called; false if not.
 */
bool TransferThread::_ProcessMessages()
{
	bool bWantSchedule = false;
	Lock();
	while( !m_apcMessages.empty() ) {

		Message* pcMsg = m_apcMessages.front();
		m_apcMessages.pop();

		
		switch( pcMsg->GetCode() )
		{
			case M_TT_CLOSE:
			{
				delete( pcMsg );
				_Close();
				break;
			}
			case M_TT_PAUSE:
			{
				int nID;
				if( pcMsg->FindInt32( "id", &nID ) != 0 ) {
					DEBUG( "TransferThread: Got M_PAUSE without id!\n" );
					break;
				}
				_PauseJob( nID );
				bWantSchedule = true;
				break;
			}
			case M_TT_RESUME:
			{
				int nID;
				if( pcMsg->FindInt32( "id", &nID ) != 0 ) {
					DEBUG( "TransferThread: Got M_RESUME without id!\n" );
					break;
				}
				_ResumeJob( nID );
				bWantSchedule = true;
				break;

			}
			case M_TT_CANCEL:
			{
				int nID;
				if( pcMsg->FindInt32( "id", &nID ) != 0 ) {
					DEBUG( "TransferThread: Got M_CANCEL without id!\n" );
					break;
				}
				_CancelJob( nID );
				bWantSchedule = true;
				break;
			}
			case M_TT_REMOVE:
			{
				int nID;
				if( pcMsg->FindInt32( "id", &nID ) != 0 ) {
					DEBUG( "TransferThread: Got M_REMOVE without id!\n" );
					break;
				}
				_CleanupJob( nID );
				bWantSchedule = true;
				break;
			}
			case M_TT_SCHEDULE:
			{
				bWantSchedule = true;
				break;
			}
			case M_GUI_OVERWRITE_REPLY:
			{
				int nID;
				if( pcMsg->FindInt32( "id", &nID ) != 0 ) {
					DEBUG( "TransferThreadL Got M_GUI_OVERWRITE_REPLY without id!\n" );
					break;
				}
				m_pcServer->Lock();
				std::map< int, Job* >::iterator i = m_pcServer->m_apcQueue.find( nID );
				if( i == m_pcServer->m_apcQueue.end() ) {
					DEBUG( "TransferThread: Got M_GUI_OVERWRITE_REPLY for non-existent job %i!\n", nID );
					m_pcServer->Unlock();
					break;
				}
				Job* pcJob = i->second;
				pcJob->HandleOverwriteReply( pcMsg );
				m_pcServer->Unlock();
				bWantSchedule = true;
				break;
			}
			default:
			{
				DEBUG( "TransferThread::_ProcessMessages(): Got unknown message code %i!\n", pcMsg->GetCode() );
				break;
			}
		}
		delete( pcMsg );
	}
	Unlock();
	return( bWantSchedule );
}

/** \brief The main loop of the thread.
 */
int32 TransferThread::Run()
{
	DEBUG( "Transfer thread: TID %i\n", GetThreadId() );
	fd_set sReadSet, sWriteSet, sExcSet;
	struct timeval sSelectTime;
	int nMaxFD;

	CURLMcode nResult;
	int nRunning;
	
	/* TODO: if there are no transfers active, then call select only on our port with infinite timeout to save on unneeded calls to _ProcessMessages() and curl_multi_process() */
	while( 1 ) {
		FD_ZERO( &sReadSet );
		FD_ZERO( &sWriteSet );
		FD_ZERO( &sExcSet );

		/* Get curl's file descriptors for later call to select() */
		nResult = curl_multi_fdset( m_pCurlMultiHandle, &sReadSet, &sWriteSet, &sExcSet, &nMaxFD );
		if( nResult != CURLM_OK ) {
			DEBUG( "TransferThread: curl_multi_fdset returned %i (%s)!\n", nResult, curl_multi_strerror( nResult ) );
			/* continue ? */
		}
		FD_SET( m_nReadPipe, &sReadSet );
		nMaxFD = max( nMaxFD, m_nReadPipe );
		
		/* Get curl's preferred timeout */
		long nTimeout;
		nResult = curl_multi_timeout( m_pCurlMultiHandle, &nTimeout );
		if( nResult != CURLM_OK ) {
			DEBUG( "TransferThread: curl_multi_timeout returned %i (%s)!\n", nResult, curl_multi_strerror( nResult ) );
		}
		if( nTimeout == -1 ) nTimeout = 1000000;	/* curl doesn't have a preference; default timeout 1 second */
		if( nTimeout > 1000000 ) nTimeout = 1000000;	/* max 1 second timeout */
		sSelectTime.tv_sec = nTimeout / 1000000;
		sSelectTime.tv_usec = nTimeout % 1000000;
		
		/* Wait for some data to arrive or another thread to message us, via select() */
		int nCount;
		nCount = select( nMaxFD + 1, &sReadSet, &sWriteSet, &sExcSet, &sSelectTime );

		/* Clear the data from the pipe */
		char zBuf[16];
		while( read( m_nReadPipe, zBuf, 16 ) > 0 ) ;

		/* Check for messages from other threads */
		bool bNeedSchedule = false;
		bNeedSchedule = _ProcessMessages();

		/* Call curl_multi_perform() to transfer some data until it is happy. */
		/* Curl will call our callback functions Transfer::ReadCallback() etc to read data etc. */
		while( curl_multi_perform( m_pCurlMultiHandle, &nRunning ) == CURLM_CALL_MULTI_PERFORM ) {
//			DEBUG( "TransferThread::Run(): %i transfers still running.\n", nRunning );
		}
		
		/* Get any responses from libcurl */
		CURLMsg* psMsg;
		int nRemaining;
		while( (psMsg = curl_multi_info_read( m_pCurlMultiHandle, &nRemaining )) != NULL )
		{
//			DEBUG( "curl_multi_info_read(): code %i, handle %x, result %i, %i messages remaining\n", psMsg->msg, psMsg->easy_handle, psMsg->data.result, nRemaining );
			if( psMsg->msg == CURLMSG_DONE )
			{
				/* Get the associated handle & job */
				CurlHandle_s* psHandle;
				curl_easy_getinfo( psMsg->easy_handle, CURLINFO_PRIVATE, (char**)&psHandle );	/* Curl docs say to pass the pointer as char** */
				int nJobID = psHandle->m_nStatus;
				long nCode;
				curl_easy_getinfo( psMsg->easy_handle, CURLINFO_RESPONSE_CODE, &nCode );
				DEBUG( " handle %x for job %i reports response %li\n", psMsg->easy_handle, nJobID, nCode );

				if( nJobID == -1 ) {
					DEBUG( "TransferThread::Run(): Got job id -1 from finished curl handle!\n" );
					continue;
				}
				
				m_pcServer->Lock();
				Job* pcJob = m_pcServer->m_apcQueue[ nJobID ];
				pcJob->TransferFinished( psHandle, nCode );
				m_pcServer->Unlock();
				/* It is up to the job to clean itself up, free the handle and delete itself if necessary */
				
				bNeedSchedule = true;
			} else { DEBUG( "TransferThread: Got unknown message code %i from curl_multi_info_read()!\n", psMsg->msg ); }
		}
		if( bNeedSchedule ) {
			_Schedule();
		}
	}

	return( 0 );
}


