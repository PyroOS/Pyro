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

#include <gui/filerequester.h>
#include <storage/file.h>
#include <util/regexp.h>
#include <util/resources.h>

#include <composer_window.h>
#include <messages.h>
#include <qp_codec.h>
#include <base64_codec.h>
#include <resources/Whisper.h>

#include <debug.h>

AddressBar::AddressBar( const Rect cFrame ) : View( cFrame, "whisper_address_bar", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT )
{
	Rect cAddressFrame;

	_GetPosition( cAddressFrame, 0 );
	m_apcAddress[ ADDR_TO ] = new InputView( cAddressFrame, MSG_COMPWND_FIELDS_TO, 50, true, new Message( M_ADDR_LOOKUP_TO ) );
	m_abShown[ ADDR_TO ] = true;
	AddChild( m_apcAddress[ ADDR_TO ] );

	_GetPosition( cAddressFrame, 1 );
	m_apcAddress[ ADDR_CC ] = new InputView( cAddressFrame, MSG_COMPWND_FIELDS_CC, 50, true, new Message( M_ADDR_LOOKUP_CC ) );
	m_abShown[ ADDR_CC ] = true;
	AddChild( m_apcAddress[ ADDR_CC ] );

	_GetPosition( cAddressFrame, 2 );
	m_apcAddress[ ADDR_BCC ] = new InputView( cAddressFrame, MSG_COMPWND_FIELDS_BCC, 50, true, new Message( M_ADDR_LOOKUP_BCC ) );
	m_abShown[ ADDR_BCC ] = true;
	AddChild( m_apcAddress[ ADDR_BCC ] );

	_GetPosition( cAddressFrame, 3 );
	m_apcAddress[ ADDR_SUBJECT ] = new InputView( cAddressFrame, MSG_COMPWND_FIELDS_SUBJECT );
	m_abShown[ ADDR_SUBJECT ] = true;

	AddChild( m_apcAddress[ ADDR_SUBJECT ] );
}

AddressBar::~AddressBar()
{
	for( int nIndex = ADDR_TO; nIndex <= ADDR_SUBJECT; nIndex++ )
	{
		if( m_abShown[ nIndex ] )
			RemoveChild( m_apcAddress[ nIndex ] );
		delete( m_apcAddress[ nIndex ] );
	}
}

Point AddressBar::GetPreferredSize( bool bLargest )
{
	m_vHeight = 10;
	for( int nId = ADDR_TO; nId <= ADDR_SUBJECT; nId++ )
	{
		if( m_abShown[ nId ] == false )
			continue;
		m_vHeight += 25;
	}

	return Point( GetBounds().Width(), m_vHeight );
}

status_t AddressBar::ShowAddress( int nId, bool bShow )
{
	/* Can't hide the Subject */
	if( nId > ADDR_BCC )
		return EINVAL;

	/* Hide or show the AddressView as required */
	if( m_abShown[ nId ] && bShow == false )
	{
		RemoveChild( m_apcAddress[ nId ] );
		m_abShown[ nId ] = false;
	}
	else if( m_abShown[nId] == false && bShow )
	{
		AddChild( m_apcAddress[ nId ] );
		m_abShown[ nId ] = true;
	}

	/* Move everything so there are no gaps */
	int nIndex = 0;
	Rect cAddressFrame;

	for( int nId = ADDR_TO; nId <= ADDR_SUBJECT; nId++ )
	{
		if( m_abShown[ nId ] == false )
			continue;

		/* XXXKV: The RemoveChild()/AddChild() pair may look a little odd but without it the Views would
		   be re-drawn oddly after one was show. */

		RemoveChild( m_apcAddress[ nId ] );
		_GetPosition( cAddressFrame, nIndex++ );
		m_apcAddress[ nId ]->SetFrame( cAddressFrame );
		AddChild( m_apcAddress[ nId ] );
	}

	return EOK;
}

