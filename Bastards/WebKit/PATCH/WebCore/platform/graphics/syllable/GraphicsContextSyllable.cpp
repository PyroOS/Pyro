/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2006 Allan Sandfeld Jensen <sandfeld@kde.org>
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

#include "AffineTransform.h"
#include "Path.h"
#include "Color.h"
#include "GraphicsContext.h"
#include "Font.h"
#include "SimpleFontData.h"
#include "Pen.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/view.h>
#include <gui/window.h>
#include <stack>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef SVG_SUPPORT
#include "KRenderingDeviceQt.h"
#endif

namespace WebCore {
class ViewStateEntry
{
public:
	os::Color32_s m_sSavedFgColor;
	os::Color32_s m_sSavedEraseColor;    	
	os::drawing_mode m_eMode;
	os::Region m_cRegion;
	float m_vTransX;
	float m_vTransY;
};

class ViewState
{
public:
	void Save( os::View* pcView, os::Region& cRegion, float vTransX, float vTransY )
	{
		if( !pcView )
			return;
		ViewStateEntry cEntry;
		cEntry.m_sSavedFgColor = pcView->GetFgColor();
		cEntry.m_sSavedEraseColor = pcView->GetEraseColor();
		cEntry.m_eMode = pcView->GetDrawingMode();
		cEntry.m_cRegion.Set( cRegion );
		cEntry.m_vTransX = vTransX;
		cEntry.m_vTransY = vTransY;
		m_cEntries.push( cEntry );
	}
	void Restore( os::View* pcView, os::Region& cRegion, float& vTransX, float& vTransY )
	{
		if( !pcView )
			return;
		if( m_cEntries.size() == 0 )
		{
			DEBUG( "Error: View state stack empty!\n" );
			return;
		}
		const ViewStateEntry& cEntry = m_cEntries.top();
		//DEBUG( "Clip RESTORE %i %i %f\n", cEntry.m_cRegion.IsEmpty(), m_cEntries.size(), pcView->GetScrollOffset().y );		
		pcView->SetFgColor( cEntry.m_sSavedFgColor );
		pcView->SetEraseColor( cEntry.m_sSavedEraseColor );
		cRegion.Set( cEntry.m_cRegion );
		if( cEntry.m_cRegion.IsEmpty() )
			pcView->ClearDrawingRegion();
		else {
			/* Move the region to match the scroll offset */
			os::Region cMovedRegion;
			cMovedRegion.Set( cRegion );
			os::ClipRect* pcRect = NULL;
			os::IPoint cScrollOffset = os::IPoint( pcView->GetScrollOffset() );
			ENUMCLIPLIST( &cMovedRegion.m_cRects, pcRect ) {
				pcRect->m_cBounds += cScrollOffset;
			}
			pcView->SetDrawingRegion( cMovedRegion );
		}
		vTransX = cEntry.m_vTransX;
		vTransY = cEntry.m_vTransY;
		m_cEntries.pop();
	}
 	std::stack<ViewStateEntry> m_cEntries;
};

struct TransparencyLayer
{
    TransparencyLayer( os::View* pcParent, ViewState* pcState, int width, int height)
    {
    	m_pcBitmap = new os::Bitmap( width, height, os::CS_RGBA32, os::Bitmap::SHARE_FRAMEBUFFER 
    	| os::Bitmap::ACCEPT_VIEWS );
    	m_pcView = new os::View( os::Rect( 0, 0, width - 1, height - 1 ), "bitmap_view" );
    	
    	m_pcBitmap->AddChild( m_pcView );    	
    	pcState->Restore( m_pcView, m_cRegion, m_vTransX, m_vTransY );
    	m_vTransX = 0;
    	m_vTransY = 0;
    	m_pcView->SetFont( pcParent->GetFont() );
    }

    TransparencyLayer()
        : m_pcView(0), m_pcBitmap(0)
    {
    }

    void cleanup()
    {
        delete m_pcBitmap;
    }

