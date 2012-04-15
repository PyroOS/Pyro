// ImageView (C)opyright 2008 Jonas Jarvoll
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

#ifndef _ICONBAR_H_
#define _ICONBAR_H_

#include <gui/view.h>
#include <gui/image.h>

class IconBar : public os::View
{
public:
	IconBar();
	~IconBar();

	virtual os::Point GetPreferredSize( bool bLargest ) const;
	virtual void FrameSized( const os::Point& cDelta );
	
	void AllAttached();

	void SetTarget( const os::Handler* pcHandler, const os::Looper* pcLooper = NULL);

	void SetZoomSlider( double zoom );
	double GetZoomSlider();

	void SetTimeoutSlider( int zoom );
	int GetTimeoutSlider();

	void SetSlideshow( bool value );
	void SetFullscreen( bool value );
	void SetFitToWindow( bool value );

private:
	void _AddIcon( os::Message* msg, os::Image* img );
	void _AddIcon( os::Message* msg, os::Image* img, os::Image*sel_img );
	void _AddSeparator();
	void _AddView( os::View* view );
	int _GetOptimalSize();

	class _Private;
	_Private* _m;
};

#endif

