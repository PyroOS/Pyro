                                                                                                                                                                                                       
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

#include "FindDialogue.h"
#include "EditController.h"
#include "HexView.h"

#include "Strings.h"

#include <gui/layoutview.h>
#include <gui/stringview.h>
#include <gui/button.h>
#include <util/message.h>
#include <util/messenger.h>

enum Buttons
{
	BTN_FIND_PREV,
	BTN_FIND_NEXT,
	BTN_CLOSE
};

FindDialogue::FindDialogue(Looper *pcController)
	: Window(Rect(), "FindDialogue", STR_FIND_DLG_TITLE)
	, m_pcController(pcController)
{
	LayoutView *pcLayout;
	VLayoutNode		*pcRoot;
	HLayoutNode			*pcTextRow;
	StringView				*pcTextLabel;
//	DropdownMenu			*m_pcFindText;
	HLayoutNode			*pcTypeRow;
	StringView				*pcTypeLabel;
//	DropdownMenu			*m_pcFindType;
	HLayoutNode			*pcButtonRow;
	Button					*pcBtnFindPrev;
	Button					*pcBtnFindNext;
	Button					*pcBtnClose;
	
	pcLayout = new LayoutView(Rect(), "pcLayout");
	pcRoot = new VLayoutNode("pcRoot");
	
	pcTextRow = new HLayoutNode("pcTextRow");
	pcTypeRow = new HLayoutNode("pcTypeRow");
	pcButtonRow = new HLayoutNode("pcButtonRow");
	
	pcTextLabel = new StringView(Rect(), "pcTextLabel", STR_FIND_DLG_LBL_SEARCH_FOR);
	
	m_pcFindText = new DropdownMenu(Rect(), "m_pcFindText");
	m_pcFindText->SetMinPreferredSize(20);
	
	pcTypeLabel = new StringView(Rect(), "pcTypeLabel", STR_FIND_DLG_LBL_SEARCH_TYPE);
	
	m_pcFindType = new DropdownMenu(Rect(), "m_pcFindType");
	m_pcFindType->AppendItem("ASCII");
	m_pcFindType->AppendItem("HEX");
	m_pcFindType->SetSelection(0);
	m_pcFindType->SetReadOnly(true);
		
	pcBtnFindPrev = new Button(Rect(), "pcBtnFindPrev", STR_FIND_DLG_BTN_PREV, new Message(BTN_FIND_PREV));
	
	pcBtnFindNext = new Button(Rect(), "pcBtnFindNext", STR_FIND_DLG_BTN_NEXT, new Message(BTN_FIND_NEXT));
	
	pcBtnClose = new Button(Rect(), "pcBtnClose", STR_FIND_DLG_BTN_CLOSE, new Message(BTN_CLOSE));
	
	pcTextRow->AddChild(pcTextLabel);
	pcTextRow->AddChild(m_pcFindText);
	
	pcTypeRow->AddChild(pcTypeLabel);
	pcTypeRow->AddChild(m_pcFindType);

	pcButtonRow->AddChild(new HLayoutSpacer("space_1", 0.5));	
	pcButtonRow->AddChild(pcBtnFindPrev);
	pcButtonRow->AddChild(pcBtnFindNext);
	pcButtonRow->AddChild(new HLayoutSpacer("space_2", 0.5));
	pcButtonRow->AddChild(pcBtnClose);
	
	pcRoot->AddChild(pcTextRow);
	pcRoot->AddChild(pcTypeRow);
	pcRoot->AddChild(pcButtonRow);
	
	pcButtonRow->SetHAlignment(ALIGN_RIGHT);
	pcButtonRow->SameWidth("pcBtnFindPrev", "pcBtnFindNext", "pcBtnClose", NULL);
	pcButtonRow->SameHeight("pcBtnFindPrev", "pcBtnFindNext", "pcBtnClose", NULL);
	
	pcRoot->SameWidth("pcTypeLabel", "pcTextLabel", NULL);
	pcRoot->SameWidth("m_pcFindType", "m_pcFindText", NULL);
	
	pcRoot->SetBorders(Rect(2,2,2,2), "pcTypeLabel", "m_pcFindType", "pcTextLabel", "m_pcFindText", 
		"pcBtnFindPrev", "pcBtnFindNext", "pcBtnClose", NULL);
	
	pcLayout->SetRoot(pcRoot);
	
	Point cSize(pcLayout->GetPreferredSize(false));
	SetFrame(Rect(0,0,cSize.x,cSize.y));
	pcLayout->SetFrame(GetFrame());
	
	AddChild(pcLayout);
	
	SetFocusChild(m_pcFindText);
}

FindDialogue::~FindDialogue()
{
}

bool FindDialogue::OkToQuit(void)
{
	Hide();
	return false;
}

void FindDialogue::HandleMessage(Message *pcMessage)
{
	int nContCode = -1;
		
	switch( pcMessage->GetCode() )
	{
		case BTN_FIND_PREV:
			nContCode = EV_FIND_PREV;
			break;
		case BTN_FIND_NEXT:
			nContCode = EV_FIND_NEXT;
			break;
		case BTN_CLOSE:
			Hide();
			break;
		default:
			Window::HandleMessage(pcMessage);
			break;
	}
	
	if( nContCode >= 0 )
	{
		String cStr = m_pcFindText->GetCurrentString();
		if( cStr.size() > 0 )
		{
			if( m_pcFindText->GetItemCount() > 0 )
				for( int i = m_pcFindText->GetItemCount() - 1; i >= 0; --i )
					if( m_pcFindText->GetItem(i) == cStr )
						m_pcFindText->DeleteItem(i);
			m_pcFindText->InsertItem(0, cStr);
			
			Message *pcMsg = new Message(nContCode);
			pcMsg->AddInt32(FIND_TYPE_KEY, FT_ASCII + m_pcFindType->GetSelection());
			pcMsg->AddString(FIND_TEXT_KEY, cStr);
			
			Messenger cMessenger(m_pcController);
			cMessenger.SendMessage(pcMsg);
		}
	}
}



