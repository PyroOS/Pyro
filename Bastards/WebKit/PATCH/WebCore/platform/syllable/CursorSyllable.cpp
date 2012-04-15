/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Charles Samuels <charles@kde.org>
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
#include "Cursor.h"

#include "DeprecatedString.h"
#include "NotImplemented.h"

#include <stdio.h>
#include <stdlib.h>

namespace WebCore {

Cursor::Cursor(PlatformCursor p)
    : m_impl(p)
{
}

Cursor::Cursor(const Cursor& other)
    : m_impl(other.m_impl)
{
}

Cursor::~Cursor()
{
}

Cursor::Cursor(Image*, const IntPoint&)
{
    notImplemented();
}

Cursor& Cursor::operator=(const Cursor& other)
{
    m_impl = other.m_impl;
    return *this;
}
#if 0
namespace {

// FIXME: static deleter
class Cursors {
protected:
    Cursors()
        : CrossCursor(QCursor(Qt::CrossCursor))
        , MoveCursor(QCursor(Qt::SizeAllCursor))
        , PointerCursor(QCursor(Qt::ArrowCursor))
        , PointingHandCursor(QCursor(Qt::PointingHandCursor))
        , IBeamCursor(QCursor(Qt::IBeamCursor))
        , WaitCursor(QCursor(Qt::WaitCursor))
        , WhatsThisCursor(QCursor(Qt::WhatsThisCursor))
        , SizeHorCursor(QCursor(Qt::SizeHorCursor))
        , SizeVerCursor(QCursor(Qt::SizeVerCursor))
        , SizeFDiagCursor(QCursor(Qt::SizeFDiagCursor))
        , SizeBDiagCursor(QCursor(Qt::SizeBDiagCursor))
        , SplitHCursor(QCursor(Qt::SplitHCursor))
        , SplitVCursor(QCursor(Qt::SplitVCursor))
    {
    }

    ~Cursors()
    {
    }

public:
    static Cursors* self();
    static Cursors* s_self;

    Cursor CrossCursor;
    Cursor MoveCursor;
    Cursor PointerCursor;
    Cursor PointingHandCursor;
    Cursor IBeamCursor;
    Cursor WaitCursor;
    Cursor WhatsThisCursor;
    Cursor SizeHorCursor;
    Cursor SizeVerCursor;
    Cursor SizeFDiagCursor;
    Cursor SizeBDiagCursor;
    Cursor SplitHCursor;
    Cursor SplitVCursor;
};

Cursors* Cursors::s_self = 0;

Cursors* Cursors::self()
{
    if (!s_self)
        s_self = new Cursors();

    return s_self;
}

}
#endif

static Cursor g_cCursor;

const Cursor& pointerCursor()
{
    //return Cursors::self()->PointerCursor;
    return g_cCursor;
}

const Cursor& moveCursor()
{
     return g_cCursor;
}

const Cursor& crossCursor()
{
    return g_cCursor;
}

const Cursor& handCursor()
{
    return g_cCursor;
}

const Cursor& iBeamCursor()
{
     return g_cCursor;
}

const Cursor& waitCursor()
{
    return g_cCursor;
}

const Cursor& helpCursor()
{
   return g_cCursor;
}

const Cursor& eastResizeCursor()
{
     return g_cCursor;
}

const Cursor& northResizeCursor()
{
     return g_cCursor;
}

const Cursor& northEastResizeCursor()
{
     return g_cCursor;
}

const Cursor& northWestResizeCursor()
{
     return g_cCursor;
}

const Cursor& southResizeCursor()
{
    return g_cCursor;
}

const Cursor& southEastResizeCursor()
{
    return g_cCursor;
}

const Cursor& southWestResizeCursor()
{
    return g_cCursor;
}

const Cursor& westResizeCursor()
{
     return g_cCursor;
}

const Cursor& northSouthResizeCursor()
{
     return g_cCursor;
}

const Cursor& eastWestResizeCursor()
{
   return g_cCursor;
}

const Cursor& northEastSouthWestResizeCursor()
{
     return g_cCursor;
}

const Cursor& northWestSouthEastResizeCursor()
{
   return g_cCursor;
}

const Cursor& columnResizeCursor()
{
     return g_cCursor;
}

const Cursor& rowResizeCursor()
{
   return g_cCursor;
}

const Cursor& verticalTextCursor()
{
   return g_cCursor;
}

const Cursor& cellCursor()
{
     return g_cCursor;
}

const Cursor& contextMenuCursor()
{
     return g_cCursor;
}

const Cursor& noDropCursor()
{
     return g_cCursor;
}

const Cursor& copyCursor()
{
     return g_cCursor;
}

const Cursor& progressCursor()
{
     return g_cCursor;
}

const Cursor& aliasCursor()
{
     return g_cCursor;
}

const Cursor& noneCursor()
{
     return g_cCursor;
}

const Cursor& notAllowedCursor()
{
     return g_cCursor;
}


const Cursor& zoomInCursor()
{
     return g_cCursor;
}

const Cursor& zoomOutCursor()
{
     return g_cCursor;
}

}

// vim: ts=4 sw=4 et

