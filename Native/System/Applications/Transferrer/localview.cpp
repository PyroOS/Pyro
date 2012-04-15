#include <sys/stat.h>
#include <util/message.h>

#include "localview.h"
#include "server.h"

/** \brief LocalIconView Constructor.
 * This simply initializes the current server to NULL.
 *
 * \todo zName is currently ignored because of a limitation with libsyllable (to be fixed in later version).
 */
LocalIconView::LocalIconView(const os::Rect& cRect, const String& zName ) : os::IconDirectoryView(cRect,getenv("HOME"),os::CF_FOLLOW_ALL)
{
	m_pcServer = NULL;
}

/** \brief Set the server for the local view.
 * This sets the Server object for the local view.
 *
 * \param pcServer The server to set for the local view.
 */
void LocalIconView::SetServer( Server* pcServer )
{
	m_pcServer = pcServer;
}

/** \brief Retrieve Server associated with the local view.
 * Returns a pointer to the Server object associated with the local view.
 */
Server* LocalIconView::GetServer()
{
	return( m_pcServer );
}

/** \brief Mouse Movement Handler callback.
 * This handles mouse movements and dragging.
 */
void LocalIconView::MouseMove( const Point& cPos, int nCode, uint32 nButtons, Message* pcData )
{
	/* HACK to make the IconDirView highlight a folder when something is dragged over it */
	int nCount, nType;
	if( pcData != NULL && pcData->GetNameInfo( "remote-file/path", &nType, &nCount ) == 0 && nType == T_STRING && nCount > 0 ) {
		/* We add a 'fake' "file/path" member to the message to trick the IconDirView into highlighting folders when something is dragged over them */
		pcData->AddString( "file/path", "" );
	}
	IconDirectoryView::MouseMove( cPos, nCode, nButtons, pcData );
}

/** \brief Mouse Up Handler callback.
 * This handles when the mouse button is released.
 */
void LocalIconView::MouseUp( const Point& cPosition, uint32 nButtons, Message* pcData )
{
	if( pcData == NULL || m_pcServer == NULL ) {
		IconDirectoryView::MouseUp( cPosition, nButtons, pcData );
		return;
	}

	int nCount, nType;
	pcData->GetNameInfo( "remote-file/path", &nType, &nCount );
	if( nType != T_STRING || nCount == 0 ) {
		IconDirectoryView::MouseUp( cPosition, nButtons, pcData );
		return;
	}
	String zSourceServer;
	if( pcData->FindString( "server", &zSourceServer ) != 0 ) {
		IconDirectoryView::MouseUp( cPosition, nButtons, pcData );
		return;
	}
	StopScroll();
	
	if( zSourceServer != m_pcServer->GetServerAddress() ) {
		DEBUG( "Transferrer:LocalIconView: dropped file is from another server!\n" );
		return;
	}
	
	/* Check if there is a directory icon under the mouse. If so, we transfer the file into that directory.
	   Otherwise, we transfer it into this directory. */
	Path cDestDir = Path( GetPath() );
	for( uint i = 0; i < GetIconCount(); i++ ) {
		if( Rect( GetIconPosition( i ), GetIconPosition( i ) + GetIconSize() ).DoIntersect( ConvertToView( cPosition ) ) ) {
			/* Found an icon under the mouse */
			DirectoryIconData* pcData = static_cast<DirectoryIconData*>( GetIconData( i ) );
			if( S_ISDIR( pcData->m_sStat.st_mode ) ) {
				/* Found a dir under the mouse. Add it to the destination path */
				cDestDir.Append( pcData->m_zPath.c_str() );
				break;
			}
		}
	}
	
	/* Start transferring the files */
	String zSource;
	for( int i = 0; pcData->FindString( "remote-file/path", &zSource, i ) == 0; i++ ) {
		bool bIsDir = false;
		pcData->FindBool( "is_dir", &bIsDir );
		Path cDestPath = cDestDir;
		String zFileName;
		int n = zSource.str().find_last_of( '/' );
		if( n == String::npos ) { /* No '/' in the filename!? */
			zFileName = zSource;
		} else {
			zFileName = zSource.str().substr( n );
		}
		cDestPath.Append( zFileName );

		/* Ask the Server object to start the transfer */
		if( !bIsDir ) {
			m_pcServer->GetRemoteFile( zSource, cDestPath.GetPath().c_str() );
		} else {
			m_pcServer->GetRemoteDir( zSource, cDestPath.GetPath().c_str() );
		}
	}
}

void LocalIconView::FrameSized( const Point& cDelta )
{
//	DEBUG( "LocalView: FrameSized( %.0f, %.0f ). New frame: %.0f %.0f %.0f %.0f\n", cDelta.x, cDelta.y, GetFrame().left, GetFrame().top, GetFrame().right, GetFrame().bottom );
}

