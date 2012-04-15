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

#ifndef WHISPER_MESSAGE_WINDOW_H_
#define WHISPER_MESSAGE_WINDOW_H_

#include <gui/window.h>
#include <gui/rect.h>
#include <gui/textview.h>
#include <util/string.h>
#include <util/settings.h>

#include <message_view.h>
#include <mail.h>

class MessageWindow : public Window
{
	public:
		MessageWindow( const os::Rect cFrame, os::String cTitle, const Settings *pcVisualSettings, Mailmessage *pcMailMessage );
		~MessageWindow();

		bool OkToQuit( void );

	private:
		MessageView *m_pcMessageView;
};

class MessageDataWindow : public Window
{
	public:
		MessageDataWindow( const os::Rect cFrame, os::String cTitle, Mailmessage *pcMailMessage );
		~MessageDataWindow();

		bool OkToQuit( void );

	private:
		TextView *m_pcTextView;
};

#endif

