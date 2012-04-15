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

#ifndef WHISPER_SYLLABLE_MAILBOX_H_
#define WHISPER_SYLLABLE_MAILBOX_H_

#include <mailbox.h>
#include <storage/directory.h>
#include <storage/path.h>

#include <vector>

#define SYLLABLE_MAILBOX_VERSION	2

class SyllableMailfolder : public Mailfolder
{
	public:
		SyllableMailfolder( os::String cMailbox, os::String cMailfolder );
		virtual ~SyllableMailfolder();

		virtual bool IsValid( void ){return m_bIsValid;};
		virtual os::String GetPath( void ){return m_cMailfolder;};

		virtual status_t GetNextEntry( Mailsummery *pcSummery );

		virtual status_t Read( os::Variant &cReference, Mailmessage *pcMessage );
		virtual status_t Write( Mailmessage *pcMessage );

		virtual status_t ChangeStatus( Mailmessage *pcMessage );
		virtual status_t ChangeFlag( Mailmessage *pcMessage );

		virtual status_t SetUnreadCount( uint64 nCount );
		virtual uint64 GetUnreadCount( void );
		virtual uint64 DecUnreadCount( void );
		virtual uint64 IncUnreadCount( void );

		virtual status_t DeleteMessage( os::Variant &cReference );

		virtual int GetChildCount( void );
		virtual os::String GetChildName( int nChild );

		virtual status_t CreateFolder( os::String cFolder );
		virtual status_t Rename( os::String cName );
		virtual status_t Delete( void );

		virtual FolderProperties * GetProperties( void );
		virtual status_t SetProperties( FolderProperties *pcProperties );

	private:
		bool _Exists( const os::String cFolder ) const;

		os::String m_cMailbox;
		os::String m_cMailfolder;
		os::String m_cPath;
		os::Directory *m_pcDirectory;

		bool m_bIsValid;

		int m_nChildren;
		std::vector <os::String> m_vChildren;

		FolderProperties *m_pcProperties;
};

class SyllableMailboxNode : public MailboxNode
{
	public:
		SyllableMailboxNode(){};
		virtual ~SyllableMailboxNode(){};

		virtual os::String GetIdentifier( void ){return "syllable";};
		virtual Mailbox * GetMailbox( os::String cName );
};

/* Default folders. XXXKV: Localise me */
const struct
{
	os::String cFolder;
	folder_type_e nType;	
}
asDefaultFolders[] = 
{
	{ "Drafts", FL_TYPE_DRAFTS },
	{ "Inbox", FL_TYPE_INBOX },
	{ "Outbox", FL_TYPE_OUTBOX },
	{ "Sent", FL_TYPE_SENT },
	{ "Trash", FL_TYPE_TRASH }
};

class SyllableMailbox : public Mailbox
{
	public:
		SyllableMailbox( os::String cName );
		virtual ~SyllableMailbox(){};

		virtual bool IsReadOnly( void ){return false;};

		virtual os::String GetIdentifer( void ){return "syllable";};
		virtual os::String GetName( void ){return m_cName;};

		virtual Mailfolder * OpenFolder( os::String cPath );
	private:
		status_t Create( void );
		status_t Upgrade( os::Path cPath, int nCurrentVersion );

		os::String m_cName;
};

#endif
