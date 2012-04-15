#ifndef __JOB_H__
#define __JOB_H__

#include <storage/file.h>
#include <util/string.h>
#include <util/handler.h>
#include <curl/curl.h>

#include <vector>

#include "remotenode.h"
#include "transferthread.h"

using namespace os;

#ifndef DEBUG
extern bool g_bDebug;
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif

// Prototype for the Server class.
class Server;
class os::Directory;

/* Job status enum */
enum {
	STATUS_QUEUED = 1,
	STATUS_RUNNING,
	STATUS_PAUSED = 16,
	STATUS_PENDING_CHILD_JOB,
	STATUS_PENDING_USER_RESPONSE,
	STATUS_COMPLETED = 32,
	STATUS_CANCELLED,
	STATUS_SKIPPED,
	STATUS_FAILED,
};

#define STATUS_IS_ACTIVE( status ) (status >= STATUS_QUEUED && status < STATUS_PAUSED)
#define STATUS_IS_SUSPENDED( status ) (status >= STATUS_PAUSED && status < STATUS_COMPLETED)
#define STATUS_IS_FINISHED( status ) (status >= STATUS_COMPLETED)

/* Overwrite policy enum */
enum overwrite_policy {
	OP_ASK,			/* Ask the user whenever a destination file exists */
	OP_SKIP,		/* Skip a file if the destination exists */
	OP_OVERWRITE	/* Overwrite destination, even if it already exists */
};

/* Failure policy enum */
enum failure_policy {
	FP_ASK,		/* Ask the user whenever a job fails */
	FP_SKIP,	/* Ignore a failed job and continue with the rest of the recursive job */
	FP_CANCEL	/* Cancel a recursive job whenever one child job fails */
};

/** \brief Class representing a task, such as a single up/download or a recursive download.
 */
class Job
{
protected:
	Job( Server* pcServer, int nType, int nID, Job* pcParent = NULL );

public:
	virtual ~Job();	/* I want this to be private but can't be bothered debugging the compiler errors for now */
	
	/** \brief Save status information about the job into the given Message. */
	virtual bool GetJobInfo( Message* pcMsg );
	
private:

	/** \brief Read data into the provided buffer.
	 *
	 * \param pBuf Pointer to the buffer to read data into.
	 * \param nSize The amount of data to be read.
	 *
	 * \warning This is only a place holder and does nothing.
	 * It is meant to be subclassed by the different types of Job.
	 */
	virtual size_t Read( void* pBuf, size_t nSize )
	{
		DEBUG( "Warning: Job::Read() called!\n" ); 
		return( 0 );
	}
	
	/** \brief Write data from the provided buffer.
	 *
	 * \param pBuf The location of the data that should be read from to write information.
	 * \param nSize Size of the data that is in pBuf.
	 * \warning This is only a place holder and does nothing.
	 * It is meant to be subclassed by the different types of Job.
	 */
	virtual size_t Write( void* pBuf, size_t nSize )
	{
		DEBUG( "Warning: Job::Write() called!\n" );
		return( 0 );
	}

	/** \brief Seek into the data that is being manipulated.
	 * \param nOffset Amount to offset into the data being manipulated.
	 * \param nOrigin Where to start offsetting from.
	 * \warning This is only a place holder and does nothing.
	 * It is meant to be subclassed by the different types of Job.
	 */	
	virtual int Seek( curl_off_t nOffset, int nOrigin )
	{
		DEBUG( "Warning: Job::Seek() called!\n" );
		return( -1 );
	}
	
	/** \brief Update the progress bar for the Job.
	 *
	 * \param fDownTotal Total data downloaded to be downloaded
	 * \param fDownNow Total data downloaded so far.
	 * \param fUpTotal Total data to be uploaded.
	 * \param fUpNow Total data uploaded so far.
	 *
	 * \warning This is only a place holder and does nothing.
	 * It is meant to be subclassed by the different types of Job.
	 */
	virtual int ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow )
	{
		return( 0 );
	}
	
	/** \brief Handle information from the server relating to this job.
	 *
	 * \param pHandle The curl handle of the job.
	 * \param eType The type of message from libcurl.
	 * \param pzData The message data from libcurl (not necessarily NULL-terminated).
	 * \param nSize The size of the string pzData.
	 */
	virtual int DebugHandler( CURL* pHandle, curl_infotype eType, char* pzData, size_t nSize );

	static size_t ReadCallback( void* pBuf, size_t nSize, size_t nMult, void* pCookie );
	static size_t WriteCallback( void* pBuf, size_t nSize, size_t nMult, void* pCookie );
	static int SeekCallback( void* pCookie, curl_off_t nOffset, int nOrigin );
	static int ProgressBarCallback( void* pCookie, double fDownTotal, double fDownNow, double fUpTotal, double fUpNow );
	static int DebugCallback( CURL* pHandle, curl_infotype eType, char* pzData, size_t nSize, void* pCookie );

