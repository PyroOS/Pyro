#include "progresswindow.h"
#include "settings.h"
#include "messages.h"

#include <cstdlib>

/** \brief Get Resource from String.
 * This wrapper function is used to help get the resources for the images
 * within the file.
 *
 * \param fileName The file that we're getting the resource image of.
 *
 * \return A pointer to the BitmapImage corresponding to the resource image.
 */
os::BitmapImage *getResource(const os::String& fileName)
{
	os::Resources cCol( get_image_id() ); // For getting image streams
	os::ResStream *pcStream = cCol.GetResourceStream( fileName );
	os::BitmapImage *pcIcon = new os::BitmapImage( pcStream );
	return pcIcon;
}

/** \brief Converts a number into a human-readable size (eg 2.4MB)
 */
String HumanReadableSize( ssize_t nSize )
{
	String cTmp;

	if( nSize < (2<<10) ) { /* 0B - 1KB */
		cTmp = cTmp.Format( "%liB", nSize );
	} else if( nSize < ((uint64)1<<20) ) { /* 1KB - 1MB */
		cTmp = cTmp.Format( "%.1fKB", (float)nSize / ((uint64)1<<10) );
	} else if( nSize < ((uint64)1<<30) ) { /* 1MB - 1GB */
		cTmp = cTmp.Format( "%.2fMB", (float)nSize / ((uint64)1<<20) );
	} else if( nSize < ((uint64)1<<40) ) { /* 1GB - 1TB */
		cTmp = cTmp.Format( "%.2fGB", (float)nSize / ((uint64)1<<30) );
	} else { /* > 1TB */
		cTmp = cTmp.Format( "%.2fTB", (float)nSize / ((uint64)1<<40) );		
	}
	return( cTmp );
}


/** \brief ProgressListView class
    We only need to subclass this to catch right-clicks for the context menu.
*/

class ProgressListView : public os::ListView
{
public:
	ProgressListView( const os::Rect& cFrame, const os::String& cTitle, uint32 nModeFlags, uint32 nResizeMask )
		: os::ListView( cFrame, cTitle, nModeFlags, nResizeMask )
	{ }
	
	~ProgressListView()
	{ }
	
	void MouseDown( const os::Point& cPosition, uint32 nButton );
};

/** \brief Catch right-clicks and display a context menu.
 */
void ProgressListView::MouseDown( const os::Point& cPosition, uint32 nButton )
{
	if( nButton == 2 ) {
		os::ListViewStringRow* pcRow = (os::ListViewStringRow*)GetRow( cPosition );
		if( pcRow != NULL ) {
			os::Point cTmp = cPosition;
			ConvertToScreen( &cTmp );
			int nID = atoi( pcRow->GetString( ID_INDEX ).c_str() );
			bool bFinished = (pcRow->GetString( FINISHED_INDEX ) != "");
			((ProgressWindow*)GetWindow())->OpenContextMenu( cTmp, nID, bFinished );
			return;
		}
	}
	ListView::MouseDown( cPosition, nButton );
}



/** \brief Progress Window.
 *
 * \todo Must change the panels so that when the window is resized,
 * the ListView is resized as well.
 *
 * \todo Need to set up the ListView so that it's scroll bars will
 * become visible when necessary.
 */
