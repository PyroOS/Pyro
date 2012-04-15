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

#include "iconbutton.h"
#include <gui/window.h>

using namespace os;

/***********************
*						
* I C O N B U T T O N
*
***********************/
///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

class IconButton :: _Private
{
public:
	_Private()
	{
		m_pcBitmap = NULL;
		bMouseOver = false;
	}

	~_Private()
	{
		if( m_pcBitmap )
			delete m_pcBitmap;
	}

	void SetImage( Image* pcBitmap )
	{
		if( m_pcBitmap )
			delete m_pcBitmap;

		m_pcBitmap = pcBitmap;
	}

	Image* GetImage()
	{
		return m_pcBitmap;
	}

	Image* m_pcBitmap;
	bool bMouseOver;	// true for mouse over mode
	bool bAllowMouseOver; // True if we allow mouse over
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////


IconButton :: IconButton( Rect cFrame, const String& cName, Message *pcMessage, Image *pcBitmap, bool bAllowMouseOver,
						  uint32 nResizeMask, uint32 nFlags ) : Button( cFrame, cName, "", pcMessage, nResizeMask, nFlags )
{
	_m = new _Private();

	_m->SetImage( pcBitmap );
	_m->bAllowMouseOver = bAllowMouseOver;
}

IconButton :: ~IconButton()
{
	delete _m;
}

void IconButton :: SetImage( StreamableIO* pcStream )
{
	Image* vImage = new BitmapImage( pcStream );
	_m->SetImage( vImage );
}

void IconButton :: SetImage( Image * pcImage )
{
	if( pcImage )
		_m->SetImage( pcImage );
}

Image* IconButton :: GetImage( void ) const
{
	return _m->GetImage();
}

void IconButton :: ClearImage()
{
	_m->SetImage( NULL );
}

Point IconButton :: GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
	{
		return Point( COORD_MAX, COORD_MAX );
	} else {
		Point cSize;

		if( _m->m_pcBitmap )
		{
			cSize = _m->m_pcBitmap->GetSize();
		}

		cSize += Point( 2, 2 );

		if( _m->bAllowMouseOver )
			cSize += Point( 3, 3 );		// Make sure we have some space for mouseover

		return cSize;
	}
}

void IconButton :: MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message * pcData )
{
	Button::MouseMove( cNewPos, nCode, nButtons, pcData );

	if( nCode == MOUSE_ENTERED )
	{
		if( !_m->bMouseOver )
		{
			_m->bMouseOver = true;
			Invalidate();
			Flush();
		}
	}
	if( nCode == MOUSE_EXITED )
	{
		if( _m->bMouseOver )
		{
			_m->bMouseOver = false;
			Invalidate();
			Flush();
		}

	}
}

void IconButton :: Paint( const Rect& cUpdateRect )
{
	Rect cBounds = GetBounds();
	Image* pcImg = _m->GetImage();

	// Clear background
	SetFgColor( get_default_color( COL_NORMAL ) );
	FillRect( cUpdateRect );

	cBounds.left++;  // Why???

	// Draw nice frame
	if( _m->bMouseOver && _m->bAllowMouseOver )
	{	
		SetFgColor( get_default_color( COL_ICON_SELECTED ) );		
		DrawLine( Point( cBounds.left + 2, cBounds.top ), Point( cBounds.right - 2, cBounds.top ) );
		DrawLine( Point( cBounds.left + 1, cBounds.top + 1 ), Point( cBounds.right - 1, cBounds.top + 1 ) );
		DrawLine( Point( cBounds.left, cBounds.top + 2 ), Point( cBounds.left, cBounds.bottom - 2 ) );
		FillRect( Rect( cBounds.left + 1, cBounds.top + 2, cBounds.right - 1, cBounds.bottom - 2 ) );
		DrawLine( Point( cBounds.left + 1, cBounds.bottom - 1), Point( cBounds.right - 1, cBounds.bottom - 1) );
		DrawLine( Point( cBounds.left + 2, cBounds.bottom ), Point( cBounds.right - 2, cBounds.bottom ) );
		DrawLine( Point( cBounds.right, cBounds.top + 2 ), Point( cBounds.right, cBounds.bottom - 2 ) );
	}

	if( pcImg )
	{
		// The image shall be in the center of the bound
		Point img = Point( cBounds.left + cBounds.Width() / 2 - pcImg->GetSize().x / 2,
						   cBounds.top + cBounds.Height() / 2 - pcImg->GetSize().y / 2 );
 
		SetDrawingMode( DM_BLEND );
		pcImg->Draw( img, this );
		SetDrawingMode( DM_OVER );	//sets the mode to dm_over, so that hovering works completely
	}
}


/**************************
*						
* T O G G L E B U T T O N
*
**************************/
///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

class ToggleButton :: _Private
{
public:
	_Private()
	{
		m_pcBitmapSelected = NULL;	
		m_pcBitmap = NULL;
		bMouseOver = false;
		bMouseDown = false;
		bSelected = false;
	}

	~_Private()
	{
		if( m_pcBitmap )
			delete m_pcBitmap;

		if( m_pcBitmapSelected )
			delete m_pcBitmapSelected;
	}

	void SetImageSelected( Image* pcBitmap )
	{
		if( m_pcBitmapSelected )
			delete m_pcBitmapSelected;

		m_pcBitmapSelected = pcBitmap;
	}

	Image* GetImageSelected()
	{
		return m_pcBitmapSelected;
	}

	void SetImage( Image* pcBitmap )
	{
		if( m_pcBitmap )
			delete m_pcBitmap;

		m_pcBitmap = pcBitmap;
	}

