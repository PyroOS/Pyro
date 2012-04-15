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

#include <ixport.h>

#include <debug.h>

#include <storage/file.h>
#include <storage/directory.h>
#include <pyro/image.h>

using namespace os;

typedef IXNode * get_ix_node();
static IXFactory *g_pcFactory = NULL;

IXFactory::~IXFactory()
{
	std::vector<IXNode*>::const_iterator i;
	for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
		delete( (*i) );
	m_vNodes.clear();
}

IXFactory * IXFactory::GetFactory( void )
{
	if( NULL == g_pcFactory )
	{
		g_pcFactory = new IXFactory();
		if( g_pcFactory->_LoadAll() != EOK )
		{
			delete( g_pcFactory );
			g_pcFactory = NULL;
		}
	}

	return g_pcFactory;
}

IXPlugin * IXFactory::FindPlugin( const os::String cFilename )
{
	IXPlugin *pcPlugin = NULL;

	std::vector<IXNode*>::const_iterator i;
	for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
	{
		pcPlugin = (*i)->GetPlugin();
		if( pcPlugin->CheckFile( cFilename ) == EOK )
			break;
		delete( pcPlugin );
		pcPlugin = NULL;
	}

	return pcPlugin;
}

IXPlugin * IXFactory::FindPluginByIdentifier( const os::String cIdentifier )
{
        IXPlugin *pcPlugin = NULL;

        std::vector<IXNode*>::const_iterator i;
        for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
        {
                if( (*i)->GetIdentifier() == cIdentifier )
                {
                        pcPlugin = (*i)->GetPlugin();
                        break;
                }
        }

        return pcPlugin;
}

status_t IXFactory::GetPluginInfo( uint nIndex, os::String &cIdentifier, uint64 &nCaps )
{
	if( nIndex > m_vNodes.size() )
		return EINVAL;

	cIdentifier = m_vNodes[nIndex]->GetIdentifier();
	nCaps = m_vNodes[nIndex]->GetCaps();

	return EOK;
}

status_t IXFactory::_LoadAll( void )
{
	/* Load plugins and add nodes to the list */
	String cFilename;
	String cPath;

	Directory *pcDirectory = new Directory( "^/plugins/ixport" );
	if( pcDirectory == NULL )
		return EIO;
	pcDirectory->GetPath( &cPath );

	debug( "starting scan of import/export plugins directory\n" );

	while( pcDirectory->GetNextEntry( &cFilename ) )
	{
		if( cFilename == "." || cFilename == ".." )
			continue;
		cFilename = cPath + String( "/" ) + cFilename;

		image_id nId = load_library( cFilename.c_str(), 0 );
		if( nId >= 0 )
		{
			debug( "loaded plugin %s\n", cFilename.c_str() );

			get_ix_node *pGet;
			if( get_symbol_address( nId, "get_ix_node", -1, (void**)&pGet ) != 0 )
			{
				debug( "get_ix_node() not found in %s\n", cFilename.c_str() );

				unload_library( nId );
				continue;
			}

			IXNode *pcNode = pGet();
			if( pcNode == NULL )
			{
				debug( "failed to get import/export plugin node from %s\n", cFilename.c_str() );

				unload_library( nId );
				continue;
			}
			debug( "add plugin %s\n", cFilename.c_str() );

			m_vNodes.push_back( pcNode );
		}
	}

	return EOK;
}

