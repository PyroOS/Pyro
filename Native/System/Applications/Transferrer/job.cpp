#include <curl/curl.h>
#include <storage/file.h>
#include <storage/directory.h>
#include <storage/filereference.h>
#include <util/messenger.h>
#include <util/datetime.h>
#include <sys/stat.h>

#include "job.h"
#include "server.h"
#include "remotenode.h"
#include "messages.h"

/* Include: extern int ftpparse(struct ftpparse *,const char *,int); */
extern "C" {
#include "ftpparse.h"
};

/** \brief Job Constructor.
 * Initializes the Job storing all the data related to the job.
 */
Job::Job( Server* pcServer, int nType, int nID, Job* pcParent )
{
	m_pcServer = pcServer;
	
	m_nType = nType;
	m_nStatus = STATUS_QUEUED;
	m_nID = nID;
	m_pcParent = pcParent;
	m_psHandle = NULL;
	m_pzErrorBuffer = new char[CURL_ERROR_SIZE];
	m_pzErrorBuffer[0] = 0;
	m_nResponseCode = 0;
	m_eOverwritePolicy = OP_ASK;
	m_eFailurePolicy = FP_ASK;
}


/** \brief Job Default constructor.
 */
Job::~Job()
{
	delete[] m_pzErrorBuffer;
}


Job* Job::GetParent()
{
	return( m_pcParent );
}

Job* Job::GetAncestor()
{
	if(	m_pcParent == NULL ) return( this );
	else return( m_pcParent->GetAncestor() );
}

bool Job::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddInt32( "jobID", m_nID );
	pcMsg->AddInt32( "status", m_nStatus );
	pcMsg->AddInt32( "type", m_nType );
	pcMsg->AddInt32( "parent", (m_pcParent ? m_pcParent->m_nID : -1) );
	
	if( STATUS_IS_FINISHED( m_nStatus ) ) {
		pcMsg->AddBool( "finished", true );
	}

	switch( m_nStatus ) {
		case STATUS_QUEUED: pcMsg->AddString( "jobStatus", "Queued" ); break;
		case STATUS_RUNNING: pcMsg->AddString( "jobStatus", "Running" ); break;
		case STATUS_PAUSED: pcMsg->AddString( "jobStatus", "Paused" ); break;
		case STATUS_PENDING_USER_RESPONSE: pcMsg->AddString( "jobStatus", "Awaiting response" ); break;
		case STATUS_PENDING_CHILD_JOB: pcMsg->AddString( "jobStatus", "Running..." );
		case STATUS_COMPLETED: pcMsg->AddString( "jobStatus", "Completed" ); break;
		case STATUS_CANCELLED: pcMsg->AddString( "jobStatus", "Cancelled" ); break;
		case STATUS_SKIPPED: pcMsg->AddString( "jobStatus", "Skipped" ); break;
		case STATUS_FAILED: pcMsg->AddString( "jobStatus", "Failed" ); break;	/* TODO: better description? Maybe include error message */
		default: pcMsg->AddString( "jobStatus", "Unknown!" ); break;
	}
	
	return( true );
}

/* \brief Prepare the job for deletion and notify the transfer thread to delete it.
 */
void Job::FinaliseJob()
{
	if( m_psHandle ) RemoveFromHandle( m_psHandle );
	m_pcServer->JobUpdated( this );
	m_pcServer->RemoveJob( this );

}

/** \brief Set the necessary options in the curl handle.
 * Here we set the basic options like callbacks. We also associate ourself with the CurlHandle_s by setting m_nStatus.
 */
int Job::AttachToHandle( CurlHandle_s* psHandle )
{
	if( m_nStatus != STATUS_QUEUED ) {
		DEBUG( "WARNING: AttachToHandle() for job %i in non-queued state %i!\n", m_nID, m_nStatus );
	}
	
	m_psHandle = psHandle;
	m_nStatus = STATUS_RUNNING;
	psHandle->m_nStatus = m_nID;
	
	CURL* pHandle = psHandle->m_pHandle;
	
	/* Call curl_easy_setopt() to set url, callbacks, etc. */
	/* We set some general options here; specifics like resume position etc will be set by subclasses. */

	/* Set callback functions */
	curl_easy_setopt( pHandle, CURLOPT_WRITEFUNCTION, Job::WriteCallback );
	curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, this );
	curl_easy_setopt( pHandle, CURLOPT_READFUNCTION, Job::ReadCallback );
	curl_easy_setopt( pHandle, CURLOPT_READDATA, this );
	curl_easy_setopt( pHandle, CURLOPT_SEEKFUNCTION, Job::SeekCallback );	/* Not available in older versions of libcurl */
	curl_easy_setopt( pHandle, CURLOPT_SEEKDATA, this );
	curl_easy_setopt( pHandle, CURLOPT_PROGRESSFUNCTION, Job::ProgressBarCallback );
	curl_easy_setopt( pHandle, CURLOPT_PROGRESSDATA, this );
	curl_easy_setopt( pHandle, CURLOPT_NOPROGRESS, 0 );
	curl_easy_setopt( pHandle, CURLOPT_DEBUGFUNCTION, Job::DebugCallback );
	curl_easy_setopt( pHandle, CURLOPT_DEBUGDATA, this );
	curl_easy_setopt( pHandle, CURLOPT_ERRORBUFFER, m_pzErrorBuffer );
	

	/* Attach the curl easy handle to the curl multi handle */
	CURLMcode nResult = curl_multi_add_handle( psHandle->m_pMultiHandle, pHandle );
	if( nResult != CURLM_OK ) {
		DEBUG( "Job::AttachToHandle: curl_multi_add_handle() failed: %s (%i)\n", curl_multi_strerror( nResult ), nResult );
	}
	
	m_pcServer->JobUpdated( this );

	return( CURLE_OK );
}


/** \brief Remove any settings from the curl handle set in AttachToHandle().
 * \note This will be called when the transfer finishes, but it may also be called for instance
 *  if the user pauses the transfer and we need to use the curl handle for something else.
 */
int Job::RemoveFromHandle( CurlHandle_s* psHandle )
{
	DEBUG( "Job::RemoveFromHandle() job %i from curl handle %x\n", m_nID, psHandle->m_pHandle );
	CURL* pHandle = psHandle->m_pHandle;
	
	CURLMcode nResult = curl_multi_remove_handle( psHandle->m_pMultiHandle, pHandle );
	if( nResult != CURLM_OK ) {
		DEBUG( "Job::RemoveFromHandle: curl_multi_remove_handle() failed: %s (%i)\n", curl_multi_strerror( nResult ), nResult );
	}
	
	curl_easy_setopt( pHandle, CURLOPT_URL, "" );
	curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, NULL );
	curl_easy_setopt( pHandle, CURLOPT_READDATA, NULL );
	curl_easy_setopt( pHandle, CURLOPT_SEEKDATA, NULL );
	curl_easy_setopt( pHandle, CURLOPT_PROGRESSDATA, NULL );
	curl_easy_setopt( pHandle, CURLOPT_DEBUGDATA, NULL );
	curl_easy_setopt( pHandle, CURLOPT_ERRORBUFFER, NULL );
	
	m_psHandle = NULL;
	psHandle->m_nStatus = HANDLE_IDLE;

	m_pcServer->JobUpdated( this );
	
	return( CURLE_OK );
}

void Job::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	DEBUG( "Transfer for job %i finished; result code %li\n:", m_nID, nCode );

	/* It is up to subclasses to detect errors and notify the user, if necessary. */
	m_nResponseCode = nCode;
	if( nCode / 100 == 2 ) {
		/* Any other failure conditions should be caught by the subclasses and should set m_nStatus manually. */
		m_nStatus = STATUS_COMPLETED;
	} else {
		m_nStatus = STATUS_FAILED;
	}
	FinaliseJob();

	/* If this job was created recursively, then pass the call on to our parent. */
	if( m_pcParent != NULL )
	{
		m_pcParent->ChildTransferFinished( psHandle, nCode, this );
		return;
	}
}

