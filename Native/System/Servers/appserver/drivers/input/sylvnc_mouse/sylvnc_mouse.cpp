/*
 *  The AtheOS application server
 *  Copyright (C) 1999 - 2001 Kurt Skauen
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

#include <atheos/kernel.h>

#include <inputnode.h>

#include <appserver/protocol.h>
#include <util/message.h>

#define SYLVNC_MOUSE_EVENT_SIG 0x564E434D /* VNCM */

struct SylVNCMouseEvent
{
        int nSignature;
	int nButtons;
	int x;
	int y;
};

static int g_nDevice = -1;

class SylVNCMouseDriver : public InputNode
{
public:
    SylVNCMouseDriver();
    ~SylVNCMouseDriver();

    virtual bool Start();
    virtual int  GetType() { return( IN_MOUSE ); }
  
private:
    static int32 EventLoopEntry( void* pData );
    void EventLoop();
    void DispatchEvent( int nDeltaX, int nDeltaY, uint32 nButtons );
  
    thread_id m_hThread;
    int	    m_nMouseDevice;
};



//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

SylVNCMouseDriver::SylVNCMouseDriver()
{
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

SylVNCMouseDriver::~SylVNCMouseDriver()
{
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

void SylVNCMouseDriver::DispatchEvent( int nDeltaX, int nDeltaY, uint32 nButtons )
{
    Point cDeltaMove( nDeltaX, nDeltaY );
    uint32 	nButtonFlg;
    static uint32 nLastButtons = 0;
  
    nButtonFlg	= nButtons ^ nLastButtons;
    nLastButtons	= nButtons;

    if ( nButtonFlg != 0 ) {
	Message* pcEvent;
    
	if ( nButtonFlg & 0x01 ) {
	    if ( nButtons & 0x01 ) {
		pcEvent = new Message( M_MOUSE_DOWN );
//		dbprintf( "Mouse Button 1 Down\n");
	    } else {
		pcEvent = new Message( M_MOUSE_UP );
//		dbprintf( "Mouse Button 1 Up\n");
	    }
	    pcEvent->AddInt32( "_button", 1 );
	    pcEvent->AddInt32( "_buttons", 1 ); // To be removed
	    EnqueueEvent( pcEvent );
	}
	if ( nButtonFlg & 0x02 ) {
	    if ( nButtons & 0x02 ) {
		pcEvent = new Message( M_MOUSE_DOWN );
//		dbprintf( "Mouse Button 2 Down\n");
	    } else {
		pcEvent = new Message( M_MOUSE_UP );
//		dbprintf( "Mouse Button 2 Up\n");
	    }
	    pcEvent->AddInt32( "_button", 2 );
	    pcEvent->AddInt32( "_buttons", 2 ); // To be removed
	    EnqueueEvent( pcEvent );
	}
    }
    if ( nDeltaX != 0 || nDeltaY != 0 ) {
	Message* pcEvent = new Message( M_MOUSE_MOVED );
	pcEvent->AddPoint( "delta_move", cDeltaMove );
	EnqueueEvent( pcEvent );
    }
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

void SylVNCMouseDriver::EventLoop()
{
//	int nByteNum = 0;
//	int x = 0;
	int oldX = 0;
//	int y = 0;
	int oldY = 0;
//	int j = 0;
//	uint32 nButtons = 0;
	bool bGotNewData = false;
	int sig = SYLVNC_MOUSE_EVENT_SIG;
	int nData;
	struct SylVNCMouseEvent sEvent = {0,0,0,0};
	for (nData = 0; nData != sig; read(g_nDevice, &nData, sizeof(int)))
	{
	}
	sEvent.nSignature = nData;
	read(g_nDevice, &nData, sizeof(int));
	sEvent.nButtons = nData;
	read(g_nDevice, &nData, sizeof(int));
	sEvent.x = nData;
	read(g_nDevice, &nData, sizeof(int));
	sEvent.y = nData;
	bGotNewData = true;
	for (;;)
	{	    
		snooze(1);
		if ( bGotNewData == false)
		{
			if(read( g_nDevice, &sEvent, sizeof(struct SylVNCMouseEvent)) < 1 )
			{
//				dbprintf( "Error: SylVNCMouseDriver::EventLoop() failed to read from device\n" );
				snooze(100);
				continue;
			}
			else
			{
				bGotNewData = true;
			}
		}
		if ( bGotNewData )
		{
			if (sEvent.nSignature != sig)
			{
				for (nData = 0; nData != sig; read(g_nDevice, &nData, sizeof(int)))
				{
				}
				sEvent.nSignature = nData;
				read(g_nDevice, &nData, sizeof(int));
				sEvent.nButtons = nData;
				read(g_nDevice, &nData, sizeof(int));
				sEvent.x = nData;
				read(g_nDevice, &nData, sizeof(int));
				sEvent.y = nData;
				bGotNewData = true;
			}
			else
			{
				if ((sEvent.y != oldY) || (sEvent.x != oldX))
				{
					Point newMousePos(sEvent.x, sEvent.y);
					SetMousePos(newMousePos);
				}
				DispatchEvent( 0, 0, sEvent.nButtons );
				oldX = sEvent.x;
				oldY = sEvent.y;
				bGotNewData = false;
				sEvent.nSignature = 0;
				sEvent.nButtons = 0;
				sEvent.x = 0;
				sEvent.y = 0;
			}
		}
	}
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

int32 SylVNCMouseDriver::EventLoopEntry( void* pData )
{
    SylVNCMouseDriver* pcThis = (SylVNCMouseDriver*) pData;
  
    pcThis->EventLoop();
    return( 0 );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

bool SylVNCMouseDriver::Start()
{
    thread_id hEventThread;
    hEventThread = spawn_thread( "sylvnc_mouse_event_thread", (void*)EventLoopEntry, 120, 0, this );
    resume_thread( hEventThread );
    return( true );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

extern "C" bool init_input_node()
{
    g_nDevice = open( "/dev/input/sylvnc_mouse", O_RDWR );
    if ( g_nDevice < 0 ) {
	dbprintf( "Error: SylVNC Mouse driver failed to open device: %s\n", strerror( errno ) );
	return( false );
    }
    return( true );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

extern "C" int uninit_input_node()
{
    if ( g_nDevice >= 0 ) {
	close( g_nDevice );
    }
    return( 0 );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

extern "C" int get_node_count()
{
    return( 1 );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

extern "C" InputNode* get_input_node( int nIndex )
{
//    if ( nIndex != 0 ) {
//	dbprintf( "SylVNC Mouse driver: get_input_node() called with invalid index %d\n", nIndex );
//	return( NULL );
//    }
    return( new SylVNCMouseDriver() );
}

