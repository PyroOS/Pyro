/* libsmtp 0.4 -:-  (C)opyright 2001 - 2007 Kristian Van Der Vliet
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
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>

int __smtp_wait_for( const char *message_type,
			    struct smtp_session *session )
{
	/* Wait for lines of data from the server.  Return 0 if we match message_type
	   or -1 if the server responds with a different message */

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
	   (Especially 220's) so we'll need to drain the socket to ensure no stale
	   data is left lying around. */
   	socket_flags = fcntl( session->sock_fd, F_GETFL );
	fcntl( session->sock_fd, F_SETFL, socket_flags | O_NONBLOCK );
	input_bytes = read( session->sock_fd, session->buffer + input_bytes, ( SMTP_BUFFER_SIZE - input_bytes ) );
	fcntl( session->sock_fd, F_SETFL, socket_flags );
	
	return 0;
}

int smtp_get_buffer( const struct smtp_session *session,
					 void **buffer )
{
	/* Copy the session buffer into buffer */
	char *session_buffer;

	if( NULL == session || NULL == *buffer )
		return EINVAL;

	session_buffer = (char*)malloc( SMTP_BUFFER_SIZE );
	if( NULL == session_buffer )
		return ENOMEM;

	memcpy( session_buffer, session->buffer, SMTP_BUFFER_SIZE );
	*buffer = session_buffer;

	return 0;
}

int smtp_parse_error( const struct smtp_session *session,
					  struct smtp_error **err )
{
	/* Parse the raw session buffer into err */
	struct smtp_error *error;
	char code[3];					/* First three bytes of the buffer */
	int ret;

	if( NULL == session || NULL == *err )
		return EINVAL;

	error = (struct smtp_error*)malloc( sizeof( struct smtp_error ) );
	if( NULL == error )
		return ENOMEM;

	/* Copy the entire session buffer */
	ret = smtp_get_buffer( session, (void*)&error->buffer );
	if( ret )
		return ret;

	/* Get the error code */
	strncpy( code, error->buffer, 3 );
	error->code = atoi( code );

	/* Place the error message, without the leading error code, into message.
	   The data exists in buffer so point it back into that field */
	error->message = error->buffer + 4;

	*err = error;
	return 0;
}

int smtp_free_error( struct smtp_error *error )
{
	if( NULL == error )
		return EINVAL;

	if( error->buffer )
		free( error->buffer );
	free( error );

	return 0;
}