void Job::Pause()
{
	if( m_psHandle ) RemoveFromHandle( m_psHandle );
	if( STATUS_IS_ACTIVE( m_nStatus ) ) {
		m_nStatus = STATUS_PAUSED;
		m_pcServer->JobUpdated( this );
	} else {
		DEBUG( "Warning: tried to pause job %i from unexpected status %i\n", m_nID, m_nStatus );
	}
}

void Job::Resume()
{
	if( STATUS_IS_SUSPENDED( m_nStatus ) ) {
		m_nStatus = STATUS_QUEUED;
		m_pcServer->JobUpdated( this );
	} else {
		DEBUG( "Warning: tried to resume job %i from unexpected status %i\n", m_nID, m_nStatus );
	}
}

void Job::Cancel()
{
	if( STATUS_IS_ACTIVE( m_nStatus ) || STATUS_IS_SUSPENDED( m_nStatus ) ) {
		m_nStatus = STATUS_CANCELLED;
		FinaliseJob();
		/* Recursive jobs should re-implement this method and cancel their child jobs */
	} else {
		DEBUG( "Warning: tried to cancel job %i from unexpected status %i\n", m_nID, m_nStatus );
	}
}

void Job::HandleOverwriteReply( Message* pcMsg )
{
}

void Job::HandleFailureReply( Message* pcMsg )
{
	int nResponse;
	if( pcMsg->FindInt32( "userResponse", &nResponse ) != 0 ) {
		DEBUG( "Job::HandleFailureReply: got message without user response parameter!\n" );
		return;
	}
	DEBUG( "Job %i got response %i from GUI\n", m_nID, nResponse );
}

void Job::SetOverwritePolicy( enum overwrite_policy ePolicy, bool bSetParent )
{
	m_eOverwritePolicy = ePolicy;
	if( m_pcParent && bSetParent ) m_pcParent->SetOverwritePolicy( ePolicy );
}

void Job::SetFailurePolicy( enum failure_policy ePolicy, bool bSetParent )
{
	m_eFailurePolicy = ePolicy;
	if( m_pcParent && bSetParent ) m_pcParent->SetFailurePolicy( ePolicy );
}

/** \brief Read Callback
 * The libcurl read callback.
 * Can't pass a virtual function to libcurl so we use these static wrappers.
 *
 * \param pBuf The location to store input data.
 * \param nSize The amount of data to be read.
 * \param nMult The size in bytes of each data unit.
 * \param pCookie This contains the Job* that we want to call the Read method on.
 *
 */
size_t Job::ReadCallback( void* pBuf, size_t nSize, size_t nMult, void* pCookie )
{
	Job* pcTransfer = (Job*)pCookie;
	if( pcTransfer == NULL ) { return 0; }
	return( pcTransfer->Read( pBuf, nSize * nMult ) );
}


/** \brief Write Callback
 * The libcurl write callback.
 * Can't pass a virtual function to libcurl so we use these static wrappers.
 *
 * \todo Verify descriptions of callback parameters with libcurl documentation.
 *
 * \param pBuf The data to be stored.
 * \param nSize The size of the data being passed to the Job.
 * \param nMult The size of the data type being passed.
 * \param pCookie This contains the Job* that we want to call the Write method on.
 *
 */
size_t Job::WriteCallback( void* pBuf, size_t nSize, size_t nMult, void* pCookie )
{
	Job* pcTransfer = (Job*)pCookie;
	if( pcTransfer == NULL ) { return 0; }
	return( pcTransfer->Write( pBuf, nSize * nMult ) );
}

/** \brief Seek Callback
 * The libcurl seek callback.
 * Can't pass a virtual function to libcurl so we use these static wrappers.
 *
 * \param pCookie This contains the Job* that we want to call the Seek method on.
 * \param nOffset How many bytes to offset into the file.
 * \param nOrigin The origin from which to start the offset.
 */
int Job::SeekCallback( void* pCookie, curl_off_t nOffset, int nOrigin )
{
	Job* pcTransfer = (Job*)pCookie;
	if( pcTransfer == NULL ) { return 0; }
	return( pcTransfer->Seek( nOffset, nOrigin ) );
}

/** \brief Progress Bar Callback
 * The libcurl progress bar. callback.
 * Can't pass a virtual function to libcurl so we use these static wrappers.
 *
 * \param pCookie The pointer to the Job object to send the ProgreesBar update to.
 * \param fDownTotal Total data downloaded to be downloaded
 * \param fDownNow Total data downloaded so far.
 * \param fUpTotal Total data to be uploaded.
 * \param fUpNow Total data uploaded so far.
 */
int Job::ProgressBarCallback( void* pCookie, double fDownTotal, double fDownNow, double fUpTotal, double fUpNow )
{
	Job* pcTransfer = (Job*)pCookie;
	if( pcTransfer == NULL ) { return 0; }
	return( pcTransfer->ProgressBar( fDownTotal, fDownNow, fUpTotal, fUpNow ) );
}

int Job::DebugCallback( CURL* pHandle, curl_infotype eType, char* pzData, size_t nSize, void* pCookie )
{
	Job* pcJob = (Job*)pCookie;
	if( g_bDebug && eType != CURLINFO_DATA_IN && eType != CURLINFO_DATA_OUT )
	{
		String zMsg;
		if( eType == CURLINFO_TEXT ) zMsg = "* ";
		else if( eType == CURLINFO_HEADER_IN ) zMsg = "< ";
		else if( eType == CURLINFO_HEADER_OUT ) zMsg = "> ";
		
		if( nSize <= 4 || strncmp( pzData, "PASS ", 5 ) != 0 ) { 
			zMsg += String( pzData, nSize );
		} else {
			zMsg += "PASS ****\n";
		}
		DEBUG( zMsg.c_str() );
	}
	if( pcJob != NULL ) return( pcJob->DebugHandler( pHandle, eType, pzData, nSize ) );
	else return( 0 );
}

int Job::DebugHandler( CURL* pHandle, curl_infotype eType, char* pzData, size_t nSize )
{
	if( eType == CURLINFO_HEADER_IN ) {
		m_zFTPResponse = String( pzData, nSize );
		/* Strip trailing '\n' or '\r\n' at end of message */
		while( m_zFTPResponse.Length() > 0 &&
			   (m_zFTPResponse[m_zFTPResponse.Length()-1] == '\n' ||
			    m_zFTPResponse[m_zFTPResponse.Length()-1] == '\r') )
		{
			m_zFTPResponse = m_zFTPResponse.erase( m_zFTPResponse.Length()-1 );
		}
	}
	return( 0 );
}

/***** DownloadJob implementation ******/

/** \brief DownloadJob Constructor. 
 * Sets the initial download file location to null.
 */
DownloadJob::DownloadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent )
	: Job( pcServer, JOB_DOWNLOAD, nID, pcParent )
{
	m_bStarted = false;
	m_pcFile = NULL;	/* This is only created when AttachToHandle() is first called. */
	
	m_zLocalPath = zLocalPath;
	m_zRemotePath = zRemotePath;
	
	m_nCurrentDownloaded = 0;
	m_nTotalSize = -1;

	/* Set the url */
	m_zURL = m_pcServer->GetServerAddress();
//	if( m_zRemotePath.size() > 0 && m_zRemotePath[0] != '/' ) m_zURL += "/";
	m_zURL += "/";
	m_zURL += m_zRemotePath;
	DEBUG( "DownloadJob %i created; parent %i; URL %s\n", nID, pcParent==NULL?-1:pcParent->GetID(), m_zURL.c_str() );	
}

