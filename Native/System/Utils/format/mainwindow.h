#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/button.h>
#include <gui/image.h>
#include <gui/frameview.h>
#include <gui/dropdownmenu.h>
#include <gui/requesters.h>
#include <gui/filerequester.h>
#include <util/resources.h>
#include <util/application.h>
#include <util/message.h>
#include "splash.h"

using namespace os;

class MainWindow : public os::Window
{
public:
	MainWindow();
	void LoadDevices();
	void LoadFilesystems();
	void Format();
	void HandleMessage( os::Message* );

private:
	bool OkToQuit();  // Obsolete?
	#include "mainwindowLayout.h"
	BitmapImage* LoadImageFromResource( os::String zResource );
	BitmapImage* m_pcIcon;
	dirent *psEntry;
	DIR *pDir;
	os::FileRequester* m_pcFileRequester;
};

#endif

