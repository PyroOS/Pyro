
/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"


#include <util/looper.h>
#include <util/message.h>
#include <util/thread.h>
#include <util/circularbuffer.h>
#include <util/regexp.h>
#include <pyro/kernel.h>
#include <pyro/time.h>
#include <storage/file.h>
#include <vector>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <curl/curl.h>
#include <curl/multi.h>
#include <openssl/crypto.h>

#include "Frame.h"
#include "ResourceHandleManager.h"
#include "ResourceHandleInternal.h"
#include "ResourceHandleClient.h"
#include "ResourceError.h"
#include "ResourceResponse.h"
#include "CookieJar.h"
#include "Document.h"
#include "CString.h"
#include "SyllableDebug.h"

/* XXXKV: This doesn't belong here */
os::Locker g_cGlobalMutex( "global_mutex" );

using namespace WebCore;

#include <unistd.h>
#include <fcntl.h>

class SyllableJob
{
      public:
	SyllableJob( ResourceHandle * pcHandle );
	~SyllableJob();
	bool IsHTTP()
	{
		return ( m_bIsHTTP );
	}
	ResourceHandleClient *GetClient()
	{
		ASSERT( m_pcHandle->getInternal()->m_client );
		return ( m_pcHandle->getInternal()->m_client );
	}
	bool ProcessFile();
	bool ProcessHTTP();
	bool Process();
	bool LoadHTTP( CURL* pcCURLHandle, CURLM* pcMulti );
	void ReportFailure();
	void ReportSuccess();


	/* General */
	bool m_bIsRunning;
	bool m_bCancel;
	bool m_bError;
	bool m_bIsHTTP;
	ResourceHandle *m_pcHandle;
	KURL m_cURL;
	thread_id m_hProcessingThread;
	os::Locker m_cDataLock;

	/* File jobs */
	int m_nFile;

	/* HTTP jobs */
	int m_nSocket;
	bool m_bMetaDataRead;
	bool m_bMetaDataPassed;
	bool m_bDataLoaded;
	std::vector < os::String >m_cMetaData;
	os::CircularBuffer m_cDataBuffer;
};

class SyllableHTTPWorker : public os::Thread
{
public:
	SyllableHTTPWorker();
	int32 Run();
};

class SyllableLoader:public os::Thread
{
public:
	SyllableLoader();
	~SyllableLoader();
	void AddJob( ResourceHandle * pcHandle );
	void CancelJob( ResourceHandle * pcHandle );
	int32 Run();
	SyllableJob* GetNextHTTPJob( thread_id hThread );
	void WorkerFinished();
	void Quit()
	{
		m_bQuit = true;
	}
      private:
    bool m_bQuit;
	std::vector < SyllableJob * >m_cJobs;
	int m_nRunningWorkers;
};

static SyllableLoader *g_pcLoader = NULL;


SyllableJob::SyllableJob( ResourceHandle * pcHandle ) : m_cDataLock( "webcore_job_datalock" ), m_cDataBuffer( 3, 65536 )
{
	m_cURL = pcHandle->getInternal()->m_request.url();
	m_bIsHTTP = m_cURL.protocol(  ) == "http" || m_cURL.protocol(  ) == "https";
	m_pcHandle = pcHandle;
	m_hProcessingThread = -1;
	m_nFile = m_nSocket = -1;
	m_bError = false;
	m_bMetaDataRead = false;
	m_bMetaDataPassed = false;
	m_bDataLoaded = false;
	m_bCancel = false;
	m_bIsRunning = false;
}

SyllableJob::~SyllableJob()
{
	/* Cleanup */
	if( !m_bIsHTTP )
	{
		if( m_nFile >= 0 )
			close( m_nFile );
	}
	else
	{
		if( m_nSocket >= 0 )
			close( m_nSocket );
	}
}

