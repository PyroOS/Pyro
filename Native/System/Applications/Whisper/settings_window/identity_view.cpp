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

IdentityView::IdentityView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow ) : SettingsView( cFrame, "identity", pcIdentity )
{
	m_pcIdentity = pcIdentity;

	const uint32 nInputLabelWidth = 120;
	Rect cBounds = GetBounds();
	m_pcIdentityView = new View( cBounds, "identity_view", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	Rect cInputFrame = cBounds;
	cInputFrame.top = 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	m_pcNameInput = new InputView( cInputFrame, MSG_CFGWND_YOURDETAILS_SETTINGS_NAME, nInputLabelWidth );
	m_pcNameInput->SetText( m_pcIdentity->GetName() );
	m_pcIdentityView->AddChild( m_pcNameInput );

	cInputFrame.top = cInputFrame.bottom + 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	m_pcEmailInput = new InputView( cInputFrame, MSG_CFGWND_YOURDETAILS_SETTINGS_EMAIL, nInputLabelWidth );
	m_pcEmailInput->SetText( m_pcIdentity->GetAddress() );
	m_pcIdentityView->AddChild( m_pcEmailInput );

	AddChild( m_pcIdentityView );
}

IdentityView::~IdentityView()
{
	RemoveChild( m_pcIdentityView );
	delete( m_pcIdentityView );
}

status_t IdentityView::Store( Identity *pcIdentity )
{
	debug( "IdentityView::Store\n" );

	pcIdentity->SetName( m_pcNameInput->GetText() );
	pcIdentity->SetAddress( m_pcEmailInput->GetText() );

	return EOK;
}

