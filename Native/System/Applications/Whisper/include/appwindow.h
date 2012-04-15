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

#ifndef WHISPER_SYLLABLE__APPWINDOW_H_
#define WHISPER_SYLLABLE__APPWINDOW_H_

#include <pyro/types.h>
#include <util/message.h>
#include <util/string.h>
#include <util/settings.h>
#include <util/event.h>
#include <gui/window.h>
#include <gui/rect.h>
#include <gui/listview.h>
#include <gui/treeview.h>
#include <gui/splitter.h>
#include <gui/menu.h>
#include <gui/checkmenu.h>
#include <gui/statusbar.h>
#include <gui/toolbar.h>
#include <gui/imagebutton.h>

#include <vector>
#include <list>
#include <mailbox.h>
#include <syllable_mailbox.h>
#include <message_view.h>
#include <transport.h>
#include <identity.h>
#include <ixport.h>
#include <filter.h>

using namespace os;

const int MAX_TIMER_MSGS = 256;

class WhisperWindow : public Window
{
	public:
		WhisperWindow( const Rect &cFrame );
		~WhisperWindow();
		void HandleMessage( Message *pcMessage );
		virtual void TimerTick( int nId );
		bool OkToQuit( void );
	private:
		/* Message handlers */

		void HandleSelect( Message *pcMessage );
		void HandleFolderSelect( Message *pcMessage );

		void HandleCreateFolderDialog( Message *pcMessage );
		void HandleRenameFolderDialog( Message *pcMessage );
		void HandleDeleteFolder( Message *pcMessage );

		void HandlePropertiesFolder( Message *pcMessage );
		void HandlePropertiesFolderDialog( Message *pcMessage );

		void HandleCutMessage( Message *pcMessage );
		void HandleCopyMessage( Message *pcMessage );
		void HandlePasteMessage( Message *pcMessage );
		void HandleDeleteMessage( Message *pcMessage );

		void HandleFlagMessage( Message *pcMessage );
		void HandleMarkMessage( Message *pcMessage );

		void HandleStatusUpdate( Message *pcMessage );

		void HandleCheckForMail( void );

		void HandleImportComplete( Message *pcMessage );

		void HandleComposeMessageComplete( Message *pcMessage );
		void HandleCompose( Message *pcMessage );

		void HandleMailSent( Message *pcMessage );

		void HandleAppAbout( Message *pcMessage );

		void HandleImportFile( Message *pcMessage );
		void HandleImportNew( Message *pcMessage );
		void HandleImportCreateFolder( Message *pcMessage );
		void HandleExportFile( Message *pcMessage );

		void HandleEmptyTrash( Message *pcMessage );

		void HandleGetFolderList( Message *pcMessage );

		/* Event handlers */
		void HandleEventMailSend( Message *pcMessage );
		void HandleEventMailGetCount( Message *pcMessage );
		void HandleEventMailCheck( Message *pcMessage );

		/* Other methods */

		status_t LoadIdentity( Identity *pcIdentity );

		status_t LoadMailboxes( void );
		status_t AddMailbox( int nBox );
		status_t AddChildFolder( Mailfolder *pcParent, int nParentIndex, int nDepth );

		status_t AddMessage( Mailmessage *pcMailMessage );

		status_t ScanFolder( String cPath );

		status_t FindFolder( Mailfolder **ppcFolder, os::String cName, int nMailbox = -1 );
		status_t FindFolderRow( Mailfolder **ppcFolder, uint *pnRow, os::String cName, int nMailbox = -1 );
		status_t FindFolderByPath( Mailfolder **ppcFolder, os::String cPath, int nMailbox = -1 );
		status_t FindFolderByType( Mailfolder **ppcFolder, folder_type_e nType, int nMailbox = -1 );
		status_t FindFolderRowByType( Mailfolder **ppcFolder, uint *pnRow, folder_type_e nType, int nMailbox = -1 );

		enum count_direction_t
		{
			INC,
			DEC
		};
		void UpdateUnread( Mailfolder *pcFolder, uint nRow, count_direction_t eDir );

