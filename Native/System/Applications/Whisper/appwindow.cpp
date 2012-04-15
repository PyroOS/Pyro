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
#include <iostream>

WhisperWindow::WhisperWindow( const Rect &cFrame ) : Window( cFrame, "Whisper ", MSG_MAINWND_TITLE + " " + g_cVersion )
{
	m_pcMailMessage = NULL;
	m_pcFolder = NULL;
	m_nCurrentSelection = m_nCurrentFolder = -1;
	m_cCurrentFolderPath = "";
	m_eCopyMode = NONE;
	m_nAuthTransport = -1;
	m_pcOutboundTransport = NULL;
	m_pcImportPlugin = NULL;

	for( int nId = 0; nId < MAX_TIMER_MSGS; nId++ )
		m_apcTimerMessages[nId] = NULL;

	/* This resource get used at various points during the constructor to load icons etc. */
	Resources cRes( get_image_id() );

	/* Get window layouts */
	m_pcGuiSettings = new Settings();
	m_pcGuiSettings->SetFile( "Gui" );
	m_pcGuiSettings->Load();

	m_cWhisperFrame = m_pcGuiSettings->GetRect( "whisper", cFrame );
	m_cComposerFrame =  m_pcGuiSettings->GetRect( "composer", cFrame );
	m_cSettingsFrame = m_pcGuiSettings->GetRect( "settings", Rect( 25, 25, 600, 450 ) );

	SetFrame( m_cWhisperFrame );
	/* XXXKV: The size limit width of 825 means that Whisper is tricky to use on anything less than 1024x768, but
	   anything less means that the end of the Toolbar will be choped off.. */
	SetSizeLimits( Point( 825, 375 ), Point( 4096, 4096 ) );

	/* XXXKV: Whisper should get all of the available identities and the default
	   identity from the configuration.  Use "default" only for testing. */
	m_pcIdentity = new Identity( "default" );

	/* XXXKV: The "visual" settings contain additional GUI settings, mostly related to MessageView, that are
	   controlled directly by the user. Most of this hasn't been implemented yet. */
	m_pcVisualSettings = new Settings();
	m_pcVisualSettings->SetFile( "Visual" );
	m_pcVisualSettings->Load();

	/* Inbound mail filter engine */
	m_pcFilterEngine = new FilterEngine();

	/* Create GUI */
	Rect cBounds = GetBounds();

	Rect cMenuFrame = cBounds;
	cMenuFrame.bottom = 18;

	m_pcMenuBar = new Menu( cMenuFrame, "Whisper", ITEMS_IN_ROW );

	m_pcApplicationMenu = new Menu( Rect(), MSG_MAINWND_MENU_APPLICATION, ITEMS_IN_COLUMN );
	m_pcApplicationMenu->AddItem( MSG_MAINWND_MENU_APPLICATION_QUIT, new Message( M_APP_QUIT ), "Ctrl+Q" );
	m_pcApplicationMenu->AddItem( new MenuSeparator() );
	m_pcApplicationMenu->AddItem( MSG_MAINWND_MENU_APPLICATION_ABOUT, new Message( M_APP_ABOUT ) );
	m_pcMenuBar->AddItem( m_pcApplicationMenu );

	Message *pcFlagMessage, *pcMarkMessage;
	BitmapImage *pcMenuIcon;

	m_pcMessageMenu = new Menu( Rect(), MSG_MAINWND_MENU_MESSAGE, ITEMS_IN_COLUMN );
	m_pcMessageMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_NEW, new Message( M_COMPOSE_MESSAGE ), "Ctrl+N" );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageReplyItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_REPLY, new Message( M_COMPOSE_REPLY ), "Ctrl+R" );
	m_pcMessageMenu->AddItem( m_pcMessageReplyItem );
	m_pcMessageReplyAllItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_REPLYTOALL, new Message( M_COMPOSE_REPLY_ALL ) );
	m_pcMessageMenu->AddItem( m_pcMessageReplyAllItem );
	m_pcMessageForwardItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_FORWARD, new Message( M_COMPOSE_FORWARD ), "Ctrl+F" );
	m_pcMessageMenu->AddItem( m_pcMessageForwardItem );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageViewItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_VIEW, new Message( M_VIEW_MESSAGE ) );
	m_pcMessageMenu->AddItem( m_pcMessageViewItem );
	m_pcMessageViewDataItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_VIEW_DATA, new Message( M_VIEW_MESSAGE_DATA ) );
	m_pcMessageMenu->AddItem( m_pcMessageViewDataItem );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_CHECKFORMESS, new Message( M_CHECK_FOR_MAIL ) );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageSelectAll = new MenuItem( MSG_MAINWND_MENU_MESSAGE_SELECTALL, new Message( M_SELECT_ALL_MESSAGES ) );
	m_pcMessageMenu->AddItem( m_pcMessageSelectAll );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageCutItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_CUT, new Message( M_CUT_MESSAGE ) );
	m_pcMessageMenu->AddItem( m_pcMessageCutItem );
	m_pcMessageCopyItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_COPY, new Message( M_COPY_MESSAGE ) );
	m_pcMessageMenu->AddItem( m_pcMessageCopyItem );
	m_pcMessagePasteItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_PASTE, new Message( M_PASTE_MESSAGE ) );
	m_pcMessageMenu->AddItem( m_pcMessagePasteItem );
	m_pcMessageMenu->AddItem( new MenuSeparator() );
	m_pcMessageDeleteItem = new MenuItem( MSG_MAINWND_MENU_MESSAGE_DELETE, new Message( M_DELETE_MESSAGE ) );
	m_pcMessageMenu->AddItem( m_pcMessageDeleteItem );
	m_pcMessageMenu->AddItem( new MenuSeparator() );

	/* "Flag" child-menu */
	Menu *pcFlagMenu = new Menu( Rect(), MSG_MAINWND_MENU_MESSAGE_FLAG, ITEMS_IN_COLUMN );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_NONE );
	pcFlagMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_NONE, pcFlagMessage );
	pcFlagMenu->AddItem( new MenuSeparator() );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_URGENT );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_urgent.png" ) );
	pcFlagMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_URGENT, pcFlagMessage, "", pcMenuIcon );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_HIGH );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_high.png" ) );
	pcFlagMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_HIGH, pcFlagMessage, "", pcMenuIcon );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_MEDIUM );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_medium.png" ) );
	pcFlagMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_MEDIUM, pcFlagMessage, "", pcMenuIcon );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_LOW );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_low.png" ) );
	pcFlagMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_LOW, pcFlagMessage, "", pcMenuIcon );

	m_pcMessageFlagItem = new MenuItem( pcFlagMenu, NULL );
	m_pcMessageMenu->AddItem( m_pcMessageFlagItem );

	/* "Mark" child-menu */
	Menu *pcMarkMenu = new Menu( Rect(), MSG_MAINWND_MENU_MESSAGE_MARK, ITEMS_IN_COLUMN );
	pcMarkMessage = new Message( M_MARK_MESSAGE );
	pcMarkMessage->AddInt32( "status", STATUS_READ );
	pcMarkMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_MARK_READ, pcMarkMessage );
	pcMarkMenu->AddItem( new MenuSeparator() );
	pcMarkMessage = new Message( M_MARK_MESSAGE );
	pcMarkMessage->AddInt32( "status", STATUS_UNREAD );
	pcMarkMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_MARK_UNREAD, pcMarkMessage );
	pcMarkMessage = new Message( M_MARK_MESSAGE );
	pcMarkMessage->AddInt32( "status", STATUS_DRAFT );
	pcMarkMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_MARK_DRAFT, pcMarkMessage );
	m_pcMessageMarkItem = new MenuItem( pcMarkMenu, NULL );
	m_pcMessageMenu->AddItem( m_pcMessageMarkItem );

	m_pcMenuBar->AddItem( m_pcMessageMenu );

	m_pcEditMenu = new Menu( Rect(), MSG_MAINWND_MENU_EDIT, ITEMS_IN_COLUMN );
	m_pcEditCopy = new MenuItem( MSG_MAINWND_MENU_EDIT_COPY, new Message( M_EDIT_COPY ), "Ctrl+C" );
	m_pcEditCopy->SetEnable( false );
	m_pcEditMenu->AddItem( m_pcEditCopy );
	m_pcMenuBar->AddItem( m_pcEditMenu );

	m_pcFolderMenu = new Menu( Rect(), MSG_MAINWND_MENU_FOLDER, ITEMS_IN_COLUMN );
	m_pcFolderCreateItem = new MenuItem( MSG_MAINWND_MENU_FOLDER_CREATE, new Message( M_CREATE_FOLDER ) );
	m_pcFolderMenu->AddItem( m_pcFolderCreateItem );
	m_pcFolderRenameItem = new MenuItem( MSG_MAINWND_MENU_FOLDER_RENAME, new Message( M_RENAME_FOLDER ) );
	m_pcFolderMenu->AddItem( m_pcFolderRenameItem );
	m_pcFolderMenu->AddItem( new MenuSeparator() );
	m_pcFolderDeleteItem = new MenuItem( MSG_MAINWND_MENU_FOLDER_DELETE, new Message( M_DELETE_FOLDER ) );
	m_pcFolderMenu->AddItem( m_pcFolderDeleteItem );
	m_pcFolderMenu->AddItem( new MenuSeparator() );
	m_pcFolderPropertiesItem = new MenuItem( MSG_MAINWND_MENU_FOLDER_PROPERTIES, new Message( M_PROPERTIES_FOLDER ) );
	m_pcFolderMenu->AddItem( m_pcFolderPropertiesItem );
	m_pcFolderMenu->AddItem( new MenuSeparator() );
	m_pcFolderMenu->AddItem( MSG_MAINWND_MENU_FOLDER_IMPORTMESS, new Message( M_DO_IMPORT ) );
	m_pcFolderMenu->AddItem( MSG_MAINWND_MENU_FOLDER_EXPORTMESS, new Message( M_DO_EXPORT ) );
	m_pcFolderMenu->AddItem( new MenuSeparator() );
	m_pcFolderMenu->AddItem( MSG_MAINWND_MENU_FOLDER_EMPTYTRASH, new Message( M_EMPTY_TRASH ) );
	m_pcMenuBar->AddItem( m_pcFolderMenu );

	m_pcSettingsMenu = new Menu( Rect(), MSG_MAINWND_MENU_SETTINGS, ITEMS_IN_COLUMN );
	m_pcSettingsMenu->AddItem( MSG_MAINWND_MENU_SETTINGS_CONFIGURE, new Message( M_SETTINGS_CONFIGURE ) );
	m_pcSettingsMenu->AddItem( new MenuSeparator() );
	m_pcSettingsSaveOCItem = new CheckMenu( MSG_MAINWND_MENU_SETTINGS_SAVECLOSE, new Message( M_SETTINGS_SAVE_OC ) );
	m_pcSettingsMenu->AddItem( m_pcSettingsSaveOCItem );
	m_pcSettingsMenu->AddItem( MSG_MAINWND_MENU_SETTINGS_SAVENOW, new Message( M_SETTINGS_SAVE_NOW ) );
	m_pcMenuBar->AddItem( m_pcSettingsMenu );

	m_vMenuHeight = m_pcMenuBar->GetPreferredSize( true ).y - 1.0f;
	cMenuFrame.bottom = m_vMenuHeight;
	m_pcMenuBar->SetFrame( cMenuFrame );
	m_pcMenuBar->SetTargetForItems( this );
	AddChild( m_pcMenuBar );

	cBounds.top = m_vMenuHeight + 1;

	/* Toolbar */
	Rect cToolBarFrame = cBounds;
	cToolBarFrame.bottom = cToolBarFrame.top + 48;

	m_pcToolBar = new ToolBar( cToolBarFrame, "Whisper", CF_FOLLOW_LEFT | CF_FOLLOW_TOP | CF_FOLLOW_RIGHT );

	BitmapImage *pcToolBarIcon;

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "new.png" ) );
	m_pcToolBar->AddButton( "new", MSG_MAINWND_MENUBUTTONS_NEW, pcToolBarIcon, new Message( M_COMPOSE_MESSAGE ) );

	m_pcToolBar->AddSeparator( "" );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "message_cut.png" ) );
	m_pcCutButton = new ImageButton( Rect(), "message_cut", MSG_MAINWND_MENUBUTTONS_CUT, new Message( M_CUT_MESSAGE ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcCutButton, ToolBar::TB_FIXED_WIDTH );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "message_copy.png" ) );
	m_pcCopyButton = new ImageButton( Rect(), "message_copy", MSG_MAINWND_MENUBUTTONS_COPY, new Message( M_COPY_MESSAGE ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcCopyButton, ToolBar::TB_FIXED_WIDTH );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "message_paste.png" ) );
	m_pcPasteButton = new ImageButton( Rect(), "message_paste", MSG_MAINWND_MENUBUTTONS_PASTE, new Message( M_PASTE_MESSAGE ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcPasteButton, ToolBar::TB_FIXED_WIDTH );

	m_pcToolBar->AddSeparator( "" );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "message_delete.png" ) );
	m_pcDeleteButton = new ImageButton( Rect(), "message_delete", MSG_MAINWND_MENUBUTTONS_DEL, new Message( M_DELETE_MESSAGE ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcDeleteButton, ToolBar::TB_FIXED_WIDTH );

	m_pcToolBar->AddSeparator( "" );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "reply.png" ) );
	m_pcReplyButton = new ImageButton( Rect(), "reply", MSG_MAINWND_MENUBUTTONS_REPLY, new Message( M_COMPOSE_REPLY ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcReplyButton, ToolBar::TB_FIXED_WIDTH );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "reply_all.png" ) );
	m_pcReplyAllButton = new ImageButton( Rect(), "reply_all", MSG_MAINWND_MENUBUTTONS_REPLYALL, new Message( M_COMPOSE_REPLY_ALL ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcReplyAllButton, ToolBar::TB_FIXED_WIDTH );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "forward.png" ) );
	m_pcForwardButton = new ImageButton( Rect(), "forward", MSG_MAINWND_MENUBUTTONS_FORWARD, new Message( M_COMPOSE_FORWARD ), pcToolBarIcon, ImageButton::IB_TEXT_BOTTOM, true, true, true );
	m_pcToolBar->AddChild( m_pcForwardButton, ToolBar::TB_FIXED_WIDTH );

	m_pcToolBar->AddSeparator( "" );

	pcToolBarIcon = new BitmapImage();
	pcToolBarIcon->Load( cRes.GetResourceStream( "get_mail.png" ) );
	m_pcToolBar->AddButton( "get", MSG_MAINWND_MENUBUTTONS_GETMESS, pcToolBarIcon, new Message( M_CHECK_FOR_MAIL ) );

	AddChild( m_pcToolBar );

	cBounds.top = cToolBarFrame.bottom + 1;

	/* Message list */
	m_pcMessagesList = new TreeView( Rect(), "Messages", ( ListView::F_MULTI_SELECT | ListView::F_RENDER_BORDER | ListView::F_NO_AUTO_SORT | ListView::F_NO_COL_REMAP ) );

	int nIconWidth = 20;
	m_pcMessagesList->InsertColumn( "", nIconWidth );	/* Flags */
	m_pcMessagesList->InsertColumn( "", nIconWidth );	/* Status */
	m_pcMessagesList->InsertColumn( "", nIconWidth );	/* Attchments */

	int nStringWidth = (int)( cBounds.Width() - ( nIconWidth * 3 ) ) / 3;
	m_pcMessagesList->InsertColumn( MSG_MAINWND_MSGLST_SUBJECT.c_str(), nStringWidth );
	m_pcMessagesList->InsertColumn( MSG_MAINWND_MSGLST_FROM.c_str(), nStringWidth );
	m_pcMessagesList->InsertColumn( MSG_MAINWND_MSGLST_SENT.c_str(), nStringWidth );

	m_pcMessagesList->SetSelChangeMsg( new Message( M_SELECT ) );

	/* Message list context menu */
	m_pcListViewMenu = new Menu( Rect(), "Message", ITEMS_IN_COLUMN );
	m_pcMessageCutCtxItem = new MenuItem( MSG_MAINWND_MSGLST_CONMENU_CUT, new Message( M_CUT_MESSAGE ) );
	m_pcListViewMenu->AddItem( m_pcMessageCutCtxItem );
	m_pcMessageCopyCtxItem = new MenuItem( MSG_MAINWND_MSGLST_CONMENU_COPY, new Message( M_COPY_MESSAGE ) );
	m_pcListViewMenu->AddItem( m_pcMessageCopyCtxItem );
	m_pcMessagePasteCtxItem = new MenuItem( MSG_MAINWND_MSGLST_CONMENU_PASTE, new Message( M_PASTE_MESSAGE ) );
	m_pcListViewMenu->AddItem( m_pcMessagePasteCtxItem );
	m_pcListViewMenu->AddItem( new MenuSeparator() );
	m_pcMessageDeleteCtxItem = new MenuItem( MSG_MAINWND_MSGLST_CONMENU_DELETE, new Message( M_DELETE_MESSAGE ) );
	m_pcListViewMenu->AddItem( m_pcMessageDeleteCtxItem );
	m_pcListViewMenu->AddItem( new MenuSeparator() );

	/* "Flag" child menu */
	Menu *pcFlagCtxMenu = new Menu( Rect(), MSG_MAINWND_MENU_MESSAGE_FLAG, ITEMS_IN_COLUMN );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_NONE );
	pcFlagCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_NONE, pcFlagMessage );
	pcFlagCtxMenu->AddItem( new MenuSeparator() );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_URGENT );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_urgent.png" ) );
	pcFlagCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_URGENT, pcFlagMessage, "", pcMenuIcon );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_HIGH );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_high.png" ) );
	pcFlagCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_HIGH, pcFlagMessage, "", pcMenuIcon );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_MEDIUM );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_medium.png" ) );
	pcFlagCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_MEDIUM, pcFlagMessage, "", pcMenuIcon );
	pcFlagMessage = new Message( M_FLAG_MESSAGE );
	pcFlagMessage->AddInt32( "flag", FLAG_LOW );
	pcMenuIcon = new BitmapImage();
	pcMenuIcon->Load( cRes.GetResourceStream( "flag_low.png" ) );
	pcFlagCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_FLAG_LOW, pcFlagMessage, "", pcMenuIcon );

	m_pcMessageFlagCtxItem = new MenuItem( pcFlagCtxMenu, NULL );
	m_pcListViewMenu->AddItem( m_pcMessageFlagCtxItem );

	/* "Mark" child-menu */
	Menu *pcMarkCtxMenu = new Menu( Rect(), MSG_MAINWND_MENU_MESSAGE_MARK, ITEMS_IN_COLUMN );
	pcMarkMessage = new Message( M_MARK_MESSAGE );
	pcMarkMessage->AddInt32( "status", STATUS_READ );
	pcMarkCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_MARK_READ, pcMarkMessage );
	pcMarkCtxMenu->AddItem( new MenuSeparator() );
	pcMarkMessage = new Message( M_MARK_MESSAGE );
	pcMarkMessage->AddInt32( "status", STATUS_UNREAD );
	pcMarkCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_MARK_UNREAD, pcMarkMessage );
	pcMarkMessage = new Message( M_MARK_MESSAGE );
	pcMarkMessage->AddInt32( "status", STATUS_DRAFT );
	pcMarkCtxMenu->AddItem( MSG_MAINWND_MENU_MESSAGE_MARK_DRAFT, pcMarkMessage );
	m_pcMessageMarkCtxItem = new MenuItem( pcMarkMenu, NULL );
	m_pcListViewMenu->AddItem( m_pcMessageMarkCtxItem );

	m_pcListViewMenu->SetTargetForItems( this );
	m_pcMessagesList->SetContextMenu( m_pcListViewMenu );

	/* Message view */
	m_pcMessageView = new MessageView( Rect( 0, 0, 0, 0 ), "whisper_message_view", this, m_pcVisualSettings );

	/* Mail folder tree */
	m_pcTreeView = new TreeView( Rect(), "whisper_treeview", ( ListView::F_NO_AUTO_SORT | ListView::F_RENDER_BORDER | ListView::F_NO_HEADER ) );
	m_pcTreeView->InsertColumn( "folders", 1 );
	m_pcTreeView->SetSelChangeMsg( new Message( M_FOLDER_SELECT ) );

	/* Mail folder context menu */
	m_pcTreeViewMenu = new Menu( Rect(), "Mailbox", ITEMS_IN_COLUMN );
	m_pcFolderCreateCtxItem = new MenuItem( MSG_MAINWND_MAILFOLDERLIST_CONMENU_CREATE, new Message( M_CREATE_FOLDER ) );
	m_pcTreeViewMenu->AddItem( m_pcFolderCreateCtxItem );
	m_pcFolderRenameCtxItem = new MenuItem( MSG_MAINWND_MAILFOLDERLIST_CONMENU_RENAME, new Message( M_RENAME_FOLDER ) );
	m_pcTreeViewMenu->AddItem( m_pcFolderRenameCtxItem );
	m_pcTreeViewMenu->AddItem( new MenuSeparator() );
	m_pcFolderDeleteCtxItem = new MenuItem( MSG_MAINWND_MAILFOLDERLIST_CONMENU_DELETE, new Message( M_DELETE_FOLDER ) );
	m_pcTreeViewMenu->AddItem( m_pcFolderDeleteCtxItem );
	m_pcTreeViewMenu->AddItem( new MenuSeparator() );
	m_pcFolderPropertiesCtxItem = new MenuItem( MSG_MAINWND_MAILFOLDERLIST_CONMENU_PROPERTIES, new Message( M_PROPERTIES_FOLDER ) );
	m_pcTreeViewMenu->AddItem( m_pcFolderPropertiesCtxItem );
	m_pcTreeViewMenu->AddItem( new MenuSeparator() );
	m_pcFolderPostCtxItem = new MenuItem( MSG_MAINWND_MAILFOLDERLIST_CONMENU_POST, new Message( M_POST_FOLDER ) );
	m_pcTreeViewMenu->AddItem( m_pcFolderPostCtxItem );
	m_pcTreeViewMenu->SetTargetForItems( this );
	m_pcTreeView->SetContextMenu( m_pcTreeViewMenu );

	/* Status bar for user information */
	m_pcStatusBar = new StatusBar( Rect(), "whisper_status", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_BOTTOM );
	m_pcStatusBar->AddPanel( "general_messages", "" );

	Point cStatusSize = m_pcStatusBar->GetPreferredSize( false );
	cBounds.bottom -= cStatusSize.y;;
	Rect cStatusFrame;
	cStatusFrame.left = 0;
	cStatusFrame.top = cBounds.bottom + 1;
	cStatusFrame.right = cBounds.right;
	cStatusFrame.bottom = cStatusFrame.top + cStatusSize.y;

	m_pcStatusBar->SetFrame( cStatusFrame );
	AddChild( m_pcStatusBar );

	/* Set an application icon */
	BitmapImage *pcAppIcon = new BitmapImage();
	pcAppIcon->Load( cRes.GetResourceStream( "icon24x24.png" ) );
	SetIcon( pcAppIcon->LockBitmap() );
	delete( pcAppIcon );

	/* Set additional shortcuts */
	AddShortcut( ShortcutKey( "v" ), new Message( M_VIEW_MESSAGE ) );	/* View message in new window */
	AddShortcut( ShortcutKey( VK_ENTER ), new Message( M_VIEW_MESSAGE ) );	/* Same again, "Enter" key */
	AddShortcut( ShortcutKey( "d" ), new Message( M_DELETE_MESSAGE ) );	/* Delete current message */
	AddShortcut( ShortcutKey( VK_DELETE ), new Message( M_DELETE_MESSAGE ) );	/* Same again, "Delete" key */

	/* Register Events */
	m_pcSendEvent = Event::Register( "internet/Mail/Send", "Create a new email message.", this, M_MAIL_SEND );
	m_pcGetEvent = Event::Register( "internet/Mail/GetNewCount", "Get total of new messages waiting.", this, M_MAIL_GET_COUNT );
	m_pcCheckEvent = Event::Register( "internet/Mail/Check", "Check for new messages.", this, M_MAIL_CHECK );

	/* Get Identity configuration */
	LoadIdentity( m_pcIdentity );

	/* Populate the folder tree with mail boxes */
	/* XXXKV: This should be in LoadIdentity() once it is possible for
	   the user to add or remove Mailboxes from the config. */
	LoadMailboxes();

	/* Splitters */
	m_pcVSplitter = new Splitter( cBounds, "whisper_v_splitter", m_pcMessagesList, m_pcMessageView, VERTICAL, CF_FOLLOW_ALL );
	m_pcVSplitter->SetSplitRatio( m_pcGuiSettings->GetFloat( "whisper_v_ratio", 0.5f ) );
	m_pcHSplitter = new Splitter( cBounds, "whisper_h_splitter", m_pcTreeView, m_pcVSplitter, HORIZONTAL, CF_FOLLOW_ALL );
	m_pcHSplitter->SetSplitRatio( m_pcGuiSettings->GetFloat( "whisper_h_ratio", 0.3f ) );
	AddChild( m_pcHSplitter );

	/* Set saved config */
	m_pcSettingsSaveOCItem->SetChecked( m_pcGuiSettings->GetBool( "whisper_save_oc", true ) );

	/* Select the Inbox */
	m_pcTreeView->Select( m_nInbox + 2, false, true );
}

