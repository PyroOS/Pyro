// EFileBrowser (C)opyright 2007 Jonas Jarvoll
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

#include "address_field_button.h"

using namespace os;

///////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E
//
///////////////////////////////////////////////////////////////////////////////

/** \brief Private class used only by AddressFieldButton.
 *
 * \bug Is there a reason why it stores the label and image information?
 */
class AddressFieldButton :: _Private
{
public:
	
	/** \brief Default Constructor. */
	_Private() {};

	/** \brief Draws a Rounded Frame in the given View* object.
	 *
	 * \param view View* object to draw the rounded frame in.
	 *
	 */
	void _DrawRoundedFrame( View* view )
	{
		Rect cFillRect = view->GetBounds();
		cFillRect.Resize( 1, 4, -1 , -4 );
		view->SetFgColor( 180, 180, 180 );
		view->DrawLine( Point( 0, cFillRect.top ), Point( 0, cFillRect.bottom ) );
		view->DrawLine( Point( cFillRect.right + 1, cFillRect.top ), Point( cFillRect.right + 1, cFillRect.bottom ) );
		view->DrawLine( Point( 4, 0 ), Point( view->GetBounds().right - 4, 0 ) );
		view->DrawLine( Point( 2, 1 ), Point( 3, 1 ) );
		view->DrawLine( Point( 1, 2 ), Point( 1, 3 ) );
		view->DrawLine( Point( view->GetBounds().right - 2, 1 ), Point( view->GetBounds().right - 3, 1 ) );
		view->DrawLine( Point( view->GetBounds().right - 1, 2 ), Point( view->GetBounds().right - 1, 3 ) );

		cFillRect.top = view->GetBounds().bottom - 3;
		cFillRect.bottom = view->GetBounds().bottom;
		view->SetFgColor( 180, 180, 180 );
		view->DrawLine( Point( 4, cFillRect.bottom - 0 ), Point( view->GetBounds().right - 4, cFillRect.bottom - 0 ) );
		view->DrawLine( Point( 2, cFillRect.bottom - 1 ), Point( 3, cFillRect.bottom - 1 ) );
		view->DrawLine( Point( 1, cFillRect.bottom - 2 ), Point( 1, cFillRect.bottom - 3 ) );
		view->DrawLine( Point( view->GetBounds().right - 2, cFillRect.bottom - 1 ), Point(  view->GetBounds().right - 3, cFillRect.bottom - 1 ) );
		view->DrawLine( Point( view->GetBounds().right - 1, cFillRect.bottom - 2 ), Point(  view->GetBounds().right - 1, cFillRect.bottom - 3 ) );
		view->DrawLine( Point( 4, cFillRect.bottom ), Point( view->GetBounds().right - 4, cFillRect.bottom ) );
	}

	bool m_MouseOver; /**< Flag storing if there is a mouse over the button. */

	Image* m_Image; /**< Image object associated with the button. */
	String m_Label; /**< The text label of the button. */
	float m_FontHeight; /**< The height of the font that is to be drawn. */
};

///////////////////////////////////////////////////////////////////////////////
//
// T H E   C L A S S
//
///////////////////////////////////////////////////////////////////////////////

/** \brief AddressFieldButton Constructor.
 * This initializes all of the internal variables and calculates the height of
 * the font.
 *
 * \warning Do not remove os:: namespace from parameter names or it will mess up
 * the doxygen documentation.
 *
 * \param cFrame Rectangular dimensions of the object. This is passed directly to os::Button.
 * \param label Label for the address field button.
 * \param bitmap Image associated with the button.
 * \param msg Message to send to the object. This is passed directly to os::Button.
 * \param nResizeMask Resize mask for the object. See os::Button definition for more information.
 * \param nFlags Flags for the button. See os::Button definition for more information.
 */
AddressFieldButton::AddressFieldButton( const os::Rect& cFrame, os::String label, os::Image* bitmap, os::Message* msg, uint32 nResizeMask, uint32 nFlags ) 
							    : Button( cFrame, "", "", msg, nResizeMask, nFlags )
{
	// Create the private class
	m = new _Private();

	m->m_Label = label;
	m->m_Image = bitmap;
	m->m_MouseOver = false;

	// Calculate font height
	font_height tmp;	
	GetFontHeight( &tmp );
	m->m_FontHeight = tmp.ascender + tmp.descender;	
}

