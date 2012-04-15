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

OutboundView::OutboundView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow ) : SettingsView( cFrame, "outbound", pcIdentity )
{
	m_pcIdentity = pcIdentity;

	Server cOutboundServer;
	bool bOutboundValid = false;
	if( m_pcIdentity->GetServer( cOutboundServer, SERVER_OUTBOUND, 0 ) == EOK )
		bOutboundValid = true;

	const uint32 nInputLabelWidth = 90;
	Rect cBounds = GetBounds();

	m_pcOutboundView = new View( cBounds, "outbound_view", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	Rect cInputFrame = cBounds;
	cInputFrame.top = 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;

	/* Server */
	m_pcServerInput = new InputView( cInputFrame, MSG_CFGWND_SENDINGEMAIL_SETTINGS_SMTPSERVER, nInputLabelWidth );
	if( bOutboundValid )
		m_pcServerInput->SetText( cOutboundServer.GetServer() );
	m_pcOutboundView->AddChild( m_pcServerInput );

	/* Does this SMTP server require a authentication? */
	Rect cAuthFrame = cBounds;
	cAuthFrame.top = cInputFrame.bottom + 5;
	cAuthFrame.bottom = cAuthFrame.top + INPUT_HEIGHT;

	m_pcRequiresAuth = new CheckBox( cAuthFrame, "outbound_auth1", MSG_CFGWND_SENDINGEMAIL_SETTINGS_REQUIRESAUTH, new Message( ID_REQUIRES_AUTH ) );
	cAuthFrame.right = m_pcRequiresAuth->GetPreferredSize( false ).x;
	m_pcRequiresAuth->SetFrame( cAuthFrame );
	m_pcOutboundView->AddChild( m_pcRequiresAuth );

	/* Username */
	cInputFrame.top = cAuthFrame.bottom + 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;
	m_pcUsernameInput = new InputView( cInputFrame, MSG_CFGWND_SENDINGEMAIL_SETTINGS_USERNAME, nInputLabelWidth );
	m_pcOutboundView->AddChild( m_pcUsernameInput );

	/* Password */
	cInputFrame.top = cInputFrame.bottom + 5;
	cInputFrame.bottom = cInputFrame.top + INPUT_HEIGHT;
	m_pcPasswordInput = new InputView( cInputFrame, MSG_CFGWND_SENDINGEMAIL_SETTINGS_PASSWORD, nInputLabelWidth );
	m_pcPasswordInput->SetPasswordMode();
	m_pcOutboundView->AddChild( m_pcPasswordInput );

	Rect cPortFrame = cBounds;
	cPortFrame.top = cInputFrame.bottom + 5;
	cPortFrame.bottom = cPortFrame.top + INPUT_HEIGHT;

	StringView *pcPortLabel = new StringView( cPortFrame, "outbound_port_label", MSG_CFGWND_SENDINGEMAIL_SETTINGS_SRVERPORT );
	cPortFrame.right = pcPortLabel->GetPreferredSize( false ).x;
	pcPortLabel->SetFrame( cPortFrame );
	m_pcOutboundView->AddChild( pcPortLabel );

	cPortFrame.left = cPortFrame.right + 5;
	cPortFrame.right = cBounds.right - 5;

	m_pcPort = new Spinner( cPortFrame, "outbound_port", 25, NULL );
	if( bOutboundValid )
		m_pcPort->SetValue( cOutboundServer.GetPort() );
	m_pcPort->SetMinValue( 1 );
	m_pcPort->SetMaxValue( 65535 );
	m_pcPort->SetStep( 1.0 );
	m_pcPort->SetFormat( "%.0f" );

	/* XXXKV: Spinner doesn't produce a usable width */
	//cPortFrame.right = cPortFrame.left + m_pcPort->GetPreferredSize( false ).x + 1;
	cPortFrame.right = cPortFrame.left + 70;
	cPortFrame.bottom = cPortFrame.top + m_pcPort->GetPreferredSize( false ).y + 1;
	m_pcPort->SetFrame( cPortFrame );
	m_pcOutboundView->AddChild( m_pcPort );

	cAuthFrame = cBounds;
	cAuthFrame.top = cPortFrame.bottom + 5;
	cAuthFrame.bottom = cAuthFrame.top + INPUT_HEIGHT;

	m_pcPopBeforeSmtp = new CheckBox( cAuthFrame, "outbound_auth2", MSG_CFGWND_SENDINGEMAIL_SETTINGS_POPBEFORESMTP, new Message( ID_POP3_AUTH ) );
	cAuthFrame.right = m_pcPopBeforeSmtp->GetPreferredSize( false ).x;
	m_pcPopBeforeSmtp->SetFrame( cAuthFrame );
	m_pcOutboundView->AddChild( m_pcPopBeforeSmtp );

	cAuthFrame.top = cAuthFrame.bottom + 5;
	cAuthFrame.bottom += INPUT_HEIGHT;

	StringView *pcAuthLabel = new StringView( cAuthFrame, "outbound_auth_label", MSG_CFGWND_SENDINGEMAIL_SETTINGS_SMTPAUTH );
	cAuthFrame.right = pcAuthLabel->GetPreferredSize( false ).x;
	pcAuthLabel->SetFrame( cAuthFrame );
	m_pcOutboundView->AddChild( pcAuthLabel );

	cAuthFrame.left = cAuthFrame.right + 5;
	cAuthFrame.right = cBounds.right - 5;

	m_pcPop3Account = new DropdownMenu( cAuthFrame, "outbound_pop3_auth_instance", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	m_pcPop3Account->SetTabOrder( NEXT_TAB_ORDER );

	cAuthFrame.bottom = cAuthFrame.top + m_pcPop3Account->GetPreferredSize( false ).y + 1;
	m_pcPop3Account->SetFrame( cAuthFrame );

	int i = 0;
	Server cInboundServer;
	while( m_pcIdentity->GetServer( cInboundServer, SERVER_INBOUND, i++ ) == EOK )
		m_pcPop3Account->AppendItem( cInboundServer.GetServer() );

	m_pcOutboundView->AddChild( m_pcPop3Account );

	if( bOutboundValid && cOutboundServer.GetFlags() & AUTH_SMTP )
	{
		m_pcRequiresAuth->SetValue( true );
		m_pcUsernameInput->SetEnable( true );
		m_pcUsernameInput->SetText( cOutboundServer.GetUsername() );
		m_pcPasswordInput->SetEnable( true );
		m_pcPasswordInput->SetText( cOutboundServer.GetPassword() );
	}
	else
	{
		m_pcRequiresAuth->SetValue( false );
		m_pcUsernameInput->SetEnable( false );
		m_pcPasswordInput->SetEnable( false );
	}

	if( bOutboundValid && cOutboundServer.GetFlags() & AUTH_POP3_BEFORE )
	{
		m_pcPopBeforeSmtp->SetValue( true );
		m_pcPop3Account->SetEnable( true );
		m_pcPop3Account->SetSelection( cOutboundServer.GetData().AsInt32() );
	}
	else
	{
		m_pcPopBeforeSmtp->SetValue( false );
		m_pcPop3Account->SetEnable( false );
	}

	AddChild( m_pcOutboundView );
}

OutboundView::~OutboundView()
{
	RemoveChild( m_pcOutboundView );
	delete( m_pcOutboundView );
}

void OutboundView::AttachedToWindow()
{
	View::AttachedToWindow();
	m_pcRequiresAuth->SetTarget( this );
	m_pcPopBeforeSmtp->SetTarget( this );
	m_pcPop3Account->SetTarget( this );
}

void OutboundView::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_REQUIRES_AUTH:
		{
			bool bEnable = m_pcRequiresAuth->GetValue();

			m_pcRequiresAuth->SetValue( bEnable );
			m_pcUsernameInput->SetEnable( bEnable );
			m_pcPasswordInput->SetEnable( bEnable );

			break;
		}

		case ID_POP3_AUTH:
		{
			m_pcPop3Account->SetEnable( m_pcPopBeforeSmtp->GetValue() );
			break;
		}

		default:
		{
			GetParent()->HandleMessage( pcMessage );
			break;
		}
	}
}

