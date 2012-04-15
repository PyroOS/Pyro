/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */

#include <icon.h>

Icon::Icon( Rect cFrame, Image *pcImage, String cName ) : View( cFrame, "icon", CF_FOLLOW_ALL )
{
	m_pcImage = pcImage;
	if( m_pcImage )
	{
		m_cSize = m_pcImage->GetSize();
		m_cImageFrame = m_pcImage->GetBounds();
	}
	else
	{
		m_cSize.x = cFrame.Width();
		m_cSize.y = cFrame.Height();
	}

	m_cName = cName;

	Point cNameSize = GetTextExtent( m_cName, DTF_ALIGN_TOP | DTF_IGNORE_FMT );

	/* Default values.  These might be changed below. */
	m_cNamePos.x = 0;
	m_cNamePos.y = m_cImageFrame.bottom + cNameSize.y + 5;

	/* Center the Image or the String, depending on which is the widest */
	if( cNameSize.x > m_cSize.x )
	{
		m_cSize.x = cNameSize.x;

		/* Center the Image against the String */
		m_cImageFrame.left = ( cNameSize.x - m_cImageFrame.Width() ) / 2;
		m_cImageFrame.right = m_cImageFrame.left + m_pcImage->GetSize().x;
	}
	else
	{
		/* Center the String against the Image */
		m_cNamePos.x = ( m_cImageFrame.Width() - cNameSize.x ) / 2;
	}

	/* Allow space below the icon for the name string */
	m_cSize.y += cNameSize.y + 10;

	m_bSelected = false;
}

Icon::~Icon()
{
	if( m_pcImage )
		delete( m_pcImage );
}

void Icon::AttachedToWindow( void )
{
	View *pcParent = GetParent();

	m_sHighlightColor = m_sEraseColor = pcParent->GetEraseColor();
	m_sBgColor = pcParent->GetBgColor();
	m_sFgColor = pcParent->GetFgColor();

	SetEraseColor( m_sEraseColor );
	SetBgColor( m_sBgColor );
	SetFgColor( m_sFgColor );
}

Point Icon::GetPreferredSize( bool bLargest ) const
{
	return m_cSize;
}

void Icon::Paint( const Rect &cUpdateRect )
{
	EraseRect( cUpdateRect );

	if( m_bSelected )
	{
		Rect cUpdateFrame = GetBounds();
		cUpdateFrame.top += 2;
		cUpdateFrame.left += 2;

		/* XXXKV: This is a hack; why is the height from GetBounds() so large? */
		cUpdateFrame.right = m_cSize.x - 2;
		cUpdateFrame.bottom = m_cSize.y - 2;

		FillRect( cUpdateFrame, m_sHighlightColor );

		/* Round edges */
		SetDrawingMode( DM_COPY );
		SetFgColor( m_sHighlightColor );

		DrawLine( os::Point( cUpdateFrame.left + 2, cUpdateFrame.top - 2 ), 
					os::Point( cUpdateFrame.right - 2, cUpdateFrame.top - 2 ) );
		DrawLine( os::Point( cUpdateFrame.left, cUpdateFrame.top - 1 ), 
					os::Point( cUpdateFrame.right, cUpdateFrame.top - 1 ) );
			
		DrawLine( os::Point( cUpdateFrame.left - 2, cUpdateFrame.top + 2 ), 
					os::Point( cUpdateFrame.left - 2, cUpdateFrame.bottom - 2 ) );
		DrawLine( os::Point( cUpdateFrame.left - 1, cUpdateFrame.top ), 
					os::Point( cUpdateFrame.left - 1, cUpdateFrame.bottom ) );
								
		DrawLine( os::Point( cUpdateFrame.left + 2, cUpdateFrame.bottom + 2 ), 
					os::Point( cUpdateFrame.right - 2, cUpdateFrame.bottom + 2 ) );
		DrawLine( os::Point( cUpdateFrame.left, cUpdateFrame.bottom + 1 ), 
					os::Point( cUpdateFrame.right, cUpdateFrame.bottom + 1 ) );
								
		DrawLine( os::Point( cUpdateFrame.right + 2, cUpdateFrame.top + 2 ), 
						os::Point( cUpdateFrame.right + 2, cUpdateFrame.bottom - 2 ) );
		DrawLine( os::Point( cUpdateFrame.right + 1, cUpdateFrame.top ), 
					os::Point( cUpdateFrame.right + 1, cUpdateFrame.bottom ) );

		SetFgColor( m_sFgColor );
	}

	SetDrawingMode( DM_BLEND );

	/* XXXKV: Will only work with BitmapImage; should use RTTI to find
	   type and handle accordingly */
	BitmapImage *pcImage = static_cast<BitmapImage*>( m_pcImage );
	Bitmap *pcBitmap = pcImage->LockBitmap();
	DrawBitmap( pcBitmap, pcImage->GetBounds(), m_cImageFrame );
	pcImage->UnlockBitmap();

	/* Draw the icon name */
	SetDrawingMode( DM_OVER );
	MovePenTo( m_cNamePos );
	DrawString( m_cName );
}

void Icon::Select( bool bSelect )
{
	if( bSelect )
		m_sHighlightColor = Color32( 186, 199, 227 );
	else
		m_sHighlightColor = m_sEraseColor;

	m_bSelected = bSelect;

	Invalidate();
	Flush();
}

