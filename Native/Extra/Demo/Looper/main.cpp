// EFileBrowser -:-  (C)opyright 2006 Jonas Jarvoll
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

#include <iostream>

#include "main.h"

using namespace os;
using namespace std;

int main( int argc, char* argv[] )
{
 	SBasicApp* pcBrowser = new SBasicApp( argc, argv );
	pcBrowser->Run();

	return( 0 );
}

AppWindow* SBasicApp :: m_AppWindow = NULL;

SBasicApp :: SBasicApp( int argc, char* argv[] ) : Application( "application/SBasic" )
{
	m_AppWindow = new AppWindow( Rect( 100, 125, 400, 500 ) );
	m_AppWindow->Show();
	m_AppWindow->MakeFocus();
}