WhisperWindow::~WhisperWindow()
{
	RemoveChild( m_pcHSplitter );
	delete( m_pcHSplitter );

	RemoveChild( m_pcStatusBar );
	delete( m_pcStatusBar );

	RemoveChild( m_pcToolBar );
	delete( m_pcToolBar );

	std::list <Mailmessage*>::iterator i;
	for( i = m_vCopyMessages.begin(); i != m_vCopyMessages.end(); i++ )
		delete( (*i) );
	m_vCopyMessages.clear();

	if( m_pcFolder )
		delete( m_pcFolder );

	if( m_pcIdentity )
		delete( m_pcIdentity );

	std::vector <Mailbox*>::iterator j;
	for( j = m_vBoxes.begin(); j != m_vBoxes.end(); j++ )
		delete( (*j) );
	m_vBoxes.clear();

	std::vector <Transport*>::iterator k;
	for( k = m_vInboundTransports.begin(); k != m_vInboundTransports.end(); k++ )
		delete( (*k) );
	m_vInboundTransports.clear();
	if( m_pcOutboundTransport )
		delete( m_pcOutboundTransport );

	if( m_pcSettingsSaveOCItem->IsChecked() )
		m_pcGuiSettings->Save();
	delete( m_pcGuiSettings );
	delete( m_pcVisualSettings );

	if(	m_pcSendEvent )
		delete( m_pcSendEvent );
	if(	m_pcGetEvent )
		delete( m_pcGetEvent );
	if(	m_pcCheckEvent )
		delete( m_pcCheckEvent );

	if( m_pcFilterEngine )
		delete( m_pcFilterEngine );

	for( int nId = 0; nId < MAX_TIMER_MSGS; nId++ )
		if( m_apcTimerMessages[ nId ] != NULL )
			delete( m_apcTimerMessages[ nId ] );
}

