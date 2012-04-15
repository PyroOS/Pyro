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

#ifndef WHISPER_TRANSPORT_H_
#define WHISPER_TRANSPORT_H_

#include <mail.h>
#include <identity.h>

#include <util/string.h>
#include <util/thread.h>
#include <util/handler.h>
#include <util/messenger.h>

#include <list>

class Transport
{
	public:
		Transport( os::String cServer = "", os::String cUsername = "", os::String cPassword = "", uint nPort = 0 ){};
		virtual ~Transport( void ){};

		virtual os::String GetIdentifier( void ) = 0;

		virtual void SetConnection( Server cServer ) = 0;

		virtual void SetServer( os::String cServer ) = 0;
		virtual void SetUsername( os::String cUsername ) = 0;
		virtual void SetPassword( os::String cPassword ) = 0;
		virtual void SetPort( uint nPort ) = 0;

		virtual uint64 GetFlags( void ) = 0;

		virtual status_t Connect( void ) = 0;
		virtual status_t IsConnected( void ) = 0;
		virtual status_t Disconnect( void ) = 0;

		virtual bool IsRead( void ){return false;};
		virtual bool IsWrite( void ){return false;};

		virtual int GetMessageCount( void ){return 0;};
		virtual status_t Read( Mailmessage *pcMessage, int nIndex ){return ENOSYS;};
		virtual status_t Write( Mailmessage *pcMessage ){return ENOSYS;};
		virtual status_t Delete( int nIndex ){return ENOSYS;};
};

class TransportNode
{
	public:
		TransportNode(){};
		virtual ~TransportNode(){};

		virtual os::String GetIdentifier( void ) = 0;
		virtual Transport * GetTransport( void ) = 0;
};

class TransportFactory
{
	public:
		TransportFactory(){};
		~TransportFactory();

		static TransportFactory * GetFactory( void );

		Transport * FindTransport( const os::String cIdentifier );
	private:
		status_t _LoadAll( void );

		std::list <TransportNode*> m_vNodes;
};

class TransportWorker : public os::Thread
{
	public:
		TransportWorker( const char *pzName, Transport *pcTransport, os::Handler *pcTarget, os::String cPanelId );
		~TransportWorker(){};

		virtual int32 Run() = 0;

	protected:
		status_t _UpdateStatus( os::String cStatusMessage, int16 nTimeout );

		Transport *m_pcTransport;
		os::Handler *m_pcTarget;
		os::String m_cPanelId;
		os::Messenger *m_pcMessenger;
};

class InboundWorker : public TransportWorker
{
	public:
		InboundWorker( const char *pzName, Transport *pcTransport, os::Handler *pcTarget, os::String cPanelId );
		~InboundWorker(){};

		virtual int32 Run();
};

class OutboundWorker : public TransportWorker
{
	public:
		OutboundWorker( Mailfolder *pcOutbox, const char *pzName, Transport *pcTransport, os::String cAddress, Transport *pcAuth, os::Handler *pcTarget, os::String cPanelId );
		~OutboundWorker();

		virtual int32 Run();

	private:
		Mailfolder *m_pcOutbox;
		Transport *m_pcAuth;
		os::String m_cAddress;
};

#endif
