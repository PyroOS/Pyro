// IView (C)opyright 2008 Jonas Jarvoll
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

#include "iconbutton.h"
#include <gui/slider.h>

#include "common.h"
#include "messages.h"
#include "iconbar.h"
#include "main.h"

using namespace os;

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

class MiniSlider : public Slider
{
public:
	MiniSlider( Message* msg ) : Slider( Rect(), "", msg, TICKS_BELOW, 0 ) 
	{
	}

	~MiniSlider() 
	{
	}

	Point GetPreferredSize( bool bLargest ) const
	{
		Point slider = Slider::GetPreferredSize( bLargest );
		slider.x = 50;
		return slider;
	}
	
};

class IconBar :: _Private
{
public:

	_Private()
	{	
		IconCounter = 0;
	}	

	int IconCounter;

	MiniSlider* _TimeoutSlider;
	MiniSlider* _ZoomSlider;
	ToggleButton* _Slideshow;
	ToggleButton* _Fullscreen;
	ToggleButton* _FitToWindow;
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////
IconBar :: IconBar() : View( Rect(), "icon_bar", CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_BOTTOM )
{	
	// Create the private class
	_m = new _Private();

	// Create the actual iconbar
	_AddIcon( new Message( MSG_DELETE ), LoadImage( "delete.png" ) );
	_AddSeparator();
	_AddIcon( new Message( MSG_SET_DESKTOP ), LoadImage( "desktop.png" ) );
	_AddSeparator();
	_m->_TimeoutSlider = new MiniSlider( new Message( MSG_TIMEOUT_SLIDER ) );
	_m->_TimeoutSlider->SetMinMax( 1, 10 );
	_AddView( _m->_TimeoutSlider );
	_AddIcon( new Message( MSG_SLIDESHOW_STOP ), LoadImage( "stop.png" ) );
	_m->_Slideshow = new ToggleButton( Rect(), "", new Message( MSG_SLIDESHOW_START ), LoadImage( "play.png" ), NULL );
	_AddView( _m->_Slideshow );
	_AddSeparator();
	_m->_Fullscreen = new ToggleButton( Rect(), "", new Message( MSG_FLIP_FULLSCREEN ), LoadImage( "fullscreen.png" ), LoadImage( "windowed.png" ) );
	_AddView( _m->_Fullscreen );
	 _AddSeparator();
	_m->_ZoomSlider = new MiniSlider( new Message( MSG_ZOOM_SLIDER ) );
	_m->_ZoomSlider->SetMinMax( 10.0f, 200.0f );
	_AddView( _m->_ZoomSlider );
	_m->_FitToWindow = new ToggleButton( Rect(), "", new Message( MSG_VIEW_FIT ), LoadImage( "fit.png" ), NULL );
	_AddView( _m->_FitToWindow  );
	_AddIcon( new Message( MSG_VIEW_100 ), LoadImage( "100.png" ) );
	_AddIcon( new Message( MSG_ZOOM_OUT ), LoadImage( "zoom-out.png" ) );
	_AddIcon( new Message( MSG_ZOOM_IN ), LoadImage( "zoom-in.png" ) );
	_AddSeparator();
	_AddIcon( new Message( MSG_ROTATE_CW ), LoadImage( "rotate_cw.png" ) );
	_AddIcon( new Message( MSG_ROTATE_CCW ), LoadImage( "rotate_ccw.png" ) );
	_AddSeparator();
	_AddIcon( new Message( MSG_NEXT_IMAGE ), LoadImage( "go-next.png" ) );
	_AddIcon( new Message( MSG_PREV_IMAGE ),  LoadImage( "go-previous.png" ) );
	_AddSeparator();
	_AddIcon( new Message( MSG_OPEN ), LoadImage( "open.png" ) );
}

IconBar :: ~IconBar()
{
	delete _m->_TimeoutSlider;
	delete _m->_ZoomSlider;
	delete _m;
}

void IconBar :: AllAttached()
{
	SetTarget( NView::GetAppWindow() );
}

Point IconBar :: GetPreferredSize( bool bLargest ) const
{
	return Point( COORD_MAX, 24 );
}

void IconBar :: FrameSized( const Point& cDelta )
{
	View::FrameSized( cDelta );

	Rect bounds = GetBounds();

	// Calculate how much additional space between the icons
	// we can afford with
	double extra_distance = 0.0f;
	if( bounds.Width() > _GetOptimalSize() )
	{
		extra_distance = ( bounds.Width() - _GetOptimalSize() ) / ( _m->IconCounter - 1 );

		// Make sure it is not a too large distance
		if( extra_distance > 4.0f )
			extra_distance = 4.0f;
	}

	// Calculate starting point for the first icon
	double x = ( bounds.Width() - _GetOptimalSize() - extra_distance * ( _m->IconCounter - 1 ) ) / 2.0f;

	for( int i = 0; ; i++ )
	{
		View* v = GetChildAt( i );
		if( v == NULL )
			break;

		int icon_width = (int) v->GetPreferredSize( false ).x;

		Rect r( x, 2, x + icon_width, 22 );
		v->SetFrame( r );
		x += icon_width + extra_distance;
	}
}

void IconBar :: SetTarget( const os::Handler* pcHandler, const Looper* pcLooper )
{
	for( int i = 0 ; ; i++ )
	{
		View* v = GetChildAt( i );

		if( v == NULL )
			break;

		Invoker* t = dynamic_cast<Invoker*>( v );

		if( t != NULL )
			t->SetTarget( pcHandler, pcLooper );
	}

}

void IconBar :: SetZoomSlider( double zoom )
{
	double slider;

	if( zoom <= 1.0f )
		slider = zoom * 100.0f;
	else
		slider = (100.0f + zoom * ( 100.0f / 4.0f ) );

	_m->_ZoomSlider->SetValue( slider, false );
}

double IconBar :: GetZoomSlider()
{
	float slider = _m->_ZoomSlider->GetValue().AsFloat();
	double zoom = 1.0f;

	if( slider <= 100.0f )
		zoom = slider / 100.0f;
	else
		zoom = 1.0f + ( ( slider - 100.0f ) * 4.0f ) / 100.0f;

	if( zoom < 0.1f )
		zoom = 0.1f;
	if( zoom > 4.0f )
		zoom = 4.0f;

	return zoom;
}

void IconBar :: SetTimeoutSlider( int timeout )
{
	_m->_TimeoutSlider->SetValue( timeout );
}

int IconBar :: GetTimeoutSlider()
{
	return _m->_TimeoutSlider->GetValue().AsInt32();
}

void IconBar :: SetSlideshow( bool value )
{
	_m->_Slideshow->SetSelected( value );
	Invalidate( _m->_Slideshow->GetBounds() );
	Flush();
}

void IconBar :: SetFullscreen( bool value )
{
	_m->_Fullscreen->SetSelected( value );
	Invalidate( _m->_Fullscreen->GetBounds() );
	Flush();
}

void IconBar :: SetFitToWindow( bool value )
{
	_m->_FitToWindow->SetSelected( value );
	Invalidate( _m->_FitToWindow->GetBounds() );
	Flush();
}

////////////////////////////////////////////////////////
//
// P R I V A T E
//
////////////////////////////////////////////////////////
void IconBar :: _AddIcon( Message* msg, Image* img )
{
	_AddView( new IconButton( Rect(), "", msg, img, true ) );	
}

void IconBar :: _AddIcon( Message* msg, Image* img, Image* sel_img )
{
	_AddView( new ToggleButton( Rect(), "", msg, img, sel_img ) );	
}

void IconBar :: _AddSeparator()
{
	_AddView( new IconButton( Rect(), "", NULL, LoadImage( "separator.png" ), false ) );	
}

void IconBar :: _AddView( View* view )
{
	AddChild( view );	
	_m->IconCounter++;
}

int IconBar :: _GetOptimalSize()
{
	int width = 0;

	for( int i = 0; ; i++ )
	{
		View* v = GetChildAt( i );
		if( v == NULL )
			break;

		width += (int)v->GetPreferredSize( false ).x;
	}

	return width;
}