bool SyllableJob::ProcessFile()
{
	if( m_nFile < 0 )
	{
		/* Open file */
		os::String cFile( m_cURL.path (  ) );
		if( cFile.empty() )
		{
			ReportFailure();
			return ( false );			
		}
		DEBUG( "Loading %s...\n", cFile.c_str() );
		m_nFile = open( cFile.c_str(), O_RDONLY );
		if( m_nFile < 0 )
		{
			DEBUG( "Could not open file!\n" );
			ReportFailure();
			return ( false );
		}
		/* FIXME: Use registrar */
		ResourceResponse cResponse( m_cURL, "text/html", 0, "", "" );
		GetClient()->didReceiveResponse( m_pcHandle, cResponse );
		return ( true );
	}

	/* Read data */
	char buffer[4096];
again:
	size_t nLength = read( m_nFile, buffer, 4096 );

	if( nLength < 0 )
	{
		DEBUG( "Could not read from %s!\n", os::String ( m_cURL.string(  ) ).c_str(  ) );
		ReportFailure();
		return ( false );
	}
	/* Pass data to client */
	if( nLength > 0 )
	{
		GetClient()->didReceiveData( m_pcHandle, buffer, nLength, nLength );
	}
	if( nLength < 4096 )
	{
		ReportSuccess();
		return ( false );
	}
	goto again;
	/* Never reached */
	return( true );
}



bool SyllableJob::ProcessHTTP()
{
	if( m_bError )
	{
		ReportFailure();
		return ( false );
	}

	if( m_bCancel )
		return( false );

	if( !m_bMetaDataRead )
		return ( true );

	if( !m_bMetaDataPassed )
	{
		os::String cMimeType;
		os::String cCharset;
		os::String cLocation;
		int nResponseCode = 0;
		int nDataSize = 1;
		os::String cTotalResponseString;

		/* Check the fields we need to build the response object */
		for( uint i = 0; i < m_cMetaData.size(); i++ )
		{
			cTotalResponseString += m_cMetaData[i] + "\r\n";
			/* Check for the response code */
			if( strncasecmp( m_cMetaData[i].c_str(), "HTTP/", 5 ) == 0 )
			{
				nResponseCode = atoi( m_cMetaData[i].c_str() + 9 );
			}

			/* Check meta data in the format Key:Value */
			size_t nIndex = m_cMetaData[i].find( ":" );

			if( nIndex != std::string::npos )
			{
				os::String cKey( m_cMetaData[i], 0, nIndex );
				os::String cValue( m_cMetaData[i], nIndex + 1, -1 );

				cKey.Strip();
				cValue.Strip();

				if( cKey.CompareNoCase( "Location" ) == 0 )
				{
					cLocation = cValue;
				}
				else if( cKey.CompareNoCase( "Content-type" ) == 0 )
				{
					int nEnd = cValue.find( ";" );
					if( nEnd != std::string::npos )
					{
						/* Get charset and mimetype */
						try
						{
							os::String cSearch = cValue.substr( nEnd + 1 );
							os::RegExp cExp;
							cExp.Compile( "charset[ ]*=[ ]*", true );
							if( cExp.IsValid() && cExp.Search( cSearch ) )
							{
								cCharset = cSearch.substr( cExp.GetEnd() );
							}
						} catch( ... )
						{
						}
						cValue = cValue.substr( 0, nEnd );
					}
					cMimeType = cValue;
				}
				else if( cKey.CompareNoCase( "Content-Length" ) == 0 )
				{
					nDataSize = atoi( cValue.c_str() );
				}
			}
		}

		DEBUG( "Location: %s Mime: %s Charset: %s Size: %i Response: %i\n", cLocation.c_str(), cMimeType.c_str(), cCharset.c_str(), nDataSize, nResponseCode );

		//DEBUG("Pass response!\n");
		/* Pass response */
		ResourceResponse cResponse( m_cURL, cMimeType, nDataSize, cCharset, "" );
		cResponse.setHTTPStatusCode( nResponseCode );
		
		/* Add all header fields */
		for( uint i = 0; i < m_cMetaData.size(); i++ )
		{
			/* Check meta data in the format Key:Value */
			size_t nIndex = m_cMetaData[i].find( ":" );

			if( nIndex != std::string::npos )
			{
				os::String cKey( m_cMetaData[i], 0, nIndex );
				os::String cValue( m_cMetaData[i], nIndex + 1, -1 );

				cKey.Strip();
				cValue.Strip();

				cResponse.setHTTPHeaderField( cKey.c_str(  ), cValue.c_str(  ) );
			}
		}
		
		/* Check response code */
		if( !cLocation.empty() && nResponseCode > 299 && nResponseCode < 400  )
		{
			ResourceRequest* pcRequest = &m_pcHandle->getInternal()->m_request;
			KURL cURL( pcRequest->url(), DeprecatedString( cLocation.c_str() ) );
			//DEBUG( "Redirect to %s... (%i %x) from %s\n", cURL.string().ascii(), m_pcHandle->refCount(), (uint)m_pcHandle, pcRequest->url().string().ascii() );
			
			
			/* TODO: Is this correct? */
			pcRequest->setHTTPReferrer( pcRequest->url().string() );
			pcRequest->setURL( cURL );
//			pcRequest->setHTTPBody( NULL );
//			pcRequest->setHTTPMethod( "GET" );
			
			/* Tell the client that we have modified the request */
//			m_pcHandle->ref();
			GetClient()->willSendRequest( m_pcHandle, m_pcHandle->getInternal()->m_request, cResponse );

			/* Create new request */
			ResourceHandleManager::self()->add( m_pcHandle );

			/* Quit */
			m_bMetaDataPassed = true;
			m_bDataLoaded = true;
			return( false );
		}
		GetClient()->didReceiveResponse( m_pcHandle, cResponse );
		m_bMetaDataPassed = true;
		
		
		return ( true );
	}

	if( !m_bDataLoaded && !m_pcHandle->getInternal()->m_progressive )
		return ( true );
	
	/* Pass data */
	char anBuffer[65536];
	m_cDataLock.Lock();
	size_t nSize = m_cDataBuffer.Read( anBuffer, 65536 );
	m_cDataLock.Unlock();

	DEBUG("%i bytes\n", nSize );
	if( nSize > 0 )
	{
		GetClient()->didReceiveData( m_pcHandle, anBuffer, nSize, nSize );
	}
	if( m_cDataBuffer.Size() == 0 && m_bDataLoaded )
	{
		ReportSuccess();
		return ( false );
	}

	return ( true );
}