/** \brief DownloadJob destructor.
 * This frees up the file pointer m_pcFile if necessary.
 */
DownloadJob::~DownloadJob()
{
	if( m_pcFile )
		delete( m_pcFile );
}

/** \brief Setup Function for Curl Transfer.
 * Configures the given CURL handle to perform this transfer, such as setting the url.
 */
int DownloadJob::AttachToHandle( CurlHandle_s* psHandle )
{
	if( !m_bStarted ) {
		m_bStarted = true;

		if( m_eOverwritePolicy != OP_OVERWRITE ) {
			/* Check if file exists */
			struct stat sStat;
			int nResult = stat( m_zLocalPath.c_str(), &sStat );

			if( nResult == 0 ) {
				if( m_eOverwritePolicy == OP_ASK ) {
					/* File exists. Display overwrite notification to user and suspend job. */
					m_nStatus = STATUS_PENDING_USER_RESPONSE;
					m_pcServer->SendOverwriteNotification( this, m_pcParent != NULL );
					m_pcServer->JobUpdated( this );
					return( -1 );
				} else if( m_eOverwritePolicy == OP_SKIP ) {
					DEBUG( "DownloadJob: destination %s exists, skipping\n", m_zLocalPath.c_str() );
					m_nStatus = STATUS_SKIPPED;
					FinaliseJob();
					if( m_pcParent ) m_pcParent->ChildTransferFinished( psHandle, -1, this );
					return( -1 );
				}
			}
		}
	}
	
	if( !m_pcFile ) {
		try {
			m_pcFile = new File( m_zLocalPath, O_WRONLY | O_CREAT | O_TRUNC );
		} catch( errno_exception& e ) {
			String zMsg;
			zMsg.Format( "Could not open the file %s: %s\n", m_zLocalPath.c_str(), e.what() );
			m_pcServer->SendFailureNotification( this, zMsg );

			/* Delete the job */
			m_nStatus = STATUS_FAILED;
			FinaliseJob();
			
			/* If this job was created recursively, then notify our parent. */
			if( m_pcParent != NULL )
			{
				m_pcParent->ChildTransferFinished( psHandle, e.error(), this );
			}

			return( -1 );
		}
	}
	
	curl_off_t nOffset = m_pcFile->Seek( 0, SEEK_CUR );	/* Get current position in file, for if we are resuming */
	
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_URL, m_zURL.c_str() );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_UPLOAD, 0 );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_RESUME_FROM_LARGE, nOffset );
	
	return( Job::AttachToHandle( psHandle ) );
}

int DownloadJob::RemoveFromHandle( CurlHandle_s* psHandle )
{
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0 );
	return( Job::RemoveFromHandle( psHandle ) );
}

void DownloadJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	if( m_pcFile ) m_pcFile->Flush();

	if( nCode / 100 != 2 ) {
		/* 2xx response code indicates success; anything else is an error. */
		String zMsg;
		/* Use FTP response, if present; use libcurl error message if not. */
		/* nCode == 0 typically indicates a connection problem, eg bad address */
		zMsg.Format( "Download of %s failed! %s", m_zRemotePath.c_str(), (nCode != 0 && m_zFTPResponse != "" ? m_zFTPResponse.c_str() : m_pzErrorBuffer) );
		m_pcServer->SendFailureNotification( this, zMsg );
	}

	/* Should always be the last thing before returning */
	Job::TransferFinished( psHandle, nCode );
}

void DownloadJob::HandleOverwriteReply( Message* pcMsg )
{
	int nResponse;
	bool bPersistent = false;
	pcMsg->FindInt32( "userResponse", &nResponse );
	pcMsg->FindBool( "persistent", &bPersistent );
	if( nResponse == RESPONSE_YES ) {
		if( bPersistent ) SetOverwritePolicy( OP_OVERWRITE );
		Resume();
	} else if( nResponse == RESPONSE_NO ) {
		if( bPersistent ) SetOverwritePolicy( OP_SKIP );
		m_nStatus = STATUS_SKIPPED;
		FinaliseJob();
		if( m_pcParent ) m_pcParent->ChildTransferFinished( NULL, -1, this );
	}
	else {
		DEBUG( "DownloadJob::HandleOverwriteReply: Got unexpected response %i!\n", nResponse );
	}
}

void DownloadJob::HandleFailureReply( Message* pcMsg )
{
	DEBUG( "DownloadJob::HandleFailureReply()\n" );
}

bool DownloadJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "localPath", m_zLocalPath );
	pcMsg->AddString( "remotePath", m_zRemotePath );
	pcMsg->AddInt32( "totalTransferred", m_nCurrentDownloaded );
	pcMsg->AddInt32( "totalSize", m_nTotalSize );
	pcMsg->AddString( "description", "Download" );

	return( Job::GetJobInfo( pcMsg ) );
}

void DownloadJob::Pause()
{
	if( m_pcFile ) m_pcFile->Flush();
	Job::Pause();
}

void DownloadJob::Cancel()
{
	if( m_pcFile ) m_pcFile->Flush();
	Job::Cancel();
}

/** \brief Write to file.
 * This performs the actual writing of data to the file.
 *
 * \param pBuf The data to write to the file.
 * \param nSize The size of the data being written.
 *
 * \return The amount of data actually written.
 */
size_t DownloadJob::Write( void* pBuf, size_t nSize )
{
	size_t nResult = m_pcFile->Write( pBuf, nSize );
//	m_pcFile->Flush();
	return( nResult );
}

/** \brief Performs a seek operation on the file.
 * Seek into a particular position in the file so curl can begin operating from there.
 *
 * \param nOffset The offset into the file to start operating at.
 * \param nOrigin The origin of the offset.
 *
 * \todo Make sure that File::Seek() returns the same as libcurl expects.
 */
int DownloadJob::Seek( curl_off_t nOffset, int nOrigin )
{
	return( m_pcFile->Seek( nOffset, nOrigin ) );

}

/** \brief Updates the progress bar.
 * This just outputs progress information to the terminal.
 *
 * \param fDownTotal Total bytes to be downloaded.
 * \param fDownNow Total bytes downloaded so far.
 * \param fUpTotal Total bytes to upload.
 * \param fUpNow Total bytes uploaded at the moment.
 */
int DownloadJob::ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow )
{
	m_nCurrentDownloaded = (ssize_t)fDownNow;
	m_nTotalSize = (ssize_t)fDownTotal;
	m_pcServer->JobUpdated( this );
//	DEBUG( "Progress for download %s: %.1f%%\n", m_zRemotePath.c_str(), fDownNow/fDownTotal * 100 );
	return( 0 );
}

/****** UploadJob implementation ********/

/** \brief UploadJob Constructor. 
 * Sets the initial upload file location to null.
 */
UploadJob::UploadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent )
	: Job( pcServer, JOB_UPLOAD, nID, pcParent )
{
	m_bStarted = false;
	m_bChildFinished = false;
	m_pcFile = NULL;	/* This is only created when AttachToHandle() is first called. */
	m_pcChild = NULL;
	
	m_zLocalPath = zLocalPath;
	m_zRemotePath = zRemotePath;
	
	m_nTotalSize = -1;
	m_nCurrentUploaded = 0;

	/* Set the url */
	m_zURL = m_pcServer->GetServerAddress();
//	if( m_zRemotePath.size() > 0 && m_zRemotePath[0] != '/' ) zURL += "/";
	m_zURL += "/";
	m_zURL += m_zRemotePath;
	DEBUG( "UploadJob %i created; parent %i; URL %s\n", nID, pcParent==NULL?-1:pcParent->GetID(), m_zURL.c_str() );	
}

