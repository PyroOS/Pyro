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

#ifndef __F_WEBVIEW_FRAMELOADERCLIENT_H__
#define __F_WEBVIEW_FRAMELOADERCLIENT_H__

#include <util/messenger.h>

#include <ResourceRequest.h>
#include <ResourceHandle.h>
#include <ResourceHandleInternal.h>
#include <ResourceResponse.h>

#include <FrameView.h>

#include <FrameLoaderClientSyllable.h>

#include <DocumentLoader.h>

#include <CachedPage.h>

namespace os
{
	class WebView;
}

/* WebCore::FrameLoaderClientSyllable implementation */
class WebViewFrameLoaderClient : public WebCore::FrameLoaderClientSyllable
{
	public:
		WebViewFrameLoaderClient( os::WebView* pcView );
		~WebViewFrameLoaderClient();

		bool hasWebView() const;
		bool hasFrameView() const;

		bool privateBrowsingEnabled() const;
		bool shouldFallBack(const WebCore::ResourceError&);

		void download(WebCore::ResourceHandle* handle, const WebCore::ResourceRequest& request, const WebCore::ResourceRequest&, const WebCore::ResourceResponse& response);
		void dispatchDecidePolicyForMIMEType(WebCore::FramePolicyFunction policyFunction, const WebCore::String& mimeType, const WebCore::ResourceRequest&);
		WebCore::Frame* dispatchCreatePage();

		void dispatchDidStartProvisionalLoad();
		void dispatchDidFailProvisionalLoad(const WebCore::ResourceError& error);

		void updateGlobalHistoryForStandardLoad(const WebCore::KURL& url);
		void updateGlobalHistoryForReload(const WebCore::KURL& url);

		void dispatchDidReceiveContentLength( WebCore::DocumentLoader*, unsigned long identifier, int lengthReceived);
		void dispatchDidFinishLoading( WebCore::DocumentLoader*, unsigned long identifier );
		void dispatchDidHandleOnloadEvents();

		WebCore::String overrideMediaType() const;
		bool canCachePage() const;

		void setDocumentViewFromCachedPage(WebCore::CachedPage* cachedPage);
		void loadedFromCachedPage();

		void forceLayout();
		void dispatchDidReceiveTitle(const WebCore::String& title);

		PassRefPtr<WebCore::Frame> createFrame(const WebCore::KURL& url, const WebCore::String& name, WebCore::HTMLFrameOwnerElement* ownerElement, const WebCore::String& referrer, bool allowsScrolling, int marginWidth, int marginHeight);

		void detachedFromParent3();
		void detachedFromParent4();

	private:
		os::WebView *m_pcView;
		os::Messenger *m_pcMessenger;

		WTF::RefPtr<WebCore::Frame> m_pcFrame;
		WTF::RefPtr<WebCore::FrameView> m_pcFrameView;
};

#endif	/* __F_WEBVIEW_FRAMELOADERCLIENT_H__ */
