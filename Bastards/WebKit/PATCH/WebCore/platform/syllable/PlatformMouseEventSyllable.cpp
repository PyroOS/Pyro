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
#include "PlatformMouseEvent.h"
#include "SystemTime.h"

#include <gui/guidefines.h>
#include <util/application.h>

namespace WebCore {


PlatformMouseEvent::PlatformMouseEvent(MouseEventType ev, float x, float y, float g_x, float g_y, uint32 buttons, int clickCount)
{
    m_position = IntPoint((int)x, (int)y);
    m_globalPosition = IntPoint((int)g_x, (int)g_y);
	m_timestamp = WebCore::currentTime();
	m_eventType = ev;

    if (buttons == 1)
        m_button = LeftButton;
    else if (buttons == 2)
        m_button = RightButton;
    else if (buttons == 3)
        m_button = MiddleButton;

    m_clickCount = clickCount;
    
    uint32 nQ = os::Application::GetInstance()->GetQualifiers();
    
    m_shiftKey =  (nQ & os::QUAL_SHIFT) != 0;
    m_ctrlKey = (nQ & os::QUAL_CTRL) != 0;
    m_altKey =  (nQ & os::QUAL_ALT) != 0;
    m_metaKey = false;
}


}

// vim: ts=4 sw=4 et











