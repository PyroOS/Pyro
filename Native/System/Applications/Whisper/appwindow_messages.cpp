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

#include <posix/errno.h>
#include <gui/image.h>
#include <gui/requesters.h>
#include <util/application.h>
#include <util/resources.h>
#include <storage/file.h>

#include <appwindow.h>
#include <settings_window.h>
#include <composer_window.h>
#include <dialogs.h>
#include <identity.h>
#include <mailbox.h>
#include <qp_codec.h>
#include <base64_codec.h>
#include <transport.h>
#include <messages.h>
#include <ixport.h>
#include <version.h>
#include <messagenode.h>
#include <rfctime.h>
#include <message_window.h>
#include <resources/Whisper.h>

#include <debug.h>

#include <unistd.h>
#include <pwd.h>

void WhisperWindow::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_SELECT:
		{
			HandleSelect( pcMessage );
			break;
		}

		case M_FOLDER_SELECT:
		{
			HandleFolderSelect( pcMessage );
			break;
		}

		case M_CREATE_FOLDER:
		{
			/* Pop dialog to get name of new folder */
			FolderNameDialog *pcDialog = new FolderNameDialog( Rect( 0, 0, 300, 50 ), MSG_MAINWND_MAILFOLDERLIST_CREATE, MSG_MAINWND_MAILFOLDERLIST_CREATE_DEFAULT );
			pcDialog->CenterInWindow( this );
			pcDialog->Go( new Invoker( new Message( M_CREATE_FOLDER_DIALOG ), this ) );

			break;
		}

		case M_CREATE_FOLDER_DIALOG:
		{
			/* Check if OK or Cancel was selected */
			int32 nWhich;
			if( pcMessage->FindInt32( "which", &nWhich ) == EOK )
				if( nWhich == 0 )
					HandleCreateFolderDialog( pcMessage );
			break;
		}

		case M_RENAME_FOLDER:
		{
			/* Get the current name of the selected folder */
			ListViewRow *pcRow = m_pcTreeView->GetRow( m_nCurrentFolder );
			folder_cookie_s *psCookie = (folder_cookie_s*)dynamic_cast<TreeViewStringNode*>(pcRow)->GetCookie().AsPointer();

			/* Pop dialog to get the new name of the folder */
			FolderNameDialog *pcDialog = new FolderNameDialog( Rect( 0, 0, 300, 50 ), MSG_MAINWND_MAILFOLDERLIST_RENAME, psCookie->cName );
			pcDialog->CenterInWindow( this );
			pcDialog->Go( new Invoker( new Message( M_RENAME_FOLDER_DIALOG ), this ) );

			break;
		}

		case M_RENAME_FOLDER_DIALOG:
		{
			/* Check if OK or Cancel was selected */
			int32 nWhich;
			if( pcMessage->FindInt32( "which", &nWhich ) == EOK )
				if( nWhich == 0 )
					HandleRenameFolderDialog( pcMessage );
			break;
		}

		case M_DELETE_FOLDER:
		{
			HandleDeleteFolder( pcMessage );
			break;
		}

		case M_PROPERTIES_FOLDER:
		{
			HandlePropertiesFolder( pcMessage );
			break;
		}

		case M_PROPERTIES_FOLDER_DIALOG:
		{
			/* Check if OK or Cancel was selected */
			int32 nWhich;
			if( pcMessage->FindInt32( "which", &nWhich ) == EOK )
				if( nWhich == 0 )
					HandlePropertiesFolderDialog( pcMessage );
			break;
		}

		case M_SELECT_ALL_MESSAGES:
		{
			m_pcMessagesList->Select( 0, m_pcMessagesList->GetRowCount() - 1, true, true );
			break;
		}

		case M_CUT_MESSAGE:
		{
			HandleCutMessage( pcMessage );
			break;
		}

		case M_COPY_MESSAGE:
		{
			HandleCopyMessage( pcMessage );
			break;
		}

		case M_PASTE_MESSAGE:
		{
			HandlePasteMessage( pcMessage );
			break;
		}

		case M_DELETE_MESSAGE:
		{
			HandleDeleteMessage( pcMessage );
			break;
		}

		case M_FLAG_MESSAGE:
		{
			HandleFlagMessage( pcMessage );
			break;
		}

		case M_MARK_MESSAGE:
		{
			HandleMarkMessage( pcMessage );
			break;
		}

		case M_VIEW_MESSAGE:
		{
			if( m_nCurrentSelection >= 0 )
			{
				Mailmessage *pcMailMessage = new Mailmessage();

				m_pcFolder->Lock();
				m_pcFolder->Read( m_cCurrentReference, pcMailMessage );
				m_pcFolder->Unlock();

				MessageWindow *pcMessageWindow = new MessageWindow( Rect( 0, 0, 450, 500 ), "View message", m_pcVisualSettings, pcMailMessage );
				pcMessageWindow->CenterInWindow( this );
				pcMessageWindow->Show();
				pcMessageWindow->MakeFocus();
			}

			break;
		}

		case M_VIEW_MESSAGE_DATA:
		{
			if( m_nCurrentSelection >= 0 )
			{
				Mailmessage *pcMailMessage = new Mailmessage();

				m_pcFolder->Lock();
				m_pcFolder->Read( m_cCurrentReference, pcMailMessage );
				m_pcFolder->Unlock();

				MessageDataWindow *pcMessageWindow = new MessageDataWindow( Rect( 0, 0, 450, 500 ), "View message data", pcMailMessage );
				pcMessageWindow->CenterInWindow( this );
				pcMessageWindow->Show();
				pcMessageWindow->MakeFocus();
			}

			break;
		}

		case M_ALERT_DONE:
			break;

		case M_STATUS_UPDATE:
		{
			HandleStatusUpdate( pcMessage );
			break;
		}

		case M_STATUS_CLEAR:
		{
			String cPanel;
			if( pcMessage->FindString( "panel", &cPanel ) == EOK )
				m_pcStatusBar->SetText( cPanel, "" );
			break;
		}

		case M_CHECK_FOR_MAIL:
		{
			HandleCheckForMail();
			break;
		}

		case M_NEW_MAIL:
		{
			Mailmessage *pcNewMessage;
			if( pcMessage->FindPointer( "message", (void**)&pcNewMessage ) == EOK )
				if( AddMessage( pcNewMessage ) != EOK )
					debug( "failed to write message\n" );

			break;
		}

		case M_IMPORT_COMPLETE:
		{
			HandleImportComplete( pcMessage );
			break;
		}

		case M_NEW_MAIL_COMPLETE:
		{
			/* Do nothing */
			break;
		}

		case M_POST_FOLDER:
		{
			FolderProperties *pcProperties = m_pcFolder->GetProperties();

			ComposerWindow *pcComposer = new ComposerWindow( m_cComposerFrame, MSG_COMPWND_TITLE_NEWMESS, this, m_pcGuiSettings, m_pcIdentity );

			pcComposer->SetTo( pcProperties->GetListAddress() );

			pcComposer->Show();
			pcComposer->MakeFocus();
			break;
		}

		case M_COMPOSE_MESSAGE:
		{
			ComposerWindow *pcComposer = new ComposerWindow( m_cComposerFrame, MSG_COMPWND_TITLE_NEWMESS, this, m_pcGuiSettings, m_pcIdentity );
			pcComposer->Show();
			pcComposer->MakeFocus();
			break;
		}

		case M_COMPOSE_MESSAGE_COMPLETE:
		{
			HandleComposeMessageComplete( pcMessage );
			break;
		}

		case M_COMPOSE_REPLY:
		case M_COMPOSE_REPLY_ALL:
		case M_COMPOSE_FORWARD:
		{
			HandleCompose( pcMessage );
			break;
		}

		case M_MAIL_SENT:
		{
			HandleMailSent( pcMessage );
			break;
		}

		case M_SETTINGS_SAVE_NOW:
		{
			_SaveGui();
			m_pcGuiSettings->Save();
			break;
		}

		case M_SETTINGS_CONFIGURE:
		{
			Rect cSettingsFrame = m_pcGuiSettings->GetRect( "settings", m_cSettingsFrame );
			SettingsWindow *pcSettingsWindow = new SettingsWindow( cSettingsFrame, MSG_CFGWND_TITLE, this, m_pcGuiSettings, m_pcIdentity );
			pcSettingsWindow->Show();
			pcSettingsWindow->MakeFocus();

			break;
		}

		case M_SETTINGS_CONFIGURE_APPLY:
		{
			Identity *pcIdentity;
			if( pcMessage->FindPointer( "identity", (void**)&pcIdentity ) == EOK )
			{
				delete( m_pcIdentity );
				m_pcIdentity = pcIdentity;
			}

			/* Reload config */
			LoadIdentity( m_pcIdentity );

			break;
		}

		case M_SETTINGS_CONFIGURE_SAVE:
		{
			Identity *pcIdentity;
			if( pcMessage->FindPointer( "identity", (void**)&pcIdentity ) == EOK )
			{
				delete( m_pcIdentity );
				m_pcIdentity = pcIdentity;
				m_pcIdentity->Save( "default" );
			}

			/* Reload config */
			LoadIdentity( m_pcIdentity );

			break;
		}

		case M_APP_QUIT:
		{
			OkToQuit();
			break;
		}

		case M_APP_ABOUT:
		{
			HandleAppAbout( pcMessage );
			break;
		}

		case M_DO_IMPORT:
		{
			if( m_pcImportPlugin == NULL )
			{
				Message *pcImportMessage = new Message( M_IMPORT_FILE );
				FileRequester *pcLoadRequester = new FileRequester( FileRequester::LOAD_REQ, new Messenger( this ), "", FileRequester::NODE_FILE, false, pcImportMessage );
				pcLoadRequester->Show();
			}
			break;
		}

		case M_IMPORT_FILE:
		{
			HandleImportFile( pcMessage );
			break;
		}

		case M_IMPORT_NEW:
		{
			HandleImportNew( pcMessage );
			break;
		}

		case M_IMPORT_CREATE_FOLDER:
		{
			HandleImportCreateFolder( pcMessage );
			break;
		}

		case M_DO_EXPORT:
		{
			Message *pcExportMessage = new Message( M_EXPORT_FILE );
			FileRequester *pcSaveRequester = new FileRequester( FileRequester::SAVE_REQ, new Messenger( this ), "", FileRequester::NODE_FILE, false, pcExportMessage );
			pcSaveRequester->Show();

			break;
		}

		case M_EXPORT_FILE:
		{
			HandleExportFile( pcMessage );
			break;
		}

		case M_EMPTY_TRASH:
		{
			HandleEmptyTrash( pcMessage );
			break;
		}

		case M_EDIT_COPY:
		{
			m_pcMessageView->Copy();
			break;
		}

		case M_ENABLE_EDIT_COPY:
		{
			bool bEnable;
			if( pcMessage->FindBool( "enable", &bEnable ) == EOK )
				m_pcEditCopy->SetEnable( bEnable );
			break;
		}

		case M_GET_FOLDER_LIST:
		{
			HandleGetFolderList( pcMessage );
			break;
		}

		/* Events */
		case M_MAIL_SEND:	/* internet/Mail/Send */
		{
			HandleEventMailSend( pcMessage );
			break;
		}

		case M_MAIL_GET_COUNT:	/* internet/Mail/GetNewCount */
		{
			HandleEventMailGetCount( pcMessage );
			break;
		}

		case M_MAIL_CHECK:	/* internet/Mail/Check */
		{
			HandleEventMailCheck( pcMessage );
			break;
		}

		default:
			Window::HandleMessage( pcMessage );
	}
}

