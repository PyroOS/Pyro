/** \todo Should cancel any previous dirlistings if a new listing is requested
*/

#include <storage/file.h>
#include <storage/path.h>
#include <storage/filereference.h>
#include <util/message.h>

#include <gui/menu.h>
#include <gui/guidefines.h>

#include "remoteview.h"
#include "server.h"
#include "requesters.h"

/** \brief FTP Remote View Constructor.
 * Initializes all the information necessary for the view.
 */
RemoteIconView::RemoteIconView( const Rect& cFrame, const String& zName ) : IconView( cFrame, zName, CF_FOLLOW_RIGHT | CF_FOLLOW_BOTTOM )
{
	m_pcServer = NULL;
	m_zPath = "";

	m_bUpdatePending = false;
	
	m_pcDirChangedMsg = NULL;
	
	m_pcFileContextMenu = NULL;
	m_pcDirContextMenu = NULL;
	
	SetAutoSort( false );
	
#if 0
	/** \bug This crashes when quitting !? */
//	SetFlags( GetFlags() | WID_DRAW_ON_CHILDREN );
	m_pcStringView = new StringView( GetBounds(), "remote_stringview", "", ALIGN_CENTER, CF_FOLLOW_ALL );
	AddChild( m_pcStringView );
	m_pcStringView->Hide();
#endif
}

/** \brief FTP Remote View deconstructor.
 * This removes the m_pcDirChangedMsg object before destruction.
 */
RemoteIconView::~RemoteIconView()
{
	if( m_pcDirChangedMsg )
		delete( m_pcDirChangedMsg );
		
	if( m_pcFileContextMenu ) delete( m_pcFileContextMenu );
	if( m_pcDirContextMenu ) delete( m_pcDirContextMenu );
	
#if 0
	RemoveChild( m_pcStringView );
	delete( m_pcStringView );
#endif
}

/** \brief Set Director Change Message.
 * This removes the old directory changed message and
 * replaces it with the new one.
 */
void RemoteIconView::SetDirChangeMsg( Message* pcMsg )
{
	if( m_pcDirChangedMsg )
		delete( m_pcDirChangedMsg );

	m_pcDirChangedMsg = pcMsg;
}

/** \brief SetServer
 * This sets the server for the current view and sets the path
 * to the root of this server. It then updates the view.
 * 
 * \todo Find out from the server what the current dir is via PWD.
 */
void RemoteIconView::SetServer( Server* pcServer )
{
	m_pcServer = pcServer;
	m_zPath = "";
	Update();
}

/** \brief Retrieve the current server.
 * Returns the current Server object for this view.
 */
Server* RemoteIconView::GetServer()
{
	return( m_pcServer );
}

/** \brief Set Path to the given path.
 * Sets the path to zPath and updates the window.
 *
 * \param zPath The path to set to.
 */
void RemoteIconView::SetPath( const String& zPath )
{
	m_zPath = zPath;
	Update();
}

/** \brief Retrieve the current path.
 * This returns the current path for the remote view.
 */
String RemoteIconView::GetPath()
{
	return( m_zPath );
}

/** \brief AttachedToWindow Overloaded Method.
 * This updates the current view once the object is attached to a window.
 */
void RemoteIconView::AttachedToWindow()
{
	Update();
}

#if 0
void RemoteIconView::Paint( const Rect& cUpdateRect )
{
	if( m_pcServer == NULL ) {
		/* Draw a message if we aren't connected to a server */
		DrawMessage( "Not connected!\nEnter a server in the toolbar above and hit 'Connect'." );	/* TODO: localised strings */
	} else if( m_bUpdatePending && GetIconCount() == 0 ) {
		/* Draw a 'Waiting for server' message */
		DrawMessage( "Waiting for server..." );	/* TODO: localised strings */
	} else {
		IconView::Paint( cUpdateRect );
	}
}

void RemoteIconView::DrawMessage( const String& zMsg )
{
	DEBUG( "RemoteIconView::DrawMessage( %s )\n", zMsg.c_str() );
	EraseRect( GetBounds() );
	SetFgColor( Color32_s( 255,0,0 ) );
	Point cSize = GetTextExtent( zMsg );
	Point cPos;
	cPos.x = Width()/2 - cSize.x/2;
	cPos.y = Height()/2 - cSize.y/2;
	DrawString( cPos, zMsg );
	Flush();
}
#endif