bool SyllableJob::Process()
{
	bool bReturn;
	if( m_bCancel )
	{
		DEBUG( "Error: Tried to process canceled job!\n" );
		return( false );
	}
	if( !m_bIsHTTP )
		bReturn = ProcessFile();
	else
		bReturn = ProcessHTTP();
	return( bReturn );
}

class CURLDataGate
{
public:
	CURLDataGate( const char* pzName )
	{
		m_hSema = create_semaphore( pzName, 100000000, SEM_RECURSIVE );
		m_bShared = false;
	}
	int Lock() const       { return( lock_semaphore( m_hSema ) ); }
	int Unlock() const     { return( unlock_semaphore( m_hSema ) ); }

	int Close() const     { return( lock_semaphore_x( m_hSema, 100000000, 0, INFINITE_TIMEOUT ) ); }
	int Open() const      { return( unlock_semaphore_x( m_hSema, 100000000, 0 ) ); }
  	bool IsLocked() const { return( !( get_semaphore_count( m_hSema ) == 100000000 ) ); }
  	
  
	bool m_bShared; 
	sem_id m_hSema;
};

/* CURL locking and callback code */
CURLSH* g_pcShareHandle;
os::String g_cCookieFile;
CURLDataGate g_cShareLock( "curl_share_lock" );
CURLDataGate g_cCookieLock( "curl_cookie_lock" );
CURLDataGate g_cDNSLock( "curl_dns_lock" );
CURLDataGate g_cSSLLock( "curl_ssl_lock" );
CURLDataGate g_cConnectLock( "curl_connect_lock" );
os::Locker** g_apcSSLLocks;

CURLDataGate* getLockGate( curl_lock_data eData )
{
	CURLDataGate* pcGate;
	switch( eData )
	{
		case CURL_LOCK_DATA_SHARE:
			pcGate = &g_cShareLock;
		break;
		case CURL_LOCK_DATA_COOKIE:
			pcGate = &g_cCookieLock;
		break;
		case CURL_LOCK_DATA_DNS:
			pcGate = &g_cDNSLock;
		break;
		case CURL_LOCK_DATA_SSL_SESSION:
			pcGate = &g_cSSLLock;
		break;		
		case CURL_LOCK_DATA_CONNECT:
			pcGate = &g_cConnectLock;
		break;
		default:
			DEBUG( "Error: getLockGate() called with invalid lock\n" );
			return( NULL );
		break;
	}
	return( pcGate );
}