void WhisperWindow::HandleSelect( Message *pcMessage )
{
	int nSelected = m_pcMessagesList->GetLastSelected();
	if( nSelected == m_nCurrentSelection )
		return;

	if( nSelected < 0 )
	{
		m_cCurrentReference = "";
		return;
	}

	m_cCurrentReference = m_pcMessagesList->GetRow( nSelected )->GetCookie();
	debug( "%s\n", m_cCurrentReference.AsString().c_str() );

	m_pcMailMessage = new Mailmessage();
	m_pcFolder->Lock();
	m_pcFolder->Read( m_cCurrentReference, m_pcMailMessage );
	m_pcFolder->Unlock();
	m_pcMailMessage->Parse();
	m_pcMessageView->Display( m_pcMailMessage );

	if( m_pcCurrentBox->IsReadOnly() )
		EnableMessagesEdit( false );
	else
	{
		if( m_pcMailMessage->GetStatus() == STATUS_UNREAD )
		{
			m_pcMailMessage->SetStatus( STATUS_READ );
			m_pcFolder->Lock();
			m_pcFolder->ChangeStatus( m_pcMailMessage );
			UpdateUnread( m_pcFolder, m_nCurrentFolder, DEC );
			m_pcFolder->Unlock();

			/* Clear the highlighting.  You just have to trust me that SetFont( NULL ) really does
			   work as intended. */
			TreeViewMessageNode *pcNode = static_cast<TreeViewMessageNode *>( m_pcMessagesList->GetRow( nSelected ) );
			pcNode->SetFont( NULL );
			m_pcMessagesList->InvalidateRow( nSelected, 0 );
		}

		EnableMessagesEdit( true );
	}
	EnableMessagesReply( true );

	m_nCurrentSelection = nSelected;
}

