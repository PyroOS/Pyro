#include <libsmtp.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
	int ret;
	struct smtp_session *session;
	struct smtp_message *message;
	char *server, *from, *to, *data;
	int bytes;

	if( argc < 4 )
	{
		printf("smtptest [server] [from] [to]\n");
		return EXIT_FAILURE;
	}

	server = argv[1];
	from = argv[2];
	to = argv[3];

	printf("Connecting to %s\n", server );

	session = smtp_create_session( server, SMTP_DEFAULT_PORT, SMTP_DEFAULT_TIMEOUT, 0 );
	if( NULL == session )
	{
		printf("smtp_create_session() failed\n");
		return EXIT_FAILURE;
	}
	
	ret = smtp_connect( session );
	if( ret )
	{
		printf("smtp_connect() failed\n");
		printf("%s", session->buffer );
		return EXIT_FAILURE;
	}
	printf("%s", session->buffer );

	data = (char*)malloc( 4096 );
	if( NULL == data )
	{
		printf("Couldn't malloc() data, but this is nothing to do with libsmtp!\n");
		return EXIT_FAILURE;
	}

	printf("Enter the message to be sent.  End with CTRL+D\n");

	memset( data, 0, 4096 );
	for( bytes = 0; bytes < 406; ++bytes )
	{
		int c = getchar();
		if( c == EOF )
			break;
		*(data + bytes) = c;
	}

	printf("\nMessage to be sent is as follows");
	printf("\n--\n%s\n--\n", data );

	message = smtp_create_message( from, data );
	smtp_add_recipiant( message, to );

	ret = smtp_send( session, message );
	if( ret )
	{
		printf("smtp_send() failed\n");
		printf("%s", session->buffer );
		return EXIT_FAILURE;
	}
	printf("%s", session->buffer );

	smtp_destroy_message( message );
	smtp_disconnect( session );
	printf("%s", session->buffer );

	smtp_destroy_session( session );
	return EXIT_SUCCESS;
}
