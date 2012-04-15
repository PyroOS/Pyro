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

#include <gui/scrollbar.h>

#include "layouter.h"
#include "iconbar.h"
#include "iview.h"
#include "main.h"
#include "messages.h"

using namespace os;

/* 
   The Layouter is a Singleton and is responsible for handling the layout
   of the different widgets such as the iconbar and the image. It calculates
   the size and position for every widget depending on the current state (e.g. 
   if it is in fullscreen mode or not, if scrollbars is visible or not etc.)
*/

///////////////////////////////////////////////////////////////////////////////
//
// S I N G L E T O N
//
///////////////////////////////////////////////////////////////////////////////

Layouter* Layouter::m_theInstance = NULL;

Layouter* Layouter :: GetInstance()
{
	if( m_theInstance == NULL )
		m_theInstance = new Layouter();

	return m_theInstance;
}

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

#define ICONBAR_SENS_AREA 12 // How close to the bottom of the screen the mouse 
							 // pointer needs to be before the iconbar is visible
							 // Valid only in fullscreen mode

class Layouter :: _Private
{
public:
	_Private()
	{		
		InFullscreenMode = false;
		ShowIconbarInFullscreen = false;
		_IconBar = NULL;
		_IView = NULL;
	}

	bool InFullscreenMode;
	bool ShowIconbarInFullscreen;

	ScrollBar* _HorizBar;
	ScrollBar* _VertBar;

	IconBar* _IconBar;
	IView* _IView;
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////
Layouter :: Layouter() : View( Rect(), "", CF_FOLLOW_ALL, WID_WILL_DRAW )
{
	// Create the private class
	_m = new _Private();

	_m->_IconBar = new IconBar();
	AddChild( _m->_IconBar );
	_m->_IView = new IView();
	AddChild( _m->_IView );

	_m->_HorizBar = new ScrollBar( Rect(), "", new Message( MSG_SCROLLBAR ), 0, 1000, HORIZONTAL );
	_m->_HorizBar->SetSteps( 10.0f, 100.0f );
	AddChild( _m->_HorizBar );

	_m->_VertBar = new ScrollBar( Rect(), "", new Message( MSG_SCROLLBAR ), 0, 1000, VERTICAL );
	_m->_VertBar->SetSteps( 10.0f, 100.0f );
	AddChild( _m->_VertBar );

	// Set start values
	_m->_IconBar->SetTimeoutSlider( NView::GetAppWindow()->GetTimeoutValue() );
	_m->_IconBar->SetZoomSlider( 1.0f );

	UpdateAllViews();
}

Layouter :: ~Layouter()
{
	delete _m;
}

void Layouter :: AttachedToWindow()
{
	View::AttachedToWindow();
	_m->_HorizBar->SetTarget( this );
	_m->_VertBar->SetTarget( this );
}

void Layouter :: HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )	//Get the message code from the message
	{
		case MSG_SCROLLBAR:
		{
			double horiz = _m->_HorizBar->GetValue().AsFloat();
			double vert = _m->_VertBar->GetValue().AsFloat();
			_m->_IView->UpdateViewForScrollbars( horiz, vert );
			break;
		}

		default:
			View::HandleMessage( pcMessage );	
	}
}

void Layouter :: Paint( const Rect& cUpdate )
{
	if( _m->InFullscreenMode )
		SetFgColor( 0, 0, 0 );
	else
		SetFgColor( get_default_color( COL_NORMAL ) );

	FillRect( cUpdate );
}

void Layouter :: FrameSized( const Point& cDelta )
{
	View::FrameSized( cDelta );

	UpdateAllViews();
}

void Layouter :: MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData )
{
	// Check if the iconbar shall be visible or not in fullscreen mode
	// In none-fullscreen mode it is always visible
	if( _m->InFullscreenMode )
	{
		if( cNewPos.y > GetBounds().bottom - ICONBAR_SENS_AREA )
		{
			if( !_m->ShowIconbarInFullscreen )
			{
				_m->ShowIconbarInFullscreen = true;
				UpdateAllViews();
			}
		}

		if( cNewPos.y < _m->_IconBar->GetFrame().top )
		{
			if( _m->ShowIconbarInFullscreen )
			{
				_m->ShowIconbarInFullscreen = false;
				UpdateAllViews();
			}
		}		
	}
}

