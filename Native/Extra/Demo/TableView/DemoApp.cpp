// TableTest	 (C)opyright 2006 Jonas Jarvoll
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

#include <util/application.h>
#include <gui/window.h>
#include <gui/button.h>
#include <gui/stringview.h>
#include <gui/textview.h>
#include <util/message.h>

#include "tableview.h"

using namespace os;

///////////////////////////////////////////////////////////7
//
// The Window
//
///////////////////////////////////////////////////////////7

class AppWindow : public Window
{
public:
	AppWindow( const Rect& cFrame );
	~AppWindow();
	virtual bool OkToQuit( void );
private:
	TableView* m_pcTable;

	Button* m_pcButton_clear;
	Button* m_pcButton_div;
	Button* m_pcButton_multi;
	Button* m_pcButton_minus;
	Button* m_pcButton_plus;
	Button* m_pcButton_equal;
	Button* m_pcButton_decimal;
	Button* m_pcButton_0;
	Button* m_pcButton_1;
	Button* m_pcButton_2;
	Button* m_pcButton_3;
	Button* m_pcButton_4;
	Button* m_pcButton_5;
	Button* m_pcButton_6;
	Button* m_pcButton_7;
	Button* m_pcButton_8;
	Button* m_pcButton_9;

	TextView* m_Result;
};

AppWindow :: AppWindow( const Rect& cFrame ) : Window( cFrame, "main_window", "TableTest" )
{	
	// Create the table
	m_pcTable = new TableView( GetBounds(), "", 4, 6, false, CF_FOLLOW_ALL );
	m_pcTable->SetRowSpacings( 3 );
	m_pcTable->SetColumnSpacings( 3 );
	AddChild( m_pcTable );

	// Create textview
	m_Result = new TextView( Rect(), "", "" );
	m_Result->SetReadOnly();
	m_pcTable->Attach( m_Result, 0, 4, 0, 1,  ( TABLE_EXPAND | TABLE_FILL ),  0, 4, 4 );

	// Create buttons
	m_pcButton_clear = new Button( Rect(), "", "C", NULL );
	m_pcTable->Attach( m_pcButton_clear, 0, 1, 1, 2 );

	m_pcButton_div = new Button( Rect(), "", "/", NULL );
	m_pcTable->Attach( m_pcButton_div, 1, 2, 1, 2 );

	m_pcButton_multi = new Button( Rect(), "", "*", NULL );
	m_pcTable->Attach( m_pcButton_multi, 2, 3, 1, 2 );

	m_pcButton_minus = new Button( Rect(), "", "-", NULL );
	m_pcTable->Attach( m_pcButton_minus, 3, 4, 1, 2 );

	m_pcButton_plus = new Button( Rect(), "", "+", NULL );
	m_pcTable->Attach( m_pcButton_plus, 3, 4, 2, 4 );

	m_pcButton_equal = new Button( Rect(), "", "=", NULL );
	m_pcTable->Attach( m_pcButton_equal, 3, 4, 4, 6 );

	m_pcButton_decimal = new Button( Rect(), "", ".", NULL );
	m_pcTable->Attach( m_pcButton_decimal, 2, 3, 5, 6 );

	m_pcButton_0 = new Button( Rect(), "", "0", NULL );
	m_pcTable->Attach( m_pcButton_0, 0, 2, 5, 6 );

	m_pcButton_1 = new Button( Rect(), "", "1", NULL );
	m_pcTable->Attach( m_pcButton_1, 0, 1, 4, 5 );

	m_pcButton_2 = new Button( Rect(), "", "2", NULL );
	m_pcTable->Attach(m_pcButton_2, 1, 2, 4, 5 );

	m_pcButton_3 = new Button( Rect(), "", "3", NULL );
	m_pcTable->Attach( m_pcButton_3, 2, 3, 4, 5 );

	m_pcButton_4 = new Button( Rect(), "", "4", NULL );
	m_pcTable->Attach( m_pcButton_4, 0, 1, 3, 4 );

	m_pcButton_5 = new Button( Rect(), "", "5", NULL );
	m_pcTable->Attach( m_pcButton_5, 1, 2, 3, 4 );

	m_pcButton_6 = new Button( Rect(), "", "6", NULL );
	m_pcTable->Attach( m_pcButton_6, 2, 3, 3, 4 );

	m_pcButton_7 = new Button( Rect(), "", "7", NULL );
	m_pcTable->Attach( m_pcButton_7, 0, 1, 2, 3 );

	m_pcButton_8 = new Button( Rect(), "", "8", NULL );
	m_pcTable->Attach( m_pcButton_8, 1, 2, 2, 3 );

	m_pcButton_9 = new Button( Rect(), "", "9", NULL );
	m_pcTable->Attach( m_pcButton_9, 2, 3, 2, 3 );
	
	// We need to manually call the Layout function after we have added children to the table
	m_pcTable->Layout();
}

AppWindow :: ~AppWindow()
{	
	delete m_pcTable;
	delete m_pcButton_clear;
	delete m_pcButton_div;
	delete m_pcButton_multi;
	delete m_pcButton_minus;
	delete m_pcButton_plus;
	delete m_pcButton_equal;
	delete m_pcButton_decimal;
	delete m_pcButton_0;
	delete m_pcButton_1;
	delete m_pcButton_2;
	delete m_pcButton_3;
	delete m_pcButton_4;
	delete m_pcButton_5;
	delete m_pcButton_6;
	delete m_pcButton_7;
	delete m_pcButton_8;
	delete m_pcButton_9;

	delete m_Result;
}

bool AppWindow :: OkToQuit( void )
{
	return true;
}

///////////////////////////////////////////////////////////7
//
// The Application
//
///////////////////////////////////////////////////////////7

class TestApp : public Application
{
	public:
		TestApp();   

	private:
		// The main window
		AppWindow* m_AppWindow;
};


TestApp :: TestApp() : Application( "application/tabletest" )
{
	m_AppWindow = new AppWindow( Rect( 100, 125, 250, 300 ) );
	m_AppWindow->Show();
	m_AppWindow->MakeFocus();
}

///////////////////////////////////////////////////////////7
//
// Main
//
///////////////////////////////////////////////////////////7

int main( int argc, char* argv[] )
{
 	TestApp* pcTest = new TestApp();
	pcTest->Run();

	return( 0 );
}



