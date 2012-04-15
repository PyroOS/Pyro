/*  libwebview.so - Web rendering library for Syllable
 *  Copyright (C) 2000 Kurt Skauen
 *  Copyright (C) 2007, 2008 Syllable Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 *  MA 02111-1307, USA
 */

#ifndef __F_WEBVIEW_DOWNLOADDIALOG_H__
#define __F_WEBVIEW_DOWNLOADDIALOG_H__

#include <gui/window.h>
#include <gui/layoutview.h>

#include <util/message.h>
#include <util/messenger.h>
#include <ResourceHandleClient.h>
#include <storage/tempfile.h>

namespace os
{
    class Button;
    class StringView;
    class ProgressBar;
    class FileRequester;
}

namespace WebCore
{
	class ResourceHandle;
}

class StatusView;

class DownloadRequesterView : public os::LayoutView
{
public:
    DownloadRequesterView( const os::Rect&      cFrame,
			   const std::string&   cURL,
			   const std::string&   cPreferredName,
			   const std::string&   cType,
			   off_t	        nContentSize,
			   const os::Messenger& cMsgTarget,
			   const os::Message&   cOkMsg,
			   const os::Message&   cCancelMessage);
    ~DownloadRequesterView();

    virtual void HandleMessage( os::Message* pcMessage );
    virtual void AllAttached();
    
private:
    static std::string s_cDownloadPath;

    os::Button*	m_pcOkBut;
    os::Button* m_pcCancelBut;

    os::Messenger m_cMsgTarget;
    os::Message	  m_cOkMsg;
    os::Message   m_cCancelMsg;
    os::Message*  m_pcTermMsg;
    
    std::string	       m_cPreferredName;
    os::FileRequester* m_pcFileReq;
};


class DownloadRequester : public os::Window
{
public:
    DownloadRequester( const os::Rect&      cFrame,
		       const std::string&   cTitle,
		       const std::string&   cURL,
		       const std::string&   cPreferredName,
		       const std::string&   cType,
		       off_t		    nContentSize,
		       const os::Messenger& cMsgTarget,
		       const os::Message&   cOkMsg,
		       const os::Message&   cCancelMessage );
    ~DownloadRequester();
private:
    DownloadRequesterView* m_pcTopView;
};


class DownloadProgressView : public os::LayoutView
{
public:
    DownloadProgressView( const os::Rect& cFrame,
			  const std::string& cURL,
			  const std::string& cPath,
			  off_t nTotalSize,
			  bigtime_t nStartTime,
			  const os::Messenger& cMsgTarget,
			  const os::Message& cCancelMessage );
    ~DownloadProgressView();
    virtual void HandleMessage( os::Message* pcMessage );
    virtual void AllAttached();
    
    void UpdateProgress( off_t nBytesReceived );

    void ClearTermMsg() { m_pcTermMsg = NULL; }
private:
    os::ProgressBar* m_pcProgressBar;
    StatusView*	     m_pcStatusView;
    os::Button*	     m_pcCancelBut;

    
    bigtime_t	  m_nStartTime;
    off_t 	  m_nTotalSize;
    
    os::Messenger m_cMsgTarget;
    os::Message   m_cCancelMsg;
    os::Message*  m_pcTermMsg;
};


class DownloadProgress : public os::Window
{
public:
    DownloadProgress( const os::Rect& cFrame,
		      const std::string& cTitle,
		      const std::string& cURL,
		      const std::string& cPath,
		      off_t nTotalSize,
		      bigtime_t nStartTime,
		      const os::Messenger& cMsgTarget,
		      const os::Message& cCancelMessage );
    ~DownloadProgress();

    void UpdateProgress( off_t nBytesReceived );
    void Terminate();
private:
    DownloadProgressView* m_pcTopView;
    std::string		  m_cTitle;
    off_t		  m_nTotalSize;
};

class DownloadNode
{
	public:
	DownloadNode()
	{
		m_pcRequester = NULL;
		m_pcProgressDlg = NULL;
		m_nContentSize = -1;
		m_pcTmpFile = NULL;
		m_pcFile = NULL;
		m_nBytesReceived = 0;
		m_bDone = false;
		m_bCanceled = false;
		m_bStarted = false;
	}

	bool m_bStarted;
	WebCore::ResourceHandle* m_pcJob;
	DownloadRequester *m_pcRequester;
	DownloadProgress *m_pcProgressDlg;

	os::String m_cURL;
	os::String m_cMimeType;
	os::String m_cPreferredName;
	off_t m_nContentSize;
	bigtime_t m_nStartTime;

	os::String m_cPath;
	os::TempFile * m_pcTmpFile;
	os::File * m_pcFile;
	int m_nBytesReceived;
	bool m_bDone;
	bool m_bCanceled;
};


class DownloadHandler : public WebCore::ResourceHandleClient, os::Looper
{
public:
	DownloadHandler();
	~DownloadHandler();
	void Start( WebCore::ResourceHandle* job );
	void didReceiveResponse( WebCore::ResourceHandle* job, const WebCore::ResourceResponse& response );
	void didReceiveData( WebCore::ResourceHandle* job, const char* data, int, int length );
	void didFinishLoading( WebCore::ResourceHandle* );
	void didFail(WebCore::ResourceHandle*, const WebCore::ResourceError&);
	void HandleMessage( os::Message* );
private:
	void Finish();
	DownloadNode m_cNode;
};


#endif

