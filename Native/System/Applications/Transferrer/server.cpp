#include "server.h"
#include "job.h"
#include "remotenode.h"
#include "settings.h"
#include "messages.h"

/** \brief Default constructor for the Server.
 * This initializes all the details about the server.
 *
 * \param zURL This is the string containing the parsed server URL to connect to.
 * \param pcTarget Pointer to the handler for server messages (ie. the window containing the server).
 * \param nParallelConnections Number of allowable parallel connections (simultaneous transfers).
 */
Server::Server( const String& zURL, const String& zUsername, const String& zPassword, os::Window* pcTarget, AppSettings* pcSettings )
{
	m_zURL = zURL;
	m_zUsername = zUsername;
	m_zPassword = zPassword;
	m_pcTarget = pcTarget;
	m_pcSettings = pcSettings;
	m_zEntryPath = "";
	
	m_nNextID = 1;
	
	m_pcLock = new Locker( "server_lock" );
	
	m_pcThread = new TransferThread( this );
	m_pcThread->Start();
}

/** \brief Server Deconstructor.
 * This removes the server lock and quits. The m_pcThread is cleaned up
 * on its own when it receives the M_TT_CLOSE message.
 *
 * \todo Is it true that all other cleanup is done in TransferThread.
 * \todo I want this to be private but can't be bothered debugging the error messages yet.
 */
Server::~Server()
{
	delete( m_pcLock );
}

/** \brief Lock the Server.
 * This acts as a mutex lock for the Server so that the internal
 * data structures aren't corrupted due to concurrency issues.
 *
 * \warning Must call Lock() before accessing the job queue.
 */
int Server::Lock()
{
	return( m_pcLock->Lock() );
}

/** \brief Unlock the Server.
 * This unlocks the Server so that other items can operate on
 * the server.
 *
 * \warning This must be called by the function that called Lock() to
 * allow it to be used by other parts of the application.
 */
int Server::Unlock()
{
	return( m_pcLock->Unlock() );
}

/** \brief Returns the server address.
 * Returns the address part of the url, eg ftp://my.ftp.com
 */
String Server::GetServerAddress()
{
	return( m_zURL );
}

/** \brief Get the entry path of the server.
 *  Returns the entry path for the user's ftp session.
 *  This is the current directory immediately after logging on to the server (eg, user's home dir).
 *  The pointer is provided by libcurl.
 *  If libcurl hasn't provided the entry path yet, this returns NULL.
 */
String Server::GetEntryPath()
{
	return( m_zEntryPath );
}

/** \brief Save the entry path for the server.
 *  This is the current directory immediately after logging on to the server (eg, user's home dir).
 *  \sa Server::GetEntryPath()
 */
int Server::_SetEntryPath( const char* pzPath )
{
	m_zEntryPath = pzPath;
	return( 0 );
}

/** \brief Close the server connection.
 * Stop all transfers, close the server connection, 
 * free curl handles, stop transfer thread, delete self.
 */
int Server::Close()
{
	m_pcThread->SendMessage( M_TT_CLOSE );
	return( 0 );
}

/** \brief Get a new ID number which can be used for a newly created job.
 */
int Server::GetNewJobID()
{
	int nNewID;
	
	Lock();
	nNewID = m_nNextID++;
	Unlock();
	
	return( nNewID );
}


/** \brief Add a newly created job to the job list.
 */
int Server::AddJob( Job* pcJob )
{
	Lock();
	m_apcQueue[pcJob->m_nID] = pcJob;
	JobUpdated( pcJob );
	Unlock();
	
	m_pcThread->SendMessage( M_TT_SCHEDULE );
	
	return( 0 );
}

/** \brief Send a message to the transfer thread to delete a job & remove it from the job lists.
 * \note The job must already have cleaned itself up and have detached itself from any curl handles.
 */
int Server::RemoveJob( Job* pcJob )
{
	Message* pcMsg = new Message( M_TT_REMOVE );
	pcMsg->AddInt32( "id", pcJob->m_nID );
	return( m_pcThread->SendMessage( pcMsg ) );
}

/** \brief Get Remote file.
 * Queue a download from the remote file to the local file.
 *
 * \param zRemoteSource Remote source relative to the ftp server, eg /remotedir/file
 * \param zLocalDest Local destination for the file.
 * \retval The ID of the newly created job, or -1 for error.
 */
