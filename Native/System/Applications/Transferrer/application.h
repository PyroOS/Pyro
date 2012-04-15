#ifndef APPLICATION_H
#define APPLICATION_H

#include <util/application.h>
#include <gui/window.h>

#ifndef DEBUG
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif

class MainWindow;

/** \brief Global debugging flag.
 * Indicates if debugging output is enabled.
 */
extern bool g_bDebug;


/** \brief Container Application for Transferrer.
 * This contains the main application window.
 *
 * \todo This should be changed so that it doesn't create
 * the window until the arguments have been verified. This
 * will avoid the output of any extra information except
 * for the usage information.
 */
class App : public os::Application
{
public:
	App(int argc, char **argv);
	~App();
	
	bool OkToQuit();
	
private:
	
	bool _parseArgs(MainWindow* myWindow, int argc, char **argv);
	void _Usage(char *name);
	
	/** The actual window for Transferrer. */
	os::Window* m_pcMainWindow;
};

#endif

