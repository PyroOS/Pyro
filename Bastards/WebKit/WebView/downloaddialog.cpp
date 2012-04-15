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

#include <webview/downloaddialog.h>

#include <ResourceRequest.h>
#include <ResourceHandle.h>
#include <ResourceHandleInternal.h>
#include <ResourceHandleClient.h>
#include <ResourceResponse.h>
#include <CString.h>
#include <SyllableDebug.h>

#include <pyro/time.h>

#include <gui/button.h>
#include <gui/stringview.h>
#include <gui/progressbar.h>
#include <gui/filerequester.h>
#include <gui/font.h>
#include <gui/requesters.h>
#include <util/message.h>
#include <storage/path.h>

#include <vector>
#include <unistd.h>

using namespace os;

enum {
	ID_OK = 1,
	ID_CANCEL,
	ID_START_DOWNLOAD,
	ID_CANCEL_DOWNLOAD
};

std::string DownloadRequesterView::s_cDownloadPath;

static String get_size_str( off_t nSize )
{
    if ( nSize < 1024 ) {
	return( String().Format( "%Ld", nSize ) );
    } else if ( nSize < 1024LL * 1024LL ) {
	return( String().Format( "%.2fKB", double(nSize) / 1024.0 ) );
    } else {
	return( String().Format( "%.2fMB", double(nSize) / (1024.0*1024.0) ) );
    }
}


class StatusView : public View
{
public:
    StatusView( const Rect& cFrame, const std::string& cTitle );

    int  AddLabel( const std::string& cLabel, const std::string& cText );
    void SetText( int nIndex, const std::string& cText );

    virtual void  Paint( const Rect& cUpdateRect );
    virtual Point GetPreferredSize( bool bLargest ) const;
    
private:
    float 	m_vLabelWidth;
    font_height m_sFontHeight;
    float	m_vGlyphHeight;
    std::vector<std::pair<std::string,std::string> > m_cStrList;
};

StatusView::StatusView( const Rect& cFrame, const std::string& cTitle ) : View( cFrame, cTitle )
{
    GetFontHeight( &m_sFontHeight );
    m_vGlyphHeight = ceil( (m_sFontHeight.ascender + m_sFontHeight.descender + m_sFontHeight.line_gap ) * 1.2f );
    m_vLabelWidth = 0.0f;
}

int StatusView::AddLabel( const std::string& cLabel, const std::string& cText )
{
    m_cStrList.push_back( std::make_pair( cLabel, cText ) );
    float vLabelWidth = GetStringWidth( cLabel );
    if ( vLabelWidth > m_vLabelWidth ) {
	m_vLabelWidth = vLabelWidth;
    }
    return( m_cStrList.size() - 1 );
}

void StatusView::SetText( int nIndex, const std::string& cText )
{
    m_cStrList[nIndex].second = cText;
    Invalidate( Rect( m_vLabelWidth, float(nIndex) * m_vGlyphHeight, COORD_MAX, float(nIndex+1) * m_vGlyphHeight ) );
    Flush();
}

Point StatusView::GetPreferredSize( bool bLargest ) const
{
    if ( bLargest ) {
	return( Point( COORD_MAX, m_vGlyphHeight * float(m_cStrList.size()) ) );
    } else {
	return( Point( m_vLabelWidth, m_vGlyphHeight * float(m_cStrList.size()) ) );
    }
}

void StatusView::Paint( const Rect& /*cUpdateRect*/ )
{
    float y = ceil( m_sFontHeight.ascender );

    FillRect( GetBounds(), get_default_color( COL_NORMAL ) );
    SetFgColor( 0, 0, 0 );
    for ( uint i = 0 ; i < m_cStrList.size() ; ++i ) {
	DrawString( Point( 0.0f, y ), m_cStrList[i].first  );
	DrawString( Point( m_vLabelWidth + m_vGlyphHeight, y ), m_cStrList[i].second );
	y += m_vGlyphHeight;
    }
}