/** \brief Show a message on the (blank) remote view.
 * This is used to show a message on the remoteview, eg while waiting for the server to send directory data.
 * Currently it doesn't work - the code is #ifdef'd out since it crashes. It just printfs the message to the terminal.
 * 
 * \todo Fix the crashes (may be libsyllable bugs?).
 */
void RemoteIconView::ShowMessage( const String& zMsg )
{
	DEBUG( "RemoteIconView::ShowMessage( %s )\n", zMsg.c_str() );
#if 0
	m_pcStringView->SetString( zMsg );
	if( !m_pcStringView->IsVisible() ) m_pcStringView->Show();
#endif
}

/** \brief Hide Message.
 * This is used to hide the on screen message shown by ShowMessage().
 *
 * \todo Not implemented yet since ShowMessage() doesn't work.
 *
 */
void RemoteIconView::HideMessage()
{
#if 0
	m_pcStringView->SetString( "" );
	if( m_pcStringView->IsVisible() ) m_pcStringView->Hide();
#endif
}

/** \brief Update the current directory listing.
 * Request the Server to download a directory listing and send it to us asynchronously.
 * When the listing is downloaded, it is sent back to us via a M_REMOTE_DIRLISTING message
 * and SetContents() is called.
 */
void RemoteIconView::Update()
{
	Clear();  /* Remove all icons */
	if( m_pcServer ) {
		m_bUpdatePending = true;
		m_pcServer->GetDirListing( m_zPath, this );
		ShowMessage( "Waiting for server..." );	/* TODO: localise strings */
	} else {
		ShowMessage( "Not connected.\nEnter a server in the toolbar above and hit 'Connect'." );
	}
}

/** \brief Get a suitable icon for the file.
 * This should retrieve a suitable icon for the file to display and return
 * an Image pointer to it.
 *
 * \param pcNode The item from the remote view that needs an icon.
 * \param bSmall If true, the icon should be 24x24. Otherwise it should be 48x48.
 *
 * \todo Ask the registrar for a suitable icon, based on the file extension.
 * \todo Add basic file/dir icons as resources, and fall back to them if the icon file is missing.
 */
Image* RemoteIconView::GetNodeImage( RemoteNode* pcNode, bool bSmall )
{
	File* pcFile;
	/* Todo: ask the Registrar for suitable icon */
	/* Looks like with current RegistrarManager class, either have to parse the list of registered types, or create a 'fake' temp file and query registrar about that instead */
	if( pcNode->m_bIsDir ) {
		/* TODO: fall back to resource if file isn't present */
		pcFile = new File( "/system/icons/folder.png" );
	} else {
		pcFile = new File( "/system/icons/file.png" );
	}
	BitmapImage* pcImage = new BitmapImage( pcFile );
	
	if( bSmall ) {
		if( pcImage && pcImage->GetSize() != Point( 24,24 ) )
			pcImage->SetSize( Point( 24,24 ) );
	} else {
		if( pcImage && pcImage->GetSize() != Point( 48,48 ) )
			pcImage->SetSize( Point( 48,48 ) );
	}
	return( pcImage );
}

/** \brief Create and display icons for each of the RemoteNodes. in pacNodes.
 * This sets the remote view contents and updates the layout when necessary.
 * It also gets the initial path the first time it is called.
 *
 * \todo Perform periodic layouts while downloading large dirlisting.
 * \todo Display file sizes in a human readable format?
 * \todo Move the entry path stuff somewhere else?
 *
 * \param pacNodes List of RemoteNodes that should be updated for the remote view contents.
 * \param bInitial Set this to true if this is the first of several messages and we should clear the iconview first.
 * \param bFinal Set this to true if this is the last of the directory listing. and should Layout() when done.
 */
