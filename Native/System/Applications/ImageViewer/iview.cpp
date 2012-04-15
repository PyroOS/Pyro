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

#include <gui/guidefines.h>
#include <gui/imageview.h>
#include <gui/scrollbar.h>
#include <util/application.h>

#include "iview.h"
#include "main.h"
#include "layouter.h"

using namespace os;

#define MIN( x, y ) ( x < y ? x : y )

/* 
   This class is the widget that shows the actual image. It also handles some mouse events
   such as pan the image and clicking on the image. The IView calculates the size and
   position of the scrollbars based on the current view but is not responsible for 
   showing them. The scrollbars are insteas handles by the Layouter.
   There mainly two different views of an image, either fit to window (ie. adapt the 
   image after the size of the widget) or normal view (ie. zooming is allowed).
*/

///////////////////////////////////////////////////////////////////////////////
//
// G L O B A L
//
///////////////////////////////////////////////////////////////////////////////

uint8 g_anMouseImg[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,1,1,0,1,1,0,0,0,0,0,
	0,1,3,3,1,3,3,1,3,3,1,0,0,0,0,
	0,1,3,3,3,3,3,3,3,3,1,1,0,0,0,
	0,0,1,3,3,3,3,3,3,3,1,3,1,0,0,
	0,1,1,3,3,3,3,3,3,3,3,3,1,0,0,
	1,3,3,3,3,3,3,3,3,3,3,3,1,0,0,
	1,3,3,3,3,3,3,3,3,3,3,3,1,0,0,
	0,1,3,3,3,3,3,3,3,3,3,1,0,0,0,
	0,0,1,3,3,3,3,3,3,3,3,1,0,0,0,
	0,0,0,1,3,3,3,3,3,3,1,0,0,0,0,
	0,0,0,1,3,3,3,3,3,3,1,0,0,0,0
	};

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

class IView :: _Private
{
public:
	_Private()
	{	
		_Zoom = 1.0f;	
		_Center = Point( 0, 0 );
		_Image = NULL;
		_FitWindow = true;
		_MousePressed = false;
	}

	Point ConvertToImageUnit( Point screen )
	{
		return ( Point( screen.x / _Zoom, screen.y / _Zoom ) + _Center );
	}

	Point ConvertFromImageUnit( Point image )
	{
		return Point( ( image.x - _Center.x ) * _Zoom, ( image.y - _Center.y ) * _Zoom );
	}

	void ValidateZoomWindow( Rect widget )
	{
		if( _Image == NULL )
			return;

		float iw = widget.Width();
		float ih = widget.Height();


		if( _Center.x  > _Image->GetSize().x - iw / _Zoom )
		{
			_Center.x = _Image->GetSize().x - iw / _Zoom;
		}
		if( _Center.y > _Image->GetSize().y - ih / _Zoom )
		{
			_Center.y = _Image->GetSize().y - ih / _Zoom;
		}

		if( _Center.x < 0.0f )
			_Center.x = 0.0f;
		if( _Center.y < 0.0f )
			_Center.y = 0.0f;
	}

	Rect CalculateSourceRect( Rect dst )
	{
		Rect res;		

		if( _Image == NULL )
			return res;

		if( _FitWindow )
		{
			res = Rect( 0, 0, _Image->GetSize().x - 1, _Image->GetSize().y - 1 );
		}
		else
			res = Rect( _Center.x, _Center.y, _Center.x + ( dst.right - dst.left ) / _Zoom, _Center.y + ( dst.bottom - dst.top ) / _Zoom );

		return res;
	}

	double _Zoom;
 	Point _Center;
	bool _FitWindow;

	bool _MousePressed;
	Point _PreviousPoint;
	BitmapImage* _Image;
};
	
///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////
IView::IView( ) : View( Rect(), "", CF_FOLLOW_ALL, WID_WILL_DRAW )
{
	// Create the private class
	_m = new _Private();
}

IView::~IView( )
{
	delete _m;
}

void IView :: Paint( const Rect& cUpdate )
{
	SetFgColor( 0, 100, 0 );
	Rect b = GetBounds();

	if( _m->_Image == NULL )
	{
		FillRect( b );
		return;
	}

	b.Resize( 1, 1, -1, -1 ); // We dont want to overdraw the frame
	Rect src = 	_m->CalculateSourceRect( b );
	DrawBitmap( _m->_Image->LockBitmap(), src, b );
	_m->_Image->UnlockBitmap();

	// Draw a thin line around the image	
	// We do this after the bitmap has been drawn. The scaling function of bitmaps is not perfect and be wrong by one pixel
	SetFgColor( 10, 10, 10 );
	b = GetBounds();
	DrawLine( Point( b.left, b.top ), Point( b.right, b.top ) );
	DrawLine( Point( b.right, b.top ), Point( b.right, b.bottom  ) );
	DrawLine( Point( b.left, b.top ), Point( b.left, b.bottom ) );
	DrawLine( Point( b.left, b.bottom ), Point( b.right, b.bottom ) );
}

void IView :: WheelMoved( const Point& delta )
{
	Point mouse;
	uint32 buttons;

	if( _m->_Image == NULL )
		return;

	GetMouse( &mouse, &buttons);

	// in which direction did the user scroll the wheel?
	if( delta.y < 0 )
		_m->_Zoom *= 0.9f;
	else if( delta.y > 0 )
		_m->_Zoom *= 1.1f;	

	SetZoom( _m->_Zoom );
}

void IView :: MouseDown( const Point& cPos, uint32 nButtons )
{
	if( !_m->_FitWindow )	// We cannot pan the image if we are in fit to window mode (there is no point in doing so)
	{
		MakeFocus( true );
		_m->_MousePressed = true;
		_m->_PreviousPoint = cPos;

		Application::GetInstance()->PushCursor( MPTR_MONO, g_anMouseImg, 15, 15, IPoint( 8, 10 ) );
	}
}

