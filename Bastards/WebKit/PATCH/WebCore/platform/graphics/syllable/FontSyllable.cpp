/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2006 Allan Sandfeld Jensen <sandfeld@kde.org>
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

#include "SimpleFontData.h"
#include "GraphicsContext.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/font.h>
#include <gui/view.h>

namespace WebCore {
	
static inline bool CompColors( os::Color32_s sColor1, os::Color32_s sColor2 )
{
	if( sColor1.red == sColor2.red &&
		sColor1.green == sColor2.green &&
		sColor1.blue == sColor2.blue &&
		sColor1.alpha == sColor2.alpha )
		return( true );
	return( false );
}

void Font::drawGlyphs(GraphicsContext* graphicsContext, const SimpleFontData* font, const GlyphBuffer& glyphBuffer,
                      int from, int numGlyphs, const FloatPoint& point) const
{
	os::View* view = graphicsContext->platformContext();
	
	char zBuffer[numGlyphs*4];
	int nPointer = 0;
	
    const GlyphBufferGlyph *glyphs = glyphBuffer.glyphs(from);
    const GlyphBufferAdvance *advances = glyphBuffer.advances(from);

	#if 0	
	for( int i = from; i < from + numGlyphs; i++ )
	{
		nPointer += os::unicode_to_utf8( zBuffer + nPointer, glyphBuffer.glyphAt( i ) );
	}
	zBuffer[nPointer] = '\0';

	DEBUG( "Font::drawGlyphs() %s\n", zBuffer );
		for( int i = from; i < from + numGlyphs; i++ )
		{
			DEBUG("%i %i %i\n", glyphBuffer.glyphAt( i ), glyphBuffer.glyphAt( i ) & 0xff, ( glyphBuffer.glyphAt( i ) & 0xff00 ) >> 8 );
		}
#endif
	
	os::Color32_s sOldColor = view->GetFgColor();
	os::Color32_s sNewColor = graphicsContext->fillColor();
	
	if( !CompColors( sOldColor, sNewColor ) )
		view->SetFgColor( sNewColor );
	os::drawing_mode eMode = view->GetDrawingMode();
	
	view->SetDrawingMode( os::DM_OVER );
	view->SetFont( font->platformData().fontPtr());	

	int spanStart = 0;
    float x = point.x();
    float y = point.y();
    float width = 0;
    int i = 0;
    for (; i < numGlyphs; ++i) {
        if (advances[i].width() == font->widthForGlyph(glyphs[i])
            && advances[i].height() == 0) {
            width += advances[i].width();
            continue;
        }
        nPointer = 0;
		for( int j = spanStart; j < i; j++ )
		{
			nPointer += os::unicode_to_utf8( zBuffer + nPointer, glyphs[j]/*glyphBuffer.glyphAt( from + j )*/ );
		}
		zBuffer[nPointer] = '\0';

		view->DrawString( os::Point( x, y ), zBuffer );    
        x += width;
        width = advances[i].width();
        spanStart = i;
    }

    if (i - spanStart > 0)
    {
        nPointer = 0;
		for( int j = spanStart; j < i; j++ )
		{
			nPointer += os::unicode_to_utf8( zBuffer + nPointer, glyphs[j]/*glyphBuffer.glyphAt( j )*/ );
		}
		zBuffer[nPointer] = '\0';

		view->DrawString( os::Point( x, y ), zBuffer );    
	}
	view->SetDrawingMode( eMode );
	if( !CompColors( sOldColor, sNewColor ) )
		view->SetFgColor( sOldColor );
}

void Font::drawComplexText(GraphicsContext* ctx, const TextRun& run, const FloatPoint& point, int from, int to) const
{
	const UChar* buffer = (run.characters() + from);

	
	DEBUG("Font::drawComplexText\n" );
	
	char zBuffer[run.length()*4];
	int nPointer = 0;
	
	for( int i = 0; i < run.length(); i++ )
	{
		nPointer += os::unicode_to_utf8( zBuffer + nPointer, buffer[i] );
	}
	zBuffer[nPointer] = '\0';
	Color color = ctx->fillColor();
	ctx->platformContext()->SetFgColor( color );
	os::drawing_mode eMode = ctx->platformContext()->GetDrawingMode();
	ctx->platformContext()->SetDrawingMode( os::DM_OVER );
	ctx->platformContext()->DrawText( os::Rect( point.x(), point.y(), 0, 0 ), zBuffer );
	ctx->platformContext()->SetDrawingMode( eMode );
	ctx->platformContext()->SetDrawingMode( os::DM_OVER );                              
}

float Font::floatWidthForComplexText(const TextRun& run) const
{
	notImplemented();
	const UChar* buffer = (run.characters() );
	
	char zBuffer[run.length()*4];
	int nPointer = 0;
	
	for( int i = 0; i < run.length(); i++ )
	{
		nPointer += os::unicode_to_utf8( zBuffer + nPointer, buffer[i] );
	}
	zBuffer[nPointer] = '\0';
	os::Point cSize = primaryFont()->m_font.fontPtr()->GetTextExtent( zBuffer );
	return( cSize.x );
}

}

// vim: ts=4 sw=4 et









