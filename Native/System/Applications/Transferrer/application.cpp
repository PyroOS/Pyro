#include "application.h"
#include "mainwindow.h"

#include <string.h>

bool g_bDebug = false;

/** \brief Parse the arguments and pass it to MainWindow.
 * This is used internally by App.
 *
 * \param myWindow Window to pass the arguments to.
 * \param argc Number of arguments.
 * \param argv Array of argument strings.
 *
 */
bool App::_parseArgs(MainWindow* myWindow, int argc, char **argv)
{
	os::String zScheme = "ftp"; // Default is ftp
	os::String zHost = ""; // No host
	os::String zUser = ""; // No user
	os::String zPass = ""; // No password
	os::String zPort = ""; // No port.
	
	
	// Go to argc - 1 so we don't go past the end.
	for(int i = 1; i < argc; i++)
	{
		/* Type of connection (eg. ftp) */
		if ( !strncmp(argv[i], "-t", 2) )
		{
			if ( i != argc - 1 )
				zScheme = argv[i+1];
			i++;
		}
		else if ( !strncmp(argv[i], "-H", 2) ) // Host.
		{
			if ( i != argc - 1 )
				zHost = argv[i+1];
			i++;
		}
		else if ( !strncmp(argv[i], "-U", 2) ) // Username.
		{
			if ( i != argc - 1 )
				zUser = argv[i+1];
			i++;
		}
		else if ( !strncmp(argv[i], "-P", 2) ) // Password.
		{
			if ( i != argc - 1 )
				zPass = argv[i+1];		
			i++;
		}
		else if ( !strncmp(argv[i], "-N", 2) ) // Port number.
		{
			if ( i != argc - 1 )
				zPort = argv[i+1];		
			i++;
		}
		else // Output usage information.
		{
			_Usage(argv[0]);
			return false;
		}
	}
	
	if ( zScheme != "" && zHost != "" )
	{
		myWindow->OpenConnection(zScheme, zHost, zPort, zUser, zPass);
	}
	
	return true;
}

/** \brief Output Usage Information.
 * Output user information and aborts.
 *
 * \param name Name of the program.
 */
void App::_Usage(char *name)
{
	fprintf(stderr, "Usage: %s [-t (ftp)] [-H host address] [-U username] [-P password] [-N port]\n\n", name);
	
	// Send a message to the App telling it to quit.
	PostMessage(M_QUIT);
	
}

/** \brief Application class constructor.
 * Create the initial window and place it in the center
 * of the screen.
 *
 * \param argc The number of arguments passed to the program.
 * \param argv The array of char * strings that were passed to the program.
 */
App::App(int argc, char **argv) : os::Application( "application/x-vnd.syllable-net-Transferrer" )
{
	m_pcMainWindow = new MainWindow();
	
	// Show the window if the command line arguments were correct.
	if ( _parseArgs((MainWindow *)m_pcMainWindow, argc, argv) )
	{
		m_pcMainWindow->Show();
		m_pcMainWindow->MakeFocus();
	}
}

/** \brief Default deconstructor.
 */
App::~App()
{
}

/** Terminate the main window.
 */
bool App::OkToQuit()
{
	m_pcMainWindow->Terminate();
	return( true );
}


