                                                                                                                                                                                                       
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

#include "EditController.h"
#include "HexView.h"
#include "HexEditWindow.h"

#include <gui/requesters.h>
#include <util/application.h>
#include <util/variant.h>
#include <storage/file.h>

enum PrivateEvents
{
	// Invoked when the user has selected a file to save as.
	EV_SAVE_FILE_SELECTED,
};

EditController::EditController(HexEditWindow *pcWin, HexView *pcHexView)
			: Looper("EditController")
			, m_pcWin(pcWin)
			, m_pcHexView(pcHexView)
			, m_cFileName("")
			, m_bFileChanged(false)
			, m_pcOpenReq(NULL)
			, m_pcSaveReq(NULL)
			, m_pcFindDialogue(NULL)
			, m_pcNodeMon(NULL)
			, m_nMonCount(0)
			, m_bSaving(false)
{ 
	// Ensure the window and main view have a pointer to this controller.
	m_pcWin->SetController(this);
	m_pcHexView->SetController(this);
	
	UpdateWindowTitle();
}

EditController::~EditController()
{ 

}

void EditController::HandleMessage(Message *pcMessage)
{
	switch(  pcMessage->GetCode() )
	{
		case EV_ABOUT:
			ShowAboutMessage();
			break;
			
		case EV_SETTINGS:
			ShowSettingsDialogue();
			break;
			
		case EV_QUIT:
			if( CloseFile() )
				Application::GetInstance()->PostMessage(M_QUIT);
			break;
		
		case EV_OPEN_FILE:
			ShowOpenDialogue();
			break;
			
		case EV_OPEN_FILE_SELECTED:
			OpenFile(pcMessage);
			break;
			
		case EV_SAVE_FILE:
			SaveFile();
			break;
			
		case EV_SAVE_FILE_AS:
			ShowSaveDialogue();
			break;
			
		case EV_SAVE_FILE_SELECTED:
			SaveFileAs(pcMessage);
			break;
			
		case EV_CLOSE_FILE:
			CloseFile();	
			break;
			
		case EV_COPY:
			m_pcHexView->Copy();
			break;
			
		case EV_UNDO:
			m_pcHexView->Undo();
			break;
			
		case EV_REDO:
			m_pcHexView->Redo();
			break;
			
		case EV_SELECT_ALL:
			m_pcHexView->SelectAll();
			break;
			
		case EV_SELECT_NONE:
			m_pcHexView->SelectNone();
			break;
		
		case EV_FIND:
			ShowFindDialogue();
			break;
		
		case EV_CURSOR_MOVED:
			m_pcWin->SetCursor((uint32)(m_pcHexView->GetCursor() - m_pcHexView->GetBuffer()));
			break;
			
		case EV_FILE_CHANGED:
			m_bFileChanged = true;
			UpdateWindowTitle();
			break;
			
		case EV_SHOW_MESSAGE:
			ShowMessage(pcMessage);
			break;
			
		case EV_FIND_NEXT:
			FindNext(pcMessage);
			break;
			
		case EV_FIND_PREV:
			FindPrev(pcMessage);
			break;
			
		case M_NODE_MONITOR:
			// Annoyingly the NodeMonitor will send us three messages
			// when the underlying file has changed so we want to 
			// act on the first and ignore the other two.
			m_nMonCount++;
			if( m_nMonCount == 1 && ! m_bSaving )
				FileChanged();
			else if( m_nMonCount == 3 )
				m_nMonCount = 0;
			break;
			
		default:
			break;
	}
}

void EditController::ShowAboutMessage(void)
{
	Alert *pcAlert = new Alert(STR_ABOUT_DLG_TITLE + APP_NAME, STR_ABOUT_DLG_MSG, Alert::ALERT_INFO, 0, STR_ABOUT_DLG_BTN_CLOSE.c_str(), NULL);
	pcAlert->Go(NULL);
}

void EditController::ShowSettingsDialogue(void)
{
}

void EditController::ShowSaveDialogue(void)
{
	if( m_cFileName != "" )
	{
		if( m_pcSaveReq == NULL )
		{
			m_pcSaveReq = new FileRequester(FileRequester::SAVE_REQ, new Messenger(this), m_cFileName, 
				FileRequester::NODE_FILE, false, new Message(EV_SAVE_FILE_SELECTED), NULL, false, true);
		}
			
		m_pcSaveReq->Show(true);
		m_pcSaveReq->MakeFocus(true);
	}
}

void EditController::ShowOpenDialogue(void)
{
	if( m_pcOpenReq == NULL )
	{
		m_pcOpenReq = new FileRequester(FileRequester::LOAD_REQ, new Messenger(this), "", 
			FileRequester::NODE_FILE, false, new Message(EV_OPEN_FILE_SELECTED), NULL, false, true);
	}
	
	m_pcOpenReq->Show(true);
	m_pcOpenReq->MakeFocus(true);
}

void EditController::OpenFile(Message *pcMessage)
{
	String cFileName;
	if( pcMessage->FindString("file/path", &cFileName) == EOK )
		OpenFile(cFileName);
}

void EditController::OpenFile(const String &cFileName)
{
	File cFile;
	if( cFile.SetTo(cFileName) == EOK && CloseFile() )
	{			
		int nSize = cFile.GetSize();
		uint8 *pBuffer = (uint8 *)malloc(nSize);
		cFile.Read((void *)pBuffer, nSize);
		m_pcHexView->SetBuffer(pBuffer, nSize);
		
		m_cFileName = cFileName;			
		m_bFileChanged = false;
		UpdateWindowTitle();
		m_pcWin->SetCursor(0);
		char zBuf[STR_FMT_LOADED.size() + 16];
		sprintf(zBuf, STR_FMT_LOADED.c_str(), nSize);
		ShowMessage(zBuf);
		
		m_pcNodeMon = new NodeMonitor(m_cFileName, NWATCH_STAT, this);
	}
}

