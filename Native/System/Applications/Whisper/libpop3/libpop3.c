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

#include <libpop3.h>
#include <pop3_timer.h>
#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static int __pop3_wait_for_ok( struct pop3_session *session )
{
	/* Wait for lines of data from the server.  Return 0 if we match +OK
	   or -1 if the server responds with a different message */

	unsigned int input_bytes = 0;

	if( !(session->status & POP3_CONNECTED) )
		return ENOTCONN;

	/* Clear the input buffer of any junk */
	memset( session->buffer, 0, POP3_BUFFER_SIZE );

	/* Read data into the buffer until we match CRLF */
	__pop3_start_timer( session );
	do
	{
		read( session->sock_fd, session->buffer + input_bytes, 1 );
		if( *( session->buffer + input_bytes ) == '\n' &&
		    *( session->buffer + ( input_bytes - 1 ) ) == '\r' )
			break;
		++input_bytes;
	}
	while( input_bytes < POP3_BUFFER_SIZE &&
	       !( session->status & POP3_TIME ) );

	/* Check that we did not time out */
	if( session->status & POP3_TIME )
		return ETIME;

	__pop3_stop_timer( session );

	/* Check to see what type of message we recieved */
	if( strncmp( session->buffer, "+OK", 3 ) )
	{
		/* The message type recieved from the server did not match */
		return -1;
	}
	
	/* The server sent +OK */
	return 0;
}