void WhisperWindow::HandleFolderSelect( Message *pcMessage )
{
	int nSelected = m_pcTreeView->GetLastSelected();
	if( nSelected == m_nCurrentFolder )
		return;

	int nBox;
	folder_cookie_s *psCookie = (folder_cookie_s*)m_pcTreeView->GetRow( nSelected )->GetCookie().AsPointer();
	if( psCookie )
	{
		m_cCurrentFolderPath = psCookie->cPath;
		nBox = psCookie->nBox;
	}
	else
	{
		m_cCurrentFolderPath = "";
		nBox = -1;
	}
	m_nCurrentFolder = nSelected;
	m_nCurrentSelection = -1;
	m_cCurrentReference = "";

	if( nBox >= 0 )
	{
		m_pcCurrentBox = m_vBoxes[nBox];

		if( ScanFolder( psCookie->cPath ) != EOK )
			return;

		if( m_pcCurrentBox->IsReadOnly() )
			EnableFoldersEdit( false );
		else
			EnableFoldersEdit( true );
	}
	else
	{
		m_pcCurrentBox = NULL;

		m_pcMessagesList->Clear();
		m_pcMessageView->Clear();

		EnableFoldersEdit( false );
	}

	/* Editing or replying is only valid once a message is selected */
	EnableMessagesEdit( false );
	EnableMessagesReply( false );

	/* Select the last (most recent) message in the folder */
	int nRowCount = m_pcMessagesList->GetRowCount();
	if( nRowCount > 0 )
	{
		m_pcMessagesList->Select( nRowCount - 1, false, true );
		m_pcMessagesList->MakeVisible( nRowCount - 1, false );
	}
}

void WhisperWindow::HandleCreateFolderDialog( Message *pcMessage )
{
	os::String cFolder;
	if( pcMessage->FindString( "name", &cFolder ) == EOK )
	{
		m_pcFolder->Lock();
		status_t nError = m_pcFolder->CreateFolder( cFolder );
		m_pcFolder->Unlock();
		if( EOK != nError )
		{
			/* Pop an error Alert */
			String cError;
			if( EEXIST == nError )
				cError = String( MSG_ALERTWND_CREATEFOLDER_TEXTONE_ONE + " \"" ) + cFolder + String( "\" " ) + String( MSG_ALERTWND_CREATEFOLDER_TEXTONE_TWO );
			else
				cError = String( MSG_ALERTWND_CREATEFOLDER_TEXTTWO + " \"" ) + cFolder + String( "\"." );

			Alert *pcAlert = new Alert( MSG_ALERTWND_CREATEFOLDER, cError, Alert::ALERT_WARNING, 0x00, MSG_ALERTWND_CREATEFOLDER_OK.c_str(), NULL );
			pcAlert->CenterInWindow( this );
			pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
		}
		else
			/* Reload the folders tree */
			LoadMailboxes();
	}
}

void WhisperWindow::HandleRenameFolderDialog( Message *pcMessage )
{
	os::String cFolder;
	if( pcMessage->FindString( "name", &cFolder ) == EOK )
	{
		m_pcFolder->Lock();
		status_t nError = m_pcFolder->Rename( cFolder );
		m_pcFolder->Unlock();
		if( EOK != nError )
		{
			/* Pop an error Alert */
			String cError;
			if( EEXIST == nError )
				cError = String( MSG_ALERTWND_RENAMEFOLDER_TEXTONE_ONE + " \"" ) + cFolder + String( "\" " ) + String( MSG_ALERTWND_RENAMEFOLDER_TEXTONE_TWO );
			else
				cError = String( MSG_ALERTWND_RENAMEFOLDER_TEXTTWO + " \"" ) + cFolder + String( "\" " );

			Alert *pcAlert = new Alert( MSG_ALERTWND_RENAMEFOLDER, cError, Alert::ALERT_WARNING, 0x00, MSG_ALERTWND_RENAMEFOLDER_OK.c_str(), NULL );
			pcAlert->CenterInWindow( this );
			pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
		}
		else
			/* Reload the folders tree */
			LoadMailboxes();
	}
}

void WhisperWindow::HandleDeleteFolder( Message *pcMessage )
{
	m_pcFolder->Lock();
	FolderProperties *pcProperties = m_pcFolder->GetProperties();
	m_pcFolder->Unlock();

	/* Don't delete any of the default folders */
	if( pcProperties && pcProperties->GetType() != FL_TYPE_NORMAL )
	{
			Alert *pcAlert = new Alert( MSG_ALERTWND_DELETEFOLDER, MSG_ALERTWND_DELETEFOLDER_PERMISIONS_TEXT, Alert::ALERT_WARNING, 0x00, MSG_ALERTWND_DELETEFOLDER_OK.c_str(), NULL );
			pcAlert->CenterInWindow( this );
			pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
	}
	else
	{
		/* XXXKV: Pop a confirmation dialog before deletion */
		m_pcFolder->Lock();
		status_t nError = m_pcFolder->Delete();
		m_pcFolder->Unlock();
		if( EOK != nError )
		{
			String cError = String( MSG_ALERTWND_DELETEFOLDER_TEXT + "\n\n" ) + strerror( nError );
			Alert *pcAlert = new Alert( MSG_ALERTWND_DELETEFOLDER, cError, Alert::ALERT_WARNING, 0x00, MSG_ALERTWND_DELETEFOLDER_OK.c_str(), NULL );
			pcAlert->CenterInWindow( this );
			pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
		}
		else
		{
			/* Reload the folders tree */
			LoadMailboxes();

			/* The currently selected folder has just been deleted */
			m_pcTreeView->Select( m_nCurrentFolder - 1, true, true );
		}
	}
}

