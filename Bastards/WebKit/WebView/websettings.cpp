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

#include <util/settings.h>
#include <webview/websettings.h>

#include <Settings.h>
#include <ResourceHandleManager.h>

using namespace os;

class WebSettings::Private
{
	public:
		bool m_bEnableJavaScript;
		bool m_bEnablePopups;

		String m_cStandard;
		String m_cFixed;
		String m_cSerif;
		String m_cSansSerif;
		String m_cCursiveFamily;
		String m_cFantasyFamily;

		float m_vMinimumFontSize;
		float m_vDefaultFontSize;

		String m_cHTTPProxy;
		String m_cHTTPSProxy;
		String m_cFTPProxy;
};

WebSettings::WebSettings()
{
	m = new Private();

	m->m_bEnableJavaScript = true;
	m->m_bEnablePopups = true;

	m->m_cStandard = "Arial";
	m->m_cFixed = "Courier";
	m->m_cSerif = "Times New Roman";
	m->m_cSansSerif = "Arial";
	m->m_cCursiveFamily = "";
	m->m_cFantasyFamily = "";

	m->m_vMinimumFontSize = 5.0f;
	m->m_vDefaultFontSize = 14.0f;

	m->m_cHTTPProxy = "";
	m->m_cHTTPSProxy = "";
	m->m_cFTPProxy = "";
}

WebSettings::~WebSettings()
{
	delete( m );
}

void WebSettings::Load()
{
	Settings cSettings;

	Path cPath = String( getenv( "HOME" ) ) + String( "/Settings/" );
	cSettings.SetPath( &cPath );
	cSettings.SetFile( "web" );
	cSettings.Load();

	m->m_bEnableJavaScript = cSettings.GetBool( "enable_javascript", m->m_bEnableJavaScript );
	m->m_bEnablePopups = cSettings.GetBool( "enable_popups", m->m_bEnablePopups );

	m->m_cStandard = cSettings.GetString( "standard_font", m->m_cStandard.c_str() );
	m->m_cFixed = cSettings.GetString( "fixed_font", m->m_cFixed.c_str() );
	m->m_cSerif = cSettings.GetString( "serif_font", m->m_cSerif.c_str() );
	m->m_cSansSerif = cSettings.GetString( "san_serif_font", m->m_cSansSerif.c_str() );
	m->m_cCursiveFamily = cSettings.GetString( "cursive_font", m->m_cCursiveFamily.c_str() );
	m->m_cFantasyFamily = cSettings.GetString( "fantasy_font", m->m_cFantasyFamily.c_str() );

	m->m_vMinimumFontSize = cSettings.GetFloat( "minimum_font_size", m->m_vMinimumFontSize );
	m->m_vDefaultFontSize = cSettings.GetFloat( "default_font_size", m->m_vDefaultFontSize );

	m->m_cHTTPProxy = cSettings.GetString( "http_proxy", m->m_cHTTPProxy.c_str() );
	m->m_cHTTPSProxy = cSettings.GetString( "https_proxy", m->m_cHTTPSProxy.c_str() );
	m->m_cFTPProxy = cSettings.GetString( "ftp_proxy", m->m_cFTPProxy.c_str() );

	/* Environment variables can take precedence over the saved settings */
	char *zEnvProxy;

	zEnvProxy = getenv( "http_proxy" );
	if( zEnvProxy == NULL )
		zEnvProxy = getenv( "HTTP_PROXY" );
	if( zEnvProxy )
		m->m_cHTTPProxy = zEnvProxy;

	zEnvProxy = getenv( "https_proxy" );
	if( zEnvProxy == NULL )
		zEnvProxy = getenv( "HTTPS_PROXY" );
	if( zEnvProxy )
		m->m_cHTTPSProxy = zEnvProxy;

	zEnvProxy = getenv( "ftp_proxy" );
	if( zEnvProxy == NULL )
		zEnvProxy = getenv( "FTP_PROXY" );
	if( zEnvProxy )
		m->m_cFTPProxy = zEnvProxy;

	SetProxies();
}

void WebSettings::Save()
{
	Settings cSettings;

	cSettings.SetBool( "enable_javascript", m->m_bEnableJavaScript );
	cSettings.SetBool( "enable_popups", m->m_bEnablePopups );

	cSettings.SetString( "standard_font", m->m_cStandard );
	cSettings.SetString( "fixed_font", m->m_cFixed );
	cSettings.SetString( "serif_font", m->m_cSerif );
	cSettings.SetString( "san_serif_font", m->m_cSansSerif );
	cSettings.SetString( "cursive_font", m->m_cCursiveFamily );
	cSettings.SetString( "fantasy_font", m->m_cFantasyFamily );

	cSettings.SetFloat( "minimum_font_size", m->m_vMinimumFontSize );
	cSettings.SetFloat( "default_font_size", m->m_vDefaultFontSize );

	cSettings.SetString( "http_proxy", m->m_cHTTPProxy );
	cSettings.SetString( "https_proxy", m->m_cHTTPSProxy );
	cSettings.SetString( "ftp_proxy", m->m_cFTPProxy );

	Path cPath = String( getenv( "HOME" ) ) + String( "/Settings/" );
	cSettings.SetPath( &cPath );
	cSettings.SetFile( "web" );
	cSettings.Save();
}