/** \brief UploadJob destructor.
 * Frees the pointer to the upload file.
 */
UploadJob::~UploadJob()
{
	if( m_pcFile )
		delete( m_pcFile );
}

/** \brief Setup Handle for Curl Transfer.
 * This does the setup for an upload curl handler.
 */
int UploadJob::AttachToHandle( CurlHandle_s* psHandle )
{
	if( !m_bChildFinished )
	{
		if( !m_bStarted )
		{
			m_bStarted = true;
			m_pcFile = new File( m_zLocalPath, O_RDONLY );
		
			/* Create initial CommandJob to check if destination exists */
			std::vector<String> azCommands;
			String zCommand = "SIZE ";
			zCommand += m_zRemotePath;
			azCommands.push_back( zCommand );
			m_pcChild = new CommandJob( m_pcServer, azCommands, false, m_pcServer->GetNewJobID(), this );
			m_pcServer->AddJob( m_pcChild );
			/* The job will get started by _Schedule() */
		}
		m_nStatus = STATUS_PENDING_CHILD_JOB;
		m_pcServer->JobUpdated( this );
		return( -1 );
	}
	else
	{
		curl_off_t nOffset = m_pcFile->Seek( 0, SEEK_CUR );	/* Get current position in file, for if we are resuming */
		curl_easy_setopt( psHandle->m_pHandle, CURLOPT_RESUME_FROM_LARGE, nOffset );

		curl_off_t nSize = m_pcFile->GetSize();
		curl_easy_setopt( psHandle->m_pHandle, CURLOPT_INFILESIZE_LARGE, nSize );
	
		curl_easy_setopt( psHandle->m_pHandle, CURLOPT_URL, m_zURL.c_str() );
		curl_easy_setopt( psHandle->m_pHandle, CURLOPT_UPLOAD, 1 );
	
		return( Job::AttachToHandle( psHandle ) );
	}
}

void UploadJob::ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcChild )
{
	m_bChildFinished = true;
	m_pcChild = NULL;
	
	if( nCode / 100 == 2 ) {
		/* Response code 2xx indicates success, therefore file exists */
		if( m_eOverwritePolicy == OP_ASK ) {
			m_nStatus = STATUS_PENDING_USER_RESPONSE;
			m_pcServer->SendOverwriteNotification( this, m_pcParent != NULL );
			m_pcServer->JobUpdated( this );
		} else if( m_eOverwritePolicy == OP_SKIP ) {
			m_nStatus = STATUS_SKIPPED;
			FinaliseJob();
			if( m_pcParent ) m_pcParent->ChildTransferFinished( psHandle, -1, this );
		} else if( m_eOverwritePolicy == OP_OVERWRITE ) {
			m_nStatus = STATUS_QUEUED;
		}
	} else {
		/* File does not exist; proceed with upload */
		m_nStatus = STATUS_QUEUED;
	}
}

void UploadJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	if( nCode / 100 != 2 ) {
		/* 2xx response code indicates success; anything else is an error. */
		String zMsg;
		/* Use FTP response, if present; use libcurl error message if not. */
		/* nCode == 0 typically indicates a connection problem, eg bad address */
		zMsg.Format( "Upload of %s failed! %s", m_zRemotePath.c_str(), (nCode != 0 && m_zFTPResponse != "" ? m_zFTPResponse.c_str() : m_pzErrorBuffer) );
		m_pcServer->SendFailureNotification( this, zMsg );
	}
	
	Job::TransferFinished( psHandle, nCode );
}

int UploadJob::RemoveFromHandle( CurlHandle_s* psHandle )
{
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0 );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_INFILESIZE_LARGE, 0 );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_UPLOAD, 0 );
	
	return( Job::RemoveFromHandle( psHandle ) );
}

void UploadJob::HandleOverwriteReply( Message* pcMsg )
{
	int nResponse;
	bool bPersistent = false;
	pcMsg->FindInt32( "userResponse", &nResponse );
	pcMsg->FindBool( "persistent", &bPersistent );
	if( nResponse == RESPONSE_YES ) {
		if( bPersistent ) SetOverwritePolicy( OP_OVERWRITE );
		Resume();
	} else if( nResponse == RESPONSE_NO ) {
		if( bPersistent ) SetOverwritePolicy( OP_SKIP );
		m_nStatus = STATUS_SKIPPED;
		FinaliseJob();
		if( m_pcParent ) m_pcParent->ChildTransferFinished( NULL, -1, this );
	}
	else DEBUG("UploadJob::HandleOverwriteReply: Got unexpected response %i!\n", nResponse );
}

void UploadJob::HandleFailureReply( Message* pcMsg )
{
	DEBUG( "UploadJob::HandleFailureReply()\n" );
}

/** \brief Read from file.
 * This performs the read from the file and returns the size read.
 *
 * \param pBuf A pointer to a location that the data can be read into.
 * \param nSize The amount of data to be read.
 */
size_t UploadJob::Read( void* pBuf, size_t nSize )
{
	return( m_pcFile->Read( pBuf, nSize ) );
}

/** \brief Seek into the upload file.
 * Seek into a particular position in the file so curl can begin operating from there.
 *
 * \param nOffset The offset into the file to start operating at.
 * \param nOrigin The origin of the offset.
 *
 * \todo Make sure that File::Seek() returns the same as libcurl expects.
 */
int UploadJob::Seek( curl_off_t nOffset, int nOrigin )
{
	return( m_pcFile->Seek( nOffset, nOrigin ) );
}

/** \brief Progress Bar Update.
 * This just outputs progress information to the terminal.
 *
 * \param fDownTotal Total bytes to be downloaded.
 * \param fDownNow Total bytes downloaded so far.
 * \param fUpTotal Total bytes to upload.
 * \param fUpNow Total bytes uploaded at the moment.
 */
int UploadJob::ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow )
{
	m_nCurrentUploaded = (ssize_t)fUpNow;
	m_nTotalSize = (ssize_t)fUpTotal;
	m_pcServer->JobUpdated( this );
//	DEBUG( "Progress for upload %s: %.1f%%\n", m_zRemotePath.c_str(), (float)(100 * fUpNow/fUpTotal) );
	return( 0 );
}

bool UploadJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "localPath", m_zLocalPath );
	pcMsg->AddString( "remotePath", m_zRemotePath );
	pcMsg->AddInt32( "totalTransferred", m_nCurrentUploaded );
	pcMsg->AddInt32( "totalSize", m_nTotalSize );
	pcMsg->AddString( "description", "Upload" );

	return( Job::GetJobInfo( pcMsg ) );
}


/****** DirListJob implementation *******/

/** \brief Default constructor for DirListJob.
 * This initializes the target of the directory listing and makes note that
 * this is starting the listing.
 */
DirListJob::DirListJob( Server* pcServer, const String& zRemotePath, int nID, Job* pcParent )
	: Job( pcServer, JOB_DIRLIST, nID, pcParent )
{
	m_zRemotePath = zRemotePath;
	m_bInitial = true;

	m_zURL = pcServer->GetServerAddress();
//	if( m_zRemotePath.size() > 0 && m_zRemotePath[0] != '/' ) zURL += "/";
	m_zURL += "/";
	m_zURL += m_zRemotePath;
	if( m_zRemotePath.size() > 0 && m_zRemotePath[m_zRemotePath.size()-1] != '/' )  m_zURL += "/";	/* Make it clear to libcurl that we want a dir listing */
	DEBUG( "DirListJob %i created; parent %i; URL %s\n", m_nID, (m_pcParent?m_pcParent->GetID():-1), m_zURL.c_str() );

}

