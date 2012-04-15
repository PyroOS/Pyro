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

#ifndef __COMMTHREAD_H_
#define __COMMTHREAD_H_

#include <vector>
#include <util/looper.h>
#include <util/message.h>
#include <util/messenger.h>
#include <util/string.h>

class CommThread : public os::Looper
{
public:	
	CommThread( const os::Messenger& cTarget );
	virtual void HandleMessage( os::Message* pcMessage );
	virtual bool Idle();
	virtual bool OkToQuit();
private:
	#define SLEEP 1000000

	void SendMessage( const os::String& cName );


	enum state_t { S_START, S_STOP };

	os::Messenger m_cTarget;
	state_t m_eState;
	uint m_OldTime;

	bool m_bWaitForAddReply;

	std::vector < os::String > m_cAddMap;
};

#endif
