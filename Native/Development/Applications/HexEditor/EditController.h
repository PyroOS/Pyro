                                                                                                                                                                                                       
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

#ifndef __EDIT_CONTROLLER_H__
#define __EDIT_CONTROLLER_H__

#include <util/looper.h>
#include <util/string.h>
#include <util/message.h>
#include <gui/window.h>
#include <gui/filerequester.h>
#include <storage/nodemonitor.h>

#include "FindDialogue.h"

#include "Strings.h"

#define APP_NAME STR_APP_NAME

enum EventType
{
	EV_ABOUT = 0x50000,
	EV_SETTINGS,
	EV_QUIT,
	
	EV_OPEN_FILE,
	EV_SAVE_FILE,
	EV_SAVE_FILE_AS,
	EV_CLOSE_FILE,
	
	EV_COPY,
	EV_UNDO,
	EV_REDO,
	EV_SELECT_ALL,
	EV_SELECT_NONE,
	
	EV_FIND,
	
	EV_CURSOR_MOVED,
	EV_FILE_CHANGED,
	
	EV_SHOW_MESSAGE,
	
	EV_FIND_PREV,
	EV_FIND_NEXT,
	
	EV_OPEN_FILE_SELECTED
};

#define FIND_TYPE_KEY "type"
#define FIND_TEXT_KEY "text"
#define MESSAGE_TEXT_KEY "text"


using namespace os;

class HexView;
class HexEditWindow;

class EditController : public Looper
{
	public:
		EditController(HexEditWindow *pcWin, HexView *pcHexView);
		~EditController();
			
		void HandleMessage(Message *pcMessage);

		const String &GetFileName(void) const  { return m_cFileName; }
		
	private:
		HexEditWindow *m_pcWin;
		HexView *m_pcHexView;	
		String m_cFileName;
		bool m_bFileChanged;
		FileRequester *m_pcOpenReq;
		FileRequester *m_pcSaveReq;
		FindDialogue *m_pcFindDialogue;
		NodeMonitor *m_pcNodeMon;
		uint8 m_nMonCount;
		bool m_bSaving;
				
		void ShowAboutMessage(void);
		void ShowSettingsDialogue(void);
		void ShowOpenDialogue(void);
		void ShowSaveDialogue(void);
		void OpenFile(Message *pcMessage);
		void OpenFile(const String &cFileName);
		void SaveFile(void);
		void SaveFileAs(Message *pcMessage);
		void PerformSave(const String &cFileName);
		void ShowFindDialogue(void);
		bool CloseFile(void);
		void ShowMessage(Message *pcMessage);
		void ShowMessage(const String &cText);
		void FindPrev(Message *pcMessage);
		void FindNext(Message *pcMessage);
		void UpdateWindowTitle(void);
		void FileChanged(void);
};


#endif








