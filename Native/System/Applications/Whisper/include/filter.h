/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef WHISPER_FILTER_H_
#define WHISPER_FILTER_H_

#include <mail.h>
#include <mailbox.h>
#include <settings_views.h>

#include <gui/view.h>
#include <util/string.h>

#include <vector>
#include <errno.h>

class Filter
{
	public:
		Filter( void ){};
		virtual ~Filter( void ){};

		virtual status_t FilterMessage( Mailmessage *pcMessage, FolderReference &cReference ){return ENOSYS;};

		virtual os::String GetIdentifier( void ) const {return "";};
		virtual SettingsTab * GetSettingsView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow ){return NULL;};
};

class FilterEngine
{
	public:
		FilterEngine( void );
		~FilterEngine();

		FolderReference FilterMessage( Mailmessage *pcMessage );

		static Filter * GetFilter( uint nIndex );

	private:
		static std::vector <Filter*> s_vFilters;
};

#endif