/* WhisperWindow::HandleMessage() is implemented in appwindow_messages.cpp */

void WhisperWindow::TimerTick( int nId )
{
	Message *pcMessage = m_apcTimerMessages[ nId ];
	m_apcTimerMessages[ nId ] = NULL;
	HandleMessage( pcMessage );
}

bool WhisperWindow::OkToQuit( void )
{
	_SaveGui();

	Application::GetInstance()->PostMessage( M_QUIT );
	return true;
}

status_t WhisperWindow::LoadIdentity( Identity *pcIdentity )
{
	/* Clear current configuration */
	std::vector <Mailbox*>::iterator i;
	for( i = m_vBoxes.begin(); i != m_vBoxes.end(); i++ )
		delete( (*i) );
	m_vBoxes.clear();

	std::vector <Transport*>::iterator j;
	for( j = m_vInboundTransports.begin(); j != m_vInboundTransports.end(); j++ )
		delete( (*j) );
	m_vInboundTransports.clear();
	if( m_pcOutboundTransport )
		delete( m_pcOutboundTransport );

	/* Get all of the configured mailboxes from the current identity */
	/* XXXKV: This will only work with one single identity.  With multiple identities, all
	   of the configured transports for each identity would be lumped together.  At the moment
	   I can't remember exactly how I wanted to tie multiple identities & mailboxes together
	   either; one identity per mailbox?  Multiple identities per user with only one identity
	   "active" at any one time?  Must check and get my head around it again, it's been too long. */
	MailboxId cBoxId;
	Mailbox *pcBox;
	MailboxFactory *pcMailboxFactory = MailboxFactory::GetFactory();

	int n = 0;
	while( pcIdentity->GetMailboxId( cBoxId, n++ ) == EOK )
	{
		debug( "box #%d: type=%s, name=%s\n", n, cBoxId.GetIdentifier().c_str(),cBoxId.GetName().c_str() );

		pcBox = pcMailboxFactory->FindMailbox( cBoxId.GetIdentifier(), cBoxId.GetName() );
		m_vBoxes.push_back( pcBox );
	}

	/* Get all transports.  Outbound, then inbound. */
	Server cServer;
	TransportFactory *pcTransportFactory = TransportFactory::GetFactory();

	if( pcIdentity->GetServer( cServer, SERVER_OUTBOUND, 0 ) == EOK )
	{
		m_pcOutboundTransport = pcTransportFactory->FindTransport( cServer.GetTransport() );
		if( NULL != m_pcOutboundTransport )
		{
			m_pcOutboundTransport->SetConnection( cServer );

			if( cServer.GetFlags() & AUTH_POP3_BEFORE )
				m_nAuthTransport = cServer.GetData().AsInt16();

			debug( "outbound: %s at %s\n", cServer.GetTransport().c_str(), cServer.GetServer().c_str() );
			debug( "port #%d, user=%s, flags=%Lu\n", cServer.GetPort(), cServer.GetUsername().c_str(), cServer.GetFlags() );
		}
	}
	else
		m_pcOutboundTransport = NULL;

	Transport *pcTransport;
	n = 0;
	while( pcIdentity->GetServer( cServer, SERVER_INBOUND, n++ ) == EOK )
	{
		pcTransport = pcTransportFactory->FindTransport( cServer.GetTransport() );
		if( NULL != pcTransport )
		{
			pcTransport->SetConnection( cServer );
			m_vInboundTransports.push_back( pcTransport );

			debug( "inbound: %s at %s\n", cServer.GetTransport().c_str(), cServer.GetServer().c_str() );
			debug( "port #%d, user=%s, flags=%Lu\n", cServer.GetPort(), cServer.GetUsername().c_str(), cServer.GetFlags() );
		}
	}
	debug( "from: %s\n", pcIdentity->GetFrom().c_str() );

	/* Reset the status bar (The number of inbound servers may have changed) */
	RemoveChild( m_pcStatusBar );
	delete( m_pcStatusBar );

	m_pcStatusBar = new StatusBar( Rect(), "whisper_status", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_BOTTOM );
	m_pcStatusBar->AddPanel( "general_messages", "" );

	/* Each inbound transport has it's own panel in the status bar */
	for( n = 0; n < (int)m_vInboundTransports.size(); n++ )
	{
		char zName[11] = {0};
		sprintf( zName, "transport%i", n );
		m_pcStatusBar->AddPanel( zName, "" );
	}

	Point cStatusSize = m_pcStatusBar->GetPreferredSize( false );
	Rect cBounds = GetBounds();
	cBounds.bottom -= cStatusSize.y;;
	Rect cStatusFrame;
	cStatusFrame.left = 0;
	cStatusFrame.top = cBounds.bottom + 1;
	cStatusFrame.right = cBounds.right;
	cStatusFrame.bottom = cStatusFrame.top + cStatusSize.y;

	m_pcStatusBar->SetFrame( cStatusFrame );
	AddChild( m_pcStatusBar );

	return EOK;
}

