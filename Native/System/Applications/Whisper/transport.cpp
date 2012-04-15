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

#include <transport.h>
#include <pop3_transport.h>
#include <smtp_transport.h>
#include <messages.h>
#include <resources/Whisper.h>

static TransportFactory *g_pcFactory = NULL;

TransportFactory::~TransportFactory()
{
	std::list<TransportNode*>::const_iterator i;
	for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
		delete( (*i) );
	m_vNodes.clear();
}

TransportFactory * TransportFactory::GetFactory( void )
{
	if( NULL == g_pcFactory )
	{
		g_pcFactory = new TransportFactory();
		if( g_pcFactory->_LoadAll() != EOK )
		{
			delete( g_pcFactory );
			g_pcFactory = NULL;
		}
	}

	return g_pcFactory;
}

Transport * TransportFactory::FindTransport( const os::String cIdentifier )
{
	Transport *pcTransport = NULL;

	std::list<TransportNode*>::const_iterator i;
	for( i = m_vNodes.begin(); i != m_vNodes.end(); i++ )
	{
		if( (*i)->GetIdentifier() == cIdentifier )
		{
			pcTransport = (*i)->GetTransport();
			break;
		}
	}

	return pcTransport;
}

status_t TransportFactory::_LoadAll( void )
{
	/* POP3 & SMTP are internal */
	m_vNodes.push_back( new Pop3TransportNode() );
	m_vNodes.push_back( new SmtpTransportNode() );

	/* XXXKV: Load plugins and add nodes to the list */

	return EOK;
}

TransportWorker::TransportWorker( const char *pzName, Transport *pcTransport, os::Handler *pcTarget, os::String cPanelId )
	: os::Thread( pzName )
{
	m_pcTransport = pcTransport;
	m_pcTarget = pcTarget;
	m_pcMessenger = new os::Messenger( m_pcTarget );
	m_cPanelId = cPanelId;
}

status_t TransportWorker::_UpdateStatus( os::String cStatusMessage, int16 nTimeout )
{
	os::Message *pcMessage = new os::Message( M_STATUS_UPDATE );
	pcMessage->AddString( "panel", m_cPanelId );
	pcMessage->AddString( "message", cStatusMessage );
	pcMessage->AddInt16( "timeout", nTimeout );

	return m_pcMessenger->SendMessage( pcMessage );	
}


InboundWorker::InboundWorker( const char *pzName, Transport *pcTransport, os::Handler *pcTarget, os::String cPanelId )
	: TransportWorker( pzName, pcTransport, pcTarget, cPanelId )
{
}

int32 InboundWorker::Run()
{
	status_t nError;
	char zMessage[256];

	if( NULL == m_pcTransport || NULL == m_pcTarget )
		return EINVAL;

	if( m_pcTransport->IsRead() == false )
		return EIO;

	/* XXXKV: This message needs more information E.g. server name */
	_UpdateStatus( MSG_STATUS_CONNECTINGTOSERVER.c_str(), 0 );

	/* Attempt to connect the transport */
	nError = m_pcTransport->Connect();
	if( nError != EOK )
	{
		/* XXXKV: Same as above; more info */
		_UpdateStatus( MSG_STATUS_FAILEDTOCONNECTTOSERVER.c_str(), 5 );
		return (int32)nError;
	}

	/* Get each mail on the server and pass to the main thread to filter & store */
	int nCount = m_pcTransport->GetMessageCount();
	if( nCount < 1 )
	{
		_UpdateStatus( MSG_STATUS_NOMAILONSERVER.c_str(), 3 );
		m_pcTransport->Disconnect();
		return EOK;
	}

	int nRead = 0;
	uint64 nFlags = m_pcTransport->GetFlags();
	for( int nIndex = 1; nIndex < nCount + 1; nIndex++ )
	{
		sprintf( zMessage, MSG_STATUS_RETRIEVINGMESSAGE.c_str(), nIndex, nCount, ( 100 / nCount ) * nIndex );
		_UpdateStatus( zMessage, 0 );

		Mailmessage *pcMail = new Mailmessage();
		nError = m_pcTransport->Read( pcMail, nIndex );
		if( nError != EOK )
		{
			delete( pcMail );
			continue;
		}
		else
			nRead++;

		/* Pass message to the main thread */
		os::Message *pcMessage = new os::Message( M_NEW_MAIL );
		pcMessage->AddPointer( "message", pcMail );

		m_pcMessenger->SendMessage( pcMessage );

		/* Delete the email if required */
		if( nFlags & DELETE_ON_READ )
			nError = m_pcTransport->Delete( nIndex ); 
	}

	sprintf( zMessage, MSG_STATUS_MESSAGESSUCCESSFULLYRETRIEVED.c_str(), nRead, nCount );
	_UpdateStatus( zMessage, 3 );

	/* Tell the main thread to update the display */
	os::Message *pcMessage = new os::Message( M_NEW_MAIL_COMPLETE );
	m_pcMessenger->SendMessage( pcMessage );

	m_pcTransport->Disconnect();
	return EOK;
}