void lockCallback( CURL* pcHandle, curl_lock_data eData, curl_lock_access eAccess, void* pPtr )
{
	CURLDataGate* pcGate = getLockGate( eData );
	DEBUG( "lockCallback %i %i\n", eData, eAccess );
	

	if( pcGate == NULL )
		return;
	
	if( eAccess == CURL_LOCK_ACCESS_NONE || eAccess == CURL_LOCK_ACCESS_SINGLE )
	{
		pcGate->Close();
		pcGate->m_bShared = false;
	}
	else
	{
		pcGate->Lock();
		pcGate->m_bShared = true;
	}
}

void unlockCallback( CURL* pcHandle, curl_lock_data eData, void* pPtr )
{
	CURLDataGate* pcGate = getLockGate( eData );
	DEBUG( "unlockCallback %i\n", eData );
	
	if( pcGate == NULL )
		return;
	
	if( !pcGate->IsLocked() )
	{
		DEBUG( "Error: unlockCallback() called but gate not locked!\n" );
		return;
	}
	if( pcGate->m_bShared )
		pcGate->Unlock();
	else
		pcGate->Open();
}

static size_t writeCallback(void* ptr, size_t size, size_t nmemb, void* obj)
{
	SyllableJob* pcJob = (SyllableJob*)obj;
	if( pcJob->m_bCancel )
		return( -1 );
	int totalSize = size * nmemb;
	if( totalSize <= 0 || ptr == NULL )
		return totalSize;

	DEBUG( "Received %i bytes\n", totalSize );
	pcJob->m_bMetaDataRead = true;
	pcJob->m_cDataLock.Lock();
	pcJob->m_cDataBuffer.Write( ptr, totalSize );
	pcJob->m_cDataLock.Unlock();
	
	return totalSize;
}

static size_t headerCallback(void* ptr, size_t size, size_t nmemb, void* obj)
{
	SyllableJob* pcJob = (SyllableJob*)obj;
	if( pcJob->m_bCancel )
		return( -1 );
		

	int totalSize = size * nmemb;
	if( totalSize <= 0 || ptr == NULL )
		return totalSize;
	DEBUG( "Received %i header bytes\n", totalSize );	
	pcJob->m_cDataLock.Lock();
	pcJob->m_cMetaData.push_back( os::String( (const char*)ptr, totalSize ) );
	pcJob->m_cDataLock.Unlock();

	return totalSize;
}

int progressCallback(void *obj,
                    double dltotal,
                    double dlnow,
                    double ultotal,
                    double ulnow)
{
	SyllableJob* pcJob = (SyllableJob*)obj;
	if( pcJob->m_bCancel )
		return( -1 );
	return( 0 );
}

static void SSLLockCallback( int nMode, int nLock, const char* pzFile, int nLine )
{
	DEBUG( "SSL lock %i %i\n", nLock, nMode & CRYPTO_LOCK );
	if( nMode & CRYPTO_LOCK )
		g_apcSSLLocks[nLock]->Lock();
	else
		g_apcSSLLocks[nLock]->Unlock();
}


static unsigned long SSLIdCallback( void )
{
	return( get_thread_id( NULL ) );
}

