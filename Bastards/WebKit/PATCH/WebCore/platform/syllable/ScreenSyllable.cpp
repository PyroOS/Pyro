/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 *           (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "Screen.h"

#include "Page.h"
#include "Frame.h"
#include "FrameView.h"
#include "Widget.h"
#include "IntRect.h"
#include "FloatRect.h"
#include "NotImplemented.h"

#include <gui/view.h>
#include <gui/desktop.h>
#include <gui/bitmap.h>
#include <appserver/protocol.h>
#include <util/application.h>
#include <util/messenger.h>

namespace WebCore {

static os::View* syllableWidgetForPage(const Page* page)
{
    Frame* frame = (page ? page->mainFrame() : 0);
    FrameView* frameView = (frame ? frame->view() : 0);

    if (!frameView)
        return 0;

    return frameView->syllableWidget();
}
    
FloatRect screenRect(Widget*)
{
	os::Desktop cDesktop;
	
	os::IPoint cSize = cDesktop.GetResolution();
	
	return( FloatRect( 0, 0, cSize.x, cSize.y ) );
}

FloatRect screenAvailableRect(Widget*)
{
	/* TODO: Add an API to libsyllable */
	os::Rect cViewFrame;
	os::Message cReq( os::DR_GET_DESKTOP_MAX_WINFRAME );
	os::Message cReply;
	cReq.AddInt32( "desktop", os::Desktop::ACTIVE_DESKTOP );
	os::Application* pcApp = os::Application::GetInstance();
	os::Messenger( pcApp->GetServerPort() ).SendMessage( &cReq, &cReply );
	cReply.FindRect( "frame", &cViewFrame );
	if( cViewFrame.IsValid() )
		return( cViewFrame );
	return( screenRect( NULL ) );
}

int screenDepth(Widget*)
{
	
	os::Desktop cDesktop;
	
	return( os::BitsPerPixel( cDesktop.GetColorSpace() ) );
}

int screenDepthPerComponent(Widget*)
{
	notImplemented();
	return( 8 );
}


bool screenIsMonochrome(Widget*)
{
	return( false );
}

}

// vim: ts=4 sw=4 et