void Layouter :: SetFullscreenMode( bool enable )
{
	_m->InFullscreenMode = enable;

	_m->ShowIconbarInFullscreen = false;
	
	_m->_IconBar->SetFullscreen( enable );

	UpdateAllViews();
}

void Layouter :: UpdateAllViews()
{
	Rect iview = GetBounds();

	// Only hide the iconbar if we are in fullscreen mode
	// and user is not pointing at it
	if( _m->InFullscreenMode && !_m->ShowIconbarInFullscreen )
	{
		// Keep it simple, move the iconbar out of the way
		_m->_IconBar->SetFrame( Rect( -1, -1, -1, -1 ) );
	}
	// Always show the iconbar if we are not in fullscreen mode
	else
	{
		// Center it in the bottom of the window
		Point f = _m->_IconBar->GetPreferredSize( false );
		Rect b = GetBounds();
		Rect r;
		r.left = b.left;
		r.right = b.right;
		r.top = b.bottom - f.y;
		r.bottom = b.bottom;

		_m->_IconBar->SetFrame( r );

		iview.bottom = r.top - 1;
	}

	// How much space do we have for the image?
	bool horiz, vert;

	Point s = _m->_IView->FindOptimalSize( Point( iview.Width(), iview.Height() ), horiz, vert );

	// Set up horizontal scrollbar
	if( horiz )
	{
		double horiz_bottom = iview.bottom;
		iview.bottom -= _m->_HorizBar->GetPreferredSize( false ).y;
		_m->_HorizBar->SetFrame( Rect( iview.left, iview.bottom, iview.right,horiz_bottom ) );
		iview.bottom--;
	}
	else
		_m->_HorizBar->SetFrame( Rect( -1, -1, -1, -1  ) );

	// Set up vertical scrollbar
	if( vert )
	{
		double vert_right = iview.right;
		iview.right -= _m->_VertBar->GetPreferredSize( false ).x;
		_m->_VertBar->SetFrame( Rect( iview.right, iview.top, vert_right, iview.bottom ) );
		iview.right--;		
	}
	else
		_m->_VertBar->SetFrame( Rect( -1, -1, -1, -1  ) );

	// As we have added the scrollbars we need to recalculate the size (but we ignore the scrollbars)
	if( horiz || vert )
		s = _m->_IView->FindOptimalSize( Point( iview.Width(), iview.Height() ), horiz, vert );

	Point center = Point( iview.left + iview.Width() / 2, iview.top + iview.Height() / 2 );
	Rect e;
	e.left =  center.x - s.x / 2;
	e.right = center.x + s.x / 2;
	e.top =  center.y - s.y / 2;
	e.bottom = center.y + s.y / 2;
	
	_m->_IView->SetFrame( e );

	_m->_IView->Invalidate();
	_m->_IView->Flush();

	// Update the zoom slider
	_m->_IconBar->SetZoomSlider( _m->_IView->GetZoom() );

	// Update the scrollbars
	SetScrollBars();

	Invalidate();
	Flush();
}

void Layouter :: SetImage( BitmapImage *pcImage )
{
	_m->_IView->SetImage( pcImage );
	_m->_IconBar->SetFitToWindow( true );
	UpdateAllViews();
}

void Layouter :: SetZoom( double zoom )
{
	_m->_IView->SetZoom( zoom );
	_m->_IconBar->SetFitToWindow( false );
}

double Layouter :: GetZoom()
{
	return _m->_IView->GetZoom();
}

float Layouter :: GetZoomSlider()
{
	return _m->_IconBar->GetZoomSlider();
}

void Layouter :: Set100()
{
	_m->_IView->Set100();
	IconBarFitToWindow( false );
}

void Layouter :: SetFit()
{
	_m->_IView->SetFit();
	IconBarFitToWindow( true );	
}

void Layouter :: IconBarFitToWindow( bool value )
{
	_m->_IconBar->SetFitToWindow( value );
}

void Layouter :: SetScrollBars()
{
	// Vertical scrollbar
	double prop, pos;
	_m->_IView->GetDataForVertScrollbar( pos, prop );

	_m->_VertBar->SetValue( pos );
	_m->_VertBar->SetProportion( (float) prop );

	// Horizontal scrollbar
	_m->_IView->GetDataForHorizScrollbar( pos, prop );

	_m->_HorizBar->SetValue( pos );
	_m->_HorizBar->SetProportion( (float) prop );
}

void Layouter :: SetSlideshow( bool value )
{
	_m->_IconBar->SetSlideshow( value );
}
