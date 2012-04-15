/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
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

#include "TextBoundaries.h"
#include "PlatformString.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <util/string.h>
#include <stdio.h>

// This is very primitive. When I'll have time I'll do the "proper" implementation based on
// http://www.unicode.org/reports/tr29/tr29-4.html
namespace WebCore
{

int findNextSentenceFromIndex(UChar const* buffer, int len, int position, bool forward)
{
    os::String str(reinterpret_cast<char const*>(buffer), len);
    notImplemented();
    return 0;
}

void findSentenceBoundary(UChar const* buffer, int len, int position, int* start, int* end)
{
    os::String str(reinterpret_cast<char const*>(buffer), len);
    notImplemented();
}

int findNextWordFromIndex(UChar const* buffer, int len, int position, bool forward)
{
    os::String str(reinterpret_cast<char const*>(buffer), len);
    notImplemented();
    return 0;
}

// FIXME: This does not work for unicode
void findWordBoundary(UChar const* buffer, int len, int position, int* start, int* end)
{
	String str( buffer, len );
	
	#if 0
	for( int i = 0; i < len; i++ )
		DEBUG( "%i", buffer[i] );
	DEBUG("\n" );
	#endif

    if (position > str.length()) {
        *start = 0;
        *end = 0;
        return;
    }
    os::String cString = str;
    
    //DEBUG("String %s %i %i\n", cString.c_str(), len, position );
  
    int currentPosition = position - 1;
    while (currentPosition >= 0 &&
           cString[currentPosition] > 32) {
        --currentPosition;
    }

    // currentPosition == 0 means the first char is not letter
    // currentPosition == -1 means we reached the beginning
    int startPos = (currentPosition < 0) ? 0 : ++currentPosition;
    
    currentPosition = position;
    if (cString[currentPosition] > 32) {
            while (cString[currentPosition] > 32 && currentPosition < cString.Length()) {
                ++currentPosition;
            }
    }

    *start = startPos;
    *end = currentPosition;
}


}