int Server::GetRemoteFile( const String& zRemoteSource, const String& zLocalDest )
{
//	DEBUG( "GetRemoteFile( %s, %s )\n", zRemoteSource.c_str(), zLocalDest.c_str() );

	int nID;

	Lock();	/* While accessing job list */

#if 0	/* AWM Fixme */
	bool bFound = false;
	
	std::map< int, Job* >::iterator i = m_apcQueue.begin();
	/* Check that there isn't already a queued transfer with this source and destination */
	for( ; i != m_apcQueue.end(); i++ ) {
		Job* pcJob = i->second;
		if( pcJob->m_zLocalPath == zLocalDest && pcJob->m_zRemotePath == zRemoteSource && pcJob->m_nType == JOB_DOWNLOAD ) {
			DEBUG( "Server::GetRemoteFile: Already got a download job queued for %s to %s\n", zRemoteSource.c_str(), zLocalDest.c_str() );
			bFound = true;
			break;
		}
	}
	if( bFound ) {
		Unlock();
		return( -1 );
	}
#endif
	nID = GetNewJobID();
	Unlock();
	Job* pcJob = new DownloadJob( this, zLocalDest, zRemoteSource, nID );

	AddJob( pcJob );
	return( nID );
}

/** \brief Send a local file.
 * Queue an upload of a local file to a remote file. 
 *
 * \param zRemoteDest Remote destination relative to the ftp server, eg /remotedir/file
 * \param zLocalSource Local source where the local file is coming from.
 * \retval The ID of the newly created job, or -1 for error.
 */
int Server::SendLocalFile( const String& zLocalSource, const String& zRemoteDest )
{
//	DEBUG( "SendLocalFile( %s, %s )\n", zLocalSource.c_str(), zRemoteDest.c_str() );
	
	int nID;

	Lock();	/* While accessing job list */

#if 0 /* AWM Fixme */
	bool bFound = false;
	
	std::map< int, Job* >::iterator i = m_apcQueue.begin();
	/* Check that there isn't already a queued transfer with this source and destination */
	for( ; i != m_apcQueue.end(); i++ ) {
		Job* pcJob = i->second;
		if( pcJob->m_zLocalPath == zLocalSource && pcJob->m_zRemotePath == zRemoteDest && pcJob->m_nType == JOB_UPLOAD ) {
			DEBUG( "Server::SendLocalFile: Already got an upload job queued for %s to %s\n", zLocalSource.c_str(), zRemoteDest.c_str() );
			bFound = true;
			break;
		}
	}
	if( bFound ) {
		Unlock();
		return( -1 );
	}
#endif
	nID = GetNewJobID();
	Unlock();
	Job* pcJob = new UploadJob( this, zLocalSource, zRemoteDest, nID );

	AddJob( pcJob );
	return( nID );
}

/** \brief Get Directory Listing.
 * Create a job to request a directory listing for a remote directory.
 * The job will parse the directory listing text into RemoteNodes,
 * which will be sent to the target Handler (a RemoteView) to be displayed, etc.
 *
 * \param zPath The path to get a directory listing of.
 * \param pcTarget The handler to send the resulting nodes to.
 * \retval The ID of the newly created job, or -1 for error.
 */
int Server::GetDirListing( const String& zPath, Handler* pcTarget )
{
	int nID;
	
	nID = GetNewJobID();
	
	GUIDirListJob* pcJob = new GUIDirListJob( this, zPath, pcTarget, nID );
//	DEBUG( "GetDirListing( %s ) created job %i\n", zPath.c_str(), nID );
	
	AddJob( pcJob );
	return( nID );
}

/** \brief Recursively download a directory & its contents.
 * Creates a RecursiveDownloadJob which will create the dir, get a dir listing and download the contents.
 */
int Server::GetRemoteDir( const String& zRemoteSource, const String& zLocalDest )
{
//	DEBUG( "GetRemoteDir( %s, %s )\n", zRemoteSource.c_str(), zLocalDest.c_str() );
	
	int nID;

	Lock();	/* While accessing job list */
#if 0	/* AWM Fixme */
	bool bFound = false;
	
	std::map< int, Job* >::iterator i = m_apcQueue.begin();
	/* Check that there isn't already a queued transfer with this source and destination */
	for( ; i != m_apcQueue.end(); i++ ) {
		Job* pcJob = i->second;
		if( pcJob->m_zLocalPath == zLocalDest && pcJob->m_zRemotePath == zRemoteSource && pcJob->m_nType == JOB_DOWNLOAD ) {
			DEBUG( "Server::GetRemoteDir: Already got a download job queued for %s to %s\n", zRemoteSource.c_str(), zLocalDest.c_str() );
			bFound = true;
			break;
		}
	}
	if( bFound ) {
		Unlock();
		return( -1 );
	}
#endif
	nID = GetNewJobID();
	Unlock();
	Job* pcJob = new RecursiveDownloadJob( this, zLocalDest, zRemoteSource, nID );

	AddJob( pcJob );
	return( nID );
}


