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

#ifndef __SMTP_SMTP_TIMER_H_
#define __SMTP_SMTP_TIMER_H_ 1

#ifdef __cplusplus
extern "C"{
#endif

#include <libsmtp.h>

int __smtp_setup_sighandler( const struct smtp_session *session );
void __smtp_teardown_sighandler( const struct smtp_session *session );
void __smtp_start_timer( const struct smtp_session *session );
void __smtp_stop_timer( struct smtp_session *session );

#ifdef __cplusplus
}	/* extern "C"{ */
#endif

#endif	/* __SMTP_SMTP_TIMER_H_ */
