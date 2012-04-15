/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
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
#include "Pasteboard.h"

#include "DocumentFragment.h"
#include "Editor.h"
#include "markup.h"
#include "KURL.h"
#include "NotImplemented.h"

#include <util/message.h>
#include <util/clipboard.h>

namespace WebCore {

Pasteboard::Pasteboard()
{
}

Pasteboard* Pasteboard::generalPasteboard()
{
    static Pasteboard* pasteboard = new Pasteboard();
    return pasteboard;
}

void Pasteboard::writeSelection(Range* selectedRange, bool canSmartCopyOrDelete, Frame* frame)
{
	String text = frame->selectedText();
    text.replace('\\', frame->backslashAsCurrencySymbol());
    os::Clipboard cBoard;
	cBoard.Lock();
	cBoard.Clear();	
	os::Message* pcData = cBoard.GetData();
	pcData->AddString( "text/plain", os::String( text ) );
	cBoard.Commit();
	cBoard.Unlock();
}

bool Pasteboard::canSmartReplace()
{
    return false;
}

String Pasteboard::plainText(Frame* frame)
{
    os::Clipboard cBoard;
	cBoard.Lock();
	os::Message* pcData = cBoard.GetData();	
	os::String cString;
	pcData->FindString( "text/plain", &cString );
	cBoard.Unlock();
	return( cString );
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame* frame, PassRefPtr<Range> context,
                                                          bool allowPlainText, bool& chosePlainText)
{
    notImplemented();
    return 0;
}

void Pasteboard::writeURL(const KURL& url, const String&, Frame*)
{
    os::Clipboard cBoard;
	cBoard.Lock();
	cBoard.Clear();
	os::Message* pcData = cBoard.GetData();	
	pcData->AddString( "text/plain", url.string() );
	cBoard.Commit();
	cBoard.Unlock();
}

void Pasteboard::writeImage(Node*, const KURL&, const String&)
{
    notImplemented();
}

void Pasteboard::clear()
{
    os::Clipboard cBoard;
	cBoard.Lock();
	cBoard.Clear();
	cBoard.Commit();
	cBoard.Unlock();
}

}