status_t WhisperWindow::LoadMailboxes( void )
{
	status_t nError = EOK;

	/* Clear any existing folder tree */
	int nRow = 0;
	ListViewRow *pcRow;
	while( ( pcRow = m_pcTreeView->GetRow( nRow++ ) ) )
	{
		folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();
		if( psCookie )
			delete psCookie;
	}
	m_pcTreeView->Hide();
	m_pcTreeView->Clear();

	/* Create root node */
	TreeViewStringNode *pcNode = new TreeViewStringNode();

	/* XXXKV: This is quite a lot of bother just to get the current username; Glibc
	   loads nss_compat etc. which takes some time.  Is it really worth it? */
	struct passwd *psPwd;
	psPwd = getpwuid( geteuid() );
	os::String cTitle = os::String( MSG_MAINWND_MAILFOLDERLIST_MAILFOR + " " ) + os::String( psPwd->pw_name );

	pcNode->AppendString( cTitle );
	pcNode->SetIndent( 1 );
	pcNode->SetCookie( (void*)NULL );

	m_pcTreeView->InsertNode( pcNode );

	/* Add all Mailboxes */
	for( uint i = 0; i < m_vBoxes.size(); i++ )
		nError = AddMailbox( i );

	m_pcTreeView->Show();

	m_pcTreeView->Invalidate();
	m_pcTreeView->Flush();

	return nError;
}

