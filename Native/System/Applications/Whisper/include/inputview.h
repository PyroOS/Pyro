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

#ifndef WHISPER_SYLLABLE__INPUTVIEW_H_
#define WHISPER_SYLLABLE__INPUTVIEW_H_

#include <gui/stringview.h>
#include <gui/button.h>
#include <gui/textview.h>
#include <util/string.h>
#include <util/message.h>

using namespace os;

class InputView : public View
{
	public:
		InputView( const Rect cFrame, const String cLabel, const uint32 nLabelWidth = 50, const bool bButton = false, Message *pcMessage = NULL );
		~InputView();

		void SetPasswordMode( bool bPassword = true );
		void SetMultiLine( bool bMultiLine = true );

		status_t SetTarget( const Handler *pcHandler, const Looper *pcLooper = NULL );

		String GetText( void );
		void SetText( String cText );
		void Clear( bool bSendNotify = true );

		void SetFocus();
		virtual void SetEnable( bool bEnabled );

	private:
		Button *m_pcButton;
		StringView *m_pcStringView;
		TextView *m_pcTextView;
};


#endif