DownloadRequesterView::DownloadRequesterView( const Rect&          cFrame,
					      const std::string&   cURL,
					      const std::string&   cPreferredName,
					      const std::string&   cType,
					      off_t		   nContentSize,
					      const os::Messenger& cMsgTarget,
					      const os::Message&   cOkMsg,
					      const os::Message&   cCancelMessage ) :
	LayoutView( cFrame, "download_diag", NULL, CF_FOLLOW_ALL ),
	m_cMsgTarget( cMsgTarget ), m_cOkMsg( cOkMsg ), m_cCancelMsg( cCancelMessage ), m_cPreferredName(cPreferredName)
{
    VLayoutNode* pcRoot = new VLayoutNode( "root" );

    m_pcTermMsg = &m_cCancelMsg;
    
    m_pcOkBut = new Button( Rect(0,0,0,0), "ok", "Ok", new Message( ID_OK ) );
    m_pcCancelBut = new Button( Rect(0,0,0,0), "cancel", "Cancel", new Message( ID_CANCEL ) );
    
    HLayoutNode* pcButtonBar = new HLayoutNode( "button_bar" );

    StatusView* m_pcStatusView = new StatusView( Rect(), "status_view" );

    m_pcStatusView->AddLabel( "Location:", cURL );
    m_pcStatusView->AddLabel( "File type:", cType );
    if ( nContentSize != -1 ) {
	m_pcStatusView->AddLabel( "File size:", get_size_str( nContentSize ) );
    } else {
	m_pcStatusView->AddLabel( "File size:", "UNKNOWN" );
    }
    
    
    pcButtonBar->AddChild( new HLayoutSpacer( "space" ) );
    pcButtonBar->AddChild( m_pcOkBut )->SetBorders( Rect( 0, 0, 10, 10 ) );
    pcButtonBar->AddChild( m_pcCancelBut )->SetBorders( Rect( 0, 0, 10, 10 ) );
    
    pcRoot->AddChild( new VLayoutSpacer( "space" ) );
    pcRoot->AddChild( m_pcStatusView )->SetBorders( Rect( 10.0f, 5.0f, 5.0f, 5.0f ) );
    pcRoot->AddChild( new VLayoutSpacer( "space" ) );
    pcRoot->AddChild( pcButtonBar );

    pcRoot->SameWidth( "ok", "cancel", NULL );
    
    SetRoot( pcRoot );
}

DownloadRequesterView::~DownloadRequesterView()
{
    if ( m_pcTermMsg != NULL ) {
	m_cMsgTarget.SendMessage( m_pcTermMsg );
	DEBUG("Send TERM!\n");
    }
}

void DownloadRequesterView::AllAttached()
{
    m_pcOkBut->SetTarget( this );
    m_pcCancelBut->SetTarget( this );
}

void DownloadRequesterView::HandleMessage( os::Message* pcMessage )
{
    switch( pcMessage->GetCode() )
    {
	case ID_OK:
	{
	    if ( s_cDownloadPath.empty() ) {
		const char* pzHome = getenv( "HOME" );
		if ( pzHome != NULL ) {
		    s_cDownloadPath = pzHome;
		    if ( s_cDownloadPath[s_cDownloadPath.size()-1] != '/' ) {
			s_cDownloadPath += '/';
		    }
		} else {
		    s_cDownloadPath = "/boot/System/temp/";
		}
	    }
	    m_pcFileReq = new FileRequester( FileRequester::SAVE_REQ,
					     new Messenger( this ),
					     (s_cDownloadPath + m_cPreferredName).c_str(),
					     FileRequester::NODE_FILE,
					     false );
	    m_pcFileReq->Show();
	    m_pcFileReq->MakeFocus();
	    GetWindow()->Hide();
	    break;
	}
	case ID_CANCEL:
	    m_pcTermMsg = &m_cCancelMsg;
//	    m_cMsgTarget.SendMessage( &m_cCancelMsg );
	    GetWindow()->PostMessage( M_QUIT );
	    break;
	case M_SAVE_REQUESTED:
	{
	    if ( m_pcFileReq != NULL ) {
		s_cDownloadPath = m_pcFileReq->GetPath();
		if ( s_cDownloadPath[s_cDownloadPath.size()-1] != '/' ) {
		    s_cDownloadPath += '/';
		}
		m_pcFileReq->PostMessage( M_QUIT );
		m_pcFileReq = NULL;
	    }
	    std::string cPath;
	    if ( pcMessage->FindString( "file/path", &cPath ) == 0 ) {
		m_cOkMsg.AddString( "path", cPath );
		m_pcTermMsg = &m_cOkMsg;
//		m_cMsgTarget.SendMessage( &m_cOkMsg );
	    } else {
		m_pcTermMsg = &m_cCancelMsg;
//		m_cMsgTarget.SendMessage( &m_cCancelMsg );
	    }
	    GetWindow()->PostMessage( M_QUIT );
	    break;
	}
	default:
	    View::HandleMessage( pcMessage );
	    break;
    }
}



