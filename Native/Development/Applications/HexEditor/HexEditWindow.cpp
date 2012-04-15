                                                                                                                                                                                                       
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

#include "HexEditWindow.h"
#include "EditController.h"

#include <util/application.h>
#include <util/messenger.h>
#include <gui/image.h>
#include <util/resources.h>

enum MenuEvents
{
	ME_APP_ABOUT = 0x40000,
	ME_APP_SETTINGS,
	ME_APP_QUIT,
	
	ME_FILE_OPEN,
	ME_FILE_CLOSE,
	ME_FILE_SAVE,
	ME_FILE_SAVE_AS,
	
	ME_EDIT_UNDO,
	ME_EDIT_REDO,
	ME_EDIT_COPY,
	ME_EDIT_SELECT_ALL,
	ME_EDIT_SELECT_NONE,
	ME_EDIT_FIND
};

#define CURSOR_PANEL "cursor"
#define MESSAGE_PANEL "message"

#define IMG_TB_OPEN "fileopen.png"
#define IMG_TB_SAVE "filesave.png"
#define IMG_TB_SAVE_AS "filesaveas.png"
#define IMG_TB_COPY "editcopy.png"
#define IMG_TB_FIND "filefind.png"
#define IMG_ICON "icon24x24.png"

BitmapImage *load_image(const char *zName)
{
	BitmapImage *pcImg = NULL;
	
	Resources cRes(get_image_id());
	ResStream *pcStr = cRes.GetResourceStream(zName);
	if( pcStr != NULL )
	{
		pcImg = new BitmapImage();
		pcImg->Load(pcStr);
	}
	return pcImg;
}

HexEditWindow::HexEditWindow(const Rect &cFrame)
	: Window(cFrame, "HexEditorWindow", "")
{
	m_pcMenu = new Menu(Rect(), "m_pcMenu", ITEMS_IN_ROW, CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|CF_FOLLOW_TOP);
	Menu *pcAppMenu = new Menu(Rect(), STR_MNU_APP, ITEMS_IN_COLUMN);
	Menu *pcFileMenu = new Menu(Rect(), STR_MNU_FILE, ITEMS_IN_COLUMN);
	Menu *pcEditMenu = new Menu(Rect(), STR_MNU_EDIT, ITEMS_IN_COLUMN);
	Menu *pcHelpMenu = new Menu(Rect(), STR_MNU_HELP, ITEMS_IN_COLUMN);
	
	pcAppMenu->AddItem(STR_MNU_APP_ABOUT, new Message(ME_APP_ABOUT));
	
	// TODO: Make application configurable.
	//pcAppMenu->AddItem(new MenuSeparator());
	//pcAppMenu->AddItem("Settings...", new Message(ME_APP_SETTINGS));
	
	pcAppMenu->AddItem(new MenuSeparator());
	pcAppMenu->AddItem(new MenuItem(STR_MNU_APP_QUIT, new Message(ME_APP_QUIT), "CTRL+Q"));
	
	pcFileMenu->AddItem(new MenuItem(STR_MNU_FILE_OPEN, new Message(ME_FILE_OPEN), "CTRL+O"));
	pcFileMenu->AddItem(new MenuSeparator());
	pcFileMenu->AddItem(new MenuItem(STR_MNU_FILE_SAVE, new Message(ME_FILE_SAVE), "CTRL+S"));
	pcFileMenu->AddItem(STR_MNU_FILE_SAVE_AS, new Message(ME_FILE_SAVE_AS));
	pcFileMenu->AddItem(new MenuSeparator());
	pcFileMenu->AddItem(new MenuItem(STR_MNU_FILE_CLOSE, new Message(ME_FILE_CLOSE), "CTRL+W"));
	
	pcEditMenu->AddItem(new MenuItem(STR_MNU_EDIT_UNDO, new Message(ME_EDIT_UNDO), "CTRL+Z"));
	pcEditMenu->AddItem(new MenuItem(STR_MNU_EDIT_REDO, new Message(ME_EDIT_REDO), "CTRL+SHIFT+Z"));
	pcEditMenu->AddItem(new MenuSeparator());
	pcEditMenu->AddItem(new MenuItem(STR_MNU_EDIT_COPY, new Message(ME_EDIT_COPY), "CTRL+C"));
	pcEditMenu->AddItem(new MenuSeparator());
	pcEditMenu->AddItem(new MenuItem(STR_MNU_EDIT_SELECT_ALL, new Message(ME_EDIT_SELECT_ALL), "CTRL+A"));
	pcEditMenu->AddItem(STR_MNU_EDIT_SELECT_NONE, new Message(ME_EDIT_SELECT_NONE));
	pcEditMenu->AddItem(new MenuSeparator());
	pcEditMenu->AddItem(new MenuItem(STR_MNU_EDIT_FIND, new Message(ME_EDIT_FIND), "CTRL+F"));
	
	m_pcMenu->AddItem(pcAppMenu);
	m_pcMenu->AddItem(pcFileMenu);
	m_pcMenu->AddItem(pcEditMenu);
	m_pcMenu->AddItem(pcHelpMenu);
	
	Point cMenuSize(m_pcMenu->GetPreferredSize(false));
	m_pcMenu->SetFrame(Rect(0,0,cFrame.Width(),cMenuSize.y));
	
	m_pcStatusBar = new StatusBar(Rect(), "m_pcStatusBar", CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM);
	m_pcStatusBar->AddPanel(new StatusPanel(CURSOR_PANEL, "", 20, StatusPanel::F_FIXED));
	m_pcStatusBar->AddPanel(MESSAGE_PANEL, "");
	Point cStatusSize(m_pcStatusBar->GetPreferredSize(false));
	m_pcStatusBar->SetFrame(Rect(0, cFrame.Height() - cStatusSize.y, cFrame.Width(), cFrame.Height()));
		
	m_pcToolBar = new ToolBar(Rect(), "m_pcToolBar", CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|CF_FOLLOW_TOP);
	m_pcToolBar->AddButton("tbOpen", STR_TB_OPEN, load_image(IMG_TB_OPEN), new Message(ME_FILE_OPEN));
	m_pcToolBar->AddButton("tbSave", STR_TB_SAVE, load_image(IMG_TB_SAVE), new Message(ME_FILE_SAVE));
	m_pcToolBar->AddButton("tbSaveAs", STR_TB_SAVE_AS, load_image(IMG_TB_SAVE_AS), new Message(ME_FILE_SAVE_AS));
	m_pcToolBar->AddSeparator("sep_1");
	m_pcToolBar->AddButton("tbCopy", STR_TB_COPY, load_image(IMG_TB_COPY), new Message(ME_EDIT_COPY));
	m_pcToolBar->AddSeparator("sep_2");
	m_pcToolBar->AddButton("tbFind", STR_TB_FIND, load_image(IMG_TB_FIND), new Message(ME_EDIT_FIND));
	Point cTBSize(m_pcToolBar->GetPreferredSize(false));	
	m_pcToolBar->SetFrame(Rect(0,cMenuSize.y + 1, cFrame.Width(),cMenuSize.y + cTBSize.y + 1));
		
	m_pcHexView = new HexView(Rect(0,cMenuSize.y + cTBSize.y + 2,cFrame.Width(), cFrame.Height() - cStatusSize.y), "m_pcHexView");
	Point cHVSize(m_pcHexView->GetPreferredSize(false));

	EditController *pcController = new EditController(this, m_pcHexView);
	SetController(pcController);
	pcController->Run();

	AddChild(m_pcMenu);
	AddChild(m_pcToolBar);
	AddChild(m_pcHexView);
	AddChild(m_pcStatusBar);
	
	m_pcMenu->SetTargetForItems(this);
	
	ResizeTo(Point(cHVSize.x, cFrame.Height()));
	
	BitmapImage *pcIcon = load_image(IMG_ICON);
	SetIcon(pcIcon->LockBitmap());
	delete(pcIcon);
	
	SetFocusChild(m_pcHexView);
}

