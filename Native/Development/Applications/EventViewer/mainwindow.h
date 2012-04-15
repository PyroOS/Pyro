#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <util/application.h>
#include <gui/window.h>
#include <gui/image.h>
#include <gui/treeview.h>
#include <gui/layoutview.h>
#include <util/message.h>
#include <util/resources.h>
#include <util/event.h>

class MainWindow : public os::Window
{
public:
	MainWindow();
	void HandleMessage( os::Message* );
private:
	void GetEvents( os::Event* pcEvent, os::String zID, int nLevel );
	bool OkToQuit();
	os::TreeView* m_pcTree;
};

#endif