void RemoteIconView::SetContents( std::vector< RemoteNode >* pacNodes, bool bInitial, bool bFinal )
{
	/* If we haven't yet got the entry path, get it now. */
	if( m_zPath == "" ) {	/* \todo Is it possible that GetEntryPath() could be "" ? */
		if( m_pcServer->GetEntryPath() != "" ) {
			m_zPath = m_pcServer->GetEntryPath();

			if( m_pcDirChangedMsg ) {
				Message cTmp = *m_pcDirChangedMsg;
				cTmp.AddString( "file/path", m_zPath );
				Invoke( &cTmp );
			}
		} else {
			DEBUG( "RemoteView: GetEntryPath() is empty.\n" );
		}
	}
	
	if( bInitial ) Clear();	/* Remove all icons */

	HideMessage();

	int nCount = pacNodes->size();
//	DEBUG( "RemoteIconView: Dirlisting fragment contains %i nodes; %s initial, %sfinal\n", nCount, bInitial?"":"not ", bFinal?"":"not " );

	for( int i = 0; i < nCount; i++ ) {
		RemoteNode cNode = (*pacNodes)[i];
//		DEBUG( "  '%s', %s, size %i, permissions %i\n", cNode.GetName().c_str(), (cNode.IsDir() ? "dir" : "file"), cNode.GetSize(), cNode.GetPermissions() );

		Image* pcImage = GetNodeImage( &cNode, (GetView() == VIEW_LIST || GetView() == VIEW_DETAILS) );
		RemoteIconData* pcData = new RemoteIconData;
		pcData->m_cNode = cNode;

		uint nIndex = AddIcon( pcImage, pcData );
		AddIconString( nIndex, cNode.GetName() );

		String zTmp;
		if( cNode.IsDir() ) {
			zTmp = "<Dir>";
		} else {
			zTmp.Format( "%li", cNode.GetSize() );
		}
		AddIconString( nIndex, zTmp );
		
		AddIconString( nIndex, cNode.GetTimestamp().GetDate() );
	}
	
	// When this is the last of the directory listing, reset the update flag.
	if( bFinal ) {
		m_bUpdatePending = false;
	}
	
	Layout();
}

/** \brief Message Handling Callback.
 * This handles all callbacks to the remote view window.
 */