void WhisperWindow::HandlePropertiesFolder( Message *pcMessage )
{
	/* Get the current name of the selected folder */
	ListViewRow *pcRow = m_pcTreeView->GetRow( m_nCurrentFolder );
	folder_cookie_s *psCookie = (folder_cookie_s*)dynamic_cast<TreeViewStringNode*>(pcRow)->GetCookie().AsPointer();

	String cTitle;
	cTitle.Format( "%s %s", MSG_MAINWND_MAILFOLDERLIST_PROPERTIES_TITLE.c_str(), psCookie->cName.c_str() );

	/* Get current properties */
	FolderProperties *pcProperties = m_pcFolder->GetProperties();

	/* Pop properties dialog for the folder */
	PropertiesDialog *pcDialog = new PropertiesDialog( Rect( 0, 0, 300, 75 ), cTitle, pcProperties );
	pcDialog->CenterInWindow( this );
	pcDialog->Go( new Invoker( new Message( M_PROPERTIES_FOLDER_DIALOG ), this ) );
}

void WhisperWindow::HandlePropertiesFolderDialog( Message *pcMessage )
{
	FolderProperties *pcProperties;
	if( pcMessage->FindPointer( "properties", (void**)&pcProperties ) == EOK )
	{
		m_pcFolder->Lock();
		(void)m_pcFolder->SetProperties( pcProperties );
		m_pcFolder->Unlock();

		EnableFoldersEdit( true );
	}
}

void WhisperWindow::HandleCutMessage( Message *pcMessage )
{
	/* Clear out the current list of Cut/Copied files */
	std::list <Mailmessage*>::iterator i;
	for( i = m_vCopyMessages.begin(); i != m_vCopyMessages.end(); i++ )
		delete( (*i) );
	m_vCopyMessages.clear();

	int nFirst = m_pcMessagesList->GetFirstSelected();
	if( nFirst < 0 )
		return;

	/* Cut every file currently selected */
	for( int nRow = nFirst; nRow <= m_pcMessagesList->GetLastSelected(); nRow++ )
		if( m_pcMessagesList->GetRow( nRow )->IsSelected() == true )
		{
			Variant cReference = m_pcMessagesList->GetRow( nRow )->GetCookie();

			debug( "cut %s\n", cReference.AsString().c_str() );

			/* Add to list of cut messages */
			Mailmessage *pcCutMessage = new Mailmessage();

			m_pcFolder->Lock();
			m_pcFolder->Read( cReference, pcCutMessage );
			m_pcFolder->DeleteMessage( cReference );
			if( pcCutMessage->GetStatus() == STATUS_UNREAD )
			{
				pcCutMessage->SetStatus( STATUS_READ );
				UpdateUnread( m_pcFolder, m_nCurrentFolder, DEC );
			}
			m_pcFolder->Unlock();

			m_vCopyMessages.push_back( pcCutMessage );
		}

	/* Reload the list of messages */
	ScanFolder( m_cCurrentFolderPath );

	/* The currently selected message has just been deleted */
	int nSelection = m_nCurrentSelection - 1;
	if( nSelection < 0 )
	{
		nSelection = 0;
		m_nCurrentSelection = -1;
	}
	m_pcMessagesList->Select( nSelection, false, true );
	m_pcMessagesList->MakeVisible( nSelection, false );

	m_pcMessagePasteItem->SetEnable( true );
	m_pcMessagePasteCtxItem->SetEnable( true );
	m_eCopyMode = CUT;
}

void WhisperWindow::HandleCopyMessage( Message *pcMessage )
{
	/* Clear out the current list of Cut/Copied files */
	std::list <Mailmessage*>::iterator i;
	for( i = m_vCopyMessages.begin(); i != m_vCopyMessages.end(); i++ )
		delete( (*i) );
	m_vCopyMessages.clear();

	int nFirst = m_pcMessagesList->GetFirstSelected();
	if( nFirst < 0 )
		return;

	/* Copy every file currently selected */
	for( int nRow = nFirst; nRow <= m_pcMessagesList->GetLastSelected(); nRow++ )
		if( m_pcMessagesList->GetRow( nRow )->IsSelected() == true )
		{
			Variant cReference = m_pcMessagesList->GetRow( nRow )->GetCookie();

			debug( "copy %s\n", cReference.AsString().c_str() );

			/* Add to list of cut messages */
			Mailmessage *pcCopyMessage = new Mailmessage();

			m_pcFolder->Read( cReference, pcCopyMessage );
			m_vCopyMessages.push_back( pcCopyMessage );
		}

	m_pcMessagePasteItem->SetEnable( true );
	m_pcMessagePasteCtxItem->SetEnable( true );
	m_eCopyMode = COPY;
}

void WhisperWindow::HandlePasteMessage( Message *pcMessage )
{
	if( m_vCopyMessages.size() > 0 )
	{
		std::list <Mailmessage*>::iterator i;
		for( i = m_vCopyMessages.begin(); i != m_vCopyMessages.end(); i++ )
		{
			/* Parse message headers */
			(*i)->ParseHeaders();

			/* Write message to new folder */
			m_pcFolder->Lock();

			/* XXXKV: Should we handle other statuses?  Should DRAFT become UNREAD or READ? */
			if( (*i)->GetStatus() == STATUS_DRAFT )
			{
				(*i)->SetStatus( STATUS_UNREAD );
				UpdateUnread( m_pcFolder, m_nCurrentFolder, INC );
			}

			if( m_pcFolder->Write( (*i) ) != EOK )
				debug( "failed to write message\n" );

			m_pcFolder->Unlock();

			delete( (*i) );
		}
		m_vCopyMessages.clear();
	}

	/* Reload the list of messages & reselect a message*/
	ScanFolder( m_cCurrentFolderPath );

	int nSelected = m_nCurrentSelection;
	if( nSelected < 0 )
		nSelected = 0;
	m_nCurrentSelection = -1;
	m_pcMessagesList->Select( nSelected, false, true );
	m_pcMessagesList->MakeVisible( nSelected, false );

	m_pcMessagePasteItem->SetEnable( false );
	m_pcMessagePasteCtxItem->SetEnable( false );
	m_eCopyMode = NONE;
}

