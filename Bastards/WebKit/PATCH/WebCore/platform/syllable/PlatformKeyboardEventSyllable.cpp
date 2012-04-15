/*
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
#include "KeyboardCodes.h"
#include "PlatformKeyboardEvent.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/guidefines.h>
#include <util/string.h>

namespace WebCore {


static String keyIdentifierForSyllableKeyCode(os::String cRawString, os::String cString, uint32 nRawKey)
{
	if( cRawString[0] == os::VK_FUNCTION_KEY )
	{
		char zTemp[4];
		sprintf( zTemp, "F%i", nRawKey - 1 );
		return( zTemp );
	}
	switch( cRawString[0] )
	{
        case os::VK_DOWN_ARROW:
            return "Down";
        case os::VK_END:
            return "End";
        case os::VK_ENTER:
            return "Enter";
        case os::VK_HOME:
            return "Home";
        case os::VK_INSERT:
            return "Insert";
        case os::VK_LEFT_ARROW:
            return "Left";
        case os::VK_PAGE_DOWN:
            return "PageDown";
        case os::VK_PAGE_UP:
            return "PageUp";
        case os::VK_RIGHT_ARROW:
            return "Right";
        case os::VK_UP_ARROW:
            return "Up";
            // Standard says that DEL becomes U+00007F.
		case os::VK_DELETE:
            return "U+00007F";
        default:
            return String::format("U+%06X", toupper(cString[0]));
    }
}

static int windowsKeyCodeForKeyEvent(os::String cRawString, os::String cString)
{
	if( cRawString[0] >= 'a' && cRawString[0] <= 'z' )
		return( VK_A + (int)cRawString[0] - 'a' );

	if( cRawString[0] >= '0' && cRawString[0] <= '9' )
		return( VK_0 + (int)cRawString[0] - '0' );

	
    switch (cRawString[0]) {
        /* FIXME: Need to supply a bool in this func, to determine wheter the event comes from the keypad
		*/
        case os::VK_BACKSPACE:
            return VK_BACK; // (08) BACKSPACE key
        case os::VK_TAB:
            return VK_TAB; // (09) TAB key
        case os::VK_RETURN:
            return VK_RETURN; //(0D) Return key
        case os::VK_ESCAPE:
            return VK_ESCAPE; // (1B) ESC key
            // VK_CONVERT (1C) IME convert
            // VK_NONCONVERT (1D) IME nonconvert
            // VK_ACCEPT (1E) IME accept
            // VK_MODECHANGE (1F) IME mode change request
        case os::VK_SPACE:
            return VK_SPACE; // (20) SPACEBAR
        case os::VK_PAGE_UP:
            return VK_PRIOR; // (21) PAGE UP key
        case os::VK_PAGE_DOWN:
            return VK_NEXT; // (22) PAGE DOWN key
        case os::VK_END:
            return VK_END; // (23) END key
        case os::VK_HOME:
            return VK_HOME; // (24) HOME key
        case os::VK_LEFT_ARROW:
            return VK_LEFT; // (25) LEFT ARROW key
        case os::VK_UP_ARROW:
            return VK_UP; // (26) UP ARROW key
        case os::VK_RIGHT_ARROW:
            return VK_RIGHT; // (27) RIGHT ARROW key
        case os::VK_DOWN_ARROW:
            return VK_DOWN; // (28) DOWN ARROW key
        case os::VK_INSERT:
            return VK_INSERT; // (2D) INS key
        case os::VK_DELETE:
            return VK_DELETE; // (2E) DEL key
        case 0x2e:
        	return( 0 );
        default:
        	return( 0 );
    }

}


PlatformKeyboardEvent::PlatformKeyboardEvent(os::String cString, os::String cRawString, uint32 nKeyCode, uint32 nQ, bool isKeyUp)
{
	DEBUG("PlatformKeyboardEvent::PlatformKeyboardEvent\n");
	DEBUG( "%i %i %i %i\n", (int)cString[0], (int)cString[1], (int)cRawString[0], nKeyCode );
    m_text = cString;
    
    m_unmodifiedText = cRawString;
    m_keyIdentifier = keyIdentifierForSyllableKeyCode(cRawString, cString, nKeyCode);
    DEBUG( "%i %i %i %s\n", m_text[0], m_text[1], m_text.length(), os::String( m_keyIdentifier ).c_str() );
    if( isKeyUp )
      m_type = KeyUp;
    else
      m_type = KeyDown;
    m_autoRepeat = ( nQ & os::QUAL_REPEAT ) != 0;
    m_isKeypad = false;
    m_windowsVirtualKeyCode = windowsKeyCodeForKeyEvent(cRawString, cString);
    m_shiftKey = (nQ & os::QUAL_SHIFT) != 0;
    m_ctrlKey = (nQ & os::QUAL_CTRL) != 0;
    m_altKey = (nQ & os::QUAL_ALT) != 0;
    m_metaKey = false;
//    m_metaKey = (event->modifiers() & Qt::MetaModifier) != 0;    
}

void PlatformKeyboardEvent::disambiguateKeyDownEvent(Type type, bool)
{
    // Can only change type from KeyDown to RawKeyDown or Char, as we lack information for other conversions.
    ASSERT(m_type == KeyDown);
    m_type = type;

    if (type == RawKeyDown) {
        m_text = String();
        m_unmodifiedText = String();
    } else {
        m_keyIdentifier = String();
        m_windowsVirtualKeyCode = 0;
    }
}

bool PlatformKeyboardEvent::currentCapsLockState()
{
    notImplemented();
    return false;
}

}

// vim: ts=4 sw=4 et




























