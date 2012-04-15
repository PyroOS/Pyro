#ifndef __TRANSFERTHREAD_H__
#define __TRANSFERTHREAD_H__

#include <util/thread.h>
#include <util/message.h>
#include <util/locker.h>
#include <list>
#include <queue>

#include <curl/curl.h>

using namespace os;

#ifndef DEBUG
extern bool g_bDebug;
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif

class Server;
class Job;

class os::Handler;

/** CurlHandleStatus
 * Status numbers for CurlHandle_s */
enum CurlHandleStatus {
	HANDLE_IDLE = -1,
	HANDLE_INVALID = -2
};

/** \brief Structure for storing the pointer to the curl handle and its
 * current status.
 */
typedef struct {
	CURL*	m_pHandle; /**< The curl easy handle. */
	int		m_nStatus;	/**< If positive, it is the index of a Job in the Server's list; if negative it is one of the enum in CurlHandleStatus. */
	CURLM*	m_pMultiHandle;	/**< The curl multi handle. This is provided so that nodes can remove and add_easy_handle when they make changes, as a workaround for a libcurl bug. */
} CurlHandle_s;

/** \brief Class for transfering items in a threaded manner.
 * This is the thread in which all the transfers are done via calls to 
 * curl_multi_perform().
 *
 * \todo The class needs to be cleaned up properly.
 */
class TransferThread : public Thread
{
public:
	TransferThread( Server* pcServer );
	~TransferThread();
	
	/* Main thread code is in Run() */
	int32 Run();
	
	/* Lock/unlock the message list so no-one will change it while we are using it */
	int Lock();
	int Unlock();
	
	/* Send a message to the transfer thread asking it to add, start, etc a given transfer */
	int SendMessage( int nCode );
	int SendMessage( Message* pcMsg );
	
private:
	void _Notify();		/* Send a signal to the thread to notify it that it has new messages */
	
	bool _ProcessMessages();	/* Process the messages in the message list */
	
	void _Schedule();	/* Assign queued transfers from the queue to idle curl handles */
	
	void _Close();		/* Clean up, delete objects and kill the thread */

	void _PauseJob( int nID );
	void _ResumeJob( int nID );
	void _CancelJob( int nID );
	void _CleanupJob( int nJob );		/* Remove & delete a finished queue job */
	
	
	/** \brief Our server object */
	Server* m_pcServer;
	
	/* Inter-thread communication */
	int m_nReadPipe;  /**< Read pipe for communication. See pipe function in unistd.h. */
	int m_nWritePipe; /**< Write pipe for communication. See pipe function in unistd.h. */
	std::queue< Message* > m_apcMessages; /**< Stores the queue of messages for this thread.  */
	Locker* m_pcLock; /**< \brief A mutex lock for the thread. */
	
	/* libcurl stuff */
	CURLM* m_pCurlMultiHandle; /**< Our CURLM handle. Contains a pointer to our curl_multi_init() handle. */
	std::vector< CurlHandle_s* > m_asHandles;	/**< List of all the curl handles we own, and the jobs they are currently attached to. */
	
	friend class Server;
};


#endif	/* __TRANSFERTHREAD_H__ */