status_t WhisperWindow::AddMailbox( int nBox )
{
	m_pcCurrentBox = m_vBoxes[nBox];

	/* Create Mailbox node */
	TreeViewStringNode *pcNode = new TreeViewStringNode();
	pcNode->AppendString( m_pcCurrentBox->GetName() );
	pcNode->SetIndent( 2 );

	folder_cookie_s *psCookie = new folder_cookie_s;
	psCookie->cName = m_pcCurrentBox->GetName();
	psCookie->cPath = "";	/* Add an empty entry for the Mailbox node */
	psCookie->nBox = nBox;
	pcNode->SetCookie( psCookie );

	/* Load an appropriate image for the mailbox */
	Resources cRes( get_image_id() );		
	ResStream *pcStream = cRes.GetResourceStream( "mailbox.png" );
	BitmapImage *pcMailboxImage = new BitmapImage();
	pcMailboxImage->Load( pcStream );
	delete( pcStream );

	pcNode->SetIcon( pcMailboxImage );

	m_pcTreeView->InsertNode( pcNode );

	/* Get list of child folders from the Mailbox and populate TreeView */
	return AddChildFolder( m_pcCurrentBox->OpenFolder( "" ), nBox, 3);
}

status_t WhisperWindow::AddChildFolder( Mailfolder *pcParent, int nParentIndex, int nDepth )
{
	if( pcParent->IsValid() == false )
		return EIO;

	int nChildCount = pcParent->GetChildCount();

	Resources cRes( get_image_id() );		
	ResStream *pcStream = NULL;

	for( int nChild = 0; nChild < nChildCount; nChild++ )
	{
		os::String cChildName = pcParent->GetChildName( nChild );

		String cChildPath = pcParent->GetPath() + os::String( "/" ) + cChildName;

		TreeViewStringNode *pcNode = new TreeViewStringNode();
		pcNode->AppendString( cChildName );
		pcNode->SetIndent( nDepth );

		folder_cookie_s *psCookie = new folder_cookie_s;
		psCookie->cName = cChildName;
		psCookie->cPath = cChildPath;
		psCookie->nBox = nParentIndex;
		pcNode->SetCookie( psCookie );

		/* Load an appropriate image for the folder */
		pcStream = cRes.GetResourceStream( "folder.png" );
		BitmapImage *pcFolderImage = new BitmapImage();
		pcFolderImage->Load( pcStream );

		pcNode->SetIcon( pcFolderImage );

		/* Add the node to the tree and an entry in the folder map */
		m_pcTreeView->InsertNode( pcNode );

		/* If this folder has children, recursively call AddChildFolder() for it's children */
		Mailfolder *pcChildMailfolder = m_pcCurrentBox->OpenFolder( cChildPath );

		if( pcChildMailfolder->IsValid() )
		{
			int64 nUnreadCount = pcChildMailfolder->GetUnreadCount();
			if( nUnreadCount > 0 )
			{
				String cDisplayName;

				cDisplayName.Format( "%s [%d]", cChildName.c_str(), nUnreadCount );
				pcNode->SetString( 0, cDisplayName );
			}

			if( pcChildMailfolder->GetChildCount() > 0 )
				AddChildFolder( pcChildMailfolder, nParentIndex, nDepth + 1 );

			FolderProperties *pcChildProperties = pcChildMailfolder->GetProperties();
			if( pcChildProperties->GetType() == FL_TYPE_INBOX )
				m_nInbox = nChild;

			delete( pcChildMailfolder );
		}
	}

	if( pcStream )
		delete( pcStream );

	return EOK;
}