    os::Bitmap* m_pcBitmap;
    os::View* m_pcView;
    ViewState m_cState;
    os::Region m_cRegion;
    float m_vTransX;
    float m_vTransY;
    float opacity;
};
#if 0
struct TextShadow
{
    TextShadow()
        : x(0)
        , y(0)
        , blur(0)
    {
    }

    bool isNull() { return !x && !y && !blur; }

    int x;
    int y;
    int blur;

    Color color;
};
#endif
class GraphicsContextPlatformPrivate
{
public:
    GraphicsContextPlatformPrivate(os::View* pcView);
    ~GraphicsContextPlatformPrivate();

	os::View* p()
	{
		if (layers.empty()) {
            return m_pcView;
        } else
            return layers.top().m_pcView;
	}
	os::Region& region()
	{
		if (layers.empty()) {
            return m_cRegion;
        } else
            return layers.top().m_cRegion;
	}
	ViewState& state()
	{
		if (layers.empty()) {
            return m_cState;
        } else
            return layers.top().m_cState;
	}
	float& transX()
	{
		if (layers.empty()) {
            return m_vTransX;
        } else
            return layers.top().m_vTransX;
	}
	float& transY()
	{
		if (layers.empty()) {
            return m_vTransY;
        } else
            return layers.top().m_vTransY;
	}
	
	
	std::stack<TransparencyLayer> layers;
	ViewState m_cState;
	os::Region m_cRegion;
	IntRect focusRingClip;
	float m_vTransX;
    float m_vTransY;
private:
	
