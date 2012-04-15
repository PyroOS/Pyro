#include <gui/layoutview.h>
#include <gui/iconview.h>

#include <util/message.h>

#include "containerview.h"
#include "address_field.h"
#include "localview.h"
#include "remoteview.h"


/** \brief ContainerView Constructor.
 * This creates the different portions of the local and remote view window.
 *
 * \param cRect The rectangle to pass to os::View.
 * \param cName The name for the view.
 */
ContainerView::ContainerView(const os::Rect& cRect,const os::String& cName) : os::LayoutView(cRect,cName)
{
	// Initialize the remote view window.
	remote = new RemoteIconView(cRect, "remote_view");
	remote->SetView(os::IconView::VIEW_DETAILS);
	remote->SetDirChangeMsg( new Message( M_REMOTE_DIR_CHANGED ) );
	
	// Initialize the local icon view.
	local = new LocalIconView( cRect, "local_view" );		/* The name parameter currently isn't used because of libsyllable problems, but will be in future */
	local->SetView(os::IconDirectoryView::VIEW_DETAILS);
	local->SetDirChangeMsg(NULL);
	local->SetName("local_icon_dir");
	local->SetDirChangeMsg( new Message( M_LOCAL_DIR_CHANGED ) );
	
	local->SetTabOrder(100);	/* For keyboard navigation around the app */
	remote->SetTabOrder(101);
	
	// Create the local and remote AddressField bars.
	m_pcLocalBar = new AddressField( local->GetPath(), new Message( M_LOCAL_PATH_BAR_INVOKED ) );
	m_pcRemoteBar = new AddressField( "", new Message( M_REMOTE_PATH_BAR_INVOKED ) );
	
	// Generate the left pane node.
	VLayoutNode* pcLeftRoot = new VLayoutNode( "left_pane_node" );
	pcLeftRoot->SetBorders( Rect(3,1,1,1) );
	HLayoutNode* pcTmp = new HLayoutNode( "left_top_node" );
	pcTmp->AddChild( m_pcLocalBar );		/* path bar */
	pcTmp->LimitMaxSize( Point( 1000000, 30 ) );
	pcLeftRoot->AddChild( pcTmp );
	pcLeftRoot->AddChild( local );
	LayoutView* pcLeft = new LayoutView( cRect, "left_pane", pcLeftRoot );
	
	// Generate the right pane node.
	VLayoutNode* pcRightRoot = new VLayoutNode( "right_pane_node" );
	pcRightRoot->SetBorders( Rect(1,1,3,1) );
	HLayoutNode* pcTmp2 = new HLayoutNode( "right_top_node" );
	pcTmp2->AddChild( m_pcRemoteBar );		/* path bar */
	pcTmp2->LimitMaxSize( Point( 1000000, 30 ) );
	pcRightRoot->AddChild( pcTmp2 );
	pcRightRoot->AddChild( remote );
	LayoutView* pcRight = new LayoutView( cRect, "right_pane", pcRightRoot );
	
	// Create the layoutview with the left and right side being the 
	// local and remote views respectively.
	HLayoutNode* pcRootNode = new HLayoutNode( "root_node" );
	pcRootNode->AddChild( pcLeft );
	pcRootNode->AddChild( pcRight );
	SetRoot( pcRootNode );
}

/** \brief Message Handler for the local and remote file views.
 * This passes the correct events along to the different view components.
 *
 * \param pcMessage Pointer to the message to handle.
 */
void ContainerView::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_LOCAL_DIR_CHANGED:
		{
			String zPath;
			if( pcMessage->FindString( "file/path", &zPath ) != 0 ) {
				DEBUG( "ContainerView: Got LOCAL_DIR_CHANGED without path parameter!\n" );
				break;
			}
			m_pcLocalBar->SetPath( zPath );
			break;
		}
		case M_REMOTE_DIR_CHANGED:
		{
			String zPath;
			if( pcMessage->FindString( "file/path", &zPath ) != 0 ) {
				DEBUG( "ContainerView: Got REMOTE_DIR_CHANGED without path parameter!\n" );
				break;
			}
			m_pcRemoteBar->SetPath( zPath );
			break;
		}
		case M_LOCAL_PATH_BAR_INVOKED:
		{
			String zPath;
			if( pcMessage->FindString( "file/path", &zPath ) != 0 ) {
				DEBUG( "ContainerView: Got LOCAL_PATH_BAR_INVOKED without file/path parameter!\n" );
				break;
			}
			local->SetPath( zPath );
			local->Clear();
			local->ReRead();
			m_pcLocalBar->SetPath( zPath );
			
			break;
		}
		case M_REMOTE_PATH_BAR_INVOKED:
		{
			String zPath;
			if( pcMessage->FindString( "file/path", &zPath ) != 0 ) {
				DEBUG( "ContainerView: Got REMOTE_PATH_BAR_INVOKED without file/path parameter!\n" );
				break;
			}
			remote->SetPath( zPath );
			m_pcRemoteBar->SetPath( zPath );
			break;
		}
		default:
		{
			View::HandleMessage( pcMessage );
		}
	}
}

/** \brief Set the server for the view.
 * This sets the Server object for the local and remote views.
 *
 * \param pcServer Pointer to server that should be set for the views.
 */
void ContainerView::SetServer( Server* pcServer )
{
	local->SetServer( pcServer );
	remote->SetServer( pcServer );
}

/** \brief AllAttached Overloaded Function.
 */
void ContainerView::AllAttached()
{
	local->SetTarget(this);
	local->ReRead();
	
	remote->SetTarget(this);
	remote->Layout();

	m_pcLocalBar->SetTarget( this );
	m_pcRemoteBar->SetTarget( this );
}

/** \brief GetPreferredSize(bool) overloading function.
 * Returns the current width and height for the preferred size.
 *
 * \brief value This is ignored but required by the subclass.
 */
os::Point ContainerView::GetPreferredSize(bool value) const
{
	return (os::Point(GetBounds().Width(),GetBounds().Height()));
}

