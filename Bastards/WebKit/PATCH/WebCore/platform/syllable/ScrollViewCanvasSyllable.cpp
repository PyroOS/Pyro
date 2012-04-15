/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
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

#include "ScrollViewCanvasSyllable.h"


#include "EventHandler.h"
#include "Frame.h"
#include "FrameView.h"
#include "FrameLoader.h"
#include "TypingCommand.h"
#include "KeyboardCodes.h"
#include "GraphicsContext.h"
#include "SelectionController.h"
#include "PlatformMouseEvent.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformWheelEvent.h"
#include "HitTestResult.h"
#include "Page.h"
#include "ChromeClient.h"
#include "FocusController.h"
#include "SyllableDebug.h"

#include <gui/scrollbar.h>
#include <util/looper.h>
#include <util/message.h>
#include <pyro/kernel.h>


extern os::Locker g_cGlobalMutex;


namespace WebCore {

ScrollViewCanvasSyllable::ScrollViewCanvasSyllable(ScrollView* frameView, os::View* parent, uint32 nFlags)
    : os::View( parent->GetBounds(), "scroll_view_canvas", os::CF_FOLLOW_ALL, nFlags),
      m_frameView(frameView)
{
	parent->AddChild( this );
	DEBUG("ScrollViewCanvasSyllable::ScrollViewCanvasSyllable\n" );
    //setMouseTracking(true);
    //setFocusPolicy(Syllable::StrongFocus);
}


void ScrollViewCanvasSyllable::Paint( const os::Rect& cUpdate )
{  
	ASSERT( g_cGlobalMutex.IsLocked() );
	FrameView* fv = static_cast<FrameView*>(m_frameView);
//	DEBUG("%x %x %x\n", fv, fv->frame(), fv->frame()->renderer());
    if (!fv || !fv->frame() || !fv->frame()->renderer())
    {
    	if( !( GetFlags() & os::WID_TRANSPARENT ) )
			FillRect( cUpdate, os::get_default_color( os::COL_NORMAL ) );
        return;
    }
          
    while( fv->needsLayout() )
    {
    	DEBUG( "Layout still pending!\n" );
    	fv->layout();
    }

//    DEBUG( "Paint!!! %f %f %f %f %f %x %x %i\n", cUpdate.left, cUpdate.top, cUpdate.right, cUpdate.bottom, ConvertToScreen( cUpdate ).left, this, GetParent(), fv->needsLayout() );
    
   
    os::Rect cFrame = GetBounds();  

	GraphicsContext ctx(this);
	
    fv->frame()->paint(&ctx, os::IRect( cUpdate ) );
}

void ScrollViewCanvasSyllable::MouseMove( const os::Point& cNewPos, int nCode, uint32 nButtons, os::Message* pcData )
{
	os::Point cConvert = ConvertToScreen( cNewPos );
	FrameView* fv = static_cast<FrameView*>(m_frameView);
    if (!fv || !fv->frame())
        return( os::View::MouseMove( cNewPos, nCode, nButtons, pcData ) );
    Frame* frame = fv->frame();

	if( frame && frame->eventHandler() )
	{    
	    WebCore::HitTestResult result = frame->eventHandler()->hitTestResultAtPoint( (os::IPoint)cNewPos, true );
	    os::String cUrl = os::String( result.absoluteLinkURL().string() );
	    if( frame->page() && frame->page()->chrome() && frame->page()->chrome()->client() )
	    	frame->page()->chrome()->client()->setStatusbarText( cUrl );
	
	    frame->eventHandler()->handleMouseMoveEvent(PlatformMouseEvent(MouseEventMoved, cNewPos.x, cNewPos.y, cConvert.x, cConvert.y, nButtons, 0));
	}
	os::View::MouseMove( cNewPos, nCode, nButtons, pcData );
}

void ScrollViewCanvasSyllable::MouseDown( const os::Point& cNewPos, uint32 nButtons )
{
	os::Point cConvert = ConvertToScreen( cNewPos );
	FrameView* fv = static_cast<FrameView*>(m_frameView);
    if (!fv || !fv->frame())
        return( os::View::MouseDown( cNewPos, nButtons ) );
   
	if( !( nButtons & 0x8000 ) )
	{
		MakeFocus();
	}
    Frame* frame = fv->frame();
    
    frame->loader()->resetMultipleFormSubmissionProtection();
    
    if( nButtons == 2 )
	    frame->eventHandler()->sendContextMenuEvent(PlatformMouseEvent(MouseEventPressed, cNewPos.x, cNewPos.y, cConvert.x, cConvert.y, nButtons & ~0x8000, 1));
	else
    	frame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(MouseEventPressed, cNewPos.x, cNewPos.y, cConvert.x, cConvert.y, nButtons & ~0x8000, 1));
    nButtons |= 0x8000;
	os::View::MouseDown( cNewPos, nButtons );
}