bool SyllableJob::LoadHTTP( CURL* pcCurlHandle, CURLM* pcMulti )
{
	char nErrorBuffer[CURL_ERROR_SIZE];
	DEBUG( "LoadHTTP() %s by thread %i...\n", os::String( m_cURL.string() ).c_str(), m_hProcessingThread );

	// This might not be the best place for this: remove an anchor from the URL if there is one
	if( m_cURL.hasRef() )
		m_cURL.setRef("");

	os::String cURL = os::String( m_cURL.string() );
	os::String cProxy = ProxyManager::getProxy("http").utf8().data();

	curl_easy_reset( pcCurlHandle );
#if !defined(NDEBUG)
	curl_easy_setopt( pcCurlHandle , CURLOPT_VERBOSE, 1 );
#endif
	curl_easy_setopt( pcCurlHandle , CURLOPT_NOSIGNAL, 1 );
	if( cProxy != "" )
		curl_easy_setopt( pcCurlHandle , CURLOPT_PROXY, cProxy.c_str() );
	curl_easy_setopt( pcCurlHandle , CURLOPT_SHARE, g_pcShareHandle );
	curl_easy_setopt( pcCurlHandle , CURLOPT_PRIVATE, this );
    curl_easy_setopt( pcCurlHandle , CURLOPT_ERRORBUFFER, nErrorBuffer );
    curl_easy_setopt( pcCurlHandle , CURLOPT_WRITEFUNCTION, writeCallback );
    curl_easy_setopt( pcCurlHandle , CURLOPT_WRITEDATA, this );
    curl_easy_setopt( pcCurlHandle , CURLOPT_HEADERFUNCTION, headerCallback );
    curl_easy_setopt( pcCurlHandle , CURLOPT_WRITEHEADER, this );
    curl_easy_setopt( pcCurlHandle , CURLOPT_PROGRESSFUNCTION, progressCallback );
    curl_easy_setopt( pcCurlHandle , CURLOPT_PROGRESSDATA, this );
    curl_easy_setopt( pcCurlHandle , CURLOPT_NOPROGRESS, 0 );
    curl_easy_setopt( pcCurlHandle , CURLOPT_HTTPAUTH, CURLAUTH_BASIC );
	curl_easy_setopt( pcCurlHandle , CURLOPT_SSL_VERIFYPEER, 0 );
	curl_easy_setopt( pcCurlHandle , CURLOPT_CONNECTTIMEOUT, 30 );
    curl_easy_setopt( pcCurlHandle , CURLOPT_URL, cURL.c_str() );
    
	if( m_bCancel )
		return( false );
	
	/* Create postdata string */
	ResourceRequest cRequest = m_pcHandle->getInternal()->m_request;

	DeprecatedString cPostData;

	if( cRequest.httpBody() && !cRequest.httpBody(  )->isEmpty(  ) )
	{
		cPostData = cRequest.httpBody()->flattenToString(  ).deprecatedString(  );
		DEBUG( "Post data %s %i\n", cPostData.ascii(), cPostData.length(  ) );
	}

	/* Build http header */

	/* Add our header flags */
	cRequest.setHTTPHeaderField( "User-Agent", "Mozilla/5.0 (compatible; U; WebView 0.1; Pyro) AppleWebKit/420+ (KHTML, like Gecko)" );
#if 0
	/* Add username */
	if( !m_pcHandle->url().user().isEmpty() )
	{

		os::String auth( m_pcHandle->url().user() );
		auth += ":";
		auth += m_pcHandle->url().pass();
		String cAuthResult( "Basic " );
		cAuthResult += base64Encode( auth, false );
		cRequest.setHTTPHeaderField( "Authorization", cAuthResult );
	}
#endif	
	/* Add cookies */
	if( cRequest.allowHTTPCookies() )
	{	
		curl_easy_setopt( pcCurlHandle, CURLOPT_COOKIEFILE, g_cCookieFile.c_str() );
		curl_easy_setopt( pcCurlHandle, CURLOPT_COOKIEJAR, g_cCookieFile.c_str() );
	}
	
//	if( cRequest.httpReferrer().isEmpty() && !m_pcHandle->getInternal()->m_referrer.empty() )
//		cRequest.setHTTPReferrer( m_pcHandle->getInternal()->m_referrer );

	/* Prepare header */
	curl_slist* psHeaderList = NULL;

	HTTPHeaderMap::const_iterator end = cRequest.httpHeaderFields().end(  );

	for( HTTPHeaderMap::const_iterator it = cRequest.httpHeaderFields().begin(  ); it != end; ++it )
	{
		psHeaderList = curl_slist_append( psHeaderList, os::String( os::String ( it->first ) + ": " + os::String ( it->second ) ).c_str() );
	
	}
	
	/* Add post data */
	if( !cPostData.isEmpty() )
	{
		os::String cHeader( "Content-Type: application/x-www-form-urlencoded" );
		//psHeaderList = curl_slist_append( psHeaderList, cHeader.c_str() );
		curl_easy_setopt( pcCurlHandle, CURLOPT_POSTFIELDS, cPostData.ascii() );
		curl_easy_setopt( pcCurlHandle, CURLOPT_POSTFIELDSIZE, cPostData.length() );
	}
	
	curl_easy_setopt( pcCurlHandle, CURLOPT_HTTPHEADER, psHeaderList );
	if( m_bCancel )
	{
		curl_slist_free_all( psHeaderList );		
		return( false );
	}
		
	/* Add handle */
	CURLMcode nRes = curl_multi_add_handle( pcMulti, pcCurlHandle );
	if( nRes != CURLM_CALL_MULTI_PERFORM && nRes != CURLM_OK )
	{
		curl_slist_free_all( psHeaderList );		
		DEBUG( "Error: Failed to add CURL handle!\n" );
		return( false );
	}
	
	CURLcode nResult = CURL_LAST;
	
	/* Perform */
	while( 1 )
	{
		/* Get the used sockets */
	    fd_set fdread;
	    FD_ZERO(&fdread);
    	fd_set fdwrite;
	    FD_ZERO(&fdwrite);
    	fd_set fdexcep;
	    FD_ZERO(&fdexcep);
    	int maxfd = 0;
    	int nRunningHandles;

	    curl_multi_fdset( pcMulti, &fdread, &fdwrite, &fdexcep, &maxfd);
    	
    	
    	/* Wait for data */
        struct timeval timeout;
	    timeout.tv_sec = 0;
    	timeout.tv_usec = 50000;       // select waits microseconds

	    int rc = ::select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
		
		/* Perform */
		CURLMcode nCode = CURLM_CALL_MULTI_PERFORM;
		while( nCode == CURLM_CALL_MULTI_PERFORM )
		{
			if( m_bCancel )
			{
				curl_multi_remove_handle( pcMulti, pcCurlHandle );			
				curl_slist_free_all( psHeaderList );
				return( false );			
			}
			nCode = curl_multi_perform( pcMulti, &nRunningHandles );
		}	
		
		/* Check messages */
		while( true )
		{
			int nMessages;
        	CURLMsg* pcMsg = curl_multi_info_read( pcMulti, &nMessages );
        	if( pcMsg == NULL )
        		break;
        		
        	if( pcMsg->msg == CURLMSG_DONE )
			{
				nResult = pcMsg->data.result;
				break;
			}        	
		}
		
		if( nResult != CURL_LAST )
			break;
		
		if( nRunningHandles == 0 )
		{
			nResult = CURLE_OK;
			break;
		}
	}

#if 0
	/* Perform */
	CURLcode nResult = curl_easy_perform( pcCurlHandle );
#endif	
	DEBUG( "CURL request returned %i\n", nResult );
	
	curl_multi_remove_handle( pcMulti, pcCurlHandle );
	
	curl_slist_free_all( psHeaderList );
	
	if( nResult != CURLE_OK )
	{
		m_bError = true;
		return( false );
	}
	m_bMetaDataRead = true;
	m_bDataLoaded = true;
	DEBUG( "%i Finished!\n", get_thread_id( NULL ) );
	return( false );
}

