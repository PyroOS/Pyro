#ifndef _LOCAL_VIEW_H
#define _LOCAL_VIEW_H

#include <gui/icondirview.h>

class Server;

using namespace os;


/** \brief LocalIconView shows a view of the current local directory.
 * This extends os::IconDirectoryView in order to support viewing the local
 * directory and storing the current server associated with the view.
 *
 */
class LocalIconView : public os::IconDirectoryView 
{
public:
	LocalIconView(const os::Rect&, const String& zName );
	
	void SetServer( Server* pcServer );
	Server* GetServer();
	
	void MouseUp( const Point& cPosition, uint32 nButtons, Message* pcData );
	void MouseMove( const Point& cPos, int nCode, uint32 nButtons, Message* pcData );

	void FrameSized( const Point& cDelta );
	
private:
	/** \brief Current Server object associated with the local view. */
	Server* m_pcServer;
};

#endif








