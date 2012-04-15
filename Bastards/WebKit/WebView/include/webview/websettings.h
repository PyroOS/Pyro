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

#ifndef __F_WEBVIEW_WEBSETTINGS_H__
#define __F_WEBVIEW_WEBSETTINGS_H__

#include <util/string.h>

#include <Page.h>

namespace os
{

class WebView;

class WebSettings
{
	public:
		WebSettings();
		virtual ~WebSettings();

		/* XXXKV: Copy constructor may be useful */

		virtual void Load();
		virtual void Save();

		virtual void EnableJavaScript( bool bEnable );
		virtual void EnablePopupWindows( bool bEnable );

		virtual void SetStandardFontFamily(const String&);
		virtual const String& GetStandardFontFamily() const;

		virtual void SetFixedFontFamily(const String&);
		virtual const String& GetFixedFontFamily() const;

		virtual void SetSerifFontFamily(const String&);
		virtual const String& GetSerifFontFamily() const;

		virtual void SetSansSerifFontFamily(const String&);
		virtual const String& GetSansSerifFontFamily() const;

		virtual void SetCursiveFontFamily(const String&);
		virtual const String& GetCursiveFontFamily() const;

		virtual void SetFantasyFontFamily(const String&);
		virtual const String& GetFantasyFontFamily() const;

		virtual void SetMinimumFontSize( float vSize );
		virtual float GetMinimumFontSize( void );

		virtual void SetDefaultFontSize( float vSize );
		virtual float GetDefaultFontSize( void );

		virtual void SetDefaultProxy(const String &);

		virtual void SetHTTPProxy(const String &);
		virtual const String& GetHTTPProxy( void );

		virtual void SetHTTPSProxy(const String &);
		virtual const String& GetHTTPSProxy( void );

		virtual void SetFTPProxy(const String &);
		virtual const String& GetFTPProxy( void );

	protected:
		void Apply(const WebCore::Page *pcPage) const;
		void SetProxies(void) const;

	private:
		friend class WebView;

		class Private;
		Private *m;
};

}

#endif