ProgressWindow::ProgressWindow(MainWindow *mainWindow, AppSettings* pcSettings)
	: os::Window( os::Rect( 0, 0, 500, 200 ), "progress_wnd", "Progress Window" )
{
	m_pcMainWindow = mainWindow; // Keep track of the main window so we can send messages to it.
	m_pcSettings = pcSettings;
	
	os::LayoutView* pcView = new os::LayoutView( GetBounds(), "layout_view" );
	AddChild( pcView );
	
	/* Tool Bar */
	os::ToolBar *myToolBar = new os::ToolBar(os::Rect(0, 0, GetBounds().Width(), 40), "toolbar");
	myToolBar->AddButton("play_button", "", getResource("play_button.png"), new os::Message(M_PW_RESUME_BUTTON));
	myToolBar->AddButton("pause_button", "", getResource("pause_button.png"), new os::Message(M_PW_PAUSE_BUTTON));
	myToolBar->AddButton("stop_button", "", getResource("stop_button.png"), new os::Message(M_PW_CANCEL_BUTTON));
	pcView->AddChild(myToolBar);
	
    /* List View */
	m_pcListView = new ProgressListView( os::Rect(0, 40, GetBounds().Width(), GetBounds().Height() - 20), "ListView", os::ListView::F_MULTI_SELECT | os::ListView::F_RENDER_BORDER, os::CF_FOLLOW_ALL );

	// First Create the Columns
	int nUnitWidth = (int)GetBounds().Width();
	nUnitWidth /= 15;
	m_pcListView->InsertColumn("Description", nUnitWidth*3, DESCRIPTION_INDEX );
	m_pcListView->InsertColumn("Remote path", nUnitWidth*3, REMOTE_PATH_INDEX);
	m_pcListView->InsertColumn("Direction", nUnitWidth, DIRECTION_INDEX);
	m_pcListView->InsertColumn("Local path", nUnitWidth*3, LOCAL_PATH_INDEX);	
	m_pcListView->InsertColumn("Status", nUnitWidth*2, STATUS_INDEX);
	m_pcListView->InsertColumn("Total Transferred", nUnitWidth, TRANSFERRED_INDEX);
	m_pcListView->InsertColumn("File Size", nUnitWidth, FILE_SIZE_INDEX);
	m_pcListView->InsertColumn("ID", nUnitWidth, ID_INDEX);
	
	/* TODO: Hide the id column from users */

	pcView->AddChild(m_pcListView);

		
#if 0
	/* Status Bar */
    m_pcStatusBar = new os::StatusBar(os::Rect(0,GetBounds().Height()-20,GetBounds().Width(),GetBounds().Height()),"",os::CF_FOLLOW_LEFT | os::CF_FOLLOW_RIGHT | os::CF_FOLLOW_BOTTOM);
    m_pcStatusBar->AddPanel("Status Panel", "X Downloads / X Uploads / X in Queue");
    m_pcStatusBar->AddPanel("Percentage Panel", "Percent Complete: X.XX%");
	pcView->AddChild(m_pcStatusBar);
	/* TODO: properly initialise status bars */
#endif	/* Until statusbar is implemented */

	/* Set Icon */
	os::Resources cCol( get_image_id() );
	os::ResStream *pcStream = cCol.GetResourceStream( "icon48x48.png" );
	os::BitmapImage *pcIcon = new os::BitmapImage( pcStream );
	SetIcon( pcIcon->LockBitmap() );
	delete( pcIcon );
}

ProgressWindow::~ProgressWindow()
{
}


/** \brief Update the list item of the given transfer.
 * Find the ListViewRow corresponding to the given transfer id, and update it.
 * If the data is null, it will delete the listview row.
 * \param pcMsg: the message containing the id and other status data of the job to be updated.
 */
