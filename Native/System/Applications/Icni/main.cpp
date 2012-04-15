#include <gui/window.h>
#include <gui/requesters.h>
#include <util/application.h>

#include "win.h"

using namespace os;

class MyApp : public Application
{
    public:
    MyApp() : Application( "application/x-vnd.digitaliz-Icni" ) {
    	m_pcWindow = new Win( Rect( 0, 0, 400, 400 ) );
    	m_pcWindow->CenterInScreen();
    	m_pcWindow->Show();
    }

    ~MyApp() {
    }

    private:
    Window *m_pcWindow;
};

int main(void)
{
    MyApp *pcApp = new MyApp();

	srand( time( NULL ) );

    pcApp->Run();
}


