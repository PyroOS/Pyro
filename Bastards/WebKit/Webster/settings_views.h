/* Webster (C)opyright	2005-2008 Kristian Van Der Vliet
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

#ifndef BROWSER_SETTINGS_VIEWS_H
#define BROWSER_SETTINGS_VIEWS_H

#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/frameview.h>
#include <gui/scrollbar.h>
#include <gui/textview.h>
#include <gui/checkbox.h>
#include <gui/dropdownmenu.h>
#include <gui/listview.h>
#include <gui/button.h>
#include <gui/spinner.h>
#include <gui/tabview.h>
#include <util/string.h>
#include <util/settings.h>
#include <util/message.h>

#include <inputview.h>

#include <webview/websettings.h>

#include <vector>

#define IL_SCROLL_WIDTH		16

class SettingsView : public View
{
	public:
		SettingsView( Rect cFrame, String cTitle, Handler *pcMainWindow = NULL ) : View( cFrame, cTitle, CF_FOLLOW_ALL ){};
		virtual ~SettingsView(){};

		virtual void AttachedToWindow(){View::AttachedToWindow();};
		virtual void HandleMessage( Message *pcMessage ){View::HandleMessage( pcMessage );};

		virtual status_t Store( Settings *pcSettings, WebSettings *pcWebSettings ){return ENOSYS;};
		virtual status_t Update( Settings *pcSettings, WebSettings *pcWebSettings ){return ENOSYS;};

		virtual status_t Save( void ){return ENOSYS;};
		virtual status_t Apply( void ){return ENOSYS;};
		virtual status_t Cancel( void ){return ENOSYS;};
};

/* Views that will be attached within a TabView I.e. for Filters */
class SettingsTab : public View
{
	public:
		SettingsTab( Rect cFrame, String cName, Settings *pcSettings, WebSettings *pcWebSettings, Handler *pcMainWindow = NULL ) : View( cFrame, cName, CF_FOLLOW_ALL ){};
		virtual ~SettingsTab(){};

		virtual void AttachedToWindow(){View::AttachedToWindow();};
		virtual void HandleMessage( Message *pcMessage ){View::HandleMessage( pcMessage );};

		virtual status_t Store( Settings *pcSettings, WebSettings *pcWebSettings ){return ENOSYS;};
		virtual status_t Update( Settings *pcSettings, WebSettings *pcWebSettings ){return ENOSYS;};

		virtual status_t Save( void ){return ENOSYS;};
		virtual status_t Apply( void ){return ENOSYS;};
		virtual status_t Cancel( void ){return ENOSYS;};
};

#define INPUT_HEIGHT	22

class GeneralView : public SettingsView
{
	public:
		GeneralView( Rect cFrame, Settings *pcSettings, WebSettings *pcWebSettings, Handler *pcMainWindow );
		~GeneralView();

		status_t Store( Settings *pcSettings, WebSettings *pcWebSettings );

	private:
		View *m_pcGeneralView;

		InputView *m_pcHomepageInput;
};

class ProxyView : public SettingsView
{
	public:
		ProxyView( Rect cFrame, Settings *pcSettings, WebSettings *pcWebSettings, Handler *pcMainWindow );
		~ProxyView();

		status_t Store( Settings *pcSettings, WebSettings *pcWebSettings );

	private:
		View *m_pcProxyView;

		InputView *m_pcHTTPProxyInput;
		InputView *m_pcHTTPSProxyInput;
		InputView *m_pcFTPProxyInput;
};

#endif