void WhisperWindow::HandleDeleteMessage( Message *pcMessage )
{
	bool bMessagesDeleted = false;

	int nFirst = m_pcMessagesList->GetFirstSelected();
	if( nFirst < 0 )
		return;

	Mailfolder *pcTrashFolder;
	if( FindFolderByType( &pcTrashFolder, FL_TYPE_TRASH ) != EOK )
	{
		debug( "failed to find Trash folder, can't delete anything\n" );
		return;
	}

	if( pcTrashFolder->IsValid() == false )
		return;

	for( int nRow = nFirst; nRow <= m_pcMessagesList->GetLastSelected(); nRow++ )
		if( m_pcMessagesList->GetRow( nRow )->IsSelected() == true )
		{
			Variant cReference = m_pcMessagesList->GetRow( nRow )->GetCookie();

			debug( "deleting %s\n", cReference.AsString().c_str() );

			/* Move to Trash folder */
			Mailmessage *pcDeleteMessage = new Mailmessage();

			m_pcFolder->Lock();
			m_pcFolder->Read( cReference, pcDeleteMessage );
			m_pcFolder->DeleteMessage( cReference );
			if( pcDeleteMessage->GetStatus() == STATUS_UNREAD )
			{
				pcDeleteMessage->SetStatus( STATUS_READ );
				UpdateUnread( m_pcFolder, m_nCurrentFolder, DEC );
			}
			m_pcFolder->Unlock();

			pcDeleteMessage->ParseHeaders();

			/* Write message to Trash folder */
			pcTrashFolder->Lock();
			if( pcTrashFolder->Write( pcDeleteMessage ) != EOK )
				debug( "failed to write message to Trash folder\n" );
			pcTrashFolder->Unlock();

			delete( pcDeleteMessage );

			bMessagesDeleted = true;
		}

	delete( pcTrashFolder );

	if( bMessagesDeleted = true )
	{
		/* Reload the list of messages */
		ScanFolder( m_cCurrentFolderPath );

		/* The currently selected message has just been deleted.  Select the previous message in the folder.
		   Deal with cases where the first message has been deleted (Select the "new" first message) and where
		   we may have deleted multiple messages (In which case where we think the previous message should be
		   may extend past the end of the list) */

		int nSelection = m_nCurrentSelection - 1;
		if( nSelection < 0 )
		{
			nSelection = 0;
			m_nCurrentSelection = -1;
		}
		else
			nSelection = std::min( static_cast<int>( nSelection ), static_cast<int>( m_pcMessagesList->GetRowCount() - 1 ) );

		if( nSelection >= 0 )
		{
			m_pcMessagesList->Select( nSelection, false, true );
			m_pcMessagesList->MakeVisible( nSelection, false );
		}
	}
}

void WhisperWindow::HandleFlagMessage( Message *pcMessage )
{
	int32 nFlag;
	if( pcMessage->FindInt32( "flag", &nFlag ) == EOK )
	{
		m_pcMailMessage->SetFlag( nFlag );
		m_pcFolder->Lock();
		m_pcFolder->ChangeFlag( m_pcMailMessage );
		m_pcFolder->Unlock();

		TreeViewMessageNode *pcNode = static_cast<TreeViewMessageNode *>( m_pcMessagesList->GetRow( m_nCurrentSelection ) );
		Resources cRes( get_image_id() );
		BitmapImage *pcIcon;

		switch( nFlag )
		{
			case FLAG_LOW:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_low.png" ) );
				pcNode->SetIcon( 0, pcIcon );
				break;
			}

			case FLAG_MEDIUM:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_medium.png" ) );
				pcNode->SetIcon( 0, pcIcon );
				break;
			}

			case FLAG_HIGH:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_high.png" ) );
				pcNode->SetIcon( 0, pcIcon );
				break;
			}

			case FLAG_URGENT:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_urgent.png" ) );
				pcNode->SetIcon( 0, pcIcon );
				break;
			}

			case FLAG_NONE:
			default:
			{
				pcNode->SetIcon( 0, NULL );
				break;
			}
		}
		m_pcMessagesList->InvalidateRow( m_nCurrentSelection, 0 );
	}
}

void WhisperWindow::HandleMarkMessage( Message *pcMessage )
{
	int32 nOldStatus, nNewStatus;
	if( pcMessage->FindInt32( "status", &nNewStatus ) != EOK )
		return;

	int nFirst = m_pcMessagesList->GetFirstSelected();
	if( nFirst < 0 )
		return;

	for( int nRow = nFirst; nRow <= m_pcMessagesList->GetLastSelected(); nRow++ )
		if( m_pcMessagesList->GetRow( nRow )->IsSelected() == true )
		{
			Variant cReference = m_pcMessagesList->GetRow( nRow )->GetCookie();

			Mailmessage *pcMarkMessage = new Mailmessage();

			m_pcFolder->Lock();
			m_pcFolder->Read( cReference, pcMarkMessage );
			m_pcFolder->Unlock();

			nOldStatus = pcMarkMessage->GetStatus();
			if( nOldStatus != nNewStatus )
			{
				pcMarkMessage->SetStatus( nNewStatus );

				m_pcFolder->Lock();
				m_pcFolder->ChangeStatus( pcMarkMessage );
				m_pcFolder->Unlock();

				if( nNewStatus == STATUS_UNREAD )
					UpdateUnread( m_pcFolder, m_nCurrentFolder, INC );
				else if( nNewStatus == STATUS_READ )
					UpdateUnread( m_pcFolder, m_nCurrentFolder, DEC );

				TreeViewMessageNode *pcNode = static_cast<TreeViewMessageNode *>( m_pcMessagesList->GetRow( nRow ) );
				if( nNewStatus != STATUS_READ )
				{
					Font *pcFont;
					font_properties cFontProperties;

					pcFont = new Font();
					pcFont->GetDefaultFont( DEFAULT_FONT_BOLD, &cFontProperties );
					pcFont->SetProperties( cFontProperties );

					if( nNewStatus == STATUS_DRAFT )
						pcFont->SetFlags( FPF_ITALIC );

					pcNode->SetFont( pcFont );
				}
				else
					pcNode->SetFont( NULL );

				m_pcMessagesList->InvalidateRow( nRow, 0 );
			}
		}
}

