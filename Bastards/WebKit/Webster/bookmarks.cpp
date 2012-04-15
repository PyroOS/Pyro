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

#include <gui/menu.h>
#include <gui/image.h>
#include <util/string.h>
#include <util/resources.h>
#include <storage/directory.h>
#include <storage/fsnode.h>
#include <storage/file.h>
#include <storage/registrar.h>

#include <bookmarks.h>
#include <messages.h>

using namespace os;

class Bookmark::Private
{
	public:
		Private(){};

		void SetName( const String cName )
		{
			m_cName = cName;
		};
		String GetName()
		{
			return m_cName;
		}
		void SetURL( const String cURL )
		{
			m_cURL = cURL;
		}
		String GetURL()
		{
			return m_cURL;
		}
		void SetIndex( const uint nIndex )
		{
			m_nIndex = nIndex;
		}
		uint GetIndex()
		{
			return m_nIndex;
		}

		void Load( const Path cPath )
		{
			char zBuffer[PATH_MAX] = {0};
			try
			{
				File cFile( cPath.GetPath() );
				if( cFile.Read( zBuffer, PATH_MAX ) > 0 )
					m_cURL = zBuffer;

				cFile.ReadAttr( "index", ATTR_TYPE_INT32, &m_nIndex, 0, sizeof( m_nIndex ) );
			}
			catch(...)
			{
			}
		}

	private:
		String m_cName;
		String m_cURL;
		uint m_nIndex;
};

Bookmark::Bookmark( String cName, String cURL, uint nIndex )
{
	m = new Private();

	m->SetName( cName );
	m->SetURL( cURL );
	m->SetIndex( nIndex );
}

Bookmark::~Bookmark()
{
	delete m;
}

void Bookmark::SetName( const String cName )
{
	m->SetName( cName );
}

String Bookmark::GetName()
{
	return m->GetName();
}

void Bookmark::SetURL( const String cURL )
{
	m->SetURL( cURL );
}

String Bookmark::GetURL()
{
	return m->GetURL();
}

void Bookmark::SetIndex( const uint nIndex )
{
	m->SetIndex( nIndex );
}

uint Bookmark::GetIndex()
{
	return m->GetIndex();
}

void Bookmark::Load( const Path cPath )
{
	m->Load( cPath );
}

class BookmarksMenu::BookmarksMenuLooper : public Looper
{
	public:
		BookmarksMenuLooper( BookmarksMenu *pcOwner ) : Looper( "bookmarksmenu_looper" )
		{
			m_pcOwner = pcOwner;
		}
		void HandleMessage( Message *pcMessage )
		{
			switch( pcMessage->GetCode() )
			{
				case M_NODE_MONITOR:
				{
					if( m_pcOwner->GetWindow() == NULL )
					{
						m_pcOwner->Reload();
						m_pcOwner->ResetTargetForItems();
					}	
					break;
				}
				default:
				{
					Looper::HandleMessage( pcMessage );
					break;
				}
			}
		}

	private:
		BookmarksMenu *m_pcOwner;

};

BookmarksMenu::BookmarksMenu( Rect cFrame, const char* pzName, MenuLayout_e eLayout, const Path cPath, const bool bIsRoot )
	 	: Menu( cFrame, pzName, eLayout )
{
	m_cPath = cPath;
	m_bIsRoot = bIsRoot;

	/* Create a nodemonitor for this directory */
	m_pcLooper = new BookmarksMenuLooper( this );
	try
	{
		m_pcMonitor = new NodeMonitor( m_cPath.GetPath(), NWATCH_ALL, m_pcLooper, m_pcLooper );
		m_pcLooper->Run();
	}
	catch( std::exception &e )
	{
		fprintf( stderr, "BookmarksMenu failed to create NodeMonitor: %s\n", e.what() );
	}

	Reload();
}

BookmarksMenu::~BookmarksMenu()
{
	delete m_pcMonitor;
	m_pcLooper->PostMessage( M_TERMINATE );
}

status_t BookmarksMenu::SetTargetForItems( Handler* pcTarget )
{
	m_pcTargetForItems = pcTarget;
	Menu::SetTargetForItems( m_pcTargetForItems );
}

