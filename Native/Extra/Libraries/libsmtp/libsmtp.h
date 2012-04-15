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

#ifndef __SMTP_LIBSMTP_H_
#define __SMTP_LIBSMTP_H_ 1

#ifdef __cplusplus
extern "C"{
#endif

#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct smtp_session{
	char *server;				/* Server URI */
	unsigned int port;			/* Port number */
	time_t timeout;				/* Maximum time allowed for socket operations */
	unsigned int flags;			/* Various connection & control flags */
	int sock_fd;				/* fd for server connection */
	struct sockaddr_in addr;		/* Socket connection details */
	struct hostent *host;			/* Host details */
	char *buffer;				/* Input buffer */
	int status;				/* Current connection/error status */
	struct sigaction *old_sigaction;	/* Data returned by sigaction(), restored by smtp_destroy_session() */
};

#define SMTP_DEFAULT_PORT	25
#define SMTP_DEFAULT_TIMEOUT	60	/* One minute */

#define SMTP_BUFFER_SIZE	1024	/* 1k input buffer */

struct smtp_recipiant{
	char *to;			/* RCPT TO: recipiant information */
	struct smtp_recipiant *next;	/* Next recipiant */
};

struct smtp_message{
	char *from;			/* MAIL FROM: sender information */
	struct smtp_recipiant *to;	/* Linked list of RCPT TO: recipiants */
	void *data;			/* Message body data */
};

#define SMTP_NEW		0x00	/* New session, never connected to the server */
#define SMTP_CONNECTED		0x01	/* Currently connected */
#define SMTP_NOT_CONNECTED	0x02	/* Not currently connected to a server */
#define SMTP_ERROR		0x04	/* An error occured */
#define SMTP_TIME		0x08	/* Timeout */

#define	SMTP_220	"220"	/* Hello */
#define SMTP_221	"221"	/* Goodbye */
#define SMTP_250	"250"	/* O.K */
#define SMTP_354	"354"	/* Send data */
#define SMTP_451	"451"	/* Server error */
#define SMTP_501	"501"	/* Syntax error */
#define SMTP_551	"551"	/* Syntax error */

struct smtp_error{
	char *buffer;	/* Copy of the entire error message */
	int code;		/* The message code */
	char *message;	/* The error message, without the code */
};

struct smtp_session* smtp_create_session( const char *server,
					  unsigned int port,
					  time_t timeout,
					  unsigned int flags );
int smtp_destroy_session( struct smtp_session *session );
int smtp_connect( struct smtp_session *session );
int smtp_disconnect( struct smtp_session *session );

const char* smtp_get_server( const struct smtp_session *session );
int smtp_set_server( struct smtp_session *session,
		     const char *server );
int smtp_get_port( const struct smtp_session *session );
int smtp_set_port( struct smtp_session *session,
		   int port );
time_t smtp_get_timeout( const struct smtp_session *session );
int smtp_set_timeout( struct smtp_session *session,
			   time_t timeout );
unsigned int smtp_get_flags( const struct smtp_session *session );
int smtp_set_flags( struct smtp_session *session,
			 unsigned int flags );

struct smtp_message* smtp_create_message( const char *from,
					  const char *data );
int smtp_add_recipiant( struct smtp_message *message,
			     const char *to );
int smtp_send( struct smtp_session *session,
		    const struct smtp_message *message );
int smtp_destroy_message( struct smtp_message *message );

/* Helper functions provided by smtp_util.c */
int smtp_get_buffer( const struct smtp_session *session,
					 void **buffer );
int smtp_parse_error( const struct smtp_session *session,
					  struct smtp_error **error );
int smtp_free_error( struct smtp_error *error );

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif /* __SMTP_LIBSMTP_H_ */