void WhisperWindow::HandleStatusUpdate( Message *pcMessage )
{
	String cPanel, cStatusMessage;
	if( pcMessage->FindString( "panel", &cPanel ) == EOK )
		if( pcMessage->FindString( "message", &cStatusMessage ) == EOK )
			m_pcStatusBar->SetText( cPanel, cStatusMessage );

	int16 nTimeout;
	if( pcMessage->FindInt16( "timeout", &nTimeout ) == EOK )
		if( nTimeout > 0 )
		{
			Message *pcClearMessage = new Message( M_STATUS_CLEAR );
			pcClearMessage->AddString( "panel", cPanel );

			/* Find the first free slot in the messages table, insert the message and schedule
			   the Timer callback for that slot */
			int nId = 0;
			while( m_apcTimerMessages[nId] != NULL && nId < MAX_TIMER_MSGS )
				nId++;
			if( nId < MAX_TIMER_MSGS )
			{
				m_apcTimerMessages[nId] = pcClearMessage;
				AddTimer( this, nId, nTimeout * 1000000 );
			}
		}
}

void WhisperWindow::HandleCheckForMail( void )
{
	/* Create a TransportWorker thread for each inbound transport */
	for( uint i = 0; i < m_vInboundTransports.size(); i++ )
	{
		char zName[11] = {0};
		sprintf( zName, "transport%i", i );
		InboundWorker *pcWorker = new InboundWorker( zName, m_vInboundTransports[i], this, zName );
		if( pcWorker != NULL )
			pcWorker->Start();
	}
}

void WhisperWindow::HandleImportComplete( Message *pcMessage )
{
	delete( m_pcImportPlugin );
	m_pcImportPlugin = NULL;

	/* Reload the list of messages & reselect a message*/
	ScanFolder( m_cCurrentFolderPath );

	int nSelected = m_nCurrentSelection;
	if( nSelected < 0 )
		nSelected = 0;
	m_nCurrentSelection = -1;
	if( m_pcMessagesList->GetRowCount() > 0 )
	{
		m_pcMessagesList->Select( nSelected, false, true );
		m_pcMessagesList->MakeVisible( nSelected, false );
	}
}

void WhisperWindow::HandleComposeMessageComplete( Message *pcMessage )
{
	/* Generate a valid RFC2822 message via. Compose() & then send */
	debug( "composition complete\n" );

	Mailmessage *pcNewMessage;
	if( pcMessage->FindPointer( "message", (void**)&pcNewMessage ) == EOK )
	{
		pcNewMessage->SetFrom( m_pcIdentity->GetFrom() );
		/* XXXKV: What if the user wants a different Reply-To for this message?
		   Add a Reply-To address field in the ComposerWindow? */
		pcNewMessage->SetReplyTo( m_pcIdentity->GetFrom() );

		if( pcNewMessage->Compose() == EOK )
		{
			debug( "Composition succeeded\n" );

			/* Write to the Outbox, then start sending everything in the Outbox
			   via. outbound transport */

			pcNewMessage->SetStatus( STATUS_DRAFT );

			Mailfolder *pcOutbox;
			if( FindFolderByType( &pcOutbox, FL_TYPE_OUTBOX ) != EOK )
				return;

			pcOutbox->Lock();
			if( pcOutbox->Write( pcNewMessage ) != EOK )
				debug( "failed to write message\n" );
			pcOutbox->Unlock();

			if( NULL != m_pcOutboundTransport )
			{
				/* Start sending everything in the Outbox */
				Transport *pcAuth = NULL;
				if( m_nAuthTransport >= 0 )
					pcAuth = m_vInboundTransports[ m_nAuthTransport ];

				OutboundWorker *pcWorker = new OutboundWorker( pcOutbox, "outbound", m_pcOutboundTransport, m_pcIdentity->GetAddress(), pcAuth, this, "general_messages" );
				if( pcWorker != NULL )
					pcWorker->Start();
			}

			/* If this was a Reply or Forward, update the original messages status */
			int16 nReplyMode;
			if( pcMessage->FindInt16( "reply_mode", &nReplyMode ) == EOK )
			{
				/* We need to know the original Mailbox # and folder name, as well as the original message that was replied to.  This
				   information was provided when the Composer window was created */
				Mailmessage *pcReplyMessage;
				int16 nReplyMailbox;
				String cReplyFolder;

				if( pcMessage->FindPointer( "reply_message", (void**)&pcReplyMessage ) == EOK &&
					pcMessage->FindInt16( "reply_mailbox", &nReplyMailbox ) == EOK &&
					pcMessage->FindString( "reply_folder", &cReplyFolder ) == EOK )
				{
					Mailfolder *pcReplyFolder;
					if( FindFolder( &pcReplyFolder, cReplyFolder, nReplyMailbox ) == EOK )
					{
						pcReplyFolder->Lock();
						pcReplyFolder->ChangeStatus( pcReplyMessage );
						pcReplyFolder->Unlock();

						/* XXXKV: How can we add an icon? */
					}

					delete( pcReplyMessage );
				}
			}
		}
	}
}

void WhisperWindow::HandleCompose( Message *pcMessage )
{
	if( m_nCurrentSelection < 0 )
		return;

	/* Snarf the current selected message */
	Mailmessage *pcReplyMessage = new Mailmessage();
	m_pcFolder->Read( m_cCurrentReference, pcReplyMessage );

	os::String cTitle;
	int nMode = COMPOSE_NEW;

	switch( pcMessage->GetCode() )
	{
		case M_COMPOSE_REPLY:
		{
			cTitle = MSG_COMPWND_TITLE_REPLY;
			nMode = COMPOSE_REPLY;
			break;
		}

		case M_COMPOSE_REPLY_ALL:
		{
			cTitle = MSG_COMPWND_TITLE_REPLYALL;
			nMode = COMPOSE_REPLY_ALL;
			break;
		}

		case M_COMPOSE_FORWARD:
		{
			cTitle = MSG_COMPWND_TITLE_FORWARD;
			nMode = COMPOSE_FORWARD;
			break;
		}
	}

	/* Get the current name of the selected folder */
	ListViewRow *pcRow = m_pcTreeView->GetRow( m_nCurrentFolder );
	folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();

	ComposerWindow *pcComposer = new ComposerWindow( m_cComposerFrame, cTitle, this, m_pcGuiSettings, m_pcIdentity, nMode, pcReplyMessage, psCookie->nBox, psCookie->cName );
	pcComposer->Show();
	pcComposer->MakeFocus();
}

