/* libsmtp 0.3 -:-  (C)opyright 2001 - 2004 Kristian Van Der Vliet
/
/  This library is free software; you can redistribute it and/or
/  modify it under the terms of the GNU Library General Public
/  License as published by the Free Software Foundation; either
/  version 2 of the License, or (at your option) any later version.
/
/  This library is distributed in the hope that it will be useful,
/  but WITHOUT ANY WARRANTY; without even the implied warranty of
/  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/  Library General Public License for more details.
/
/  You should have received a copy of the GNU Library General Public
/  License along with this library; if not, write to the Free
/  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
/  MA 02111-1307, USA
*/

#include <smtp_timer.h>
#include <sys/time.h>
#include <errno.h>

/* Internal functions, mostly used to deal with timeout signals */
static void __smtp_sig_handler( int signal, siginfo_t *info, void *data )
{
	/* Handle signals; only deal with SIGALRM */
	switch( info->si_signo )
	{
		case SIGALRM:
		{
			struct smtp_session *session = (struct smtp_session*)data;
			if( session )
				session->status |= SMTP_TIME;
		}
	}
}

int __smtp_setup_sighandler( const struct smtp_session *session )
{
	struct sigaction sig_action;

	/* Setup the signal handler to catch SIGALRM timeout signals */
	sig_action.sa_sigaction = __smtp_sig_handler;
	sig_action.sa_flags = SA_NOMASK | SA_SIGINFO;
	if( sigaction( SIGALRM, &sig_action, session->old_sigaction ) < 0 )
		return ECONNABORTED;
	return 0;
}

void __smtp_teardown_sighandler( const struct smtp_session *session )
{
	/* Reset the current signal handlers to their previous configuration */
	if( session && session->old_sigaction )
		sigaction( SIGALRM, session->old_sigaction, NULL );
}

void __smtp_start_timer( const struct smtp_session *session )
{
	/* Setup a timer to generate SIGALRM after the timeout period elapses */
	struct itimerval alarm;
	alarm.it_interval.tv_sec = session->timeout;
	setitimer( ITIMER_REAL, &alarm, NULL );
}

void __smtp_stop_timer( struct smtp_session *session )
{
	/* Clear a currently running timer */
	struct itimerval alarm;
	alarm.it_interval.tv_sec = 0;
	setitimer( ITIMER_REAL, &alarm, NULL );
	if( session->status & SMTP_TIME )
		session->status ^= SMTP_TIME;
}