HexEditWindow::~HexEditWindow()
{
	m_pcController->Quit();
}

void HexEditWindow::HandleMessage(Message *pcMessage)
{
	int nEvent = 0;
	
	switch( pcMessage->GetCode() )
	{
		case ME_APP_ABOUT:
			nEvent = EV_ABOUT;
			break;
			
		case ME_APP_SETTINGS:
			nEvent = EV_SETTINGS;
			break;
			
		case ME_APP_QUIT:
			nEvent = EV_QUIT;
			break;
	
		case ME_FILE_OPEN:
			nEvent = EV_OPEN_FILE;
			break;
			
		case ME_FILE_CLOSE:
			nEvent = EV_CLOSE_FILE;
			break;
			
		case ME_FILE_SAVE:
			nEvent = EV_SAVE_FILE;
			break;
			
		case ME_FILE_SAVE_AS:
			nEvent = EV_SAVE_FILE_AS;
			break;
	
		case ME_EDIT_UNDO:
			nEvent = EV_UNDO;
			break;
			
		case ME_EDIT_REDO:
			nEvent = EV_REDO;
			break;
			
		case ME_EDIT_COPY:
			nEvent = EV_COPY;
			break;
			
		case ME_EDIT_SELECT_ALL:
			nEvent = EV_SELECT_ALL;
			break;
			
		case ME_EDIT_SELECT_NONE:
			nEvent = EV_SELECT_NONE;
			break;
			
		case ME_EDIT_FIND:
			nEvent = EV_FIND;
			break;
			
		default:
			break;
	}
			
	if( nEvent > 0 )
	{
		Messenger cMnger(m_pcController);
		cMnger.SendMessage(new Message(nEvent));
	}
}

void HexEditWindow::SetController(Looper *pcController)
{
	m_pcController = pcController;
}

void HexEditWindow::SetCursor(uint32 nCursor)
{
	static char zBuf[20];
	sprintf(zBuf, STR_FMT_CURSOR.c_str() , nCursor);
	m_pcStatusBar->SetText(CURSOR_PANEL, zBuf);
}

void HexEditWindow::SetMessage(const String &cMessage)
{
	m_pcStatusBar->SetText(MESSAGE_PANEL, cMessage);
}

bool HexEditWindow::OkToQuit(void)
{
	Messenger cMnger(m_pcController);
	cMnger.SendMessage(new Message(EV_QUIT));
	return false;
}