void SyllableJob::ReportFailure()
{
	m_bError = true;
	ResourceError error( "", -1, m_cURL.string(  ), m_cURL.string(  ) );

	GetClient()->didFail( m_pcHandle, error );
}

void SyllableJob::ReportSuccess()
{
	DEBUG( "%s finished!\n", os::String ( m_cURL.string(  ) ).c_str(  ) );

	GetClient()->didFinishLoading( m_pcHandle );
}


/* HTTP worker thread */
SyllableHTTPWorker::SyllableHTTPWorker():os::Thread( "webcore_http_worker" )
{
	DEBUG( "Created new HTTP worker thread\n" );
}


int32 SyllableHTTPWorker::Run()
{
	DEBUG( "HTTP worker thread %i running\n", get_thread_id( NULL ) );
	
	CURL* pcHandle = curl_easy_init();
	CURLM* pcMulti = curl_multi_init();
	
	while( 1 )
	{
		/* Get next job */
		g_cGlobalMutex.Lock();
		SyllableJob* pcJob = g_pcLoader->GetNextHTTPJob( get_thread_id( NULL ) );
		if( pcJob == NULL )
		{
			/* Exit thread */
			DEBUG( "HTTP worker thread %i exit\n", get_thread_id( NULL ) );
			curl_easy_cleanup( pcHandle );
			curl_multi_cleanup( pcMulti );
			g_pcLoader->WorkerFinished();
			g_cGlobalMutex.Unlock();
			return( 0 );
		}
		g_cGlobalMutex.Unlock();
		/* Load */
		pcJob->LoadHTTP( pcHandle, pcMulti );
		pcJob->m_hProcessingThread = -1;
	}
	
}

#define MAX_WORKERS 4

