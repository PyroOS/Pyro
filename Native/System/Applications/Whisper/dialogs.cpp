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

#include <dialogs.h>
#include <resources/Whisper.h>

#include <gui/button.h>

FolderNameDialog::FolderNameDialog( const Rect &cFrame, String cTitle, String cFolderName ) : Window( cFrame, "folder_name_dialog", cTitle, WND_NOT_RESIZABLE )
{
	m_pcInvoker = NULL;

	/* Create dialog */
	m_pcLayoutView = new LayoutView( GetBounds(), "folder_name_dialog" );

	VLayoutNode *pcNode = new VLayoutNode( "folder_name_dialog_root" );
	pcNode->SetBorders( Rect( 5, 4, 5, 4 ) );

	m_pcTextView = new TextView( Rect(), "folder_name", cFolderName.c_str() );
	m_pcTextView->SetMinPreferredSize( 10, 1 );
	pcNode->AddChild( m_pcTextView );
	pcNode->AddChild( new VLayoutSpacer( "folder_name_v_spacer", 1.0f ) );

	HLayoutNode *pcButtons = new HLayoutNode( "folder_name_buttons" );
	pcButtons->AddChild( new HLayoutSpacer( "folder_name_h_spacer" ) );
	Button *pcOkButton = new Button( Rect(), "folder_name_ok", MSG_MAINWND_MAILFOLDERLIST_CR_BUTTON_OK, new Message( ID_DIALOG_OK ) );
	pcButtons->AddChild( pcOkButton, 0.0f );
	pcButtons->AddChild( new HLayoutSpacer( "folder_name_h_spacer", 0.5f, 0.5f, pcButtons, 0.1f ) );
	pcButtons->AddChild( new Button( Rect(), "folder_name_cancel", MSG_MAINWND_MAILFOLDERLIST_CR_BUTTON_CANCEL, new Message( ID_DIALOG_CANCEL ) ), 0.0f );

	pcNode->AddChild( pcButtons );
	pcNode->AddChild( new VLayoutSpacer( "folder_name_v_spacer", 0.5f ) );

	m_pcLayoutView->SetRoot( pcNode );
	AddChild( m_pcLayoutView );

	/* Focus the controls */
	SetDefaultButton( pcOkButton );
	SetFocusChild( m_pcTextView );
}

FolderNameDialog::~FolderNameDialog()
{
	RemoveChild( m_pcLayoutView );
	delete( m_pcLayoutView );

	if( m_pcInvoker )
		delete( m_pcInvoker );
}

void FolderNameDialog::Go( Invoker *pcInvoker )
{
	m_pcInvoker = pcInvoker;
	Show();
	MakeFocus();
}

void FolderNameDialog::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_DIALOG_OK:
		case ID_DIALOG_CANCEL:
		{
			if( m_pcInvoker )
			{
				Message *pcInvokerMessage = m_pcInvoker->GetMessage();
				if( pcInvokerMessage )
				{
					pcInvokerMessage->AddInt32( "which", pcMessage->GetCode() );
					pcInvokerMessage->AddString( "name", m_pcTextView->GetValue().AsString() );
					m_pcInvoker->Invoke();
				}
			}

			PostMessage( M_QUIT );
			break;
		}

		default:
			Window::HandleMessage( pcMessage );
	}
}

IXPortProgressDialog::IXPortProgressDialog( const Rect &cFrame, String cTitle ) : Window( cFrame, "import_export_progress_dialog", cTitle, WND_NOT_RESIZABLE | WND_NO_CLOSE_BUT | WND_NO_ZOOM_BUT | WND_NO_DEPTH_BUT )
{
	m_pcInvoker = NULL;

	/* Create dialog */
	m_pcLayoutView = new LayoutView( GetBounds(), "folder_name_dialog" );

	VLayoutNode *pcNode = new VLayoutNode( "import_export_progress_dialog_root" );
	pcNode->SetBorders( Rect( 5, 4, 5, 4 ) );

	m_pcStringView = new StringView( Rect(), "import_export_progress_label", "" );
	m_pcStringView->SetMinPreferredSize( 10, 1 );

	HLayoutNode *pcMessage = new HLayoutNode( "import_export_progress_message" );
	pcMessage->AddChild( new HLayoutSpacer( "import_export_progress_h_spacer" ) );
	pcMessage->AddChild( m_pcStringView, 20.0f );
	pcMessage->AddChild( new HLayoutSpacer( "import_export_progress_h_spacer" ) );
	pcNode->AddChild( pcMessage );
	pcNode->AddChild( new VLayoutSpacer( "import_export_progress_v_spacer", 1.0f ) );

	m_pcProgressBar = new ProgressBar( Rect(), "import_export_progress" );
	pcNode->AddChild( m_pcProgressBar );

	pcNode->AddChild( new VLayoutSpacer( "import_export_progress_v_spacer", 0.5f ) );

	HLayoutNode *pcButtons = new HLayoutNode( "import_export_progress_buttons" );
	pcButtons->AddChild( new HLayoutSpacer( "import_export_progress_h_spacer" ) );
	pcButtons->AddChild( new Button( Rect(), "import_export_progress_cancel", MSG_MAINWND_MENU_FOLDER_IEPROGRESSCANCEL, new Message( ID_DIALOG_CANCEL ) ), 0.0f );

	pcNode->AddChild( pcButtons );
	pcNode->AddChild( new VLayoutSpacer( "import_export_progress_v_spacer", 0.5f ) );

	m_pcLayoutView->SetRoot( pcNode );
	AddChild( m_pcLayoutView );

	m_bCancelled = false;
}

