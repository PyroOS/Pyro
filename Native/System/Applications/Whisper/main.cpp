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

#include <stdlib.h>
#include <gui/rect.h>
#include <util/application.h>

#include <appwindow.h>

/* Run-time debug switch */
int g_nDebug = 1;

using namespace os;

class WhisperApplication : public Application
{
	public:
		WhisperApplication();

	private:
		WhisperWindow *m_pcWindow;
};

WhisperApplication::WhisperApplication() : Application( "application/x-VND.pyro-Whisper" )
{
	SetCatalog("Whisper.catalog");

	m_pcWindow = new WhisperWindow( Rect( 50,50,875,500 ) );
	m_pcWindow->Show();
	m_pcWindow->MakeFocus();
}

int main( int argc, char **argv )
{
	WhisperApplication *pcApplication = new WhisperApplication();
	pcApplication->Run();

	return EXIT_SUCCESS;
}

