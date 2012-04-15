/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
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

#ifndef SCROLLVIEWCANVASQT_H
#define SCROLLVIEWCANVASQT_H

#include <gui/view.h>

namespace WebCore {

class ScrollView;

class ScrollViewCanvasSyllable : public os::View
{
public:
    ScrollViewCanvasSyllable(ScrollView*, os::View* parent, uint32 nFlags);
	void SetScrollBarMaxV( float vV )
	{
		m_vScrollBarMaxV = vV;
	}
	void SetScrollBarMaxH( float vH )
	{
		m_vScrollBarMaxH = vH;
	}
protected:
	void Paint( const os::Rect& cUpdate );
	void MouseMove( const os::Point& cNewPos, int nCode, uint32 nButtons, os::Message* pcData );
    void MouseDown( const os::Point& cPosition, uint32 nButtons );
    void MouseUp( const os::Point& cPosition, uint32 nButtons, os::Message* pcData );
    void KeyDown( const char* pzString, const char* pzRawString, uint32 nQualifiers );
    void KeyUp( const char* pzString, const char* pzRawString, uint32 nQualifiers );
	void WheelMoved( const os::Point& cDelta );
	void Activated( bool bActivate );
	
	
private:
	void DoScroll(bool isHorizontal, float multiplier);
    ScrollView* m_frameView;
    float m_vScrollBarMaxH;
    float m_vScrollBarMaxV;
};

}

#endif

// vim: ts=4 sw=4 et





