/* Loader */
SyllableLoader::SyllableLoader():os::Thread( "webcore_loader" )
{
	m_nRunningWorkers = 0;
	m_bQuit = false;
	curl_global_init(CURL_GLOBAL_ALL);
	const char *pzHome = getenv( "HOME" );
	g_cCookieFile = os::String( pzHome ) + ( "/Settings/Webster/CURLCookies" );
	/* Create CURL share handle */
	g_pcShareHandle = curl_share_init();
	curl_share_setopt( g_pcShareHandle, CURLSHOPT_LOCKFUNC, lockCallback );
	curl_share_setopt( g_pcShareHandle, CURLSHOPT_UNLOCKFUNC, unlockCallback );
	if( curl_share_setopt( g_pcShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE ) != CURLSHE_OK )
		DEBUG( "Error: Could not setup shared cookies\n" );
	if( curl_share_setopt( g_pcShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS ) != CURLSHE_OK )
		DEBUG( "Error: Could not setup shared dns entries\n" );
	/* Create SSL locks */
	DEBUG( "Creating %i SSL locks\n", CRYPTO_num_locks() );
	g_apcSSLLocks = (os::Locker**)malloc( sizeof( os::Locker* ) * CRYPTO_num_locks() );
	for( int i = 0; i < CRYPTO_num_locks(); i++ )
		g_apcSSLLocks[i] = new os::Locker( "ssl_lock" );
	CRYPTO_set_id_callback( SSLIdCallback );
	CRYPTO_set_locking_callback( SSLLockCallback );
}

SyllableLoader::~SyllableLoader()
{
	CRYPTO_set_id_callback( NULL );
	CRYPTO_set_locking_callback( NULL );
	for( int i = 0; i < CRYPTO_num_locks(); i++ )
		delete( g_apcSSLLocks[i] );
	curl_share_cleanup( g_pcShareHandle );
	curl_global_cleanup();
	
}

void SyllableLoader::AddJob( ResourceHandle * pcHandle )
{
	if( pcHandle == NULL || pcHandle->getInternal() == NULL )
	{
		DEBUG( "Error: SyllableLoader::AddJob() Tried to add empty job!\n" );
		return;
	}
	
	/* Add job */
	g_cGlobalMutex.Lock();
	SyllableJob *pcJob = new SyllableJob( pcHandle );

	m_cJobs.push_back( pcJob );
	
	
	/* Create a http worker thread if necessary */
	if( pcJob->IsHTTP()	&& m_nRunningWorkers < MAX_WORKERS )
	{
		m_nRunningWorkers++;
		SyllableHTTPWorker* pcWorker = new SyllableHTTPWorker();
		pcWorker->Start();
	}
			
	g_cGlobalMutex.Unlock();
}

void SyllableLoader::CancelJob( ResourceHandle * pcHandle )
{
again:
	g_cGlobalMutex.Lock();
	for( uint i = 0; i < m_cJobs.size(); i++ )
	{
		if( m_cJobs[i]->m_pcHandle == pcHandle )
		{
			DEBUG( "Cancel loading of %s!\n", os::String ( m_cJobs[i]->m_cURL.string(  ) ).c_str(  ) );

			/* Wait for the http thread */
			SyllableJob *pcJob = m_cJobs[i];
			pcJob->m_bCancel = true;
			if( pcJob->m_hProcessingThread != -1 )
			{
				/* TODO: Signal the thread to quit */
				DEBUG( "Wait for http thread to finish...\n" );

				/* The Run() method might delete the job while we are sleeping */
				g_cGlobalMutex.Unlock();
				snooze( 50000 );
				goto again;
			}
			/* If the job is currently running we let the Run() method delete it */
			if( !pcJob->m_bIsRunning )
			{
				m_cJobs.erase( m_cJobs.begin() + i );
				delete( pcJob );
				DEBUG( "Job canceled\n" );
			} else {
				DEBUG( "Cancel of loading delayed\n" );
			}
			g_cGlobalMutex.Unlock();
			return;
		}
	}
	DEBUG("Job to cancel not found!\n");
	g_cGlobalMutex.Unlock();
}

