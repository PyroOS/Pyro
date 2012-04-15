/*
 * This file is part of the popup menu implementation for <select> elements in WebCore.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com 
 * Coypright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "EventHandler.h"
#include "config.h"
#include "FrameView.h"
#include "Frame.h"
#include "PopupMenu.h"
#include "HTMLNames.h"
#include "HTMLOptGroupElement.h"
#include "HTMLOptionElement.h"
#include "HTMLSelectElement.h"
#include "RenderMenuList.h"
#include "NotImplemented.h"

#include <stdio.h>
#include <gui/menu.h>
#include <pyro/kernel.h>

namespace WebCore {
	
using namespace HTMLNames;

/* FIXME: Find a better way to do this */

class PopupMenuReceiver : public os::Looper
{
public:
	PopupMenuReceiver() : os::Looper( "popup_menu_receiver" )
	{
		m_nResult = -1;
	}
	void HandleMessage( os::Message* pcMsg )
	{
		m_nResult = pcMsg->GetCode();
	}
	int GetResult()
	{
		return m_nResult;
	}
private:
	int m_nResult;
};


PopupMenu::PopupMenu(PopupMenuClient* client)
		 : m_popupClient(client)
{
	m_menu = NULL;
}

PopupMenu::~PopupMenu()
{
	delete( m_menu );
	m_menu = NULL;
}

void PopupMenu::show(const IntRect& rect, FrameView* view, int index)
{
	if( m_menu == NULL )
	{
		m_menu = new os::Menu( os::Rect(), "popup_menu", os::ITEMS_IN_COLUMN );
	} else {
		os::MenuItem *pcItem;
		while( ( pcItem = m_menu->RemoveItem( 0 ) ) != NULL )
			delete( pcItem );
	}
		
	os::View* pcView = view->syllableWidget();
	
	
    int size = client()->listSize();
    for (size_t i = 0; i < size; ++i) {
    	if( client()->itemIsSeparator(i))
    		m_menu->AddItem( new os::MenuSeparator() );
    	else
    	{
    		os::MenuItem* pcItem = new os::MenuItem( client()->itemText(i), new os::Message( i ) );
    		pcItem->SetEnable( client()->itemIsEnabled(i) );
    		m_menu->AddItem( pcItem );
    	}
    }
    
	if( m_menu->GetWindow() == NULL )
	{
		PopupMenuReceiver* pcReceiver = new PopupMenuReceiver();
		pcReceiver->Run();
	    m_menu->SetTargetForItems( pcReceiver );
		/* FIXME: Get this from ScrollViewSyllable directly */
		int nChild = 0;
		while( pcView->GetChildAt( nChild ) != NULL )
		{
			if( pcView->GetChildAt( nChild )->GetName() == "scroll_view_canvas" ) {
				os::Point cTarget = pcView->GetChildAt( nChild )->ConvertToScreen( os::Point( rect.x(), rect.y() ) );
				
				m_menu->Open( cTarget + os::Point( 0, 20 ) ); // We do not want to open the menu directly under the cursor
				while( m_menu->GetWindow() != NULL )
				{
					snooze( 10000 );
				}
				pcReceiver->SpoolMessages();
				if (client())
				{
					m_menu->SetTargetForItems( NULL );
					int nResult =  pcReceiver->GetResult();
					pcReceiver->Quit();
					
					client()->hidePopup();

					/* Warning: This call might delete ourself */					
					if( nResult >= 0 && index != nResult )
						client()->valueChanged( nResult );
					
					return;
				}
			}
			nChild++;
		}
		
		m_menu->SetTargetForItems( NULL );
		pcReceiver->Quit();
		
	}
}

void PopupMenu::hide()
{
	if( !m_menu )
		return;
	/* FIXME: Implement menu closing in libsyllable */
    if( m_menu && m_menu->GetParent() )
    {
    	if( m_menu->GetWindow() != NULL )
    		m_menu->WindowActivated( false );
    }
}

void PopupMenu::updateFromElement()
{
}


bool PopupMenu::itemWritingDirectionIsNatural()
{
    return false;
}

}

// vim: ts=4 sw=4 et























