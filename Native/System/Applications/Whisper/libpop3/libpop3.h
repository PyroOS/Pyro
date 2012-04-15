/* libpop3 0.5 -:-  (C)opyright 2001 - 2004 Kristian Van Der Vliet
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

#ifndef __POP3_LIBPOP3_H_
#define __POP3_LIBPOP3_H_ 1

#ifdef __cplusplus
extern "C"{
#endif

#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct pop3_session {
	char *server;				/* Server URI */
	unsigned int port;			/* Port number */
	time_t timeout;				/* Maximum time allowed for socket operations */
	unsigned int flags;			/* Various connection & control flags */
	int sock_fd;				/* fd for server connection */
	struct sockaddr_in addr;		/* Socket connection details */
	struct hostent *host;			/* Host details */
	char *buffer;				/* Input buffer */
	int status;				/* Current connection/error status */
	struct sigaction old_sigaction;	/* Data returned by sigaction(), restored by smtp_destroy_session() */
};

#define POP3_DEFAULT_PORT	110
#define POP3_DEFAULT_TIMEOUT	60	/* One minute */

#define POP3_BUFFER_SIZE	4096	/* 4k input buffer */

#define POP3_NEW		0x00	/* New session, never connected to the server */
#define POP3_CONNECTED		0x01	/* Currently connected */
#define POP3_NOT_CONNECTED	0x02	/* Not currently connected to a server */
#define POP3_TIME		0x04	/* Timeout */

struct pop3_session* pop3_create_session( const char *server,
					  unsigned int port,
					  time_t timeout,
					  unsigned int flags );
int pop3_destroy_session( struct pop3_session *session );
int pop3_connect( struct pop3_session *session,
		  const char *username,
		  const char *password );
int pop3_disconnect( struct pop3_session *session );

const char* pop3_get_server( const struct pop3_session *session );
int pop3_set_server( struct pop3_session *session,
		     const char *server );
int pop3_get_port( const struct pop3_session *session );
int pop3_set_port( struct pop3_session *session,
		   int port );
time_t pop3_get_timeout( const struct pop3_session *session );
int pop3_set_timeout( struct pop3_session *session,
			   time_t timeout );
unsigned int pop3_get_flags( const struct pop3_session *session );
int pop3_set_flags( struct pop3_session *session,
			 unsigned int flags );

int pop3_get_message_count( struct pop3_session *session,
			    int* message_count,
			    int* mailbox_size );
int pop3_get_message_size( struct pop3_session *session,
			   unsigned int message_id,
			   int *message_size );
int pop3_get_message( struct pop3_session *session,
			unsigned int message_id,
			void **data,
			size_t *size );
int pop3_delete_message( struct pop3_session *session,
			 unsigned int message_id );

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif	/* __POP3_LIBPOP3_H_ */
