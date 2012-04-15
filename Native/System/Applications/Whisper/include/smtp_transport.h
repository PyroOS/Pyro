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

#ifndef WHISPER_SMTP_TRANSPORT_H_
#define WHISPER_SMTP_TRANSPORT_H_

#include <transport.h>
#include <libsmtp.h>

class SmtpTransportNode : public TransportNode
{
	public:
		SmtpTransportNode(){};
		~SmtpTransportNode(){};

		os::String GetIdentifier( void ){return "smtp";};
		Transport * GetTransport( void );
};

#define SMTP_DEFAULT_PORT 25

class SmtpTransport : public Transport
{
	public:
		SmtpTransport( os::String cServer = "", os::String cUsername = "", os::String cPassword = "", uint nPort = SMTP_DEFAULT_PORT );
		~SmtpTransport( void );

		void SetConnection( Server cServer );

		void SetServer( os::String cServer ){m_cServer = cServer;};
		void SetUsername( os::String cUsername ){m_cUsername = cUsername;};
		void SetPassword( os::String cPassword ){m_cPassword = cPassword;};
		void SetPort( uint nPort ){m_nPort = nPort;};

		os::String GetIdentifier( void ){return "smtp";};

		uint64 GetFlags( void ){ return m_nFlags; };

		status_t Connect( void );
		status_t IsConnected( void ){return m_bIsConnected;};
		status_t Disconnect( void );

		bool IsWrite( void ){return true;};

		status_t Write( Mailmessage *pcMessage );
	private:
		os::String m_cServer;
		uint m_nPort;
		os::String m_cUsername;
		os::String m_cPassword;

		uint64 m_nFlags;

		struct smtp_session *m_psSession;

		bool m_bIsConnected;
};

#endif
