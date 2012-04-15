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
 "@(#) $Id: SDL_systhread.c,v 1.1 2008/12/11 03:07:10 kajdevos Exp $";
#endif

/* Syllable thread management routines for SDL */

#include <stdio.h>
#include <signal.h>
#include <pyro/kernel.h>
#include <pyro/types.h>

#include "../../../include/SDL_error.h"
#include "../../../include/SDL_mutex.h"
#include "../../../include/SDL_thread.h"
#include "../SDL_thread_c.h"
#include "../SDL_systhread.h"


static int sig_list[] = {
	SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGALRM, SIGTERM, SIGWINCH, 0
};

void SDL_MaskSignals(sigset_t *omask)
{
	sigset_t mask;
	int i;

	sigemptyset(&mask);
	for ( i=0; sig_list[i]; ++i ) {
		sigaddset(&mask, sig_list[i]);
	}
	sigprocmask(SIG_BLOCK, &mask, omask);
}
void SDL_UnmaskSignals(sigset_t *omask)
{
	sigprocmask(SIG_SETMASK, omask, NULL);
}

static int32 RunThread(void *data)
{
	SDL_RunThread(data);
	return(0);
}

int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	/* Create the thread and go! */
	thread->handle=spawn_thread("SDL", RunThread, NORMAL_PRIORITY, 0, args);
	if ( (thread->handle < 0 ) ) {
		SDL_SetError("Not enough resources to create thread");
		return(-1);
	}
	resume_thread(thread->handle);
	return(0);
}

void SDL_SYS_SetupThread(void)
{
	/* Mask asynchronous signals for this thread */
	SDL_MaskSignals(NULL);
}

Uint32 SDL_ThreadID(void)
{
	return((Uint32)get_thread_id(NULL));
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	status_t the_status;

	the_status = wait_for_thread(thread->handle);
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{
	kill(thread->handle, SIGKILL);
}
