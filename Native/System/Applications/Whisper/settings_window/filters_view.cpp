/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */

#include <settings_views.h>
#include <messages.h>
#include <filter.h>
#include <resources/Whisper.h>

#include <debug.h>

FiltersView::FiltersView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow ) : SettingsView( cFrame, "filters", pcIdentity )
{
	m_pcIdentity = pcIdentity;

	Rect cBounds = GetBounds();
	m_pcFiltersTabView = new TabView( cBounds, "filters_tab_view", CF_FOLLOW_ALL );//CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	Rect cTabBounds = m_pcFiltersTabView->GetBounds();

	uint nIndex = 0;
	Filter *pcFilter;
	while( ( pcFilter = FilterEngine::GetFilter( nIndex ) ) != NULL )
	{
		debug( "got filter #%d, %s\n", nIndex, pcFilter->GetIdentifier().c_str() );

		SettingsTab *pcTab = pcFilter->GetSettingsView( cTabBounds, pcIdentity, pcMainWindow );
		if( pcTab )
			m_pcFiltersTabView->AppendTab( pcFilter->GetIdentifier(), pcTab );

		nIndex++;
	}

	AddChild( m_pcFiltersTabView );
}

FiltersView::~FiltersView()
{
	RemoveChild( m_pcFiltersTabView );
	delete( m_pcFiltersTabView );
}

status_t FiltersView::Store( Identity *pcIdentity )
{
	debug( "FiltersView::Store\n" );

	/* Store each individual Tab */
	uint nTabCount = m_pcFiltersTabView->GetTabCount();
	for( uint nIndex = 0; nIndex < nTabCount; nIndex++ )
	{
		SettingsTab *pcTab = static_cast<SettingsTab *>(m_pcFiltersTabView->GetTabView( nIndex ) );
		if( pcTab )
			pcTab->Store( pcIdentity );
	}

	return EOK;
}

status_t FiltersView::Save( void )
{
	debug( "FiltersView::Save\n" );

	/* Save each individual Tab */
	uint nTabCount = m_pcFiltersTabView->GetTabCount();
	for( uint nIndex = 0; nIndex < nTabCount; nIndex++ )
	{
		SettingsTab *pcTab = static_cast<SettingsTab *>(m_pcFiltersTabView->GetTabView( nIndex ) );
		if( pcTab )
			pcTab->Save();
	}

	return EOK;
}

