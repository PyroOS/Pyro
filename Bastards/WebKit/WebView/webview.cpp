/*  libwebview.so - Web rendering library for Syllable
 *  Copyright (C) 2008 Syllable Team
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

#include <gui/window.h>
#include <util/message.h>
#include <util/locker.h>
#include <util/clipboard.h>
#include <webview/webview.h>
#include <webview/downloaddialog.h>

#include <DragClientSyllable.h>
#include <FrameLoaderClientSyllable.h>
#include <Editor.h>
#include <EditorClientSyllable.h>
#include <InspectorClientSyllable.h>

#include <Frame.h>
#include <FrameView.h>
#include <FrameLoader.h>

#include <WindowFeatures.h>
#include <CachedPage.h>
#include <Settings.h>
#include <FocusController.h>

#include <HitTestResult.h>
#include <CString.h>

#include <page/Page.h>
#include <page/EventHandler.h>

using namespace os;

WebViewContextMenuClient::WebViewContextMenuClient( WebView* pcView )
{
	m_pcView = pcView;
	m_pcMessenger = m_pcView->CreateMessenger();
}

WebViewContextMenuClient::~WebViewContextMenuClient()
{
	if( m_pcMessenger )
		delete( m_pcMessenger );
}

void WebViewContextMenuClient::downloadURL(const WebCore::KURL& url)
{
	if( m_pcMessenger )
	{
		Message cMsg( ID_SAVE_LINK );
		cMsg.AddPointer( "view", m_pcView );
		cMsg.AddString( "url", url.string().utf8().data() );
		m_pcMessenger->SendMessage( &cMsg );
	}
}

class WebViewLooper : public Looper
{
	public:
		WebViewLooper( void ) : Looper( "webviewlooper" )
		{
			m_pcMessenger = NULL;
		}

		~WebViewLooper()
		{
			if( m_pcMessenger )
				delete( m_pcMessenger );
		}

		void HandleMessage( Message *pcMessage )
		{
			if( m_pcMessenger )
				m_pcMessenger->SendMessage( pcMessage );
		}

		void SetTarget( Handler *pcHandler, Looper *pcLooper )
		{
			if( m_pcMessenger )
				delete( m_pcMessenger );
			m_pcMessenger = new Messenger( pcHandler, pcLooper );
		}

	private:
		Messenger *m_pcMessenger;
};

/* The global mutex is supplied by WebCore */
extern Locker g_cGlobalMutex;

class WebView::Private
{
	public:
		WebViewLooper* m_pcLooper;
		Messenger* m_pcMessenger;
		WebSettings *m_pcDefaultSettings;

		WebCore::Page* m_pcPage;
		WTF::RefPtr<WebCore::Frame> m_pcFrame;
		WTF::RefPtr<WebCore::FrameView> m_pcFrameView;
};

WebView::WebView( const Rect& cFrame, const String& cTitle, WebCore::ChromeClient* pcClient, WebSettings *pcSettings, uint32 nResizeMask, uint32 nFlags ) : View( cFrame, cTitle, nResizeMask, nFlags )
{
	_WebView( cFrame, cTitle, pcClient, pcSettings, nResizeMask, nFlags );
}

WebView::WebView( const Rect& cFrame, const String& cTitle, SimpleChromeClient* pcClient, WebSettings *pcSettings, uint32 nResizeMask, uint32 nFlags )  : View( cFrame, cTitle, nResizeMask, nFlags )
{
	_WebView( cFrame, cTitle, pcClient->GetClient(), pcSettings, nResizeMask, nFlags );
}