void WhisperWindow::HandleMailSent( Message *pcMessage )
{
	Variant cReference;
	if( pcMessage->FindVariant( "reference", &cReference ) == EOK )
	{
		debug( "message %s sent\n", cReference.AsString().c_str() );

		/* Move from Outbox to Sent */
		Mailfolder *pcOutbox, *pcSent;
		if( FindFolderByType( &pcOutbox, FL_TYPE_OUTBOX ) != EOK )
			return;
		if( FindFolderByType( &pcSent, FL_TYPE_SENT ) != EOK )
			return;

		Mailmessage *pcMailMessage = new Mailmessage();
		pcOutbox->Lock();
		pcOutbox->Read( cReference, pcMailMessage );
		pcOutbox->DeleteMessage( cReference );
		pcOutbox->Unlock();
		delete( pcOutbox );

		pcMailMessage->SetStatus( STATUS_READ );

		pcSent->Lock();
		pcSent->Write( pcMailMessage );
		pcSent->Unlock();
		delete( pcSent );
	}
}

void WhisperWindow::HandleAppAbout( Message *pcMessage )
{
	String cTitle, cAbout;

	cTitle = String( MSG_ABOUTWND_TITLE + " " ) + g_cVersion;

	cAbout  = String( MSG_ABOUTWND_TEXT_LINEONE + "\n" );
	cAbout += String( MSG_ABOUTWND_TEXT_VERSION + " " ) + g_cVersion + String( "\n\n" );
	cAbout += String( MSG_ABOUTWND_TEXT_WEBSITEONE + " http://www.syllable.org " + MSG_ABOUTWND_TEXT_WEBSITETWO + "\n\n" );
	cAbout += String( MSG_ABOUTWND_TEXT_COPYRIGHTONE );
	cAbout += String( " http://www.gnu.org/licenses/licenses.html\n" );
	cAbout += String( MSG_ABOUTWND_TEXT_COPYRIGHTTWO );

	Resources cRes( get_image_id() );
	BitmapImage *pcAboutIcon = new BitmapImage();
	pcAboutIcon->Load( cRes.GetResourceStream( "icon48x48.png" ) );

	Alert *pcAlert = new Alert( cTitle, cAbout, pcAboutIcon->LockBitmap(), 0x00, MSG_ABOUTWND_CLOSE.c_str(), NULL );
	pcAlert->CenterInWindow( this );
	pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
}

void WhisperWindow::HandleImportFile( Message *pcMessage )
{
	const char* pzFilename;

	FileRequester *pcRequester;
	if( pcMessage->FindPointer( "source", (void**)&pcRequester ) == EOK )
		pcRequester->Close();

	if( pcMessage->FindString( "file/path", &pzFilename ) == EOK )
	{
		/* Get the current name of the selected folder and use this as the target */
		ListViewRow *pcRow = m_pcTreeView->GetRow( m_nCurrentFolder );
		folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();

		Mailfolder *pcFolder;
		if( FindFolder( &pcFolder, psCookie->cName ) != EOK )
		{
			debug( "failed to find import target folder\n" );
			return;
		}
		String cTarget = pcFolder->GetPath();
		IXFactory *pcFactory = IXFactory::GetFactory();
		m_pcImportPlugin = pcFactory->FindPlugin( pzFilename );
		if( m_pcImportPlugin != NULL )
		{
			status_t nError = m_pcImportPlugin->Import( this, cTarget, pzFilename );
			if( nError != EOK )
			{
				String cError = String( MSG_ALERTWND_ERRIMPORTFILE_TEXT + " \"" ) + pzFilename + String( "\"." );

				Alert *pcAlert = new Alert( MSG_ALERTWND_ERRIMPORTFILE, cError, Alert::ALERT_WARNING, 0x00, MSG_ALERTWND_ERRIMPORTFILE_OK.c_str(), NULL );
				pcAlert->CenterInWindow( this );
				pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
			}
		}
		else
		{
			String cError = String( MSG_ALERTWND_ERRIMPORTBOX_TEXT + " \"" ) + pzFilename + String( "\"." );

			Alert *pcAlert = new Alert( MSG_ALERTWND_ERRIMPORTBOX, cError, Alert::ALERT_INFO, 0x00, MSG_ALERTWND_ERRIMPORTBOX_OK.c_str(), NULL );
			pcAlert->CenterInWindow( this );
			pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
		}
	}
}

void WhisperWindow::HandleImportNew( Message *pcMessage )
{
	Mailmessage *pcImportMessage;
	if( pcMessage->FindPointer( "message", (void**)&pcImportMessage ) != EOK )
		return;

	String cFolder;
	if( pcMessage->FindString( "folder", &cFolder ) != EOK )
	{
		delete( pcImportMessage );
		return;
	}

	/* Parse message headers */
	//pcImportMessage->ParseHeaders();
	/* XXXKV: Changed to Parse() so we get an attachment count */
	pcImportMessage->Parse();

	Mailfolder *pcFolder;
	if( FindFolderByPath( &pcFolder, cFolder ) != EOK )
	{
		delete( pcImportMessage );
		return;
	}

	if( pcFolder->IsValid() == true )
	{
		pcImportMessage->SetStatus( STATUS_READ );

		/* Write message to folder */
		pcFolder->Lock();
		if( pcFolder->Write( pcImportMessage ) != EOK )
			debug( "failed to write message\n" );
		pcFolder->Unlock();
	}

	delete( pcFolder );
	delete( pcImportMessage );
}

void WhisperWindow::HandleImportCreateFolder( Message *pcMessage )
{
	os::String cParent, cFolder;
	if( pcMessage->FindString( "parent", &cParent ) != EOK )
		return;

	if( pcMessage->FindString( "name", &cFolder ) != EOK )
		return;

	Mailfolder *pcFolder;
	if( FindFolderByPath( &pcFolder, cParent ) != EOK )
		return;

	if( pcFolder->IsValid() == true )
	{
		pcFolder->Lock();
		pcFolder->CreateFolder( cFolder );
		pcFolder->Unlock();
	}
	delete( pcFolder );

	/* Reload the folders tree */
	LoadMailboxes();
}