status_t OutboundView::Store( Identity *pcIdentity )
{
	debug( "OutboundView::Store\n" );

	Server cServer;
	cServer.SetTransport( "smtp" );
	cServer.SetServer( m_pcServerInput->GetText() );
	cServer.SetPort( m_pcPort->GetValue().AsInt16() );

	if( m_pcRequiresAuth->GetValue().AsBool() == true )
	{
		cServer.SetFlags( AUTH_SMTP );
		cServer.SetUsername( m_pcUsernameInput->GetText() );
		cServer.SetPassword( m_pcPasswordInput->GetText() );
	}
	else if( m_pcPopBeforeSmtp->GetValue().AsBool() == true )
	{
		cServer.SetFlags( AUTH_POP3_BEFORE );
		cServer.SetData( Variant( m_pcPop3Account->GetSelection() ) );
	}
	pcIdentity->AddServer( cServer, SERVER_OUTBOUND );

	return EOK;
}

status_t OutboundView::Update( Identity *pcIdentity )
{
	debug( "OutboundView::Update\n" );

	/* Remember which server is currently selected.  We have to be clever about this;
	   the order of the servers may have changed or the current server may no longer
	   exist at all. */
	int nSelection;
	String cAuthServer;

	nSelection = m_pcPop3Account->GetSelection();
	if( nSelection >= 0 )
		cAuthServer = m_pcPop3Account->GetItem( nSelection );
	nSelection = 0;

	m_pcPop3Account->Clear();

	/* Reload the list of available POP3 servers */
	int i = 0;
	Server cInboundServer;
	while( pcIdentity->GetServer( cInboundServer, SERVER_INBOUND, i++ ) == EOK )
	{
		String cServer = cInboundServer.GetServer();
		m_pcPop3Account->AppendItem( cServer );
		if( cAuthServer == cServer )
			nSelection = i + 1;
	}
	m_pcPop3Account->SetSelection( nSelection, false );

	return EOK;
}