void IView :: MouseUp( const Point& cPos, uint32 nButtons, Message* pcData )
{
	MakeFocus( false );
	_m->_MousePressed = false;
	Application::GetInstance()->PopCursor();
}


void IView :: MouseMove( const Point& cNewPos, int nCode, uint32 nButton, Message* pcData )
{
	if( _m->_MousePressed )
	{
		Point dist = cNewPos - _m->_PreviousPoint;

		dist.x /= _m->_Zoom;
		dist.y /= _m->_Zoom;

		_m->_Center -= dist;

		_m->ValidateZoomWindow( this->GetBounds() );

		_m->_PreviousPoint = cNewPos;

		// We need to tell the layouter that there as been zooming
		Layouter::GetInstance()->SetScrollBars();

		Invalidate();
		Flush();
	}
}

void IView :: SetImage( BitmapImage* pcImage )
{
	_m->_Image = pcImage;

	// Reset the view
	_m->_Zoom = 1.0f;	
	_m->_Center = Point( 0, 0 );
	_m->_FitWindow = true;
}

Point IView :: FindOptimalSize( const Point& cBound, bool& HorizNeed, bool& VertNeed  )
{
	// Assume no need for horizontal or vertical scrollbar
	HorizNeed = VertNeed = false;

	// Get size of image
	Point isize( 0, 0 );

	if( _m->_Image != NULL )
		isize = _m->_Image->GetSize();

	if( !_m->_FitWindow )
	{
		isize.x *= _m->_Zoom;
		isize.y *= _m->_Zoom;
	}

	// If the image is smaller than cBound we return the size of the image
	if( isize.x < cBound.x && isize.y < cBound.y )
		return isize;

	if( _m->_FitWindow )
	{
		// If we fit to window we need to do some aspect calculating
		if( isize.x / cBound.x < isize.y / cBound.y )
			isize = Point( isize.x / ( isize.y / cBound.y ), cBound.y );					
		else
			isize = Point( cBound.x, isize.y / ( isize.x / cBound.x ) );

		if( _m->_Image != NULL )
			_m->_Zoom = isize.x / _m->_Image->GetSize().x;	
	}
	else
	{
		HorizNeed = isize.x > cBound.x;
		VertNeed = isize.y > cBound.y;

		isize = Point( isize.x < cBound.x ? isize.x : cBound.x , isize.y < cBound.y ? isize.y : cBound.y );
	}

	return isize;
}

void IView :: SetFit()
{
	_m->_FitWindow = true;		

	// We need to tell the layouter that there as been zooming
	Layouter::GetInstance()->UpdateAllViews();

	Invalidate();
	Flush();
}

void IView :: Set100()
{
	_m->_Zoom = 1.0f;
	_m->_FitWindow = false;
	_m->_Center = Point( 0, 0 );

	// We need to tell the layouter that there as been zooming
	Layouter::GetInstance()->UpdateAllViews();

	Invalidate();
	Flush();
}

void IView :: SetZoom( double zoom )
{
	// Make sure we are still within the zoom range ( 10 % - 400 % )
	if( zoom < 0.1f )
		zoom = 0.1f;
	if( zoom > 4.0f )
		zoom = 4.0f;

	Layouter::GetInstance()->IconBarFitToWindow( false );
	_m->_Zoom = zoom;
	_m->_FitWindow = false;
	_m->ValidateZoomWindow( this->GetBounds() );

	// We need to tell the layouter that there as been zooming
	Layouter::GetInstance()->UpdateAllViews();

	Invalidate();
	Flush();
}

double IView :: GetZoom()
{
	return _m->_Zoom;
}

void IView :: UpdateViewForScrollbars( double horiz_pos, double vert_pos )
{
	Point isize ;

	// Get width of image
	if( _m->_Image != NULL )
		isize = _m->_Image->GetSize();

	isize.x *= _m->_Zoom;
	isize.y *= _m->_Zoom;

	double scale_x = ( isize.x - GetBounds().Width() ) / _m->_Zoom;
	_m->_Center.x = ( horiz_pos * scale_x ) / 1000.0f;

	double scale_y = ( isize.y - GetBounds().Height() ) / _m->_Zoom;
	_m->_Center.y = ( vert_pos * scale_y ) / 1000.0f;

	Invalidate();
	Sync();

}

void IView :: GetDataForHorizScrollbar( double& pos, double& prop )
{
	double isize = 0;

	// Get width of image
	if( _m->_Image != NULL )
		isize = _m->_Image->GetSize().x;

	isize *= _m->_Zoom;

	// Calc size of the slider
	if( isize > 0 )
		prop = ( GetBounds().Width() / isize );
	else
		prop = 1.0f;

	// Calc position of the slider
	double scale = ( isize - GetBounds().Width() ) / _m->_Zoom;
	if( scale > 0.0f )
		pos = 1000 * ( _m->_Center.x /  scale  );
	else
		pos = 0.0f;
}

void IView :: GetDataForVertScrollbar( double& pos, double& prop )
{	
	double isize = 0;

	// Get height of image
	if( _m->_Image != NULL )
		isize = _m->_Image->GetSize().y;

	isize *= _m->_Zoom;

	// Calc size of the slider
	if( isize > 0 )
		prop = ( GetBounds().Height() / isize );
	else
		prop = 1.0f;

	// Calc position of the slider
	double scale = ( ( isize -  GetBounds().Height() ) / _m->_Zoom  );
	if( scale > 0.0f )
		pos = 1000 * ( _m->_Center.y / scale );
	else
		pos = 0.0f;
}