void ScrollViewCanvasSyllable::MouseUp( const os::Point& cNewPos, uint32 nButtons, os::Message* pcData )
{
	os::Point cConvert = ConvertToScreen( cNewPos );
	FrameView* fv = static_cast<FrameView*>(m_frameView);
    if (!fv || !fv->frame())
    {
        return( os::View::MouseUp( cNewPos, nButtons, pcData ) );
    }
    Frame* frame = fv->frame();    
    frame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(MouseEventReleased, cNewPos.x, cNewPos.y, cConvert.x, cConvert.y, nButtons, 0));
    os::View::MouseUp( cNewPos, nButtons, pcData );
}

void ScrollViewCanvasSyllable::DoScroll(bool isHorizontal, float multiplier)
{
    os::ScrollBar* pcScrollBar = isHorizontal ? GetHScrollBar() : GetVScrollBar();


	if( pcScrollBar == NULL || !pcScrollBar->IsVisible() )
		return;
		
	float vValue = pcScrollBar->GetValue().AsFloat();
	
	vValue += multiplier;
	
	vValue = std::max( 0.0f, vValue );
	vValue = std::min( vValue, isHorizontal ? m_vScrollBarMaxH : m_vScrollBarMaxV );
	
	pcScrollBar->SetValue( vValue );
}

void ScrollViewCanvasSyllable::KeyDown( const char* pzString, const char* pzRawString, uint32 nQualifiers )
{
	
	os::Message* pcKey = GetLooper()->GetCurrentMessage();
	int32 nRawKey;
	pcKey->FindInt32("_raw_key", &nRawKey);
	PlatformKeyboardEvent kevent(pzString, pzRawString, nRawKey, nQualifiers, false);

	
    FrameView* fv = static_cast<FrameView*>(m_frameView);
    Frame* frame = fv->frame();
    if (!frame)
        return;
        
    bool handled = frame->eventHandler()->keyEvent(kevent);
    

    if( !handled ) {
    	/* Scroll */
    	switch(kevent.windowsVirtualKeyCode()) {
			case VK_RIGHT:
				DoScroll( true, 30 );
			break;
			case VK_LEFT:
				DoScroll( true, -30 );
			break;
			case VK_UP:
				DoScroll( false, -30 );
			break;
			case VK_DOWN:
				DoScroll( false, 30 );
			break;
			case VK_NEXT:
				DoScroll( false, GetBounds().Height() );
			break;
			case VK_PRIOR:
				DoScroll( false, -GetBounds().Height() );
			break;
		}
		return( os::View::KeyDown( pzString, pzRawString, nQualifiers ) );
	}
	
	
}
void ScrollViewCanvasSyllable::KeyUp( const char* pzString, const char* pzRawString, uint32 nQualifiers )
{
	os::Message* pcKey = GetLooper()->GetCurrentMessage();
	int32 nRawKey = 0;
	pcKey->FindInt32("_raw_key", &nRawKey);
	nRawKey &= ~0x80;
	PlatformKeyboardEvent kevent(pzString, pzRawString, nRawKey, nQualifiers, true);
    FrameView* fv = static_cast<FrameView*>(m_frameView);
      Frame* frame = fv->frame();
    if (!frame)
        return;
        
        
    bool handled = frame->eventHandler()->keyEvent(kevent);
    
    if( !handled )
		return( os::View::KeyUp( pzString, pzRawString, nQualifiers ) );
}

void ScrollViewCanvasSyllable::WheelMoved( const os::Point& cDelta )
{
	WebCore::FrameView* fv = static_cast<WebCore::FrameView*>(m_frameView);
    if (!fv || !fv->frame())
   	    return;

	PlatformWheelEvent wevent(cDelta, -GetScrollOffset());

   	fv->frame()->eventHandler()->handleWheelEvent(wevent);
}

void ScrollViewCanvasSyllable::Activated( bool bActive )
{
	Page* page = static_cast<const FrameView*>(m_frameView)->frame()->page();
	if (!page || !page->focusController())
		return;

	page->focusController()->setActive( bActive );
}

}

// vim: ts=4 sw=4 et

















































































































