status_t AddressBar::GetText( int nId, String &cText )
{
	if( nId < ADDR_TO || nId > ADDR_SUBJECT )
		return EINVAL;

	cText = m_apcAddress[ nId ]->GetText();
	return EOK;
}

status_t AddressBar::SetText( int nId, String cText )
{
	if( nId < ADDR_TO || nId > ADDR_SUBJECT )
		return EINVAL;

	m_apcAddress[ nId ]->SetText( cText );
	return EOK;
}

status_t AddressBar::SetFocus( int nId )
{
	if( nId < ADDR_TO || nId > ADDR_SUBJECT )
		return EINVAL;

	m_apcAddress[ nId ]->SetFocus();
	return EOK;
}

void AddressBar::_GetPosition( Rect &cFrame, int nIndex )
{
	cFrame.left = 5;
	cFrame.top = 5 + ( nIndex * 25 ); 
	cFrame.right = GetFrame().Width();
	cFrame.bottom = 45 + ( nIndex * 25 );
}

ComposerWindow::ComposerWindow( const Rect &cFrame, const String cTitle, Handler *pcParent, Settings *pcGuiSettings, Identity *pcIdentity, int nMode, Mailmessage *pcEditMessage, int nMailbox, String cFolder ) : Window( cFrame, "whisper_composer", cTitle )
{
	m_pcParent = pcParent;
	m_pcGuiSettings = pcGuiSettings;
	m_pcIdentity = 	pcIdentity;
	m_nMode = nMode;
	m_pcEditMessage = pcEditMessage;
	m_nMailbox = nMailbox;
	m_cFolder = cFolder;

	SetSizeLimits( Point( 575, 325 ), Point( 4096, 4096 ) );

	bool bShowTo, bShowCc, bShowBcc;

	bShowTo = m_pcGuiSettings->GetBool( "composer_show_to", true );
	bShowCc = m_pcGuiSettings->GetBool( "composer_show_cc", true );
	bShowBcc = m_pcGuiSettings->GetBool( "composer_show_bcc", true );

	Rect cBounds = GetBounds();

	Rect cMenuFrame = cBounds;
	cMenuFrame.bottom = 18;

	m_pcMenuBar = new Menu( cMenuFrame, "whisper_composer_menu", ITEMS_IN_ROW );

	m_pcMessageMenu = new Menu( Rect(), MSG_COMPWND_MENU_MESSAGE, ITEMS_IN_COLUMN );
	m_pcMessageMenu->AddItem( MSG_COMPWND_MENU_MESSAGE_SEND, new Message( M_COMPOSER_MESSAGE_SEND ) );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageMenu->AddItem( MSG_COMPWND_MENU_MESSAGE_SAVE, new Message( M_COMPOSER_MESSAGE_SAVE ) );
	m_pcMessageMenu->AddItem( MSG_COMPWND_MENU_MESSAGE_CLOSE, new Message( M_COMPOSER_MESSAGE_CLOSE ) );

	m_pcMenuBar->AddItem( m_pcMessageMenu );

	m_pcEditMenu = new Menu( Rect(), MSG_COMPWND_MENU_EDIT, ITEMS_IN_COLUMN );
	m_pcEditMenu->AddItem( MSG_COMPWND_MENU_EDIT_CUT, new Message( M_COMPOSER_EDIT_CUT ) );
	m_pcEditMenu->AddItem( MSG_COMPWND_MENU_EDIT_COPY, new Message( M_COMPOSER_EDIT_COPY ) );
	m_pcEditMenu->AddItem( MSG_COMPWND_MENU_EDIT_PASTE, new Message( M_COMPOSER_EDIT_PASTE ) );

	m_pcMenuBar->AddItem( m_pcEditMenu );

	m_pcViewMenu = new Menu( Rect(), MSG_COMPWND_MENU_VIEW, ITEMS_IN_COLUMN );

	m_pcViewMenuTo = new CheckMenu( MSG_COMPWND_MENU_VIEW_TO, new Message( M_COMPOSER_VIEW_TO ), bShowTo );
	m_pcViewMenu->AddItem( m_pcViewMenuTo );
	m_pcViewMenuCc = new CheckMenu(  MSG_COMPWND_MENU_VIEW_CC, new Message( M_COMPOSER_VIEW_CC ), bShowCc );
	m_pcViewMenu->AddItem( m_pcViewMenuCc );
	m_pcViewMenuBcc = new CheckMenu(  MSG_COMPWND_MENU_VIEW_BCC, new Message( M_COMPOSER_VIEW_BCC ), bShowBcc );
	m_pcViewMenu->AddItem( m_pcViewMenuBcc );

	m_pcMenuBar->AddItem( m_pcViewMenu );

	m_pcInsertMenu =  new Menu( Rect(), MSG_COMPWND_MENU_INSERT, ITEMS_IN_COLUMN );
	m_pcInsertSignatureMenu = new Menu( Rect(), MSG_COMPWND_MENU_INSERT_SIG, ITEMS_IN_COLUMN );
	for( int nIndex = 0;; nIndex++ )
	{
		String cName, cText; 

		if( m_pcIdentity->GetSignature( cName, cText, nIndex ) != EOK )
			break;

		/* XXXKV: We could handle this two ways: put the signature text directly into the Message, or put
		   the index in the message and retrieve the signature text again later.  I've chosen the later
		   on the basis that a signature could be fairly large and may not get used anyway, especially
		   if the user has mutliple signatures configured. */

		Message *pcSignature = new Message( M_COMPOSER_INSERT_SIG );
		pcSignature->AddInt32( "index", nIndex );

		m_pcInsertSignatureMenu->AddItem( cName, pcSignature );
	}
	m_pcInsertMenu->AddItem( m_pcInsertSignatureMenu );
	m_pcInsertMenu->AddItem( new MenuSeparator() );
	m_pcInsertMenu->AddItem( MSG_COMPWND_MENU_INSERT_FILE, new Message( M_COMPOSER_INSERT_FILE ) );

	m_pcMenuBar->AddItem( m_pcInsertMenu );

	m_vMenuHeight = m_pcMenuBar->GetPreferredSize( true ).y - 1.0f;
	cMenuFrame.bottom = m_vMenuHeight;
	m_pcMenuBar->SetFrame( cMenuFrame );
	m_pcMenuBar->SetTargetForItems( this );
	AddChild( m_pcMenuBar );

	cBounds.top = m_vMenuHeight + 1;

	/* Toolbar */
	m_vToolBarHeight = 48;

	Rect cToolBarFrame = cBounds;
	cToolBarFrame.bottom = cToolBarFrame.top + m_vToolBarHeight;

	m_pcToolBar = new ToolBar( cToolBarFrame, "Compose", CF_FOLLOW_LEFT | CF_FOLLOW_TOP | CF_FOLLOW_RIGHT );

	Resources cRes( get_image_id() );
	BitmapImage *pcToolBarIcon;

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "compose_send.png" ) );
	m_pcToolBar->AddButton( "send", MSG_COMPWND_MENUBUTTONS_SEND, pcToolBarIcon, new Message( M_COMPOSER_MESSAGE_SEND ) );

	m_pcToolBar->AddSeparator( "" );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "compose_cut.png" ) );
	m_pcToolBar->AddButton( "edit_cut", MSG_COMPWND_MENUBUTTONS_CUT, pcToolBarIcon, new Message( M_COMPOSER_EDIT_CUT ) );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "compose_copy.png" ) );
	m_pcToolBar->AddButton( "edit_copy", MSG_COMPWND_MENUBUTTONS_COPY, pcToolBarIcon, new Message( M_COMPOSER_EDIT_COPY ) );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "compose_paste.png" ) );
	m_pcToolBar->AddButton( "edit_paste", MSG_COMPWND_MENUBUTTONS_PASTE, pcToolBarIcon, new Message( M_COMPOSER_EDIT_PASTE ) );

	m_pcToolBar->AddSeparator( "" );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "compose_attach.png" ) );
	m_pcToolBar->AddButton( "insert_file", MSG_COMPWND_MENUBUTTONS_INSERT, pcToolBarIcon, new Message( M_COMPOSER_INSERT_FILE ) );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "compose_remove.png" ) );
	m_pcRemoveButton = new ImageButton( Rect(), "remove_file", MSG_COMPWND_MENUBUTTONS_REMOVE, new Message( M_COMPOSER_REMOVE_FILE ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcRemoveButton, ToolBar::TB_FIXED_WIDTH );

	/* Disable the Remove button until at least one file is attached */
	m_pcRemoveButton->SetEnable( false );

	AddChild( m_pcToolBar );

	cBounds.top = cToolBarFrame.bottom + 1;

	/* The "AddressBar" which contains the various input fields for the recipients, subject etc. */
	Rect cChildFrame = cBounds;
	cChildFrame.bottom = cChildFrame.top + 110;

	m_pcAddressBar = new AddressBar( cChildFrame );
	AddChild( m_pcAddressBar );
	Point cAddressBarSize = m_pcAddressBar->GetPreferredSize( true );

	/* A TextView for the message text */
	cChildFrame.top = cChildFrame.bottom + 1;
	cChildFrame.bottom = cBounds.bottom;

	m_pcMessageText = new TextView( cChildFrame, "whisper_message_text", "", CF_FOLLOW_ALL );
	m_pcMessageText->SetMultiLine( true );
	AddChild( m_pcMessageText );
	m_pcMessageText->SetTabOrder( NEXT_TAB_ORDER );

	/* The attachments are listed at the bottom of the Window.  This ListView is only visible if
	   at least one file is attached */
	cChildFrame.top = cChildFrame.bottom - 100;

	m_pcAttachments = new ListView( cChildFrame, "whisper_attachments", ListView::F_NO_AUTO_SORT, CF_FOLLOW_LEFT | CF_FOLLOW_BOTTOM | CF_FOLLOW_RIGHT );
	m_pcAttachments->InsertColumn( MSG_COMPWND_ATTACHMENT_FILENAME.c_str(), (int)cChildFrame.Width() / 2 );
	m_pcAttachments->InsertColumn( MSG_COMPWND_ATTACHMENT_FILESIZE.c_str(), (int)cChildFrame.Width() / 2 );
	m_bAttachmentsShown = false;

	m_pcAttachmentsMenu = new Menu( Rect(), "Attachments", ITEMS_IN_COLUMN );
	m_pcAttachmentsMenu->AddItem( MSG_COMPWND_ATTACHMENT_CONMENU_ADD, new Message( M_COMPOSER_INSERT_FILE ) );
	m_pcAttachmentsMenu->AddItem( new MenuSeparator() );
	m_pcAttachmentsMenu->AddItem( MSG_COMPWND_ATTACHMENT_CONMENU_REMOVE, new Message( M_COMPOSER_REMOVE_FILE ) );

	m_pcAttachmentsMenu->SetTargetForItems( this );
	m_pcAttachments->SetContextMenu( m_pcAttachmentsMenu );

	/* Fill in any pre-provided details */
	if( NULL != m_pcEditMessage )
		_SetupReply();

	/* Honour GUI settings */
	m_pcAddressBar->ShowAddress( ADDR_TO, bShowTo );
	m_pcAddressBar->ShowAddress( ADDR_CC, bShowCc );
	m_pcAddressBar->ShowAddress( ADDR_BCC, bShowBcc );
	_ResizeTextView();

	if( bShowTo )
		m_pcAddressBar->SetFocus( ADDR_TO );
	else if( bShowCc )
		m_pcAddressBar->SetFocus( ADDR_CC );
	else if( bShowBcc )
		m_pcAddressBar->SetFocus( ADDR_BCC );
	else
		m_pcAddressBar->SetFocus( ADDR_SUBJECT );

	/* Set a window icon */
	BitmapImage *pcWindowIcon = new BitmapImage();
	String cIconName = "";
	switch( m_nMode )
	{
		case COMPOSE_NEW:
		{
			cIconName = "new.png";
			break;
		}
		case COMPOSE_REPLY:
		{
			cIconName = "reply.png";
			break;
		}

		case COMPOSE_REPLY_ALL:
		{
			cIconName = "reply_all.png";
			break;
		}

		case COMPOSE_FORWARD:
		{
			cIconName = "forward.png";
			break;
		}
	}
	pcWindowIcon->Load( cRes.GetResourceStream( cIconName ) );
	SetIcon( pcWindowIcon->LockBitmap() );
	delete( pcWindowIcon );

	/* Add some shortcuts */
	/* XXXKV: This doesn't work, even when the TextView isn't swallowing the keypress */
	AddShortcut( ShortcutKey( VK_RETURN, QUAL_CTRL ), new Message( M_COMPOSER_MESSAGE_SEND ) );	/* Send */
}

