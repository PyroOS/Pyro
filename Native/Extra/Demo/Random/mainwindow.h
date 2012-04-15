#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <util/application.h>
#include <gui/window.h>
#include <gui/image.h>
#include <util/message.h>
#include <util/resources.h>
#include <gui/stringview.h>

#include "util/random.h"
#include "view.h"

class MainWindow : public os::Window
{
public:
	MainWindow();
	void HandleMessage( os::Message* );
private:
	bool OkToQuit();
	RandView* m_pcRandView;
	os::Random cRandom;
	
	os::StringView *rand_float_text, *rand_float;
	os::StringView *rand_int_text, *rand_int;
};

#endif





