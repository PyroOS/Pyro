#include <util/message.h>
#include <util/application.h>
#include <gui/window.h>
#include <gui/button.h>
#include <gui/textview.h>
#include <gui/layoutview.h>

using namespace os;

#include "postoffice.h"

#include <iostream.h>

#define ID_SEND_TEXT		1

/* --- Transmitter Window --- */

class TransmitterWindow:public Window
{
	public:
	TransmitterWindow(const Rect & r);
	~TransmitterWindow();
	bool OkToQuit();
	void HandleMessage(Message *msg);

	private:
	TextView	*m_TextView;
	Button		*m_SendButton;
};

bool TransmitterWindow::OkToQuit()
{
	Application::GetInstance()->PostMessage(M_QUIT);
	return true;
}

TransmitterWindow::TransmitterWindow(const Rect & r)
	:Window(r, "TransmitterWindow", "Transmitter", 0, CURRENT_DESKTOP)
{
	LayoutView *view = new LayoutView(GetBounds(), "", NULL, CF_FOLLOW_ALL);
	VLayoutNode *root = new VLayoutNode("root");

	m_TextView = new TextView(Rect(0, 0, 0, 0), "textview", "", CF_FOLLOW_ALL, /*WID_FULL_UPDATE_ON_RESIZE|*/WID_WILL_DRAW);
	m_TextView->SetMultiLine(false);
	m_TextView->SetReadOnly(false);
	root->AddChild(m_TextView, 1.0f);

	m_SendButton = new Button(Rect(0, 0, 0, 0), "button", "Send!", new Message(ID_SEND_TEXT));
	root->AddChild(m_SendButton, 1.0f);

	view->SetRoot(root);

	AddChild(view);

	AddMailbox("Transmitter");
}

TransmitterWindow::~TransmitterWindow()
{
	RemMailbox("Transmitter");
}

void TransmitterWindow::HandleMessage(Message *msg)
{
	if(msg->GetCode() == ID_SEND_TEXT) {
		Message *newmsg;
		newmsg = new Message(ID_SEND_TEXT);
		newmsg->AddString("TextToSend", 
			m_TextView->GetBuffer()[0]);
   
		cout << "Transmitter: Sending Message..." << endl;

		Mail("Receiver", newmsg);
		return;
	}
	Window::HandleMessage(msg);
}

/* --- Receiver Window --- */

class ReceiverWindow:public Window
{
	public:
	ReceiverWindow(const Rect & r);
	~ReceiverWindow();
	bool OkToQuit();
	void HandleMessage(Message *msg);

	private:
	TextView	*m_TextView;
};

ReceiverWindow::ReceiverWindow(const Rect & r)
	:Window(r, "ReceiverWindow", "Receiver", 0, CURRENT_DESKTOP)
{
	Rect bounds = GetBounds();

	m_TextView = new TextView(bounds, "tv", "", CF_FOLLOW_ALL, /*WID_FULL_UPDATE_ON_RESIZE|*/WID_WILL_DRAW);

	m_TextView->SetMultiLine(true);
	m_TextView->SetReadOnly(true);

	AddChild(m_TextView);

	AddMailbox("Receiver");
}

ReceiverWindow::~ReceiverWindow()
{
	RemMailbox("Receiver");
}

void ReceiverWindow::HandleMessage(Message *msg)
{
	if(msg->GetCode() == ID_SEND_TEXT) {
		std::string	str;	

		cout << "Receiver: ID_SEND_TEXT Message Received!" << endl;

		msg->FindString("TextToSend", &str);
		m_TextView->Insert(str.c_str(), true);
		m_TextView->Insert("\n", true);
		return;
	}
	Window::HandleMessage(msg);
}

bool ReceiverWindow::OkToQuit()
{
	Application::GetInstance()->PostMessage(M_QUIT);
	return true;
}

/* --- Application Object --- */

class MyApp:public Application
{
	public:
	MyApp();

	private:
	Window *win1;
	Window *win2;
};

MyApp::MyApp()
	:Application("application/x-vnd.digitaliz-PostOfficeTest")
{
	win1 = new TransmitterWindow(Rect(20,20,350,60));
	win1->Show();

	win1 = new ReceiverWindow(Rect(380,20,730,170));
	win1->Show();
}

/* --- main --- */

int main(void)
{
	MyApp *thisApp;

	thisApp = new MyApp;
	thisApp->Run();

	PostOffice::GetInstance()->Quit();

	return 0;
}