ComposerWindow::~ComposerWindow()
{
	RemoveChild( m_pcMenuBar );
	delete( m_pcMenuBar );

	RemoveChild( m_pcToolBar );
	delete( m_pcToolBar );

	RemoveChild( m_pcAddressBar );
	delete( m_pcAddressBar );

	RemoveChild( m_pcMessageText );
	delete( m_pcMessageText );

	if( m_bAttachmentsShown )
		RemoveChild( m_pcAttachments );
	delete( m_pcAttachments );
}

void ComposerWindow::HandleMessage( Message *pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_ADDR_LOOKUP_TO:
		{
			break;
		}

		case M_ADDR_LOOKUP_CC:
		{
			break;
		}

		case M_ADDR_LOOKUP_BCC:
		{
			break;
		}

		case M_COMPOSER_MESSAGE_SEND:
		{
			/* Create a skeletal Mailmessage with the information available */
			Mailmessage *pcMailMessage = new Mailmessage();

			String cText;
			if( m_pcAddressBar->GetText( ADDR_TO, cText ) == EOK )
				pcMailMessage->SetTo( cText );

			if( m_pcAddressBar->GetText( ADDR_CC, cText ) == EOK )
				pcMailMessage->SetCc( cText );

			if( m_pcAddressBar->GetText( ADDR_BCC, cText ) == EOK )
				pcMailMessage->SetBcc( cText );

			if( m_pcAddressBar->GetText( ADDR_SUBJECT, cText ) == EOK )
				pcMailMessage->SetSubject( cText );

			/* The data being inserted into the Mailmessage class is not a valid RFC2822 email at this
			   point.  It is simply the raw message that Mailmessage will encode later */
			cText = m_pcMessageText->GetValue().AsString();
			pcMailMessage->SetData( cText.c_str(), cText.Length() );

			/* Validate the addresses */
			if( pcMailMessage->Validate() == false )
			{
				/* Get the invalid list so we can inform the user */
				std::list<os::String> vInvalid;
				pcMailMessage->GetRecipiants( vInvalid, false );

				/* XXXKV: Display a warning dialog */
				std::list<os::String>::iterator i;
				for( i = vInvalid.begin(); i != vInvalid.end(); i++ )
					debug( "%s\n", (*i).c_str() );
			}

			/* Add attachments */
			for( uint nRow = 0; nRow < m_pcAttachments->GetRowCount(); nRow++ )
			{
				ListViewStringRow *pcRow = static_cast<ListViewStringRow*>( m_pcAttachments->GetRow( nRow ) );
				pcMailMessage->Attach( pcRow->GetString( 0 ) );
			}

			Message *pcNewMessage = new Message( M_COMPOSE_MESSAGE_COMPLETE );
			pcNewMessage->AddPointer( "message", (void*)pcMailMessage );

			/* If we need to, set the reply information */
			if( COMPOSE_NEW != m_nMode )
			{
				if( COMPOSE_REPLY == m_nMode || COMPOSE_REPLY_ALL == m_nMode )
				{
					pcMailMessage->SetInReplyTo( m_pcEditMessage->GetId() );
					m_pcEditMessage->SetStatus( STATUS_REPLIED );
				}
				else
					m_pcEditMessage->SetStatus( STATUS_FORWARDED );

				pcNewMessage->AddInt16( "reply_mode", m_nMode );
				pcNewMessage->AddPointer( "reply_message", (void*)m_pcEditMessage );
				pcNewMessage->AddInt16( "reply_mailbox", m_nMailbox );
				pcNewMessage->AddString( "reply_folder", m_cFolder );
			}

			/* Tell the application to send this message */
			Messenger cMessenger( m_pcParent );
			cMessenger.SendMessage( pcNewMessage );

			/* Close this window */
			_SaveGui();
			Close();

			break;
		}

		case M_COMPOSER_MESSAGE_SAVE:
		{
			break;
		}

		case M_COMPOSER_MESSAGE_CLOSE:
		{
			_SaveGui();
			Close();
			break;
		}

		case M_COMPOSER_EDIT_CUT:
		{
			m_pcMessageText->Cut( false );
			break;
		}

		case M_COMPOSER_EDIT_COPY:
		{
			m_pcMessageText->Copy();
			break;
		}

		case M_COMPOSER_EDIT_PASTE:
		{
			m_pcMessageText->Paste( false );
			break;
		}

		case M_COMPOSER_VIEW_TO:
		{
			m_pcAddressBar->ShowAddress( ADDR_TO, m_pcViewMenuTo->IsChecked() );
			_ResizeTextView();

			break;
		}

		case M_COMPOSER_VIEW_CC:
		{
			m_pcAddressBar->ShowAddress( ADDR_CC, m_pcViewMenuCc->IsChecked() );
			_ResizeTextView();

			break;
		}

		case M_COMPOSER_VIEW_BCC:
		{
			m_pcAddressBar->ShowAddress( ADDR_BCC, m_pcViewMenuBcc->IsChecked() );
			_ResizeTextView();

			break;
		}

		case M_COMPOSER_INSERT_SIG:
		{
			int32 nIndex;
			if( pcMessage->FindInt32( "index", &nIndex ) == EOK )
			{
				String cName, cText, cSignature;
				if( m_pcIdentity->GetSignature( cName, cText, nIndex ) != EOK )
					break;

				cSignature = String( "\n-- \n" ) + cText;
				m_pcMessageText->Insert( cSignature.c_str(), false );
			}
			break;
		}

		case M_COMPOSER_INSERT_FILE:
		{
			FileRequester *pcLoadRequester = new FileRequester( FileRequester::LOAD_REQ, new Messenger( this ), "", FileRequester::NODE_FILE );
			pcLoadRequester->Show();
			break;
		}

		case M_LOAD_REQUESTED:
		{
			const char* pzFilename;

			FileRequester *pcRequester;
			if( pcMessage->FindPointer( "source", (void**)&pcRequester ) == EOK )
				pcRequester->Close();

			if( pcMessage->FindString( "file/path", &pzFilename ) == EOK )
				AddAttachment( pzFilename );

			break;
		}

		case M_COMPOSER_REMOVE_FILE:
		{
			int nSelected = m_pcAttachments->GetLastSelected();
			if( nSelected >= 0 )
			{
				m_pcAttachments->RemoveRow( nSelected );

				/* Hide the attachments if no files remain */
				if( m_pcAttachments->GetRowCount() == 0 && m_bAttachmentsShown )
				{
					RemoveChild( m_pcAttachments );
					m_bAttachmentsShown = false;
					_ResizeTextView();

					/* Disable the Remove button */
					m_pcRemoveButton->SetEnable( false );
				}
			}

			break;
		}

		default:
			Window::HandleMessage( pcMessage );
	}
}

