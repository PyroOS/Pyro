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
#include "NotImplemented.h"

#include <gui/menu.h>
#include <util/string.h>

using namespace WebCore;

ContextMenuItem::ContextMenuItem(PlatformMenuItemDescription item)
{
    m_platformDescription = item;
}

ContextMenuItem::ContextMenuItem(ContextMenu* subMenu)
{
	m_platformDescription = NULL;
	notImplemented();
}

ContextMenuItem::ContextMenuItem(ContextMenuItemType type, ContextMenuAction action,
                                 const String& title, ContextMenu* subMenu)
{
	if( type == ActionType )
		m_platformDescription = new os::MenuItem( os::String( title ), new os::Message( action ) );
	else if( type == SeparatorType )
		m_platformDescription = new os::MenuSeparator();
	else {
		m_platformDescription = new os::MenuItem( os::String( title ), new os::Message( action ) );
	}
		
}

ContextMenuItem::~ContextMenuItem()
{
	delete( m_platformDescription );
}

PlatformMenuItemDescription ContextMenuItem::releasePlatformDescription()
{
	os::MenuItem* pcItem = m_platformDescription;
	m_platformDescription = NULL;
    return( pcItem );
}

ContextMenuItemType ContextMenuItem::type() const
{
	if( dynamic_cast<os::MenuSeparator*>(m_platformDescription) != NULL )
		return SeparatorType;
	if( m_platformDescription->GetSubMenu() != NULL )
		return( SubmenuType );
    return ActionType;
}

void ContextMenuItem::setType(ContextMenuItemType)
{
	notImplemented();
}

ContextMenuAction ContextMenuItem::action() const
{ 
	if( m_platformDescription == NULL )
		return( ContextMenuItemTagNoAction );
    return ( WebCore::ContextMenuAction)m_platformDescription->GetMessage()->GetCode();
}

void ContextMenuItem::setAction(ContextMenuAction action)
{
	if( m_platformDescription != NULL )
		m_platformDescription->GetMessage()->SetCode( action );
}

String ContextMenuItem::title() const 
{
    if( m_platformDescription == NULL )
		return( "" );
    return m_platformDescription->GetLabel();
}

void ContextMenuItem::setTitle(const String& title)
{
	m_platformDescription->SetLabel( title );
}


PlatformMenuDescription ContextMenuItem::platformSubMenu() const
{
    return m_platformDescription->GetSubMenu();
}

void ContextMenuItem::setSubMenu(ContextMenu* menu)
{
	os::String cTitle = m_platformDescription->GetLabel();
	delete( m_platformDescription );
	menu->platformDescription()->SetLabel( cTitle );
	m_platformDescription = new os::MenuItem( menu->platformDescription(), new os::Message( -1 ) );
}

void ContextMenuItem::setChecked(bool)
{
	notImplemented();
}

void ContextMenuItem::setEnabled(bool enable)
{
	if( m_platformDescription != NULL )
		m_platformDescription->SetEnable( enable );
}

bool ContextMenuItem::enabled() const
{
	if( m_platformDescription == NULL )
		return true;
    return( m_platformDescription->IsEnabled() );
}