DownloadRequester::DownloadRequester( const Rect& cFrame,
				      const std::string& cTitle,
				      const std::string& cURL,
				      const std::string& cPreferredName,
				      const std::string& cType,
				      off_t		 nContentSize,
				      const Messenger&   cMsgTarget,
				      const Message&     cOkMsg,
				      const Message&     cCancelMessage ) :
	Window( cFrame, "donwload_dialog", cTitle )
{
    m_pcTopView = new DownloadRequesterView( GetBounds(), cURL, cPreferredName, cType, nContentSize, cMsgTarget, cOkMsg, cCancelMessage );
    AddChild( m_pcTopView );
}

DownloadRequester::~DownloadRequester()
{
}


DownloadProgressView::DownloadProgressView( const Rect& cFrame,
					    const std::string& cURL,
					    const std::string& cPath,
					    off_t nTotalSize,
					    bigtime_t nStartTime,
					    const os::Messenger& cMsgTarget,
					    const os::Message&   cCancelMessage ) :
	LayoutView( cFrame, "download_progress_diag", NULL, CF_FOLLOW_ALL ),
	m_cMsgTarget( cMsgTarget ), m_cCancelMsg( cCancelMessage )
{
    m_nTotalSize = nTotalSize;
    m_nStartTime = nStartTime;
    m_pcTermMsg  = &m_cCancelMsg;
    
    VLayoutNode* pcRoot = new VLayoutNode( "root" );

    m_pcProgressBar = new ProgressBar( Rect(0,0,0,0), "progress_bar" );
    
    m_pcCancelBut = new Button( Rect(0,0,0,0), "cancel", "Cancel", new Message( ID_CANCEL ) );
    
    HLayoutNode* pcButtonBar    = new HLayoutNode( "button_bar" );

    m_pcStatusView = new StatusView( Rect(), "status_view" );

    m_pcStatusView->AddLabel( "Download to:", cPath );
    m_pcStatusView->AddLabel( "Progress:", (m_nTotalSize==-1) ? "0" : String().Format( "0/%s", get_size_str( m_nTotalSize ).c_str() ) );
    m_pcStatusView->AddLabel( "Transfer rate:", "" );
    
    
    pcButtonBar->AddChild( new HLayoutSpacer( "space" ) );
    pcButtonBar->AddChild( m_pcCancelBut )->SetBorders( Rect( 0, 0, 10, 10 ) );
    
    pcRoot->AddChild( new VLayoutSpacer( "space" ) );
    pcRoot->AddChild( new StringView( Rect(), "", "Saving:" ) );
    pcRoot->AddChild( new StringView( Rect(), "", cURL.c_str() ) )->SetBorders( Rect( 10.0f, 5.0f, 10.0f, 5.0f ) );
    pcRoot->AddChild( m_pcProgressBar )->SetBorders( Rect( 10.0f, 5.0f, 10.0f, 5.0f ) );
    pcRoot->AddChild( new VLayoutSpacer( "space" ) );
    pcRoot->AddChild( m_pcStatusView )->SetBorders( Rect( 10.0f, 5.0f, 5.0f, 5.0f ) );
    pcRoot->AddChild( new VLayoutSpacer( "space" ) );
    pcRoot->AddChild( pcButtonBar );
    
    SetRoot( pcRoot );
}

DownloadProgressView::~DownloadProgressView()
{
    if ( m_pcTermMsg != NULL ) {
	m_cMsgTarget.SendMessage( m_pcTermMsg );
    }
}

void DownloadProgressView::AllAttached()
{
    m_pcCancelBut->SetTarget( this );
}

