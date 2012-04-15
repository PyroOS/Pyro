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
 "@(#) $Id: SDL_dspaudio.h,v 1.1 2008/12/11 03:07:10 kajdevos Exp $";
#endif

#ifndef _SDL_dspaudio_h
#define _SDL_dspaudio_h

#include "../SDL_sysaudio.h"
#include <media/format.h>
#include <media/packet.h>
#include <media/output.h>
#include <media/manager.h>

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_AudioDevice *_this

struct SDL_PrivateAudioData {
	/* The file descriptor for the audio device */
	//int audio_fd;
	os::MediaManager* manager;
	os::MediaOutput* output;

	/* The parent process id, to detect when application quits */
	pid_t parent;

	/* Raw mixing buffer */
	Uint8 *mixbuf;
	int    mixlen;
};
#define FUDGE_TICKS	10	/* The scheduler overhead ticks per frame */

/* Old variable names */
//#define audio_fd		(_this->hidden->audio_fd)
#define manager			(_this->hidden->manager)
#define output			(_this->hidden->output)
#define parent			(_this->hidden->parent)
#define mixbuf			(_this->hidden->mixbuf)
#define mixlen			(_this->hidden->mixlen)
#define frame_ticks		(_this->hidden->frame_ticks)
#define next_frame		(_this->hidden->next_frame)

#endif /* _SDL_dspaudio_h */
