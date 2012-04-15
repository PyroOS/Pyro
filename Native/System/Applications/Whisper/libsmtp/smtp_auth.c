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
#include <base64.h>
#include <MD5.h>
#include <errno.h>
#include <error.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>

static int __smtp_auth_cram_md5( struct smtp_session *session, const char *username, const char *password )
{
	int ret = 0;
	char challenge64[512], response[512], padded_passwd[64];
	size_t len;
	char *challenge, *digest, *response64;
	const char *p;
	char opad[65] = {0x5c}, ipad[65] = {0x36};
	int i;

	debug( "Using AUTH CRAM-MD5\n" );

	/* Pad the password to 64 bytes */
	if( strlen( password ) > 64 )
		return EINVAL;
	memset( padded_passwd, '\0', 64 );
	strcpy( padded_passwd, password );

	/* Send the auth command */
	ret = write( session->sock_fd, (void*)"AUTH CRAM-MD5\r\n", 15 );
	if( ret < 15 )
		return ret;

	ret = __smtp_wait_for( SMTP_334, session );
	if( ret < 0 )
		return ret;

	/* Copy the challenge, minus the response code & CRLF */
	memset( challenge64, '\0', 512 );
	strncpy( challenge64, ( session->buffer + 4 ), ( strlen( session->buffer ) - 6 ) );
	len = base64_decode( challenge64, strlen( challenge64 ), &challenge );

	debug( "challenge64: %s, challenge: %s\n", challenge64, challenge );

	/* XXXKV: Generate the digest with MD5( ( password ^ opad ), MD5( ( password ^ ipad ), challenge ) ) */
	//char * crypt_md5(const char *pw, const char *salt);

	p = password;
	for( i = 0; i < 64 && *p != '\0'; i++ )
		opad[i] ^= *p++;
	p = password;
	for( i = 0; i < 64 && *p != '\0'; i++ )
		ipad[i] ^= *p++;
	opad[64] = ipad[64] = '\0';

	digest = crypt_md5( opad, crypt_md5( ipad, challenge ) );
	free(  challenge );

	debug( "digest=%s\n", digest );

	/* Build and send response */
	memset( response, '\0', 512 );
	sprintf( response, "%s %s", username, digest );
	len = base64_encode( &response64, response, strlen( response ) );

	debug( "response: %s, response64: %s\n", response, response64 );

	ret = write( session->sock_fd, (void*)response64, len );
	free( response64 );
	if( ret < len )
		return ret;

	ret = write( session->sock_fd, (void*)"\r\n", 2 );
	if( ret < 2 )
		return ret;

	ret = __smtp_wait_for( SMTP_235, session );
	if( ret < 0 )
		return ret;

	debug( "Authentication succeeded.\n%s\n", session->buffer );

	return 0;
}

static int __smtp_auth_plain( struct smtp_session *session, const char *username, const char *password )
{
	int ret = 0;
	char credentials[512];
	size_t len;
	char *credentials64;

	debug( "Using AUTH PLAIN\n" );

	/* Send the auth command */
	ret = write( session->sock_fd, (void*)"AUTH PLAIN ", 11 );
	if( ret < 11 )
		return ret;

	/* Encode the username & password */
	memset( credentials, '\0', 512 );
	snprintf( credentials, 512, "%c%s%c%s", '\0', username, '\0', password );
	len = base64_encode( &credentials64, credentials, ( strlen( username ) + strlen( password ) + 2 ) );

	debug( "%s\n", credentials64 );

	/* Send credentials */
	ret = write( session->sock_fd, (void*)credentials64, len );
	free( credentials64 );
	if( ret < len )
		return ret;

	ret = write( session->sock_fd, (void*)"\r\n", 2 );
	if( ret < 2 )
		return ret;

	ret = __smtp_wait_for( SMTP_235, session );
	if( ret < 0 )
		return ret;

	debug( "Authentication succeeded.\n%s\n", session->buffer );

	return 0;
}

static int __smtp_auth_login( struct smtp_session *session, const char *username, const char *password )
{
	int ret = 0;
	char *username64, *password64;
	size_t len;
	char login_message[] = "AUTH LOGIN\r\n";

	debug( "Using AUTH LOGIN\n" );

	ret = write( session->sock_fd, (void*)login_message, strlen( login_message ) );
	if( ret < strlen( login_message ) )
		return ret;

	ret = __smtp_wait_for( SMTP_334, session );
	if( ret < 0 )
		return ret;

	/* Send username */
	len = base64_encode( &username64, username, strlen( username ) );
	if( len < 0 )
	{
		if( username64 )
			free( username64 );
		return EINVAL;
	}
	debug( "Username: %s\n", username64 );

	ret = write( session->sock_fd, (void*)username64, len );
	free( username64 );
	if( ret < len )
		return ret;
	ret = write( session->sock_fd, (void*)"\r\n", 2 );
	if( ret < 2 )
		return ret;

	ret = __smtp_wait_for( SMTP_334, session );
	if( ret < 0 )
		return ret;

	/* Send password */
	len = base64_encode( &password64, password, strlen( password ) );
	if( len < 0 )
	{
		if( password64 )
			free( password64 );
		return EINVAL;
	}
	debug( "Password: %s\n", password64 );

	ret = write( session->sock_fd, (void*)password64, len );
	free( password64 );
	if( ret < len )
		return ret;
	ret = write( session->sock_fd, (void*)"\r\n", 2 );
	if( ret < 2 )
		return ret;

	ret = __smtp_wait_for( SMTP_235, session );
	if( ret < 0 )
		return ret;

	debug( "Authentication succeeded.\n%s\n", session->buffer );

	return 0;
}

int __smtp_authenticate( struct smtp_session *session, const char *username, const char *password )
{
	int ret = 0;
#if 0
	if( session->auth_flags & SMTP_AUTH_CRAM_MD5 )
		ret = __smtp_auth_cram_md5( session, username, password );
	else if( session->auth_flags & SMTP_AUTH_PLAIN )
#else
	if( session->auth_flags & SMTP_AUTH_PLAIN )
#endif
		ret = __smtp_auth_plain( session, username, password );
	else if( session->auth_flags & SMTP_AUTH_LOGIN )
		ret = __smtp_auth_login( session, username, password );
	else
		ret = ECONNABORTED;

	return ret;
}