public:
	int GetID() const
	{
		return( m_nID );
	}
	
	Job* GetParent();
	Job* GetAncestor();
	
	virtual int AttachToHandle( CurlHandle_s* psHandle );
	virtual int RemoveFromHandle( CurlHandle_s* psHandle );
	
	void FinaliseJob();
	
	virtual void Pause();
	virtual void Resume();
	virtual void Cancel();
	
	/** \brief Notification from curl that the data transfer has finished.
	 * This is called from the transfer thread when curl informs us that the data transfer for
	 * this job's curl handle is complete.
	 * For a simple standalone job, we would then want to release the curl handle, flush data to disk, clean up etc.
	 * For a recursive job, we might need to do something more complex.
	 * If the job is a child of a recursive job, we typically pass the call on to out parent by calling
	 * ChildTransferFinished() on our parent with ourself as pcJob.
	 * \param psHandle The curl handle to which the job attached itself (via a previous AttachToHandle() call)
	 * \param nCode The result code from the server, eg 226 (transfer ok)
	 */
	 /* TODO: this should return 'true' if transfer thread should call _Schedule(), false if not. */
	virtual void TransferFinished( CurlHandle_s* psHandle, long nCode );
	
	/** \brief Notification from a child job that the child has finished.
	 * This is used by recursive jobs to schedule the next job after the previous child finishes.
	 * It is called from the child's TransferFinished().
	 * \param psHandle The curl handle to which the child was attached
	 * \param nCode The result code from the server for the child job
	 * \param pcJob The child job which has finished
	 */
	virtual void ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcChild )
	{
	}
	
	/** \brief Handle a reply from an overwrite dialog.
	 * This method provides a job with the user's response to an overwrite query dialog
	 * previously triggered by the job via Server::SendOverwriteNotification().
	 * \param pcMsg The message containing the response from the GUI.
	 */
	virtual void HandleOverwriteReply( Message* pcMsg );

	/** \brief Handle a reply from a failure notification dialog.
	 * This method provides a job with the user's response to a failure dialog
	 * previously triggered by the job via Server::SendFailureNotification().
	 * \param pcMsg The message containing the response from the GUI.
	 */
	virtual void HandleFailureReply( Message* pcMsg );

	
	virtual void SetOverwritePolicy( enum overwrite_policy ePolicy, bool bSetParent = true );
	virtual void SetFailurePolicy( enum failure_policy ePolicy, bool bSetParent = true );

protected:
	/** \brief Server that this Job belongs to. */
	Server* m_pcServer;

	/** \brief Stores type of transfer this job represents.
	 * Could be: Upload, Download, Directory Listing, etc.
	 *
	 * See the enums in the header file.
	 *
	 * \todo This should be an enum type
	 */
	int m_nType;
	
	/** \brief Stores the status of the job.
	 * Should use one of the enums for Active or Queued.
	 *
	 * See the enums in the header file.
	 *
	 * \todo This should be an enum type
	 */
	int m_nStatus;
	
	/** \brief The id number of this job, which uniquely identifies us. */
	int m_nID;
	
	/** \brief Pointer to our parent Job (or NULL if none). This is used for child jobs of recursive jobs. */
	Job* m_pcParent;
	
	/** \brief The handle this job is currently attached to, or NULL if none. */
	CurlHandle_s* m_psHandle;
	
	/** \brief A buffer for libcurl to store its error messages. */
	char* m_pzErrorBuffer;
	
	/** \brief Stores the most recent response message from the FTP server. */
	String m_zFTPResponse;

	/** \brief The response code as returned by libcurl. */
	long m_nResponseCode;
	
	enum overwrite_policy m_eOverwritePolicy;
	enum failure_policy  m_eFailurePolicy;
	
	friend class TransferThread;
	friend class Server;
};

/** \brief Single file download job.
 * This Job represents an individual file to be downloaded (as opposed to a directory).
 */
class DownloadJob : public Job
{
public:
	DownloadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~DownloadJob();