/** \brief Recursively upload a directory & its contents.
 * Creates a RecursiveUploadJob which will walk the directory and recursively upload all subdirs & files.
 */
int Server::SendLocalDir( const String& zLocalSource, const String& zRemoteDest )
{
//	DEBUG( "SendLocalDir( %s, %s )\n", zLocalSource.c_str(), zRemoteDest.c_str() );
	
	int nID;

	Lock();	/* While accessing job list */
#if 0	/* AWM Fixme */
	bool bFound = false;
	
	std::map< int, Job* >::iterator i = m_apcQueue.begin();
	/* Check that there isn't already a queued transfer with this source and destination */
	for( ; i != m_apcQueue.end(); i++ ) {
		Job* pcJob = i->second;
		if( pcJob->m_zLocalPath == zLocalSource && pcJob->m_zRemotePath == zRemoteDest && pcJob->m_nType == JOB_UPLOAD ) {
			DEBUG( "Server::SendLocalDir: Already got an upload job queued for %s to %s\n", zLocalSource.c_str(), zRemoteDest.c_str() );
			bFound = true;
			break;
		}
	}
	if( bFound ) {
		Unlock();
		return( -1 );
	}
#endif
	nID = GetNewJobID();
	Unlock();
	Job* pcJob = new RecursiveUploadJob( this, zLocalSource, zRemoteDest, nID );

	AddJob( pcJob );
	return( nID );
}

/** \brief Delete a remote file.
 * Queue a delete command to delete a remote file.
 */
int Server::DeleteRemoteFile( const String& zPath )
{
	int nID;

	nID = GetNewJobID();

	Job* pcJob = new DeleteJob( this, zPath, nID );

	AddJob( pcJob );
	return( nID );
}

/** \brief Delete a remote directory.
 * Creates a RecursiveDeleteJob to recursively delete the contents of a directory
 * and then remove the dir itself when empty.
 */
int Server::RemoveRemoteDir( const String& zPath )
{
	int nID;

	nID = GetNewJobID();
	
	Job* pcJob = new RecursiveDeleteJob( this, zPath, nID );
	
	AddJob( pcJob );
	return( nID );
}

/** \brief Create a remote directory.
 * Queue a mkdir command to create a remote directory.
 */
int Server::CreateRemoteDir( const String& zPath )
{
	int nID;
	
	nID = GetNewJobID();
	
	Job* pcJob = new MkDirJob( this, zPath, nID );

	AddJob( pcJob );
	return( nID );
}

/** \brief Move/rename a remote file or dir.
 * Tells the transfer thread to queue a request to rename the given file or dir.
 */
int Server::MoveRemote( const String& zOldPath, const String& zNewPath )
{
	int nID;
	
	nID = GetNewJobID();
	
	Job* pcJob = new RenameJob( this, zOldPath, zNewPath, nID );

	AddJob( pcJob );
	return( nID );
}


/** \brief Pause a job.
 * This pauses the job with the given id.
 *
 * \param nJobID The job ID of the job to pause.
 */
int Server::PauseTransfer( int nJobID )
{
	Message* pcMsg = new Message( M_TT_PAUSE );
	pcMsg->AddInt32( "id", nJobID );
	return( m_pcThread->SendMessage( pcMsg ) );
}

/** \brief Resume (unpause) a job.
 * This resumes the job with the given id.
 *
 * \param nJobID The job ID of the job to resume
 */
int Server::ResumeTransfer( int nJobID )
{
	Message* pcMsg = new Message( M_TT_RESUME );
	pcMsg->AddInt32( "id", nJobID );
	return( m_pcThread->SendMessage( pcMsg ) );
}

/** \brief Cancel a job.
 * This cancels the job with the given id.
 *
 * \param nJobID The ID of the job to cancel.
 */
int Server::CancelTransfer( int nJobID )
{
	Message* pcMsg = new Message( M_TT_CANCEL );
	pcMsg->AddInt32( "id", nJobID );
	return( m_pcThread->SendMessage( pcMsg ) );
}

/**** The following methods are only called from within the transfer thread ****/

/** \brief Setup Curl Handle.
 * This initializes the curl handler for the FTP connection.
 *
 * \param pHandle The pointer to the CURL handle.
 *
 * \todo This should be a virtual function that is called by the particular server
 * type. Currently it directly implements FTP style transfers which defeates the
 * purpose of having separate Server and FTPServer classes.
 */
