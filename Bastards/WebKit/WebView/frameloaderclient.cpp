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
#include <gui/requesters.h>

#include <util/locker.h>
#include <util/message.h>

#include <webview/frameloaderclient.h>
#include <webview/webview.h>
#include <webview/downloaddialog.h>

#include <Frame.h>
#include <FrameView.h>
#include <FrameTree.h>
#include <FrameLoader.h>
#include <FrameLoadRequest.h>
#include <HTMLFrameOwnerElement.h>

#include <ContextMenuClientSyllable.h>

#include <WindowFeatures.h>

#include <HitTestResult.h>
#include <CString.h>
#include <SyllableDebug.h>

#include <page/Page.h>
#include <page/EventHandler.h>

using namespace os;

WebViewFrameLoaderClient::WebViewFrameLoaderClient( WebView* pcView )
{
	m_pcView = pcView;
	m_pcMessenger = m_pcView->CreateMessenger();

	m_pcFrameView = NULL;
	m_pcFrame = NULL;
}

WebViewFrameLoaderClient::~WebViewFrameLoaderClient()
{
	if( m_pcMessenger )
		delete( m_pcMessenger );
}

bool WebViewFrameLoaderClient::hasWebView() const
{
	return( true );
}

bool WebViewFrameLoaderClient::hasFrameView() const
{
	return( true );
}

bool WebViewFrameLoaderClient::privateBrowsingEnabled() const
{
	return( false );
}

bool WebViewFrameLoaderClient::shouldFallBack(const WebCore::ResourceError&)
{
	return( true );
}

void WebViewFrameLoaderClient::download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceRequest&, const WebCore::ResourceResponse& response)
{
	DEBUG( "WebViewFrameLoaderClient::download()\n" );

	DownloadHandler* pcHandler = new DownloadHandler();
	pcHandler->Start( handle );
	pcHandler->didReceiveResponse( handle, response );
}
	
void WebViewFrameLoaderClient::dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction policyFunction, const WebCore::String& mimeType, const WebCore::ResourceRequest&)
{
   	// FIXME: we need to call directly here (comment copied from Qt version)
   	ASSERT(frame() && policyFunction);
	if (!frame() || !policyFunction)
       	return;

	if( strncasecmp( os::String( mimeType ).c_str(), "text/", 5 ) == 0 ||
		strncasecmp( os::String( mimeType ).c_str(), "image/", 6 ) == 0 ||
		strncasecmp( os::String( mimeType ).c_str(), "application/xml", 15 ) == 0 ||
		strncasecmp( os::String( mimeType ).c_str(), "application/xhtml+xml", 21 ) == 0 /*||
		mimeType.isEmpty()*/ )
		(frame()->loader()->*policyFunction)(WebCore::PolicyUse);
	else
		(frame()->loader()->*policyFunction)(WebCore::PolicyDownload);
}

WebCore::Frame* WebViewFrameLoaderClient::dispatchCreatePage()
{
	WebCore::ResourceRequest cRequest;
	cRequest.setURL( "about:blank" );

	WebCore::WindowFeatures cFeatures;

	WebCore::Page* pcPage = frame()->page()->chrome()->createWindow( frame(), WebCore::FrameLoadRequest( cRequest ), cFeatures );
	return( pcPage->mainFrame() );
}

void WebViewFrameLoaderClient::dispatchDidStartProvisionalLoad()
{
	if( m_pcView && m_pcView->GetWebCoreFrame() == frame() )
	{
		Message cMsg( ID_URL_OPENED );
		cMsg.AddPointer( "view", m_pcView );
		cMsg.AddString( "url", frame()->loader()->provisionalDocumentLoader()->url().string().utf8().data() );
		m_pcMessenger->SendMessage( &cMsg );
	}
}

void WebViewFrameLoaderClient::dispatchDidFailProvisionalLoad(const WebCore::ResourceError& error)
{
	if( m_pcView && m_pcView->GetWebCoreFrame() == frame() )
	{
		Alert * pcAlert = new Alert( "Error", String( "Failed to load " ) + String( error.failingURL() ), Alert::ALERT_WARNING, 0x00, "Ok", NULL );
		pcAlert->Go( NULL );

		Message cMsg( ID_LOAD_FINISHED );
		cMsg.AddPointer( "view", m_pcView );
		m_pcMessenger->SendMessage( &cMsg );
	}
}

void WebViewFrameLoaderClient::updateGlobalHistoryForStandardLoad(const WebCore::KURL& url)
{
	if( m_pcView && m_pcView->GetWebCoreFrame() == frame() )
	{
		DEBUG( "Start %s\n", url.string().utf8().data() );

		Message cMsg( ID_URL_OPENED );
		cMsg.AddPointer( "view", m_pcView );
		cMsg.AddString( "url", url.string().utf8().data() );
		m_pcMessenger->SendMessage( &cMsg );
	}
}

