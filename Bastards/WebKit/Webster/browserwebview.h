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

#ifndef BROWSER_WEBVIEW_H
#define BROWSER_WEBVIEW_H

#include <gui/guidefines.h>
#include <gui/rect.h>
#include <util/string.h>
#include <util/messenger.h>

#include <browserchromeclient.h>

#include <webview/webview.h>
#include <webview/websettings.h>

class BrowserWebView : public os::WebView
{
	public:
		BrowserWebView( const os::Rect& cFrame, const os::String& cTitle, uint nTabIndex, BrowserChromeClient* pcClient, os::WebSettings *pcSettings, uint32 nResizeMask = os::CF_FOLLOW_ALL );
		virtual ~BrowserWebView();

		virtual void AllAttached( void );

		virtual void SaveLink( os::WebView *pcWebView, os::String &cURL );
		virtual void SetTitle( os::WebView *pcWebView, os::String &cTitle );
		virtual void LoadStarted( os::WebView *pcWebView, os::String &cURL );
		virtual void LoadFinished( os::WebView *pcWebView );

		virtual void SetTabIndex( uint nTabIndex )
		{
			m_nTabIndex = nTabIndex;
		}
		virtual uint GetTabIndex( void )
		{
			return m_nTabIndex;
		}

		virtual void GetTitle( os::String &cShortTitle, os::String &cLongTitle ) const;

	private:
		os::Messenger* m_pcMessenger;
		/* XXXKV: The Tab index may change if a tab is deleted (closed) or inserted before us */
		uint m_nTabIndex;

		os::String m_cShortTitle;
		os::String m_cLongTitle;
};

#endif