void EditController::SaveFile(void)
{
	PerformSave(m_cFileName);
}

void EditController::SaveFileAs(Message *pcMessage)
{
	String cFileName;
	if( pcMessage->FindString("file/path", &cFileName) == EOK )
	{
		PerformSave(cFileName);
		m_cFileName = cFileName;
		UpdateWindowTitle();
	}
}

void EditController::PerformSave(const String &cFileName)
{
	bool bSaved = false;

	// Disable the node monitor.
	if( m_pcNodeMon != NULL )
		m_pcNodeMon->Unset();
		
	if( cFileName != "" )
	{
		File cFile;
		if( cFile.SetTo(cFileName, O_WRONLY|O_CREAT) == EOK )
		{
			cFile.Write((const void *)m_pcHexView->GetBuffer(), m_pcHexView->GetBufferLength());
			cFile.Flush();
			m_bFileChanged = false;
			UpdateWindowTitle();
			ShowMessage(STR_MSG_SAVED);
			bSaved = true;
		}
	}
	
	if( bSaved )
	{
		// Enabled the node monitor on the file.
		// We do this after cFile has gone out of scope
		// so it doesn't pick up changes we made ourselves.
		m_pcNodeMon->SetTo(cFileName, NWATCH_STAT, this);
	}
}

void EditController::ShowFindDialogue(void)
{
	if( m_pcFindDialogue == NULL )
	{
		m_pcFindDialogue = new FindDialogue(this);
		m_pcFindDialogue->Start();
	}
	
	if( ! m_pcFindDialogue->IsVisible() )
	{
		m_pcFindDialogue->Show();
	}
	else
	{
		m_pcFindDialogue->ToggleDepth();
	}

	m_pcFindDialogue->CenterInWindow(m_pcWin);
	m_pcFindDialogue->MakeFocus();
}

bool EditController::CloseFile(void)
{
	if( m_cFileName != "" )
	{
		bool bClose = true;
		if( m_bFileChanged )
		{
			 Alert *pcAlert = new Alert(STR_CLOSE_DLG_TITLE, 
			 	STR_CLOSE_DLG_MSG_PREFIX + m_cFileName + STR_CLOSE_DLG_MSG_SUFFIX,
			 	Alert::ALERT_QUESTION, 0, STR_CLOSE_DLG_BTN_SAVE.c_str(), STR_CLOSE_DLG_BTN_DISCARD.c_str(), STR_CLOSE_DLG_BTN_CANCEL.c_str(), NULL);
			 	
			 switch( pcAlert->Go() )
			 {
			 	case 0: // Save It
			 		SaveFile();
			 		break;
			 	case 1: // Discard It
			 		break;
			 	case 2: // Don't Close
			 		bClose = false;
			 }
		}
		if( bClose )
		{
			m_pcHexView->SetBuffer(NULL, 0);
			m_cFileName = "";
			m_bFileChanged = false;
			UpdateWindowTitle();
			delete(m_pcNodeMon);
		}
		return bClose;
	}
	
	return true;	
}

void EditController::FileChanged(void)
{
	String cMsg = STR_CHANGE_DLG_MSG_PREFIX + m_cFileName + STR_CHANGE_DLG_MSG_SUFFIX;
	
	if( m_bFileChanged )
		cMsg += STR_CHANGE_DLG_MSG_LOST;
	
	 Alert *pcAlert = new Alert(STR_CHANGE_DLG_TITLE, cMsg, Alert::ALERT_QUESTION, 0, STR_CHANGE_DLG_BTN_LOAD.c_str(), STR_CHANGE_DLG_BTN_IGNORE.c_str(), NULL);
	 
	 switch( pcAlert->Go() )
	 {
	 	case 0: // Load
		{
			if( m_cFileName != "" )
			{
				String cFileName = m_cFileName;
				m_cFileName = "";
				m_pcHexView->SetBuffer(NULL, 0);
				m_bFileChanged = false;
				delete(m_pcNodeMon);
				OpenFile(cFileName);
			}
	 		break;
	 	}
		default: // Ignore
			break;
	 }
}

void EditController::UpdateWindowTitle(void)
{
	String cTitle("");
	if( m_cFileName != "" )
	{
		cTitle = m_cFileName;
		if( m_bFileChanged )
			cTitle += "*";
		cTitle += " - ";
	}
	cTitle += APP_NAME;
	
	m_pcWin->SetTitle(cTitle);
}

void EditController::ShowMessage(Message *pcMessage)
{
	String cText = "";
	pcMessage->FindString(MESSAGE_TEXT_KEY, &cText);
	ShowMessage(cText);
}

void EditController::ShowMessage(const String &cText)
{
	m_pcWin->SetMessage(cText);
}

void EditController::FindPrev(Message *pcMessage)
{
	ShowMessage("");
	
	FindType nType = FT_ASCII;
	String cText = "";
	pcMessage->FindInt32(FIND_TYPE_KEY, (int32 *)&nType);
	pcMessage->FindString(FIND_TEXT_KEY, &cText);
	
	m_pcHexView->FindPrev(nType, cText);
}

void EditController::FindNext(Message *pcMessage)
{
	ShowMessage("");
	
	FindType nType = FT_ASCII;
	String cText = "";
	pcMessage->FindInt32(FIND_TYPE_KEY, (int32 *)&nType);
	pcMessage->FindString(FIND_TEXT_KEY, &cText);
	
	m_pcHexView->FindNext(nType, cText);
}





