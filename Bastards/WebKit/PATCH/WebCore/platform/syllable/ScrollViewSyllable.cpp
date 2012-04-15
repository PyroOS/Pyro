/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
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
#include "ScrollView.h"
#include "FloatRect.h"
#include "IntPoint.h"

#include "Frame.h"
#include "ScrollViewCanvasSyllable.h"
#include "FrameView.h"
#include "PlatformWheelEvent.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/view.h>
#include <gui/scrollbar.h>

class SyllableScrollView : public os::View
{
public:
	SyllableScrollView( WebCore::ScrollView* pcScrollView, os::Rect cFrame ) : os::View( cFrame, "scroll_view", os::CF_FOLLOW_ALL, os::WID_FULL_UPDATE_ON_RESIZE )
	{
		m_pcScrollView = pcScrollView;
		m_pcHScroll = new os::ScrollBar( os::Rect(), "h_scroll", NULL, 0, FLT_MAX, os::HORIZONTAL,
								os::CF_FOLLOW_LEFT | os::CF_FOLLOW_RIGHT | os::CF_FOLLOW_BOTTOM );

		m_pcHScroll->SetSteps( 50, 100 );
		AddChild( m_pcHScroll );
		m_pcVScroll = new os::ScrollBar( os::Rect(), "v_scroll", NULL, 0, FLT_MAX, os::VERTICAL,
								os::CF_FOLLOW_TOP | os::CF_FOLLOW_RIGHT | os::CF_FOLLOW_BOTTOM );
		
		m_pcVScroll->SetSteps( 50, 100 );
		AddChild( m_pcVScroll );
		os::Rect cScrollFrame = cFrame;
		cScrollFrame.top = cScrollFrame.bottom - m_pcHScroll->GetPreferredSize( false ).y + 1;
		cScrollFrame.right -= m_pcVScroll->GetPreferredSize( false ).x;
		m_pcHScroll->SetFrame( cScrollFrame );
		cScrollFrame.bottom = cScrollFrame.top - 1;		
		cScrollFrame.top = 0;
		cScrollFrame.left = cScrollFrame.right + 1;
		cScrollFrame.right = cFrame.right;
		m_pcVScroll->SetFrame( cScrollFrame );
		m_pcHScroll->Hide();
		m_pcVScroll->Hide();
		m_cContentSize.x = (int)cFrame.Width() + 1;
		m_cContentSize.y = (int)cFrame.Height() + 1;
		m_bHScrollVisible = m_bVScrollVisible = false;
		m_eHScrollMode = m_eVScrollMode = WebCore::ScrollbarAuto;
		m_vHMax = m_vVMax = 0;
		m_pcCanvas = NULL;
		m_bStaticBackground = false;
		m_bAllowsScrolling = true;
	}
	void SetCanvas( WebCore::ScrollViewCanvasSyllable* pcView )
	{
		m_pcCanvas = pcView;
		m_pcHScroll->SetScrollTarget( pcView );
		m_pcVScroll->SetScrollTarget( pcView );
		Update();
	}
	WebCore::ScrollViewCanvasSyllable* GetCanvas()
	{
		return( m_pcCanvas );
	}
	void SetContentSize( os::IPoint cSize )
	{
		m_cContentSize = cSize;
		Update();
	}
	os::IPoint GetContentSize()
	{
		return( m_cContentSize );
	}
	void SetContentPos( os::IPoint cPos )
	{
		if( !m_pcCanvas )
			return;
		cPos.x = std::min( cPos.x, (int)m_vHMax );
		cPos.y = std::min( cPos.y, (int)m_vVMax );
		cPos.x = std::max( cPos.x, 0 );
		cPos.y = std::max( cPos.y, 0 );
		m_pcCanvas->ScrollTo( -cPos.x, -cPos.y );
	}
	void Paint( const os::Rect& cUpdate )
	{
		if( !( GetFlags() & os::WID_TRANSPARENT ) )
			return( os::View::Paint( cUpdate ) );
	}
	void DoScrollBy( os::IPoint cDelta )
	{
		if( !m_pcCanvas || !m_bAllowsScrolling )
			return;
		
		os::IPoint cNew = -os::IPoint( m_pcCanvas->GetScrollOffset() ) + cDelta;
		SetContentPos( cNew );
	}
	os::IPoint DoGetScrollOffset()
	{
		return( -os::IPoint( m_pcCanvas->GetScrollOffset() ) );
	}
	os::Rect GetVisibleFrame()
	{
		os::Rect cLeftFrame;
		if( !m_pcCanvas )
			return( GetBounds() );
		cLeftFrame = m_pcCanvas->GetBounds();
		//DEBUG( "Visible %f %f %f %f\n", cLeftFrame.left, cLeftFrame.top, cLeftFrame.right, cLeftFrame.bottom );
		return( cLeftFrame );
	}
	void EnsureVisible( os::Point cPoint )
	{
		if( !m_pcCanvas )
			return;
		//DEBUG( "Ensure visible %f %f\n", cPoint.x, cPoint.x );
		os::Point cCurrent = -m_pcCanvas->GetScrollOffset();
		os::Point cNew = -m_pcCanvas->GetScrollOffset();
		if( cPoint.x < cCurrent.x )
			cNew.x = cPoint.x;
		if( cPoint.y < cCurrent.y )
			cNew.y = cPoint.y;
		if( cPoint.x > cCurrent.x + m_pcCanvas->GetBounds().Width() )
			cNew.x = cPoint.x;
		if( cPoint.y > cCurrent.y + m_pcCanvas->GetBounds().Height() )
			cNew.y = cPoint.y;			
		if( cNew.x < 0 )
			cNew.x = 0;
		if( cNew.y < 0 )
			cNew.y = 0;
		if( cNew.x > m_cContentSize.x - m_pcCanvas->GetBounds().Width() )
			cNew.x = m_cContentSize.x - m_pcCanvas->GetBounds().Width();
		if( cNew.y > m_cContentSize.y - m_pcCanvas->GetBounds().Height() )
			cNew.y = m_cContentSize.y - m_pcCanvas->GetBounds().Height();
		
		if( cNew != cCurrent ) {
			
			m_pcHScroll->SetValue( cNew.x );
			m_pcVScroll->SetValue( cNew.y );
		}
	}
	void Update()
	{
		if( m_pcCanvas == NULL )
			return;
		os::Rect cLeftFrame = GetBounds();
		
		if( ( ( ( (int)( m_cContentSize.x - 1.0f ) > (int)( cLeftFrame.Width() + 1.0f ) ) && m_eHScrollMode == WebCore::ScrollbarAuto )
		|| m_eHScrollMode == WebCore::ScrollbarAlwaysOn ) && !m_bHScrollVisible )
		{
			m_pcHScroll->Show();
			m_bHScrollVisible = true;
		}
		else if( ( ( ( (int)( m_cContentSize.x - 1.0f ) <= (int)( cLeftFrame.Width() + 1.0f ) ) && m_eHScrollMode == WebCore::ScrollbarAuto )
		|| m_eHScrollMode == WebCore::ScrollbarAlwaysOff ) && m_bHScrollVisible )
		{	
			m_pcHScroll->Hide();
			m_bHScrollVisible = false;
		}
		if( ( ( ( (int)( m_cContentSize.y - 1.0f ) > (int)( cLeftFrame.Height() + 1.0f ) ) && m_eVScrollMode == WebCore::ScrollbarAuto )
		|| m_eVScrollMode == WebCore::ScrollbarAlwaysOn ) && !m_bVScrollVisible )
		{
			m_pcVScroll->Show();
			m_bVScrollVisible = true;
		}
		else if( ( ( ( (int)( m_cContentSize.y - 1.0f ) <= (int)( cLeftFrame.Height() + 1.0f ) ) && m_eVScrollMode == WebCore::ScrollbarAuto )
		|| m_eVScrollMode == WebCore::ScrollbarAlwaysOff ) && m_bVScrollVisible )
		{
			m_pcVScroll->Hide();
			m_bVScrollVisible = false;
		}
		if( m_bHScrollVisible )
			cLeftFrame.bottom -= m_pcHScroll->GetPreferredSize( false ).y;
		if( m_bVScrollVisible )
			cLeftFrame.right -= m_pcVScroll->GetPreferredSize( false ).x;
		m_vHMax = m_vVMax = 0;
		if( m_bHScrollVisible )
		{
			m_vHMax = m_cContentSize.x - cLeftFrame.Width() - 1;
			m_pcCanvas->SetScrollBarMaxH( m_vHMax );
			m_pcHScroll->SetMinMax( 0, m_vHMax );
			
			m_pcHScroll->SetProportion( cLeftFrame.Width() / m_cContentSize.x );
		}
		if( m_bVScrollVisible )
		{
			m_vVMax = m_cContentSize.y - cLeftFrame.Height() - 1;
			m_pcCanvas->SetScrollBarMaxV( m_vVMax );
			m_pcVScroll->SetMinMax( 0, m_vVMax );
			m_pcVScroll->SetProportion( cLeftFrame.Height() / m_cContentSize.y );	
		}
		if( cLeftFrame != m_pcCanvas->GetFrame() )
		{
			//DEBUG( "Update frame!\n" );
			m_pcCanvas->SetFrame( cLeftFrame );
		}
		
		if( ( m_pcHScroll->GetValue().AsFloat() + cLeftFrame.Width() + 1 > m_cContentSize.x ) )
			m_pcHScroll->SetValue( std::max( m_cContentSize.x - cLeftFrame.Width() - 1, 0.0f ) );
		if(  ( m_pcVScroll->GetValue().AsFloat() + cLeftFrame.Height() + 1 > m_cContentSize.y ) )
			m_pcVScroll->SetValue( std::max( m_cContentSize.y - cLeftFrame.Height() - 1, 0.0f ) );
		
		//DEBUG( "FRAME %f %f %f %f\n", cLeftFrame.left, cLeftFrame.top, cLeftFrame.right, cLeftFrame.bottom );
	}
	WebCore::ScrollbarMode GetHScrollBarMode()
	{
		return( m_eHScrollMode );
	}
	WebCore::ScrollbarMode GetVScrollBarMode()
	{
		return( m_eVScrollMode );
	}
	void SetHScrollBarMode( WebCore::ScrollbarMode eMode )
	{
		m_eHScrollMode = eMode;
		Update();
	}
	void SetVScrollBarMode( WebCore::ScrollbarMode eMode )
	{
		m_eVScrollMode = eMode;
		Update();
	}
	void FrameSized( const os::Point& cDelta )
	{
		DEBUG("Frame sized!\n" );
		WebCore::FrameView* fv = static_cast<WebCore::FrameView*>(m_pcScrollView);
		if (!fv || !fv->frame())
	        return;

		fv->frame()->forceLayout();
#if 0
		/* XXXKV: This causes an assertion failure in FrameView (frame->view() != this)
		   but it doesn't seem to be required */
        fv->adjustViewSize();
#endif
    	Update();
	}
	void Activated( bool bActive )
	{
		if( bActive && m_pcCanvas )
			m_pcCanvas->MakeFocus();
	}
	void SetTransparent( os::View* pcView, bool bFlag )
	{
		uint32 nFlags = pcView->GetFlags();
		nFlags &= ~os::WID_TRANSPARENT;
		if( bFlag )
			nFlags |= os::WID_TRANSPARENT;
		pcView->SetFlags( nFlags );
		
	}
	void SetStaticBackground( bool bFlag )
	{
		if( bFlag != m_bStaticBackground )
		{
			m_bStaticBackground = bFlag;
			SetTransparent( this, bFlag );
			if( m_pcCanvas )
				SetTransparent( m_pcCanvas, bFlag );
			SetTransparent( m_pcHScroll, bFlag );
			SetTransparent( m_pcVScroll, bFlag );
			Invalidate( true );
			Sync();
		}
	}
	bool GetStaticBackground()
	{
		return( m_bStaticBackground );
	}
	void SetAllowsScrolling( bool bFlag )
	{
		m_bAllowsScrolling = bFlag;
	}
private:
	WebCore::ScrollView* m_pcScrollView;
	os::IPoint m_cContentSize;
	WebCore::ScrollViewCanvasSyllable* m_pcCanvas;
	os::ScrollBar* m_pcHScroll;
	os::ScrollBar* m_pcVScroll;
	bool m_bHScrollVisible;
	bool m_bVScrollVisible;
	WebCore::ScrollbarMode m_eHScrollMode;
	WebCore::ScrollbarMode m_eVScrollMode;
	float m_vHMax;
	float m_vVMax;
	bool m_bStaticBackground;
	bool m_bAllowsScrolling;
};

