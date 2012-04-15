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

#ifndef WHISPER_POP3_TRANSPORT_H_
#define WHISPER_POP3_TRANSPORT_H_

#include <transport.h>
#include <libpop3.h>

class Pop3TransportNode : public TransportNode
{
	public:
		Pop3TransportNode(){};
		~Pop3TransportNode(){};

		os::String GetIdentifier( void ){return "pop3";};
		Transport * GetTransport( void );
};

#define POP3_DEFAULT_PORT 110

class Pop3Transport : public Transport
{
	public:
		Pop3Transport( os::String cServer = "", os::String cUsername = "", os::String cPassword = "", uint nPort = POP3_DEFAULT_PORT );
		~Pop3Transport( void );

		os::String GetIdentifier( void ){return "pop3";};

		void SetConnection( Server cServer );

		void SetServer( os::String cServer ){m_cServer = cServer;};
		void SetUsername( os::String cUsername ){m_cUsername = cUsername;};
		void SetPassword( os::String cPassword ){m_cPassword = cPassword;};
		void SetPort( uint nPort ){m_nPort = nPort;};

		uint64 GetFlags( void ){ return m_nFlags; };

		status_t Connect( void );
		status_t IsConnected( void ){return m_bIsConnected;};
		status_t Disconnect( void );

		bool IsRead( void ){return true;};

		int GetMessageCount( void );
		status_t Read( Mailmessage *pcMessage, int nIndex );
		status_t Delete( int nIndex );
	private:
		os::String m_cServer;
		uint m_nPort;
		os::String m_cUsername;
		os::String m_cPassword;

		uint64 m_nFlags;

		struct pop3_session *m_psSession;

		bool m_bIsConnected;
};

#endif

