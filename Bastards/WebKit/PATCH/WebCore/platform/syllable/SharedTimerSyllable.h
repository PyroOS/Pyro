/*
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
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

#ifndef SharedTimerSyllable_H
#define SharedTimerSyllable_H

#include "SystemTime.h"
#include "SyllableDebug.h"

#include <util/handler.h>
#include <util/looper.h>
#include <util/application.h>

extern os::Locker g_cGlobalMutex;

namespace WebCore {

class SharedTimerSyllable : public os::Looper {
protected:
    SharedTimerSyllable()
        : os::Looper( "webcore_timer" )
        , m_timerFunction(0)
    {
    	
    	g_cGlobalMutex.Lock();
    	SetMutex( &g_cGlobalMutex );
    	DEBUG("SharedTimerSyllable\n" );
    }

    ~SharedTimerSyllable()
    {
    }

public:
	void TimerTick( int nID )
	{
		DEBUG("Timer fire!\n" );
		if( m_timerFunction )
			m_timerFunction();
	}
    static SharedTimerSyllable* inst()
    {
        if (!s_self)
        {
            s_self = new SharedTimerSyllable();
            s_self->Run();
            DEBUG("Timer running!\n");
        }
        return s_self;
    }

    void (*m_timerFunction)();


private:
    static SharedTimerSyllable* s_self;
};

}

#endif

// vim: ts=4 sw=4 et





