void WhisperWindow::HandleExportFile( Message *pcMessage )
{
	const char* pzFilename;

	FileRequester *pcRequester;
	if( pcMessage->FindPointer( "source", (void**)&pcRequester ) == EOK )
		pcRequester->Close();

	if( pcMessage->FindString( "file/path", &pzFilename ) == EOK )
	{
		/* Get the current name of the selected folder and use this as the source */
		ListViewRow *pcRow = m_pcTreeView->GetRow( m_nCurrentFolder );
		folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();

		Mailfolder *pcFolder;
		if( FindFolder( &pcFolder, psCookie->cName ) != EOK )
		{
			debug( "failed to find export source folder\n" );
			return;
		}

		IXFactory *pcFactory = IXFactory::GetFactory();
		IXPlugin *pcExportPlugin = pcFactory->FindPluginByIdentifier( "UNIX mbox" );
		if( pcExportPlugin != NULL )
		{
			status_t nError = pcExportPlugin->Export( this, pcFolder, pzFilename );
			if( nError != EOK )
			{
				String cError = String( MSG_ALERTWND_ERREXPORT_TEXT + " \"" ) + pzFilename + String( "\"." );

				Alert *pcAlert = new Alert( MSG_ALERTWND_ERREXPORT, cError, Alert::ALERT_WARNING, 0x00, MSG_ALERTWND_ERREXPORT_OK.c_str(), NULL );
				pcAlert->CenterInWindow( this );
				pcAlert->Go( new Invoker( new Message( M_ALERT_DONE ) ) );
			}
			delete( pcExportPlugin );
		}
	}
}

void WhisperWindow::HandleEmptyTrash( Message *pcMessage )
{
	Mailfolder *pcTrashFolder;
	if( FindFolderByType( &pcTrashFolder, FL_TYPE_TRASH ) != EOK )
	{
		debug( "failed to find Trash folder, can't empty it\n" );
		return;
	}

	if( pcTrashFolder->IsValid() == false )
		return;

	pcTrashFolder->Lock();

	Mailsummery cSummery;
	status_t nError;
	while( ( nError = pcTrashFolder->GetNextEntry( &cSummery ) ) != ENOENT )
	{
		if( EISDIR == nError )
			continue;

		/* Delete message from Trash folder */
		pcTrashFolder->DeleteMessage( cSummery.cReference );
	}

	pcTrashFolder->Unlock();
	delete( pcTrashFolder );

	/* Refresh if the user is currently looking at the Trash folder */
	ListViewRow *pcRow = m_pcTreeView->GetRow( m_nCurrentFolder );
	folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();

	if( psCookie->cName == "Trash" )
	{
		/* Reload the list of messages */
		ScanFolder( psCookie->cPath );

		/* The currently selected message has just been deleted */
		int nSelection = m_nCurrentSelection - 1;
		if( nSelection < 0 )
		{
			nSelection = 0;
			m_nCurrentSelection = -1;
		}
		m_pcMessagesList->Select( nSelection, false, true );
		m_pcMessagesList->MakeVisible( nSelection, false );
	}
}

void WhisperWindow::HandleGetFolderList( Message *pcMessage )
{
	int nCount, nRow;
	Message *pcReply = new Message( M_GET_FOLDER_LIST );

	nCount = m_pcTreeView->GetRowCount();
	for( nRow = 0; nRow < nCount; nRow++ )
	{
		ListViewRow *pcRow = m_pcTreeView->GetRow( nRow );
		folder_cookie_s *psCookie = (folder_cookie_s*)dynamic_cast<TreeViewStringNode*>(pcRow)->GetCookie().AsPointer();

		if( psCookie == NULL )
			continue;

		const int nBox = psCookie->nBox;

		if( nBox < 0 )
			continue;

		Mailbox *pcBox = m_vBoxes[nBox];
		Mailfolder *pcFolder = pcBox->OpenFolder( psCookie->cPath );
		FolderProperties *pcProperties = pcFolder->GetProperties();

		pcReply->AddString( "name", psCookie->cName );
		pcReply->AddInt64( "type", pcProperties->GetType() );
	}

	pcMessage->SendReply( pcReply );
}

void WhisperWindow::HandleEventMailSend( Message *pcMessage )
{
	String cSubject;
	String cTo, cAllTo, cCc, cAllCc, cBcc, cAllBcc;
	String cBody, cAttachment;
	int nIndex;

	debug( "internet/Mail/Send event received.\n" );

	ComposerWindow *pcComposer = new ComposerWindow( m_cComposerFrame, MSG_COMPWND_TITLE_NEWMESS, this, m_pcGuiSettings, m_pcIdentity );

	/* Optional parameters */

	/* Subject: */
	if( pcMessage->FindString( "subject", &cSubject ) == EOK )
		pcComposer->SetSubject( cSubject );

	/* To: */
	nIndex = 0;
	while( pcMessage->FindString( "to", &cTo, nIndex++ ) == EOK )
	{
		if( cAllTo != "" )
			cAllTo += String( ", " );
		cAllTo += cTo;
	}
	if( cAllTo != "" )
		pcComposer->SetTo( cAllTo );

	/* Cc: */
	nIndex = 0;
	while( pcMessage->FindString( "cc", &cCc, nIndex++ ) == EOK )
	{
		if( cAllCc != "" )
			cAllCc += String( ", " );
		cAllCc += cCc;
	}
	if( cAllCc != "" )
		pcComposer->SetCc( cAllCc );

	/* Bcc: */
	nIndex = 0;
	while( pcMessage->FindString( "bcc", &cBcc, nIndex++ ) == EOK )
	{
		if( cAllBcc != "" )
			cAllBcc += String( ", " );
		cAllBcc += cBcc;
	}
	if( cAllBcc != "" )
		pcComposer->SetBcc( cAllBcc );

	/* Message body text */
	if( pcMessage->FindString( "message", &cBody ) == EOK )
		pcComposer->SetBody( cBody );

	/* Attachment filenames */
	nIndex = 0;
	while( pcMessage->FindString( "attachment", &cAttachment, nIndex++ ) == EOK )
		pcComposer->AddAttachment( cAttachment );

	/* Show the composer window */
	pcComposer->Show();
	pcComposer->MakeFocus();
}

void WhisperWindow::HandleEventMailGetCount( Message *pcMessage )
{
	int32 nReplyCode, nCount = 0;

	debug( "internet/Mail/GetNewCount event received.\n" );

	if( !pcMessage->IsSourceWaiting() )
		return;

	/* Poll each inbound transport for a count. Note that this is a blocking operation! */
	for( uint i = 0; i < m_vInboundTransports.size(); i++ )
		nCount += m_vInboundTransports[i]->GetMessageCount();

	/* Reply with the total POP mail count */
	pcMessage->FindInt32( "reply_code", &nReplyCode );

	Message cReply( nReplyCode );
	cReply.AddInt32( "count", nCount );

	pcMessage->SendReply( &cReply );
}

void WhisperWindow::HandleEventMailCheck( Message *pcMessage )
{
	debug( "internet/Mail/Check event received.\n" );
	HandleCheckForMail();
}