/** \brief DirListJob destructor.
 */
DirListJob::~DirListJob()
{
}

/** \brief Setup the Curl handle for the directory listing.
 */
int DirListJob::AttachToHandle( CurlHandle_s* psHandle )
{
	String zServer = m_pcServer->GetServerAddress(); 	/* Address of the form "ftp://user:pwd@host:port" (no path) */
	
	/* Set the url, set to upload */
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_URL, m_zURL.c_str() );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_UPLOAD, 0 );
//	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0 );
	
	return( Job::AttachToHandle( psHandle ) );
}

/** \brief Find the first occurrence of nChar in the first nLength bytes of pzStr. 
 * If not found, points to the first char *after* the end of the string.
 */
const char* strnchr( const char* pzStr, size_t nLength, int nChar )
{
	size_t nPos = 0;
	while( nPos < nLength && pzStr[nPos] != nChar && pzStr[nPos] != 0 ) ++nPos;
	return( pzStr + nPos );
}

/** \brief Parse (part of) a dirlisting into a list of filesystem nodes.
 * This functionality is used by RecursiveDownloadJob also.
 */
size_t DirListJob::ParseDirListData( const char* pzBuf, size_t nSize, std::vector< RemoteNode >* pacList )
{
	struct ftpparse sData;
	memset( &sData, 0, sizeof( sData ) );
	
	size_t nTotalRead = 0; // Total bytes read.
	String zLine = m_zLastLineFragment; // Copy the last fragment read if we didn't read a complete line at the end last time.
	size_t nLineLength = m_zLastLineFragment.size(); // Initialize the current line length.
	const char* pzLine = pzBuf;
	
	// Read in the next line up to and excluding the '\n'
	nTotalRead = strnchr( pzLine, nSize, '\n' ) - pzLine;
	nLineLength += nTotalRead;
	
	// Append the new text to the end of the line.
	zLine += String( pzLine, nTotalRead );
	
	/* Didn't find a \n; the line is incomplete so save it for later */
	if( nTotalRead == nSize )
	{
		m_zLastLineFragment = zLine;
		return( nSize );
	}
	
	do
	{
//		DEBUG( "pzBuf: %x  pzLine: %x  length: %i     pzLine - pzBuf: %i\n", pzBuf, pzLine, nLineLength, pzLine - pzBuf );
//		DEBUG( "Line: '%s' length: %i\n", zLine.c_str(), nLineLength );

		// Attempt to parse the current line.
		int nResult;
		nResult = ftpparse( &sData, zLine.c_str(), nLineLength );
//		DEBUG( "Parse result: %i\n", nResult );

		if( nResult == 1 )	/* 1 indicates success */
		{
			// Create a new RemoteNode for the directory entry.
			RemoteNode cNode;
			cNode.m_zName = String( sData.name, sData.namelen );
			cNode.m_bIsDir = sData.flagtrycwd;
			cNode.m_nSize = sData.size;
			cNode.m_cTimestamp = DateTime( sData.mtime );
			cNode.m_zPath.Format( "%s/%s", m_zRemotePath.c_str(), cNode.m_zName.c_str() );
			/* ftpparse doesn't provide permissions! Guess we'll do without for now. */
			cNode.m_nPermissions = 0777;
//			DEBUG( "Parsed: name '%s', path '%s', size %i, %s, %s\n", cNode.m_zName.c_str(), cNode.m_zPath.c_str(), cNode.m_nSize, (cNode.m_bIsDir?"dir":"file"), cNode.m_cTimestamp.GetDate().c_str() );
			pacList->push_back( cNode );
		}
		else
		{
			String zTmp = String( pzLine, nLineLength );
			DEBUG( "ParseDirListing: ftpparse() failed on '%s' (%i bytes)\n", zTmp.c_str(), nLineLength );
		}

		nTotalRead++;	/* Skip the \n */
		pzLine = pzBuf + nTotalRead;
		nLineLength = strnchr( pzLine, nSize - nTotalRead, '\n' ) - pzLine;
		nTotalRead += nLineLength;
		zLine = String( pzLine, nLineLength );
		
		
		/* Save anything after the last \n (this will be empty if the pzBuf ends with \n) */
		if( nTotalRead >= nSize )
		{
			m_zLastLineFragment = zLine;
			break;
		}
	} while( 1 );
	
//	DEBUG( " nTotalRead: %i   nSize: %i   zLastLineFragment: '%s'\n", nTotalRead, nSize, m_zLastLineFragment.c_str() );	
	return( nSize );
}

/** \brief Write the data into the data buffer.
 * This "writes" the data coming from the remote server into the RemoteNode
 * objects. These are then passed to RemoteIconView to display to the user.
 *
 * \warning The RemoteNode vector that is sent is assumed to be deleted by the
 * RemoteIconView that calls for the update when it is done processing.
 *
 * \bug Generated nodes do not have proper permissions due to a deficiency in
 * how ftpparse operates.
 *
 * \todo The dirlist parsing might be better in Server, but we need to do it here because we need to save any fragments of the last line.
 *
 * \todo Fix up initial, final flags that are sent to the RemoteIconView. Maybe remove them. Currently the 'final' flag isn't being sent.
 */
size_t DirListJob::Write( void* pBuf, size_t nSize )
{
	/* If ftp entry path hasn't been determined yet, do it now. */
	/* We do this here, since we want to provide the entry path to the GUI at the earliest opportunity */
	if( m_pcServer->m_zEntryPath == "" )
	{
		const char* pzEntryPath = NULL;
		curl_easy_getinfo( m_psHandle->m_pHandle, CURLINFO_FTP_ENTRY_PATH, &pzEntryPath );
		if( pzEntryPath == NULL ) {
			DEBUG( "DirListJob: FTP_ENTRY_PATH returned NULL!\n" );
		} else {
			m_pcServer->_SetEntryPath( pzEntryPath );
			DEBUG( "DirListJob: Got entry path %s\n", pzEntryPath );
		}
	}

	char* pzBuf = (char*)pBuf;
	String zTmp( pzBuf, nSize );
//	DEBUG( "DirListJob::Write( %s, %i bytes, %sinitial ):\n%s\n------ end listing data ------\n", m_zRemotePath.c_str(), (int)nSize, m_bInitial?"":"not ", zTmp.c_str() );

	std::vector< RemoteNode >* pacList = new std::vector< RemoteNode >;	/* Will be deleted by the RemoteIconView when it's finished processing */
	size_t nResult = ParseDirListData( pzBuf, nSize, pacList );

	if( pacList->size() == 0 )
	{
		/* We didn't sucessfully parse any entries! Clean up. */
		delete( pacList );
		return( nResult );
	}
	
	HandleDirListData( pacList );
	
	m_bInitial = false;
	
	return( nResult );
}

/** \brief Update the Progress bar for the job.
 * This just outputs progress information to the terminal.
 *
 * \param fDownTotal Total bytes to be downloaded.
 * \param fDownNow Total bytes downloaded so far.
 * \param fUpTotal Total bytes to upload.
 * \param fUpNow Total bytes uploaded at the moment.
 */
int DirListJob::ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow )
{
//	DEBUG( "Progress for dirlisting %s: %.1f out of %.1f\n", m_zRemotePath.c_str(), (float)fDownNow, (float)fDownTotal );
	return( 0 );
}

void DirListJob::HandleDirListData( std::vector< RemoteNode >* pacList )
{
	/* Only the derived versions of this method should be used */
	DEBUG( "Warning: DirListJob::HandleDirListData() called!\n" );
}

bool DirListJob::GetJobInfo( Message* pcMsg )
{
	Job::GetJobInfo( pcMsg );
	return( false );
}

