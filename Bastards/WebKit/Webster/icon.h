/* Webster (C)opyright	2005-2008 Kristian Van Der Vliet
 * 						2004-2007 Arno Klenke
 *						2001 Kurt Skauen
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef BROWSER_SETTINGS_ICON_H
#define BROWSER_SETTINGS_ICON_H

#include <gui/view.h>
#include <gui/image.h>
#include <util/string.h>
#include <pyro/types.h>

using namespace os;

class Icon : public View
{
	public:
		Icon( Rect cFrame, Image *pcImage, String cName );
		virtual ~Icon();

		virtual void AttachedToWindow( void );
	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void Paint( const Rect &cUpdateRect );

		virtual void Select( bool bSelect );

	private:
		Point m_cSize;

		Image *m_pcImage;
		Rect m_cImageFrame;

		String m_cName;
		Point m_cNamePos;

		bool m_bSelected;
		bigtime_t m_nSelectionTime;

		Color32 m_sEraseColor;
		Color32 m_sBgColor;
		Color32 m_sFgColor;
		Color32 m_sHighlightColor;
};

#endif
