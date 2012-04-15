                                                                                                                                                                                                       
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

#ifndef __FIND_DIALOGUE_H__
#define __FIND_DIALOGUE_H__

#include <gui/window.h>
#include <gui/dropdownmenu.h>
#include <util/looper.h>

using namespace os;

class FindDialogue : public Window
{
	public:
		FindDialogue(Looper *pcController);
		~FindDialogue();
		
		bool OkToQuit(void);
		
		void HandleMessage(Message *pcMessage);
		
	private:
		Looper *m_pcController;
		
		DropdownMenu *m_pcFindText;
		DropdownMenu *m_pcFindType;
};

#endif

