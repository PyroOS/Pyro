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

#include <syllable_mailbox.h>
#include <storage/file.h>
#include <storage/filereference.h>

#include <stdio.h>

#include <rfctime.h>

#include <debug.h>

using namespace os;

SyllableMailfolder::SyllableMailfolder( String cMailbox, String cMailfolder )
{
	m_cMailbox = cMailbox;
	m_cMailfolder = cMailfolder;
	m_pcProperties = new FolderProperties();
	m_bIsValid = false;

	/* An explanation of how the Mailbox name & Mailfolder relate to each other may be required.
	   All Mailboxes have names.  This is primarily for the user benefit when they have multiple
	   Mailboxes.  Mailboxes have folders, and Mail is only stored within folders.
	   For Syllable Mailboxes the name is used as a subfolder within the users ~/Mail directory.
	   The Mailbox name and Mailfolder path are simply concatenated together to provide the
	   on-filesystem location of any folder.
	   Other types of Mailbox may choose to use the Mailbox name in a different manner.
	*/

	try
	{
		Path cMailboxPath( getenv( "HOME" ) );
		cMailboxPath.Append( "/Mail/" );
		cMailboxPath.Append( cMailbox );
		cMailboxPath.Append( "/" );
		cMailboxPath.Append( cMailfolder );

		m_cPath = cMailboxPath.GetPath();
		m_pcDirectory = new Directory( cMailboxPath );

		/* Look for children */
		ssize_t nBytes;
		int32 nChildren;

		nBytes = m_pcDirectory->ReadAttr( "children", ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );
		if( nBytes > 0 )
			m_nChildren = nChildren;
		else
			m_nChildren = 0;

		if( m_nChildren > 0 )
		{
			char pcAttrName[10], pcBuf[4096];
			for( int nChild = 0; nChild < m_nChildren; nChild++ )
			{
				memset( pcBuf, 0, 4096 );

				sprintf( pcAttrName, "child%i", nChild );
				nBytes = m_pcDirectory->ReadAttr( pcAttrName, ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
				if( nBytes > 0 )
					m_vChildren.push_back( String( pcBuf ) );
				else
					m_vChildren.push_back( String( "Unknown" ) );
			}		
		}

		/* Load the folder properties */
		int64 nProp;
		nBytes = m_pcDirectory->ReadAttr( "type", ATTR_TYPE_INT64, (void*)&nProp, 0, sizeof( nProp ) );
		if( nBytes > 0 )
			m_pcProperties->SetType( nProp );

		nBytes = m_pcDirectory->ReadAttr( "flags", ATTR_TYPE_INT64, (void*)&nProp, 0, sizeof( nProp ) );
		if( nBytes > 0 )
			m_pcProperties->SetFlags( nProp );

		if( m_pcProperties->GetFlags() & FL_HAS_LIST )
		{
			char pcBuf[4096] = {0};
			nBytes = m_pcDirectory->ReadAttr( "list_address", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
			if( nBytes > 0 )
				m_pcProperties->SetListAddress( String( pcBuf ) );
		}

		m_bIsValid = true;
	}
	catch( std::exception &e )
	{
		m_pcDirectory = NULL;
		debug( "%s: %s\n", cMailfolder.c_str(), e.what() );
	}
}

SyllableMailfolder::~SyllableMailfolder()
{
	if( m_pcDirectory )
		delete( m_pcDirectory );

	if( m_pcProperties )
		delete( m_pcProperties );
}

status_t SyllableMailfolder::GetNextEntry( Mailsummery *pcSummery )
{
	if( NULL == pcSummery )
		return EINVAL;

	String cEntry;

	while( cEntry == "" || cEntry == "." || cEntry == ".." )
		if( m_pcDirectory->GetNextEntry( &cEntry ) == 0 )
			return ENOENT;

	pcSummery->cReference = cEntry;

	try
	{
		ssize_t nBytes;
		char pcBuf[4096];
		int32 nStatus;
		int32 nFlag;
		int32 nAttachments;

		FSNode cNode( m_cPath + "/" + cEntry );
		if( cNode.IsFile() == false )
			return EISDIR;

		File cFile( cNode );

		nBytes = cFile.ReadAttr( "status", ATTR_TYPE_INT32, (void*)&nStatus, 0, sizeof( nStatus ) );
		if( nBytes > 0 )
			pcSummery->nStatus = nStatus;
		else
			pcSummery->nStatus = STATUS_UNREAD;

		nBytes = cFile.ReadAttr( "flag", ATTR_TYPE_INT32, (void*)&nFlag, 0, sizeof( nFlag ) );
		if( nBytes > 0 )
			pcSummery->nFlag = nFlag;
		else
			pcSummery->nFlag = FLAG_NONE;

		nBytes = cFile.ReadAttr( "attachments", ATTR_TYPE_INT32, (void*)&nAttachments, 0, sizeof( nAttachments ) );
		if( nBytes > 0 )
			pcSummery->nAttachments = nAttachments;
		else
			pcSummery->nAttachments = 0;

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "subject", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcSummery->cSubject = pcBuf;
		else
			pcSummery->cSubject = "<?>";
			
		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "from", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcSummery->cFrom = pcBuf;
		else
			pcSummery->cFrom = "<?>";

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "date", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcSummery->nDate = convert_date( pcBuf );
		else
			pcSummery->nDate = 0;

	}
	catch( std::exception &e )
	{
		debug( "%s/%s: %s\n", m_cPath.c_str(), cEntry.c_str(), e.what() );
		return EIO;
	}

	return EOK;
}

status_t SyllableMailfolder::Read( Variant &cReference, Mailmessage *pcMessage )
{
	String cRef = cReference.AsString();

	if( NULL == pcMessage )
		return EINVAL;

	try
	{
		ssize_t nSize, nBytes;
		char pcBuf[4096];
		char *pcBuffer;

		File cFile( m_cPath + "/" + cRef );
		nSize = cFile.GetSize();

		pcBuffer = (char*)calloc( 1, nSize + 1 );
		if( NULL == pcBuffer )
			return ENOMEM;

		if( cFile.Read( pcBuffer, nSize ) < nSize )
		{
			debug( "%s: Failed to read all data\n", cRef.c_str() );
			free( pcBuffer );
			return EIO;
		}

		pcMessage->SetData( pcBuffer, nSize );
		free( pcBuffer );
		pcMessage->SetReference( cReference );

		/* Get headers from the attributes */
		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "to", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetTo( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "cc", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetCc( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "bcc", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetBcc( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "reply_to", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetReplyTo( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "subject", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetSubject( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "in_reply_to", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetInReplyTo( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "message_id", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetId( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "from", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetFrom( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "sender", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetSender( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "date", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetDate( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "content_type", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetContent( pcBuf );

		memset( pcBuf, 0, 4096 );
		nBytes = cFile.ReadAttr( "content_encoding", ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
		if( nBytes > 0 )
			pcMessage->SetEncoding( pcBuf );

		int32 nStatus;
		nBytes = cFile.ReadAttr( "status", ATTR_TYPE_INT32, (void*)&nStatus, 0, sizeof( int32 ) );
		if( nBytes > 0 )
			pcMessage->SetStatus( nStatus );

		int32 nFlag;
		nBytes = cFile.ReadAttr( "flag", ATTR_TYPE_INT32, (void*)&nFlag, 0, sizeof( nFlag ) );
		if( nBytes > 0 )
			pcMessage->SetFlag( nFlag );
	}
	catch( std::exception &e )
	{
		debug( "%s/%s: %s\n", m_cPath.c_str(), cRef.c_str(), e.what() );
		return EIO;
	}

	return EOK;
}

status_t SyllableMailfolder::Write( Mailmessage *pcMessage )
{
	if( NULL == pcMessage )
		return EINVAL;

	/* Generate a suitable reference (In this instance, a file-name) for this mail.  Each folder
	   has a counter "ref_count" which is incremented every time a mail is written to the folder.
	   It is never decremented.  This reference number is used to generate the first 16-digit string
	   of the mail, which ensures that the mail on disk is always ordered for us in some sort of
	   sane manner. */

	int64 nRefCount;
	if( m_pcDirectory->ReadAttr( "ref_count", ATTR_TYPE_INT64, (void*)&nRefCount, 0, sizeof( int64 ) ) < 0 )
		nRefCount = 0;
	nRefCount += 1;

	char pzFileRef[_POSIX_PATH_MAX] = {0};
	sprintf( pzFileRef, "%.16Li+%s", nRefCount, pcMessage->GetId().c_str() );

	String cRef = m_cPath + "/" + pzFileRef;

	try
	{
		File cFile( cRef, O_CREAT );
		ssize_t nSize = pcMessage->GetDataSize();

		if( cFile.Write( pcMessage->GetData(), nSize ) < nSize )
		{
			debug( "%s: Failed to write all data\n", cRef.c_str() );
			return EIO;
		}

		/* Set the reference for the callers information */
		pcMessage->SetReference( String( pzFileRef ) );

		/* Write all attributes */
		String cAttr;

		cAttr = pcMessage->GetTo();
		cFile.WriteAttr( "to", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetCc();
		cFile.WriteAttr( "cc", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetBcc();
		cFile.WriteAttr( "bcc", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetReplyTo();
		cFile.WriteAttr( "reply_to", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetSubject();
		cFile.WriteAttr( "subject", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetInReplyTo();
		cFile.WriteAttr( "in_reply_to", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetId();
		cFile.WriteAttr( "message_id", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetFrom();
		cFile.WriteAttr( "from", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetSender();
		cFile.WriteAttr( "sender", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetDate();
		cFile.WriteAttr( "date", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetContent();
		cFile.WriteAttr( "content_type", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );
		cAttr = pcMessage->GetEncoding();
		cFile.WriteAttr( "content_encoding", 0, ATTR_TYPE_STRING, (void*)cAttr.c_str(), 0, cAttr.Length() );

		int32 nStatus = pcMessage->GetStatus();
		cFile.WriteAttr( "status", 0, ATTR_TYPE_INT32, (void*)&nStatus, 0, sizeof( nStatus ) );

		int32 nFlag = pcMessage->GetFlag();
		cFile.WriteAttr( "flag", 0, ATTR_TYPE_INT32, (void*)&nFlag, 0, sizeof( nFlag ) );

		int32 nAttchments = pcMessage->GetAttachmentCount();
		cFile.WriteAttr( "attachments", 0, ATTR_TYPE_INT32, (void*)&nAttchments, 0, sizeof( nAttchments ) );

		/* Update the reference count */
		m_pcDirectory->WriteAttr( "ref_count", 0, ATTR_TYPE_INT64, (void*)&nRefCount, 0,  sizeof( int64 ) );
	}
	catch( std::exception &e )
	{
		debug( "%s/%s: %s\n", m_cPath.c_str(), cRef.c_str(), e.what() );
		return EIO;
	}

	return EOK;
}

status_t SyllableMailfolder::ChangeStatus( Mailmessage *pcMessage )
{
	if( NULL == pcMessage )
		return EINVAL;

	String cRef = m_cPath + "/" + pcMessage->GetReference().AsString();

	try
	{
		File cFile( cRef, O_CREAT );

		int32 nStatus = pcMessage->GetStatus();
		cFile.WriteAttr( "status", 0, ATTR_TYPE_INT32, (void*)&nStatus, 0, sizeof( nStatus ) );
	}
	catch( std::exception &e )
	{
		debug( "%s/%s: %s\n", m_cPath.c_str(), cRef.c_str(), e.what() );
		return EIO;
	}

	return EOK;
}

status_t SyllableMailfolder::ChangeFlag( Mailmessage *pcMessage )
{
	if( NULL == pcMessage )
		return EINVAL;

	String cRef = m_cPath + "/" + pcMessage->GetReference().AsString();

	try
	{
		File cFile( cRef, O_CREAT );

		int32 nFlag = pcMessage->GetFlag();
		cFile.WriteAttr( "flag", 0, ATTR_TYPE_INT32, (void*)&nFlag, 0, sizeof( nFlag ) );
	}
	catch( std::exception &e )
	{
		debug( "%s/%s: %s\n", m_cPath.c_str(), cRef.c_str(), e.what() );
		return EIO;
	}

	return EOK;
}

status_t SyllableMailfolder::SetUnreadCount( uint64 nCount )
{
	debug( "set unread count %Lu\n", nCount );

	return m_pcDirectory->WriteAttr( "unread", 0, ATTR_TYPE_INT64, (void*)&nCount, 0,  sizeof( nCount ) );
}

uint64 SyllableMailfolder::GetUnreadCount( void )
{
	uint64 nCount;
	if( m_pcDirectory->ReadAttr( "unread", ATTR_TYPE_INT64, (void*)&nCount, 0, sizeof( nCount ) ) < 0 )
		nCount = 0;

	debug( "got unread count %Lu\n", nCount );

	return nCount;
}

uint64 SyllableMailfolder::DecUnreadCount( void )
{
	uint64 nCount = GetUnreadCount();
	if( nCount > 0 )
	{
		nCount--;
		SetUnreadCount( nCount );
	}

	/* XXXKV: Debug */
	printf( "dec unread count %Lu\n", nCount );

	return nCount;
}

uint64 SyllableMailfolder::IncUnreadCount( void )
{
	uint64 nCount = GetUnreadCount();
	nCount++;
	SetUnreadCount( nCount );

	/* XXXKV: Debug */
	printf( "inc unread count %Lu\n", nCount );

	return nCount;
}

status_t SyllableMailfolder::DeleteMessage( Variant &cReference )
{
	String cName = cReference.AsString();

	try
	{
		FileReference cRef( m_cPath + "/" + cName );
		cRef.Delete();
	}
	catch( std::exception &e )
	{
		debug( "%s/%s: %s\n", m_cPath.c_str(), cReference.AsString().c_str(), e.what() );
		return EIO;
	}

	return EOK;
}

int SyllableMailfolder::GetChildCount( void )
{
	return m_nChildren;
}

String SyllableMailfolder::GetChildName( int nChild )
{
	String cChild;

	if( nChild < 0 || nChild > m_nChildren )
		cChild = "Invalid";
	else
		cChild = m_vChildren[nChild];

	return cChild;
}

status_t SyllableMailfolder::CreateFolder( String cFolder )
{
	int nChildren = m_nChildren;

	/* Check for duplicates */
	if( _Exists( cFolder ) )
		return EEXIST;

	try
	{
		Directory *pcChild = new Directory();
		m_pcDirectory->CreateDirectory( cFolder, pcChild );

		int64 nCount = 0;
		pcChild->WriteAttr( "ref_count", 0, ATTR_TYPE_INT64, (void*)&nCount, 0, sizeof( nCount ) );
		pcChild->WriteAttr( "unread", 0, ATTR_TYPE_INT64, (void*)&nCount, 0,  sizeof( nCount ) );

		const uint64 nType = FL_TYPE_NORMAL;
		pcChild->WriteAttr( "type", 0, ATTR_TYPE_INT64, (void*)&nType, 0, sizeof( nType ) );
		const uint64 nFlags = FL_NONE;
		pcChild->WriteAttr( "flags", 0, ATTR_TYPE_INT64, (void*)&nFlags, 0, sizeof( nFlags ) );

		delete( pcChild );

		/* Add a child entry to the parent */
		nChildren += 1;
		m_pcDirectory->WriteAttr( "children", 0, ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );

		char pcAttrName[10];
		sprintf( pcAttrName, "child%i", m_nChildren );
		m_pcDirectory->WriteAttr( pcAttrName, 0, ATTR_TYPE_STRING, (void*)cFolder.c_str(), 0, cFolder.Length()  );
	}
	catch( std::exception &e )
	{
		debug( "%s: %s\n", cFolder.c_str(), e.what() );
		return EIO;
	}

	m_vChildren.push_back( cFolder );
	m_nChildren = nChildren;

	return EOK;
}

status_t SyllableMailfolder::Rename( String cName )
{
	/* Check for duplicates */
	if( _Exists( cName ) )
		return EEXIST;

	/* Get the absolute path of this folder */
	String cFolderPath, cParentPath, cNewPath;
	m_pcDirectory->GetPath( &cFolderPath );

	/* Get the absolute path of the parent folder */
	Directory cParent( cFolderPath + "/" + ".." );
	cParent.GetPath( &cParentPath );

	cNewPath = cParentPath + "/" + cName;

	/* Rename the folder & mark this Mailbox instance invalid  */
	if( rename( cFolderPath.c_str(), cNewPath.c_str() ) < 0 )
	{
		debug( "%s: %s\n", m_cPath.c_str(), strerror( errno ) );
		return errno;
	}
	m_bIsValid = false;

	/* Update parent attributes to reflect our new name */
	/* Update the parent attributes to reflect the change in its children */
	try
	{
		/* Look for children */
		ssize_t nBytes;
		int32 nChildren;

		nBytes = cParent.ReadAttr( "children", ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );
		if( nBytes > 0 && nChildren > 0 )
		{
			char pcAttrName[10], pcBuf[4096];
			for( int nChild = 0; nChild < nChildren; nChild++ )
			{
				memset( pcBuf, 0, 4096 );

				sprintf( pcAttrName, "child%i", nChild );
				nBytes = cParent.ReadAttr( pcAttrName, ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
				if( nBytes <= 0 )
					continue;

				cParent.RemoveAttr( pcAttrName );

				/* Change the folder name if this is the folder that has been renamed */
				if( cFolderPath == cParentPath + "/" + pcBuf )
				{
					memset( pcBuf, 0, 4096 );
					strcpy( pcBuf, cName.c_str() );
				}

				/* Re-write this attribute */
				cParent.WriteAttr( pcAttrName, O_TRUNC, ATTR_TYPE_STRING, (void*)pcBuf, 0, strlen( pcBuf ) );
			}
		}
	}
	catch( std::exception &e )
	{
		debug( "%s: %s\n", m_cPath.c_str(), e.what() );
	}

	return EOK;
}

/* Remove all messages, sub-folders and folders backing this mailbox instance */
status_t SyllableMailfolder::Delete( void )
{
	/* The absolute path of this folder will be important when updating the parent attributes */
	String cFolderPath;
	m_pcDirectory->GetPath( &cFolderPath );

	/* Delete all children */
	if( m_nChildren > 0 )
		for( int nChild = 0; nChild < m_nChildren; nChild++ )
		{
			SyllableMailfolder cChildMailBox( m_cMailbox, m_cMailfolder + "/" + m_vChildren[nChild] );
			if( cChildMailBox.IsValid() )
				cChildMailBox.Delete();
		}

	m_nChildren = 0;
	m_vChildren.clear();

	/* Delete all files */
	FileReference cRef;

	m_pcDirectory->Rewind();
	while( m_pcDirectory->GetNextEntry( &cRef ) > 0 )
	{
		String cEntry = cRef.GetName();
		if( cEntry == "" || cEntry == "." || cEntry == ".." )
			continue;

		cRef.Delete();
	}

	/* Mark this Mailbox instance invalid */
	m_bIsValid = false;

	/* Update the parent attributes to reflect the change in its children */
	try
	{
		Directory cParent( m_cPath + "/" + ".." );
		String cParentPath;
		cParent.GetPath( &cParentPath );

		/* Look for children */
		ssize_t nBytes;
		int32 nChildren;

		nBytes = cParent.ReadAttr( "children", ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );
		if( nBytes > 0 && nChildren > 0 )
		{
			int nNewChildren = nChildren - 1;
			cParent.WriteAttr( "children", 0, ATTR_TYPE_INT32, (void*)&nNewChildren, 0, sizeof( nNewChildren ) );

			char pcAttrName[10], pcBuf[4096];
			int nChild, nNewChild;
			for( nChild = 0, nNewChild = 0; nChild < nChildren; nChild++ )
			{
				memset( pcBuf, 0, 4096 );

				sprintf( pcAttrName, "child%i", nChild );
				nBytes = cParent.ReadAttr( pcAttrName, ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
				if( nBytes <= 0 )
					continue;

				cParent.RemoveAttr( pcAttrName );

				/* Don't re-write this entry if it is the folder that has been deleted */
				if( cFolderPath == cParentPath + "/" + pcBuf )
					continue;

				/* Re-write this attribute */
				sprintf( pcAttrName, "child%i", nNewChild++ );
				cParent.WriteAttr( pcAttrName, O_TRUNC, ATTR_TYPE_STRING, (void*)pcBuf, 0, strlen( pcBuf ) );
			}
		}
	}
	catch( std::exception &e )
	{
		debug( "%s: %s\n", m_cPath.c_str(), e.what() );
	}
	
	/* Unlink ourselves  */
	if( m_pcDirectory->Delete() < 0 )
	{
		debug( "%s: %s\n", m_cPath.c_str(), strerror( errno ) );
		return errno;
	}

	return EOK;
}

FolderProperties * SyllableMailfolder::GetProperties( void )
{
	return m_pcProperties;
}

status_t SyllableMailfolder::SetProperties( FolderProperties *pcProperties )
{
	status_t nError = EOK;

	m_pcProperties = pcProperties;

	try
	{
		/* Write properties to disk */
		uint64 nType = m_pcProperties->GetType();
		m_pcDirectory->WriteAttr( "type", 0, ATTR_TYPE_INT64, (void*)&nType, 0, sizeof( nType ) );

		uint64 nFlags = m_pcProperties->GetFlags();
		m_pcDirectory->WriteAttr( "flags", 0, ATTR_TYPE_INT64, (void*)&nFlags, 0, sizeof( nFlags ) );

		if( nFlags & FL_HAS_LIST )
		{
			String cListAddress = m_pcProperties->GetListAddress();
			m_pcDirectory->WriteAttr( "list_address", 0, ATTR_TYPE_STRING, (void*)cListAddress.c_str(), 0, cListAddress.Length()  );
		}
	}
	catch( std::exception &e )
	{
		debug( "%s: %s\n", m_cPath.c_str(), e.what() );
		nError = EIO;
	}

	return nError;
}

bool SyllableMailfolder::_Exists( const String cFolder ) const
{
	ssize_t nBytes;
	int32 nChildren;

	nBytes = m_pcDirectory->ReadAttr( "children", ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );
	if( nBytes > 0 && nChildren > 0 )
	{
		char pcAttrName[10], pcBuf[4096];
		for( int nChild = 0; nChild < nChildren; nChild++ )
		{
			memset( pcBuf, 0, 4096 );

			sprintf( pcAttrName, "child%i", nChild );
			nBytes = m_pcDirectory->ReadAttr( pcAttrName, ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
			if( nBytes <= 0 )
				continue;

			if( cFolder == pcBuf )
				return true;
		}
	}

	return false;
}

Mailbox * SyllableMailboxNode::GetMailbox( String cName )
{
	return new SyllableMailbox( cName );
}

SyllableMailbox::SyllableMailbox( String cName )
{
	m_cName = cName;

	struct stat sStat;

	Path cMailboxPath( getenv( "HOME" ) );
	cMailboxPath.Append( "/Mail/" );
	cMailboxPath.Append( m_cName );
	cMailboxPath.Append( "/" );

	/* If the mailbox we're being asked for doesn't exist, create it */
	if( stat( cMailboxPath.GetPath().c_str(), &sStat ) < 0 )
	{
		if( Create() != EOK )
			debug( "Unable to find %s and Create() failed!\n", cMailboxPath.GetPath().c_str() );
			/* XXXKV: Should probably throw an exception here */
	}
	else
	{
		/* Check the version */
		Directory cMailboxRoot( cMailboxPath.GetPath(), O_RDWR );
		int32 nVersion = 0;

		cMailboxRoot.ReadAttr( "version",  ATTR_TYPE_INT32, (void*)&nVersion, 0, sizeof( nVersion ) );

		if( nVersion < SYLLABLE_MAILBOX_VERSION )
			Upgrade( cMailboxPath, nVersion );
	}
}

/* Create a default mailbox from scratch */
status_t SyllableMailbox::Create( void )
{
	String cPath;
	struct stat sStat;

	Path cMailboxPath( getenv( "HOME" ) );
	cMailboxPath.Append( "/Mail" );

	cPath = cMailboxPath.GetPath();

	/* Ensure the users "~/Mail" directory exists */
	if( stat( cPath.c_str(), &sStat ) < 0 )
		if( mkdir( cPath.c_str(), S_IRWXU ) < 0 )
			return EIO;

	/* Create the root Mailbox folder within that */
	cMailboxPath.Append( "/" );
	cMailboxPath.Append( m_cName );

	cPath = cMailboxPath.GetPath();

	if( stat( cPath.c_str(), &sStat ) < 0 )
		if( mkdir( cMailboxPath.GetPath().c_str(), S_IRWXU ) < 0 )
			return EIO;

	Directory *pcMailboxRoot;
	try
	{
		pcMailboxRoot = new Directory( cPath, O_WRONLY );
	}
	catch( std::exception &e )
	{
		debug( "%s\n", e.what() );
		return EIO;
	}

	const int32 nChildren = 5;
	pcMailboxRoot->WriteAttr( "children", 0, ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );

	/* Create default folder inside Mailbox root folder */
	for( int n = 0; n < 5; n++ )
	{
		const int64 nCount = 0;
		String cFolder = asDefaultFolders[ n ].cFolder;
		uint64 nType = asDefaultFolders[ n ].nType;
		const uint64 nFlags = FL_NONE;

		Directory *pcChild = new Directory();
		pcMailboxRoot->CreateDirectory( cFolder, pcChild );
		if( NULL == pcChild )
			continue;
		pcChild->WriteAttr( "ref_count", 0, ATTR_TYPE_INT64, (void*)&nCount, 0, sizeof( nCount ) );
		pcChild->WriteAttr( "unread", 0, ATTR_TYPE_INT64, (void*)&nCount, 0, sizeof( nCount ) );
		pcChild->WriteAttr( "type", 0, ATTR_TYPE_INT64, (void*)&nType, 0, sizeof( nType ) );
		pcChild->WriteAttr( "flags", 0, ATTR_TYPE_INT64, (void*)&nFlags, 0, sizeof( nFlags ) );
		delete( pcChild );

		char pcAttrName[10];
		sprintf( pcAttrName, "child%i", n );
		pcMailboxRoot->WriteAttr( pcAttrName, 0, ATTR_TYPE_STRING, (void*)cFolder.c_str(), 0, cFolder.Length()  );
	}

	const int nVersion = SYLLABLE_MAILBOX_VERSION;
	pcMailboxRoot->WriteAttr( "version", 0, ATTR_TYPE_INT32, (void*)&nVersion, 0, sizeof( nVersion ) );

	delete( pcMailboxRoot );

	return EOK;
}

status_t SyllableMailbox::Upgrade( Path cPath, int nCurrentVersion )
{
	status_t nError = EOK;

	debug( "Mailbox at %s requires an upgrade from version %d to %d\n", cPath.GetPath().c_str(), nCurrentVersion, SYLLABLE_MAILBOX_VERSION );

	Directory *pcMailboxRoot = new Directory( cPath, O_RDWR );

	switch( nCurrentVersion )
	{
		case 0:
		default:
		{
			/* Pre-alpha7 mailboxes without a version number */
			int32 nChildren = 0;
			pcMailboxRoot->ReadAttr( "children", ATTR_TYPE_INT32, (void*)&nChildren, 0, sizeof( nChildren ) );

			char pcAttrName[10], pcBuf[4096];
			int nChild, nBytes;
			Path cMailfolderPath;
			for( nChild = 0; nChild < nChildren; nChild++ )
			{
				memset( pcBuf, 0, 4096 );

				sprintf( pcAttrName, "child%i", nChild );
				nBytes = pcMailboxRoot->ReadAttr( pcAttrName, ATTR_TYPE_STRING, (void*)pcBuf, 0, 4096 );
				if( nBytes <= 0 )
					continue;

				/* Get the child folder */
				cMailfolderPath = cPath;
				cMailfolderPath.Append( "/" );
				cMailfolderPath.Append( pcBuf );

				debug( "Updating child folder \"%s\"\n", cMailfolderPath.GetPath().c_str() );

				/* Attempt to identify the folder type */
				uint64 nType = FL_TYPE_NORMAL;
				for( int n = 0; n < 5; n++ )
					if( asDefaultFolders[n].cFolder == pcBuf )
					{
						debug( "Found %s\n", pcBuf );

						nType = asDefaultFolders[n].nType;
						break;
					}

				/* Type */
				Directory cMailfolder( cMailfolderPath, O_WRONLY );
				cMailfolder.WriteAttr( "type", 0, ATTR_TYPE_INT64, (void*)&nType, 0, sizeof( nType ) );

				/* Default flags */
				const uint64 nFlags = FL_NONE;
				cMailfolder.WriteAttr( "flags", 0, ATTR_TYPE_INT64, (void*)&nFlags, 0, sizeof( nFlags ) );
			}

			break;
		}
	}

	/* The mailbox is upto date */
	const int nVersion = SYLLABLE_MAILBOX_VERSION;
	pcMailboxRoot->WriteAttr( "version", 0, ATTR_TYPE_INT32, (void*)&nVersion, 0, sizeof( nVersion ) );

	delete( pcMailboxRoot );

	return nError;
}

Mailfolder * SyllableMailbox::OpenFolder( String cPath )
{
	return new SyllableMailfolder( m_cName, cPath );
}

