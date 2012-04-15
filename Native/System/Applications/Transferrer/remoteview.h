#ifndef __REMOTEICONVIEW_H__
#define __REMOTEICONVIEW_H__

#include <gui/iconview.h>
#include <util/datetime.h>
#include <gui/image.h>
#include <gui/bitmap.h>
#include <gui/stringview.h>

#include <vector>

#include "messages.h"
#include "remotenode.h"

using namespace os;

class Server;


/** \brief RemotIconData Class.
 * This extends the IconData class to contain a RemoteNode object.
 */
class RemoteIconData : public IconData {
public:
	RemoteNode m_cNode; /**< RemoteNode object corresponding to this icon. */
};

/** \brief RemoteIconView
 * This class provides functionality for request directory information from the
 * remote server and to copy items from the remote directory into the local directory.
 *
 * \todo Once this is under CVS all ifdef 0 code should be removed.
 * \todo This should be renamed so that it doesn't sound like it's unique to FTP 
 * servers only.
 */
class RemoteIconView : public IconView
{
public:
	RemoteIconView( const Rect& cFrame, const String& zName );
	~RemoteIconView();
	
	void SetServer( Server* pcServer );
	Server* GetServer();
	void SetPath( const String& zPath );
	String GetPath();
	void ShowMessage( const String& zMsg );
	void HideMessage();
	
	void Update();
	void SetContents( std::vector< RemoteNode >* pacContents, bool bInitial, bool bFinal );
	Image* GetNodeImage( RemoteNode* pcNode, bool bSmall );
	

	/* os::IconView methods */
	void AttachedToWindow();
	void SetDirChangeMsg( Message* pcMsg );
	void Invoked( uint nIcon, IconData* pcData );
	void MouseUp( const Point& cPoint, uint32 nButtons, Message* pcData );
	void KeyDown( const char* pzString, const char* pzRawString, uint32 nQualifiers );

	void DragSelection( Point cStartPoint );
	void OpenContextMenu( Point cPosition, bool bMouseOverIcon );
	
	void HandleMessage( Message* pcMessage );
	
	void FrameSized( const Point& cDelta );
	
private:

	/** \brief Server object that the remote view connects to.
	 * Server object provides GetChildren(), StartTransfer(), 
	 * PauseTransfer(), CancelTransfer() etc 
	 */
	Server* m_pcServer;

	/** \brief Path of current directory.
	 * This contains the absolute path from remote server root (eg /home/user ) */
	String m_zPath;
	
	/** \brief Flag for whether an update is pending or not.
	 * If true, we are waiting for the server to give a directory listing
	 */
	bool m_bUpdatePending;
	
	/** \brief Sent to the target when user changes directory. */
	Message* m_pcDirChangedMsg;
	
//	StringView* m_pcStringView;		/* Used for displaying "Not connected" message, etc */	/* TODO: crashes when quitting !? */

	/** \brief The context menu that is displayed when the user right-clicks on an icon.
	 *  Created in OpenContextMenu(). */
	Menu* m_pcFileContextMenu;
	/** \brief The context menu that is displayed when the user right-clicks on blank background.
	 *  Created in OpenContextMenu(). */
	Menu* m_pcDirContextMenu;
};


#endif  /* __REMOTEICONVIEW_H__ */

