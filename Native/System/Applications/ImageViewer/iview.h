//  AView (C)opyright 2005 Kristian Van Der Vliet
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

#ifndef __IVIEW_H_
#define __IVIEW_H_

#include <gui/view.h>
#include <gui/image.h>

class IView : public os::View
{
public:
	IView();
	~IView();

	void Paint( const os::Rect& cUpdate );
	void WheelMoved( const os::Point& delta );
	
	void MouseDown( const os::Point& cPos, uint32 nButtons );
	void MouseUp( const os::Point& cPos, uint32 nButtons, os::Message* pcData );
	void MouseMove( const os::Point& cNewPos, int nCode, uint32 nButton, os::Message* pcData );

	void SetImage( os::BitmapImage* pcImage );

	os::Point FindOptimalSize( const os::Point& cBound, bool& HorizNeed, bool& VertNeed );

	void UpdateViewForScrollbars( double horiz_pos, double vert_pos );

	void GetDataForHorizScrollbar( double& pos, double& prop );
	void GetDataForVertScrollbar( double& pos, double& prop );

	void SetFit();
	void Set100();

	void SetZoom( double zoom );
	double GetZoom();
	
private:	
	class _Private;
	_Private* _m;
};

#endif