void WebSettings::EnableJavaScript( bool bEnable )
{
	m->m_bEnableJavaScript = bEnable;
}

void WebSettings::EnablePopupWindows( bool bEnable )
{
	m->m_bEnablePopups = bEnable;
}

void WebSettings::SetStandardFontFamily(const String& cFont)
{
	m->m_cStandard = cFont;
}

const String& WebSettings::GetStandardFontFamily() const
{
	return m->m_cStandard;
}

void WebSettings::SetFixedFontFamily(const String& cFont)
{
	m->m_cFixed = cFont;
}

const String& WebSettings::GetFixedFontFamily() const
{
	return m->m_cFixed;
}

void WebSettings::SetSerifFontFamily(const String& cFont)
{
	m->m_cSerif = cFont;
}

const String& WebSettings::GetSerifFontFamily() const
{
	return m->m_cSerif;
}

void WebSettings::SetSansSerifFontFamily(const String& cFont)
{
	m->m_cSansSerif = cFont;
}

const String& WebSettings::GetSansSerifFontFamily() const
{
	return m->m_cSansSerif;
}

void WebSettings::SetCursiveFontFamily(const String& cFont)
{
	m->m_cCursiveFamily = cFont;
}

const String& WebSettings::GetCursiveFontFamily() const
{
	return m->m_cCursiveFamily;
}

void WebSettings::SetFantasyFontFamily(const String& cFont)
{
	m->m_cFantasyFamily = cFont;
}

const String& WebSettings::GetFantasyFontFamily() const
{
	return m->m_cFantasyFamily;
}

void WebSettings::SetMinimumFontSize( float vSize )
{
	m->m_vMinimumFontSize = vSize;
}

float WebSettings::GetMinimumFontSize( void )
{
	return m->m_vMinimumFontSize;
}

void WebSettings::SetDefaultFontSize( float vSize )
{
	m->m_vDefaultFontSize = vSize;
}

float WebSettings::GetDefaultFontSize( void )
{
	return m->m_vDefaultFontSize;
}

void WebSettings::SetDefaultProxy(const String & cURL)
{
	m->m_cHTTPProxy = cURL;
	m->m_cHTTPSProxy = cURL;
	m->m_cFTPProxy = cURL;

	SetProxies();
}

void WebSettings::SetHTTPProxy(const String & cURL)
{
	m->m_cHTTPProxy = cURL;
	SetProxies();
}

const String& WebSettings::GetHTTPProxy( void )
{
	return m->m_cHTTPProxy;
}

void WebSettings::SetHTTPSProxy(const String & cURL)
{
	m->m_cHTTPSProxy = cURL;
	SetProxies();
}

const String& WebSettings::GetHTTPSProxy( void )
{
	return m->m_cHTTPSProxy;
}

void WebSettings::SetFTPProxy(const String & cURL)
{
	m->m_cFTPProxy = cURL;
	SetProxies();
}

const String& WebSettings::GetFTPProxy( void )
{
	return m->m_cFTPProxy;
}

void WebSettings::Apply(const WebCore::Page *pcPage) const
{
	WebCore::Settings *pcSettings = pcPage->settings();
	pcSettings->setLoadsImagesAutomatically( true );
	pcSettings->setShouldPrintBackgrounds( true );

	pcSettings->setJavaScriptEnabled( m->m_bEnableJavaScript );
	pcSettings->setJavaScriptCanOpenWindowsAutomatically( m->m_bEnablePopups );

	pcSettings->setStandardFontFamily( m->m_cStandard.c_str() );
	pcSettings->setFixedFontFamily( m->m_cFixed.c_str() );
	pcSettings->setSerifFontFamily( m->m_cSerif.c_str() );
	pcSettings->setSansSerifFontFamily( m->m_cSansSerif.c_str() );
	pcSettings->setCursiveFontFamily( m->m_cCursiveFamily.c_str() );
	pcSettings->setFantasyFontFamily( m->m_cFantasyFamily.c_str() );

	pcSettings->setMinimumFontSize( static_cast<int>( m->m_vMinimumFontSize ) );
	pcSettings->setMinimumLogicalFontSize( static_cast<int>( m->m_vMinimumFontSize ) );
	pcSettings->setDefaultFontSize( static_cast<int>( m->m_vDefaultFontSize ) );
	pcSettings->setDefaultFixedFontSize( static_cast<int>( m->m_vDefaultFontSize ) );
}

void WebSettings::SetProxies(void) const
{
	WebCore::ProxyManager::setProxy( m->m_cHTTPProxy, "http" );
	WebCore::ProxyManager::setProxy( m->m_cHTTPSProxy, "https" );
	WebCore::ProxyManager::setProxy( m->m_cFTPProxy, "ftp" );
}