void BookmarksMenu::Reload()
{
	/* Clear */
	MenuItem* pcItem;
	while( ( pcItem = RemoveItem( 0 ) ) != NULL )
		delete( pcItem );
	
	/* Check if the directory is valid */
	FSNode* pcNode = NULL;
	try
	{
		pcNode = new FSNode( m_cPath.GetPath() );
	}
	catch( ... )
	{
		return;
	}

	if( !pcNode->IsValid() )
	{
		delete( pcNode );
		return;
	}
	delete( pcNode );
	
	/* Iterate through the directory */
	Directory cDir( m_cPath.GetPath() );
	cDir.Rewind();
	String zFile;
	
	/* Get the registrar manager */
	RegistrarManager* pcManager = NULL;
	try
	{
		pcManager = RegistrarManager::Get();
	}
	catch( ... )
	{
		delete( pcNode );
		return;
	}

	if( m_bIsRoot )
	{
		AddItem( new MenuItem( "Add this page", new Message( ID_MENU_BOOKMARKS_ADD ), "Ctrl+D" ) );

		Message* pcManageMsg = new Message( ID_MENU_BOOKMARKS_MANAGE );
		pcManageMsg->AddString( "path", m_cPath.GetPath() );
		AddItem( new MenuItem( "Manage bookmarks", pcManageMsg ) );

		AddItem( new MenuSeparator() );
	}
	
	while( cDir.GetNextEntry( &zFile ) == 1 )
	{
		if( zFile == String( "." ) ||
			zFile == String( ".." ) )
			continue;
		Path cFilePath = m_cPath;
		
		cFilePath.Append( zFile.c_str() );
		
		Image* pcItemIcon = NULL;
		
		/* Get icon */
		if( pcManager )
		{
			String zTemp;
			if( pcManager->GetTypeAndIcon( cFilePath, Point( 16, 16 ), &zTemp, &pcItemIcon ) < 0 )
				continue;
		}
		
		/* Validate entry */
		try
		{
			pcNode = new FSNode( cFilePath );
		}
		catch( ... )
		{
			continue;
		}
		if( !pcNode->IsValid() )
		{
			delete( pcNode );
			continue;
		}
			
		if( pcNode->IsFile() )
		{
			/* Load the bookmark */
			Bookmark cBookmark( zFile );
			cBookmark.Load( cFilePath );

			/* Create a menu item */
			Message *pcMsg = new Message( ID_BOOKMARK_GO );
			pcMsg->AddString( "url", cBookmark.GetURL() );

			uint nIndex = cBookmark.GetIndex();
			if( m_bIsRoot && nIndex < 3 )
				nIndex = 3;
			AddItem( new MenuItem( cBookmark.GetName(), pcMsg, "", pcItemIcon ), nIndex );
		}
		else if( pcNode->IsDir() )
		{
			/* Create another DirMenu */
			BookmarksMenu* pcMenu = new BookmarksMenu( Rect(), zFile.c_str(), ITEMS_IN_COLUMN, cFilePath, false ) ;
			AddItem( new MenuItem( pcMenu, NULL, "", pcItemIcon ) );
		}
		delete( pcNode );
	}
	
	if( pcManager )
		pcManager->Put();
}

void BookmarksMenu::ResetTargetForItems()
{
	Menu::SetTargetForItems( m_pcTargetForItems );
}

class BookmarksManager::Private
{
	public:
		Private(){};

		BookmarksMenu* CreateMenu( const char *pzName, const Path cPath )
		{
			if( cPath == Path( "" ) )
			{
				char *pzHome = getenv( "HOME" );
				if( NULL != pzHome )
					m_cPath = String( pzHome ) + "/Bookmarks";
			}
			else
				m_cPath = cPath;

			return new BookmarksMenu( Rect(), pzName, ITEMS_IN_COLUMN, m_cPath, true );
		};

		status_t AddBookmark( String cName, const String cURL )
		{
			status_t nError = EOK;
			String cPath;

			/* Remove any invalid characters from the title */
			for( uint i = 0; i < cName.Length(); i++ )
				if ( cName[i] == '/' )
					cName[i] = '_';

			cPath = m_cPath.GetPath() + "/" + cName;

			try
			{
				String cType = "text/x-bookmark";

				File cFile( cPath, O_RDWR | O_CREAT | O_TRUNC );
				cFile.Write( cURL.c_str(), cURL.Length() );
				cFile.WriteAttr( "os::MimeType", O_TRUNC, ATTR_TYPE_STRING, cType.c_str(), 0, cType.Length() );
				cFile.Flush();
			}
			catch( std::exception &e )
			{
				fprintf( stderr, "Failed to add bookmark: %s\n", e.what() );
				nError = EIO;
			}

			return nError;
		};

		status_t DeleteBookmark( const String cName, const String cURL )
		{
			return ENOSYS;
		};

	private:
		Path m_cPath;
};

BookmarksManager::BookmarksManager()
{
	m = new Private();
}

BookmarksManager::~BookmarksManager()
{
	delete m;
}

BookmarksMenu* BookmarksManager::CreateMenu( const char *pzName, const Path cPath )
{
	return m->CreateMenu( pzName, cPath );
}

status_t BookmarksManager::AddBookmark( Bookmark *pcBookmark )
{
	return m->AddBookmark( pcBookmark->GetName(), pcBookmark->GetURL() );
}

status_t BookmarksManager::AddBookmark( String cName, const String cURL )
{
	return m->AddBookmark( cName, cURL );
}

status_t BookmarksManager::DeleteBookmark( Bookmark *pcBookmark )
{
	return m->DeleteBookmark( pcBookmark->GetName(), pcBookmark->GetURL() );
}

status_t BookmarksManager::DeleteBookmark( String cName, const String cURL )
{
	return m->DeleteBookmark( cName, cURL );
}
