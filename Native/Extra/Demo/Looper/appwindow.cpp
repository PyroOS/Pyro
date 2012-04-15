// EFileBrowser	 (C)opyright 2006 Jonas Jarvoll
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "appwindow.h"
#include "messages.h"
#include <storage/file.h>

using namespace os;
using namespace std;

AppWindow :: AppWindow( const Rect& cFrame ) : Window( cFrame, "main_window", "Test window" )
{	
	// Add Start button
	m_Start = new Button( Rect(), "button_add_message", "Start", new Message( MSG_BTN_START ) );
	m_Start->SetTarget( this );
	AddChild( m_Start );

	// Add Stop button
	m_Stop = new Button( Rect(), "button_add_message", "Stop", new Message( MSG_BTN_STOP ) );
	m_Stop->SetTarget( this );
	AddChild( m_Stop );

	// Add textviw
	m_Text = new TextView( Rect(), "textview", "" );
	m_Text->SetMultiLine( true );
	m_Text->SetReadOnly( true );
	AddChild( m_Text );

	// Fix layout
	Layout();

	// Set up new comm thread
	m_CommThread = new CommThread( this );
	m_CommThread->Run();
}

AppWindow :: ~AppWindow()
{
	m_CommThread->PostMessage( os::M_QUIT );
	m_CommThread = NULL;

	delete m_Start;
	delete m_Stop;
	delete m_Text;
}

void AppWindow :: Layout()
{
	float offset_x = 4.0f;
	float offset_y = 4.0f;

	// Get size of the window
	Rect cFrame = GetBounds();

	// Calculate size of the Start button
	Point cBtnStart = m_Start->GetPreferredSize( false );
	Rect cStartFrame( offset_x, offset_y, offset_x + cBtnStart.x, offset_y + cBtnStart.y );
	m_Start->SetFrame( cStartFrame );

	// Calculate size of the Stop button
	Point cBtnStop = m_Start->GetPreferredSize( false );
	Rect cStopFrame( cStartFrame.right + offset_x, offset_y, cStartFrame.right + cBtnStop.x, offset_y + cBtnStop.y );
	m_Stop->SetFrame( cStopFrame );

	// Calculate size of the textview
	Rect cTextFrame( offset_x, cStopFrame.bottom + offset_y, cFrame.right - offset_x, cFrame.bottom - offset_y );
	m_Text->SetFrame( cTextFrame );


}

void AppWindow :: FrameSized( const Point& delta )
{
	Window::FrameSized( delta );

	Layout();
}

void AppWindow :: HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )	//Get the message code from the message
	{		
		case MSG_BTN_START:
		{	
			m_Text->Clear();
			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_START, m_CommThread, m_CommThread ); 
			break;
		}
		case MSG_BTN_STOP:
		{
			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_STOP, m_CommThread, m_CommThread ); 
			break;
		}
		case MSG_FROMLOOPER_NEW_MESSAGE:
		{
			const char *pzName;
			int nCount = 0;

			pcMessage->GetNameInfo( "name", NULL, &nCount );

			for( int i = 0; i < nCount ; ++i )
			{
				if( pcMessage->FindString( "name", &pzName, i ) == 0 )
				{				
					AddStringToTextView( String( pzName ) );
					break;
				}
			}

			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_ACK, m_CommThread, m_CommThread ); 
			break;
		}
		default:
		{
			Window::HandleMessage( pcMessage );
			break;
		}
	}
}

bool AppWindow :: OkToQuit( void )
{
	return true;
}

void AppWindow :: AddStringToTextView( String name )
{
	String t = m_Text->GetValue().AsString().c_str();
	if( t == "" )
		t = name;
	else	
		t = t + "\n" + name;

	m_Text->Set( t.c_str() );
}