void WebView::_WebView( const Rect& cFrame, const String& cTitle, WebCore::ChromeClient* pcClient, WebSettings *pcSettings, uint32 nResizeMask, uint32 nFlags )
{
	m = new Private;

	m->m_pcLooper = new WebViewLooper();
	m->m_pcLooper->Run();
	m->m_pcMessenger = CreateMessenger();

	WebCore::EditorClientSyllable* pcEditor = new WebCore::EditorClientSyllable();
	WebViewContextMenuClient *pcContextMenu = new WebViewContextMenuClient( this );

	m->m_pcPage = new WebCore::Page(static_cast<WebCore::ChromeClient*>(pcClient), pcContextMenu, pcEditor, new WebCore::DragClientSyllable(), new WebCore::InspectorClientSyllable() );
	pcEditor->setPage( m->m_pcPage );

	m->m_pcDefaultSettings = NULL;
	if( pcSettings == NULL )
		m->m_pcDefaultSettings = pcSettings = new WebSettings();
	pcSettings->Apply( m->m_pcPage );

	WebViewFrameLoaderClient *pcLoaderClient = new WebViewFrameLoaderClient( this );

	m->m_pcFrame = new WebCore::Frame( m->m_pcPage, 0, pcLoaderClient );
	pcLoaderClient->setFrame( m->m_pcFrame.get() );

	m->m_pcFrameView = new WebCore::FrameView( m->m_pcFrame.get() );
	m->m_pcFrameView->deref();
	m->m_pcFrame->setView(m->m_pcFrameView.get());
	m->m_pcFrame->init();
	m->m_pcFrameView->setParentWidget(this);
	m->m_pcFrame->view()->syllableWidget()->SetFrame( GetBounds() );
}

WebView::~WebView()
{
	g_cGlobalMutex.Lock();

	m->m_pcFrame->page()->focusController()->setActive( false );
	m->m_pcFrame->loader()->detachFromParent();

	/* XXXKV: Do these calls have side-effects? If not this entire block can be removed */
	WebCore::FrameView* pcFrameView = m->m_pcFrameView.get();
	WebCore::Frame* pcFrame = m->m_pcFrame.get();
	m->m_pcFrameView = NULL;
	m->m_pcFrame = NULL;

	if( m->m_pcLooper )
		m->m_pcLooper->Terminate();
	if( m->m_pcMessenger )
		delete( m->m_pcMessenger );
	if( m->m_pcDefaultSettings )
		delete( m->m_pcDefaultSettings );

	delete( m->m_pcPage );	
	delete( m );

	g_cGlobalMutex.Unlock();
}

void WebView::Shutdown()
{
	g_cGlobalMutex.Lock();

	WebCore::pageCache()->releaseAutoreleasedPagesNow();
	(WebCore::cache())->setDisabled(true);
	WebCore::stopSharedTimer();

	g_cGlobalMutex.Unlock();
}

void WebView::AllAttached()
{
	/* We have a valid looper so we can now receive messages */
	m->m_pcLooper->SetTarget( this, GetWindow() );
}

/* The client can overload this method and trap any other keys it's interested in */
void WebView::KeyDown( const char* pzString, const char* pzRawString, uint32 nQualifiers )
{
	if( nQualifiers & QUAL_CTRL )
	{
		switch( pzRawString[0] )
		{
			case 'r':
			{
				Reload();
				break;
			}

			case VK_INSERT:
			case 'c':
			{
				Copy();
				break;
			}

			default:
			{
				View::KeyDown( pzString, pzRawString, nQualifiers );
				break;
			}
		}
	}
	else
	{
		switch( pzRawString[0] )
		{
			case VK_FUNCTION_KEY:
			{
				Message *pcMsg = GetLooper()->GetCurrentMessage();
				ASSERT( pcMsg != NULL );
				int32 nKeyCode;

				if( pcMsg->FindInt32( "_raw_key", &nKeyCode ) != 0 )
					return;

				if( nKeyCode == 6 )	/* F5 */
					Reload();
				else
					View::KeyDown( pzString, pzRawString, nQualifiers );

				break;
			}

			default:
			{
				View::KeyDown( pzString, pzRawString, nQualifiers );
				break;
			}
		}
	}
}

void WebView::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_SET_TITLE:
		{
			WebView *pcWebView;
			String cTitle;

			pcMessage->FindPointer( "view", (void**)&pcWebView );
			pcMessage->FindString( "title", &cTitle );

			SetTitle( pcWebView, cTitle );
			break;
		}

		case ID_URL_OPENED:
		{
			WebView *pcWebView;
			String cURL;

			pcMessage->FindPointer( "view", (void**)&pcWebView );
			pcMessage->FindString( "url", &cURL );

			LoadStarted( pcWebView, cURL );

			break;
		}

		case ID_LOAD_FINISHED:
		{
			WebView *pcWebView;

			pcMessage->FindPointer( "view", (void**)&pcWebView );

			LoadFinished( pcWebView );

			break;
		}

		case ID_SAVE_LINK:
		{
			WebView *pcWebView;
			String cURL;

			pcMessage->FindPointer( "view", (void**)&pcWebView );
			pcMessage->FindString( "url", &cURL );

			SaveLink( pcWebView, cURL );

			break;
		}

		default:
		{
			View::HandleMessage( pcMessage );
			break;
		}
	}
}

