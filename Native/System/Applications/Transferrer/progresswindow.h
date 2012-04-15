#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include <gui/window.h>
#include <gui/view.h>
#include <gui/layoutview.h>
#include <gui/button.h>
#include <gui/image.h>
#include <gui/listview.h>
#include <gui/statusbar.h>
#include <gui/toolbar.h>
#include <util/resources.h>
#include <util/application.h>
#include <util/message.h>

#include "mainwindow.h"

class AppSettings;

/* Enum for the indecies into the list view. */
enum {
	DESCRIPTION_INDEX = 0,
	REMOTE_PATH_INDEX,
	DIRECTION_INDEX,
	LOCAL_PATH_INDEX,
	STATUS_INDEX,
	TRANSFERRED_INDEX,
	FILE_SIZE_INDEX,
	ID_INDEX,
	FINISHED_INDEX
};

/** \brief Displays a window containing the progress of downloads.
 * This also allows the user to pause/force the start of items in queue.
 *
 * \todo When this window is closed it closes the whole program. It should just hide
 * until the Terminate method is called.
 */
class ProgressWindow : public os::Window
{
public:
	ProgressWindow(MainWindow *mainWindow, AppSettings* pcSettings);
	~ProgressWindow();
	void HandleMessage( os::Message* );
	bool OkToQuit(void);

	void SendPauseMessage( os::Message* pcMsg );
	void SendResumeMessage( os::Message* pcMsg );
	void SendCancelMessage( os::Message* pcMsg );
	
	void OpenContextMenu( const Point& cPos, int nID, bool bDisabled );
	
private:
	void UpdateListItem( Message* pcMsg );

	os::VLayoutNode* m_pcRoot;
	os::StatusBar *m_pcStatusBar;
	os::ListView* m_pcListView;

	MainWindow *m_pcMainWindow;
	AppSettings* m_pcSettings;
};

#endif



