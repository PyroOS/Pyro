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
#include "PlatformScrollBar.h"
#include "NotImplemented.h"

#include <gui/view.h>
#include <gui/scrollbar.h>
#include <util/message.h>

using namespace WebCore;

class SyllableScrollBar : public os::ScrollBar
{
public:
	SyllableScrollBar( PlatformScrollbar* pcScroll, int nOrientation ) : os::ScrollBar( 
		( nOrientation == os::VERTICAL ) ? os::Rect( 0, 0, 15, 0) : os::Rect( 0, 0, 0, 15), "scroll_bar", new os::Message( 0 ), 0, 1, nOrientation )
	{
		m_pcScroll = pcScroll;
	}
	void AttachedToWindow()
	{
		SetTarget( this );
	}
	void HandleMessage( os::Message* pcMsg )
	{
		m_pcScroll->valueUpdated();
	}
private:
	PlatformScrollbar* m_pcScroll;	
};

PlatformScrollbar::PlatformScrollbar(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize size)
				: Scrollbar( client, orientation, size )
{
	SyllableScrollBar* pcScroll = new SyllableScrollBar( this, ( orientation == VerticalScrollbar ) ? os::VERTICAL : os::HORIZONTAL );
	pcScroll->SetSteps( 0.1f, 0.2f );		
	setSyllableWidget( pcScroll );
}				

PlatformScrollbar::~PlatformScrollbar()
{
	setSyllableWidget( NULL );
}


void PlatformScrollbar::valueUpdated()
{
	SyllableScrollBar* pcScroll = static_cast<SyllableScrollBar*>(syllableWidget());
	int nValue = (int)( pcScroll->GetValue().AsFloat() * (float)(m_totalSize - m_visibleSize) );
	setValue( nValue );
}

void PlatformScrollbar::updateThumbPosition()
{
	SyllableScrollBar* pcScroll = static_cast<SyllableScrollBar*>(syllableWidget());
    pcScroll->SetValue( (float)m_currentPos / (float)(m_totalSize - m_visibleSize), false );
}

void PlatformScrollbar::updateThumbProportion()
{
	SyllableScrollBar* pcScroll = static_cast<SyllableScrollBar*>(syllableWidget());
    float val = (float)m_visibleSize/(float)m_totalSize;
    if (!(val == pcScroll->GetProportion() || val < 0.0))
    {
    	pcScroll->SetProportion( val );
    	pcScroll->SetValue( pcScroll->GetValue() );
//    	valueUpdated();
    }
    
}

int PlatformScrollbar::width() const
{
	 return Widget::width();
}

int PlatformScrollbar::height() const
{
    return Widget::height();
}

void PlatformScrollbar::setRect(const IntRect& rect)
{
    setFrameGeometry(rect);
}

void PlatformScrollbar::setEnabled(bool enabled)
{
    Widget::setEnabled(enabled);
}

void PlatformScrollbar::paint(GraphicsContext* graphicsContext, const IntRect& damageRect)
{    
}

