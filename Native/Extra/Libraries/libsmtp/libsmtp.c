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

#include <libsmtp.h>
#include <smtp_timer.h>
#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

static int __smtp_wait_for( const char *message_type,
			    struct smtp_session *session )
{
	/* Wait for lines of data from the server.  Return 0 if we match message_type
	   or -1 if the server responds with a different message */
	
	char garbage[4096];			/* A big 'ol scratch buffer to hold any data we need
					   	   to drain from the socket */
	unsigned int input_bytes = 0;
	long socket_flags;

	if( !(session->status & SMTP_CONNECTED) )
		return ENOTCONN;

	/* Clear the input buffer of any junk */
	memset( session->buffer, 0, SMTP_BUFFER_SIZE );

	/* Read data into the buffer until we match CRLF */
	__smtp_start_timer( session );
	do
	{
		read( session->sock_fd, session->buffer + input_bytes, 1 );
		if( *( session->buffer + input_bytes ) == '\n' &&
		    *( session->buffer + ( input_bytes - 1 ) ) == '\r' )
			break;
		++input_bytes;
	}
	while( input_bytes < SMTP_BUFFER_SIZE &&
	       !( session->status & SMTP_TIME ) );

	/* Check that we did not time out */
	if( session->status & SMTP_TIME )
		return ETIME;

	__smtp_stop_timer( session );

	/* Check to see what type of message we recieved */
	if( strncmp( session->buffer, message_type, 3 ) )
	{
		/* The message type recieved from the server did not match */
		return -1;
	}

	/* Got the message we wanted, but some server send multiple lines of data
	   (Especially 220's) so we'll need to drain the socket to ensure no stall
	   data is left lying around. */
   	socket_flags = fcntl( session->sock_fd, F_GETFL );
	fcntl( session->sock_fd, F_SETFL, socket_flags | O_NONBLOCK );
	input_bytes = read( session->sock_fd, garbage, 4096 );
	fcntl( session->sock_fd, F_SETFL, socket_flags );
	
	return 0;
}

struct smtp_session* smtp_create_session( const char *server,
					  unsigned int port,
					  time_t timeout,
					  unsigned int flags )
{
	struct smtp_session *session;

	session = (struct smtp_session*)malloc( sizeof( struct smtp_session ) );
	if( NULL == session )
	{
		errno = ENOMEM;
		return NULL;
	}

	/* Setup default values for this session */
	session->server = NULL;
	session->port = SMTP_DEFAULT_PORT;
	session->timeout = SMTP_DEFAULT_TIMEOUT;
	session->flags = flags;
	session->sock_fd = 0;
	session->status = SMTP_NEW;

	/* Create an input buffer */
	session->buffer = (char*)malloc( SMTP_BUFFER_SIZE + 1 );
	if( NULL == session->buffer )
	{
		free( session );
		return NULL;
	}
	/* Offset the start of the buffer by one byte.  This is because of the way
	  which __smtp_wait_for() performs the check for a CRLF pair; it first checks
	  for LF at the current position and then checks for the CR one byte *before*
	  the current position.  If we start using the buffer at the first byte then
	  this check would access the buffer at byte -1 */
	session->buffer += 1;

	/* If arguments have been specified overide the default values */
	if( server )
	{
		session->server = (char*)malloc( strlen( server ) );
		if( NULL == session->server )
		{
			free( session->buffer );
			free( session );
			return NULL;
		}
		memcpy( session->server, server, strlen( server ) );
	}
	if( port > 0 )
		session->port = port;
	if( timeout > 0 )
		session->timeout = timeout;

	return session;
}

int smtp_destroy_session( struct smtp_session *session )
{
	if( NULL == session )
		return EINVAL;

	/* Do not destroy this session if we are currently connected as this would be
	   fatal.  smtp_disconnect() must be called before smtp_destroy_session() */
	if( session->status & SMTP_CONNECTED )
		return EINVAL;

	/* Free the input buffer & session data */
	if( session->buffer )
		free( session->buffer - 1 );	/* See smtp_create_session() for why we
						   offset the buffer */

	if( session->server )
		free( session->server );

	free( session );
	return 0;
}

