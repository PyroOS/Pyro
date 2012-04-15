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

#ifndef WHISPER_MAILBOX_H_
#define WHISPER_MAILBOX_H_

#include <pyro/types.h>
#include <util/string.h>
#include <util/variant.h>
#include <util/locker.h>

#include <mail.h>

#include <list>

typedef enum folder_type
{
	FL_TYPE_NORMAL = 0,
	FL_TYPE_INBOX,
	FL_TYPE_OUTBOX,
	FL_TYPE_SENT,
	FL_TYPE_DRAFTS,
	FL_TYPE_TRASH
} folder_type_e;

typedef enum folder_flags
{
	FL_NONE = 0x00,
	FL_HAS_LIST = 0x01,
	FL_ARCHIVE = 0x02
} folder_flags_e;

class FolderProperties
{
	public:
		FolderProperties()
		{
			m_nType = FL_TYPE_NORMAL;
			m_nFlags = FL_NONE;

			m_cListAddress = "";
		};
		~FolderProperties(){};

		uint64 GetType( void )
		{
			return m_nType;
		};
		void SetType( uint64 nType )
		{
			m_nType = nType;
		};

		uint64 GetFlags( void )
		{
			return m_nFlags;
		};
		void SetFlags( uint64 nFlags )
		{
			m_nFlags = nFlags;
		};

		os::String GetListAddress( void )
		{
			return m_cListAddress;
		};
		void SetListAddress( os::String cAddress )
		{
			m_cListAddress = cAddress;
		};

		FolderProperties & operator=( FolderProperties cProperties )
		{
			m_nType = cProperties.m_nType;
			m_nFlags = cProperties.m_nFlags;
			m_cListAddress = cProperties.m_cListAddress;

			return( *this );
		};
		FolderProperties & operator=( FolderProperties *pcProperties )
		{
			m_nType = pcProperties->m_nType;
			m_nFlags = pcProperties->m_nFlags;
			m_cListAddress = pcProperties->m_cListAddress;

			return( *this );
		};

	private:
		uint64 m_nType;
		uint64 m_nFlags;

		os::String m_cListAddress;
};

class FolderReference
{
	public:
		FolderReference()
		{
			m_eType = FL_TYPE_INBOX;
			m_cName = "Inbox";
		};
		folder_type_e m_eType;
		os::String m_cName;
};

class Mailfolder : public os::Locker
{
	public:
		Mailfolder( os::String cParent = "", os::String cFolder = "" ) : os::Locker( "mailbox", false, false ){};
		virtual ~Mailfolder(){};

		virtual bool IsValid( void ){return false;};
		virtual os::String GetPath( void ){return "";};

		virtual status_t GetNextEntry( Mailsummery *pcSummery ){return ENOSYS;};

		virtual status_t Read( os::Variant &cReference, Mailmessage *pcMessage ){return ENOSYS;};
		virtual status_t Write( Mailmessage *pcMessage ){return ENOSYS;};

		virtual status_t ChangeStatus( Mailmessage *pcMessage ){return ENOSYS;};
		virtual status_t ChangeFlag( Mailmessage *pcMessage ){return ENOSYS;};

		virtual status_t SetUnreadCount( uint64 nCount ){return ENOSYS;};
		virtual uint64 GetUnreadCount( void ){return 0;};
		virtual uint64 DecUnreadCount( void ){return 0;};
		virtual uint64 IncUnreadCount( void ){return 0;};

		virtual status_t DeleteMessage( os::Variant &cReference ){return ENOSYS;};

		virtual int GetChildCount( void ){return 0;};
		virtual os::String GetChildName( int nChild ){return "";};

		virtual status_t CreateFolder( os::String cFolder ){return ENOSYS;};
		virtual status_t Rename( os::String cName ){return ENOSYS;};
		virtual status_t Delete( void ){return ENOSYS;};

		virtual FolderProperties * GetProperties( void ){return NULL;};
		virtual status_t SetProperties( FolderProperties *pcProperties ){return ENOSYS;};
};

class Mailbox
{
	public:
		Mailbox( os::String cName = "" ){};
		virtual ~Mailbox(){};

		virtual bool IsReadOnly( void ){return true;};

		virtual os::String GetIdentifer( void ) = 0;
		virtual os::String GetName( void ) = 0;

		virtual Mailfolder * OpenFolder( os::String cPath ) = 0;
};

class MailboxNode
{
	public:
		MailboxNode(){};
		virtual ~MailboxNode(){};

		virtual os::String GetIdentifier( void ) = 0;
		virtual Mailbox * GetMailbox( os::String cName ) = 0;
};

class MailboxId
{
	public:
		MailboxId( os::String cIdentifier = "", os::String cName = "" )
		{
			m_cIdentifier = cIdentifier;
			m_cName = cName;
		};
		os::String GetIdentifier( void ){return m_cIdentifier;};
		void SetIdentifier( os::String cIdentifier ){m_cIdentifier = cIdentifier;};

		os::String GetName( void ){return m_cName;};
		void SetName( os::String cName ){m_cName = cName;};

		MailboxId & operator=( MailboxId &cId )
		{
			m_cIdentifier = cId.m_cIdentifier;
			m_cName = cId.m_cName;

			return( *this );
		};
	private:
		os::String m_cIdentifier;
		os::String m_cName;
};

class MailboxFactory
{
	public:
		MailboxFactory(){};
		~MailboxFactory();

		static MailboxFactory * GetFactory( void );

		Mailbox * FindMailbox( const os::String cIdentifier, os::String cName );
	private:
		status_t _LoadAll( void );

		std::list <MailboxNode*> m_vNodes;
};

#endif