	Image* GetImage()
	{
		return m_pcBitmap;
	}

	void _DrawRound( View* view, Rect& cBounds )
	{
		view->SetFgColor( get_default_color( COL_ICON_SELECTED ) );		
		view->DrawLine( Point( cBounds.left + 2, cBounds.top ), Point( cBounds.right - 2, cBounds.top ) );
		view->DrawLine( Point( cBounds.left + 1, cBounds.top + 1 ), Point( cBounds.right - 1, cBounds.top + 1 ) );
		view->DrawLine( Point( cBounds.left, cBounds.top + 2 ), Point( cBounds.left, cBounds.bottom - 2 ) );
		view->FillRect( Rect( cBounds.left + 1, cBounds.top + 2, cBounds.right - 1, cBounds.bottom - 2 ) );
		view->DrawLine( Point( cBounds.left + 1, cBounds.bottom - 1), Point( cBounds.right - 1, cBounds.bottom - 1) );
		view->DrawLine( Point( cBounds.left + 2, cBounds.bottom ), Point( cBounds.right - 2, cBounds.bottom ) );
		view->DrawLine( Point( cBounds.right, cBounds.top + 2 ), Point( cBounds.right, cBounds.bottom - 2 ) );
	}

	Image* m_pcBitmap;
	Image* m_pcBitmapSelected;
	bool bMouseOver;	// true for mouse over mode
	bool bMouseDown;
	bool bSelected;
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////


ToggleButton :: ToggleButton( Rect cFrame, const String& cName, Message *pcMessage, Image *pcBitmap, Image *pcBitmapSelected,
						  	  uint32 nResizeMask, uint32 nFlags ) : Button( cFrame, cName, "", pcMessage, nResizeMask, nFlags )
{
	_m = new _Private();

	_m->SetImage( pcBitmap );
	_m->SetImageSelected( pcBitmapSelected );
}

ToggleButton :: ~ToggleButton()
{
	delete _m;
}

void ToggleButton :: SetImages( Image* pcImage, Image* pcImageSelected )
{
	if( pcImage )
		_m->SetImage( pcImage );
	if( pcImageSelected )
		_m->SetImageSelected( pcImageSelected );
}

void ToggleButton :: ClearImages()
{
	_m->SetImage( NULL );
	_m->SetImageSelected( NULL );
}

Image* ToggleButton :: GetImageSelected( void ) const
{
	return _m->GetImageSelected();
}

Image* ToggleButton :: GetImage( void ) const
{
	return _m->GetImage();
}
   
bool ToggleButton :: IsSelected()
{
	return _m->bSelected;
}

void ToggleButton :: SetSelected( bool selected )
{
	if( _m->bSelected != selected )
	{
		_m->bSelected = selected;
		Invalidate();
		Flush();
	}
}

void ToggleButton :: MouseDown( const os::Point& cPosition, uint32 nButton )
{
	_m->bMouseDown = true;
	Button::MouseDown( cPosition, nButton );
}

void ToggleButton :: MouseMove(const os::Point &cNewPos, int nCode, uint32 nButtons, os::Message* pcData)
{
	Button::MouseMove( cNewPos, nCode, nButtons, pcData );

	if( nCode == MOUSE_ENTERED )
	{
		if( !_m->bMouseOver )
		{
			_m->bMouseOver = true;
			Invalidate();
			Flush();
		}
	}
	if( nCode == MOUSE_EXITED )
	{
		if( _m->bMouseOver )
		{
			_m->bMouseOver = false;
			Invalidate();
			Flush();
		}
	}
}

void ToggleButton :: MouseUp( const os::Point& cPosition, uint32 nButton, os::Message* pcData )
{
	if( _m->bMouseDown )
	{
		_m->bMouseDown = false;
		_m->bSelected = !_m->bSelected;
		Invalidate();
		Flush();
		Button::MouseUp( cPosition, nButton, pcData );
	}
}

void ToggleButton :: Paint( const os::Rect &cUpdateRect )
{
	Rect cBounds = GetBounds();	

	// Clear background
	SetFgColor( get_default_color( COL_NORMAL ) );
	FillRect( cUpdateRect );

	cBounds.left++;  // Why???

	// Draw nice frame
	if( _m->bMouseOver )
		_m->_DrawRound( this, cBounds );


	if( !_m->bSelected )
	{
		Image* pcImg = _m->GetImage();

		if( pcImg )
		{
			// The image shall be in the center of the bound
			Point img = Point( cBounds.left + cBounds.Width() / 2 - pcImg->GetSize().x / 2,
							   cBounds.top + cBounds.Height() / 2 - pcImg->GetSize().y / 2 );
 
			SetDrawingMode( DM_BLEND );
			pcImg->Draw( img, this );
			SetDrawingMode( DM_OVER );	//sets the mode to dm_over, so that hovering works completely
		}
	}
	else
	{
		Image* pcImg = _m->GetImageSelected();
		if( pcImg == NULL )
		{
			_m->_DrawRound( this, cBounds );
			pcImg = _m->GetImage();
		}

		if( pcImg )
		{
			// The image shall be in the center of the bound
			Point img = Point( cBounds.left + cBounds.Width() / 2 - pcImg->GetSize().x / 2,
							   cBounds.top + cBounds.Height() / 2 - pcImg->GetSize().y / 2 );
 
			SetDrawingMode( DM_BLEND );
			pcImg->Draw( img, this );
			SetDrawingMode( DM_OVER );	//sets the mode to dm_over, so that hovering works completely
		}
	}
}