namespace WebCore {

ScrollView::ScrollView()
    //: m_area(new QScrollArea(0))
{
	m_area = new SyllableScrollView( this, os::Rect( 0, 0, 1, 1 ) );
	setSyllableWidget(m_area);
}

ScrollView::~ScrollView()
{
}

void ScrollView::setParentWidget(os::View* pcParent)
{
	DEBUG(" ScrollView::setParentWidget\n" );
    
    // 'isFrameView()' can not be called yet in the constructor!
    if (isFrameView() && m_area->GetCanvas() == NULL) {
        ScrollViewCanvasSyllable* pcCanvas = new ScrollViewCanvasSyllable(this, m_area, os::WID_FULL_UPDATE_ON_RESIZE 
        																| ( m_area->GetStaticBackground() ? os::WID_TRANSPARENT : 0 ) );
		m_area->SetCanvas( pcCanvas );
        
    }
    
    Widget::setParentWidget(pcParent);

}

// Does not work correctly yet. We need a second drawing region
static void recursivePaint( os::View* pcView, os::Rect cUpdate )
{
	os::Rect cRegion = cUpdate + pcView->GetScrollOffset();
	os::Region cReg( cRegion );
	pcView->SetDrawingRegion( cReg );
	pcView->Paint( cUpdate );
	pcView->ClearDrawingRegion();	
	
	int nChild = 0;
	os::View* pcChild;
	while( ( pcChild = pcView->GetChildAt( nChild ) ) != NULL )
	{
		
		os::Rect cChildUpdate = cUpdate - pcChild->GetFrame().LeftTop() - pcChild->GetScrollOffset();
		cChildUpdate &= ( pcChild->GetBounds() );
		if( cChildUpdate.IsValid() )
			recursivePaint( pcChild, cChildUpdate );
	
		nChild++;
	}
}


void ScrollView::updateContents(const IntRect& updateRect, bool now)
{
	DEBUG("ScrollView::updateContents() %x %i %i %i %i %i\n", m_area->GetCanvas(), updateRect.x(), updateRect.y(), updateRect.width(), updateRect.height(), now);
	if( updateRect.width() == 0 || updateRect.height() == 0 )
		return;
	os::IRect cUpdate( updateRect );
	
	os::View* pcView = m_area->GetCanvas();
	
	if( !cUpdate.IsValid() || pcView == NULL )
		return;
	
	if( now )
	{
		cUpdate &= m_area->GetVisibleFrame();		
		//recursivePaint( pcView, cUpdate );
		pcView->Invalidate( cUpdate, true );
		pcView->Sync();
	}
	else
	{
		cUpdate &= m_area->GetVisibleFrame();
		pcView->Invalidate( cUpdate, true );
		pcView->Flush();
	}		
}

int ScrollView::visibleWidth() const
{
	return( (int)m_area->GetVisibleFrame().Width() + 1 );
}

int ScrollView::visibleHeight() const
{
	return( (int)m_area->GetVisibleFrame().Height() + 1 );
}

FloatRect ScrollView::visibleContentRect() const
{
	return( m_area->GetVisibleFrame() );
}

FloatRect ScrollView::visibleContentRectConsideringExternalScrollers() const
{
    // external scrollers not supported for now
    return visibleContentRect();
}

void ScrollView::setContentsPos(int newX, int newY)
{
	m_area->SetContentPos( os::IPoint( newX, newY ) );
}

void ScrollView::resizeContents(int w, int h)
{
	DEBUG("Resize contents to %i %i\n", w, h );
	m_area->SetContentSize( os::IPoint( w, h ) );
}

int ScrollView::contentsX() const
{
    return( (int)m_area->GetVisibleFrame().left );
}

int ScrollView::contentsY() const
{
   return( (int)m_area->GetVisibleFrame().top );
}

int ScrollView::contentsWidth() const
{
    return( m_area->GetContentSize().x );
}

int ScrollView::contentsHeight() const
{
    return( m_area->GetContentSize().y );    
}


IntPoint ScrollView::contentsToWindow(const IntPoint& point) const
{
	//DEBUG("ScrollView::contentsToWindow\n");
    return point;
}

IntPoint ScrollView::windowToContents(const IntPoint& point) const
{
	//DEBUG("ScrollView::windowToContents\n");
    return point;
}

IntSize ScrollView::scrollOffset() const
{
	DEBUG( "ScrollView::scrollOffset()\n" );
    return( m_area->DoGetScrollOffset() );
}

void ScrollView::scrollBy(int dx, int dy)
{
	m_area->DoScrollBy( os::IPoint( dx, dy ) );
}

ScrollbarMode ScrollView::hScrollbarMode() const
{
	return( m_area->GetHScrollBarMode() );
}

ScrollbarMode ScrollView::vScrollbarMode() const
{
	return( m_area->GetVScrollBarMode() );
}

void ScrollView::suppressScrollbars(bool suppressed, bool /* repaintOnSuppress */)
{
    setScrollbarsMode(suppressed ? ScrollbarAlwaysOff : ScrollbarAuto);
}

void ScrollView::setHScrollbarMode(ScrollbarMode newMode)
{
	m_area->SetHScrollBarMode( newMode );
}

void ScrollView::setVScrollbarMode(ScrollbarMode newMode)
{
	m_area->SetVScrollBarMode( newMode );
}

void ScrollView::setScrollbarsMode(ScrollbarMode newMode)
{
    setHScrollbarMode(newMode);
    setVScrollbarMode(newMode);
}

void ScrollView::setStaticBackground(bool flag)
{
	m_area->SetStaticBackground( flag );
	DEBUG("%x SETSTATIC %i\n", m_area->GetCanvas(), flag );
    // no-op
}

void ScrollView::addChild(Widget* child)
{
	DEBUG( "ScrollView::addChild\n");
    ASSERT(child != 0);
    ASSERT(m_area /*&& m_area->widget()*/);
    
    os::View* pcView = m_area->GetCanvas();
	if( pcView )
	    child->setParentWidget( pcView );
}

void ScrollView::removeChild(Widget* child)
{ 
	if( child->syllableWidget() )
		child->syllableWidget()->RemoveThis();
}

void ScrollView::scrollRectIntoViewRecursively(const IntRect& rect)
{
	int x = rect.x();
	int y = rect.y();
    x = (x < 0) ? 0 : x;
    y = (y < 0) ? 0 : y;

    m_area->EnsureVisible( os::Point( x, y ) );
}

bool ScrollView::inWindow() const
{
    return true;
}

void ScrollView::wheelEvent(PlatformWheelEvent& ev)
{
	m_area->DoScrollBy( os::IPoint( (int)ev.deltaX() * 50, (int)ev.deltaY() * 50 ) );
}

PlatformScrollbar* ScrollView::scrollbarUnderMouse(const PlatformMouseEvent& mouseEvent)
{
    // Probably don't care about this.
    return 0;
}


void ScrollView::setAllowsScrolling(bool allows)
{
    if (!allows)
        suppressScrollbars(true);
    m_area->SetAllowsScrolling( allows );
}


void ScrollView::update()
{
	DEBUG("ScrollView::update()\n");
    if (m_area) {
        m_area->Flush();
    }
}

}

// vim: ts=4 sw=4 et