void ProgressWindow::UpdateListItem( Message* pcMsg )
{
	int nID;
	if( pcMsg->FindInt32( "jobID", &nID ) != 0 ) {
		DEBUG( "ProgressWindow: Got M_JOB_UPDATED without id parameter!\n" );
		return;
	}
	
	ListViewStringRow* pcRow;
	/* Look through the ListView for a corresponding row */
	bool bFound = false;
	uint i = 0;
	for( ; i < m_pcListView->GetRowCount(); i++ )
	{
		pcRow = static_cast<ListViewStringRow*>( m_pcListView->GetRow( i ) );
		if( atoi( pcRow->GetString( ID_INDEX ).c_str() ) == nID ) {
			bFound = true;
			break;
		}
	}
	
	/* If  we didn't find the row, create it. */
	if( !bFound ) {
		pcRow = new ListViewStringRow();
		/* Workaround for a libsyllable bug: ListViewStringRow::SetString() crashes if we try to add a new string. So add some dummy strings now. */
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
		pcRow->AppendString( "" );
	}

	/* Update the strings in the row */
	String cDescription = "Unknown";
	pcMsg->FindString( "description", &cDescription );
	pcRow->SetString( DESCRIPTION_INDEX, cDescription );
	
	String cStatus = "Unknown";
	pcMsg->FindString( "jobStatus", &cStatus );
	pcRow->SetString( STATUS_INDEX, cStatus );

	String cRemotePath, cLocalPath;
	int nType;
	if( pcMsg->FindString( "remotePath", &cRemotePath ) == 0 ) {
		pcRow->SetString( REMOTE_PATH_INDEX, cRemotePath );
	}
	if( pcMsg->FindString( "localPath", &cLocalPath ) == 0 ) {
		pcRow->SetString( LOCAL_PATH_INDEX, cLocalPath );
	}
	if( pcMsg->FindInt32( "type", &nType ) == 0 ) {
		String zTmp;
		if( nType == JOB_DOWNLOAD ) zTmp = "-->";
		else if( nType == JOB_UPLOAD ) zTmp = "<--";
		pcRow->SetString( DIRECTION_INDEX, zTmp );
	}

	
	ssize_t nTotalTransferred = -1;
	ssize_t nTotalSize = -1;
	pcMsg->FindInt32( "totalTransferred", (int32*)&nTotalTransferred );
	pcMsg->FindInt32( "totalSize", (int32*)&nTotalSize );
	if( nTotalTransferred != -1 ) {
		pcRow->SetString( TRANSFERRED_INDEX, HumanReadableSize( nTotalTransferred ) );
	}
	if( nTotalSize != -1 ) {
		pcRow->SetString( FILE_SIZE_INDEX, HumanReadableSize( nTotalSize ) );
	}
	
	String cTmp;
	cTmp = cTmp.Format( "%d", nID );
	pcRow->SetString( ID_INDEX, cTmp );
	
	bool bFinished = false;
	if( pcMsg->FindBool( "finished", &bFinished ) == 0 && bFinished ) {	/* Mark it as finished */
		pcRow->SetString( FINISHED_INDEX, "true" );
		/* TODO: Schedule a timer to remove the finished row?
				 Automatically remove older finished rows?
		*/
	}

	if( !bFound ) {
		m_pcListView->InsertRow( pcRow );
	} else {
		m_pcListView->InvalidateRow( i, ListView::INV_VISUAL );	/* Docs say to use INV_VISUAL */
	}
	
	/* TODO: average speeds, total transfers, etc in statusbar */
}


/** \brief Window Message Handler.
 * Handles messages sent to the window.
 *
 * \param pcMessage The messages being sent.
 */
void ProgressWindow::HandleMessage( os::Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		/* Update the list */		
		case M_JOB_UPDATED:
		{
			UpdateListItem( pcMessage );
			break;
		}
		/* Handle messages from the buttons. */
		case M_PW_PAUSE_BUTTON:
		{
			SendPauseMessage( pcMessage );
			break;
		}

		case M_PW_RESUME_BUTTON:
		{
			SendResumeMessage( pcMessage );
			break;
		}

		case M_PW_CANCEL_BUTTON:
		{
			SendCancelMessage( pcMessage );
			break;
		}

		/* Close the window. */
		case M_APP_QUIT:
		{
			PostMessage( os::M_QUIT );
			break;
		}
	}
}


/** Send a pause message for the current download to the main window.
 * \param pcMsg: The message from the GUI.
 *   If this is from the context menu, it contains a transfer id as the parameter 'id'.
 */