void RemoteIconView::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_REMOTE_DIRLISTING:
		{
			if( !m_bUpdatePending ) {
				DEBUG( "RemoteIconView: Got REMOTE_DIRLISTING while no dirlisting is pending!\n" );
				return;
			}
			
			String zPath;
			if( pcMessage->FindString( "path", &zPath ) != 0 ) {
				DEBUG( "RemoteIconView: Got REMOTE_DIRLISTING without path!\n" );
				return;
			}
			if( zPath != m_zPath ) {
				DEBUG( "RemoteIconView: Got REMOTE_DIRLISTING with wrong path %s! Expecting %s.\n", zPath.c_str(), m_zPath.c_str() );
				return;
			}
			
			std::vector< RemoteNode >* pacNodes;
			/* The Server creates an array of RemoteNodes and passes us the pointer. We should delete it when done. */
			if( pcMessage->FindPointer( "list", (void**)&pacNodes ) ) {
				DEBUG( "RemoteIconView: Got REMOTE_DIRLISTING without pointer to data!\n" );
				return;
			}
			
			bool bInitial;
			if( pcMessage->FindBool( "initial", &bInitial ) != 0 ) bInitial = false;
			
			bool bFinal;
			if( pcMessage->FindBool( "final", &bFinal ) != 0 ) bFinal = false;
			
			SetContents( pacNodes, bInitial, bFinal );
			if( pacNodes ) delete( pacNodes );
		
			break;
		}
		/* Messages from the context menu */
		case M_REMOTE_RENAME:
		{
			/* Check that only one icon is selected & get the selected icon */
			uint nSelectedIcon = -1;
			uint nNumSelected = 0;
			for( uint i = 0; i < GetIconCount(); i++ ) {
				if( GetIconSelected( i ) ) {
					nSelectedIcon = i;
					nNumSelected++;
				}
			}
			if( nNumSelected != 1 ) {
				DEBUG( "RemoteView: Got M_REMOTE_RENAME while %i icons are selected!\n", nNumSelected );
				break;
			}
			RemoteIconData* pcData = (RemoteIconData*)GetIconData( nSelectedIcon );

			/* Display rename dialog */
			Window* pcDialog = new RenameRequester( pcData->m_cNode.m_zPath, this );
			pcDialog->CenterInWindow( GetWindow() );
			pcDialog->Show();
			pcDialog->MakeFocus( true );
			break;
		}
		case M_REMOTE_DELETE:
		{
			/* Save a list of the selected files */
			std::vector< RemoteNode >* pacDeleteList = new std::vector< RemoteNode >;
			for( uint i = 0; i < GetIconCount(); i++ ) {
				if( GetIconSelected( i ) ) {
					pacDeleteList->push_back( ((RemoteIconData*)GetIconData( i ))->m_cNode );
				}
			}
			if( pacDeleteList->size() == 0 ) {
				DEBUG( "RemoteView: Got M_REMOTE_DELETE while no icons are selected!\n" );
				delete( pacDeleteList );
				break;
			}

			/* Display confirmation dialog */
			Window* pcDialog = new DeleteConfirmDialog( pacDeleteList, this );
			pcDialog->CenterInWindow( GetWindow() );
			pcDialog->Show();
			pcDialog->MakeFocus( true );
			break;			
		}
		case M_REMOTE_MKDIR:
		{
			Window* pcDialog = new MkDirRequester( m_zPath, this );
			pcDialog->CenterInWindow( GetWindow() );
			pcDialog->Show();
			pcDialog->MakeFocus( true );
			break;
		}
		case M_DELETE_CONFIRMED:
		{
			std::vector< RemoteNode >* pacDeleteList = NULL;
			pcMessage->FindPointer( "files", (void**)&pacDeleteList );
			if( m_pcServer == NULL ) {	/* Just in case */
				DEBUG( "RemoteView: Got M_DELETE_CONFIRMED while m_pcServer == NULL!\n" );
				delete( pacDeleteList );
				break;
			}
			while( !pacDeleteList->empty() ) {
				RemoteNode* pcNode = &pacDeleteList->back();
				if( pcNode->m_bIsDir ) {
//					DEBUG( "RemoteView: calling RemoveRemoteDir( %s )\n", pcNode->m_zPath.c_str() );
					m_pcServer->RemoveRemoteDir( pcNode->m_zPath );
				} else {
//					DEBUG( "RemoteView: calling DeleteRemote( '%s' )\n", pcNode->m_zPath.c_str() );
					m_pcServer->DeleteRemoteFile( pcNode->m_zPath );
				}
				pacDeleteList->pop_back();
			}
			delete( pacDeleteList );
			Update();
			break;
		}
		case M_MKDIR_CONFIRMED:
		{
			String zPath;
			pcMessage->FindString( "remotepath", &zPath );
			if( m_pcServer == NULL ) {
				DEBUG( "RemoteView: Got M_MKDIR_CONFIRMED while m_pcServer == NULL!\n" );
				break;
			}
			m_pcServer->CreateRemoteDir( zPath );
			Update();
			break;
		}
		case M_RENAME_CONFIRMED:
		{
			String zOldPath, zNewPath;
			pcMessage->FindString( "old_path", &zOldPath );
			pcMessage->FindString( "new_path", &zNewPath );
			if( m_pcServer == NULL ) {
				DEBUG( "RemoteView: Got M_RENAME_CONFIRMED while m_pcServer == NULL!\n" );
				break;
			}
			m_pcServer->MoveRemote( zOldPath, zNewPath );
			Update();
			break;
		}
		default:
		{
			IconView::HandleMessage( pcMessage );
		}
	}
}

/** \brief Invoked
 * Handles when an icon within the view is invoked. If it is a directory
 * it should, the current directory should move into it. If it is a file
 * it should be opened if possible and otherwise it should be downloaded.
 *
 * \todo Should attempt to dowload and open the remote file if it's not a directory?
 * \todo What should be the default action when the file type is unknown?
 */
void RemoteIconView::Invoked( uint nIcon, IconData* pcData )
{
//	DEBUG( "RemoteIconView::Invoked on icon %i (%s)\n", nIcon, GetIconString( nIcon, 0 ).c_str() );
	
	RemoteIconData* pcRData = (RemoteIconData*)pcData;
	
	if( pcRData->m_cNode.IsDir() ) {
		Path cPath = m_zPath;
		cPath.Append( pcRData->m_cNode.m_zName );
		DEBUG( "RemoteView: Changing to %s\n", cPath.GetPath().c_str() );
		if( m_pcDirChangedMsg ) {
			Message cTmp = *m_pcDirChangedMsg;
			cTmp.AddString( "file/path", cPath.GetPath() );
			Invoke( &cTmp );
		}
		SetPath( cPath.GetPath() );
	}
}

/** \brief KeyDown method - used to capture shortcut keys.
 */