bool ComposerWindow::OkToQuit( void )
{
	_SaveGui();

	return true;
}

void ComposerWindow::SetSubject( const String cSubject )
{
	m_pcAddressBar->SetText( ADDR_SUBJECT, cSubject );
}

void ComposerWindow::SetTo( const String cTo )
{
	m_pcAddressBar->SetText( ADDR_TO, cTo );
}

void ComposerWindow::SetCc( const String cCc )
{
	m_pcAddressBar->SetText( ADDR_CC, cCc );
}

void ComposerWindow::SetBcc( const String cBcc )
{
	m_pcAddressBar->SetText( ADDR_BCC, cBcc );
}

void ComposerWindow::SetBody( const String cBody )
{
	m_pcMessageText->Set( cBody.c_str() );
	SetFocusChild( m_pcMessageText );
}

status_t ComposerWindow::AddAttachment( const String cFilename )
{
	if( m_bAttachmentsShown == false )
		_ShowAttachments();

	/* Get the file size */
	String cFilesize;
	try
	{
		File cFile( cFilename );
		off_t nSize = cFile.GetSize();

		String cSuffix;

		if( nSize < 1024 )
		{
			cSuffix = MSG_COMPWND_ATTACHMENT_BYTE;
		}
		else if( nSize < 1024 * 1024 )
		{
			nSize /= 1024;
			cSuffix = MSG_COMPWND_ATTACHMENT_KILOBYTE;
		}
		else if( nSize < 1024 * 1024 * 1024 )
		{
			nSize /= ( 1024 * 1024 );
			cSuffix = MSG_COMPWND_ATTACHMENT_MEGABYTE;
		}
		else
		{
			nSize /= ( 1024 * 1024 * 1024 );
			cSuffix = MSG_COMPWND_ATTACHMENT_GIGABYTE;
		}

		cFilesize.Format( "%li", nSize );
		cFilesize += cSuffix;
	}
	catch( std::exception &e )
	{
		debug( "%s\n", e.what() );
		return EINVAL;
	}

	/* Add the file to the list */
	ListViewStringRow *pcAttachmentRow = new ListViewStringRow();
	pcAttachmentRow->AppendString( cFilename );
	pcAttachmentRow->AppendString( cFilesize );

	m_pcAttachments->InsertRow( pcAttachmentRow );

	/* Enable the Remove button */
	m_pcRemoveButton->SetEnable( true );

	return EOK;
}

