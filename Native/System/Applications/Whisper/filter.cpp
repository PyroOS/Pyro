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

#include <filter.h>
#include <rule_filter.h>
#include <debug.h>

#include <storage/file.h>
#include <storage/directory.h>
#include <atheos/image.h>

using namespace os;

/* XXXKV: Instantiating a static vector? Might want to rethink this design. */ 
std::vector <Filter*> FilterEngine::s_vFilters;

FilterEngine::FilterEngine( void )
{
	Filter *pcFilter;

	/* RuleFilter is built in */
	pcFilter = new RuleFilter();
	s_vFilters.push_back( pcFilter );

	/* Load plugins and add nodes to the list */
	String cFilename;
	String cPath;

	try
	{
		Directory *pcDirectory = new Directory( "^/plugins/filter" );
		if( pcDirectory != NULL )
		{
			pcDirectory->GetPath( &cPath );

			debug( "starting scan of filter plugins directory\n" );

			while( pcDirectory->GetNextEntry( &cFilename ) )
			{
				if( cFilename == "." || cFilename == ".." )
					continue;
				cFilename = cPath + String( "/" ) + cFilename;

				image_id nId = load_library( cFilename.c_str(), 0 );
				if( nId >= 0 )
				{
					debug( "loaded plugin %s\n", cFilename.c_str() );

					typedef Filter * get_filter();
					get_filter *pGet;
					if( get_symbol_address( nId, "get_filter", -1, (void**)&pGet ) != 0 )
					{
						debug( "get_filter() not found in %s\n", cFilename.c_str() );

						unload_library( nId );
						continue;
					}

					pcFilter = pGet();
					if( pcFilter == NULL )
					{
						debug( "failed to get filter plugin from %s\n", cFilename.c_str() );

						unload_library( nId );
						continue;
					}
					debug( "add plugin %s\n", cFilename.c_str() );

					s_vFilters.push_back( pcFilter );
				}
			}
		}
	}
	catch( std::exception &e )
	{
		debug( "failed to load filter plugins: %s\n", e.what() );
	}
}

FilterEngine::~FilterEngine()
{
	std::vector<Filter*>::const_iterator i;
	for( i = s_vFilters.begin(); i != s_vFilters.end(); i++ )
		delete( (*i) );
	s_vFilters.clear();
}

FolderReference FilterEngine::FilterMessage( Mailmessage *pcMessage )
{
	FolderReference cReference;

	/* Walk the list of filters. If one of them "hits" I.e. returns EOK, then we stop. */
	std::vector<Filter*>::const_iterator i;
	for( i = s_vFilters.begin(); i != s_vFilters.end(); i++ )
	{
		status_t nError = (*i)->FilterMessage( pcMessage, cReference );
		if( nError == EOK )
			break;
	}

	return cReference;
}

Filter * FilterEngine::GetFilter( uint nIndex )
{
	Filter *pcFilter = NULL;

	if( nIndex >= 0 && nIndex < s_vFilters.size() )
		pcFilter = s_vFilters[nIndex];

	return pcFilter;
}