/*************** GUIDirListJob implementation ************************/

GUIDirListJob::GUIDirListJob( Server* pcServer, const String& zRemotePath, Handler* pcTarget, int nID, Job* pcParent )
	: DirListJob( pcServer, zRemotePath, nID, pcParent )
{
	m_pcTarget = pcTarget;
}

GUIDirListJob::~GUIDirListJob()
{
}

void GUIDirListJob::HandleDirListData( std::vector< RemoteNode >* pacList )
{

	Message cMsg( M_REMOTE_DIRLISTING );
	cMsg.AddString( "path", m_zRemotePath );
	cMsg.AddPointer( "list", pacList );

	if( m_bInitial )
		cMsg.AddBool( "initial", true );	/* TODO: fix up initial, final flags. Maybe remove them. */
//	cMsg.AddBool( "final", true );
	
	Messenger cMessenger( m_pcTarget );
	cMessenger.SendMessage( &cMsg );
}

void GUIDirListJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	if( nCode / 100 != 2 ) {
		/* 2xx response code indicates success; anything else is an error. */
		String zMsg;
		zMsg.Format( "Error getting directory listing%s%s! %s", (m_zRemotePath != "" ? " for " : ""), m_zRemotePath.c_str(), (nCode != 0 && m_zFTPResponse != "" ? m_zFTPResponse.c_str() : m_pzErrorBuffer) );
		m_pcServer->SendFailureNotification( this, zMsg );
	}

	DirListJob::TransferFinished( psHandle, nCode );
}

/**** CommandJob implementation *******/

CommandJob::CommandJob( Server* pcServer, const std::vector<String>& azCommands, bool bUserVisible, int nID, Job* pcParent )
 : Job( pcServer, JOB_COMMAND, nID, pcParent )
{
	m_psCommands = NULL;
	
	m_bUserVisible = bUserVisible;
	
	Init( azCommands );
}

CommandJob::CommandJob( Server* pcServer, bool bUserVisible, int nID, Job* pcParent )
 : Job( pcServer, JOB_COMMAND, nID, pcParent )
{
	m_bUserVisible = bUserVisible;
	m_psCommands = NULL;
}

CommandJob::~CommandJob()
{
}

void CommandJob::Init( const std::vector<String>& azCommands )
{
	DEBUG( "CommandJob %i created; parent %i; doing %s; %suser visible\n", m_nID, (m_pcParent?m_pcParent->GetID():-1), azCommands.front().c_str(), (m_bUserVisible?"":"not ") );
	m_azCommands = azCommands;
}

int CommandJob::AttachToHandle( CurlHandle_s* psHandle )
{
	String zURL = m_pcServer->GetServerAddress();
	zURL += "/";	/* Dummy path */
	
	/* Attach the commands to the handle */
	std::vector<String>::iterator i = m_azCommands.begin();
	for( ; i != m_azCommands.end(); i++ ) {
		m_psCommands = curl_slist_append( m_psCommands, i->c_str() );
	}

	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_QUOTE, m_psCommands );
	
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_URL, zURL.c_str() );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_NOBODY, 1 );

	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_NOPROGRESS, 1 );	/* Don't want progress notifications */

	return( Job::AttachToHandle( psHandle ) );
}

int CommandJob::RemoveFromHandle( CurlHandle_s* psHandle )
{
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_NOBODY, 0 );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_QUOTE, NULL );
	curl_easy_setopt( psHandle->m_pHandle, CURLOPT_NOPROGRESS, 0 );
	
	curl_slist_free_all( m_psCommands );
	
	return( Job::RemoveFromHandle( psHandle ) );
}

bool CommandJob::GetJobInfo( Message* pcMsg )
{
	bool bResult =  Job::GetJobInfo( pcMsg );
	if( !m_bUserVisible ) bResult = false;
	
	return( bResult );
}

/**** DeleteJob implementation ****/

DeleteJob::DeleteJob( Server* pcServer, const String& zPath, int nID, Job* pcParent )
 : CommandJob( pcServer, true, nID, pcParent )
{
	m_zRemotePath = zPath;
	std::vector<String> acCommands;
	String zTmp = "DELE ";
	zTmp += zPath;
	acCommands.push_back( zTmp );

	CommandJob::Init( acCommands );
}

DeleteJob::~DeleteJob()
{
}

void DeleteJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	if( nCode / 100 != 2 ) {
		/* 2xx response code indicates success; anything else is an error. */
		String zMsg;
		zMsg.Format( "Deletion of %s failed! %s", m_zRemotePath.c_str(), (nCode != 0 && m_zFTPResponse != "" ? m_zFTPResponse.c_str() : m_pzErrorBuffer) );
		m_pcServer->SendFailureNotification( this, zMsg );
	}

	CommandJob::TransferFinished( psHandle, nCode );
}

bool DeleteJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "description", "Delete" );
	pcMsg->AddString( "remotePath", m_zRemotePath );
	return( CommandJob::GetJobInfo( pcMsg ) );
}

/**** MkDirJob implementation ****/

MkDirJob::MkDirJob( Server* pcServer, const String& zPath, int nID, Job* pcParent )
 : CommandJob( pcServer, true, nID, pcParent )
{
	m_zRemotePath = zPath;
	String zCommand = "MKD ";
	zCommand += zPath;
	std::vector<String> cTmp;
	cTmp.push_back( zCommand );

	CommandJob::Init( cTmp );
}

MkDirJob::~MkDirJob()
{
}

void MkDirJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	if( nCode / 100 != 2 ) {
		/* 2xx response code indicates success; anything else is an error. */
		String zMsg;
		zMsg.Format( "Creation of directory %s failed! %s", m_zRemotePath.c_str(), (nCode != 0 && m_zFTPResponse != "" ? m_zFTPResponse.c_str() : m_pzErrorBuffer) );
		m_pcServer->SendFailureNotification( this, zMsg );
	}

	CommandJob::TransferFinished( psHandle, nCode );
}

bool MkDirJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "description", "Create directory" );
	pcMsg->AddString( "remotePath", m_zRemotePath );
	return( CommandJob::GetJobInfo( pcMsg ) );
}


/**** RenameJob implementation ****/

RenameJob::RenameJob( Server* pcServer, const String& zRemotePath, const String& zNewName, int nID, Job* pcParent )
 : CommandJob( pcServer, true, nID, pcParent )
{
	m_zRemotePath = zRemotePath;
	m_zNewName = zNewName;

	std::vector<String> cTmp;
	String zCommand = "RNFR ";
	zCommand += zRemotePath;
	cTmp.push_back( zCommand );
	zCommand = "RNTO ";
	zCommand += zNewName;
	cTmp.push_back( zCommand );

	CommandJob::Init( cTmp );
}

RenameJob::~RenameJob()
{
}

void RenameJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	if( nCode / 100 != 2 ) {
		/* 2xx response code indicates success; anything else is an error. */
		String zMsg;
		zMsg.Format( "Renaming of %s failed! %s", m_zRemotePath.c_str(), (nCode != 0 && m_zFTPResponse != "" ? m_zFTPResponse.c_str() : m_pzErrorBuffer) );
		m_pcServer->SendFailureNotification( this, zMsg );
	}

	CommandJob::TransferFinished( psHandle, nCode );
}

bool RenameJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "description", "Rename" );
	pcMsg->AddString( "remotePath", m_zRemotePath );
	pcMsg->AddString( "localPath", m_zNewName );
	return( CommandJob::GetJobInfo( pcMsg ) );
}


/**** RecursiveDownloadJob implementation ****/