    os::View* m_pcView;
    
};


GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate(os::View* pcView)
{
    m_pcView = pcView;
   
}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{
}

GraphicsContext::GraphicsContext(PlatformGraphicsContext* context)
    : m_common(createGraphicsContextPrivate())
    , m_data(new GraphicsContextPlatformPrivate(context))
{
	//DEBUG( "GraphicsContext::GraphicsContext() for %s\n", context->GetName().c_str() );
    setPaintingDisabled(!context);
    m_data->m_vTransX = m_data->m_vTransY = 0;
}

GraphicsContext::~GraphicsContext()
{

    while(!m_data->layers.empty())
        endTransparencyLayer();

    destroyGraphicsContextPrivate(m_common);
    delete m_data;
}

PlatformGraphicsContext* GraphicsContext::platformContext() const
{
    return m_data->p();
}

void GraphicsContext::savePlatformState()
{
	m_data->state().Save( m_data->p(), m_data->region(), m_data->transX(), m_data->transY() );
}

void GraphicsContext::restorePlatformState()
{
	m_data->state().Restore( m_data->p(), m_data->region(), m_data->transX(), m_data->transY() );
}



// Draws a filled rectangle with a stroked border.
void GraphicsContext::drawRect(const IntRect& r)
{
    if (paintingDisabled())
        return;
        
       
    IntRect rect = r;
    rect.move( (int)m_data->transX(), (int)m_data->transY() );
	
	
	if( m_data->p()->GetEraseColor().alpha > 0 )
	    m_data->p()->EraseRect( os::Rect( rect ) );

	if( strokeStyle() == NoStroke )
		return;

    m_data->p()->MovePenTo( os::Point( rect.x(), rect.y() ) );
    m_data->p()->DrawLine( os::Point( rect.x() + rect.width() - 1, rect.y() ) );
    m_data->p()->DrawLine( os::Point( rect.x() + rect.width() - 1, rect.y() + rect.height() - 1 ) );
    m_data->p()->DrawLine( os::Point( rect.x(), rect.y() + rect.height() - 1 ) );
    m_data->p()->DrawLine( os::Point( rect.x(), rect.y() ) );
}

// FIXME: Now that this is refactored, it should be shared by all contexts.
static void adjustLineToPixelBoundaries(FloatPoint& p1, FloatPoint& p2, float strokeWidth,
                                        const StrokeStyle& penStyle)
{
    // For odd widths, we add in 0.5 to the appropriate x/y so that the float arithmetic
    // works out.  For example, with a border width of 3, KHTML will pass us (y1+y2)/2, e.g.,
    // (50+53)/2 = 103/2 = 51 when we want 51.5.  It is always true that an even width gave
    // us a perfect position, but an odd width gave us a position that is off by exactly 0.5.
    if (penStyle == DottedStroke || penStyle == DashedStroke) {
        if (p1.x() == p2.x()) {
            p1.setY(p1.y() + strokeWidth);
            p2.setY(p2.y() - strokeWidth);
        } else {
            p1.setX(p1.x() + strokeWidth);
            p2.setX(p2.x() - strokeWidth);
        }
    }

    if (((int) strokeWidth) % 2) {
        if (p1.x() == p2.x()) {
            // We're a vertical line.  Adjust our x.
            p1.setX(p1.x() + 0.5);
            p2.setX(p2.x() + 0.5);
        } else {
            // We're a horizontal line. Adjust our y.
            p1.setY(p1.y() + 0.5);
            p2.setY(p2.y() + 0.5);
        }
    }
}

// This is only used to draw borders.
void GraphicsContext::drawLine(const IntPoint& point1, const IntPoint& point2)
{
    if (paintingDisabled())
        return;
        
//   	if( strokeStyle() == NoStroke )
//		return;

  
    FloatPoint p1 = point1;
    FloatPoint p2 = point2;
    
	p1.move( m_data->transX(), m_data->transY() );
    p2.move( m_data->transX(), m_data->transY() );
	

    adjustLineToPixelBoundaries(p1, p2, strokeThickness(), strokeStyle());
    m_data->p()->DrawLine( p1, p2);
}

// This method is only used to draw the little circles used in lists.
void GraphicsContext::drawEllipse(const IntRect& rect)
{
    if (paintingDisabled())
        return;
	notImplemented();
//    m_data->p().drawEllipse(rect);
}

void GraphicsContext::strokeArc(const IntRect& rect, int startAngle, int angleSpan)
{
    if (paintingDisabled())
        return;

	notImplemented();
	#if 0
    const QPen oldPen = m_data->p().pen();
    QPen nPen = oldPen;
    nPen.setWidthF(thickness);
    m_data->p().setPen(nPen);
    m_data->p().drawArc(rect, startAngle, angleSpan);
    m_data->p().setPen(oldPen);
    #endif
}

void GraphicsContext::drawConvexPolygon(size_t npoints, const FloatPoint* points, bool shouldAntialias)
{
    if (paintingDisabled())
        return;
        
    /* This is a hack */
    os::Color32_s sSaveColor;
    if (strokeStyle() == NoStroke) {
		sSaveColor = m_data->p()->GetFgColor();
		m_data->p()->SetFgColor( m_data->p()->GetEraseColor() );
    }

    if (npoints <= 1)
        return;
        
    
	m_data->p()->MovePenTo( os::Point( points[0] ) + os::Point( m_data->transX(), m_data->transY() ) );
	for (size_t i = 1; i < npoints; i++)
		m_data->p()->DrawLine( os::Point( points[i] ) + os::Point( m_data->transX(), m_data->transY() ) );
	m_data->p()->DrawLine( os::Point( points[0] ) + os::Point( m_data->transX(), m_data->transY() ) );
	
    if (strokeStyle() == NoStroke) {
    	m_data->p()->SetFgColor( sSaveColor );
    }
	
    #if 0

    QPolygonF polygon(npoints);

    for (size_t i = 0; i < npoints; i++)
        polygon[i] = points[i];

    m_data->p().save();
    m_data->p().setRenderHint(QPainter::Antialiasing, shouldAntialias);
    m_data->p().drawConvexPolygon(polygon);
    m_data->p().restore();
    #endif
}

void GraphicsContext::fillRect(const IntRect& rect, const Color& c)
{
    if (paintingDisabled())
        return;

	if( c.alpha() )
	    m_data->p()->FillRect( os::Rect( rect ) + os::Point( m_data->transX(), m_data->transY() ), os::Color32_s(c));
}

void GraphicsContext::fillRect(const FloatRect& rect, const Color& c)
{
    if (paintingDisabled())
        return;

	if( c.alpha() )
	    m_data->p()->FillRect( os::Rect( rect ) + os::Point( m_data->transX(), m_data->transY() ), os::Color32_s(c));
}


void GraphicsContext::fillRoundedRect(const IntRect& rect, const IntSize& topLeft, const IntSize& topRight, const IntSize& bottomLeft, const IntSize& bottomRight, const Color& color)
{
    if (paintingDisabled() || !color.alpha())
        return;

    // FIXME: Implement.
    notImplemented();
}

void GraphicsContext::beginPath()
{
    notImplemented();
}

void GraphicsContext::addPath(const Path& path)
{
   notImplemented();
}


void GraphicsContext::clip(const IntRect& rect)
{
    if (paintingDisabled())
        return;

	os::Rect cRect = os::IRect( rect );
	//cRect += os::Point( m_data->transX(), m_data->transY() );
	m_data->region().Set( cRect );
	cRect += m_data->p()->GetScrollOffset();	
	m_data->p()->SetDrawingRegion( os::Region( cRect ) );
	//DEBUG("Clip %i %i %i %i\n", rect.x(), rect.y(), rect.width(), rect.height() );
    
}

AffineTransform GraphicsContext::getCTM() const
{ 
    notImplemented();
    return AffineTransform();
}

void setFocusRingColorChangeFunction(void (*)()) { }

void GraphicsContext::drawFocusRing(const Color& color)
{
    if (paintingDisabled())
        return;

    const Vector<IntRect>& rects = focusRingRects();
    unsigned rectCount = rects.size();
    os::Color32_s sSaveColor = m_data->p()->GetFgColor();
    m_data->p()->SetFgColor( os::get_default_color( os::COL_FOCUS ) );
    for (unsigned i = 0; i < rectCount; i++)
    {
    	strokeRect( rects[i], 1 );
    }
    m_data->p()->SetFgColor( sSaveColor );
}

#if 0
void GraphicsContext::setFocusRingClip(const IntRect& rect)
{
    if (paintingDisabled())
        return;

    m_data->focusRingClip = rect;
}

void GraphicsContext::clearFocusRingClip()
{
    if (paintingDisabled())
        return;

    m_data->focusRingClip = IntRect();
}
#endif

void GraphicsContext::drawLineForText(const IntPoint& origin, int width, bool printing)
{
    if (paintingDisabled())
        return;

    IntPoint endPoint = origin + IntSize(width, 0);
    drawLine(origin, endPoint);
}

void GraphicsContext::drawLineForMisspellingOrBadGrammar(const IntPoint&,
                                                         int width, bool grammar)
{
    if (paintingDisabled())
        return;

    notImplemented();
}

FloatRect GraphicsContext::roundToDevicePixels(const FloatRect& frect)
{
	os::Rect cRect = frect;
	cRect.Ceil();
	
	return( cRect );
	#if 0
	
    QRectF rect(frect);
    rect = m_data->p().deviceMatrix().mapRect(rect);

    QRect result = rect.toRect(); //round it
    return FloatRect(QRectF(result));
    #endif
}

void GraphicsContext::setShadow(const IntSize& pos, int blur, const Color &color)
{
    if (paintingDisabled())
        return;

	notImplemented();
	#if 0
    m_data->shadow.x = pos.width();
    m_data->shadow.y = pos.height();
    m_data->shadow.blur = blur;
    m_data->shadow.color = color;
    #endif
}

void GraphicsContext::clearShadow()
{
    if (paintingDisabled())
        return;

    //m_data->shadow = TextShadow();
}

void GraphicsContext::beginTransparencyLayer(float opacity)
{
    if (paintingDisabled())
        return;
        
    DEBUG( "Warning: transparency layers not fully implemented (opacity=%f)!\n", opacity );

    ViewState cState;
    cState.Save( m_data->p(), m_data->region(), m_data->transX(), m_data->transY() );
    TransparencyLayer layer(m_data->p(), &cState,
                            (int)m_data->p()->GetFrame().Width() + 1,
                            (int)m_data->p()->GetFrame().Height() + 1);

	layer.opacity = opacity;
    m_data->layers.push(layer);
}

void GraphicsContext::endTransparencyLayer()
{
    if (paintingDisabled())
        return;
        
    TransparencyLayer layer = m_data->layers.top();
    m_data->layers.pop();
    layer.m_pcBitmap->Sync();
    
    DEBUG("EndTransparencyLayer %f %f!\n", layer.m_pcBitmap->GetBounds().Width(), layer.m_pcBitmap->GetBounds().Height());

	uint32* pRaster = (uint32*)layer.m_pcBitmap->LockRaster();
	uint32 nOpacity = (uint32)( layer.opacity * 255 ) << 24;
	for( int i = 0; i <= layer.m_pcBitmap->GetBounds().Height(); i++ )
	{
		for( int j = 0; j <= layer.m_pcBitmap->GetBounds().Width(); j++ )
		{
			pRaster[j] &= ~0xff000000;
			pRaster[j] |= nOpacity;
		}
		pRaster += layer.m_pcBitmap->GetBytesPerRow() / 4;
	}

	os::drawing_mode eMode = m_data->p()->GetDrawingMode();
	m_data->p()->SetDrawingMode( os::DM_BLEND );
	m_data->p()->DrawBitmap( layer.m_pcBitmap, layer.m_pcBitmap->GetBounds(), m_data->p()->GetBounds() );
	m_data->p()->SetDrawingMode( eMode );
	m_data->p()->Sync();
	
    layer.cleanup();

}

void GraphicsContext::clearRect(const FloatRect& rect)
{
    if (paintingDisabled())
        return;

    m_data->p()->EraseRect( os::Rect( rect ) + os::Point( m_data->transX(), m_data->transY() ) );
}

void GraphicsContext::strokeRect(const FloatRect& r, float width)
{
    if (paintingDisabled())
        return;
        
	FloatRect rect = r;
	rect.move( m_data->transX(), m_data->transY() );
  
	m_data->p()->MovePenTo( os::Point( rect.x(), rect.y() ) );
    m_data->p()->DrawLine( os::Point( rect.x() + rect.width() , rect.y() ) );
    m_data->p()->DrawLine( os::Point( rect.x() + rect.width(), rect.y() + rect.height() ) );
    m_data->p()->DrawLine( os::Point( rect.x(), rect.y() + rect.height() ) );
    m_data->p()->DrawLine( os::Point( rect.x(), rect.y() ) );
}


void GraphicsContext::setLineCap(LineCap lc)
{
    if (paintingDisabled())
        return;
        
    notImplemented();
    #if 0

    QPen nPen = m_data->p().pen();
    nPen.setCapStyle(toQtLineCap(lc));
    m_data->p().setPen(nPen);
    #endif
}

void GraphicsContext::setLineJoin(LineJoin lj)
{
    if (paintingDisabled())
        return;
    
    notImplemented();
    #if 0

    QPen nPen = m_data->p().pen();
    nPen.setJoinStyle(toQtLineJoin(lj));
    m_data->p().setPen(nPen);
    #endif
}

void GraphicsContext::setMiterLimit(float limit)
{
    if (paintingDisabled())
        return;

	notImplemented();
	#if 0

    QPen nPen = m_data->p().pen();
    nPen.setMiterLimit(limit);
    m_data->p().setPen(nPen);
    #endif
}

void GraphicsContext::setAlpha(float opacity)
{
    if (paintingDisabled())
        return;
    notImplemented();
    
    #if 0

    m_data->p().setOpacity(opacity);
    #endif
}

void GraphicsContext::setCompositeOperation(CompositeOperator op)
{
    if (paintingDisabled())
        return;
        
    switch( op )
    {
    	case CompositeCopy:
    	case CompositeClear:    		
    		m_data->p()->SetDrawingMode( os::DM_COPY );
    	break;
    	case CompositeSourceOver:
    		m_data->p()->SetDrawingMode( os::DM_BLEND );
    	break;
    	default:
    	{
    		DEBUG( "Unknown composite operation %i\n", (int) op );
    		m_data->p()->SetDrawingMode( os::DM_COPY );
    	}
    }

}

void GraphicsContext::clip(const Path& path)
{
    if (paintingDisabled())
        return;

	notImplemented();
	#if 0

    m_data->p().setClipPath(*path.platformPath());
    #endif
}

void GraphicsContext::clipOut(const Path& path)
{
    if (paintingDisabled())
        return;
        
    // FIXME: Implement
    notImplemented();
}

void GraphicsContext::translate(float x, float y)
{
    if (paintingDisabled())
        return;
        
    m_data->transX() = x;
    m_data->transY() = y;
    
    #if 0

    m_data->p().translate(x, y);
    #endif
}

IntPoint GraphicsContext::origin()
{
	return IntPoint((int)m_data->transX(), (int)m_data->transY());
	#if 0
    return IntPoint(qRound(m_data->p().matrix().dx()),
                    qRound(m_data->p().matrix().dy()));
    #endif
}

void GraphicsContext::rotate(float radians)
{
    if (paintingDisabled())
        return;
        
    notImplemented();
    #if 0

    m_data->p().rotate(radians);
    #endif
}

void GraphicsContext::scale(const FloatSize& s)
{
    if (paintingDisabled())
        return;
        
    notImplemented();
    #if 0

    m_data->p().scale(s.width(), s.height());
    #endif
}


void GraphicsContext::clipOut(const IntRect& rect)
{
    if (paintingDisabled())
        return;
        
    // FIXME: Implement
    notImplemented();
}

void GraphicsContext::clipOutEllipseInRect(const IntRect& rect)
{
    if (paintingDisabled())
        return;
    
    // FIXME: Implement.
    notImplemented();
}


void GraphicsContext::addInnerRoundedRectClip(const IntRect& rect,
                                              int thickness)
{
    if (paintingDisabled())
        return;
        
    notImplemented();
    #if 0

    clip(rect);
    QPainterPath path;

    // Add outer ellipse
    path.addEllipse(QRectF(rect.x(), rect.y(), rect.width(), rect.height()));

    // Add inner ellipse.
    path.addEllipse(QRectF(rect.x() + thickness, rect.y() + thickness,
                           rect.width() - (thickness * 2), rect.height() - (thickness * 2)));

    path.setFillRule(Qt::OddEvenFill);
    m_data->p().setClipPath(path, Qt::IntersectClip);
    #endif
}


void GraphicsContext::concatCTM(const AffineTransform& transform)
{
    if (paintingDisabled())
        return;
        
    notImplemented();

    //m_data->p().setMatrix(transform, true);
}

void GraphicsContext::setURLForRect(const KURL& link, const IntRect& destRect)
{
    notImplemented();
}

void GraphicsContext::setPlatformFont(const Font& aFont)
{
    m_data->p()->SetFont(aFont.primaryFont()->platformData().fontPtr());
}


void GraphicsContext::setPlatformStrokeColor(const Color& color)
{
    if (paintingDisabled())
        return;
    m_data->p()->SetFgColor( color );
}


void GraphicsContext::setPlatformStrokeStyle(const StrokeStyle& strokeStyle)
{   
    if (paintingDisabled())
        return;
//	notImplemented();    
}


void GraphicsContext::setPlatformStrokeThickness(float thickness)
{
    if (paintingDisabled())
        return;
}

void GraphicsContext::setPlatformFillColor(const Color& color)
{
	//DEBUG( "Color %i %i %i\n", color.red(), color.green(), color.blue() );
	m_data->p()->SetEraseColor( os::Color32_s( color ) );
}


}

// vim: ts=4 sw=4 et








































































