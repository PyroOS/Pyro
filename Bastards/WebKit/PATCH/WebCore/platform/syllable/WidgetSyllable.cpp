/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 George Stiakos <staikos@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * All rights reserved.
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

#include "Font.h"
#include "Widget.h"
#include "Cursor.h"
#include "IntRect.h"
#include "FontData.h"
#include "RenderObject.h"
#include "GraphicsContext.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/view.h>
#include <gui/window.h>
#include <gui/control.h>

namespace WebCore {

struct WidgetPrivate
{
    WidgetPrivate() : m_parent(0), m_widget(0), m_visible( true ) { }
    ~WidgetPrivate() { delete m_widget; }

    os::View* m_parent;
    os::View* m_widget;
    WidgetClient* m_client;
    bool m_visible;
};

Widget::Widget()
    : data(new WidgetPrivate())
{
}

Widget::~Widget()
{
    delete data;
    data = 0;
}

void Widget::setClient(WidgetClient* c)
{
	data->m_client = c;
}

WidgetClient* Widget::client() const
{
    return( data->m_client );
}

IntRect Widget::frameGeometry() const
{
    if (!data->m_widget)
        return IntRect();

	IntRect cRect( os::IRect( data->m_widget->GetFrame() ) );
	DEBUG( "Widget::frameGeometry()" );
	DEBUG( "%s %i %i %i %i\n", data->m_widget->GetName().c_str(), cRect.x(), cRect.y(), cRect.width(), cRect.height() );
    return cRect;
}


void Widget::setFocus()
{
    if (data->m_widget)
        data->m_widget->MakeFocus();
}



void Widget::setCursor(const Cursor& cursor)
{
//	notImplemented();
  //  if (data->m_widget)
//        data->m_widget->setCursor(cursor.impl());
}

void Widget::show()
{
	if( !data->m_visible && data->m_widget )
	{
		data->m_widget->Show();
	}
	data->m_visible = true;
}

void Widget::hide()
{
	if( data->m_visible && data->m_widget )
		data->m_widget->Hide();
	data->m_visible = false;
}

void Widget::setSyllableWidget(os::View* child)
{
    delete data->m_widget;
    data->m_widget = child;
}

os::View* Widget::syllableWidget()
{
    return data->m_widget;
}

void Widget::setParentWidget(os::View* parent)
{
	DEBUG("SetParent widget!\n");
    data->m_parent = parent;
    if( data->m_widget != NULL && data->m_widget->GetParent() == NULL )
    	parent->AddChild( data->m_widget );
}

os::View* Widget::parentWidget() const
{
    return data->m_parent;
}

void Widget::setFrameGeometry(const IntRect& r)
{
    if (!data->m_widget)
        return;
        
    DEBUG("Widget::setFrameGeometry %s %i %i %i %i\n", data->m_widget->GetName().c_str(),
    r.x(), r.y(), r.width(), r.height() );
//    if( data->m_widget->GetWindow() )
//	    DEBUG( "%f %f\n", data->m_widget->GetWindow()->GetFrame().Width(), data->m_widget->GetWindow()->GetFrame().Height() );

	os::Rect cRect( r.x(), r.y(), r.x() + r.width() - 1, r.y() + r.height() - 1 );
    data->m_widget->SetFrame(cRect);
}


void Widget::paint(GraphicsContext* p, IntRect const& r)
{
	notImplemented();
}
bool Widget::isEnabled() const
{
    if (!data->m_widget)
        return false;
        
    os::Control* pcControl = dynamic_cast<os::Control*>( data->m_widget );
    if( pcControl != NULL )
    	return( pcControl->IsEnabled() );

    return( true );
}

void Widget::setIsSelected(bool)
{
    notImplemented();
}

void Widget::setEnabled(bool en)
{
  if (!data->m_widget)
        return;
        
    os::Control* pcControl = dynamic_cast<os::Control*>( data->m_widget );
    if( pcControl != NULL )
    	pcControl->SetEnable( en );
}

void Widget::invalidate()
{
	if( !data->m_widget )
		return;
	data->m_widget->Invalidate( true );
	data->m_widget->Flush();
}

void Widget::invalidateRect(const IntRect& r)
{
	data->m_widget->Invalidate( FloatRect( r ), true );
	data->m_widget->Flush();
}

void Widget::removeFromParent()
{
	if(data->m_widget)
	{
   		DEBUG("Remove %s\n", data->m_widget->GetName().c_str() );
		data->m_widget->RemoveThis();
	}
}

}

// vim: ts=4 sw=4 et






