RecursiveDownloadJob::RecursiveDownloadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent )
	: DirListJob( pcServer, zRemotePath, nID, pcParent )
{
	DEBUG( "RecursiveDownloadJob %i created; parent %i; path %s\n", m_nID, (m_pcParent?m_pcParent->GetID():-1), zRemotePath.c_str() );
	m_nType = JOB_DOWNLOAD;
	m_zLocalPath = zLocalPath;
	m_zRemotePath = zRemotePath;
	
	m_bStarted = false;
	
	m_pIterator = m_acList.begin();
}

RecursiveDownloadJob::~RecursiveDownloadJob()
{
}

int RecursiveDownloadJob::AttachToHandle( CurlHandle_s* psHandle )
{
	if( !m_bStarted ) {
		m_bStarted = true;
		/* Create the destination dir, if it doesn't already exist */
		int nResult = mkdir( m_zLocalPath.c_str(), S_IRWXU );
		if( nResult != 0 && errno != EEXIST) {
			DEBUG( "RecursiveDownloadJob: mkdir( %s ) failed! %i %s\n", m_zLocalPath.c_str(), errno, strerror(errno) );
			String zMsg;
			zMsg.Format("Could not create directory %s: %s", m_zLocalPath.c_str(), strerror(errno) );
			m_nStatus = STATUS_FAILED;
			m_pcServer->SendFailureNotification( this, zMsg );
			FinaliseJob();
			return( -1 );
		}
		if( nResult == 0 && errno == EEXIST ) {
			/* TODO: check if destination exists but isn't a directory */
			DEBUG( "RecursiveDownloadJob: Local dir %s already exists.\n", m_zLocalPath.c_str() );
		}
		else DEBUG( "RecursiveDownloadJob: Created local dir %s\n", m_zLocalPath.c_str() );
	}
	return( DirListJob::AttachToHandle( psHandle ) );
}

void RecursiveDownloadJob::HandleDirListData( std::vector< RemoteNode >* pacList )
{
	m_acList.insert( m_acList.end(), pacList->begin(), pacList->end() );
	m_pIterator = m_acList.begin();
	delete( pacList );
}

void RecursiveDownloadJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	RemoveFromHandle( psHandle );
	m_nStatus = STATUS_PENDING_CHILD_JOB;
	m_pcServer->JobUpdated( this );
	
	ChildTransferFinished( psHandle, 0, NULL );	/* Call this to create the first child job */
}

void RecursiveDownloadJob::ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcJob )
{
	if( pcJob != NULL ) {
		if( m_pcChildJob != pcJob ) {
			DEBUG( "RecursiveDownloadJob: WARNING! Got TransferFinished() for job %i which isn't current child of %i!\n", pcJob->GetID(), m_nID );
			return;
		}
		/* The child job should have cleaned itself up by now; remove it from our list. */
		m_pcChildJob = NULL;
	}
	
	/* Iterate through the entry list and create & attach a Job for each entry. */
	/* m_pIterator should already be valid; it is initially set to m_acList.start() in HandleDirListData(). */
	if( m_pIterator == m_acList.end() )
	{
		/* Finished; clean up this job. */
		DEBUG( "All children for job %i (RecursiveDownloadJob) finished.\n:", m_nID );

		m_nStatus = STATUS_COMPLETED;
		FinaliseJob();
		
		/* TODO: keep track of result codes for all transfers and pass an appropriate value */
		if( m_pcParent != NULL ) {
			m_pcParent->ChildTransferFinished( psHandle, nCode, this );
		}
		return;
	}
	Job* pcChild;
	String zDest = m_zLocalPath + "/" + m_pIterator->m_zName;
	int nNewID = m_pcServer->GetNewJobID();
	if( m_pIterator->IsDir() )
	{
		DEBUG( "RecursiveDownloadJob: Recursively getting dir '%s'  id %i\n", m_pIterator->m_zPath.c_str(), nNewID );
		pcChild = new RecursiveDownloadJob( m_pcServer, zDest, m_pIterator->m_zPath, nNewID, this );
	} else {
		DEBUG( "RecursiveDownloadJob: Recursively getting file '%s'  id %i\n", m_pIterator->m_zPath.c_str(), nNewID );
		pcChild = new DownloadJob( m_pcServer, zDest, m_pIterator->m_zPath, nNewID, this );
	}
	pcChild->SetOverwritePolicy( m_eOverwritePolicy, false );
	pcChild->SetFailurePolicy( m_eFailurePolicy, false );
	m_pcChildJob = pcChild;
	m_pcServer->AddJob( pcChild );
	/* Child job will be activated by _Schedule() */
	/* When the child job finishes, it will call parent->ChildTransferFinished() so that we end up back here. */
	
	m_pIterator++;
}

bool RecursiveDownloadJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "description", "Download" );
	pcMsg->AddString( "remotePath", m_zRemotePath );
	pcMsg->AddString( "localPath", m_zLocalPath );
	
	return( Job::GetJobInfo( pcMsg ) );
}

/**** RecursiveUploadJob implementation *****/
RecursiveUploadJob::RecursiveUploadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent )
	: Job( pcServer, JOB_UPLOAD, nID, pcParent )
{
	DEBUG( "RecursiveUploadJob %i created; parent %i; path %s\n", m_nID, (m_pcParent?m_pcParent->GetID():-1), zLocalPath.c_str() );
	m_zRemotePath = zRemotePath;
	m_zLocalPath = zLocalPath;
}

RecursiveUploadJob::~RecursiveUploadJob()
{
	if( m_pcDirIterator ) delete( m_pcDirIterator );
}


int RecursiveUploadJob::AttachToHandle( CurlHandle_s* psHandle )
{
	/* First open the local directory for listing; if it can't be opened then fail with error message */
	try {
		m_pcDirIterator = new Directory( m_zLocalPath );
	} catch( errno_exception& e ) {
		DEBUG( "RecursiveUploadJob: Error accessing directory '%s': %s (%i)!\n", m_zLocalPath.c_str(), e.what(), e.error() );
		String zMsg;
		zMsg.Format( "Could not read directory %s: %s", m_zLocalPath.c_str(), e.what() );
		m_pcServer->SendFailureNotification( this, zMsg );
		m_nStatus = STATUS_FAILED;
		FinaliseJob();
		return( -1 );
	}
	
	/* Then create & attach a child CommandJob to create the remote dir */
	String zCommand = "MKD ";
	zCommand += m_zRemotePath;
	std::vector<String> cTmp;
	cTmp.push_back( zCommand );
	
	int nNewID = m_pcServer->GetNewJobID();
	Job* pcChild = new CommandJob( m_pcServer, cTmp, false, nNewID, this );
	/* TODO: overwrite/failure policy here? */
	m_pcChildJob = pcChild;
	m_pcServer->AddJob( pcChild );
	m_nStatus = STATUS_PENDING_CHILD_JOB;
	m_pcServer->JobUpdated( this );
	/* Child will be activated by _Schedule() */
	
	/* The uploading will be done in ChildTransferFinished() */

	return( CURLE_OK );
}

void RecursiveUploadJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	/* Since this job never gets directly attached to a handle, this should never be used. */
	DEBUG( "RecursiveUploadJob: WARNING! RecursiveUploadJob::TransferFinished() called!\n" );
}