void DownloadProgressView::HandleMessage( os::Message* pcMessage )
{
    switch( pcMessage->GetCode() )
    {
	case ID_CANCEL:
	    m_cMsgTarget.SendMessage( &m_cCancelMsg );
	    GetWindow()->PostMessage( M_QUIT );
	    break;
	default:
	    LayoutView::HandleMessage( pcMessage );
	    break;
    }
}

void DownloadProgressView::UpdateProgress( off_t nBytesReceived )
{
    double vBytesPerSec = double(nBytesReceived) / (double(get_system_time() - m_nStartTime) / 1000000.0);

    os::String cSpeedStr;
    if ( vBytesPerSec < 1024.0 ) {
	cSpeedStr.Format( "%.2f bytes/s", vBytesPerSec );
    } else {
	cSpeedStr.Format( "%.2f KB/s", vBytesPerSec / 1024.0 );
    }
    m_pcStatusView->SetText( 2, cSpeedStr );
    if ( m_nTotalSize != -1 ) {
	int nETA = int(double(m_nTotalSize - nBytesReceived) / vBytesPerSec);
	String cETA;
	if ( nETA < 60 ) {
	    cETA.Format( "%02d seconds", nETA );
	} else if ( nETA < 60*60 ) {
	    cETA.Format( "%d:%02d minutes", nETA/60, nETA % 60 );
	} else {
	    int nHours = nETA / (60*60);
	    nETA -= nHours * 60*60;
	    cETA.Format( "%d:%02d:%02d hours", nHours, nETA/60, nETA % 60 );
	}
	m_pcStatusView->SetText( 1, String().Format( "%s / %s (ETA: %s)",
						     get_size_str( nBytesReceived ).c_str(),
						     get_size_str( m_nTotalSize ).c_str(),
						     cETA.c_str() ) );
	m_pcProgressBar->SetProgress( double(nBytesReceived) / double(m_nTotalSize) );
    } else {
	m_pcStatusView->SetText( 1, get_size_str( nBytesReceived ).c_str() );
    }
    GetRoot()->Layout();
}


DownloadProgress::DownloadProgress( const Rect& cFrame,
				    const std::string& cTitle,
				    const std::string& cURL,
				    const std::string& cPath,
				    off_t nTotalSize,
				    bigtime_t nStartTime,
				    const Messenger& cMsgTarget,
				    const Message& cCancelMessage ) :
	Window( cFrame, "donwload_progress_dialog", cTitle ), m_cTitle(cTitle)
{
    m_nTotalSize = nTotalSize;
    m_pcTopView = new DownloadProgressView( GetBounds(), cURL, cPath, nTotalSize, nStartTime, cMsgTarget, cCancelMessage );
    AddChild( m_pcTopView );
}

DownloadProgress::~DownloadProgress()
{
}

void DownloadProgress::UpdateProgress( off_t nBytesReceived )
{
    Lock();
    m_pcTopView->UpdateProgress( nBytesReceived );
    SetTitle( String().Format( "(%Ld%%) - %s", nBytesReceived * 100 / m_nTotalSize, m_cTitle.c_str() ) );
    Unlock();
}

void DownloadProgress::Terminate()
{
    if ( SafeLock() >= 0 ) {
	m_pcTopView->ClearTermMsg();
	PostMessage( M_QUIT );
	Unlock();
    }
}

extern os::Locker g_cGlobalMutex;

DownloadHandler::DownloadHandler() : os::Looper( "abrowse_download_handler" )
{
	DEBUG("DownloadHandler::DownloadHandler()\n");

	m_cNode.m_pcJob = NULL;
	m_cNode.m_nContentSize = -1;
	
	g_cGlobalMutex.Lock();
	SetMutex( &g_cGlobalMutex );
}

DownloadHandler::~DownloadHandler()
{	DEBUG("DownloadHandler::~DownloadHandler()\n");
	if( m_cNode.m_pcJob )
		m_cNode.m_pcJob->deref();
}

void DownloadHandler::Start( WebCore::ResourceHandle* job )
{
	m_cNode.m_pcJob = job;
	m_cNode.m_pcJob->ref();
	
	job->getInternal()->m_client = this;
	job->getInternal()->m_progressive = true;
	Run();
}

