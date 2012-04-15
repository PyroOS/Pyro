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

#ifndef WHISPER_MAIL_H_
#define WHISPER_MAIL_H_

#include <time.h>

#include <pyro/types.h>
#include <util/string.h>
#include <util/variant.h>

#include <rfctime.h>

#include <vector>
#include <list>

class MimeType
{
	public:
		os::String cSuperType;
		os::String cSubType;

		MimeType & operator=( const MimeType &cType )
		{
			cSuperType = cType.cSuperType;
			cSubType = cType.cSubType;
			return( *this );
		};
};

enum disposition_t
{
	BODY,
	BODY_ALTERNATIVE,
	BODY_SIGNED,
	BODY_ALTERNATIVE_HIDDEN,
	ATTACHMENT,
	INLINE,
	MULTIPART,
	ENCRYPTION_KEY
};

class Mailmessage;

/* A note about the difference between the "size" and "len" used here.  The size
   is the total size of m_pData.  It includes the multi-part headers.  The len is
   the size of the data starting after the headers.  It is the size of the raw data
   contained within the part. */

class Multipart
{
	public:
		Multipart()
		{
			m_pData = NULL;
			m_nSize = 0;
			m_nLen = 0;
		};

		~Multipart()
		{
			if( m_pData )
				free( m_pData );

			m_vParts.clear();
		};

		const char * GetData( void )
		{
			return m_pData;			
		};

		ssize_t GetDataSize( void )
		{
			return m_nSize;
		};

		ssize_t GetDataLen( void )
		{
			return m_nLen;
		};

		void AddChild( Multipart *pcPart )
		{
			m_vParts.push_back( pcPart );
		};

		int GetChildCount( void )
		{
			return m_vParts.size();
		};

		Multipart & GetChild( uint nIndex );

		void SetId( uint64 nId )
		{
			m_nId = nId;
		};

		uint64 GetId( void )
		{
			return m_nId;
		};

		Multipart & operator=( const Multipart &cPart )
		{
			cMimeType = cPart.cMimeType;
			cEncoding = cPart.cEncoding;
			cDisposition = cPart.cDisposition;
			cCharset = cPart.cCharset;
			cFilename = cPart.cFilename;
			eDisposition = cPart.eDisposition;
			m_nSize = cPart.m_nSize;
			m_nLen = cPart.m_nLen;

			pStart = cPart.pStart;
			pBegin = cPart.pBegin;
			pEnd = cPart.pEnd;

			m_pData = (char*)calloc( 1, m_nLen + 1 );
			if( NULL == m_pData )
					m_nLen = 0;
			else
					m_pData = strncpy( m_pData, pBegin, m_nLen );

			for( int nArg = 0; nArg <= 3; nArg++ )
			{
				m_cContentTypeArgs[nArg] = cPart.m_cContentTypeArgs[nArg];
				m_cDispositionArgs[nArg] = cPart.m_cDispositionArgs[nArg];
			}

			m_vParts = cPart.m_vParts;
			m_nId = cPart.m_nId;

			return( *this );
		};
		Multipart & operator=( const Multipart *pcPart )
		{
			cMimeType = pcPart->cMimeType;
			cEncoding = pcPart->cEncoding;
			cDisposition = pcPart->cDisposition;
			cCharset = pcPart->cCharset;
			cFilename = pcPart->cFilename;
			eDisposition = pcPart->eDisposition;
			m_nSize = pcPart->m_nSize;
			m_nLen = pcPart->m_nLen;

			pStart = pcPart->pStart;
			pBegin = pcPart->pBegin;
			pEnd = pcPart->pEnd;

			m_pData = (char*)calloc( 1, m_nLen + 1 );
			if( NULL == m_pData )
					m_nLen = 0;
			else
					m_pData = strncpy( m_pData, pBegin, m_nLen );

			for( int nArg = 0; nArg <= 3; nArg++ )
			{
				m_cContentTypeArgs[nArg] = pcPart->m_cContentTypeArgs[nArg];
				m_cDispositionArgs[nArg] = pcPart->m_cDispositionArgs[nArg];
			}

			m_vParts = pcPart->m_vParts;
			m_nId = pcPart->m_nId;

			return( *this );
		};

