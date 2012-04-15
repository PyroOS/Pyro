#include "main_documentation.h"
#include "application.h"

#include <curl/curl.h>

/** \brief Simple main function
 * Calls the application class and runs it.
 *
 */
int main( int argc, char *argv[] )
{
	/* Initialise libcurl. 
	 * This has to be done before any other threads are running.
	 */
	int nResult = curl_global_init( CURL_GLOBAL_ALL );
	if( nResult ) {
		fprintf( stderr, "Transferrer: ERROR: Could not initialise curl library!\n" );
		exit( 1 );
	}
	DEBUG( "Libcurl version: %s\n", curl_version() );
	
	App* pcApp = new App(argc, argv);
	pcApp->Run();
	
	/* Cleanup curl library. 
	 * This has to be done here after all other threads have stopped.
	 */
	curl_global_cleanup();
	return( 0 );
}







