/*
 * This file is part of the internal font implementation.  It should not be included by anyone other than
 * FontMac.cpp, FontWin.cpp and Font.cpp.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "FontPlatformData.h"

#include "DeprecatedString.h"
#include "FontDescription.h"
#include "AtomicString.h"
#include "SyllableDebug.h"

#include <gui/font.h>

namespace WebCore {

FontPlatformData::FontPlatformData()
    : m_font( 0 )
{
	DEBUG("FontPlatformData::FontPlatformData()\n" );
}

FontPlatformData::FontPlatformData(Deleted)
    : m_font( 0 )
{
	DEBUG("FontPlatformData::FontPlatformData(Deleted)\n" );
}

FontPlatformData::FontPlatformData(const FontPlatformData& f)
{
	DEBUG("FontPlatformData::FontPlatformData(FontPlatformData) %s\n", f.fontPtr()->GetFamily().c_str() );

	m_font = f.m_font;
	if( m_font )
		m_font->AddRef();
}

FontPlatformData::FontPlatformData(const FontDescription& fontDescription, const AtomicString& familyName)
    : m_font( 0 )
{
	bool bFound = false;
	bool bUsingDefault = false;
	bool bItalic = fontDescription.italic();
	bool bBold = fontDescription.bold();
	os::font_properties sProps;
	char zStyleName[FONT_STYLE_LENGTH];
	os::String cFamily = os::String( familyName.domString() );
	os::String cLower = cFamily;
	cLower.Lower();

	/* Replace some common family names 
	 * Note: If we change the fonts that ship with syllable we need to change this
	 */
	
	if( cLower == "arial" || cLower == "helvetica" )
		cFamily = "DejaVu Sans";
	else if( cLower == "times" || cLower == "times new roman" )
		cFamily = "DejaVu Serif";
	else if( cLower == "courier" || cLower == "courier new" )
		cFamily = "DejaVu Sans Mono";
again:	
	/* Search the font
	 * TODO: Improve the syllable font system to make this easier
	 */
	for( int i = 0; i < os::Font::GetStyleCount( cFamily.c_str() ); ++ i )
	{
		//DEBUG( "%s STYLE %i\n", cFamily.c_str(), i );
		uint32 nFlags;
		if( os::Font::GetStyleInfo( cFamily.c_str(), i, zStyleName, &nFlags ) == 0 )
		{
			
			if( ( bItalic && bBold ) &&  ( !strcmp( zStyleName, "Bold Italic" )
				|| !strcmp( zStyleName, "Bold Oblique" ) ) )
			{
				sProps.m_cStyle = zStyleName;
				bFound = true;
				//DEBUG( "FOUND BOLD ITALIC!\n" );
				break;
			}
			if( ( bItalic && !bBold ) && ( !strcmp( zStyleName, "Italic" )
				|| !strcmp( zStyleName, "Oblique" ) ) )
			{
				sProps.m_cStyle = zStyleName;
				bFound = true;				
				//DEBUG( "FOUND ITALIC!\n" );
				break;
			}
			if( ( !bItalic && bBold ) && ( !strcmp( zStyleName, "Bold" ) ) )
			{
				sProps.m_cStyle = zStyleName;
				bFound = true;				
				//DEBUG( "FOUND BOLD!\n" );
				break;
			}
			
			if( ( !bItalic && !bBold ) && ( !strcmp( zStyleName, "Roma" )
			|| !strcmp( zStyleName, "Book" ) || !strcmp( zStyleName, "Condensed" )
			|| !strcmp( zStyleName, "Regular" ) || !strcmp( zStyleName, "Medium" ) ) )
			{
				sProps.m_cStyle = zStyleName;
				bFound = true;				
				DEBUG( "FOUND REGULAR!\n" );
				break;
			}
		}
	}
	if( bFound )
	{
		sProps.m_cFamily = cFamily;
		sProps.m_vSize = fontDescription.computedSize() * 72 / 96;
		sProps.m_nFlags = os::FPF_SMOOTHED;
	}
	else
	{
		if( bUsingDefault )
		{
			DEBUG("Warning: DejaVu fonts not installed! Using default font!\n");
			os::Font::GetDefaultFont( ( fontDescription.genericFamily() == FontDescription::MonospaceFamily ) ? 
						DEFAULT_FONT_FIXED : ( bBold ? DEFAULT_FONT_BOLD : DEFAULT_FONT_REGULAR ), &sProps );
			sProps.m_vSize = fontDescription.computedSize() * 72 / 96;
		}
		else
		{
			DEBUG( "Using default font!\n");
			if( fontDescription.genericFamily() == FontDescription::MonospaceFamily )
				cFamily = "DejaVu Sans Mono";
			else if( fontDescription.genericFamily() == FontDescription::SerifFamily )
				cFamily = "DejaVu Serif";
			else
				cFamily = "DejaVu Sans";
			bUsingDefault = true;
			goto again;
		}
	}
	m_font = new os::Font( sProps );
