#ifndef CONTAINER_VIEW_H
#define CONTAINER_VIEW_H

#include <gui/layoutview.h>
#include <util/message.h>

class LocalIconView;
class RemoteIconView;
class AddressField;
class Server;

using namespace os;

#ifndef DEBUG
extern bool g_bDebug;
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif

/** \brief Main window view container.
 * This contains the local and remote directory views and displays them.
 * It also contains the address bars for the local and remote servers.
 *
 */
class ContainerView : public os::LayoutView
{
public:
	ContainerView(const os::Rect&, const os::String&);
public:
	void SetServer( Server* pcServer );
	
	void HandleMessage( Message* pcMessage );
	
	void AllAttached();
	
	os::Point GetPreferredSize(bool) const;
private:
	
	/** \brief Remote directory listing container view. */
	RemoteIconView* remote;
	
	/** \brief Contains the current remote file path buttons. */
	AddressField* m_pcRemoteBar;

	/** \brief Local directory listing container view. */
	LocalIconView* local;   //using this so we can use drag/drop

	/** \brief Contains the current local file path buttons. */
	AddressField* m_pcLocalBar;
};

#endif	/* CONTAINER_VIEW_H */
