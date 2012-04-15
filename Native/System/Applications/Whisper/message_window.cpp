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

#include <gui/image.h>
#include <util/resources.h>

#include <message_window.h>
#include <strfuncs.h>

using namespace os;

MessageWindow::MessageWindow( const Rect cFrame, String cTitle, const Settings *pcVisualSettings, Mailmessage *pcMailMessage ) : Window( cFrame, "whisper_view_message", cTitle )
{
	m_pcMessageView = new MessageView( GetBounds(), "whisper_message_window_view", this, pcVisualSettings );
	AddChild( m_pcMessageView );

	/* Set a window icon */
	BitmapImage *pcWindowIcon = new BitmapImage();
	Resources cRes( get_image_id() );
	pcWindowIcon->Load( cRes.GetResourceStream( "icon24x24.png" ) );
	SetIcon( pcWindowIcon->LockBitmap() );
	delete( pcWindowIcon );

	/* Display the message */
	pcMailMessage->Parse();
	m_pcMessageView->Display( pcMailMessage );
}

MessageWindow::~MessageWindow()
{
	RemoveChild( m_pcMessageView );
	delete( m_pcMessageView );
}

bool MessageWindow::OkToQuit( void )
{
	return true;
}

MessageDataWindow::MessageDataWindow( const Rect cFrame, String cTitle, Mailmessage *pcMailMessage ) : Window( cFrame, "whisper_view_message", cTitle )
{
	/* Strip carriage returns from the message data */
	size_t nSize = pcMailMessage->GetDataSize();
	char *pzData = (char*)calloc( 1, nSize );
	if( NULL != pzData )
		pzData = xstrncpy_to_unix( pzData, pcMailMessage->GetData(), nSize );

	m_pcTextView = new TextView( GetBounds(), "whisper_message_window_textview", pzData, CF_FOLLOW_ALL );
	m_pcTextView->SetMultiLine( true );
	m_pcTextView->SetEnable( false );
	AddChild( m_pcTextView );

	free( pzData );

	/* Set a window icon */
	BitmapImage *pcWindowIcon = new BitmapImage();
	Resources cRes( get_image_id() );
	pcWindowIcon->Load( cRes.GetResourceStream( "icon24x24.png" ) );
	SetIcon( pcWindowIcon->LockBitmap() );
	delete( pcWindowIcon );
}

MessageDataWindow::~MessageDataWindow()
{
	RemoveChild( m_pcTextView );
	delete( m_pcTextView );
}

bool MessageDataWindow::OkToQuit( void )
{
	return true;
}