/** \brief Default Deconstructor.
 * Deletes the _Private object allocated for drawing.
 */
AddressFieldButton::~AddressFieldButton()
{
//	delete m->m_Image;
	delete m;
}

/** \brief AllDetached Overloaded Function.
 * When everything is detached, set the mouse over for the _Private object to false.
 */
void AddressFieldButton::AllDetached()
{
	m->m_MouseOver = false;
}

/** \brief MouseMove Handler.
 * This handles whether or not the mouse is over the button and when to start
 * displaying the mouse over view of the button.
 *
 * \warning Do not remove os:: namespace from parameter names or it will mess up
 * the doxygen documentation.
 *
 */
void AddressFieldButton::MouseMove( const os::Point& cNewPos, int nCode, uint32 nButtons, os::Message* pcData )
{
	Button::MouseMove( cNewPos, nCode, nButtons, pcData ); 

	if( nCode == MOUSE_EXITED )
	{
		if( m->m_MouseOver )
		{
			m->m_MouseOver = false;
			Invalidate();
			Flush();
		}
	}
	else
	{
		bool mouseover = GetBounds().DoIntersect( cNewPos );

		if( mouseover != m->m_MouseOver )
		{
			m->m_MouseOver = mouseover;
			Invalidate();
			Flush();
		}
	}
}

/** \brief Draw the Button.
 * This draws the rounded button with its image and label.
 *
 * \warning Do not remove os:: namespace from parameter names or it will mess up
 * the doxygen documentation.
 *
 * \param cUpdate The region of the window to update.
 */
void AddressFieldButton::Paint( const os::Rect& cUpdate )
{
	SetDrawingMode( DM_COPY );

	float offset = 4.0f;
	Rect cFrame = GetBounds();

	// Draw selection if mouse is inside the button
	if( m->m_MouseOver )
	{
		Rect cFillRect = GetBounds();
		cFillRect.bottom = 3;
		cFillRect = GetBounds();
		cFillRect.Resize( 1, 4, -1 , -4 );
		FillRect( cFillRect, get_default_color( COL_ICON_SELECTED ) );
	
		SetFgColor( get_default_color( COL_ICON_SELECTED ) );
		DrawLine( Point( 4, 1 ), Point( GetBounds().right - 4, 1 ) );
		DrawLine( Point( 2, 2 ), Point( GetBounds().right - 2, 2 ) );
		DrawLine( Point( 2, 3 ), Point( GetBounds().right - 2, 3 ) );

		cFillRect.top = GetBounds().bottom - 3;
		cFillRect.bottom = GetBounds().bottom;
		FillRect( cFillRect, get_default_color( COL_NORMAL ) );

		SetFgColor( get_default_color( COL_ICON_SELECTED ) );
		DrawLine( Point( 4, cFillRect.bottom - 1 ), Point( GetBounds().right - 4, cFillRect.bottom - 1 ) );
		DrawLine( Point( 2, cFillRect.bottom - 2 ), Point( GetBounds().right - 2, cFillRect.bottom - 2 ) );
		DrawLine( Point( 2, cFillRect.bottom - 3 ), Point( GetBounds().right - 2, cFillRect.bottom - 3 ) );
	}
	else
		FillRect( cFrame, get_default_color( COL_NORMAL ) );

	// Always the draw the frame around the button
	m->_DrawRoundedFrame( this );

	// Draw the image
	SetDrawingMode( DM_BLEND );
	if( m->m_Image != NULL )
		m->m_Image->Draw( Point( cFrame.left + offset, cFrame.top + cFrame.Height() / 2 - m->m_Image->GetSize().y / 2 ), this );
	SetDrawingMode( DM_COPY );

	// Draw the label
	SetFgColor( 0, 0, 0 );
//	GetFont()->SetFlags( FPF_BOLD | FPF_SMOOTHED ); // Doesnt FPF_BOLD work?
	GetFont()->SetFlags( FPF_SMOOTHED ); // Doesnt FPF_BOLD work?
	DrawString( Point( cFrame.left + offset * 3 + m->m_Image->GetSize().x, cFrame.top + cFrame.Height() / 2 + m->m_FontHeight / 2 ), m->m_Label );
}

/** \brief Return the preferred size of the button.
 */
Point AddressFieldButton::GetPreferredSize( bool bLargest ) const
{
	if( bLargest )
		return Point( COORD_MAX, COORD_MAX );

	return Point( 20, 40 );
}



