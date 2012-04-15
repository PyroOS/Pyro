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

#include <smtp_transport.h>

Transport * SmtpTransportNode::GetTransport( void )
{
	return new SmtpTransport();
}

SmtpTransport::SmtpTransport( os::String cServer, os::String cUsername, os::String cPassword, uint nPort ) : Transport( cServer, cUsername, cPassword, nPort )
{
	m_cServer = cServer;
	m_cUsername = cUsername;
	m_cPassword = cPassword;

	if( nPort > 0 )
		m_nPort = nPort;
	else
		m_nPort = SMTP_DEFAULT_PORT;
	m_nFlags = 0;

	m_psSession = NULL;
	m_bIsConnected = false;
}

SmtpTransport::~SmtpTransport( void )
{
	if( IsConnected() )
		Disconnect();
}

void SmtpTransport::SetConnection( Server cServer )
{
	m_cServer = cServer.GetServer();
	m_cUsername = cServer.GetUsername();
	m_cPassword = cServer.GetPassword();
	m_nPort = cServer.GetPort();
	if( 0 == m_nPort )
		m_nPort = SMTP_DEFAULT_PORT;
	m_nFlags = cServer.GetFlags();
}

status_t SmtpTransport::Connect( void )
{
	status_t nError;

	if( m_bIsConnected )
		return EISCONN;

	m_psSession = smtp_create_session( m_cServer.c_str(), m_nPort, SMTP_DEFAULT_TIMEOUT, 0 );
	if( NULL == m_psSession )
		return ENOTCONN;

	if( m_cUsername == "" )
		nError = smtp_connect( m_psSession, NULL, NULL );
	else
		nError = smtp_connect( m_psSession, m_cUsername.c_str(), m_cPassword.c_str() );

	if( nError != 0 )
	{
		smtp_destroy_session( m_psSession );
		m_psSession = NULL;
	}
	else
		m_bIsConnected = true;

	return nError;
}

status_t SmtpTransport::Disconnect( void )
{
	status_t nError;

	if( false == m_bIsConnected )
		return ENOTCONN;

	if( NULL == m_psSession )
		return EINVAL;

	nError = smtp_disconnect( m_psSession );
	if( nError != 0 )
		return nError;

	nError = smtp_destroy_session( m_psSession );
	m_psSession = NULL;
	m_bIsConnected = false;

	return nError;
}

status_t SmtpTransport::Write( Mailmessage *pcMessage )
{
	status_t nError;

	if( false == m_bIsConnected )
		return ENOTCONN;

	if( NULL == pcMessage )
		return EINVAL;

	/* Create a new smtp mail */
	struct smtp_message *psSmtpMessage = smtp_create_message( pcMessage->GetAddress().c_str(), pcMessage->GetData() );
	if( NULL == psSmtpMessage )
		return EIO;

	/* Add the list of recipiants */
	std::list<os::String>vRecipiants;
	pcMessage->GetRecipiants( vRecipiants );

	std::list<os::String>::iterator i;
	for( i = vRecipiants.begin(); i != vRecipiants.end(); i++ )
		smtp_add_recipiant( psSmtpMessage, (*i).c_str() );

	/* Send it */
	nError = smtp_send( m_psSession, psSmtpMessage );
	smtp_destroy_message( psSmtpMessage );

	return nError;
}