OutboundWorker::OutboundWorker( Mailfolder *pcOutbox, const char *pzName, Transport *pcTransport, os::String cAddress, Transport *pcAuth, os::Handler *pcTarget, os::String cPanelId )
	: TransportWorker( pzName, pcTransport, pcTarget, cPanelId )
{
	m_pcOutbox = pcOutbox;
	m_pcAuth = pcAuth;
	m_cAddress = cAddress;
}

OutboundWorker::~OutboundWorker()
{
	if( m_pcOutbox )
		delete( m_pcOutbox );
}

int32 OutboundWorker::Run()
{
	status_t nError;
	char zMessage[256];

	if( NULL == m_pcTransport || NULL == m_pcTarget || NULL == m_pcOutbox )
		return EINVAL;

	if( m_pcTransport->IsWrite() == false )
		return EIO;

	/* If we have an "Auth" transport, connect that first.  This is required for E.g.
	   POP-before-SMTP auth. schemes */
	if( NULL != m_pcAuth )
	{
		/* XXXKV: This message needs more information E.g. server name */
		_UpdateStatus( MSG_STATUS_AUTHENTICATING.c_str(), 0 );

		nError = m_pcAuth->Connect();
		if( nError != EOK )
		{
			/* XXXKV: Same as above; more info */
			_UpdateStatus( MSG_STATUS_FAILEDTOCONNECTTOSERVER.c_str(), 5 );
			return (int32)nError;
		}
	}

	/* XXXKV: This message needs more information E.g. server name */
	_UpdateStatus( MSG_STATUS_CONNECTINGTOSERVER.c_str(), 0 );

	/* Attempt to connect the transport */
	nError = m_pcTransport->Connect();
	if( nError != EOK )
	{
		if( NULL != m_pcAuth )
			m_pcAuth->Disconnect();

		/* XXXKV: Same as above; more info */
		_UpdateStatus( MSG_STATUS_CONNECTINGTOSERVER.c_str(), 5 );
		return (int32)nError;
	}

	/* Read each mail in the inbox and send */
	Mailsummery cSummery;
	Mailmessage *pcMailMessage;
	int nCount = 1;
	bool bFailure = false;

	while( ( nError = m_pcOutbox->GetNextEntry( &cSummery ) ) != ENOENT )
	{
		if( EISDIR == nError )
			continue;

		sprintf( zMessage, MSG_STATUS_SENDINGMESSAME.c_str(), nCount );
		_UpdateStatus( zMessage, 0 );

		pcMailMessage = new Mailmessage();

		m_pcOutbox->Read( cSummery.cReference, pcMailMessage );
		pcMailMessage->ParseHeaders();
		pcMailMessage->Validate();

		pcMailMessage->SetAddress( m_cAddress );

		nError = m_pcTransport->Write( pcMailMessage );
		delete( pcMailMessage );

		if( 0 != nError )
		{
			sprintf( zMessage, MSG_STATUS_FAILEDTOSENDMESSAGE.c_str(), nCount );
			_UpdateStatus( zMessage, 5 );

			bFailure = true;
		}
		else
		{
			sprintf( zMessage, MSG_STATUS_MESSAGESENT.c_str(), nCount );
			_UpdateStatus( zMessage, 3 );

			/* Tell the main application to move the message to the Sent folder */
			os::Message *pcMessage = new os::Message( M_MAIL_SENT );
			pcMessage->AddVariant( "reference", cSummery.cReference );
			m_pcMessenger->SendMessage( pcMessage );
		}

		nCount++;
	}

	m_pcTransport->Disconnect();

	/* Disconnect the auth transport, if there is one */
	if( NULL != m_pcAuth )
		m_pcAuth->Disconnect();

	if( bFailure )
		return EIO;
	else
		return EOK;
}

