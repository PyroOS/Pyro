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

#include <identity.h>

#include <debug.h>

using namespace os;

Identity::Identity( void )
{
	m_cInstance =
	m_cName =
	m_cAddress =
	m_cFrom = "";
}

Identity::Identity( os::String cInstance )
{
	m_cInstance = cInstance;

	Settings cSettings;
	Path cPath = cSettings.GetPath();
	cPath.Append( "/Identities" );
	cSettings.SetPath( &cPath );
	cSettings.SetFile( m_cInstance );

	cSettings.Load();

	debug( "Loading identity %s\n", cSettings.GetPath().GetPath().c_str() );

	int nIndex, nCount;

	/* Find all mailboxes.  The default value is 1 so that a default mailbox
	   is still be created by the for() loop. */
	nCount = cSettings.GetInt32( "mailbox_count", 0 );
	if( 0 == nCount )
		nCount = 1;

	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		String cIdentifier, cName;
		cIdentifier = cSettings.GetString( "mailbox_identifier", "syllable", nIndex );
		cName = cSettings.GetString( "mailbox_name", "Mail", nIndex );

		MailboxId cBoxId( cIdentifier, cName );
		m_vBoxIds.push_back( cBoxId );
	}

	/* Get Servers */
	nCount = cSettings.GetInt32( "server_count", 0 );
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		Server cServer;
		cServer.SetTransport( cSettings.GetString( "server_transport", "", nIndex ) );
		cServer.SetServer( cSettings.GetString( "server_server", "", nIndex ) );
		cServer.SetUsername( cSettings.GetString( "server_username", "", nIndex ) );
		cServer.SetPassword( cSettings.GetString( "server_password", "", nIndex ) );
		cServer.SetPort( cSettings.GetInt32( "server_port", 0, nIndex ) );
		cServer.SetFlags( cSettings.GetInt64( "server_flags", 0, nIndex ) );
		cServer.SetData( cSettings.GetVariant( "server_data", 0, nIndex ) );

		switch( cSettings.GetInt32( "server_type", 0, nIndex ) )
		{
			case SERVER_INBOUND:
			{
				m_vInboundServers.push_back( cServer );
				break;
			}
			case SERVER_OUTBOUND:
			{
				m_vOutboundServers.push_back( cServer );
				break;
			}
		}
	}

	/* Get signature & from address */
	m_cName = cSettings.GetString( "name" );
	m_cAddress = cSettings.GetString( "address" );
	m_cFrom = cSettings.GetString( "from" );

	/* Get signatures */
	nCount = cSettings.GetInt32( "signature_count", 0 );
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		std::pair<os::String, os::String> cSignature;

		cSignature.first = cSettings.GetString( "signature_name", "", nIndex );
		cSignature.second = cSettings.GetString( "signature_text", "", nIndex );

		m_vSignatures.push_back( cSignature );
	}
}

Identity::~Identity()
{
	m_vBoxIds.clear();
}

status_t Identity::GetMailboxId( MailboxId &cMailboxId, uint nIndex )
{
	if( nIndex >= m_vBoxIds.size() )
		return EINVAL;

	cMailboxId = m_vBoxIds[nIndex];
	return EOK;
}

void Identity::AddMailboxId( MailboxId cMailboxId )
{
	m_vBoxIds.push_back( cMailboxId );
}

status_t Identity::GetServer( Server &cServer, server_type eType, uint nIndex )
{
	status_t nError = EOK;

	switch( eType )
	{
		case SERVER_INBOUND:
		{
			if( nIndex >= m_vInboundServers.size() )
			{
				nError = EINVAL;
				break;
			}

			cServer = m_vInboundServers[nIndex];
			break;
		}

		case SERVER_OUTBOUND:
		{
			if( nIndex >= m_vOutboundServers.size() )
			{
				nError = EINVAL;
				break;
			}

			cServer = m_vOutboundServers[nIndex];
			break;
		}
	}

	return nError;
}

void Identity::AddServer( Server cServer, server_type eType )
{
	switch( eType )
	{
		case SERVER_INBOUND:
		{
			m_vInboundServers.push_back( cServer );
			break;
		}

		case SERVER_OUTBOUND:
		{
			m_vOutboundServers.push_back( cServer );
			break;
		}
	}
}