CURLcode Server::_SetupCurlHandle( CURL* pHandle )
{
	curl_easy_setopt( pHandle, CURLOPT_VERBOSE, m_pcSettings->GetDebugMode() );	/* For testing */
	curl_easy_setopt( pHandle, CURLOPT_FTPPORT, m_pcSettings->GetPassive() ? NULL : "-" );
	curl_easy_setopt( pHandle, CURLOPT_FTP_RESPONSE_TIMEOUT, (long)20 );
	curl_easy_setopt( pHandle, CURLOPT_TIMEOUT, (long)100000 );
	curl_easy_setopt( pHandle, CURLOPT_FTP_FILEMETHOD, CURLFTPMETHOD_NOCWD );
#if 0
	/* These options work only from libcurl version 7.19.1 */
	/* TODO: use a #ifdef here			 */
	/* TODO: Make this a runtime check rather than compile-time */
	curl_easy_setopt( pHandle, CURLOPT_USERNAME, m_zUsername.c_str() );
	curl_easy_setopt( pHandle, CURLOPT_PASSWORD, m_zPassword.c_str() );
#else
	/* Must use older style for earlier libcurl versions */
	String zUserPass = m_zUsername;
	zUserPass += ":";
	zUserPass += m_zPassword;
	curl_easy_setopt( pHandle, CURLOPT_USERPWD, zUserPass.c_str() );
#endif
	/* That's it for now; other options will be set by subclasses or by Job::AttachToHandle() */
	return( CURLE_OK );
}


/** \brief Notify the gui that a given transfer's status has changed.
 * This is used to send a message to the main window signifying that some of the
 * downloads/uploads have been updated.
 *
 * If called from a GUI thread, the server lock must be held.
 *
 * \param pcJob: The job that has been updated.
 */
void Server::JobUpdated( Job* pcJob )
{
	Message cMsg( M_JOB_UPDATED );

	bool bSend = pcJob->GetJobInfo( &cMsg );

	if( bSend ) m_pcTarget->PostMessage( &cMsg, m_pcTarget );
}

/** \brief Notify the GUI that an error occurred during a transfer.
 * This is used to notify the GUI that an error occurred during a transfer,
 * and the transfer did not complete successfully.  It sends a message to the GUI
 * which will then display a notification with the error message.
 *
 * \param pcJob: The job that has experienced an error.
 * \param zErrorMessage: A message describing the error that should be displayed to the user.
 */
void Server::SendFailureNotification( Job* pcJob, String& zErrorMessage )
{
	Message cMsg( M_JOB_ERROR );
	
	pcJob->GetJobInfo( &cMsg );
	cMsg.AddString( "message", zErrorMessage );
	
	m_pcTarget->PostMessage( &cMsg, m_pcTarget );
}


/** \brief Notify the GUI that a destination file exists and ask the user whether to overwrite.
 * This is used to notify the GUI that a destinattion file exists,
 * and ask the user whether to overwrite it.  It sends a message to the GUI
 * which will then display a overwrite notification.
 *
 * \param pcJob The job sending the overwrite notification
 * \param bMultiJob True if the job is part of a recursive job and the
 *   'Do this for all remaining files' checkbox should be shown; false if not.
 */
void Server::SendOverwriteNotification( Job* pcJob, bool bMultiJob )
{
	Message cMsg( M_JOB_OVERWRITE_QUERY );
	
	pcJob->GetJobInfo( &cMsg );
	cMsg.AddBool( "multi", bMultiJob );
	
	m_pcTarget->PostMessage( &cMsg, m_pcTarget );
}

/** \brief Called by the GUI to pass a user's reply to a notification dialog back to the job.
 */
void Server::SendGUIReply( Message* pcMsg )
{
	Message* pcNewMessage = new Message;
	*pcNewMessage = *pcMsg;
	m_pcThread->SendMessage( pcNewMessage );
}


#if 0
/**** FTPServer class ****
 * Not implemented yet - all the functionality is in base Server class. 
 * Some functionality should be moved to FTPServer, or maybe we
 * can delete FTPServer.
 *************************/


/** \brief FTPServer Constructor.
 *
 * \todo This should be filled in with things that are specific to FTP Server Connections.
 */
FTPServer::FTPServer( const String& zURL, const String& zUsername, const String& zPassword, Window* pcTarget, AppSettings* pcSettings )
	: Server( zURL, zUsername, zPassword, pcTarget, pcSettings )
{
}


/** \brief FTPServer Deconstructor.
 * Currently doesn't have to delete any allocated items.
 */
FTPServer::~FTPServer()
{
}
#endif