void DownloadHandler::didReceiveResponse( WebCore::ResourceHandle* job, const WebCore::ResourceResponse& response )
{
	m_cNode.m_nContentSize = response.expectedContentLength() > 0 ? response.expectedContentLength() : -1;
	m_cNode.m_cMimeType = response.mimeType();
	DEBUG( "Download %i %s\n", m_cNode.m_nContentSize, m_cNode.m_cMimeType.c_str() );
}

void DownloadHandler::didReceiveData( WebCore::ResourceHandle* job, const char* pData, int, int nLen )
{
	Lock();
	
	if( !m_cNode.m_bStarted )
	{
		/* Start */
		m_cNode.m_nStartTime = get_system_time();
		m_cNode.m_cURL = os::String( job->request().url().string().utf8().data() );
		
		m_cNode.m_cPreferredName = os::Path( os::String( job->request().url().path() ) ).GetLeaf();
	
		try
		{
			m_cNode.m_pcTmpFile = new os::TempFile( "ab-", "/tmp" );
		} catch( ... )
		{
			DEBUG( "Error: DownloadHandler::didReceiveData() failed to create temporary file\n" );
		}

		Message cOkMsg( ID_START_DOWNLOAD );
		Message cCancelMsg( ID_CANCEL_DOWNLOAD );

		m_cNode.m_pcJob = job;
		m_cNode.m_pcRequester = new DownloadRequester( Rect( 250, 200, 599, 300 ), "File Download", m_cNode.m_cURL, m_cNode.m_cPreferredName, m_cNode.m_cMimeType, m_cNode.m_nContentSize, Messenger( this ), cOkMsg, cCancelMsg );
		m_cNode.m_pcRequester->Show();
		m_cNode.m_pcRequester->MakeFocus();
		m_cNode.m_bStarted = true;
		
	}
	
	if ( m_cNode.m_bCanceled || ( m_cNode.m_pcTmpFile == NULL && m_cNode.m_pcFile == NULL ) )
	{
		Unlock();
		return;
	}
	int nBytesWritten;

	if ( m_cNode.m_pcFile != NULL )
	{
		nBytesWritten = m_cNode.m_pcFile->Write( pData, nLen );
	}
	else
	{
		nBytesWritten = m_cNode.m_pcTmpFile->Write( pData, nLen );
	}
	if ( nBytesWritten != nLen )
	{
		( new os::Alert( "Error: Failed to create file!", os::String ().Format( "Failed to write %df bytes to\n%s\n\nError: %s\n", m_cNode.m_cPath.c_str(  ), strerror( errno ) ), 0, "Ok", NULL ) )->Go( NULL );

		m_cNode.m_bCanceled = true;

		if ( m_cNode.m_pcProgressDlg != NULL )
		{
			m_cNode.m_pcProgressDlg->Terminate();
			m_cNode.m_pcProgressDlg = NULL;
		}
		if ( m_cNode.m_pcRequester == NULL )
		{
			delete m_cNode.m_pcTmpFile;
			delete m_cNode.m_pcFile;

			if ( m_cNode.m_cPath.empty() == false )
			{
				unlink( m_cNode.m_cPath.c_str() );
			}
		}
		else
		{
			m_cNode.m_bDone = true;
		}
		PostMessage( os::M_QUIT );
	}
	else
	{
		m_cNode.m_nBytesReceived += nLen;
		if ( m_cNode.m_pcProgressDlg != NULL )
		{
			m_cNode.m_pcProgressDlg->UpdateProgress( m_cNode.m_nBytesReceived );
		}
	}
	Unlock();
}

void DownloadHandler::didFinishLoading( WebCore::ResourceHandle* )
{
	Finish();
}

void DownloadHandler::Finish()
{
	Lock();
	if ( m_cNode.m_bCanceled || ( m_cNode.m_pcTmpFile == NULL && m_cNode.m_pcFile == NULL ) )
	{
		Unlock();
		return;
	}

	if ( m_cNode.m_pcProgressDlg != NULL )
	{
		m_cNode.m_pcProgressDlg->Terminate();
		m_cNode.m_pcProgressDlg = NULL;
	}
	if ( m_cNode.m_pcRequester == NULL )
	{
		delete m_cNode.m_pcTmpFile;
		delete m_cNode.m_pcFile;
		PostMessage( os::M_QUIT );
	}
	else
	{
		m_cNode.m_bDone = true;
	}
	Unlock();
}

