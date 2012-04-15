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

#ifndef _LAYOUTER_H_
#define _LAYOUTER_H_

#include <gui/view.h>
#include <gui/image.h>

class Layouter : public os::View
{
public:
	static Layouter* GetInstance();

	Layouter();
	~Layouter();

	void MouseMove( const os::Point& cNewPos, int nCode, uint32 nButtons, os::Message* pcData );

	void FrameSized( const os::Point& cDelta );
	void AttachedToWindow();
	void HandleMessage( os::Message* pcMessage );
	void Paint( const os::Rect& cUpgrade );
	
	void SetFullscreenMode( bool enable );

	void SetImage( os::BitmapImage *pcImage );

	void UpdateAllViews();

	void SetZoom( double zoom );
	double GetZoom();
	float GetZoomSlider();
	void Set100();
	void SetFit();
	void IconBarFitToWindow( bool value );
	void SetScrollBars();
	void SetSlideshow( bool value );

private:
	static Layouter* m_theInstance;

	class _Private;
	_Private* _m;
};

#endif
