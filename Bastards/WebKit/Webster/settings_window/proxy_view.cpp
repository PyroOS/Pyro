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

#ifndef BROWSER_SETTINGS_VIEW_PROXY_H
#define BROWSER_SETTINGS_VIEW_PROXY_H

#include <settings_views.h>

ProxyView::ProxyView( Rect cFrame, Settings *pcSettings, WebSettings *pcWebSettings, Handler *pcMainWindow ) : SettingsView( cFrame, "proxy_view", pcMainWindow )
{
	const uint32 nInputLabelWidth = 80;
	Rect cBounds = GetBounds();
	m_pcProxyView = new View( cBounds, "proxy_view", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	Rect cInputFrame = cBounds;
	cInputFrame.top = 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	m_pcHTTPProxyInput = new InputView( cInputFrame, "HTTP proxy", nInputLabelWidth );
	m_pcHTTPProxyInput->SetText( pcWebSettings->GetHTTPProxy() );
	m_pcProxyView->AddChild( m_pcHTTPProxyInput );

	cInputFrame.top = cInputFrame.bottom + 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	m_pcHTTPSProxyInput = new InputView( cInputFrame, "HTTPS proxy", nInputLabelWidth );
	m_pcHTTPSProxyInput->SetText( pcWebSettings->GetHTTPSProxy() );
	m_pcProxyView->AddChild( m_pcHTTPSProxyInput );

	cInputFrame.top = cInputFrame.bottom + 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	m_pcFTPProxyInput = new InputView( cInputFrame, "FTP proxy", nInputLabelWidth );
	m_pcFTPProxyInput->SetText( pcWebSettings->GetFTPProxy() );
	m_pcProxyView->AddChild( m_pcFTPProxyInput );

	AddChild( m_pcProxyView );
}

ProxyView::~ProxyView()
{
	RemoveChild( m_pcProxyView );
	delete( m_pcProxyView );
}

status_t ProxyView::Store( Settings *pcSettings, WebSettings *pcWebSettings )
{
	pcWebSettings->SetHTTPProxy( m_pcHTTPProxyInput->GetText() );
	pcWebSettings->SetHTTPSProxy( m_pcHTTPSProxyInput->GetText() );
	pcWebSettings->SetFTPProxy( m_pcFTPProxyInput->GetText() );

	return EOK;
}

#endif
