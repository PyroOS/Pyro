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

#ifndef WHISPER_IXPORT_H_
#define WHISPER_IXPORT_H_

#include <mailbox.h>

#include <gui/window.h>
#include <util/string.h>

#include <vector>

enum ixplugin_caps
{
	IMPORT = 0x01,
	EXPORT = 0x02
};

class IXPlugin
{
	public:
		IXPlugin( void ){};
		virtual ~IXPlugin( void ){};

		virtual status_t CheckFile( const os::String cFilename ){return EINVAL; };

		virtual status_t Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename ){ return ENOSYS; };
		virtual status_t Export( os::Window *pcParent, Mailfolder *pcFolder, const os::String cFilename ){ return ENOSYS; };
};

class IXNode
{
	public:
		IXNode( void ){};
		virtual ~IXNode( void ){};

		virtual os::String GetIdentifier( void ){ return "null"; };
		virtual uint64 GetCaps( void ){ return 0; };

		virtual IXPlugin * GetPlugin( void ){ return NULL; };
};

class IXFactory
{
	public:
		IXFactory(){};
		~IXFactory();

		static IXFactory * GetFactory( void );

		uint GetPluginCount( void ){ return m_vNodes.size(); };
		status_t GetPluginInfo( uint nIndex, os::String &cIdentifier, uint64 &nCaps );

		IXPlugin * FindPlugin( const os::String cFilename );
		IXPlugin * FindPluginByIdentifier( const os::String cIdentifier );
	private:
		status_t _LoadAll( void );

		std::vector <IXNode*> m_vNodes;
};

#endif

