                                                                                                                                                                                                       
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

#ifndef __HEX_EDIT_WINDOW__
#define __HEX_EDIT_WINDOW__

#include <gui/window.h>
#include <gui/toolbar.h>
#include <gui/statusbar.h>
#include <gui/menu.h>
#include <util/looper.h>

#include "HexView.h"

using namespace os;

class HexEditWindow : public Window
{
	public:
		HexEditWindow(const Rect &cFrame);
		~HexEditWindow();
		
		void HandleMessage(Message *pcMessage);
		
		void SetController(Looper *pcController);
		Looper *GetController(void) const { return m_pcController; }
		void SetCursor(uint32 nCursor);
		void SetMessage(const String &cMessage);
				
		bool OkToQuit(void);
	private:
		Looper *m_pcController;
		String m_cFileName;
		
		Menu *m_pcMenu;
		ToolBar *m_pcToolBar;
		HexView *m_pcHexView;
		StatusBar *m_pcStatusBar;
};

#endif




