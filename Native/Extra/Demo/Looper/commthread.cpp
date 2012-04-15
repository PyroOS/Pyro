// EFileBrowser	 (C)opyright 2006 Jonas Jarvoll
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include <atheos/time.h>
#include <util/datetime.h>

#include "commthread.h"
#include "messages.h"

using namespace os;
using namespace std;

CommThread :: CommThread( const Messenger& cTarget ): Looper( "comm_worker" )
{
	m_cTarget = cTarget;
	m_bWaitForAddReply = false;
	m_eState = S_STOP;
}

bool CommThread :: Idle()
{   
	if( m_eState == S_START )
	{
		if ( ( get_system_time() - m_OldTime ) > SLEEP )
		{
			SendMessage( DateTime::Now().GetDate() );
			m_OldTime = get_system_time();
		}

		return true;
	}

	return false;
}

bool CommThread :: OkToQuit()
{
	return true;
}

void CommThread :: HandleMessage( Message * pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case MSG_TOLOOPER_START:
		{
			if( m_eState == S_STOP )
			{
				m_eState = S_START;
				m_OldTime = get_system_time() - SLEEP - 1;
			}
			break;
		}
		case MSG_TOLOOPER_STOP:
		{
			if( m_eState == S_START )
				m_eState = S_STOP;
			break;
		}
		case MSG_TOLOOPER_ACK:
		{
			m_bWaitForAddReply = false;
			if( m_cAddMap.empty() == false )
			{
				Message cMsg( MSG_FROMLOOPER_NEW_MESSAGE );
				int nCount = 0;

				while( m_cAddMap.empty() == false )
				{
					std::vector < String >::iterator i = m_cAddMap.begin();

					cMsg.AddString( "name", ( *i ));

					m_cAddMap.erase( i );

					if( ++nCount > 5 )
						break;	
				}

				m_bWaitForAddReply = true;
				m_cTarget.SendMessage( &cMsg );
			}
			break;
		}
		default:
			Looper::HandleMessage( pcMessage );
			break;
	}
}

void CommThread :: SendMessage( const String& cName )
{
	try
	{
		if( m_bWaitForAddReply )
		{
			m_cAddMap.push_back( cName );
		}
		else
		{
			Message cMsg( MSG_FROMLOOPER_NEW_MESSAGE );
			cMsg.AddString( "name", cName );

			m_cTarget.SendMessage( &cMsg );
			m_bWaitForAddReply = true;
		}
	}
	catch( ... ) { }
}