IXPortProgressDialog::~IXPortProgressDialog()
{
	RemoveChild( m_pcLayoutView );
	delete( m_pcLayoutView );

	if( m_pcInvoker )
		delete( m_pcInvoker );
}

void IXPortProgressDialog::Go( Invoker *pcInvoker )
{
	m_pcInvoker = pcInvoker;
	Show();
	MakeFocus();
}

void IXPortProgressDialog::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_DIALOG_CANCEL:
		{
			m_bCancelled = true;
			break;
		}

		default:
			Window::HandleMessage( pcMessage );
	}
}

PropertiesDialog::PropertiesDialog( const Rect &cFrame, String cTitle, FolderProperties *pcProperties ) : Window( cFrame, "folder_properties_dialog", cTitle, WND_NOT_RESIZABLE | WND_NO_CLOSE_BUT | WND_NO_ZOOM_BUT | WND_NO_DEPTH_BUT )
{
	m_pcInvoker = NULL;
	m_pcProperties = pcProperties;

	/* Create dialog */
	m_pcLayoutView = new LayoutView( GetBounds(), "folder_properties_dialog" );

	VLayoutNode *pcNode = new VLayoutNode( "folder_properties_dialog_root" );
	pcNode->SetBorders( Rect( 5, 4, 5, 4 ) );

	m_pcCheckBox = new CheckBox( Rect(), "mailing_list", MSG_MAINWND_MAILFOLDERLIST_PROPERTIES_HAS_LIST, new Message( ID_PROPERTIES_DIALOG_CHECKBOX ) );
	pcNode->AddChild( m_pcCheckBox );
	pcNode->AddChild( new VLayoutSpacer( "folder_properties_v_spacer", 1.0f ) );

	m_pcTextView = new TextView( Rect(), "mailing_list_address", "" );
	m_pcTextView->SetMinPreferredSize( 10, 1 );
	pcNode->AddChild( m_pcTextView );
	pcNode->AddChild( new VLayoutSpacer( "folder_properties_v_spacer", 1.0f ) );

	HLayoutNode *pcButtons = new HLayoutNode( "folder_properties_buttons" );
	pcButtons->AddChild( new HLayoutSpacer( "folder_properties_h_spacer" ) );
	Button *pcOkButton = new Button( Rect(), "folder_properties_ok", MSG_MAINWND_MAILFOLDERLIST_CR_BUTTON_OK, new Message( ID_DIALOG_OK ) );
	pcButtons->AddChild( pcOkButton, 0.0f );
	pcButtons->AddChild( new HLayoutSpacer( "folder_properties_h_spacer", 0.5f, 0.5f, pcButtons, 0.1f ) );
	pcButtons->AddChild( new Button( Rect(), "folder_properties_cancel", MSG_MAINWND_MAILFOLDERLIST_CR_BUTTON_CANCEL, new Message( ID_DIALOG_CANCEL ) ), 0.0f );

	pcNode->AddChild( pcButtons );
	pcNode->AddChild( new VLayoutSpacer( "folder_properties_v_spacer", 0.5f ) );

	m_pcLayoutView->SetRoot( pcNode );
	AddChild( m_pcLayoutView );

	/* Set controls */
	if( m_pcProperties->GetFlags() & FL_HAS_LIST )
	{
		m_pcCheckBox->SetValue( true );
		m_pcTextView->SetEnable( true );
		m_pcTextView->Set( m_pcProperties->GetListAddress().c_str() );
	}
	else
	{
		m_pcCheckBox->SetValue( false );
		m_pcTextView->SetEnable( false );
	}

	/* Focus the controls */
	SetDefaultButton( pcOkButton );
	SetFocusChild( m_pcTextView );
}

PropertiesDialog::~PropertiesDialog()
{
	RemoveChild( m_pcLayoutView );
	delete( m_pcLayoutView );

	if( m_pcInvoker )
		delete( m_pcInvoker );
}

void PropertiesDialog::Go( Invoker *pcInvoker )
{
	m_pcInvoker = pcInvoker;
	Show();
	MakeFocus();
}

void PropertiesDialog::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case ID_PROPERTIES_DIALOG_CHECKBOX:
		{
			m_pcTextView->SetEnable( m_pcCheckBox->GetValue().AsBool() );
			break;
		}

		case ID_DIALOG_OK:
		{
			/* Change the folder properties */
			if( m_pcCheckBox->GetValue().AsBool() )
			{
				/* Set the "Has list" flag */
				uint64 nFlags = m_pcProperties->GetFlags();
				nFlags |= FL_HAS_LIST;
				m_pcProperties->SetFlags( nFlags );

				/* Set the list address */
				m_pcProperties->SetListAddress( m_pcTextView->GetBuffer()[0] );
			}
			else
			{
				/* Clear the "Has list" flag */
				uint64 nFlags = m_pcProperties->GetFlags();
				nFlags &= ~FL_HAS_LIST;
				m_pcProperties->SetFlags( nFlags );

				/* Clear the list address */
				m_pcProperties->SetListAddress( "" );
			}

			/* fall thru */
		}
		case ID_DIALOG_CANCEL:
		{
			if( m_pcInvoker )
			{
				Message *pcInvokerMessage = m_pcInvoker->GetMessage();
				if( pcInvokerMessage )
				{
					pcInvokerMessage->AddInt32( "which", pcMessage->GetCode() );
					pcInvokerMessage->AddPointer( "properties", m_pcProperties );
					m_pcInvoker->Invoke();
				}
			}

			PostMessage( M_QUIT );
			break;
		}

		default:
			Window::HandleMessage( pcMessage );
	}
}