#if 0
	os::font_height sHeight;
	m_font->GetHeight( &sHeight );
	DEBUG("FontPlatformData::FontPlatformData() %s %i %f %i %i %i\n", cFamily.c_str(), (int)fontDescription.computedSize(), sHeight.ascender + sHeight.descender,
	fontDescription.bold(), fontDescription.italic(), fontDescription.genericFamily() );
#endif
}

FontPlatformData::FontPlatformData(const FontDescription& fontDescription, os::Font* font)
{
	m_font = font;
	m_font->SetSize( fontDescription.computedSize() * 72 / 96 );
	DEBUG("FontPlatformData::FontPlatformData() from native\n" );
}


FontPlatformData::~FontPlatformData()
{
	DEBUG("FontPlatformData::~FontPlatformData()" );
	if( m_font )
	{
		DEBUG(" %s", m_font->GetFamily().c_str() );
		m_font->Release();
	}
	DEBUG( "\n" );
}

bool FontPlatformData::isFixedPitch()
{
	uint32 nFlags = m_font->GetFlags();
	return( nFlags & os::FPF_MONOSPACED );
}

os::Font* FontPlatformData::fontPtr() const
{
    return m_font;
}

unsigned FontPlatformData::hash() const
{
	uintptr_t hashCodes[1] = { (uintptr_t)fontPtr() };
	unsigned nResult = StringImpl::computeHash(reinterpret_cast<UChar*>(hashCodes), sizeof(hashCodes) / sizeof(UChar));
	return( nResult );
}

bool FontPlatformData::operator==(const FontPlatformData& other) const
{
    if (m_font == other.m_font)
        return true;

    if (!m_font || m_font == (os::Font*) -1 ||
        !other.m_font || other.m_font == (os::Font*) -1)
       {
        return false;
       }

	os::Font* pcOther = other.fontPtr();
#if 0
	os::font_height sDumpHeight;
	os::font_height sDumpOtherHeight;
	m_font->GetHeight( &sDumpHeight );
	pcOther->GetHeight( &sDumpOtherHeight );
	DEBUG( "Warning: FontPlatformData::operator==() %s:%f %s:%f\n", m_font->GetFamily().c_str(), sDumpHeight.ascender + sDumpHeight.descender,
	other.m_font->GetFamily().c_str(), sDumpOtherHeight.ascender + sDumpOtherHeight.descender );
#endif
	
	if( m_font->GetFamily() == pcOther->GetFamily() &&
		m_font->GetStyle() == pcOther->GetStyle() &&
		m_font->GetFlags() == pcOther->GetFlags() )
	{
		os::font_height sHeight;
		os::font_height sOtherHeight;
		m_font->GetHeight( &sHeight );
		pcOther->GetHeight( &sOtherHeight );
		if( sHeight.ascender == sOtherHeight.ascender &&
			sHeight.descender == sOtherHeight.descender &&
			sHeight.line_gap == sOtherHeight.line_gap )
		{
			//DEBUG( "====\n" );
			return( true );
		}
	}

	return( false );
//    return (*m_font == *other.m_font);
}

}

// vim: ts=4 sw=4 et
