/* Webster (C)opyright	2008 Kristian Van Der Vliet
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

#ifndef BROWSER_PROGRESSPANEL_H
#define BROWSER_PROGRESSPANEL_H

#include <gui/statusbar.h>
#include <gui/spinner.h>

class Progress;

class ProgressPanel : public os::StatusPanel
{
	public:
		ProgressPanel( os::String cName, double vSize );
		~ProgressPanel();

		void FrameSized( const os::Point &cDelta );

		void SetProgress( bool bInProgress );

	private:
		Progress *m_pcProgress;

		class ProgressLooper;
		ProgressLooper *m_pcLooper;
};

#endif
