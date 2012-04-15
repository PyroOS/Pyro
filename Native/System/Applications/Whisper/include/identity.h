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

#ifndef WHISPER_IDENTITY_H_
#define WHISPER_IDENTITY_H_

#include <mailbox.h>

#include <util/string.h>
#include <util/settings.h>
#include <pyro/types.h>

#include <vector>

enum server_type
{
	SERVER_INBOUND,
	SERVER_OUTBOUND
};

enum server_flags
{
	AUTH_POP3_BEFORE = 0x01,
	AUTH_SMTP = 0x02,
	DELETE_ON_READ = 0x80
};

class Server
{
	public:
		Server()
		{
			m_cTransport =
			m_cServer =
			m_cUsername =
			m_cPassword =
			m_cData = "";

			m_nPort =
			m_nFlags = 0;

		};

		void SetTransport( os::String cTransport ){m_cTransport = cTransport;};
		os::String GetTransport( void ){return m_cTransport;};

		void SetServer( os::String cServer ){m_cServer = cServer;};
		os::String GetServer( void ){return m_cServer;};

		void SetPort( uint16 nPort ){m_nPort = nPort;};
		uint16 GetPort( void ){return m_nPort;};

		void SetUsername( os::String cUsername ){m_cUsername = cUsername;};
		os::String GetUsername( void ){return m_cUsername;};

		void SetPassword( os::String cPassword ){m_cPassword = cPassword;};
		os::String GetPassword( void ){return m_cPassword;};

		void SetFlags( uint64 nFlags ){m_nFlags = nFlags;};
		uint64 GetFlags( void ){return m_nFlags;};

		void SetData( os::Variant cData ){m_cData = cData;};
		os::Variant GetData( void ){return m_cData;};

		Server & operator=( Server &cServer )
		{
			m_cTransport = cServer.m_cTransport;
			m_cServer = cServer.m_cServer;
			m_cUsername = cServer.m_cUsername;
			m_cPassword = cServer.m_cPassword;
			m_nPort = cServer.m_nPort;
			m_nFlags = cServer.m_nFlags;
			m_cData = cServer.m_cData;

			return( *this );
		};

	private:
		os::String m_cTransport;
		os::String m_cServer;
		os::String m_cUsername;
		os::String m_cPassword;

		uint16 m_nPort;
		uint64 m_nFlags;

		os::Variant m_cData;
};

class Identity
{
	public:
		Identity( void );
		Identity( os::String cInstance );
		~Identity();

		os::String GetInstance( void ){return m_cInstance;};

		status_t GetMailboxId( MailboxId &cMailboxId, uint nIndex );
		void AddMailboxId( MailboxId cMailboxId );

		status_t GetServer( Server &cServer, server_type eType, uint nIndex );
		void AddServer( Server cServer, server_type eType );

		void AddSignature( const os::String cName, const os::String cText );
		status_t GetSignature( os::String &cName, os::String &cText, uint nIndex );

		void SetName( os::String cName );
		os::String GetName( void ){return m_cName;};

		void SetAddress( os::String cAddress );
		os::String GetAddress( void ){return m_cAddress;};

		void SetFrom( os::String cFrom );
		os::String GetFrom( void ){return m_cFrom;};

		status_t Save( void );
		status_t Save( os::String cInstance );

	private:
		os::String m_cInstance;

		std::vector <MailboxId> m_vBoxIds;
		std::vector <Server> m_vInboundServers;
		std::vector <Server> m_vOutboundServers;

		os::String m_cName;
		os::String m_cAddress;
		os::String m_cFrom;

		//std::vector <os::String> m_vSignatures;
		std::vector <std::pair <os::String, os::String> > m_vSignatures;

};

#endif