	bool GetJobInfo( Message* pcMsg );

protected:
	void Pause();
	void Cancel();
	
	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	void HandleOverwriteReply( Message* pcMsg );
	void HandleFailureReply( Message* pcMsg );

	int AttachToHandle( CurlHandle_s* psHandle );
	int RemoveFromHandle( CurlHandle_s* psHandle );

	size_t Write( void* pBuf, size_t nSize );
	int Seek( curl_off_t nOffset, int nOrigin );
	int ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow );

private:
	String m_zURL;	/**< The URL to pass to libcurl for this job. */
	File* m_pcFile; /**< A pointer to the file that is being downloaded. */

	/** \brief Local path of the file to be transfered. */
	String m_zLocalPath;
	
	/** \brief Remote path of the file to be transfered. */
	String m_zRemotePath;

	/* Upload/download totals. */
	ssize_t m_nTotalSize;
	ssize_t m_nCurrentDownloaded;
	
	bool m_bStarted;	/**< Indicates whether the job has been started. */
	
	friend class TransferThread;
};

/** \brief Single file upload job.
 * This Job represents an individual file to be uploaded.
 */
class UploadJob : public Job
{
public:

	UploadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~UploadJob();

	bool GetJobInfo( Message* pcMsg );

protected:
	int AttachToHandle( CurlHandle_s* psHandle );
	int RemoveFromHandle( CurlHandle_s* psHandle );

	size_t Read( void* pBuf, size_t nSize );
	int Seek( curl_off_t nOffset, int nOrigin );
	int ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow );

	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	void ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcChild );
	void HandleOverwriteReply( Message* pcMsg );
	void HandleFailureReply( Message* pcMsg );
	
private:
	String m_zURL;	/**< The URL to pass to libcurl for this job */
	File* m_pcFile; /**< A pointer to the file that is being uploaded. */

	/** \brief Local path of the file to be transfered. */
	String m_zLocalPath;
	
	/** \brief Remote path of the file to be transfered. */
	String m_zRemotePath;

	/* Upload/download totals. */
	ssize_t m_nTotalSize;
	ssize_t m_nCurrentUploaded;
	
	bool m_bStarted;	/**< Indicates whether the job has been started. */
	bool m_bChildFinished;
	
	Job* m_pcChild;	/**< Child CommandJob for checking existence of destination. */

	friend class TransferThread;
};

/** \brief Directory Listing Job.
 * This is a subclass of the Job type that is used for
 * directory listings.
 */
class DirListJob : public Job
{
public:
	DirListJob( Server* pcServer, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~DirListJob();

	bool GetJobInfo( Message* pcMsg );
	
protected:
	int AttachToHandle( CurlHandle_s* psHandle );
	
	size_t Write( void* pBuf, size_t nSize );
	int ProgressBar( double fDownTotal, double fDownNow, double fUpTotal, double fUpNow );

	size_t ParseDirListData( const char* pzBuf, size_t nSize, std::vector< RemoteNode >* pacList );
	
	virtual void HandleDirListData( std::vector< RemoteNode >* pacList );

	/** Path of the remote directory to list. */
	String m_zRemotePath;
	
	/** URL to pass to libcurl */
	String m_zURL;
	
	/** Whether or not this is the initial phase of the directory listing download. */
	bool m_bInitial;
	
	/** \brief Stores anything after last line ending with '\n'.
	 * This is used for when the read operation did read up to the new line.
	 * It is used to continue reading from the output of Curl. */
	String m_zLastLineFragment;

	friend class TransferThread;
};

/** \brief Directory listing initiated by the GUI
 *  This DirListNode sends the directory entries to the GUI.
 */
class GUIDirListJob : public DirListJob
{
public:
	GUIDirListJob( Server* pcServer, const String& zRemotePath, Handler* pcTarget, int nID, Job* pcParent = NULL );
	~GUIDirListJob();

protected:
	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	
	void HandleDirListData( std::vector< RemoteNode >* pacList );

private:
	/** The target location to send the directory listing to. */
	Handler* m_pcTarget;
};

/** \brief A job that contains FTP commands we want to send to the server (eg 'delete').
 */
class CommandJob : public Job
{
public:
	CommandJob( Server* pcServer, const std::vector<String>& azCommands, bool bUserVisible, int nID, Job* pcParent = NULL );
	CommandJob( Server* pcServer, bool bUserVisible, int nID, Job* pcParent = NULL );
	~CommandJob();

	bool GetJobInfo( Message* pcMsg );

protected:
	int AttachToHandle( CurlHandle_s* psHandle );
	int RemoveFromHandle( CurlHandle_s* psHandle );
	
