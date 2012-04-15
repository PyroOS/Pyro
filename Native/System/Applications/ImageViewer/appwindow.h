// ImageViewer (C)opyright 2008 Jonas Jarvoll
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

#ifndef _APPWINDOW_H_
#define _APPWINDOW_H_

#include <gui/window.h>
#include <util/message.h>

class AppWindow : public os::Window
{
public:
	AppWindow( int argc, char* argv[], const os::Rect& cFrame );
	~AppWindow();

	void HandleMessage( os::Message* pcMessage );
	void TimerTick( int nID );
	bool OkToQuit( void );

	int GetTimeoutValue();
private:
	void _StartSlideshowTimer();
	void _StopSlideshowTimer();
	void _Slideshow( bool active );
	void _LoadImage( os::String filename );
	void _FlipFullscreen();
	void _RotateImage( bool ccw ); 
	class _Private;
	_Private* m;
};

#endif
