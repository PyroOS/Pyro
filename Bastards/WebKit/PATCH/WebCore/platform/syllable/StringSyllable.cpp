/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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

#include "PlatformString.h"
#include "DeprecatedString.h"

#include <util/string.h>
#include <gui/font.h>

namespace WebCore {

// String conversions
String::String(const os::String& sstr)
{
	/* Convert from utf8 to utf16 */
	unsigned int len = sstr.Length();
	
	if (len == 0)
	{
        m_impl = StringImpl::empty();
        return;
    }
	
	UChar buffer[sstr.CountChars()];
	const char* data = sstr.c_str();
	int c = 0;
	
	for( int i = 0; i < len; )
	{
		buffer[c++] = os::utf8_to_unicode( data );
		int length = os::utf8_char_length( *data );
		data += length;
		i += length;
	}
    m_impl = StringImpl::create( buffer, c );
}

String::operator os::String() const
{
	/* Convert from utf16 to utf 8 */
	os::String cString;
	const UChar* buffer = characters();
	for( int i = 0; i < length(); i++ )
	{
		char zBuffer[5];
		zBuffer[os::unicode_to_utf8( zBuffer, buffer[i] )] = '\0';
		cString += zBuffer;
	}
	return( cString );
}

// DeprecatedString conversions
DeprecatedString::operator os::String() const
{
    return os::String(reinterpret_cast<const char*>(ascii()), length());
}

}

// vim: ts=4 sw=4 et