		MimeType cMimeType;
		os::String cEncoding;
		os::String cDisposition;
		os::String cCharset;
		os::String cFilename;

		disposition_t eDisposition;

		friend class Mailmessage;

	private:
		const char *pStart, *pBegin, *pEnd;

		char *m_pData;
		ssize_t m_nSize, m_nLen;

		os::String m_cContentTypeArgs[4];
		os::String m_cDispositionArgs[4];

		std::vector <Multipart *> m_vParts;

		uint64 m_nId;
};

class Mailsummery
{
	public:
		Mailsummery(){};
		~Mailsummery(){};

		os::Variant cReference;

		os::String cSubject;
		os::String cFrom;

		time_t nDate;

		int32 nStatus;
		int32 nFlag;
		int32 nAttachments;
};

enum status
{
	STATUS_NEW,
	STATUS_DRAFT,
	STATUS_UNREAD,
	STATUS_READ,
	STATUS_REPLIED,
	STATUS_FORWARDED
};

enum flag
{
	FLAG_NONE,
	FLAG_LOW,
	FLAG_MEDIUM,
	FLAG_HIGH,
	FLAG_URGENT
};

class Mailmessage
{
	public:
		Mailmessage();
		Mailmessage( const char *pcBuffer, ssize_t nSize );
		~Mailmessage();

		void SetReference( os::Variant cReference )
		{
			m_cReference = cReference;
		};
		os::Variant GetReference( void )
		{
			return m_cReference;
		}

		status_t SetData( const char *pcBuffer, ssize_t nSize );
		char * GetData( void );
		ssize_t GetDataSize( void );

		status_t ParseHeaders( void );
		status_t Parse( void );

		status_t GetHeader( os::String cName, os::String &cHeader );

		void SetTo( os::String cTo )
		{
			m_cTo = cTo;
			m_bHaveTo = true;
		};
		os::String GetTo( void )
		{
			return m_cTo;
		};
		void SetCc( os::String cCc )
		{
			m_cCc = cCc;
			m_bHaveCc = true;
		};
		os::String GetCc( void )
		{
			return m_cCc;
		};
		void SetBcc( os::String cBcc )
		{
			m_cBcc = cBcc;
			m_bHaveBcc = true;
		};
		os::String GetBcc( void )
		{
			return m_cBcc;
		};
		void SetReplyTo( os::String cReplyTo )
		{
			m_cReplyTo = cReplyTo;
			m_bHaveReplyTo = true;
		};
		os::String GetReplyTo( void )
		{
			return m_cReplyTo;
		};
		void SetSubject( os::String cSubject )
		{
			m_cSubject = cSubject;
			m_bHaveSubject = true;
		};
		os::String GetSubject( void )
		{
			return m_cSubject;
		};
		void SetInReplyTo( os::String cInReplyTo )
		{
			m_cInReplyTo = cInReplyTo;
			m_bHaveInReplyTo = true;
		};
		os::String GetInReplyTo( void )
		{
			return m_cInReplyTo;
		};
		void SetId( os::String cId )
		{
			m_cId = cId;
			m_bHaveId = true;
		};
		os::String GetId( void )
		{
			return m_cId;
		};
		void SetFrom( os::String cFrom )
		{
			m_cFrom = cFrom;
			m_bHaveFrom = true;
		};
		os::String GetFrom( void )
		{
			return m_cFrom;
		};
		void SetSender( os::String cSender )
		{
			m_cSender = cSender;
			m_bHaveSender = true;
		};
		os::String GetSender( void )
		{
			return m_cSender;
		};
		void SetListPost( os::String cListPost )
		{
			m_cListPost = cListPost;
			m_bHaveListPost = true;
		};
		os::String GetListPost( void )
		{
			return m_cListPost;
		};
		void SetDate( os::String cDate )
		{
			m_cDate = cDate;
			m_bHaveDate = true;

			m_nTime = convert_date( m_cDate );
		};
		os::String GetDate( void )
		{
			return m_cDate;
		};
		void SetContent( os::String cContent )
		{
			m_cContent = cContent;
			m_bHaveContent = true;
		};
		os::String GetContent( void )
		{
			return m_cContent;
		};
		void SetEncoding( os::String cEncoding )
		{
			m_cEncoding = cEncoding.Lower();
			m_bHaveEncoding = true;
		};
		os::String GetEncoding( void )
		{
			return m_cEncoding;
		};
		void SetStatus( int32 nStatus )
		{
			m_nStatus = nStatus;
		};
		int32 GetStatus( void )
		{
			return m_nStatus;
		};
		void SetFlag( int32 nFlag )
		{
			m_nFlag = nFlag;
		};
		int32 GetFlag( void )
		{
			return m_nFlag;
		};
		int GetPartCount( void )
		{
			return m_vParts.size();
		};
		int GetAttachmentCount( void )
		{
			return m_nAttachmentCount;
		};
		time_t GetTime( void )
		{
			return m_nTime;
		};

