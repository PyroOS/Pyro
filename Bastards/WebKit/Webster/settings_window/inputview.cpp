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

#include <inputview.h>

InputView::InputView( const Rect cFrame, const String cLabel, const uint32 nLabelWidth, const bool bButton, Message *pcMessage ) : View( cFrame, "whisper_input", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT )
{
	m_pcButton = NULL;
	m_pcStringView = NULL;
	m_pcTextView = NULL;

	Rect cLabelFrame;
	cLabelFrame.left = 0;
	cLabelFrame.top = 0;
	cLabelFrame.right = nLabelWidth;
	cLabelFrame.bottom = cLabelFrame.top + 20;

	if( bButton && NULL != pcMessage )
	{
		/* Create a button to the left of the text view */
		m_pcButton = new Button( cLabelFrame, "input_button", cLabel, pcMessage );
		AddChild( m_pcButton );
		m_pcButton->SetTabOrder( NEXT_TAB_ORDER );
	}
	else
	{
		/* Create a label to the left of the text view */
		m_pcStringView = new StringView( cLabelFrame, "input_label", cLabel, ALIGN_LEFT, CF_FOLLOW_LEFT );
		AddChild( m_pcStringView );
	}

	/* Create the text view */
	Rect cTextFrame;
	cTextFrame.left = cLabelFrame.right + 5;
	cTextFrame.top = cLabelFrame.top;
	cTextFrame.right = cFrame.Width() - 5;
	cTextFrame.bottom = cLabelFrame.bottom;

	m_pcTextView = new TextView( cTextFrame, "input_text", "", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT );
	AddChild( m_pcTextView );
	m_pcTextView->SetTabOrder( NEXT_TAB_ORDER );
}

InputView::~InputView()
{
	if( m_pcButton )
	{
		RemoveChild( m_pcButton );
		delete( m_pcButton );
	}

	if( m_pcStringView )
	{
		RemoveChild( m_pcStringView );
		delete( m_pcStringView );
	}

	if( m_pcTextView )
	{
		RemoveChild( m_pcTextView );
		delete( m_pcTextView );
	}
}

void InputView::SetPasswordMode( bool bPassword )
{
	m_pcTextView->SetPasswordMode( bPassword );
}

void InputView::SetMultiLine( bool bMultiLine )
{
	m_pcTextView->SetMultiLine( bMultiLine );
	if( bMultiLine )
	{
		Rect cTextFrame;

		cTextFrame = m_pcTextView->GetFrame();
		cTextFrame.bottom = GetBounds().bottom;
		m_pcTextView->SetFrame( cTextFrame );
	}
}

status_t InputView::SetTarget( const Handler *pcHandler, const Looper *pcLooper )
{
	return m_pcTextView->SetTarget( pcHandler, pcLooper );
}

String InputView::GetText( void )
{
	return m_pcTextView->GetValue().AsString();
}

void InputView::SetText( String cText )
{
	m_pcTextView->SetValue( cText, false );
}

void InputView::Clear( bool bSendNotify )
{
	m_pcTextView->Clear( bSendNotify );
}

void InputView::SetFocus()
{
	m_pcTextView->MakeFocus( true );
}

void InputView::SetEnable( bool bEnabled )
{
	if( m_pcButton )
		m_pcButton->SetEnable( bEnabled );
	m_pcTextView->SetEnable( bEnabled );
}