		void EnableMessagesEdit( bool bEnable = true );
		void EnableMessagesReply( bool bEnable = true );
		void EnableFoldersEdit( bool bEnable = true );

		void _SaveGui( void );

		Settings *m_pcGuiSettings;
		Settings *m_pcVisualSettings;

		Event *m_pcSendEvent;
		Event *m_pcGetEvent;
		Event *m_pcCheckEvent;

		Rect m_cWhisperFrame;
		Rect m_cComposerFrame;
		Rect m_cSettingsFrame;

		Splitter *m_pcVSplitter;
		Splitter *m_pcHSplitter;

		Menu *m_pcMenuBar;
		Menu *m_pcApplicationMenu;
		Menu *m_pcMessageMenu;
		MenuItem *m_pcMessageReplyItem;
		MenuItem *m_pcMessageReplyAllItem;
		MenuItem *m_pcMessageForwardItem;
		MenuItem *m_pcMessageViewItem;
		MenuItem *m_pcMessageViewDataItem;
		MenuItem *m_pcMessageSelectAll;
		MenuItem *m_pcMessageCutItem;
		MenuItem *m_pcMessageCopyItem;
		MenuItem *m_pcMessagePasteItem;
		MenuItem *m_pcMessageDeleteItem;
		MenuItem *m_pcMessageFlagItem;
		MenuItem *m_pcMessageMarkItem;
		Menu *m_pcEditMenu;
		MenuItem *m_pcEditCopy;
		Menu *m_pcFolderMenu;
		MenuItem *m_pcFolderCreateItem;
		MenuItem *m_pcFolderRenameItem;
		MenuItem *m_pcFolderPropertiesItem;
		MenuItem *m_pcFolderDeleteItem;
		Menu *m_pcSettingsMenu;
		CheckMenu *m_pcSettingsSaveOCItem;

		float m_vMenuHeight;

		ToolBar *m_pcToolBar;
		ImageButton *m_pcCutButton;
		ImageButton *m_pcCopyButton;
		ImageButton *m_pcPasteButton;
		ImageButton *m_pcDeleteButton;
		ImageButton *m_pcReplyButton;
		ImageButton *m_pcReplyAllButton;
		ImageButton *m_pcForwardButton;

		struct folder_cookie_s
		{
			String cName;
			String cPath;
			int32 nBox;
		};

		TreeView *m_pcTreeView;
		Menu *m_pcTreeViewMenu;
		MenuItem *m_pcFolderCreateCtxItem;
		MenuItem *m_pcFolderRenameCtxItem;
		MenuItem *m_pcFolderDeleteCtxItem;
		MenuItem *m_pcFolderPropertiesCtxItem;
		MenuItem *m_pcFolderPostCtxItem;

		TreeView *m_pcMessagesList;
		Menu *m_pcListViewMenu;
		MenuItem *m_pcMessageCutCtxItem;
		MenuItem *m_pcMessageCopyCtxItem;
		MenuItem *m_pcMessagePasteCtxItem;
		MenuItem *m_pcMessageDeleteCtxItem;
		MenuItem *m_pcMessageFlagCtxItem;
		MenuItem *m_pcMessageMarkCtxItem;

		MessageView *m_pcMessageView;

		StatusBar *m_pcStatusBar;

		Identity *m_pcIdentity;

		Mailfolder *m_pcFolder;
		Mailmessage *m_pcMailMessage;

		Variant m_cCurrentReference;
		int m_nCurrentSelection;

		String m_cCurrentFolderPath;
		int m_nCurrentFolder;
		int m_nInbox;

		std::vector <Mailbox*>m_vBoxes;
		Mailbox *m_pcCurrentBox;

		std::vector <Transport*>m_vInboundTransports;
		Transport *m_pcOutboundTransport;

		int16 m_nAuthTransport;

		enum copy_mode_t
		{
			NONE,
			COPY,
			CUT
		};
		copy_mode_t m_eCopyMode;
		std::list<Mailmessage *> m_vCopyMessages;

		Message *m_apcTimerMessages[MAX_TIMER_MSGS];

		IXPlugin *m_pcImportPlugin;

		FilterEngine *m_pcFilterEngine;
};

#endif