status_t WhisperWindow::AddMessage( Mailmessage *pcMailMessage )
{
	status_t nError;
	Mailfolder *pcTargetFolder;
	uint nRow;

	/* Parse message */
	pcMailMessage->Parse();

	FolderReference cReference = m_pcFilterEngine->FilterMessage( pcMailMessage );

	debug( "filter result: m_eType=%d, m_cName=%s\n", cReference.m_eType, cReference.m_cName.c_str() );

	if( cReference.m_eType == FL_TYPE_NORMAL )
		nError = FindFolderRow( &pcTargetFolder, &nRow, cReference.m_cName );
	else
		nError = FindFolderRowByType( &pcTargetFolder, &nRow, cReference.m_eType );

	if( nError != EOK )
	{
		delete( pcMailMessage );
		return ENOENT;
	}

	if( pcTargetFolder->IsValid() == true )
	{
		pcMailMessage->SetStatus( STATUS_UNREAD );

		/* Write message to folder */
		pcTargetFolder->Lock();
		if( pcTargetFolder->Write( pcMailMessage ) != EOK )
		{
			delete( pcTargetFolder );
			delete( pcMailMessage );
			return EIO;
		}
		UpdateUnread( pcTargetFolder, nRow, INC );

		/* Is the target folder currently selected? If so, make the new message visible now */
		if( m_pcFolder->GetPath() == pcTargetFolder->GetPath() )
		{
			debug( "Adding new message\n" );

			TreeViewMessageNode *pcNode = new TreeViewMessageNode();

			Font *pcFont;
			font_properties cFontProperties;

			pcFont = new Font();
			pcFont->GetDefaultFont( DEFAULT_FONT_BOLD, &cFontProperties );
			pcFont->SetProperties( cFontProperties );

			pcNode->SetFont( pcFont );

			pcNode->AppendIcon( NULL );	/* Status */
			pcNode->AppendIcon( NULL ); /* Flag */

			/* Attachments */
			if( pcMailMessage->GetAttachmentCount() > 0 )
			{
				Resources cRes( get_image_id() );
				BitmapImage *pcIcon;

				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "attachment16x16.png" ) );
				pcNode->AppendIcon( pcIcon );
			}
			else
				pcNode->AppendIcon( NULL );

			pcNode->AppendString( pcMailMessage->GetSubject() );
			pcNode->AppendString( pcMailMessage->GetFrom() );

			pcNode->AppendString( display_date( convert_date( pcMailMessage->GetDate() ) ) );

			pcNode->SetCookie( pcMailMessage->GetReference() );

			m_pcMessagesList->InsertNode( pcNode, true );
		}

		pcTargetFolder->Unlock();
	}

	delete( pcTargetFolder );
	delete( pcMailMessage );

	return EOK;
}

