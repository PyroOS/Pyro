/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org> 
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "SimpleFontData.h"

#include "Font.h"
#include "FloatRect.h"
#include "FontCache.h"
#include "GlyphBuffer.h"
#include "FontDescription.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/font.h>

namespace WebCore {

void SimpleFontData::platformInit()
{
	os::Font* pcFont = m_font.fontPtr();
	
	pcFont->AddRef();
	os::font_height sHeight;
	pcFont->GetHeight( &sHeight );
	
	DEBUG("SimpleFontData::platformInit() %s\n", pcFont->GetFamily().c_str() );
	
    m_ascent = (int)( sHeight.ascender );
    m_descent = (int)( sHeight.descender );
    m_lineGap = (int)( sHeight.line_gap );
    m_xHeight = sHeight.ascender + sHeight.descender + sHeight.line_gap + 0.5f;
    m_lineSpacing = (int)( sHeight.ascender + sHeight.descender + sHeight.line_gap + 0.5f );
}

void SimpleFontData::platformDestroy()
{
	DEBUG("SimpleFontData::platformDestroy() %s\n", m_font.fontPtr()->GetFamily().c_str() );
	m_font.fontPtr()->Release();
}

SimpleFontData* SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_smallCapsFontData) {
        FontDescription desc = FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f * fontDescription.computedSize());
        FontPlatformData pdata(desc, m_font.fontPtr()->GetFamily().c_str());
        
        m_smallCapsFontData = FontCache::getCachedFontData(&pdata);
    }

    return m_smallCapsFontData;
}

bool SimpleFontData::containsCharacters(const UChar* characters, int length) const
{
	notImplemented();
	return true;
}

void SimpleFontData::determinePitch()
{
    m_treatAsFixedPitch = m_font.isFixedPitch();
}

float SimpleFontData::platformWidthForGlyph(Glyph glyph) const
{
	char zString[5];
	zString[os::unicode_to_utf8( zString, ushort(glyph) )] = '\0';
	return( m_font.fontPtr()->GetStringWidth( zString ) );
}

}

// vim: ts=4 sw=4 et
