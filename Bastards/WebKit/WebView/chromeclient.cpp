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

#include <gui/requesters.h>
#include <webview/chromeclient.h>

using namespace os;

class WebViewChromeClient::Private
{
	public:
		Window* m_pcWindow;
};

WebViewChromeClient::WebViewChromeClient( Window* pcWindow )
{
	m = new Private();
	m->m_pcWindow = pcWindow;
}

WebViewChromeClient::~WebViewChromeClient()
{
	delete( m );
}

void WebViewChromeClient::setWindowRect(const WebCore::FloatRect& cRect)
{
	m->m_pcWindow->SetFrame( cRect );
}

WebCore::FloatRect WebViewChromeClient::windowRect()
{
	return m->m_pcWindow->GetFrame();
}

WebCore::FloatRect WebViewChromeClient::pageRect()
{
	return m->m_pcWindow->GetFrame();
}

void WebViewChromeClient::focus()
{
	m->m_pcWindow->MakeFocus();
}

void WebViewChromeClient::unfocus()
{
	m->m_pcWindow->MakeFocus( false );
}

void WebViewChromeClient::addMessageToConsole(const WebCore::String& message, unsigned int lineNumber, const WebCore::String& sourceID)
{
	fprintf( stdout, "MESSAGE %s:%i %s\n", sourceID.utf8().data(), lineNumber, message.utf8().data() );
}

void WebViewChromeClient::runJavaScriptAlert(WebCore::Frame*, const WebCore::String& msg)
{
	Alert* pcAlert = new Alert( "JavaScript", msg, Alert::ALERT_INFO, "Ok", NULL );
	pcAlert->Go();
}

bool WebViewChromeClient::runJavaScriptConfirm(WebCore::Frame*, const WebCore::String& msg)
{
	Alert* pcAlert = new Alert( "JavaScript", msg, Alert::ALERT_QUESTION, "Yes", "No", NULL );
	return( pcAlert->Go() == 0 );
}

WebCore::IntRect WebViewChromeClient::windowResizerRect() const
{
	return WebCore::IntRect();
}

class SimpleChromeClient::Private
{
	public:
		WebCore::ChromeClient* m_pcClient;
};

SimpleChromeClient::SimpleChromeClient( Window* pcWindow )
{
	m = new Private();
	m->m_pcClient = new WebViewChromeClient( pcWindow );
}

SimpleChromeClient::~SimpleChromeClient()
{
	delete( m );
}

WebCore::ChromeClient* SimpleChromeClient::GetClient()
{
	return m->m_pcClient;
}

