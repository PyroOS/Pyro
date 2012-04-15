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

#ifndef __ADDRESSFIELD_BUTTON_H__
#define __ADDRESSFIELD_BUTTON_H__

#include <gui/button.h>
#include <gui/image.h>
#include <util/string.h>
#include <util/message.h>
#include <gui/font.h>

/** \brief Addressfield Button.
 * This provides a visual representation of the current path of an item with
 * buttons corresponding to each directory tree level of the address.
 *
 * \todo This should be linked to the current EFileBrowser code listed in the source
 * directory so that they do not become too distinct from each other.
 */
class AddressFieldButton : public os::Button
{
public:

	AddressFieldButton( const os::Rect& cFrame, os::String label, os::Image* bitmap, os::Message* msg,
	 				    uint32 nResizeMask = os::CF_FOLLOW_LEFT | os::CF_FOLLOW_TOP,
			       	 	uint32 nFlags = os::WID_WILL_DRAW | os::WID_CLEAR_BACKGROUND | os::WID_FULL_UPDATE_ON_RESIZE );
			       	 	
	~AddressFieldButton();

	void MouseMove( const os::Point& cNewPos, int nCode, uint32 nButtons, os::Message* pcData );
	
	virtual void Paint( const os::Rect& cUpdate );

	virtual os::Point GetPreferredSize( bool bLargest ) const;

	virtual void AllDetached();

private:
	class _Private; /**< Private class used only by AddressFieldButton. */
	_Private* m; /**< Pointer to our private class used by AddressFieldButton. */
};

#endif


