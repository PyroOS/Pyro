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

#ifndef __F_WEBVIEW_CHROMECLIENT_H__
#define __F_WEBVIEW_CHROMECLIENT_H__

#include <gui/window.h>

#include <ChromeClient.h>
#include <Frame.h>
#include <FrameView.h>
#include <FrameLoadRequest.h>
#include <PageCache.h>
#include <Cache.h>
#include <SharedTimer.h>
#include <CString.h>
#include <FloatRect.h>

namespace os
{

class WebViewChromeClient : public WebCore::ChromeClient
{
	public:
		WebViewChromeClient( os::Window* pcWindow );
		virtual ~WebViewChromeClient();

		virtual void chromeDestroyed()
		{
		}

		virtual void setWindowRect(const WebCore::FloatRect& cRect);
		virtual WebCore::FloatRect windowRect();

		virtual WebCore::FloatRect pageRect();

		virtual float scaleFactor()
		{
			return 1;
		}

		virtual void focus();
		virtual void unfocus();

		virtual bool canTakeFocus(WebCore::FocusDirection)
		{
			return true;
		}
		virtual void takeFocus(WebCore::FocusDirection)
		{
		}

		virtual WebCore::Page* createWindow(WebCore::Frame*, const WebCore::FrameLoadRequest& cRequest, const WebCore::WindowFeatures& )
		{
			return NULL;
		}

		virtual void show()
		{
		}

		virtual bool canRunModal()
		{
			return false;
		}
		virtual void runModal()
		{
		}

		virtual void setToolbarsVisible(bool)
		{
		}
		virtual bool toolbarsVisible()
		{
			return true;
		}

		virtual void setStatusbarVisible(bool)
		{
		}
		virtual bool statusbarVisible()
		{
			return true;
		}

		virtual void setScrollbarsVisible(bool)
		{
		}
		virtual bool scrollbarsVisible()
		{
			return true;
		}

		virtual void setMenubarVisible(bool)
		{
		}
		virtual bool menubarVisible()
		{
			return true;
		}

		virtual void setResizable(bool)
		{
		}

		virtual void addMessageToConsole(const WebCore::String& message, unsigned int lineNumber, const WebCore::String& sourceID);

		virtual bool canRunBeforeUnloadConfirmPanel()
		{
			return false;
		}
		virtual bool runBeforeUnloadConfirmPanel(const WebCore::String& message, WebCore::Frame* frame)
		{
			return false;
		}

		virtual void closeWindowSoon()
		{
		}

		virtual void runJavaScriptAlert(WebCore::Frame*, const WebCore::String& msg);
		virtual bool runJavaScriptConfirm(WebCore::Frame*, const WebCore::String& msg);
		virtual bool runJavaScriptPrompt(WebCore::Frame*, const WebCore::String& message, const WebCore::String& defaultValue, WebCore::String& result)
		{
			return false;
		}

		virtual void setStatusbarText(const WebCore::String& cString)
		{
			/* URLs if we're over a link etc. are passed in here */
		}
		virtual bool shouldInterruptJavaScript()
		{
			return false;
		}
		virtual bool tabsToLinks() const
		{
			return false;
		}

		virtual WebCore::IntRect windowResizerRect() const;
		virtual void addToDirtyRegion(const WebCore::IntRect&)
		{
		}
		virtual void scrollBackingStore(int, int, const WebCore::IntRect&, const WebCore::IntRect&)
		{
		}
		virtual void updateBackingStore()
		{
		}

		virtual void mouseDidMoveOverElement(const WebCore::HitTestResult&, unsigned /*modifierFlags*/)
		{
		}

		virtual void setToolTip(const WebCore::String&)
		{
		}

		virtual void print(WebCore::Frame*)
		{
		}

		virtual void exceededDatabaseQuota(WebCore::Frame*, const WebCore::String& databaseName)
		{
		}

	private:
		class Private;
		Private *m;
};

/* Encapsulation for the WebViewChromeClient class for clients which don't
   need to override any of the callbacks. Using SimpleChromeClient will help
   maintain the ABI if the WebCore::ChromeClient changes. */

class WebView;

class SimpleChromeClient
{
	public:
		SimpleChromeClient( os::Window* pcWindow );
		~SimpleChromeClient();

	protected:
		WebCore::ChromeClient* GetClient();

	private:
		friend class WebView;

		class Private;
		Private *m;
};

}

#endif

