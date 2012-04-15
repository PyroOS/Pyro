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

#ifndef BROWSER_SETTINGS_VIEW_GENERAL_H
#define BROWSER_SETTINGS_VIEW_GENERAL_H

#include <settings_views.h>

GeneralView::GeneralView( Rect cFrame, Settings *pcSettings, WebSettings *pcWebSettings, Handler *pcMainWindow ) : SettingsView( cFrame, "general_view", pcMainWindow )
{
	const uint32 nInputLabelWidth = 80;
	Rect cBounds = GetBounds();
	m_pcGeneralView = new View( cBounds, "general_view", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	Rect cInputFrame = cBounds;
	cInputFrame.top = 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	m_pcHomepageInput = new InputView( cInputFrame, "Homepage", nInputLabelWidth );
	m_pcHomepageInput->SetText( pcSettings->GetString( "homepage", "" ) );
	m_pcGeneralView->AddChild( m_pcHomepageInput );

	AddChild( m_pcGeneralView );
}

GeneralView::~GeneralView()
{
	RemoveChild( m_pcGeneralView );
	delete( m_pcGeneralView );
}

status_t GeneralView::Store( Settings *pcSettings, WebSettings *pcWebSettings )
{
	pcSettings->SetString( "homepage", m_pcHomepageInput->GetText() );

	return EOK;
}



#endif
