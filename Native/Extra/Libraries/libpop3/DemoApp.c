#include <libpop3.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
	struct pop3_session *session;
	int ret = 0, count, size;
	char *message;

	char *server, *username, *password;

	if( argc < 4 )
	{
		printf("DemoApp [server] [username] [password]\n");
		return EXIT_FAILURE;
	}

	server = argv[1];
	username = argv[2];
	password = argv[3];

	printf("Connecting to %s\n", server );

	session = pop3_create_session(server, POP3_DEFAULT_PORT, POP3_DEFAULT_TIMEOUT, 0 );
	if( NULL == session )
	{
		printf("pop3_create_session() failed!\n");
		return EXIT_FAILURE;
	}

	ret = pop3_connect( session, username, password );
	if( ret )
	{
		printf("pop3_connect() failed\n");
		printf("%s", session->buffer );
		return EXIT_FAILURE;
	}
	printf("%s", session->buffer );

	ret = pop3_get_message_count( session, &count, &size );
	if( ret )
	{
		printf("pop3_get_message_count() failed\n");
		printf("%s", session->buffer );
		return EXIT_FAILURE;
	}
	printf("%s", session->buffer );
	printf("%i messages totaling %i bytes\n", count, size );

	ret = pop3_get_message_size( session, 1, &size );
	if( ret )
	{
		printf("pop3_get_message_size() failed\n");
		printf("%s", session->buffer );
		return EXIT_FAILURE;
	}
	printf("%s", session->buffer );
	printf("%i bytes\n", size );

	ret = pop3_get_message( session, 1, (void*)&message );
	if( ret )
	{
		printf("pop3_get_message() failed\n");
		printf("%s", session->buffer );
	}
	else
	{
		printf("%s", session->buffer );
		printf("--\n%s--\n", message );
	}
#if 0
	/* BE CAREFUL!  If you enable this code the first message WILL
	   be deleted whenever you run pop3test! */

	ret = pop3_delete_message( session, 1 );
	if( ret )
		printf("pop3_delete_message() failed\n");
	printf("%s", session->buffer );
#endif

	pop3_disconnect( session );
	printf("%s", session->buffer );

	pop3_destroy_session( session );
	return EXIT_SUCCESS;
}
