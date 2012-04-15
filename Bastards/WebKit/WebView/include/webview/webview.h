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

#ifndef __F_WEBVIEW_WEBVIEW_H__
#define __F_WEBVIEW_WEBVIEW_H__

#include <gui/view.h>
#include <util/looper.h>
#include <util/messenger.h>
#include <util/string.h>

#include <Frame.h>
#include <ContextMenuClientSyllable.h>

#include <ResourceRequest.h>

#include <webview/frameloaderclient.h>
#include <webview/chromeclient.h>
#include <webview/websettings.h>

enum messages
{
	ID_FIRST_WEBVIEW_MSG = 9999900,	/* M_LAST_USER_MSG = 9999999 */
	ID_SET_TITLE,
	ID_URL_OPENED,
	ID_LOAD_FINISHED,
	ID_SAVE_LINK
};

namespace os
{

class WebView;

/* WebCore::ContextMenuClientSyllable implementation */
class WebViewContextMenuClient : public WebCore::ContextMenuClientSyllable
{
	public:
		WebViewContextMenuClient( WebView* pcView );
		~WebViewContextMenuClient();

		void downloadURL(const WebCore::KURL& url);

	private:
		WebView* m_pcView;
		Messenger* m_pcMessenger;
};

class WebView : public os::View
{
	public:
		WebView( const os::Rect& cFrame, const os::String& cTitle, WebCore::ChromeClient* pcClient, os::WebSettings *pcSettings = NULL, uint32 nResizeMask = CF_FOLLOW_LEFT | CF_FOLLOW_TOP, uint32 nFlags = WID_WILL_DRAW | WID_CLEAR_BACKGROUND );
		WebView( const os::Rect& cFrame, const os::String& cTitle, os::SimpleChromeClient* pcClient, os::WebSettings *pcSettings = NULL, uint32 nResizeMask = CF_FOLLOW_LEFT | CF_FOLLOW_TOP, uint32 nFlags = WID_WILL_DRAW | WID_CLEAR_BACKGROUND );

		~WebView();

		static void Shutdown();

		virtual void AllAttached();

		virtual void HandleMessage( Message *pcMessage );
		virtual void KeyDown( const char* pzString, const char* pzRawString, uint32 nQualifiers );

		virtual Messenger* CreateMessenger();

		virtual WebCore::Frame* GetWebCoreFrame();

		virtual os::String GetCurrentURL();
		virtual void OpenURL( const os::String &cURL );

		virtual int GetBackListCount();
		virtual int GetForwardListCount();

		virtual void GoBack();
		virtual void GoForward();
		virtual void Reload();
		virtual void Stop();

		virtual bool IsLoading();

		virtual void Find( os::String cString, bool bFromTop, bool bCaseSensitive );

		virtual void Cut();
		virtual void Copy();
		virtual void Paste();
		virtual void Delete();

		virtual void SaveLink( WebView *pcWebView, os::String &cURL );

		virtual void SetTitle( WebView *pcWebView, os::String &cTitle );

		virtual void LoadStarted( WebView *pcWebView, os::String &cURL );
		virtual void LoadFinished( WebView *pcWebView );

	private:
		void _WebView( const os::Rect& cFrame, const os::String& cTitle, WebCore::ChromeClient* pcClient, os::WebSettings *pcSetting, uint32 nResizeMask, uint32 nFlags );

		class Private;
		Private* m;

		friend class SimpleChromeClient;
		friend class WebSettings;
};

}

#endif	/* __F_WEBVIEW_WEBVIEW_H__ */
