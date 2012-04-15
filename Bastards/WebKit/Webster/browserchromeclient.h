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

#ifndef BROWSER_CHROMECLIENT_H
#define BROWSER_CHROMECLIENT_H

#include <gui/window.h>
#include <util/messenger.h>

#include <webview/chromeclient.h>

#include <WindowFeatures.h>

class BrowserChromeClient : public os::WebViewChromeClient
{
	public:
		BrowserChromeClient( os::Window* pcWindow );
		~BrowserChromeClient();

		WebCore::Page* createWindow(WebCore::Frame*, const WebCore::FrameLoadRequest& cRequest, const WebCore::WindowFeatures& );

		void setStatusbarText(const WebCore::String& cString);

	private:
		os::Messenger* m_pcMessenger;

		bool bStatusBarTextSet;
};

#endif