void DownloadHandler::didFail(WebCore::ResourceHandle*, const WebCore::ResourceError&)
{
	Lock();
	if ( m_cNode.m_bCanceled )
	{
		Unlock();
		return;
	}
	
	( new os::Alert( "Error: Download failed!", os::String ().Format( "Failed to download %s\n", m_cNode.m_pcJob->request().url().string().utf8().data() ), os::Alert::ALERT_WARNING, 0, "Ok", NULL ) )->Go( NULL );
	
	Unlock();
	
	Finish();
}

void DownloadHandler::HandleMessage( os::Message* pcMsg )
{
	switch( pcMsg->GetCode() )
	{
		case ID_START_DOWNLOAD:
		{
			std::string cPath;
			pcMsg->FindString( "path", &cPath );
			
			m_cNode.m_cPath = cPath;
			const std::string cDstDir = os::Path( cPath.c_str() ).GetDir(  ).GetPath(  ).c_str(  );

			try
			{
				os::FSNode cDirNode( cDstDir );
				if ( cDirNode.GetDev() == m_cNode.m_pcTmpFile->GetDev(  ) )
				{
					rename( m_cNode.m_pcTmpFile->GetPath().c_str(  ), cPath.c_str(  ) );
					m_cNode.m_pcTmpFile->Detatch();	// Make sure the destructor don't do anything silly.
				}
				else
				{
					m_cNode.m_pcFile = new os::File( cPath, O_WRONLY | O_CREAT );
					m_cNode.m_pcTmpFile->Seek( 0, SEEK_SET );
					char anBuffer[32 * 1024];

					for ( ;; )
					{
						int nLen = m_cNode.m_pcTmpFile->Read( anBuffer, sizeof( anBuffer ) );

						if ( nLen < 0 )
						{
							throw os::errno_exception( "Failed to copy temporary file" );
						}
						if ( m_cNode.m_pcFile->Write( anBuffer, nLen ) != nLen )
						{
							throw os::errno_exception( "Failed to copy temporary file" );
						}
						if ( nLen < int ( sizeof( anBuffer ) ) )
						{
							break;
						}
					}
					delete m_cNode.m_pcTmpFile;

					m_cNode.m_pcTmpFile = NULL;
				}
			}
			catch( ... )
			{
				( new os::Alert( "Error: Failed to create file!", os::String ().Format( "Failed to create '%s'", cPath.c_str(  ) ), 0, "Sorry!", NULL ) )->Go( NULL );
			}

			m_cNode.m_pcRequester = NULL;

			if ( m_cNode.m_bDone == false )
			{
				Message cCancelMsg( ID_CANCEL_DOWNLOAD );

				m_cNode.m_pcProgressDlg = new DownloadProgress( os::Rect( 100, 100, 449, 349 ), os::String ().Format( "Download: %s", cPath.c_str(  ) ), m_cNode.m_cURL, cPath, m_cNode.m_nContentSize, m_cNode.m_nStartTime, os::Messenger( this ), cCancelMsg );

				m_cNode.m_pcProgressDlg->Show();
			}
			else
			{
				delete m_cNode.m_pcTmpFile;
				delete m_cNode.m_pcFile;
				PostMessage( os::M_QUIT );
			}
			break;
		}
	case ID_CANCEL_DOWNLOAD:
		{
			if ( m_cNode.m_pcProgressDlg != NULL )
			{
				m_cNode.m_pcProgressDlg->Terminate();
				m_cNode.m_pcProgressDlg = NULL;
			}
			if ( m_cNode.m_cPath.empty() == false )
			{
				unlink( m_cNode.m_cPath.c_str() );
			}
			if ( m_cNode.m_bDone == false )
			{
				if( m_cNode.m_pcJob != NULL )
					m_cNode.m_pcJob->cancel();
				m_cNode.m_pcRequester = NULL;
				m_cNode.m_bCanceled = true;
				delete m_cNode.m_pcTmpFile;

				m_cNode.m_pcTmpFile = NULL;
				delete m_cNode.m_pcFile;

				m_cNode.m_pcFile = NULL;
			}
			else
			{
				delete m_cNode.m_pcTmpFile;
				delete m_cNode.m_pcFile;
			}
			PostMessage( os::M_QUIT );			
			break;
		}
	}
}



















