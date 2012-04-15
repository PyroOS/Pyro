#ifndef __SERVER_H__
#define __SERVER_H__

#include <util/string.h>
#include <util/locker.h>
#include <gui/window.h>
#include <map>

#include "transferthread.h"
#include "job.h"
#include "settings.h"

using namespace os;

#ifndef DEBUG
extern bool g_bDebug;
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif

class AppSettings;

/** \brief Server class.
 * This is the base server class that provides a common interface for
 * accessing various types of servers. This should be sub-classed for
 * the different types of servers.
 *
 * \todo ParseDirListing function should be removed if it is unnecessary.
 */
class Server
{
public:
	Server( const String& zURL, const String& zUsername, const String& zPassword, os::Window* pcTarget, AppSettings* pcSettings );
	virtual ~Server();
	int Close();
	
	int Lock();	/**< \brief Lock the Server lock. This lock should be held whenever accessing the job list. */
	int Unlock();	/**< \brief Unlock the Server lock. */
	
	AppSettings* GetSettings() { return( m_pcSettings ); }
	
	String GetServerAddress();
	String GetEntryPath();
	
	/* Methods for creating jobs by the GUI */
	int GetDirListing( const String& zPath, Handler* pcTarger );
	int GetRemoteFile( const String& zRemoteSource, const String& zLocalDest );
	int SendLocalFile( const String& zLocalSource, const String& zRemoteDest );
	int GetRemoteDir( const String& zRemoteSource, const String& zLocalDest );
	int SendLocalDir( const String& zLocalSource, const String& zRemoteDest );
	int DeleteRemoteFile( const String& zPath );
	int RemoveRemoteDir( const String& zPath );
	int CreateRemoteDir( const String& zPath );
	int MoveRemote( const String& zOldPath, const String& zNewPath );


	/* Methods for controlling active transfers, eg pause, cancel, resume */
	int PauseTransfer( int nJobID );
	int ResumeTransfer( int nJobID );
	int CancelTransfer( int nJobID );
	
	/* Methods for communicating between backend and gui. */
	void JobUpdated( Job* pcJob );
	void SendFailureNotification( Job* pcJob, String& zErrorMessage );
	void SendOverwriteNotification( Job* pcJob, bool bMultiJob );
	void SendGUIReply( Message* pcMsg );
	
	/* Methods for manipulating the queue (ie raise/lower priority of a queued transfer) */
	/* TODO */

	/* These methods are used by the backend. Perhaps they should be private? But then we'd need to make all the Jobs be friends. Unless there's a better way? */
	int GetNewJobID();
	int AddJob( Job* pcJob );
	int RemoveJob( Job* pcJob );

private:
	/* These methods are only called from TransferThread */

	virtual CURLcode _SetupCurlHandle( CURL* pHandle );	/* Add our settings to the curl handle */
	
	/* This is used by jobs to provide the ftp entry path */
	int _SetEntryPath(  const char* pzPath );

private:

	String m_zURL; /**< Address of the server. */
	String m_zUsername;	/**< Username to use for login. */
	String m_zPassword;	/**< Password to use for login. */

	os::Window* m_pcTarget; /**< The window that created us. */
	
	TransferThread* m_pcThread;	/**< Our transfer thread, that does all the work. */

	AppSettings* m_pcSettings;	/**< Pointer to the app settings object. */
	
	
	/** \brief List of all queued and running transfers stored as a map from id numbers to Job objects.
	 * The reason for using a std::map and giving each job an id number is so that the gui can refer to jobs
	 * by their id rather than using a pointer. Since we can't be sure that the transfer thread hasn't deleted a job
	 * while the gui is still using the pointer.
	 */
	std::map< int, Job* > m_apcQueue;
	
	
	int m_nNextID; /**< Next available ID number. */
	Locker* m_pcLock; /**< Lock used for concurrency. */
	
	String m_zEntryPath;	/**< 'Entry path' of the ftp server - pointer provided by libcurl. */
	
	friend class TransferThread;
	friend class DirListJob;	/* For setting entry path */
};

#if 0
/** \brief FTP Server
 * This is an instance of the Server class that implements the
 * FTP Protocols.
 *
 * \todo Functionality specific for FTP style connections should be moved here
 * in order to better abstract the Server object.
 */
class FTPServer : public Server
{
public:
	FTPServer( const String& zURL, const String& zUsername, const String& zPassword, Window* pcTarget, AppSettings* pcSettings );

//	int ParseDirListing( const String& zDir, char* pzBuf, size_t nSize, Handler* pcTarget, bool bInitial );
	
private:
	~FTPServer();
//	CURLcode _SetupCurlHandle( CURL* pHandle );
};
#endif


#endif	/* __SERVER_H__ */