void Identity::AddSignature( const os::String cName, const os::String cText )
{
	std::pair<os::String, os::String> cSignature;

	cSignature.first = cName;
	cSignature.second = cText;

	m_vSignatures.push_back( cSignature );
}

status_t Identity::GetSignature( os::String &cName, os::String &cText, uint nIndex )
{
	if( nIndex < 0 || nIndex >= m_vSignatures.size() )
		return EINVAL;

	cName = m_vSignatures[nIndex].first;
	cText = m_vSignatures[nIndex].second;

	return EOK;
}

void Identity::SetName( os::String cName )
{
	m_cName = cName;
	m_cFrom = os::String( "\"" ) +
			  m_cName +
			  os::String( "\" <" ) +
			  m_cAddress +
			  os::String( ">" );
}

void Identity::SetAddress( os::String cAddress )
{
	m_cAddress = cAddress;
	m_cFrom = os::String( "\"" ) +
			  m_cName +
			  os::String( "\" <" ) +
			  m_cAddress +
			  os::String( ">" );
}

void Identity::SetFrom( os::String cFrom )
{
	m_cFrom = cFrom;
}

status_t Identity::Save( void )
{
	return Save( m_cInstance );
}

status_t Identity::Save( os::String cInstance )
{
	Settings cSettings;
	Path cPath = cSettings.GetPath();
	cPath.Append( "/Identities" );
	cSettings.SetPath( &cPath );
	cSettings.SetFile( cInstance );

	uint nIndex, nCount, nItem;

	nCount = m_vBoxIds.size();
	cSettings.SetInt32( "mailbox_count", nCount );
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		cSettings.SetString( "mailbox_identifier", m_vBoxIds[nIndex].GetIdentifier(), nIndex );
		cSettings.SetString( "mailbox_name", m_vBoxIds[nIndex].GetName(), nIndex );
	}

	cSettings.SetString( "name", m_cName );
	cSettings.SetString( "address", m_cAddress );
	cSettings.SetString( "from", m_cFrom );

	nCount = m_vSignatures.size();
	cSettings.SetInt32( "signature_count", nCount );
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		cSettings.SetString( "signature_name", m_vSignatures[nIndex].first, nIndex );
		cSettings.SetString( "signature_text", m_vSignatures[nIndex].second, nIndex );
	}

	nCount = m_vInboundServers.size() + m_vOutboundServers.size();
	cSettings.SetInt32( "server_count", nCount );

	/* All inbound servers */
	for( nItem = nIndex = 0; nItem < m_vInboundServers.size(); nItem++, nIndex++ )
	{
		cSettings.SetInt32( "server_type", SERVER_INBOUND, nIndex );

		cSettings.SetString( "server_transport", m_vInboundServers[nItem].GetTransport(), nIndex );
		cSettings.SetString( "server_server", m_vInboundServers[nItem].GetServer(), nIndex );
		cSettings.SetString( "server_username", m_vInboundServers[nItem].GetUsername(), nIndex );
		cSettings.SetString( "server_password", m_vInboundServers[nItem].GetPassword(), nIndex );

		cSettings.SetInt32( "server_port", m_vInboundServers[nItem].GetPort(), nIndex );
		cSettings.SetInt64( "server_flags", m_vInboundServers[nItem].GetFlags(), nIndex );

		cSettings.SetVariant( "server_data", m_vInboundServers[nItem].GetData(), nIndex );
	}

	/* All outbound servers */
	for( nItem = 0; nItem < m_vOutboundServers.size(); nItem++, nIndex++ )
	{
		cSettings.SetInt32( "server_type", SERVER_OUTBOUND, nIndex );

		cSettings.SetString( "server_transport", m_vOutboundServers[nItem].GetTransport(), nIndex );
		cSettings.SetString( "server_server", m_vOutboundServers[nItem].GetServer(), nIndex );
		cSettings.SetString( "server_username", m_vOutboundServers[nItem].GetUsername(), nIndex );
		cSettings.SetString( "server_password", m_vOutboundServers[nItem].GetPassword(), nIndex );

		cSettings.SetInt32( "server_port", m_vOutboundServers[nItem].GetPort(), nIndex );
		cSettings.SetInt64( "server_flags", m_vOutboundServers[nItem].GetFlags(), nIndex );

		cSettings.SetVariant( "server_data", m_vOutboundServers[nItem].GetData(), nIndex );
	}

	/* Write to disk */
	return cSettings.Save();
}

