/* Webster (C)opyright	2008 Kristian Van Der Vliet
 * 						2004-2007 Arno Klenke
 *						2001 Kurt Skauen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU Library
 * General Public License as published by the Free Software
 * Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 */
 
#ifndef BROWSER_BOOKMARKS_H
#define BROWSER_BOOKMARKS_H

#include <gui/menu.h>
#include <util/looper.h>
#include <storage/path.h>
#include <storage/nodemonitor.h>

class BookmarksMenu;

class Bookmark
{
	public:
		Bookmark( os::String cName, os::String cURL = "", uint nIndex = 0 );
		virtual ~Bookmark();

		virtual void SetName( const os::String cName );
		virtual os::String GetName();

		virtual void SetURL( const os::String cURL );
		virtual os::String GetURL();

		virtual void SetIndex( const uint nIndex );
		virtual uint GetIndex();

	protected:
		void Load( const os::Path cPath );

		friend class BookmarksMenu;

	private:
		class Private;
		Private *m;
};

class BookmarksMenu : public os::Menu
{
	public:
		BookmarksMenu( os::Rect cFrame, const char* pzName, os::MenuLayout_e eLayout, const os::Path cPath, const bool bIsRoot );
		~BookmarksMenu();

		virtual status_t SetTargetForItems( os::Handler* pcTarget );

	protected:
		void Reload();
		void ResetTargetForItems();

	private:
		os::Handler* m_pcTargetForItems;

		os::NodeMonitor* m_pcMonitor;
		os::Path m_cPath;

		bool m_bIsRoot;

		class BookmarksMenuLooper;
		friend class BookmarksMenuLooper;

		BookmarksMenuLooper* m_pcLooper;
};

class BookmarksManager
{
	public:
		BookmarksManager();
		virtual ~BookmarksManager();

		virtual BookmarksMenu* CreateMenu( const char *pzName, const os::Path cPath );

		virtual status_t AddBookmark( Bookmark *pcBookmark );
		virtual status_t AddBookmark( os::String cName, const os::String cURL );

		virtual status_t DeleteBookmark( Bookmark *pcBookmark );
		virtual status_t DeleteBookmark( os::String cName, const os::String cURL );

	private:
		class Private;
		Private *m;
};

#endif