void ComposerWindow::_ResizeTextView( void )
{
	Rect cMessageTextFrame = GetBounds();
	Point cAddressBarSize = m_pcAddressBar->GetPreferredSize( true );

	cMessageTextFrame.top = m_vMenuHeight + m_vToolBarHeight + cAddressBarSize.y + 5;

	if( m_bAttachmentsShown )
	{
		Rect cAttachmentsSize = m_pcAttachments->GetBounds();
		cMessageTextFrame.bottom -= cAttachmentsSize.Height();
	}

	m_pcMessageText->SetFrame( cMessageTextFrame );
}

void ComposerWindow::_ShowAttachments( void )
{
	Rect cAttachmentsFrame = GetBounds();
	cAttachmentsFrame.top = cAttachmentsFrame.bottom - 100;

	m_pcAttachments->SetFrame( cAttachmentsFrame );

	AddChild( m_pcAttachments );
	m_bAttachmentsShown = true;

	_ResizeTextView();
}

status_t ComposerWindow::_SetupReply( void )
{
	if( NULL == m_pcEditMessage )
		return EINVAL;

	char *pzBody = NULL;

	m_pcEditMessage->Parse();

	for( int j = 0; j < m_pcEditMessage->GetPartCount(); j++ )
	{
		Multipart cPart = m_pcEditMessage->GetPartInfo(j);
		if( BODY == cPart.eDisposition )
		{
			Multipart cBody = m_pcEditMessage->GetPart(j);
			if( cBody.GetDataSize() > 0 )
			{
				if( cBody.cEncoding == "quoted-printable" )
				{
					QpCodec cCodec;
					cCodec.Decode( cBody.GetData(), cBody.GetDataSize(), &pzBody );
				}
				else if( cBody.cEncoding == "base64" )
				{
					Base64Codec cCodec;
					cCodec.Decode( cBody.GetData(), cBody.GetDataSize(), &pzBody );
				}
				else
				{
					pzBody = (char*)calloc( 1, cBody.GetDataSize() + 1 );
					pzBody = (char*)memcpy( pzBody, cBody.GetData(), cBody.GetDataSize() );
				}
			}
			break;
		}
	}

	String cBodyPrefix, cTo, cCc, cSubject;

	/* XXXKV: We don't handle List- headers at all at the moment, which makes replies
	   to ML traffic a little ropey; the Reply-To: and Cc: fields may not provide the
	   correct reply address for the list.  It depends on how the list is configured. */
	switch( m_nMode )
	{
		case COMPOSE_REPLY_ALL:
		{
			cCc = m_pcEditMessage->GetCc();
			/* Fall through */
		}

		case COMPOSE_REPLY:
		{
			cTo = m_pcEditMessage->GetReplyTo();
			if( cTo == "" )
			{
				cTo = m_pcEditMessage->GetFrom();
				if( cTo == "" )
					cTo = m_pcEditMessage->GetSender();
			}

			/* Only add a Re: prefix if the Subject doesn't already have one */
			RegExp cReplyPrefix( MSG_COMPWND_REFW_REPREFIX + ":" );
			if( cReplyPrefix.Search( m_pcEditMessage->GetSubject() ) == false )
				cSubject = String( "Re: " ) + m_pcEditMessage->GetSubject();
			else
				cSubject = m_pcEditMessage->GetSubject();

			cBodyPrefix = String( MSG_COMPWND_REFW_ON + " " ) +
						  m_pcEditMessage->GetDate() +
						  String( ", " ) +
						  m_pcEditMessage->GetFrom() + String ( " " ) +
						  String( MSG_COMPWND_REFW_WROTE + ":\n\n" );
			break;
		}

		case COMPOSE_FORWARD:
		{
			/* XXXKV: Is "Fw:" or "Fwd:" prefered? */
			cSubject = os::String( MSG_COMPWND_REFW_FW + ": " ) + m_pcEditMessage->GetSubject();

			/* XXXKV: Get all attachments from the original and add to the message being forwarded. */

			/* XXXKV: This is a pretty rubbish message */
			cBodyPrefix = MSG_COMPWND_REFW_FWMSG + "\n\n";
			break;
		}
	}

	/* Strip CRs & indent body */
	if( NULL != pzBody )
	{
		String cBody = cBodyPrefix + _IndentBody( pzBody );
		free( pzBody );

		/* XXXKV: Set() needs to take a String */
		m_pcMessageText->Set( cBody.c_str() );
	}

	m_pcAddressBar->SetText( ADDR_TO, cTo );		
	m_pcAddressBar->SetText( ADDR_CC, cCc );		
	m_pcAddressBar->SetText( ADDR_SUBJECT, cSubject );		

	return EOK;
}

/* Strip all CR's and indent with "> " at the begining of each line */
String ComposerWindow::_IndentBody( const char *pzBody )
{
	String cBody = "> ";

	for( ; *pzBody != '\0'; pzBody++ )
	{
		if( *pzBody == '\r' )
			continue;

		cBody += *pzBody;
		if( *pzBody == '\n' )
			cBody += "> ";
	}

	return cBody;
}

void ComposerWindow::_SaveGui( void )
{
	/* Store current GUI settings */
	m_pcGuiSettings->SetRect( "composer", GetFrame() );
	m_pcGuiSettings->SetBool( "composer_show_to", m_pcViewMenuTo->IsChecked() );
	m_pcGuiSettings->SetBool( "composer_show_cc", m_pcViewMenuCc->IsChecked() );
	m_pcGuiSettings->SetBool( "composer_show_bcc", m_pcViewMenuBcc->IsChecked() );
}

