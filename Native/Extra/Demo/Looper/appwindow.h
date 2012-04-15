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

#ifndef __APPWINDOW_H_
#define __APPWINDOW_H_

#include <util/message.h>
#include <util/string.h>
#include <util/resources.h>
#include <util/settings.h>
#include <util/messenger.h>

#include <gui/window.h>
#include <gui/textview.h>
#include <gui/button.h>

#include "commthread.h"

class AppWindow : public os::Window
{
public:
	AppWindow( const os::Rect& cFrame );
	~AppWindow();
	virtual void HandleMessage( os::Message* pcMessage );
	virtual bool OkToQuit( void );
	virtual void FrameSized( const os::Point& delta );


private:
	void AddStringToTextView( os::String name );

	void Layout();

	os::Button* m_Start;
	os::Button* m_Stop;
	os::TextView* m_Text;

	CommThread* m_CommThread;
};

#endif /* __APPWINDOW_H_ */


