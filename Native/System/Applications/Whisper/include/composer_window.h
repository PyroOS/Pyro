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

#ifndef WHISPER_SYLLABLE__COMPOSER_WINDOW_H_
#define WHISPER_SYLLABLE__COMPOSER_WINDOW_H_

#include <gui/window.h>
#include <gui/button.h>
#include <gui/stringview.h>
#include <gui/textview.h>
#include <gui/menu.h>
#include <gui/checkmenu.h>
#include <gui/listview.h>
#include <util/string.h>
#include <util/message.h>
#include <util/settings.h>
#include <gui/toolbar.h>
#include <gui/imagebutton.h>

#include <mail.h>
#include <inputview.h>
#include <identity.h>

using namespace os;

enum
{
	ADDR_TO = 0,
	ADDR_CC,
	ADDR_BCC,
	ADDR_SUBJECT
};

class AddressBar : public View
{
	public:
		AddressBar( const Rect cFrame );
		~AddressBar();

		virtual Point GetPreferredSize( bool bLargest );

		status_t ShowAddress( int nId, bool bShow = true );

		status_t GetText( int nId, String &cText );
		status_t SetText( int nId, String cText );

		status_t SetFocus( int nId );

	private:
		void _GetPosition( Rect &cFrame, int nIndex );

		InputView *m_apcAddress[4];
		bool m_abShown[4];

		float m_vHeight;
};

enum composer_mode
{
	COMPOSE_NEW,
	COMPOSE_REPLY,
	COMPOSE_REPLY_ALL,
	COMPOSE_FORWARD
};

class ComposerWindow : public Window
{
	public:
		ComposerWindow( const Rect &cFrame, const String cTitle, Handler *pcParent, Settings *pcGuiSettings, Identity *pcIdentity, int nMode = COMPOSE_NEW, Mailmessage *pcEditMessage = NULL, int nMailbox = 0, String cFolder = "" );

		~ComposerWindow();
		void HandleMessage( Message *pcMessage );
		bool OkToQuit( void );

		void SetSubject( const String cSubject );
		void SetTo( const String cTo );
		void SetCc( const String cCc );
		void SetBcc( const String cBcc );
		void SetBody( const String cBody );

		status_t AddAttachment( const String cFilename );

	private:
		void _ResizeTextView( void );
		void _ShowAttachments( void );
		status_t _SetupReply( void );
		String _IndentBody( const char *pzBody );
		void _SaveGui( void );

		Handler *m_pcParent;
		Settings *m_pcGuiSettings;
		Identity *m_pcIdentity;
		int m_nMode;
		Mailmessage *m_pcEditMessage;
		int m_nMailbox;
		String m_cFolder;

		Menu *m_pcMenuBar;
		Menu *m_pcMessageMenu;
		Menu *m_pcEditMenu;
		Menu *m_pcViewMenu;
		Menu *m_pcInsertMenu;
		Menu *m_pcInsertSignatureMenu;
		CheckMenu *m_pcViewMenuTo;
		CheckMenu *m_pcViewMenuCc;
		CheckMenu *m_pcViewMenuBcc;

		float m_vMenuHeight;

		ToolBar *m_pcToolBar;
		ImageButton *m_pcRemoveButton;

		float m_vToolBarHeight;

		AddressBar *m_pcAddressBar;
		TextView *m_pcMessageText;

		ListView *m_pcAttachments;
		Menu *m_pcAttachmentsMenu;
		bool m_bAttachmentsShown;
};

#endif