		void SetAddress( os::String cAddress )
		{
			m_cAddress = cAddress;
		};
		os::String GetAddress( void )
		{
			return m_cAddress;
		};

		uint64 GetNextId( void )
		{
			return m_nId++;
		};

		Multipart & GetPartInfo( uint nIndex );
		Multipart & GetPart( uint nIndex );

		status_t GetPartById( Multipart &cPart, uint64 nId, bool bChild = false );

		bool Validate( void );
		status_t GetRecipiants( std::list<os::String> &vRecipiants, bool bValid = true );
		status_t Attach( os::String cFilename );
		status_t Compose( void );

	private:
		status_t FindHeader( os::String cFind, os::String &cHeader, const char *pBuffer, ssize_t nSize );
		status_t ParseHeaderArgs( os::String cHeader, os::String cArgs[] );
		status_t GetHeaderArg( os::String cArgs[], int nCount, os::String cName, os::String &cParam );
		status_t FindContentType( os::String cHeader, os::String cArgs[], MimeType &cMimeType );
		status_t FindBody( void );
		status_t FindMultiParts( const char *pcBuffer, std::vector <Multipart *> &vParts, os::String cBoundary );
		int IdentifyDisposition( std::vector <Multipart *> &vParts );
		void CleanHeader( os::String &cHeader );

		os::Variant m_cReference;

		char *m_pcBuffer;
		ssize_t m_nSize;

		os::String m_cTo;
		bool m_bHaveTo;
		os::String m_cCc;
		bool m_bHaveCc;
		os::String m_cBcc;
		bool m_bHaveBcc;
		os::String m_cReplyTo;
		bool m_bHaveReplyTo;
		os::String m_cSubject;
		bool m_bHaveSubject;
		os::String m_cInReplyTo;
		bool m_bHaveInReplyTo;
		os::String m_cId;
		bool m_bHaveId;
		os::String m_cFrom;
		bool m_bHaveFrom;
		os::String m_cSender;
		bool m_bHaveSender;
		os::String m_cDate;
		bool m_bHaveDate;
		os::String m_cListPost;
		bool m_bHaveListPost;
		os::String m_cContent;
		bool m_bHaveContent;
		os::String m_cEncoding;
		bool m_bHaveEncoding;
		os::String m_cDisposition;
		bool m_bHaveDisposition;

		os::String m_cAddress;

		int32 m_nStatus;
		int32 m_nFlag;

		MimeType m_cMimeType;
		os::String m_cContentTypeArgs[4];

		std::vector <Multipart *> m_vParts;
		int m_nBodyPartIndex;
		int m_nAttachmentCount;

		time_t m_nTime;

		std::list <os::String> m_vRecipiants;
		std::list <os::String> m_vInvalid;

		std::list <os::String> m_vAttachments;

		uint64 m_nId;
};

#endif

