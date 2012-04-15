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

#include <util/message.h>

#include <browserchromeclient.h>
#include <messages.h>

using namespace os;

BrowserChromeClient::BrowserChromeClient( Window* pcWindow ) : WebViewChromeClient( pcWindow )
{
	m_pcMessenger = new Messenger( pcWindow, pcWindow );

	bStatusBarTextSet = false;
}

BrowserChromeClient::~BrowserChromeClient()
{
	delete( m_pcMessenger );
}

WebCore::Page* BrowserChromeClient::createWindow(WebCore::Frame* pcFrame, const WebCore::FrameLoadRequest& cRequest, const WebCore::WindowFeatures& cFeatures)
{
	fprintf( stderr, "BrowserChromeClient::createWindow( %s )\n", cRequest.resourceRequest().url().string().utf8().data() );

	WebCore::Page *pcPage = NULL;
	Message cMsg, cReply;

	if( cFeatures.suggestBackgroundTab )
	{
		cMsg.SetCode( ID_CREATE_TAB );
	}
	else
	{
		Rect cFrame;

		cMsg.SetCode( ID_CREATE_WINDOW );

		if( cFeatures.xSet )
			cFrame.left = cFeatures.x;
		if( cFeatures.ySet )
			cFrame.top = cFeatures.y;
		if( cFeatures.widthSet )
			cFrame.right = cFeatures.width;
		if( cFeatures.heightSet )
			cFrame.bottom = cFeatures.height;

		if( cFrame != Rect() )
			cMsg.AddRect( "frame", cFrame );

		/* XXXKV: WindowFeatures also tells us if there should be a toolbar, menu etc. */
	}
	cMsg.AddString( "url", cRequest.resourceRequest().url().string().utf8().data() );

#if 0
	/* XXXKV: Attempting a syncronous SendMessage() just locks the app, possibly because
	   the target and ourselves share the same Looper. We might need to add a seperate
	   Looper to the Browser to handle these types of messages */
	m_pcMessenger->SendMessage( &cMsg, &cReply );
#else
	m_pcMessenger->SendMessage( &cMsg );
#endif
	cReply.FindPointer( "page", (void**)&pcPage );

	return pcPage;
}

void BrowserChromeClient::setStatusbarText(const WebCore::String& cString)
{
	/* URLs if we're over a link etc. are passed in here */
	if( cString != "" )
	{
		Message cMsg( ID_SET_STATUS_BAR_TEXT );
		cMsg.AddString( "text", cString.utf8().data() );
		m_pcMessenger->SendMessage( &cMsg );

		bStatusBarTextSet = true;
	}
	else if( bStatusBarTextSet )
	{
		Message cMsg( ID_CLEAR_STATUS_BAR_TEXT );
		m_pcMessenger->SendMessage( &cMsg );

		bStatusBarTextSet = false;
	}
}
