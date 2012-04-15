/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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
#include "HashTable.h"
#include "ClipboardSyllable.h"
#include "IntPoint.h"
#include "PlatformString.h"
#include "StringHash.h"
#include "NotImplemented.h"
#include <util/message.h>
#include <util/clipboard.h>

namespace WebCore {
    
ClipboardSyllable::ClipboardSyllable(ClipboardAccessPolicy policy, bool forDragging) 
    : Clipboard(policy, forDragging)
{
}

void ClipboardSyllable::clearData(const String& type)
{
	os::Clipboard cBoard;
	cBoard.Lock();
	os::Message* pcData = cBoard.GetData();
	pcData->RemoveName( os::String( type ).c_str() );
	cBoard.Commit();
	cBoard.Unlock();
}

void ClipboardSyllable::clearAllData() 
{
	os::Clipboard cBoard;
	cBoard.Lock();
	cBoard.Clear();
	cBoard.Commit();
	cBoard.Unlock();
}

String ClipboardSyllable::getData(const String& type, bool& success) const 
{
    os::Clipboard cBoard;
	cBoard.Lock();
	os::Message* pcData = cBoard.GetData();
	os::String cData;
	if( pcData->FindString( os::String( type ).c_str(), &cData ) == 0 )
		success = true;
	else
		success = false;
	cBoard.Unlock();
    return cData;
}

bool ClipboardSyllable::setData(const String& type, const String& data) 
{
	os::Clipboard cBoard;
	cBoard.Lock();
	os::Message* pcData = cBoard.GetData();
	pcData->RemoveName( os::String( type ).c_str() );
	pcData->AddString( os::String( type ).c_str(), data );
	cBoard.Commit();
	cBoard.Unlock();
    return true;
}

// extensions beyond IE's API
HashSet<String> ClipboardSyllable::types() const 
{
    notImplemented();
    HashSet<String> result;
    return result;
}

IntPoint ClipboardSyllable::dragLocation() const 
{ 
    notImplemented();
    return IntPoint(0,0);
}

CachedImage* ClipboardSyllable::dragImage() const 
{
    notImplemented();
    return 0; 
}

void ClipboardSyllable::setDragImage(CachedImage*, const IntPoint&) 
{
    notImplemented();
}

Node* ClipboardSyllable::dragImageElement() 
{
    notImplemented();
    return 0; 
}

void ClipboardSyllable::setDragImageElement(Node*, const IntPoint&)
{
    notImplemented();
}

DragImageRef ClipboardSyllable::createDragImage(IntPoint& dragLoc) const
{ 
    notImplemented();
    return 0;
}

void ClipboardSyllable::declareAndWriteDragImage(Element*, const KURL&, const String&, Frame*) 
{
    notImplemented();
}

void ClipboardSyllable::writeURL(const KURL&, const String&, Frame*) 
{
    notImplemented();
}

void ClipboardSyllable::writeRange(Range*, Frame*) 
{
    notImplemented();
}

bool ClipboardSyllable::hasData() 
{
	bool bHasData;
    os::Clipboard cBoard;
	cBoard.Lock();
	os::Message* pcData = cBoard.GetData();
	os::String cData;
	bHasData = !pcData->IsEmpty();
	cBoard.Unlock();
    return bHasData;
}

}