int smtp_connect( struct smtp_session *session )
{
	int ret = 0;
	char localhost_name[255];	/* Hostname of this (client) machine */
	char helo_message[383];		/* Buffer to construct our HELO message with */

	if( NULL == session )
		return EINVAL;

	/* Do not try to connect again if the session is already in use. */
	if( session->status & SMTP_CONNECTED )
		return EINVAL;

	/* Lookup the server */
	session->host = gethostbyname( session->server );
	if( NULL == session->host )
		return EHOSTUNREACH;

	/* Create a socket */
	session->sock_fd = socket( PF_INET, SOCK_STREAM, 0 );
	if( session->sock_fd < 0 )
		return EHOSTUNREACH;
	session->addr.sin_family = AF_INET;
	session->addr.sin_port = htons( session->port );
	memcpy( &session->addr.sin_addr, session->host->h_addr_list[0], session->host->h_length );
	memset( &session->addr.sin_zero, 0, 8 );

	/* Initialise timeout handling */
	ret = __smtp_setup_sighandler( session );
	if( ret )
		return ret;

	/* Attempt to connect to the server */
	__smtp_start_timer( session );
	ret = connect( session->sock_fd, (struct sockaddr*)&session->addr, sizeof( struct sockaddr ) );
	if( ret )
		return ret;
	__smtp_stop_timer( session );

	/* This session as connected */
	session->status |= SMTP_CONNECTED;

	/* The server should now respond with a 220 message to indicate that it is ready */
	ret = __smtp_wait_for( SMTP_220, session );
	if( ret )
		return ECONNABORTED;

	/* Send our HELO */
	ret = gethostname( localhost_name, sizeof( localhost_name ) );
	if( ret )
		return ret;

	memset( helo_message, 0, 383 );
	strncpy( helo_message, "HELO ", 5 );
	strncat( helo_message, localhost_name, strlen( localhost_name ) );
	strncat( helo_message, "\r\n", 2 );

	ret = write( session->sock_fd, (void*)helo_message, strlen( helo_message ) );
	if( ret < strlen( helo_message ) )
		return ECOMM;

	/* Wait for the server to respond with 250 OK */
	ret = __smtp_wait_for( SMTP_250, session );
	if( ret )
		return ECONNABORTED;

	/* We are connected and the server is ready to accept messages */
	return 0;
}

int smtp_disconnect( struct smtp_session *session )
{
	char quit_message[] = "QUIT\r\n";
	int ret;

	if( NULL == session )
		return EINVAL;

	/* Send our QUIT */
	ret = write( session->sock_fd, (void*)quit_message, strlen( quit_message ) );
	/* We don't care if sending QUIT failed; either way we will be disconnected.  We will
	   still be polite & wait for the server to respond */

	ret = __smtp_wait_for( SMTP_221, session );
	/* Again, we don't mind if we failed */

	/* Clear any session flags */
	if( session->status & SMTP_CONNECTED )
		session->status ^= SMTP_CONNECTED;

	/* Close the socket */
	close( session->sock_fd );

	/* Clear up our signal handlers */
	__smtp_teardown_sighandler( session );

	return 0;
}

const char* smtp_get_server( const struct smtp_session *session )
{
	if( NULL == session )
	{
		errno = EINVAL;
		return NULL;
	}
	return session->server;
}

int smtp_set_server( struct smtp_session *session,
		     const char *server )
{
	if( NULL == session || NULL == server )
		return EINVAL;

	if( session->status & SMTP_CONNECTED )
		return EINVAL;

	if( session->server )
		free( session->server );

	session->server = (char*)malloc( strlen( server ) );
	if( NULL == session->server )
	{
		free( session->buffer );
		free( session );
		return ENOMEM;
	}
	memcpy( session->server, server, strlen( server ) );
	
	return 0;
}

int smtp_get_port( const struct smtp_session *session )
{
	if( NULL == session )
		return EINVAL;
	return session->port;
}

int smtp_set_port( struct smtp_session *session,
		   int port )
{
	if( NULL == session )
		return EINVAL;
	
	if( session->status & SMTP_CONNECTED )
		return EINVAL;

	session->port = port;
	return 0;
}

time_t smtp_get_timeout( const struct smtp_session *session )
{
	if( session == NULL )
	{
		errno = EINVAL;
		return 0;
	}
	return session->timeout;
}
int smtp_set_timeout( struct smtp_session *session,
			   time_t timeout )
{
	if( NULL == session )
		return EINVAL;
	session->timeout = timeout;
	return 0;
}

unsigned int smtp_get_flags( const struct smtp_session *session )
{
	if( NULL == session )
	{
		errno = EINVAL;
		return EINVAL;
	}
	return session->flags;
}

int smtp_set_flags( struct smtp_session *session,
			 unsigned int flags )
{
	if( NULL == session )
		return EINVAL;
	session->flags = flags;
	return 0;
}

struct smtp_message* smtp_create_message( const char *from,
					  const char *data )
{
	struct smtp_message *message;

	message = (struct smtp_message*)malloc( sizeof( struct smtp_message ) );
	if( NULL == message )
		return NULL;

	/* Copy the MAIL FROM: data */
	if( from )
	{
		message->from = malloc( strlen( from ) );
		if( NULL == message->from )
		{
			free( message );
			return NULL;
		}
		strncpy( message->from, from, strlen( from ) );
	}

	/* Copy the message body data */
	if( data )
	{
		message->data = malloc( strlen( data ) );
		if( NULL == message->data )
		{
			if( message->from )
				free( message->from );
			free( message );
			return NULL;
		}
		memcpy( message->data, data, strlen( data ) );
	}
	
	/* Initialise the recipiants list head */
	message->to = NULL;

	return message;
}