void WebViewFrameLoaderClient::updateGlobalHistoryForReload(const WebCore::KURL& url)
{
	updateGlobalHistoryForStandardLoad( url );
}

void WebViewFrameLoaderClient::dispatchDidReceiveContentLength( WebCore::DocumentLoader*, unsigned long identifier, int lengthReceived)
{
}

void WebViewFrameLoaderClient::dispatchDidFinishLoading( WebCore::DocumentLoader*, unsigned long identifier )
{
}

void WebViewFrameLoaderClient::dispatchDidHandleOnloadEvents()
{
	DEBUG("Load finished %s!\n",os::String( frame()->tree()->name().domString() ).c_str() );
	DEBUG( "%x %x\n", frame()->tree()->firstChild(), frame()->document() );
		
	if( m_pcView )
	{
		Message cMsg( ID_LOAD_FINISHED );
		cMsg.AddPointer( "view", m_pcView );
		m_pcMessenger->SendMessage( &cMsg );
	}
}

WebCore::String WebViewFrameLoaderClient::overrideMediaType() const
{
	return( WebCore::String() );
}

bool WebViewFrameLoaderClient::canCachePage() const
{
	return( false );
}

void WebViewFrameLoaderClient::setDocumentViewFromCachedPage(WebCore::CachedPage* cachedPage)
{
   	WebCore::DocumentLoader* cachedDocumentLoader = cachedPage->documentLoader();
	cachedDocumentLoader->setFrame(frame());
}

void WebViewFrameLoaderClient::loadedFromCachedPage()
{
	frame()->forceLayout();
}

void WebViewFrameLoaderClient::forceLayout()
{
	frame()->forceLayout();
}

void WebViewFrameLoaderClient::dispatchDidReceiveTitle(const WebCore::String& title)
{
	if( m_pcView && m_pcView->GetWebCoreFrame() == frame()  )
	{
		Message cMsg( ID_SET_TITLE );
		cMsg.AddPointer( "view", m_pcView );
		cMsg.AddString( "title", String( title ) );
		m_pcMessenger->SendMessage( &cMsg );
	}
}

PassRefPtr<WebCore::Frame> WebViewFrameLoaderClient::createFrame(const WebCore::KURL& url, const WebCore::String& name, WebCore::HTMLFrameOwnerElement* ownerElement, const WebCore::String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
	if( m_pcView->GetWindow() != NULL )
		m_pcView->GetWindow()->Lock();

	WebViewFrameLoaderClient* pcLoaderClient = new WebViewFrameLoaderClient( m_pcView );
	pcLoaderClient->m_pcFrame = new WebCore::Frame( m_pcView->GetWebCoreFrame()->page(), ownerElement, pcLoaderClient );
   	pcLoaderClient->setFrame( pcLoaderClient->m_pcFrame.get() );
   	pcLoaderClient->m_pcFrameView = new WebCore::FrameView( pcLoaderClient->m_pcFrame.get() );

   	DEBUG("CREATE Frame %x %s %s %x!\n", pcLoaderClient->m_pcFrame.get(), url.string().utf8().data(), os::String( name ).c_str(), ownerElement->document()->frame() );

   	pcLoaderClient->m_pcFrame->setView(pcLoaderClient->m_pcFrameView.get());
   	pcLoaderClient->m_pcFrame->init();
	pcLoaderClient->m_pcFrame->deref();	    
   	pcLoaderClient->m_pcFrameView->deref();
   	pcLoaderClient->m_pcFrameView->setAllowsScrolling( allowsScrolling );
	pcLoaderClient->m_pcFrame->tree()->setName( name );

	frame()->tree()->appendChild(pcLoaderClient->m_pcFrame);
	pcLoaderClient->m_pcFrameView->hide();

	WebCore::FrameLoadType loadType = frame()->loader()->loadType();
	WebCore::FrameLoadType childLoadType = WebCore::FrameLoadTypeRedirectWithLockedHistory;

	if( url == "about:blank" )
		pcLoaderClient->m_pcFrameView->setStaticBackground( true );

	pcLoaderClient->m_pcFrame->loader()->load( url, referrer, childLoadType, WebCore::String(), NULL, 0);

	if( m_pcView->GetWindow() != NULL )
		m_pcView->GetWindow()->Unlock();

	// The frame's onload handler may have removed it from the document.
	if (!pcLoaderClient->m_pcFrame->tree()->parent())
		return 0;

	RefPtr<WebCore::Frame> cChildFrame = pcLoaderClient->m_pcFrame;
	return cChildFrame.get();
}

void WebViewFrameLoaderClient::detachedFromParent3()
{
}

void WebViewFrameLoaderClient::detachedFromParent4()
{
	m_pcFrameView = NULL;
	m_pcFrame = NULL;
}