int32 SyllableLoader::Run()
{
	DEBUG( "Loader running!\n" );
	while( !m_bQuit )
	{
		g_cGlobalMutex.Lock();
		/* Go through the job list */
		for( uint i = 0; i < m_cJobs.size(); i++ )
		{
			
			SyllableJob *pcJob = m_cJobs[i];
			
			pcJob->m_bIsRunning = true;

			if( pcJob->m_bCancel || pcJob->Process() == false )
			{
				/* Do not delete the job if it is still processed by the http loader */
				if( pcJob->m_hProcessingThread != -1 )
				{
					DEBUG("SyllableLoader::Run(): Job still processed\n" );
					pcJob->m_bCancel = true;
					pcJob->m_bIsRunning = false;
					continue;
				}
				
				/* Remove it from the list */
				bool bFound = false;
				pcJob->m_bIsRunning = false;
				for( uint j = 0; j < m_cJobs.size(); j++ )
				{
					if( m_cJobs[j] == pcJob ) {
						m_cJobs.erase( m_cJobs.begin() + j );
						bFound = true;
						break;
					}
				}
				if( !bFound && !pcJob->m_bCancel )
					DEBUG( "Error: Could not find processed job in joblist\n" );
				
				delete( pcJob );
				goto end;
			}
			
			pcJob->m_bIsRunning = false;
		}
	      end:
		g_cGlobalMutex.Unlock();
		if( m_cJobs.size() == 0 )
			snooze( 50000 );
		else
			snooze( 10000 );
	}
	DEBUG("Loader quit!\n");
	return ( 0 );
}

SyllableJob* SyllableLoader::GetNextHTTPJob( thread_id hThread )
{
	g_cGlobalMutex.Lock();
	/* Go through the job list */
	for( uint i = 0; i < m_cJobs.size(); i++ )
	{
		if( m_cJobs[i]->IsHTTP() && m_cJobs[i]->m_hProcessingThread == -1 && !m_cJobs[i]->m_bDataLoaded
			&& !m_cJobs[i]->m_bError && !m_cJobs[i]->m_bCancel )
		{
			SyllableJob* pcJob = m_cJobs[i];
			pcJob->m_hProcessingThread = hThread;
			g_cGlobalMutex.Unlock();
			return( pcJob );
		}
	}
	g_cGlobalMutex.Unlock();
	return( NULL );
}

void SyllableLoader::WorkerFinished()
{
	DEBUG("WorkerFinished\n");
	m_nRunningWorkers--;
	ASSERT( m_nRunningWorkers >= 0 );
}


static ResourceHandleManager *s_self = 0;

ResourceHandleManager *ResourceHandleManager::self()
{
	if( !s_self )
		s_self = new ResourceHandleManager();

	return s_self;
}


ResourceHandleManager::ResourceHandleManager()
{
	g_pcLoader = new SyllableLoader();
	g_pcLoader->Start();
}

ResourceHandleManager::~ResourceHandleManager()
{
	DEBUG( "ResourceHandleManager::~ResourceHandleManager()\n" );
	g_pcLoader->Quit();
	g_pcLoader->WaitFor();
	delete( g_pcLoader );
}



void ResourceHandleManager::add( ResourceHandle * resource )
{
/* XXXKV */
#if 0
	ResourceHandleInternal *d = resource->getInternal();

	DEBUG( "ResourceHandleManager::add %s\n", d->m_request.url().string().utf8().data() );

	if( resource->method() == "POST" )
	{
		ASSERT( resource->postData() );
		DeprecatedString postData = resource->postData()->flattenToString(  ).deprecatedString(  );

		DEBUG( "POST DATA %s\n", os::String ( postData ).c_str() );
	}
#endif
	g_pcLoader->AddJob( resource );
}

void ResourceHandleManager::cancel( ResourceHandle * job )
{
	g_pcLoader->CancelJob( job );
}

String ProxyManager::m_cHTTPProxy = "";
String ProxyManager::m_cHTTPSProxy = "";
String ProxyManager::m_cFTPProxy = "";

void ProxyManager::setProxy(const String& cURL, const String& cProto)
{
	if (cProto == "http")
		m_cHTTPProxy = cURL;
	else if (cProto == "https")
		m_cHTTPSProxy = cURL;
	else if (cProto == "ftp")
		m_cFTPProxy = cURL;
}

const String& ProxyManager::getProxy(const String& cProto)
{
	if (cProto == "http")
		return m_cHTTPProxy;
	else if (cProto == "https")
		return m_cHTTPSProxy;
	else if (cProto == "ftp")
		return m_cFTPProxy;
}