Messenger* WebView::CreateMessenger()
{
	return new Messenger( NULL, m->m_pcLooper );
}

WebCore::Frame* WebView::GetWebCoreFrame()
{
	return m->m_pcFrame.get();
}

String WebView::GetCurrentURL()
{
	if( m->m_pcFrame->loader() == NULL || m->m_pcFrame->loader()->url().isEmpty() )
		return "";

	return String( m->m_pcFrame->loader()->url().string().utf8().data() );
}

void WebView::OpenURL( const String &cURL )
{
	WebCore::ResourceRequest cRequest;
	cRequest.setURL( WebCore::KURL( cURL.c_str() ) );

	m->m_pcFrame->loader()->load( cRequest );
}

int WebView::GetBackListCount()
{
	return m->m_pcPage->backForwardList()->backListCount();
}

int WebView::GetForwardListCount()
{
	return m->m_pcPage->backForwardList()->forwardListCount();
}

void WebView::GoBack()
{
	m->m_pcPage->goBack();
}

void WebView::GoForward()
{
	m->m_pcPage->goForward();
}

void WebView::Reload()
{
	m->m_pcFrame->loader()->reload();
}

void WebView::Stop()
{
	m->m_pcFrame->loader()->stopAllLoaders();
}

bool WebView::IsLoading()
{
	return m->m_pcFrame->loader()->isLoading();
}

void WebView::Find( String cString, bool bFromTop, bool bCaseSensitive )
{
	WebCore::Frame* pcFrame = GetWebCoreFrame()->page()->focusController()->focusedOrMainFrame();
	if( pcFrame )
	{
		if( bFromTop )
			pcFrame->selectionController()->clear();
		pcFrame->findString( cString.c_str(), true, bCaseSensitive, true, true );
	}
}

void WebView::Cut( void )
{
	WebCore::Frame* pcFrame = GetWebCoreFrame()->page()->focusController()->focusedOrMainFrame();
	if( pcFrame )
	{
		WebCore::Editor* pcEditor = pcFrame->editor();
		if( pcEditor )
			pcEditor->cut();
	}
}

void WebView::Copy( void )
{
	WebCore::Frame* pcFrame = GetWebCoreFrame()->page()->focusController()->focusedOrMainFrame();
	if( pcFrame )
	{
		WebCore::Editor* pcEditor = pcFrame->editor();
		if( pcEditor )
			pcEditor->copy();
	}
}

void WebView::Paste( void )
{
	WebCore::Frame* pcFrame = GetWebCoreFrame()->page()->focusController()->focusedOrMainFrame();
	if( pcFrame )
	{
		WebCore::Editor* pcEditor = pcFrame->editor();
		if( pcEditor )
			pcEditor->pasteAsPlainText();
	}
}

void WebView::Delete( void )
{
	WebCore::Frame* pcFrame = GetWebCoreFrame()->page()->focusController()->focusedOrMainFrame();
	if( pcFrame )
	{
		WebCore::Editor* pcEditor = pcFrame->editor();
		if( pcEditor )
			pcEditor->performDelete();
	}
}

void WebView::SaveLink( WebView *pcWebView, String &cURL )
{
	WebCore::ResourceRequest cRequest;
	cRequest.setURL( WebCore::KURL( cURL.c_str() ) );

	DownloadHandler* pcHandler = new DownloadHandler();
	WTF::RefPtr<WebCore::ResourceHandle> cHandle = WebCore::ResourceHandle::create( cRequest, pcHandler, NULL, false, false);

	pcHandler->Start( cHandle.get() );
}

/* Client callback stubs */
void WebView::SetTitle( WebView *pcWebView, String &cTitle )
{
	return;
}

void WebView::LoadStarted( WebView *pcWebView, String &cURL )
{
	return;
}

void WebView::LoadFinished( WebView *pcWebView )
{
	return;
}

