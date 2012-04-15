/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "ContextMenu.h"
#include "ContextMenuItem.h"
#include "ContextMenuController.h"
#include "Frame.h"
#include "FrameView.h"
#include "Document.h"
#include "SyllableDebug.h"

#include <wtf/Assertions.h>

#include <gui/menu.h>

extern os::Locker g_cGlobalMutex;

namespace WebCore {
	
/* FIXME: Find a better way to do this */

class ContextMenuReceiver : public os::Looper
{
public:
	ContextMenuReceiver( ContextMenu* menu ) : os::Looper( "context_menu_receiver" ), m_menu( menu )
	{
		m_nResult = -1;
	}
	void HandleMessage( os::Message* pcMsg )
	{
		m_nResult = pcMsg->GetCode();
		if( m_nResult == -2 )
		{
			
			PostMessage( os::M_TERMINATE );
			return;
		}
		if( m_nResult != -1 )
		{
			m_menu->platformDescription()->SetCloseMsgTarget( os::Messenger() );
			os::MenuItem* pcItem = m_menu->platformDescription()->FindItem( m_nResult );
			if( pcItem == NULL )
			{
				DEBUG( "Error: Context menu item with code %i not found!\n", m_nResult );
				return;
			}
			ContextMenuItem item( pcItem );
			m_menu->controller()->contextMenuItemSelected( &item );
			item.releasePlatformDescription();
		}
	}
	int GetResult()
	{
		return m_nResult;
	}
private:
	ContextMenu* m_menu;
	int m_nResult;
};

ContextMenu::ContextMenu(const HitTestResult& result)
    : m_hitTestResult(result), m_platformDescription( NULL )
{
	/* Get position */
	if( result.innerNode() != NULL && result.innerNode()->document() != NULL )
	{
		os::View* pcView = result.innerNode()->document()->frame()->view()->syllableWidget();
		int nChild = 0;
		while( pcView->GetChildAt( nChild ) != NULL )
		{
			if( pcView->GetChildAt( nChild )->GetName() == "scroll_view_canvas" ) {
				m_point = pcView->GetChildAt( nChild )->ConvertToScreen( os::Point( result.point().x(), result.point().y() ) );
				break;
			}
			nChild++;
		}
	}
	DEBUG("ContextMenu::ContextMenu() %i %i\n", result.point().x(), result.point().y());
	m_platformDescription = new os::Menu( os::Rect(), "", os::ITEMS_IN_COLUMN );
}

ContextMenu::~ContextMenu()
{
}

void ContextMenu::appendItem(ContextMenuItem& item)
{
    DEBUG("ContextMenu::appendItem\n" );
    
    checkOrEnableIfNeeded( item );
    
    os::MenuItem* pcItem = item.releasePlatformDescription();
    if( pcItem != NULL )
	    m_platformDescription->AddItem( pcItem );
}

unsigned ContextMenu::itemCount() const
{
    return( m_platformDescription->GetItemCount() );
}

void ContextMenu::insertItem(unsigned position, ContextMenuItem& item)
{
	checkOrEnableIfNeeded( item );
	
	if( item.releasePlatformDescription() != NULL )
	    m_platformDescription->AddItem( item.releasePlatformDescription(), position );
}

PlatformMenuDescription ContextMenu::platformDescription() const
{
	return( m_platformDescription );
}

void ContextMenu::setPlatformDescription(PlatformMenuDescription menu)
{
	if( static_cast<os::Menu*>(menu) == m_platformDescription )
	{
		ContextMenuReceiver* pcReceiver = new ContextMenuReceiver( this );
		g_cGlobalMutex.Lock();
		pcReceiver->SetMutex( &g_cGlobalMutex );
		pcReceiver->Run();
		m_platformDescription->SetTargetForItems( pcReceiver );
		m_platformDescription->SetCloseMessage( os::Message( -2 ) );
		m_platformDescription->SetCloseMsgTarget( pcReceiver );
		m_platformDescription->Open( m_point );
		
		return;
	}
	if( m_platformDescription )
		delete m_platformDescription;
    m_platformDescription = static_cast<os::Menu*>(menu);
}

}