void RecursiveUploadJob::ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcJob )
{
	if( pcJob != NULL )
	{
		if( m_pcChildJob != pcJob ) {
			DEBUG( "RecursiveUploadJob: WARNING! Got ChildTransferFinished() for job %i which isn't current child of %i!\n", pcJob->GetID(), m_nID );
			return;
		}
		/* The child job should have cleaned itself up by now; remove it from our list. */
		m_pcChildJob = NULL;
	}

	/* Get next dir entry, skipping . and .. */
	FileReference cFileRef;
	bool bDone = false;
	status_t nResult;
	while( !bDone ) {
		nResult = m_pcDirIterator->GetNextEntry( &cFileRef );
		/* As of Syllable 0.6.7, Directory::GetNextEntry() returns 1 on *success* and 0 on *end of directory* */
		if( nResult == 0 ) break;	/* Reached end of directory */
		if( cFileRef.IsValid() && cFileRef.GetName() != "." && cFileRef.GetName() != ".." ) bDone = true;
	}
	
	if(  nResult == 0 )
	{
		DEBUG( "All children for job %i (RecursiveUploadJob) finished.\n:", m_nID );
		/* Reached end of directory. Clean up. */
		m_nStatus = STATUS_COMPLETED;
		FinaliseJob();

		/* If this job was created recursively, then pass the call on to our parent. */
		if( m_pcParent != NULL )
		{
			m_pcParent->ChildTransferFinished( psHandle, nCode, this );
			return;
		}
		return;
	}

	Job* pcChild;
	String zDest = m_zRemotePath + "/" + cFileRef.GetName();
	String zLocalPath;
	cFileRef.GetPath( &zLocalPath );
	struct stat sStat;
	cFileRef.GetStat( &sStat );	/* TODO: proper error handling */
	int nNewID = m_pcServer->GetNewJobID();
	if( S_ISDIR( sStat.st_mode ) )
	{
		DEBUG( "RecursiveUploadJob: Recursively sending dir '%s'  id %i\n", zLocalPath.c_str(), nNewID );
		pcChild = new RecursiveUploadJob( m_pcServer, zLocalPath, zDest, nNewID, this );
	} else {
		DEBUG( "RecursiveUploadJob: Recursively sending file '%s'  id %i\n", zLocalPath.c_str(), nNewID );
		pcChild = new UploadJob( m_pcServer, zLocalPath, zDest, nNewID, this );
	}
	pcChild->SetOverwritePolicy( m_eOverwritePolicy, false );
	pcChild->SetFailurePolicy( m_eFailurePolicy, false );
	m_pcChildJob = pcChild;
	m_pcServer->AddJob( pcChild );
	m_nStatus = STATUS_PENDING_CHILD_JOB;
	m_pcServer->JobUpdated( this );
	/* Child will be activated by _Schedule() */
}

bool RecursiveUploadJob::GetJobInfo( Message* pcMsg )
{
	pcMsg->AddString( "remotePath", m_zRemotePath );
	pcMsg->AddString( "localPath", m_zLocalPath );
	pcMsg->AddString( "description", "Upload" );

	return( Job::GetJobInfo( pcMsg ) );
}

/**** RecursiveDeleteJob implementation ****/

RecursiveDeleteJob::RecursiveDeleteJob( Server* pcServer, const String& zRemotePath, int nID, Job* pcParent )
	: DirListJob( pcServer, zRemotePath, nID, pcParent )
{
	DEBUG( "RecursiveDeleteJob %i created; parent %i; path '%s'\n", nID, (m_pcParent?m_pcParent->GetID():-1), zRemotePath.c_str() );
	m_nType = JOB_COMMAND;
	m_zRemotePath = zRemotePath;
	
	m_pIterator = m_acList.begin();	
	
	m_pcDeleteSelfJob = NULL;
}

RecursiveDeleteJob::~RecursiveDeleteJob()
{
}

int RecursiveDeleteJob::AttachToHandle( CurlHandle_s* psHandle )
{
	return( DirListJob::AttachToHandle( psHandle ) );
}

int RecursiveDeleteJob::RemoveFromHandle( CurlHandle_s* psHandle )
{
	return( DirListJob::RemoveFromHandle( psHandle ) );
}

void RecursiveDeleteJob::HandleDirListData( std::vector< RemoteNode >* pacList )
{
	m_acList.insert( m_acList.end(), pacList->begin(), pacList->end() );
	m_pIterator = m_acList.begin();
	delete( pacList );
}

void RecursiveDeleteJob::TransferFinished( CurlHandle_s* psHandle, long nCode )
{
	RemoveFromHandle( psHandle );
	ChildTransferFinished( psHandle, 0, NULL );	/* Call this to create the first child job */
}

void RecursiveDeleteJob::ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcJob )
{
	/* If a child job just finished, remove it from out list */
	if( pcJob != NULL )
	{
		if( m_pcChildJob != pcJob ) {
			DEBUG( "RecursiveDeleteJob: WARNING! Got TransferFinished() for job %xi which isn't current child of %i!\n", pcJob->GetID(), m_nID );
			return;
		}
		/* The child job should have cleaned itself up by now; remove it from our list. */
		m_pcChildJob = NULL;
	}
	
	/* If the final job, to rmdir this directory, has finished, then clean ourself up. */
	if( pcJob != NULL && pcJob == m_pcDeleteSelfJob )
	{
		DEBUG( "All children for job %i (RecursiveDeleteJob) finished.\n:", m_nID );
		/* Everything is finished. Clean up this job. */
		m_pcDeleteSelfJob = NULL;	/* It should have cleaned itself up by now */
		m_nStatus = STATUS_COMPLETED;
		FinaliseJob();

		/* If this job was created recursively, then pass the call on to our parent. */
		if( m_pcParent != NULL )
		{
			m_pcParent->ChildTransferFinished( psHandle, nCode, this );
			return;
		}
		return;
	}
	
	if( m_pIterator == m_acList.end() )
	{
		/* Delete the current dir finally */
		int nNewID = m_pcServer->GetNewJobID();
		DEBUG( "RecursiveDeleteJob: Deleting base dir '%s'  id %i\n", m_zRemotePath.c_str(), nNewID );

		String zCommand = "RMD ";
		zCommand += m_zRemotePath;
		std::vector<String> cTmp;
		cTmp.push_back( zCommand );

		m_pcDeleteSelfJob = new CommandJob( m_pcServer, cTmp, false, nNewID, this );
		m_pcDeleteSelfJob->SetFailurePolicy( m_eFailurePolicy, false );
		
		m_pcChildJob = m_pcDeleteSelfJob;
		m_pcServer->AddJob( m_pcDeleteSelfJob );
		m_nStatus = STATUS_PENDING_CHILD_JOB;
		m_pcServer->JobUpdated( this );
		/* Child will be activated by _Schedule() */
		return;
	}

	/* Iterate through the entry list and create & attach a Job for each entry. */
	/* m_pIterator should already be valid; it is initially set to m_acList.start() in HandleDirListData(). */
	Job* pcChild;
	int nNewID = m_pcServer->GetNewJobID();
	if( m_pIterator->IsDir() )
	{
		DEBUG( "RecursiveDeleteJob: Recursively deleting dir '%s'  id %i\n", m_pIterator->m_zPath.c_str(), nNewID );
		pcChild = new RecursiveDeleteJob( m_pcServer, m_pIterator->m_zPath, nNewID, this );
	} else {
		DEBUG( "RecursiveDeleteJob: Recursively deleting file '%s'  id %i\n", m_pIterator->m_zPath.c_str(), nNewID );
		String zCommand = "DELE ";
		zCommand += m_pIterator->m_zPath;
		std::vector<String> cTmp;
		cTmp.push_back( zCommand );

		pcChild = new CommandJob( m_pcServer, cTmp, true, nNewID, this );
	}
	pcChild->SetFailurePolicy( m_eFailurePolicy, false );
	m_pcChildJob = pcChild;
	m_pcServer->AddJob( pcChild );
	m_nStatus = STATUS_PENDING_CHILD_JOB;
	m_pcServer->JobUpdated( this );
	/* Child will be activated by _Schedule() */
	/* When the child job finishes, it will call parent->ChildTransferFinished() so that we end up back here. */
	
	m_pIterator++;	
}

