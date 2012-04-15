/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
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

#include <stdio.h>
#include <stdlib.h>
#include "Node.h"
#include "Frame.h"
#include "Font.h"
#include "IntPoint.h"
#include "Widget.h"
#include "GraphicsContext.h"
#include "Cursor.h"
#include "loader.h"
#include "FrameView.h"
#include "KURL.h"
#include "ScrollBar.h"
#include "PlatformScrollBar.h"
#include "CachedResource.h"
#include "ScrollBar.h"
#include "Path.h"
#include "PlatformMouseEvent.h"
#include "CookieJar.h"
#include "Screen.h"
#include "History.h"
#include "Language.h"
#include "LocalizedStrings.h"
#include "PlugInInfoStore.h"
#include "RenderTheme.h"
#include "TextBoundaries.h"
#include "AXObjectCache.h"
#include "IconLoader.h"
#include "Threading.h"
#include "NotImplemented.h"

using namespace WebCore;


FloatRect Font::selectionRectForComplexText(const TextRun&, const IntPoint&, int, int, int) const { return FloatRect(); notImplemented(); }
int Font::offsetForPositionForComplexText(const TextRun&, int, bool) const { notImplemented(); return 0; }

namespace WebCore
{
bool historyContains(DeprecatedString const&) { return false; }
};

// LocalizedStrings
String WebCore::AXWebAreaText() { return String(); }
String WebCore::AXLinkText() { return String(); }
String WebCore::AXListMarkerText() { return String(); }
String WebCore::AXImageMapText() { return String(); }
String WebCore::AXHeadingText() { return String(); }

Vector<char> loadResourceIntoArray(const char*) { notImplemented(); return Vector<char>(); }

PluginInfo*PlugInInfoStore::createPluginInfoForPluginAtIndex(unsigned) { notImplemented(); return 0;}
unsigned PlugInInfoStore::pluginCount() const { notImplemented(); return 0; }
bool WebCore::PlugInInfoStore::supportsMIMEType(const WebCore::String&) { notImplemented(); return false; }
String PlugInInfoStore::pluginNameForMIMEType(const String& mimeType) { notImplemented(); return String(); }
void WebCore::refreshPlugins(bool) { notImplemented(); }

// cookies
void WebCore::setCookies(Document* document, const KURL& url, const KURL& policyURL, const String& value) { notImplemented(); }
String WebCore::cookies(const Document* document, const KURL& url) { notImplemented(); return String(); }
bool WebCore::cookiesEnabled(const Document* document) { notImplemented(); return false; }

namespace WebCore {
    
Vector<String> supportedKeySizes() { notImplemented(); return Vector<String>(); }
String signedPublicKeyAndChallengeString(unsigned keySizeIndex, const String &challengeString, const KURL &url) { return String(); }
    
float userIdleTime() { notImplemented(); return 0.0; }

bool historyContains(const UChar*, unsigned) { return false; }

void callOnMainThread(MainThreadFunction*, void* context) { notImplemented(); }
}

// vim: ts=4 sw=4 et

