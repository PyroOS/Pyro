/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id: SDL_sysevents.cc,v 1.1 2008/12/11 03:07:10 kajdevos Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_Win.h"
#include "SDL_lowvideo.h"

extern "C" {

#include "../../events/SDL_events_c.h"
#include "../../events/SDL_sysevents.h"
#include "SDL_sysevents_c.h"

void SYL_PumpEvents(_THIS)
{
}

void SYL_InitOSKeymap(_THIS)
{
}

}; /* Extern C */