status_t WhisperWindow::ScanFolder( String cPath )
{
	if( m_pcFolder )
		delete( m_pcFolder );

	m_pcMessagesList->Clear();
	m_pcMessageView->Clear();

	m_pcFolder = m_pcCurrentBox->OpenFolder( cPath );
	if( m_pcFolder->IsValid() == false )
	{
		delete( m_pcFolder );
		m_pcFolder = NULL;
		return EIO;
	}

	/* Special short-circuit if the root mailbox folder has been selected */
	if( cPath == "" )
		return EOK;

	m_pcFolder->Lock();

	Mailsummery cSummery;
	TreeViewMessageNode *pcNode;

	Resources cRes( get_image_id() );
	BitmapImage *pcIcon;

	status_t nError;
	while( ( nError = m_pcFolder->GetNextEntry( &cSummery ) ) != ENOENT )
	{
		if( EISDIR == nError )
			continue;

		pcNode = new TreeViewMessageNode();

		/* Display a flag if one is set */
		switch( cSummery.nFlag )
		{
			case FLAG_LOW:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_low.png" ) );
				pcNode->AppendIcon( pcIcon );
				break;
			}

			case FLAG_MEDIUM:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_medium.png" ) );
				pcNode->AppendIcon( pcIcon );
				break;
			}

			case FLAG_HIGH:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_high.png" ) );
				pcNode->AppendIcon( pcIcon );
				break;
			}

			case FLAG_URGENT:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "flag_urgent.png" ) );
				pcNode->AppendIcon( pcIcon );
				break;
			}

			case FLAG_NONE:
			default:
			{
				pcNode->AppendIcon( NULL );
				break;
			}
		}

		switch( cSummery.nStatus )
		{
			case STATUS_NEW:
			case STATUS_DRAFT:
			case STATUS_UNREAD:
			{
				Font *pcFont;
				font_properties cFontProperties;

				pcFont = new Font();
				pcFont->GetDefaultFont( DEFAULT_FONT_BOLD, &cFontProperties );
				pcFont->SetProperties( cFontProperties );

				if( cSummery.nStatus == STATUS_DRAFT )
					pcFont->SetFlags( FPF_ITALIC );
				pcNode->SetFont( pcFont );

				pcNode->AppendIcon( NULL );

				break;
			}
			case STATUS_REPLIED:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "reply16x16.png" ) );

				pcNode->AppendIcon( pcIcon );

				break;
			}
			case STATUS_FORWARDED:
			{
				pcIcon = new BitmapImage();
				pcIcon->Load( cRes.GetResourceStream( "forward16x16.png" ) );

				pcNode->AppendIcon( pcIcon );

				break;
			}

			case STATUS_READ:
			default:
			{
				pcNode->AppendIcon( NULL );
				break;
			}
		}

		/* Attachments */
		if( cSummery.nAttachments > 0 )
		{
			pcIcon = new BitmapImage();
			pcIcon->Load( cRes.GetResourceStream( "attachment16x16.png" ) );
			pcNode->AppendIcon( pcIcon );
		}
		else
			pcNode->AppendIcon( NULL );

		pcNode->AppendString( cSummery.cSubject );
		pcNode->AppendString( cSummery.cFrom );

		pcNode->AppendString( display_date( cSummery.nDate ) );

		pcNode->SetCookie( cSummery.cReference );

		m_pcMessagesList->InsertNode( pcNode, false );
	}

	m_pcFolder->Unlock();

	return EOK;
}

/* Find the folder cName. If specified, only check the mailbox nMailbox.  A value of -1 indicates
   "Any mailbox".  Return as soon as the first match is made */
status_t WhisperWindow::FindFolder( Mailfolder **ppcFolder, os::String cName, int nMailbox )
{
	return FindFolderRow( ppcFolder, NULL, cName, nMailbox );
}

status_t WhisperWindow::FindFolderRow( Mailfolder **ppcFolder, uint *pnRow, os::String cName, int nMailbox )
{
	status_t nError = ENOENT;

	if( nMailbox >= 0 && nMailbox > (int)m_vBoxes.size() )
		return EINVAL;

	for( uint nRow = 0; nRow < m_pcTreeView->GetRowCount(); nRow++ )
	{
		ListViewRow *pcRow = m_pcTreeView->GetRow( nRow );
		folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();
		if( NULL == psCookie )
			continue;
		const int nBox = psCookie->nBox;

		if( nMailbox >= 0 && nBox != nMailbox )
			continue;

		if( psCookie->cName == cName )
		{
			Mailbox *pcBox = m_vBoxes[nBox];
			Mailfolder *pcFolder = pcBox->OpenFolder( psCookie->cPath );

			if( m_pcFolder->IsValid() == false )
			{
				nError = EINVAL;
				break;
			}
			else
			{
				*ppcFolder = pcFolder;
				if( pnRow != NULL )
					*pnRow = nRow;
				nError = EOK;
				break;
			}
		}
	}

	return nError;
}

status_t WhisperWindow::FindFolderByPath( Mailfolder **ppcFolder, os::String cPath, int nMailbox )
{
	status_t nError = ENOENT;

	if( nMailbox >= 0 && nMailbox > (int)m_vBoxes.size() )
		return EINVAL;

	for( uint nRow = 0; nRow < m_pcTreeView->GetRowCount(); nRow++ )
	{
		ListViewRow *pcRow = m_pcTreeView->GetRow( nRow );
		folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();
		if( NULL == psCookie )
			continue;
		const int nBox = psCookie->nBox;

		if( ( nMailbox >= 0 && nBox != nMailbox ) || nBox < 0 )
			continue;

		Mailbox *pcBox = m_vBoxes[nBox];
		Mailfolder *pcFolder = pcBox->OpenFolder( psCookie->cPath );

		if( pcFolder->GetPath() == cPath )
		{
			if( pcFolder->IsValid() == false )
			{
				nError = EINVAL;
				break;
			}
			else
			{
				*ppcFolder = pcFolder;
				nError = EOK;
				break;
			}

			break;
		}
		else
			delete( pcFolder );
	}

	return nError;
}

status_t WhisperWindow::FindFolderByType( Mailfolder **ppcFolder, folder_type_e nType, int nMailbox )
{
	return FindFolderRowByType( ppcFolder, NULL, nType, nMailbox );
}