void RemoteIconView::KeyDown( const char* pzString, const char* pzRawString, uint32 nQualifiers )
{
//	DEBUG( "KeyDown( '%s'[%i %i] '%s' [%i %i] %x )\n", pzString, pzString[0], pzString[1], pzRawString, pzRawString[0], pzRawString[1], nQualifiers );
	/* We want to check for function keys eg F5. Need to get the _raw_key code for this. */
	if( pzString[0] == VK_FUNCTION_KEY ) {
		Message* pcMsg = GetWindow()->GetCurrentMessage();
		int nRawCode = 0;
		pcMsg->FindInt32( "_raw_key", &nRawCode );
		if( nRawCode == 6 ) { 	/* F5 key */
			Update();
			return;
		}
	}
	/* TODO: backspace key to go up a level, delete key, etc */
	IconView::KeyDown( pzString, pzRawString, nQualifiers );
}
 

/** \brief MouseUp Callback.
 * This handles the situation where someone does a drag and drop of an item.
 * In this case we should copy it to the desired location.
 *
 * \todo Implement recursive transfer when a folder is dropped.
 */
void RemoteIconView::MouseUp( const Point& cPoint, uint32 nButtons, Message* pcData )
{
	if( pcData == NULL || m_pcServer == NULL ) {
		/* No drag & drop, or no connection */
		IconView::MouseUp( cPoint, nButtons, pcData );
		return;
	}
	StopScroll();
	
	/* We got a drag & drop. Check if it contains a local file */
	int nCount, nType;
	pcData->GetNameInfo( "file/path", &nType, &nCount );
	if( nType != T_STRING || nCount == 0 ) {
		/* Something was dropped that isn't a file. Ignore it */
		IconView::MouseUp( cPoint, nButtons, pcData );
		return;
	}
	
	/* Check if there is a directory icon under the mouse. If so, we transfer the file into that directory.
	   Otherwise, we transfer it into this directory. */
	String zDestPath = m_zPath;
	for( uint i = 0; i < GetIconCount(); i++ ) {
		if( Rect( GetIconPosition( i ), GetIconPosition( i ) + GetIconSize() ).DoIntersect( ConvertToView( cPoint ) ) ) {
			/* Found an icon under the mouse */
			RemoteIconData* pcData = (RemoteIconData*)GetIconData( i );
			if( pcData->m_cNode.IsDir() ) {
				/* Found a dir under the mouse. Add it to the destination path */
				zDestPath += "/";
				zDestPath += pcData->m_cNode.m_zName;
				break;
			}
		}
	}
	
	/* Start transferring the files */
	String zSource;
	String zDest;
	for( int i = 0; pcData->FindString( "file/path", &zSource, i ) == 0; i++ )
	{
		Path cPath( zSource );
		zDest = zDestPath;
		if( zDest.Length() > 0 && zDest[zDest.Length()-1] != '/' ) zDest += "/";
		zDest += cPath.GetLeaf();
		DEBUG( "GUI: Drag & drop %s to %s\n", zSource.c_str(), zDest.c_str() );
		
		/* Ask the Server object to start the transfer */
		/* Need to determine if it is a file or a dir first though */
		FileReference cFileRef( zSource );
		struct stat sStat;
		cFileRef.GetStat( &sStat );
		if( S_ISDIR( sStat.st_mode ) ) {
			m_pcServer->SendLocalDir( zSource, zDest );
		} else {
			m_pcServer->SendLocalFile( zSource, zDest );
		}
	}
}

/** \brief DragSelection callback.
 * Handles the results of a drag selection.
 */
