/* Webster (C)opyright	2008 Kristian Van Der Vliet
 * 						2004-2007 Arno Klenke
 *						2001 Kurt Skauen
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gui/window.h>
#include <util/message.h>

#include <browserwebview.h>
#include <browserchromeclient.h>
#include <messages.h>

using namespace os;

BrowserWebView::BrowserWebView( const Rect& cFrame, const String& cTitle, uint nTabIndex, BrowserChromeClient* pcClient, WebSettings *pcSettings, uint32 nResizeMask )
	: WebView( cFrame, cTitle, pcClient, pcSettings, nResizeMask )
{
	m_nTabIndex = nTabIndex;
	m_pcMessenger = NULL;

	m_cShortTitle =
	m_cLongTitle = "about:blank";
}

BrowserWebView::~BrowserWebView()
{
	delete( m_pcMessenger );
}

void BrowserWebView::AllAttached( void )
{
	Window *pcParent = GetWindow();
	m_pcMessenger = new Messenger( pcParent, pcParent );

	WebView::AllAttached();
}

void BrowserWebView::SaveLink( WebView *pcWebView, String &cURL )
{
	WebView::SaveLink( pcWebView, cURL );
}

#define SHORT_TITLE_LENGTH	32

void BrowserWebView::SetTitle( WebView *pcWebView, String &cTitle )
{
	m_cLongTitle = cTitle;

	/* Try to get the "short" titles as close as possible to the same width,
	   so that the tabs don't "jump" as the titles change */

	uint nLength = cTitle.Length();
	if( nLength > SHORT_TITLE_LENGTH )
	{
		m_cShortTitle = cTitle.Resize( SHORT_TITLE_LENGTH );
		m_cShortTitle += "...";
	}
	else
	{
		uint nPadding;

		m_cShortTitle = cTitle;
		nPadding = SHORT_TITLE_LENGTH - nLength;
		for( ; nPadding > 0; nPadding-- )
			m_cShortTitle += " ";
	}

	Message cMsg( ID_WEBVIEW_SET_TITLE );
	cMsg.AddString( "short", m_cShortTitle );
	cMsg.AddString( "long", m_cLongTitle );
	cMsg.AddInt32( "index", m_nTabIndex );
	m_pcMessenger->SendMessage( &cMsg );

}

void BrowserWebView::LoadStarted( WebView *pcWebView, String &cURL )
{
	Message cMsg( ID_WEBVIEW_LOAD_STARTED );
	cMsg.AddString( "url", cURL );
	cMsg.AddInt32( "index", m_nTabIndex );
	m_pcMessenger->SendMessage( &cMsg );
}

void BrowserWebView::LoadFinished( WebView *pcWebView )
{
	Message cMsg( ID_WEBVIEW_LOAD_FINISHED );
	cMsg.AddInt32( "index", m_nTabIndex );
	m_pcMessenger->SendMessage( &cMsg );	
}

void BrowserWebView::GetTitle( os::String &cShortTitle, os::String &cLongTitle ) const
{
	cShortTitle = m_cShortTitle;
	cLongTitle = m_cLongTitle;
}
