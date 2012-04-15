/*
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
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

#include "SharedTimerSyllable.h"
#include "SyllableDebug.h"

#include <util/application.h>

namespace WebCore {

SharedTimerSyllable* SharedTimerSyllable::s_self = 0; // FIXME: staticdeleter

void setSharedTimerFiredFunction(void (*f)())
{
	SharedTimerSyllable::inst()->Lock();
    SharedTimerSyllable::inst()->m_timerFunction = f;
    SharedTimerSyllable::inst()->Unlock();
}

void setSharedTimerFireTime(double fireTime)
{
	DEBUG( "Set fire time %i %i\n", (int)fireTime, int(fireTime - currentTime() ) );
	os::Application* pcApp = os::Application::GetInstance();
	SharedTimerSyllable::inst()->Lock();
	SharedTimerSyllable::inst()->AddTimer( SharedTimerSyllable::inst(), 0, int( ( fireTime - currentTime() ) * 1000000 ) , true );
	/* FIXME: Fix this in libsyllable */
	SharedTimerSyllable::inst()->PostMessage( (uint32)0 );
	SharedTimerSyllable::inst()->Unlock();
    //SharedTimerQt::inst()->start(qMax(0, int(fireTime - currentTime())));
}

void stopSharedTimer()
{
	DEBUG("stopSharedTimer!\n" );
	os::Application* pcApp = os::Application::GetInstance();
	SharedTimerSyllable::inst()->Lock();
	SharedTimerSyllable::inst()->RemoveTimer( SharedTimerSyllable::inst(), 0 );
	SharedTimerSyllable::inst()->PostMessage( (uint32)0 );	
	SharedTimerSyllable::inst()->Unlock();	
}

}

// vim: ts=4 sw=4 et






