void RemoteIconView::DragSelection( Point cStartPoint )
{
	/* Most of this is adapted from IconDirectoryView::DragSelection */
	
	if( m_pcServer == NULL )
	{
		DEBUG( "DragSelection with m_pcServer== NULL\n" );
		return;
	}
	
	String zBase = m_pcServer->GetServerAddress();
	Message cMsg( 1234 );	/* Message code isn't important */
	
	/* Add all selected icons to the message, and find the icon under the mouse. */
	int nCount = 0;
	int nLastSelected = -1;
	Point cIconPos = Point( 0,0 );
	for( uint i = 0; i < GetIconCount(); i++ ) {
		if( GetIconSelected( i ) ) {
			RemoteIconData* pcData = (RemoteIconData*)GetIconData( i );
//			DEBUG( "   %s\n", pcData->m_cNode.m_zPath.c_str() );
			cMsg.AddString( "remote-file/path", pcData->m_cNode.m_zPath );
			cMsg.AddString( "server", zBase );
			cMsg.AddBool( "is_dir", pcData->m_cNode.m_bIsDir );
			
			if( Rect( GetIconPosition( i ), GetIconPosition( i ) + GetIconSize() ).DoIntersect( cStartPoint ) ) {
				cIconPos = GetIconPosition( i );
			}
			
			nLastSelected = i;
			nCount++;
		}
	}
	if( nCount == 0 ) return;
	
	/* Create a drag&drop icon */
	Bitmap cBitmap( (int)GetIconSize().x + 1, (int)GetIconSize().y + 1, CS_RGB32, Bitmap::ACCEPT_VIEWS | Bitmap::SHARE_FRAMEBUFFER );
	View* pcView = new View( Rect( Point(0,0), GetIconSize() ), "temp" );
	cBitmap.AddChild( pcView );
	
	Image* pcIcon = NULL;
	String zLabel;
	if( nCount == 1 ) {
		/* Only one file selected; use its icon */
		zLabel = GetIconString( nLastSelected, 0 );
		pcIcon = GetIconImage( nLastSelected );
	} else {
		/* Multiple files selected; use dir icon */
		zLabel.Format( "%i items", nCount );	/* TODO: localise the string */
		/* TODO: Fall back to resource if file isn't present */
		File* pcFile = new File( "/system/icons/folder.png" );
		BitmapImage* pcBitmapImage = new BitmapImage( pcFile );
		Point cSize = (GetView() == VIEW_LIST || GetView() == VIEW_DETAILS ? Point( 24,24 ) : Point( 48,48 ));
		if( pcBitmapImage->GetSize() != cSize ) pcBitmapImage->SetSize( cSize );
		pcIcon = pcBitmapImage;
	}
	if( pcIcon ) {
		RenderIcon( zLabel, pcIcon, pcView, Point(0,0) );
	}
	cBitmap.Sync();
	if( nCount != 1 ) delete( pcIcon );
	
	BeginDrag( &cMsg, cStartPoint - cIconPos, &cBitmap );
}


/** \brief Create & open a context menu.
 *  This code is similar to os::IconDirectoryView::OpenContextMenu().
 */
void RemoteIconView::OpenContextMenu( Point cPosition, bool bMouseOverIcon )
{
	if( m_pcServer == NULL ) return;	/* Not connected, so ignore the rightclick. */
//	DEBUG( "RemoteIconView: OpenContextMenu( %.0f,%.0f, %s )\n", cPosition.x, cPosition.y, bMouseOverIcon?"true":"false" );
	Message cTmp;
	int nNumSelected = 0;	/* Number of icons that are currently selected */
	for( uint i = 0; i < GetIconCount(); i++ ) {
		if( GetIconSelected( i ) ) nNumSelected++;
	}
	
	if( bMouseOverIcon ) {
		if( m_pcFileContextMenu == NULL ) {
			/* Context menu hasn't been created yet; create it. */
			m_pcFileContextMenu = new Menu( Rect(), "remote_file_menu", ITEMS_IN_COLUMN );
			m_pcFileContextMenu->AddItem( "Rename...", new Message( M_REMOTE_RENAME ) );
			m_pcFileContextMenu->AddItem( "Delete", new Message( M_REMOTE_DELETE ) );
			m_pcFileContextMenu->AddItem( "Create new folder...", new Message( M_REMOTE_MKDIR ) );
			m_pcFileContextMenu->SetTargetForItems( this );
		}
		m_pcFileContextMenu->GetItemAt(0)->SetEnable( nNumSelected == 1 );	/* 'Rename' is only valid if a single icon is selected */
		m_pcFileContextMenu->Open( ConvertToScreen( cPosition ) );
	} else {
		if( m_pcDirContextMenu == NULL ) {
			/* Context menu hasn't been created yet; create it. */
			m_pcDirContextMenu = new Menu( Rect(), "remote_dir_menu", ITEMS_IN_COLUMN );
			m_pcDirContextMenu->AddItem( "Create new folder...", new Message( M_REMOTE_MKDIR ) );
			m_pcDirContextMenu->SetTargetForItems( this );
		}
		m_pcDirContextMenu->Open( ConvertToScreen( cPosition ) );
	}
}


void RemoteIconView::FrameSized( const Point& cDelta )
{
//	DEBUG( "RemoteView: FrameSized( %.0f, %.0f ). New frame: %.0f %.0f %.0f %.0f\n", cDelta.x, cDelta.y, GetFrame().left, GetFrame().top, GetFrame().right, GetFrame().bottom );
}