void ProgressWindow::SendPauseMessage( os::Message* pcMsg )
{
	os::Message cMessage( M_GUI_PAUSE );
	int nID;
	if( pcMsg->FindInt32( "id", &nID ) == 0 ) {
		cMessage.AddInt32( "jobID", nID );
	}
	// Go through each selected item (i is < 0 if nothing selected).
	for (int i = m_pcListView->GetFirstSelected(); i >= 0 && i <= m_pcListView->GetLastSelected(); i++)
	{
		if( m_pcListView->IsSelected( i ) ) {
			os::ListViewStringRow *row = (os::ListViewStringRow *) m_pcListView->GetRow(i);
			os::String cStr = row->GetString(ID_INDEX);
			int jobID = atoi(cStr.c_str());

			cMessage.AddInt32("jobID", jobID);

			DEBUG("ProgressWindow: sending Pause Message for job %d\n", jobID);
		}
	}
	m_pcMainWindow->PostMessage( &cMessage, m_pcMainWindow );	/* Send the message to the main window */
}


/** Send a resume message for the current download to the main window.
 */
void ProgressWindow::SendResumeMessage( os::Message* pcMsg )
{
	os::Message cMessage( M_GUI_RESUME );
	int nID;
	if( pcMsg->FindInt32( "id", &nID ) == 0 ) {
		cMessage.AddInt32( "jobID", nID );
	}
	// Go through each selected item (i is < 0 if nothing selected).
	for (int i = m_pcListView->GetFirstSelected(); i >= 0 && i <= m_pcListView->GetLastSelected(); i++)
	{
		if( m_pcListView->IsSelected( i ) ) {
			os::ListViewStringRow *row = (os::ListViewStringRow *) m_pcListView->GetRow(i);
			os::String cStr = row->GetString(ID_INDEX);
			int jobID = atoi(cStr.c_str());

			cMessage.AddInt32("jobID", jobID);

			DEBUG("ProgressWindow: sending Resume Message for job %d\n", jobID);
		}
	}
	m_pcMainWindow->PostMessage( &cMessage, m_pcMainWindow ); // Send the message to the main window
}


/** Send a cancel message for the current download to the main window.
 */
void ProgressWindow::SendCancelMessage( os::Message* pcMsg )
{
	os::Message cMessage( M_GUI_CANCEL );
	int nID;
	if( pcMsg->FindInt32( "id", &nID ) == 0 ) {
		cMessage.AddInt32( "jobID", nID );
	}
	// Go through each selected item (i is < 0 if nothing selected).
	for (int i = m_pcListView->GetFirstSelected(); i >= 0 && i <= m_pcListView->GetLastSelected(); i++)
	{
		if( m_pcListView->IsSelected( i ) ) {
			os::ListViewStringRow *row = (os::ListViewStringRow *) m_pcListView->GetRow(i);
			os::String cStr = row->GetString(ID_INDEX);
			int jobID = atoi(cStr.c_str());

			cMessage.AddInt32("jobID", jobID);

			DEBUG("ProgressWindow: sending Cancel Message for job %d\n", jobID);
		}
	}
	m_pcMainWindow->PostMessage( &cMessage, m_pcMainWindow ); // Send the message to the main window
}


bool ProgressWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return( true );
}

/** \brief Creates a context menu for the given transfer and opens it at the given position.
 */
void ProgressWindow::OpenContextMenu( const os::Point& cPos, int nID, bool bDisabled )
{
//	DEBUG( "OpenContextMenu( %.0f,%.0f,  %i )\n", cPos.x, cPos.y, nID );

	os::Message cMsg;
	cMsg.AddInt32( "id", nID );
	os::Menu* pcMenu = new os::Menu( os::Rect(), "context_menu", os::ITEMS_IN_COLUMN );
	cMsg.SetCode( M_PW_PAUSE_BUTTON );
	pcMenu->AddItem( "Pause", new os::Message( cMsg ) );
	cMsg.SetCode( M_PW_RESUME_BUTTON );
	pcMenu->AddItem( "Resume", new os::Message( cMsg ) );
	cMsg.SetCode( M_PW_CANCEL_BUTTON );
	pcMenu->AddItem( "Cancel", new os::Message( cMsg ) );
	
	pcMenu->SetTargetForItems( this );
	pcMenu->SetEnable( !bDisabled );
	pcMenu->Open( cPos );
	/* TODO: is the menu properly deleted? */
}

