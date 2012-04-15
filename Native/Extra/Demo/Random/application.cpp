#include "application.h"
#include "mainwindow.h"

#include <stdio.h>
#include "util/random.h"

App::App() : os::Application( "application/x-vnd.syllable-application" )
{	
	m_pcMainWindow = new MainWindow();
	m_pcMainWindow->CenterInScreen();
	m_pcMainWindow->Show();
	m_pcMainWindow->MakeFocus();
}




