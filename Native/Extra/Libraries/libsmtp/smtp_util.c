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
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

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