	void Init( const std::vector<String>& azCommands );
	
private:
	/** \brief True if the job was explicitly created by the user; false if it is a background job that should not be displayed in the progress window. */
	bool m_bUserVisible;
	
	std::vector<String> m_azCommands;
	
	struct curl_slist* m_psCommands;
	
	friend class TransferThread;
};


/** \brief A job representing a file to be deleted.
 */
class DeleteJob : public CommandJob
{
public:
	DeleteJob( Server* pcServer, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~DeleteJob();

	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	
	bool GetJobInfo( Message* pcMsg );

private:
	String m_zRemotePath;
};

/** \brief A job representing a directory to be created.
 */
class MkDirJob : public CommandJob
{
public:
	MkDirJob( Server* pcServer, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~MkDirJob();

	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	
	bool GetJobInfo( Message* pcMsg );

private:
	String m_zRemotePath;
};

/** \brief A job representing a file or directory to be renamed.
 */
class RenameJob : public CommandJob
{
public:
	RenameJob( Server* pcServer, const String& zRemotePath, const String& zNewName, int nID, Job* pcParent = NULL );
	~RenameJob();

	void TransferFinished( CurlHandle_s* psHandle, long nCode );

	bool GetJobInfo( Message* pcMsg );

private:
	String m_zRemotePath;
	String m_zNewName;
};


/** \brief A job representing a directory which we want to download recursively.
 *  It uses functionality from DirListJob but schedules new transfers rather than
 *  sending the data to the gui.
 */
class RecursiveDownloadJob : public DirListJob
{
public:
	RecursiveDownloadJob( Server* pcServer, const String& zLocalPath, const String& RemotePath, int nID, Job* pcParent = NULL );
	~RecursiveDownloadJob();
	
protected:
	int AttachToHandle( CurlHandle_s* psHandle );
	
	void HandleDirListData( std::vector< RemoteNode >* pacList );
	
	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	void ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcChild );

	bool GetJobInfo( Message* pcMsg );
private:
	String m_zRemotePath;
	String m_zLocalPath;
	
	bool m_bStarted;
	
	/** \brief List of the directory's contents */
	std::vector< RemoteNode > m_acList;
	
	/** \brief Iterator showing where we are up to while we download each directory list entry. */
	std::vector< RemoteNode >::iterator m_pIterator;
	
	/** \brief Pointer to the current child job */
	Job* m_pcChildJob;
	
	friend class TransferThread;
};

/** \brief A job representing a directory to be recursively uploaded to the server.
 */
class RecursiveUploadJob : public Job
{
public:
	RecursiveUploadJob( Server* pcServer, const String& zLocalPath, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~RecursiveUploadJob();

	bool GetJobInfo( Message* pcMsg );

protected:
	int AttachToHandle( CurlHandle_s* psHandle );
	
	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	void ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcChild );

private:
	String m_zRemotePath;
	String m_zLocalPath;

	/** \brief Iterator for the local directory */
	Directory* m_pcDirIterator;

	/** \brief Pointer to the current child job. */
	Job* m_pcChildJob;
};


/** \brief A job representing a directory to be recursively deleted.
 *  It uses functionality from DirListJob but schedules new jobs rather than sending the data to the gui.
 */
class RecursiveDeleteJob : public DirListJob
{
public:
	RecursiveDeleteJob( Server* pcServer, const String& zRemotePath, int nID, Job* pcParent = NULL );
	~RecursiveDeleteJob();
	
protected:
	int AttachToHandle( CurlHandle_s* psHandle );
	int RemoveFromHandle( CurlHandle_s* psHandle );

	void HandleDirListData( std::vector< RemoteNode >* pacList );	
	
	void TransferFinished( CurlHandle_s* psHandle, long nCode );
	void ChildTransferFinished( CurlHandle_s* psHandle, long nCode, Job* pcChild );

private:
	String m_zRemotePath;

	/** \brief List of the directory's contents */
	std::vector< RemoteNode > m_acList;
	
	/** \brief Iterator showing where we are up to while we download each directory list entry. */
	std::vector< RemoteNode >::iterator m_pIterator;
	
	/** \brief Pointer to our current child job. */
	Job* m_pcChildJob;
	
	/** \brief The job that will rmdir this directory once all its contents are deleted. */
	Job* m_pcDeleteSelfJob;
	
	friend class TransferThread;
};


#endif	/* __JOB_H__ */

