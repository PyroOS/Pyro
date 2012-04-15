                                                                                                                                                                                                       
// Hex Editor - Copyright 2007 Andrew Kennan
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
#include <util/messenger.h>

#include "HexEditWindow.h"
#include "EditController.h"

using namespace os;

class HexEditor : public Application
{
	public:
		HexEditor(const char *zFile) 
			: Application("application/x-vnd.ADK.HexEditor")
		{
			SetCatalog("HexEditor.catalog");
			
			HexEditWindow *pcWin = new HexEditWindow(Rect(50,50,690,530));
			pcWin->Start();
			pcWin->Show(true);
			pcWin->MakeFocus(true);
			
			if( zFile != NULL )
			{
				Message *pcMsg = new Message(EV_OPEN_FILE_SELECTED);
				pcMsg->AddString("file/path", zFile);
				Messenger cMnger(pcWin->GetController());
				cMnger.SendMessage(pcMsg);
			}
		}
};

int main(int argc, const char **argv)
{
	const char *zFile = NULL;
	if( argc > 1 )
		zFile = argv[1];
	HexEditor *pcApp = new HexEditor(zFile);
	pcApp->Run();
}




