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

InboundView::InboundView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow ) : SettingsView( cFrame, "inbound", pcIdentity )
{
	m_pcIdentity = pcIdentity;

	const uint32 nInputLabelWidth = 90;
	Rect cBounds = GetBounds();

	/* The "POP3 view" is made up of three InputViews; the server, username & password. */
	Rect cPop3Frame = cBounds;
	m_pcPop3View = new View( cPop3Frame, "inbound_pop3_view", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	/* Server */
	cPop3Frame.top = 5;
	cPop3Frame.bottom = cPop3Frame.top + INPUT_HEIGHT;
	m_pcServerInput = new InputView( cPop3Frame, MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_POPSERVER, nInputLabelWidth );
	m_pcPop3View->AddChild( m_pcServerInput );

	/* Username */
	cPop3Frame.top = cPop3Frame.bottom + 5;
	cPop3Frame.bottom = cPop3Frame.top + INPUT_HEIGHT;
	m_pcUsernameInput = new InputView( cPop3Frame, MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_USERNAME, nInputLabelWidth );
	m_pcPop3View->AddChild( m_pcUsernameInput );

	/* Password */
	cPop3Frame.top = cPop3Frame.bottom + 5;
	cPop3Frame.bottom = cPop3Frame.top + INPUT_HEIGHT;
	m_pcPasswordInput = new InputView( cPop3Frame, MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_PASSWORD, nInputLabelWidth );
	m_pcPasswordInput->SetPasswordMode();
	m_pcPop3View->AddChild( m_pcPasswordInput );

	/* Port # */
	Rect cPortFrame = cBounds;
	cPortFrame.top = cPop3Frame.bottom + 5;
	cPortFrame.bottom = cPortFrame.top + INPUT_HEIGHT;

	StringView *pcPortLabel = new StringView( cPortFrame, "inbound_port_label", MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_CONNECTONPORT );
	cPortFrame.right = pcPortLabel->GetPreferredSize( false ).x;
	pcPortLabel->SetFrame( cPortFrame );
	m_pcPop3View->AddChild( pcPortLabel );

	cPortFrame.left = cPortFrame.right + 5;
	cPortFrame.right = cBounds.right - 5;

	m_pcPort = new Spinner( cPortFrame, "inbound_port", 110, NULL );
	m_pcPort->SetMinValue( 1 );
	m_pcPort->SetMaxValue( 65535 );
	m_pcPort->SetStep( 1.0 );
	m_pcPort->SetFormat( "%.0f" );

	/* XXXKV: Spinner doesn't produce a usable width */
	//cPortFrame.right = cPortFrame.left + m_pcPort->GetPreferredSize( false ).x + 1;
	cPortFrame.right = cPortFrame.left + 70;
	cPortFrame.bottom = cPortFrame.top + m_pcPort->GetPreferredSize( false ).y + 1;
	m_pcPort->SetFrame( cPortFrame );
	m_pcPort->SetTabOrder( NEXT_TAB_ORDER );
	m_pcPop3View->AddChild( m_pcPort );

	/* Finally, a button to save the edit */
	Rect cSaveFrame = cBounds;
	cSaveFrame.top = cPortFrame.bottom + 5;
	cSaveFrame.bottom = cSaveFrame.top + INPUT_HEIGHT;
	cSaveFrame.right = ( cSaveFrame.right / 4 ) * 3;
	m_pcDeleteMail = new CheckBox( cSaveFrame, "inbound_delete", MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_DELMAIL, new Message( ID_INBOUND_DELETE_MAIL ) );
	m_pcDeleteMail->SetTabOrder( NEXT_TAB_ORDER );
	m_pcPop3View->AddChild( m_pcDeleteMail );

	cSaveFrame.left = cSaveFrame.right;
	cSaveFrame.right = cBounds.right - 5;
	m_pcSave = new Button( cSaveFrame, "inbound_save", MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_BUTTON_SAVE, new Message( ID_INBOUND_SAVE ), CF_FOLLOW_RIGHT );
	m_pcSave->SetTabOrder( NEXT_TAB_ORDER );

	cSaveFrame.left = cSaveFrame.right - m_pcSave->GetPreferredSize( false ).x + 1;
	cSaveFrame.bottom = cSaveFrame.top + m_pcSave->GetPreferredSize( false ).y + 1;
	m_pcSave->SetFrame( cSaveFrame );

	m_pcPop3View->AddChild( m_pcSave );

	/* Add the POP3 details view to the main view */
	AddChild( m_pcPop3View );

	/* The "instances" layout is composed of a ListView and some buttons, all within a
	   LayoutView.  The buttons are contained in a vertical layout node of their own */
	Rect cInstanceFrame = cBounds;
	cInstanceFrame.top = cSaveFrame.bottom + 5;

	m_pcLayoutView = new LayoutView( cInstanceFrame, "inbound_layout_view" );

	m_pcLayoutRoot = new VLayoutNode( "inbound_layout_root" );
	m_pcLayoutRoot->SetBorders( Rect( 5, 5, 5, 5 ) );

	m_pcInstancesLayout = new HLayoutNode( "inbound_instance_layout" );

	m_pcInstances = new ListView( Rect(), "inbound_instances", ListView::F_NO_AUTO_SORT | ListView::F_RENDER_BORDER );
	m_pcInstances->InsertColumn( MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_SERVERLIST.c_str(), 1 );
	m_pcInstances->SetSelChangeMsg( new Message( ID_INBOUND_SELECT ) );
	m_pcInstances->SetTabOrder( NEXT_TAB_ORDER );
	m_pcInstancesLayout->AddChild( m_pcInstances, 30.0f );
	m_pcInstancesLayout->AddChild( new HLayoutSpacer( "inbound_h_spacer", 1.0f ) );

	/* Buttons to manipulate the items in the list */
	m_pcButtonsLayout = new VLayoutNode( "inbound_layout_buttons", 2.0f );

	m_pcNewInstance = new Button( Rect(),"inbound_new", MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_BUTTON_NEW, new Message( ID_INBOUND_NEW ) );
	m_pcNewInstance->SetTabOrder( NEXT_TAB_ORDER );
	m_pcButtonsLayout->AddChild( m_pcNewInstance, 0.0f );

	m_pcDeleteInstance = new Button( Rect(),"inbound_delete", MSG_CFGWND_RECIEVINGEMAIL_SETTINGS_BUTTON_DEL, new Message( ID_INBOUND_DELETE ) );
	m_pcDeleteInstance->SetTabOrder( NEXT_TAB_ORDER );
	m_pcButtonsLayout->AddChild( m_pcDeleteInstance, 0.0f );

	m_pcButtonsLayout->AddChild( new VLayoutSpacer( "inbound_v_spacer", 20.0f ) );

	m_pcInstancesLayout->AddChild( m_pcButtonsLayout );

	/* Add the "instances" layout to the main layout node */
	m_pcLayoutRoot->AddChild( m_pcInstancesLayout );

	/* Set the root layoutnode and add the layoutview to the view */
	m_pcLayoutView->SetRoot( m_pcLayoutRoot );
	AddChild( m_pcLayoutView );

	/* Disable everything but the ListView & New button until something is selected */
	m_pcServerInput->SetEnable( false );
	m_pcUsernameInput->SetEnable( false );
	m_pcPasswordInput->SetEnable( false );
	m_pcPort->SetEnable( false );
	m_pcDeleteMail->SetEnable( false );
	m_pcSave->SetEnable( false );
	m_pcDeleteInstance->SetEnable( false );

	/* Populate the ListView (if there is anything to populate it with) */
	Server cInboundServer;
	int i=0;
	while( m_pcIdentity->GetServer( cInboundServer, SERVER_INBOUND, i++ ) == EOK )
		if( cInboundServer.GetTransport() == "pop3" )
		{
			m_vServers.push_back( cInboundServer );

			ListViewStringRow *pcRow = new ListViewStringRow();
			pcRow->AppendString( cInboundServer.GetServer() );
			m_pcInstances->InsertRow( pcRow );
		}

	m_bNew = false;
}

InboundView::~InboundView()
{
	RemoveChild( m_pcPop3View );
	delete( m_pcPop3View );

	RemoveChild( m_pcLayoutView );
	delete( m_pcLayoutView );
}

void InboundView::AllAttached()
{
	View::AllAttached();
	m_pcSave->SetTarget( this );
	m_pcNewInstance->SetTarget( this );
	m_pcDeleteInstance->SetTarget( this );
	m_pcInstances->SetTarget( this );
	m_pcDeleteMail->SetTarget( this );
}

void InboundView::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_INBOUND_SAVE:
		{
			debug( "save\n" );

			m_pcSave->SetEnable( false );
			m_pcServerInput->SetEnable( false );
			m_pcUsernameInput->SetEnable( false );
			m_pcPasswordInput->SetEnable( false );
			m_pcPort->SetEnable( false );
			m_pcDeleteMail->SetEnable( false );

			if( m_bNew == false )
			{
				int nSelected = m_pcInstances->GetLastSelected();

				m_vServers[nSelected].SetServer( m_pcServerInput->GetText() );
				m_vServers[nSelected].SetUsername( m_pcUsernameInput->GetText() );
				m_vServers[nSelected].SetPassword( m_pcPasswordInput->GetText() );
				if( m_pcDeleteMail->GetValue().AsBool() == true )
					m_vServers[nSelected].SetFlags( DELETE_ON_READ );
				else
					m_vServers[nSelected].SetFlags( 0x0 );
			}
			else
			{
				Server cServer;
				cServer.SetTransport( "pop3" );
				cServer.SetPort( m_pcPort->GetValue().AsInt16() );
				cServer.SetServer( m_pcServerInput->GetText() );
				cServer.SetUsername( m_pcUsernameInput->GetText() );
				cServer.SetPassword( m_pcPasswordInput->GetText() );

				if( m_pcDeleteMail->GetValue().AsBool() == true )
					cServer.SetFlags( DELETE_ON_READ );

				m_vServers.push_back( cServer );

				ListViewStringRow *pcRow = new ListViewStringRow();
				pcRow->AppendString( cServer.GetServer() );
				m_pcInstances->InsertRow( pcRow );
			}

			/* Clear stale values */
			m_pcServerInput->Clear();
			m_pcUsernameInput->Clear();
			m_pcPasswordInput->Clear();
			m_pcPort->SetValue( 110 );
			m_pcDeleteMail->SetValue( false );

			/* Now we're finished, clear the edit flag */
			m_bNew = false;

			break;
		}

		case ID_INBOUND_NEW:
		{
			debug( "new\n" );

			/* Set a flag so that Save knows this is a new server to be inserted */
			m_bNew = true;

			/* Clear stale values */
			m_pcServerInput->Clear();
			m_pcUsernameInput->Clear();
			m_pcPasswordInput->Clear();
			m_pcPort->SetValue( 110 );
			m_pcDeleteMail->SetValue( false );

			/* Enable edit widgets */
			m_pcServerInput->SetEnable( true );
			m_pcUsernameInput->SetEnable( true );
			m_pcPasswordInput->SetEnable( true );
			m_pcPort->SetEnable( true );
			m_pcDeleteMail->SetEnable( true );
			m_pcSave->SetEnable( true );

			break;
		}

		case ID_INBOUND_DELETE:
		{
			debug( "delete\n" );

			int nSelected = m_pcInstances->GetLastSelected();

			/* If the user was creating a new server, cancel it */
			m_bNew = false;

			/* Clear stale values & disable all controls */
			m_pcServerInput->Clear();
			m_pcUsernameInput->Clear();
			m_pcPasswordInput->Clear();
			m_pcPort->SetValue( 110 );
			m_pcDeleteMail->SetValue( false );

			m_pcSave->SetEnable( false );
			m_pcServerInput->SetEnable( false );
			m_pcUsernameInput->SetEnable( false );
			m_pcPasswordInput->SetEnable( false );
			m_pcPort->SetEnable( false );
			m_pcDeleteMail->SetEnable( false );

			/* Remove instance from the vector and list */
			m_pcInstances->RemoveRow( nSelected, true );
			std::vector<Server>::iterator i;
			int j;
			for( j = 0, i = m_vServers.begin(); j < nSelected; i++, j++)
				/* EMPTY */;
			i = m_vServers.erase( i );

			break;
		}

		case ID_INBOUND_SELECT:
		{
			debug( "select\n ");

			int nSelected = m_pcInstances->GetLastSelected();

			/* If the user was creating a new server, cancel it */
			m_bNew = false;

			if( nSelected >= 0 )
			{
				/* Enable the widgets and display details */
				m_pcServerInput->SetEnable( true );
				m_pcUsernameInput->SetEnable( true );
				m_pcPasswordInput->SetEnable( true );
				m_pcPort->SetEnable( true );
				m_pcDeleteMail->SetEnable( true );
				m_pcSave->SetEnable( true );
				m_pcDeleteInstance->SetEnable( true );

				m_pcServerInput->SetText( m_vServers[nSelected].GetServer() );
				m_pcUsernameInput->SetText( m_vServers[nSelected].GetUsername() );
				m_pcPasswordInput->SetText( m_vServers[nSelected].GetPassword() );
				m_pcPort->SetValue( m_vServers[nSelected].GetPort() );
				if( m_vServers[nSelected].GetFlags() & DELETE_ON_READ )
					m_pcDeleteMail->SetValue( true );
				else
					m_pcDeleteMail->SetValue( false );
			}

			break;
		}

		case ID_INBOUND_DELETE_MAIL:
		{
			debug( "delete mail\n" );
			break;
		}

		default:
		{
			View::HandleMessage( pcMessage );
			break;
		}
	}
}

status_t InboundView::Store( Identity *pcIdentity )
{
	debug( "InboundView::Store\n" );

	std::vector<Server>::iterator i;
	for( i = m_vServers.begin(); i != m_vServers.end(); i++ )
		pcIdentity->AddServer( *i, SERVER_INBOUND );

	return EOK;
}