struct pop3_session* pop3_create_session( const char *server,
					  unsigned int port,
					  time_t timeout,
					  unsigned int flags )
{
	struct pop3_session *session;

	session = (struct pop3_session*)malloc( sizeof( struct pop3_session ) );
	if( NULL == session )
		return NULL;

	/* Setup default values for this session */
	session->server = NULL;
	session->port = POP3_DEFAULT_PORT;
	session->timeout = POP3_DEFAULT_TIMEOUT;
	session->flags = flags;
	session->sock_fd = 0;
	session->status = POP3_NEW;

	/* Create an input buffer */
	session->buffer = calloc( 1, POP3_BUFFER_SIZE + 1 );
	if( NULL == session->buffer )
	{
		free( session );
		return NULL;
	}
	/* Offset the start of the buffer by one byte.  This is because of the way
	  which __pop3_wait_for() performs the check for a CRLF pair; it first checks
	  for LF at the current position and then checks for the CR one byte *before*
	  the current position.  If we start using the buffer at the first byte then
	  this check would access the buffer at byte -1 */
	session->buffer += 1;

	/* If arguments have been specified overide the default values */
	if( server )
	{
		session->server = calloc( 1, strlen( server ) + 1 );
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

int pop3_destroy_session( struct pop3_session *session )
{
	if( NULL == session )
		return EINVAL;

	/* Do not destroy this session if we are currently connected as this would be
	   fatal.  smtp_disconnect() must be called before pop3_destroy_session() */
	if( session->status & POP3_CONNECTED )
		return EINVAL;

	/* Free the input buffer & session data */
	if( session->buffer )
		free( session->buffer - 1 );	/* See pop3_create_session() for why we
						   offset the buffer */

	if( session->server )
		free( session->server );

	free( session );
	return 0;
}

int pop3_connect( struct pop3_session *session,
		  const char *username,
		  const char *password )
{
	int ret = 0;
	char user_message[512];		/* Buffer to construct our USER message with */
	char pass_message[512];		/* Buffer to construct our PASS message with */

	if( NULL == session || NULL == username || NULL == password )
		return EINVAL;

	/* Do not try to connect again if the session is already in use. */
	if( session->status & POP3_CONNECTED )
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
	ret = __pop3_setup_sighandler( session );
	if( ret )
		return ret;

	/* Attempt to connect to the server */
	__pop3_start_timer( session );
	ret = connect( session->sock_fd, (struct sockaddr*)&session->addr, sizeof( struct sockaddr ) );
	if( ret )
		return ret;
	__pop3_stop_timer( session );

	/* This session as connected */
	session->status |= POP3_CONNECTED;

	/* The server should now respond with +OK to indicate that it is ready */
	ret = __pop3_wait_for_ok( session );
	if( ret )
		return ECONNABORTED;

	/* Send the username */
	memset( user_message, 0, 512 );
	strncpy( user_message, "USER ", 5 );
	strncat( user_message, username, strlen( username ) );
	strncat( user_message, "\r\n", 2 );

	ret = write( session->sock_fd, (void*)user_message, strlen( user_message ) );
	if( ret < strlen( user_message ) )
		return ECOMM;

	/* Wait for the server to respond with +OK */
	ret = __pop3_wait_for_ok( session );
	if( ret )
		return ECONNABORTED;

	/* Send the password */
	memset( pass_message, 0, 512 );
	strncpy( pass_message, "PASS ", 5 );
	strncat( pass_message, password, strlen( password ) );
	strncat( pass_message, "\r\n", 2 );

	ret = write( session->sock_fd, (void*)pass_message, strlen( pass_message ) );
	if( ret < strlen( pass_message ) )
		return ECOMM;

	/* Wait for the server to respond with +OK */
	ret = __pop3_wait_for_ok( session );
	if( ret )
		return ECONNABORTED;

	/* We are connected and the server is ready to send messages */
	return 0;
}

int pop3_disconnect( struct pop3_session *session )
{
	char quit_message[] = "QUIT\r\n";
	int ret;

	if( NULL == session )
		return EINVAL;

	/* Send our QUIT */
	ret = write( session->sock_fd, (void*)quit_message, strlen( quit_message ) );
	/* We don't care if sending QUIT failed; either way we will be disconnected.  We will
	   still be polite & wait for the server to respond */

	ret = __pop3_wait_for_ok( session );
	/* Again, we don't mind if we failed */

	/* Clear any session flags */
	if( session->status & POP3_CONNECTED )
		session->status ^= POP3_CONNECTED;

	/* Close the socket */
	close( session->sock_fd );

	/* Clear up our signal handlers */
	__pop3_teardown_sighandler( session );

	return 0;
}

const char* pop3_get_server( const struct pop3_session *session )
{
	if( NULL == session )
	{
		errno = EINVAL;
		return NULL;
	}
	return session->server;
}

int pop3_set_server( struct pop3_session *session,
		     const char *server )
{
	if( NULL == session || NULL == server )
		return EINVAL;

	if( session->status & POP3_CONNECTED )
		return EINVAL;

	if( session->server )
		free( session->server );

	session->server = (char*)malloc( strlen( server ) );
	if( NULL == session->server )
		return ENOMEM;

	memcpy( session->server, server, strlen( server ) );

	return 0;
}

int pop3_get_port( const struct pop3_session *session )
{
	if( NULL == session )
		return EINVAL;
	return session->port;
}

int pop3_set_port( struct pop3_session *session,
		   int port )
{
	if( NULL == session )
		return EINVAL;

	if( session->status & POP3_CONNECTED )
		return EINVAL;

	session->port = port;
	return 0;
}

time_t pop3_get_timeout( const struct pop3_session *session )
{
	if( session == NULL )
	{
		errno = EINVAL;
		return 0;
	}
	return session->timeout;
}

int pop3_set_timeout( struct pop3_session *session,
			   time_t timeout )
{
	if( NULL == session )
		return EINVAL;
	session->timeout = timeout;
	return 0;
}

unsigned int pop3_get_flags( const struct pop3_session *session )
{
	if( NULL == session )
	{
		errno = EINVAL;
		return EINVAL;
	}
	return session->flags;
}

int pop3_set_flags( struct pop3_session *session,
			 unsigned int flags )
{
	if( NULL == session )
		return EINVAL;
	session->flags = flags;
	return 0;
}

int pop3_get_message_count( struct pop3_session *session,
			    int* message_count,
			    int* mailbox_size )
{
	/* Get the number of messages currently waiting and the total
	   mailbox size and store the data in message_count and mailbox_size */

	const char stat_message[] = "STAT\r\n";
	char response[512], response_count[256], response_size[256];	/* Somewhere to copy the response */
	int count, size;						/* Message count & total mailbox size */
	int ret, whitespace = 0;

	if( NULL == session || NULL == message_count || NULL == mailbox_size )
		return EINVAL;

	if( !(session->status & POP3_CONNECTED) )
		return ECOMM;

	/* Send the STAT */
	ret = write( session->sock_fd, stat_message, strlen( stat_message ) );
	if( ret < strlen( stat_message ) )
		return ret;

	/* Wait for the server to respond with +OK */
	ret = __pop3_wait_for_ok( session );
	if( ret < 0 )
	{
		if( session->status & POP3_TIME )
			return ETIME;
		else
			return ECOMM;
	}

	/* The response takes the form of +OK followed by the number of messages followed
	   by the total number of bytes.  Parse the buffer to find out how many messages
	   are waiting.

	   Copy the response from the buffer past starting after the +OK */
	strcpy( response, session->buffer + 4 );

	/* Find the whitespace */
	while( *(response + whitespace) != ' ' )
		++whitespace;

	/* Get the message count */
	strncpy( response_count, response, whitespace );
	count = atoi( response_count );

	/* Get the total mailbox size */
	strcpy( response_size, response + whitespace + 1 );
	size = atoi( response_size );

	/* Store the values */
	*message_count = count;
	*mailbox_size = size;

	return 0;
}

int pop3_get_message_size( struct pop3_session *session,
			   unsigned int message_id,
			   int *message_size )
{
	/* Get the size of the message and store the result in message_size */
	char list_message[256];				/* Buffer to construct our LIST message with */
	char id[256];					/* ASCIIzed Message ID */
	char response[512], response_size[256];		/* Somewhere to copy the response */
	int size;					/* Message size */
	int ret, whitespace = 0;

	if( NULL == session || NULL == message_size )
		return EINVAL;

	if( !(session->status & POP3_CONNECTED) )
		return ECOMM;

	/* Send the LIST */
	sprintf(id, "%i", message_id );
	memset( list_message, 0, 256 );
	strncpy( list_message, "LIST ", 5 );
	strncat( list_message, id, strlen( id ) );
	strncat( list_message, "\r\n", 2 );

	ret = write( session->sock_fd, list_message, strlen( list_message ) );
	if( ret < strlen( list_message ) )
		return ECOMM;

	/* Wait for the server to respond with +OK */
	ret = __pop3_wait_for_ok( session );
	if( ret < 0 )
	{
		if( session->status & POP3_TIME )
			return ETIME;
		else
			return ECOMM;
	}

	/* The response takes the form of +OK followed by the message ID followed
	   by the total number of bytes.  Parse the buffer to find out the message
	   size.  The message ID is discarded.

	   Copy the response from the buffer past starting after the +OK */
	strcpy( response, session->buffer + 4 );

	/* Find the whitespace */
	while( *(response + whitespace) != ' ' )
		++whitespace;

	/* Get the message size */
	strcpy( response_size, response + whitespace + 1 );
	size = atoi( response_size );

	/* Store the values */
	*message_size = size;

	return 0;
}

int pop3_get_message( struct pop3_session *session,
			unsigned int message_id,
			void **data,
			size_t *size )
{
	/* Get the message data from the server.  Dump the data into a buffer
	   and point *data at it. */

	int rsize = 0, msize = 0;	/* Size of the message in bytes */
	int bytes = 0;				/* bytes returned by read() */
	int bytes_total = 0;		/* Totoal bytes returned so far*/
   	char retr_message[256];		/* Buffer to construct our RETR message with */
	char id[256];				/* ASCIIzed Message ID */
	char *message = NULL;		/* Message data from RETR */
	int ret;					/* Return code from other functions */
	int err;					/* Error code returned from this function */

	if( NULL == session || NULL == *data )
	{
		err = EINVAL;
		goto error;
	}

	/* Get the reported message size */
	ret = pop3_get_message_size( session, message_id, &rsize );
	if( ret )
	{
		err = ret;
		goto error;
	}
	/* The size reported by the server can not always be relied upon to be acurate.  The
	   loop below has a specific check for the end of the message, and we'll allocate an
	   extra 1k here; although we can reallocate if the message sent is too big for the
	   buffer, we can at least avoid the call to realloc() for minor cases of less than
	   1k this way */
	msize = rsize + 1024;

	/* Pull down the message data with RETR. */
	memset( id, 0, 256 );
	sprintf(id, "%i", message_id );
	memset( retr_message, 0, 256 );
	strncpy( retr_message, "RETR ", 5 );
	strncat( retr_message, id, strlen( id ) );
	strncat( retr_message, "\r\n", 2 );

	ret = write( session->sock_fd, retr_message, strlen( retr_message ) );
	if( ret < strlen( retr_message ) )
	{
		err = ECOMM;
		goto error;
	}

	/* Wait for the server to respond with +OK */
	ret = __pop3_wait_for_ok( session );
	if( ret < 0 )
	{
		if( session->status & POP3_TIME )
		{
			err = ETIME;
			goto error;
		}
		else
		{
			err = ECOMM;
			goto error;
		}
	}

	/* Create a buffer to hold the data. */
	message = calloc( 1, msize );
	if( NULL == message )
	{
		err = ENOMEM;
		goto error;
	}

	/* Read the rest of the data from the server. */
	do
	{
		char *end;

		__pop3_start_timer( session );
		bytes = read( session->sock_fd, message + bytes_total, msize - bytes_total );
		bytes_total += bytes;

		/* Check that we did not time out */
		if( session->status & POP3_TIME )
		{
			err = ETIME;
			goto error;
		}

		__pop3_stop_timer( session );

		/* See if the message has been sent */
		end = message + ( bytes_total - 5 );
		if( strncmp( end, "\r\n.\r\n", 5 ) == 0 )
			break;

		/* If we reach this point the reported message size was too small, so we need
		   to increase the buffer by another 1k and read again */
		if( bytes_total >= msize )
		{
			msize += 1024;
			message = realloc( message, msize );
		}
	}
	/* Did we get the whole message? */
	while( bytes_total < msize );

	/* Store the message */
	*data = message;
	*size = bytes_total;

	return 0;

error:
	if( NULL != message )
		free( message );
	*data = NULL;
	*size = 0;
	return err;
}

int pop3_delete_message( struct pop3_session *session,
			 unsigned int message_id )
{
	/* Delete a message from the server */
	char dele_message[256];				/* Buffer to construct our DELE message with */
	char id[256];					/* ASCIIzed Message ID */
	int ret;

	if( NULL == session )
		return EINVAL;

	if( !(session->status & POP3_CONNECTED) )
		return ECOMM;

	/* Send the DELE */
	sprintf(id, "%i", message_id );
	memset( dele_message, 0, 256 );
	strncpy( dele_message, "DELE ", 5 );
	strncat( dele_message, id, strlen( id ) );
	strncat( dele_message, "\r\n", 2 );

	ret = write( session->sock_fd, dele_message, strlen( dele_message ) );
	if( ret < strlen( dele_message ) )
		return ret;

	/* Wait for the server to respond with +OK */
	ret = __pop3_wait_for_ok( session );
	if( ret < 0 )
	{
		if( session->status & POP3_TIME )
			return ETIME;
		else
			return ECOMM;
	}

	return 0;
}