int smtp_add_recipiant( struct smtp_message *message,
			     const char *to )
{
	struct smtp_recipiant *recipiant, *list_item;

	/* Add a recipiant to the message */
	if( NULL == message || NULL == to )
		return EINVAL;

	/* Create a new recipiant */
	recipiant = (struct smtp_recipiant*)malloc( sizeof( struct smtp_recipiant ) );
	if( NULL == recipiant )
		return ENOMEM;
	
	recipiant->to = (char*)malloc( strlen( to ) );
	if( NULL == recipiant->to )
	{
		free( recipiant );
		return ENOMEM;
	}
	memcpy( recipiant->to, to, strlen( to ) );
	recipiant->next = NULL;

	/* Add this recipiant to the list for the message */
	if( NULL == message->to )
		message->to = recipiant;
	else
	{
		list_item = message->to;
		while( list_item->next != NULL )
			list_item = list_item->next;
		list_item->next = recipiant;
	}

	return 0;
}

int smtp_send( struct smtp_session *session,
		    const struct smtp_message *message )
{
	struct smtp_recipiant *list_item;
	char from_message[512];	/* MAIL FROM: message data */
	char to_message[512];	/* RCPT TO: message data */
	const char data_message[] = "DATA\r\n";
	const char end_message[] = "\r\n.\r\n";
	int ret;

	/* Send message to the list of recipiants already attached to it to the
	   server specified by session */

	if( NULL == session || NULL == message )
		return EINVAL;

	if( !(session->status & SMTP_CONNECTED) )
		return ECOMM;

	/* Send MAIL FROM: */
	memset( from_message, 0, 512 );
	strncpy( from_message, "MAIL FROM: ", 11 );
	strncat( from_message, message->from, strlen( message->from ) );
	strncat( from_message, "\r\n", 2 );

	ret = write( session->sock_fd, from_message, strlen( from_message ) );
	if( ret < strlen( from_message ) )
		return ret;

	/* Wait for the server to respond with 250 OK */
	ret = __smtp_wait_for( SMTP_250, session );
	if( ret < 0 )
	{
		if( session->status & SMTP_TIME )
			return ETIME;
		else
			return ECOMM;
	}

	/* Send a RCPT TO: for each recipiant */
	list_item = message->to;
	while( NULL != list_item )
	{
		if( NULL != list_item->to )
		{
			memset( to_message, 0, 512 );
			strncpy( to_message, "RCPT TO: ", 11 );
			strncat( to_message, list_item->to, strlen( list_item->to ) );
			strncat( to_message, "\r\n", 2 );

			ret = write( session->sock_fd, to_message, strlen( to_message ) );
			if( ret < strlen( to_message ) )
				continue;	/* Hope and keep trying */

			/* Wait for the server to respond with a 250 OK */
			ret = __smtp_wait_for( SMTP_250, session );
			if( ret < 0 )
			{
				if( session->status & SMTP_TIME )
					return ETIME;
				else
					return ECOMM;
			}
		}
		list_item = list_item->next;
	}

	/* Send the message body */
	ret = write( session->sock_fd, data_message, strlen( data_message ) );
	if( ret < strlen( data_message ) )
		return ret;

	ret = __smtp_wait_for( SMTP_354, session );
	if( ret < 0 )
	{
		if( session->status & SMTP_TIME )
			return ETIME;
		else
			return ECOMM;
	}

	/* Send the message data */
	ret = write( session->sock_fd, message->data, strlen( message->data ) );
	if( ret < strlen( message->data ) )
		return ret;

	/* End the message */
	ret = write( session->sock_fd, end_message, strlen( end_message ) );
	if( ret < strlen( end_message ) )
		return ret;

	/* Wait for 250 OK*/
	ret = __smtp_wait_for( SMTP_250, session );
	if( ret < 0 )
	{
		if( session->status & SMTP_TIME )
			return ETIME;
		else
			return ECOMM;
	}

	return 0;
}

int smtp_destroy_message( struct smtp_message *message )
{
	/* Destroy the message and all asscociated data */

	struct smtp_recipiant *list_item, *next;

	if( NULL == message )
		return EINVAL;

	/* Free the list of recipiants */
	list_item = message->to;
	while( list_item != NULL )
	{
		free( list_item->to );
		next = list_item->next;
		free( list_item );
		list_item = next;
	}
	
	/* Free the message body & from data */
	free( message->from );
	free( message->data );
	
	/* Free the message */
	free( message );
	
	return 0;
}
