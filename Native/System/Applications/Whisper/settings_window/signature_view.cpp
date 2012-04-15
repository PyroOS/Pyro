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

SignatureView::SignatureView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow ) : SettingsView( cFrame, "signature", pcIdentity )
{
	m_pcIdentity = pcIdentity;

	const uint32 nInputLabelWidth = 90;
	Rect cBounds = GetBounds();

	Rect cSignatureFrame = cBounds;
	m_pcSignatureView = new View( cSignatureFrame, "signature_view", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP );

	/* Name */
	cSignatureFrame.top = 5;
	cSignatureFrame.bottom = cSignatureFrame.top + INPUT_HEIGHT;
	m_pcTitleInput = new InputView( cSignatureFrame, MSG_CFGWND_SIGNATURES_SETTINGS_TITLE, nInputLabelWidth );
	m_pcSignatureView->AddChild( m_pcTitleInput );

	/* Username */
	cSignatureFrame.top = cSignatureFrame.bottom + 5;
	cSignatureFrame.bottom = cSignatureFrame.top + INPUT_HEIGHT * 5;

	m_pcTextInput = new InputView( cSignatureFrame, MSG_CFGWND_SIGNATURES_SETTINGS_TEXT, nInputLabelWidth );
	m_pcTextInput->SetMultiLine();
	m_pcSignatureView->AddChild( m_pcTextInput );
	
	/* A button to save the edit */
	Rect cSaveFrame = cBounds;
	cSaveFrame.left = ( cSaveFrame.right / 4 ) * 3;
	cSaveFrame.top = cSignatureFrame.bottom + 5;
	cSaveFrame.right = cBounds.right - 5;
	cSaveFrame.bottom = cSaveFrame.top + INPUT_HEIGHT;

	m_pcSave = new Button( cSaveFrame, "signature_save", MSG_CFGWND_SIGNATURES_SETTINGS_BUTTON_SAVE, new Message( ID_SIGNATURE_SAVE ), CF_FOLLOW_RIGHT );
	m_pcSave->SetTabOrder( NEXT_TAB_ORDER );

	cSaveFrame.left = cSaveFrame.right - m_pcSave->GetPreferredSize( false ).x + 1;
	cSaveFrame.bottom = cSaveFrame.top + m_pcSave->GetPreferredSize( false ).y + 1;
	m_pcSave->SetFrame( cSaveFrame );

	m_pcSignatureView->AddChild( m_pcSave );

	/* Add the signature details view to the main view */
	AddChild( m_pcSignatureView );

	/* The "instances" layout is composed of a ListView and some buttons, all within a
	   LayoutView.  The buttons are contained in a vertical layout node of their own */
	Rect cInstanceFrame = cBounds;
	cInstanceFrame.top = cSaveFrame.bottom + 5;

	m_pcLayoutView = new LayoutView( cInstanceFrame, "signatures_layout_view" );

	m_pcLayoutRoot = new VLayoutNode( "signatures_layout_root" );
	m_pcLayoutRoot->SetBorders( Rect( 5, 5, 5, 5 ) );

	m_pcInstancesLayout = new HLayoutNode( "signatures_instance_layout" );

	m_pcInstances = new ListView( Rect(), "signatures_instances", ListView::F_NO_AUTO_SORT | ListView::F_RENDER_BORDER );
	m_pcInstances->InsertColumn( MSG_CFGWND_SIGNATURES_SETTINGS_SERVERLIST.c_str(), 1 );
	m_pcInstances->SetSelChangeMsg( new Message( ID_SIGNATURE_SELECT ) );
	m_pcInstances->SetTabOrder( NEXT_TAB_ORDER );
	m_pcInstancesLayout->AddChild( m_pcInstances, 30.0f );
	m_pcInstancesLayout->AddChild( new HLayoutSpacer( "signatures_h_spacer", 1.0f ) );

	/* Buttons to manipulate the items in the list */
	m_pcButtonsLayout = new VLayoutNode( "signatures_layout_buttons", 2.0f );

	m_pcNewInstance = new Button( Rect(),"signatures_new", MSG_CFGWND_SIGNATURES_SETTINGS_BUTTON_NEW, new Message( ID_SIGNATURE_NEW ) );
	m_pcNewInstance->SetTabOrder( NEXT_TAB_ORDER );
	m_pcButtonsLayout->AddChild( m_pcNewInstance, 0.0f );

	m_pcDeleteInstance = new Button( Rect(),"signatures_delete", MSG_CFGWND_SIGNATURES_SETTINGS_BUTTON_DEL, new Message( ID_SIGNATURE_DELETE ) );
	m_pcDeleteInstance->SetTabOrder( NEXT_TAB_ORDER );
	m_pcButtonsLayout->AddChild( m_pcDeleteInstance, 0.0f );

	m_pcButtonsLayout->AddChild( new VLayoutSpacer( "signatures_v_spacer", 20.0f ) );

	m_pcInstancesLayout->AddChild( m_pcButtonsLayout );

	/* Add the "instances" layout to the main layout node */
	m_pcLayoutRoot->AddChild( m_pcInstancesLayout );

	/* Set the root layoutnode and add the layoutview to the view */
	m_pcLayoutView->SetRoot( m_pcLayoutRoot );
	AddChild( m_pcLayoutView );

	/* Disable everything but the ListView & New button until something is selected */
	m_pcTitleInput->SetEnable( false );
	m_pcTextInput->SetEnable( false );
	m_pcSave->SetEnable( false );
	m_pcDeleteInstance->SetEnable( false );

	/* Populate the ListView (if there is anything to populate it with) */
	String cName, cText;
	int i = 0;
	while( m_pcIdentity->GetSignature( cName, cText, i++ ) == EOK )
	{
		std::pair <os::String, os::String> cSignature;

		cSignature.first = cName;
		cSignature.second = cText;

		m_vSignatures.push_back( cSignature );

		ListViewStringRow *pcRow = new ListViewStringRow();
		pcRow->AppendString( cName );
		m_pcInstances->InsertRow( pcRow );
	}

	m_bNew = false;
}

