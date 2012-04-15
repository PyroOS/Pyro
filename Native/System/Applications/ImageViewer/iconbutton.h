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

#ifndef __ICONBUTTON_H__
#define __ICONBUTTON_H__

#include <gui/button.h>
#include <gui/image.h>
#include <gui/font.h>

class IconButton : public os::Button
{
public:

public:
    IconButton( os::Rect cFrame, const os::String& cName, os::Message* pcMessage, os::Image* pcBitmap, bool bAllowMouseOver = true,
				uint32 nResizeMask = os::CF_FOLLOW_LEFT | os::CF_FOLLOW_TOP, uint32 nFlags = os::WID_WILL_DRAW | os::WID_FULL_UPDATE_ON_RESIZE );
    virtual ~IconButton( );
   	
	void SetImage( os::StreamableIO* pcStream );
    void SetImage( os::Image* pcImage );

	os::Image *GetImage( void ) const;
    void ClearImage();
    
    virtual void MouseMove(const os::Point &cNewPos, int nCode, uint32 nButtons, os::Message* pcData);
    virtual void Paint( const os::Rect &cUpdateRect );   
    virtual os::Point GetPreferredSize( bool bLargest ) const;
private:
    class _Private;
    _Private* _m;
};

class ToggleButton : public os::Button
{
public:

public:
    ToggleButton( os::Rect cFrame, const os::String& cName, os::Message* pcMessage, os::Image* pcBitmap, os::Image* pcSelectedBitmap,
				uint32 nResizeMask = os::CF_FOLLOW_LEFT | os::CF_FOLLOW_TOP, uint32 nFlags = os::WID_WILL_DRAW | os::WID_FULL_UPDATE_ON_RESIZE );
    virtual ~ToggleButton( );

	void SetImages( os::Image* pcImage, os::Image* pcImageSelected );
    void ClearImages();

	os::Image* GetImage( void ) const;
	os::Image* GetImageSelected( void ) const;

	bool IsSelected();
	void SetSelected( bool selected );

    virtual void MouseDown( const os::Point& cPosition, uint32 nButton );
    virtual void MouseMove(const os::Point &cNewPos, int nCode, uint32 nButtons, os::Message* pcData);
    virtual void MouseUp( const os::Point& cPosition, uint32 nButton, os::Message* pcData );
    virtual void Paint( const os::Rect &cUpdateRect ); 
private:
    class _Private;
    _Private* _m;
};

#endif
