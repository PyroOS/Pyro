/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 *               2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * All rights reserved.
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

#include "config.h"
#include <gui/view.h>

#include "Document.h"
#include "RenderTheme.h"
#include "GraphicsContext.h"

#include <gui/font.h>

namespace WebCore {

class RenderThemeSyllable : public RenderTheme
{
public:
    RenderThemeSyllable() : RenderTheme() { }
    // A method asking if the theme's controls actually care about redrawing when hovered.
    virtual bool supportsHover(const RenderStyle* style ) const {
    switch (style->appearance()) {
        case CheckboxAppearance:
        case RadioAppearance:
            return true;
   	    default:
			return false;
    	}

	    return false;
    }
    
    Color platformActiveSelectionBackgroundColor() const
	{
	    // Black
	    return Color(0, 0, 0);
	}
	
	 Color platformActiveSelectionForegroundColor() const
	{
	    // White
	    return Color(255, 255, 255);
	}

    virtual bool paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
    {
        return paintRadio(o, i, r);
    }

    virtual void setCheckboxSize(RenderStyle*) const;

    virtual bool paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
    {
        GraphicsContext* context = i.context;
        if( !context )
        	return( true );
        
        IntRect rect = r;
        rect.move( 1, 1 );
        rect.setWidth( rect.width() - 2 );
        rect.setHeight( rect.height() - 2 ); 
        context->setStrokeColor( Color( 0, 0, 0 ) );
        context->strokeRect( rect, 1 );
        if( isChecked( o ) )
        {
        	rect.move( 2, 2 );
	        rect.setWidth( rect.width() - 4 );
    	    rect.setHeight( rect.height() - 4 ); 
    	    context->fillRect( rect, Color( 0, 0, 0 ) );
        }
        else if( isHovered( o ) )
        {
        	rect.move( 2, 2 );
	        rect.setWidth( rect.width() - 4 );
    	    rect.setHeight( rect.height() - 4 ); 
    	    context->fillRect( rect, Color( 200, 200, 200 ) );
        }
        return( false );
    }

    virtual void setRadioSize(RenderStyle*) const;

    virtual bool paintButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r)
	{
       GraphicsContext* context = i.context;
        if( !context )
        	return( true );
        
        IntRect rect = r;
        rect.move( 1, 1 );
        rect.setWidth( rect.width() - 2 );
        rect.setHeight( rect.height() - 2 ); 
        context->setStrokeColor( Color( 0, 0, 0 ) );
        context->strokeRect( rect, 1 );
        if( isPressed( o ) )
        {
        	rect.move( 2, 2 );
	        rect.setWidth( rect.width() - 4 );
    	    rect.setHeight( rect.height() - 4 ); 
    	    context->fillRect( rect, Color( 200, 200, 200 ) );
        }

		return false;
	}

    void adjustMenuListStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
    {
    	/* Leave some space for the arrow */
    	style->setPaddingRight(Length(22, Fixed));
    	const int minHeight = 20;
	    style->setMinHeight(Length(minHeight, Fixed));
    }
    bool paintMenuList(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
	{
		GraphicsContext* context = paintInfo.context;
        if( !context )
        	return( true );
    	IntRect bounds = IntRect(r.x() + o->style()->borderLeftWidth(),
                             r.y() + o->style()->borderTopWidth(),
                             r.width() - o->style()->borderLeftWidth() - o->style()->borderRightWidth(),
                             r.height() - o->style()->borderTopWidth() - o->style()->borderBottomWidth());
        float centerY = bounds.y() + bounds.height() / 2.0;
        centerY -= 5;
        float leftEdge = bounds.right() - 4 - 10;
        os::View* pcView = context->platformContext();
        pcView->SetEraseColor( os::get_default_color( os::COL_NORMAL ) );
        pcView->FillRect( os::Rect( r ), os::Color32_s( 255, 255, 255 ) );
	    pcView->FillRect( os::Rect( leftEdge - 6, bounds.y(), bounds.right(), bounds.bottom() ), os::get_default_color( os::COL_NORMAL ) );
	    if( isFocused( o ) )
	    {
	    	pcView->SetFgColor( os::get_default_color( os::COL_FOCUS ) );
		    pcView->MovePenTo( os::Point( bounds.x(), bounds.y() ) );
		    pcView->DrawLine( os::Point( bounds.right(), bounds.y() ) );
		    pcView->DrawLine( os::Point( bounds.right(), bounds.bottom() ) );
		    pcView->DrawLine( os::Point( bounds.x(), bounds.bottom() ) );
		    pcView->DrawLine( os::Point( bounds.x(), bounds.y() ) );
		    pcView->DrawLine( os::Point( leftEdge - 6, bounds.y() ), os::Point( leftEdge - 6, bounds.bottom() ) );
		}
	    pcView->SetFgColor( os::Color32_s( 0, 0, 0 ) );
	    centerY += 3;
	    pcView->DrawLine( os::Point( leftEdge, centerY ), os::Point( leftEdge + 8, centerY ) );
	    pcView->DrawLine( os::Point( leftEdge + 1, centerY + 1 ), os::Point( leftEdge + 7, centerY + 1 ) );
	    pcView->DrawLine( os::Point( leftEdge + 2, centerY + 2 ), os::Point( leftEdge + 6, centerY + 2 ) );	    
   	    pcView->DrawLine( os::Point( leftEdge + 3, centerY + 3 ), os::Point( leftEdge + 5, centerY + 3 ) );
   	    pcView->DrawLine( os::Point( leftEdge + 4, centerY + 4 ), os::Point( leftEdge + 4, centerY + 4 ) );   	    
        return( false );
    }

    virtual void systemFont(int propId, FontDescription&) const;
    
private:
    void addIntrinsicMargins(RenderStyle*) const;
    void close();

    bool supportsFocus(EAppearance) const;
};

RenderTheme* theme()
{
    static RenderThemeSyllable rt;
    return &rt;
}

void RenderThemeSyllable::systemFont(int propId, FontDescription& fontDescription) const
{
	os::font_properties sProps;
	os::Font::GetDefaultFont( DEFAULT_FONT_REGULAR, &sProps );
	fontDescription.setIsAbsoluteSize( true );
	fontDescription.setGenericFamily(FontDescription::NoFamily);
	fontDescription.setSpecifiedSize( (int)sProps.m_vSize * 96 / 72 );
}

void RenderThemeSyllable::setCheckboxSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    // FIXME:  A hard-coded size of 13 is used.  This is wrong but necessary for now.  It matches Firefox.
    // At different DPI settings on Windows, querying the theme gives you a larger size that accounts for
    // the higher DPI.  Until our entire engine honors a DPI setting other than 96, we can't rely on the theme's
    // metrics.
    if (style->width().isIntrinsicOrAuto())
        style->setWidth(Length(13, Fixed));

    if (style->height().isAuto())
        style->setHeight(Length(13, Fixed));
}

void RenderThemeSyllable::setRadioSize(RenderStyle* style) const
{
    // This is the same as checkboxes.
    setCheckboxSize(style);
}


}

// vim: ts=4 sw=4 et





















