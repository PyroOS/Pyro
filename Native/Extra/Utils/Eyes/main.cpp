// Eyes -:-  (C)opyright 2005 Jonas Jarvoll
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <util/application.h>
#include <gui/window.h>
#include <gui/rect.h>
#include <gui/requesters.h>
#include "eyewidget.h"
#include "messages.h"


using namespace os;

int main(int argc, char* argv[]);

#define OUR_TIMER 0
#define UPDATE_RATE 50000

class EyeWindow : public Window
{
	public:
		EyeWindow(const Rect& cFrame);
		void HandleMessage(Message* pcMessage);
		bool OkToQuit(void);
		void TimerTick(int nID);
	private:
		EyeWidget* pcEyeWidget;
};


EyeWindow::EyeWindow(const Rect& cFrame) : Window(cFrame, "main_window", "Eyes")
{
	SetSizeLimits( Point( 300, 200 ), Point( 99999,99999 ) );
	pcEyeWidget = new EyeWidget( GetBounds(), "eye", CF_FOLLOW_ALL );
	AddChild(pcEyeWidget);

	AddTimer(this, OUR_TIMER, UPDATE_RATE, false);
}

void EyeWindow::HandleMessage(Message* pcMessage)
{
	switch(pcMessage->GetCode())	//Get the message code from the message
	{		
		case M_MENU_ABOUT:
		{
			Alert* pcAboutAlert = new Alert( "About Eyes","Eyes is clone of the famous XEyes but for Syllable OS.\n\nCopyright (C) 2005 Jonas Jarvoll.\n\nEyes are released under the license GPL.", (Alert::alert_icon) Alert::ALERT_INFO, "OK", NULL );
			pcAboutAlert->Go( new Invoker() );

			break;
		}
		case M_MENU_QUIT:
		{
			OkToQuit();
			break;
		}
	}
}

bool EyeWindow::OkToQuit(void)
{
	Application::GetInstance()->PostMessage(M_QUIT);

	return (true);
}

void EyeWindow::TimerTick(int nID)
{
	// Make sure it is our timer
	if(nID==OUR_TIMER)
		pcEyeWidget->UpdateEyes();
}

class EyeApp : public Application
{
	public:
		EyeApp();
   
	private:
		EyeWindow* pcMainWindow;
};

EyeApp::EyeApp(void) : Application("basic/x-Jonas Jarvoll-Eyes")
{
	pcMainWindow = new EyeWindow(Rect(100,125,450,570+32));		
	pcMainWindow->Show();
	pcMainWindow->MakeFocus();
}

int main(int argc, char* argv[])
{
	EyeApp* pcEyeApp=new EyeApp();
	pcEyeApp->Run();

	return(0);
}



