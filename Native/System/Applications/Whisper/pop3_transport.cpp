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

#include <pop3_transport.h>

Transport * Pop3TransportNode::GetTransport( void )
{
	return new Pop3Transport();
}

Pop3Transport::Pop3Transport( os::String cServer, os::String cUsername, os::String cPassword, uint nPort ) : Transport( cServer, cUsername, cPassword, nPort )
{
	m_cServer = cServer;
	m_cUsername = cUsername;
	m_cPassword = cPassword;
	if( nPort > 0 )
		m_nPort = nPort;
	else
		m_nPort = POP3_DEFAULT_PORT;
	m_nFlags = 0;

	m_psSession = NULL;
	m_bIsConnected = false;
}

Pop3Transport::~Pop3Transport( void )
{
	if( m_bIsConnected )
		Disconnect();
}

void Pop3Transport::SetConnection( Server cServer )
{
	if( m_bIsConnected )
		Disconnect();

	m_cServer = cServer.GetServer();
	m_cUsername = cServer.GetUsername();
	m_cPassword = cServer.GetPassword();
	m_nPort = cServer.GetPort();
	if( 0 == m_nPort )
		m_nPort = POP3_DEFAULT_PORT;
	m_nFlags = cServer.GetFlags();
}

status_t Pop3Transport::Connect( void )
{
	status_t nError;

	if( m_bIsConnected )
		return EISCONN;

	m_psSession = pop3_create_session( m_cServer.c_str(), m_nPort, POP3_DEFAULT_TIMEOUT, 0 );
	if( NULL == m_psSession )
		return ENOTCONN;

	nError = pop3_connect( m_psSession, m_cUsername.c_str(), m_cPassword.c_str() );
	if( nError != 0 )
	{
		pop3_destroy_session( m_psSession );
		m_psSession = NULL;
	}
	else
		m_bIsConnected = true;

	return nError;
}

status_t Pop3Transport::Disconnect( void )
{
	status_t nError;

	if( false == m_bIsConnected )
		return ENOTCONN;

	if( NULL == m_psSession )
		return EINVAL;

	nError = pop3_disconnect( m_psSession );
	if( nError != 0 )
		return nError;

	nError = pop3_destroy_session( m_psSession );
	m_psSession = NULL;
	m_bIsConnected = false;

	return nError;
}

int Pop3Transport::GetMessageCount( void )
{
	int nCount, nSize;
	status_t nError;

	if( false == m_bIsConnected )
		return -1;

	nError = pop3_get_message_count( m_psSession, &nCount, &nSize );
	if( nError != 0 )
		return -1;

	return nCount;
}

status_t Pop3Transport::Read( Mailmessage *pcMessage, int nIndex )
{
	status_t nError;
	char *pcBuffer;
	size_t nSize;

	if( false == m_bIsConnected )
		return ENOTCONN;

	if( NULL == pcMessage || nIndex < 0 )
		return EINVAL;

	nError = pop3_get_message( m_psSession, nIndex, (void**)&pcBuffer, &nSize );
	if( nError != 0 )
	{
		if( pcBuffer != NULL )
			free( pcBuffer );
		return nError;
	}

	pcMessage->SetData( pcBuffer, nSize );
	free( pcBuffer );

	return nError;
}

status_t Pop3Transport::Delete( int nIndex )
{
	if( false == m_bIsConnected )
		return ENOTCONN;

	if( nIndex < 0 )
		return EINVAL;

	return pop3_delete_message( m_psSession, nIndex );
}