status_t WhisperWindow::FindFolderRowByType( Mailfolder **ppcFolder, uint *pnRow, folder_type_e nType, int nMailbox )
{
	status_t nError = ENOENT;

	if( nMailbox >= 0 && nMailbox > (int)m_vBoxes.size() )
		return EINVAL;

	for( uint nRow = 0; nRow < m_pcTreeView->GetRowCount(); nRow++ )
	{
		ListViewRow *pcRow = m_pcTreeView->GetRow( nRow );
		folder_cookie_s *psCookie = (folder_cookie_s*)pcRow->GetCookie().AsPointer();
		if( NULL == psCookie )
			continue;
		const int nBox = psCookie->nBox;

		if( ( nMailbox >= 0 && nBox != nMailbox ) || nBox < 0 )
			continue;

		Mailbox *pcBox = m_vBoxes[nBox];
		Mailfolder *pcFolder = pcBox->OpenFolder( psCookie->cPath );
		FolderProperties *pcProperties = pcFolder->GetProperties();

		debug( "%s has type %Lu\n", psCookie->cPath.c_str(), pcProperties->GetType() );

		if( pcProperties->GetType() == (uint64)nType )
		{
			if( pcFolder->IsValid() == false )
			{
				nError = EINVAL;
				break;
			}
			else
			{
				*ppcFolder = pcFolder;
				if( pnRow != NULL )
					*pnRow = nRow;
				nError = EOK;
				break;
			}

			break;
		}
		else
			delete( pcFolder );
	}

	return nError;
}

/* Update the folder to display the number of unread messages.  Call with pcFolder locked. */
void WhisperWindow::UpdateUnread( Mailfolder *pcFolder, uint nRow, count_direction_t eDir )
{
	/* XXXKV: The dynamic casts & dereferencing make this fun */
	int64 nUnreadCount;
	String cDisplayName;
	ListViewRow *pcRow = m_pcTreeView->GetRow( nRow );
	folder_cookie_s *psCookie = (folder_cookie_s*)dynamic_cast<TreeViewStringNode*>(pcRow)->GetCookie().AsPointer();

	if( eDir == INC )
		nUnreadCount = pcFolder->IncUnreadCount();
	else
		nUnreadCount = pcFolder->DecUnreadCount();

	if( nUnreadCount > 0 )
		cDisplayName.Format( "%s [%d]", psCookie->cName.c_str(), nUnreadCount );
	else
		cDisplayName.Format( "%s", psCookie->cName.c_str() );
	dynamic_cast<TreeViewStringNode*>(pcRow)->SetString( 0, cDisplayName );
	m_pcTreeView->InvalidateRow( nRow, 0 );
}

/* Enable or disable the Cut/Copy/Paste Menus & Buttons */
void WhisperWindow::EnableMessagesEdit( bool bEnable )
{
	m_pcMessageSelectAll->SetEnable( bEnable );

	m_pcMessageCutItem->SetEnable( bEnable );
	m_pcMessageCutCtxItem->SetEnable( bEnable );

	m_pcMessageCopyItem->SetEnable( bEnable );
	m_pcMessageCopyCtxItem->SetEnable( bEnable );

	m_pcMessageDeleteItem->SetEnable( bEnable );
	m_pcMessageDeleteCtxItem->SetEnable( bEnable );

	if( NONE == m_eCopyMode )
	{
		m_pcMessagePasteItem->SetEnable( false );
		m_pcMessagePasteCtxItem->SetEnable( false );
		m_pcPasteButton->SetEnable( false );
	}
	else
	{
		m_pcMessagePasteItem->SetEnable( true );
		m_pcMessagePasteCtxItem->SetEnable( true );
		m_pcPasteButton->SetEnable( true );
	}

	m_pcMessageFlagItem->SetEnable( bEnable );
	m_pcMessageFlagCtxItem->SetEnable( bEnable );
	m_pcMessageMarkItem->SetEnable( bEnable );
	m_pcMessageMarkCtxItem->SetEnable( bEnable );

	/* Buttons */
	m_pcCutButton->SetEnable( bEnable );
	m_pcCopyButton->SetEnable( bEnable );
	m_pcDeleteButton->SetEnable( bEnable );
}

/* Enable or disable the Reply/Reply all/Forward Menus & Buttons */
void WhisperWindow::EnableMessagesReply( bool bEnable )
{
	m_pcMessageReplyItem->SetEnable( bEnable );
	m_pcMessageReplyAllItem->SetEnable( bEnable );
	m_pcMessageForwardItem->SetEnable( bEnable );

	/* The "View" items are enabled/disabled at the same time */
	m_pcMessageViewItem->SetEnable( bEnable );
	m_pcMessageViewDataItem->SetEnable( bEnable );

	/* Buttons */
	m_pcReplyButton->SetEnable( bEnable );
	m_pcReplyAllButton->SetEnable( bEnable );
	m_pcForwardButton->SetEnable( bEnable );
}

/* Enable or disable the folder management Menus & Buttons */
void WhisperWindow::EnableFoldersEdit( bool bEnable )
{
	m_pcFolderCreateItem->SetEnable( bEnable );
	m_pcFolderCreateCtxItem->SetEnable( bEnable );

	m_pcFolderRenameItem->SetEnable( bEnable );
	m_pcFolderRenameCtxItem->SetEnable( bEnable );

	m_pcFolderDeleteItem->SetEnable( bEnable );
	m_pcFolderDeleteCtxItem->SetEnable( bEnable );

	m_pcFolderPropertiesItem->SetEnable( bEnable );
	m_pcFolderPropertiesCtxItem->SetEnable( bEnable );

	/* Only enable the "Post to mailing list" item if the folder has a list */
	if( bEnable )
	{
		FolderProperties *pcProperties = m_pcFolder->GetProperties();
		if( pcProperties && ( pcProperties->GetFlags() & FL_HAS_LIST ) )
			m_pcFolderPostCtxItem->SetEnable( true );
		else
			m_pcFolderPostCtxItem->SetEnable( false );
	}
	else
		m_pcFolderPostCtxItem->SetEnable( false );
}

void WhisperWindow::_SaveGui( void )
{
	/* Store current GUI settings */
	m_pcGuiSettings->SetRect( "whisper", GetFrame() );
	m_pcGuiSettings->SetFloat( "whisper_v_ratio", m_pcVSplitter->GetSplitRatio() );
	m_pcGuiSettings->SetFloat( "whisper_h_ratio", m_pcHSplitter->GetSplitRatio() );
	m_pcGuiSettings->SetBool( "whisper_save_oc", m_pcSettingsSaveOCItem->IsChecked() );
}