SignatureView::~SignatureView()
{
	RemoveChild( m_pcSignatureView );
	delete( m_pcSignatureView );

	RemoveChild( m_pcLayoutView );
	delete( m_pcLayoutView );
}

void SignatureView::AllAttached()
{
	View::AllAttached();
	m_pcTitleInput->SetTarget( this );
	m_pcTextInput->SetTarget( this );
	m_pcSave->SetTarget( this );
	m_pcNewInstance->SetTarget( this );
	m_pcDeleteInstance->SetTarget( this );
	m_pcInstances->SetTarget( this );
}

void SignatureView::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_SIGNATURE_SAVE:
		{
			debug( "save\n" );

			m_pcSave->SetEnable( false );
			m_pcTitleInput->SetEnable( false );
			m_pcTextInput->SetEnable( false );

			if( m_bNew == false )
			{
				String cText;
				int nSelected = m_pcInstances->GetLastSelected();

				m_vSignatures[nSelected].first = m_pcTitleInput->GetText();

				/* If the user has entered their own seperator, strip it from the signature */
				const char *pzText = m_pcTextInput->GetText().c_str();
				if( pzText[0] == '-' && pzText[1] == '-' )
				{
					if( pzText[2] == ' ' && pzText[3] == '\n' )
						pzText = pzText + 4;
					else if( pzText[2] == '\n' )
						pzText = pzText + 3;
				}
				m_vSignatures[nSelected].second = pzText;
			}
			else
			{
				String cText;
				std::pair <os::String, os::String> cSignature;

				cSignature.first = m_pcTitleInput->GetText();

				/* If the user has entered their own seperator, strip it from the signature */
				const char *pzText = m_pcTextInput->GetText().c_str();
				if( pzText[0] == '-' && pzText[1] == '-' )
				{
					if( pzText[2] == ' ' && pzText[3] == '\n' )
						pzText = pzText + 4;
					else if( pzText[2] == '\n' )
						pzText = pzText + 3;
				}
				cSignature.second = pzText;

				m_vSignatures.push_back( cSignature );

				ListViewStringRow *pcRow = new ListViewStringRow();
				pcRow->AppendString( cSignature.first );
				m_pcInstances->InsertRow( pcRow );
			}

			/* Clear stale values */
			m_pcTitleInput->Clear();
			m_pcTextInput->Clear();

			/* Now we're finished, clear the edit flag */
			m_bNew = false;

			break;
		}

		case ID_SIGNATURE_NEW:
		{
			debug( "new\n" );

			/* Set a flag so that Save knows this is a new server to be inserted */
			m_bNew = true;

			/* Clear stale values */
			m_pcTitleInput->Clear();
			m_pcTextInput->Clear();

			/* Enable edit widgets */
			m_pcTitleInput->SetEnable( true );
			m_pcTextInput->SetEnable( true );
			m_pcSave->SetEnable( true );

			break;
		}

		case ID_SIGNATURE_DELETE:
		{
			debug( "delete\n" );

			int nSelected = m_pcInstances->GetLastSelected();

			/* If the user was creating a new server, cancel it */
			m_bNew = false;

			/* Clear stale values & disable all controls */
			m_pcTitleInput->Clear();
			m_pcTextInput->Clear();

			m_pcSave->SetEnable( false );
			m_pcTitleInput->SetEnable( false );
			m_pcTextInput->SetEnable( false );

			/* Remove instance from the vector and list */
			m_pcInstances->RemoveRow( nSelected, true );
			std::vector< std::pair<os::String, os::String> >::iterator i;
			int j;
			for( j = 0, i = m_vSignatures.begin(); j < nSelected; i++, j++)
				/* EMPTY */;
			i = m_vSignatures.erase( i );

			break;
		}

		case ID_SIGNATURE_SELECT:
		{
			debug( "select\n" );

			int nSelected = m_pcInstances->GetLastSelected();

			/* If the user was creating a new server, cancel it */
			m_bNew = false;

			if( nSelected >= 0 )
			{
				/* Enable the widgets and display details */
				m_pcTitleInput->SetEnable( true );
				m_pcTextInput->SetEnable( true );
				m_pcSave->SetEnable( true );
				m_pcDeleteInstance->SetEnable( true );

				m_pcTitleInput->SetText( m_vSignatures[nSelected].first );
				m_pcTextInput->SetText( m_vSignatures[nSelected].second );
			}

			break;
		}

		default:
		{
			View::HandleMessage( pcMessage );
			break;
		}
	}
}

status_t SignatureView::Store( Identity *pcIdentity )
{
	debug( "SignatureView::Store\n" );
	
	String cName, cText;
	std::vector< std::pair<os::String, os::String> >::iterator i;
	for( i = m_vSignatures.begin(); i != m_vSignatures.end(); i++ )
		pcIdentity->AddSignature( (*i).first, (*i).second );

	return EOK;
}

