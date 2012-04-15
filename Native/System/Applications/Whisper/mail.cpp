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

#include <string.h>
#include <unistd.h>
#include <posix/param.h>

#include <storage/registrar.h>
#include <storage/file.h>

/* XXXKV: Don't want this but it's forced on us by GetTypeAndIcon() */
#include <gui/image.h>

#include <mail.h>
#include <base64_codec.h>
#include <version.h>
#include <strfuncs.h>

#include <debug.h>

Multipart & Multipart::GetChild( uint nIndex )
{
	Multipart *pcPart = new Multipart();

	if( nIndex < 0 || nIndex > m_vParts.size() || m_vParts.size() == 0 )
		return( *pcPart );

	/* XXXKV: I really need to fix the Multipart copiers and bits of code so we don't
	   need to do this sort of thing. */
	pcPart = m_vParts[nIndex];
	pcPart->m_pData = (char*)calloc( 1, pcPart->m_nLen + 1 );
	if( NULL == pcPart->m_pData )
	{
		pcPart->m_nLen = 0;
		return( *pcPart );
	}

	pcPart->m_pData = strncpy( pcPart->m_pData, pcPart->pBegin, pcPart->m_nLen );

	return( *pcPart );
}

Mailmessage::Mailmessage()
{
	m_pcBuffer = NULL;
	m_nSize = 0;

	m_bHaveTo =
	m_bHaveCc =
	m_bHaveBcc =
	m_bHaveReplyTo =
	m_bHaveSubject =
	m_bHaveReplyTo =
	m_bHaveId =
	m_bHaveFrom =
	m_bHaveSender =
	m_bHaveListPost =
	m_bHaveDate =
	m_bHaveContent =
	m_bHaveEncoding =
	m_bHaveDisposition = false;
	m_nStatus = STATUS_NEW;
	m_nFlag = FLAG_NONE;

	m_nAttachmentCount = 0;
	m_nTime = 0;
	m_nBodyPartIndex = -1;
	m_nId = 0;
}

Mailmessage::Mailmessage( const char *pcBuffer, ssize_t nSize )
{
	m_pcBuffer = NULL;
	m_nSize = 0;

	m_bHaveTo =
	m_bHaveCc =
	m_bHaveBcc =
	m_bHaveReplyTo =
	m_bHaveSubject =
	m_bHaveReplyTo =
	m_bHaveId =
	m_bHaveFrom =
	m_bHaveSender =
	m_bHaveListPost =
	m_bHaveDate =
	m_bHaveContent =
	m_bHaveEncoding = 
	m_bHaveDisposition = false;
	m_nStatus = STATUS_NEW;
	m_nFlag = FLAG_NONE;

	m_nAttachmentCount = 0;
	m_nTime = 0;
	m_nBodyPartIndex = -1;
	m_nId = 0;

	SetData( pcBuffer, nSize );
}

Mailmessage::~Mailmessage()
{
	if( m_pcBuffer )
		free( m_pcBuffer );

	m_vParts.clear();
}

status_t Mailmessage::SetData( const char *pcBuffer, ssize_t nSize )
{
	if( m_pcBuffer )
		free( m_pcBuffer );

	m_pcBuffer = (char*)calloc( 1, nSize + 1 );
	if( NULL == m_pcBuffer )
		return ENOMEM;
	m_pcBuffer = (char*)memcpy( m_pcBuffer, pcBuffer, nSize );
	m_nSize = nSize;

	return EOK;
}

char * Mailmessage::GetData( void )
{
	return m_pcBuffer;
}

ssize_t Mailmessage::GetDataSize( void )
{
	return m_nSize;
}

/* Parse a raw RFC2822 email which is in m_pcBuffer. Do the following:
 * o Extract standard headers we're interested in
 * o Find content type and content encoding type
 *
 * This method does not parse the body text.
 */
status_t Mailmessage::ParseHeaders( void )
{
	status_t nError;

	if( NULL == m_pcBuffer )
		return EINVAL;

	/* Extract the headers we don't already have */
	if( false == m_bHaveTo )
	{
		nError = FindHeader( "To", m_cTo, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cTo );
			m_bHaveTo = true;
		}
	}

	if( false == m_bHaveCc )
	{
		nError = FindHeader( "Cc", m_cCc, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cCc );
			m_bHaveCc = true;
		}
	}

	if( false == m_bHaveBcc )
	{
		nError = FindHeader( "Bcc", m_cBcc, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cBcc );
			m_bHaveBcc = true;
		}
	}

	if( false == m_bHaveReplyTo )
	{
		nError = FindHeader( "Reply-To", m_cReplyTo, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cReplyTo );
			m_bHaveReplyTo = true;
		}
	}

	if( false == m_bHaveSubject )
	{
		nError = FindHeader( "Subject", m_cSubject, m_pcBuffer, m_nSize );
		if( EOK == nError )
			m_bHaveSubject = true;
	}

	if( false == m_bHaveInReplyTo )
	{
		nError = FindHeader( "In-Reply-To", m_cInReplyTo, m_pcBuffer, m_nSize );
		if( EOK == nError )
			m_bHaveInReplyTo = true;
	}

	if( false == m_bHaveId )
	{
		nError = FindHeader( "Message-ID", m_cId, m_pcBuffer, m_nSize );
		/* Some mail clients are confused about the capitalisation */
		if( ESRCH == nError )
			nError = FindHeader( "Message-Id", m_cId, m_pcBuffer, m_nSize );

		if( EOK == nError )
			m_bHaveId = true;
	}

	if( false == m_bHaveFrom )
	{
		nError = FindHeader( "From", m_cFrom, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cFrom );
			m_bHaveFrom = true;
		}
	}

	if( false == m_bHaveSender )
	{
		nError = FindHeader( "Sender", m_cSender, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cSender );
			m_bHaveSender = true;
		}
	}

	if( false == m_bHaveListPost )
	{
		nError = FindHeader( "List-Post", m_cListPost, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			CleanHeader( m_cListPost );
			m_bHaveListPost = true;
		}
	}

	if( false == m_bHaveDate )
	{
		nError = FindHeader( "Date", m_cDate, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			m_bHaveDate = true;
			m_nTime = convert_date( m_cDate );
		}
	}

	if( false == m_bHaveContent )
	{
		nError = FindHeader( "Content-Type", m_cContent, m_pcBuffer, m_nSize );
		if( EOK == nError )
			m_bHaveContent = true;
	}

	if( false == m_bHaveEncoding )
	{
		os::String cEncoding;
		nError = FindHeader( "Content-Transfer-Encoding", cEncoding, m_pcBuffer, m_nSize );
		if( EOK == nError )
		{
			m_cEncoding = cEncoding.Lower();
			m_bHaveEncoding = true;
		}
		else
			m_cEncoding = "7bit";
	}

	if( false == m_bHaveDisposition )
	{
		nError = FindHeader( "Content-Disposition", m_cDisposition, m_pcBuffer, m_nSize );
		if( EOK == nError )
			m_bHaveDisposition = true;
	}

	/* Get content type and args */
	if( m_bHaveContent )
	{
		if( FindContentType( m_cContent, m_cContentTypeArgs, m_cMimeType ) != EOK )
		{
			m_cContent = "text/plain";
			m_cMimeType.cSuperType = "text";
			m_cMimeType.cSubType = "plain";
		}
	}
	else
	{
		m_cContent = "text/plain";
		m_cMimeType.cSuperType = "text";
		m_cMimeType.cSubType = "plain";
	}

	return EOK;
}

/* Parse a raw RFC2822 email which is in m_pcBuffer. Do the following:
 * o Call ParseHeaders() to locate the required header information
 * o If MIME multipart, locate all parts
 * o If a text message, locate the body text
 *
 * This method does not decode the body text or any miltipart attachments.
 * The part data can be retrieved with GetPart() and then passed to an
 * appropriate decoder by the caller.
 */
status_t Mailmessage::Parse( void )
{
	status_t nError;

	nError = ParseHeaders();
	if( nError != EOK )
		return nError;

	/* XXXKV: Here's how multipart messages will be handled.  Have
	   GetPartCount(), GetPart() & GetPartInfo() methods.  If the
	   message is multipart,  call a method to find all the parts. 
	   If the message is text, find the start of the body text and
	   create a single part for the body text; the MIME type and
	   encoding for the part will be that of the message itself.
	   GetBodyPart() will always return the appropriate part
	   containing the body text.
	*/

	if( m_cMimeType.cSuperType == "multipart" )
	{
		os::String cBoundary;

		if( GetHeaderArg( m_cContentTypeArgs, 3, os::String( "boundary=" ), cBoundary ) != EOK )
			return EINVAL;

		if( FindMultiParts( m_pcBuffer, m_vParts, cBoundary ) != EOK )
			return EINVAL;

		m_nBodyPartIndex = IdentifyDisposition( m_vParts );
	}
	else if( FindBody() != EOK )
			return EINVAL;

	return EOK;
}

status_t Mailmessage::GetHeader( os::String cName, os::String &cHeader )
{
	return FindHeader( cName, cHeader, m_pcBuffer, m_nSize );
}

Multipart & Mailmessage::GetPartInfo( uint nIndex )
{
	Multipart *pcPart = new Multipart();

	if( nIndex < 0 || nIndex > m_vParts.size() || m_vParts.size() == 0 )
		return( *pcPart );

	pcPart = m_vParts[nIndex];

	return( *pcPart );
}

Multipart & Mailmessage::GetPart( uint nIndex )
{
	Multipart *pcPart = new Multipart();

	if( nIndex < 0 || nIndex > m_vParts.size() || m_vParts.size() == 0 )
		return( *pcPart );

	/* XXXKV: I really need to fix the Multipart copiers and bits of code so we don't
	   need to do this sort of thing. */
	pcPart = m_vParts[nIndex];
	pcPart->m_pData = (char*)calloc( 1, pcPart->m_nLen + 1 );
	if( NULL == pcPart->m_pData )
	{
		pcPart->m_nLen = 0;
		return( *pcPart );
	}

	pcPart->m_pData = strncpy( pcPart->m_pData, pcPart->pBegin, pcPart->m_nLen );

	return( *pcPart );
}

status_t Mailmessage::GetPartById( Multipart &cPart, uint64 nId, bool bChild )
{
	status_t nError = EINVAL;
	uint nPartCount;

	if( bChild )
		nPartCount = cPart.GetChildCount();
	else
		nPartCount = m_vParts.size();

	for( uint nIndex = 0; nIndex < nPartCount; nIndex++ )
	{
		if( bChild )
			cPart = cPart.GetChild( nIndex );
		else
			cPart = GetPart( nIndex );

		if( cPart.GetId() == nId )
		{
			debug( "found part with id %Lu\n", nId );
			nError = EOK;
			break;
		}

		if( cPart.eDisposition == MULTIPART )
		{
			nError = GetPartById( cPart, nId, true );
			if( nError == EOK )
				break;
		}
	}

	return nError;
}

/* Parse any addresses in the To, Cc & Bcc fields.  Check them for validity and create a single
   list of all recipiant addresses.  Returns true if all recipiants are valid or false if one or
   more addresses are invalid */
bool Mailmessage::Validate( void )
{
	m_vRecipiants.clear();
	m_vInvalid.clear();

	for( int nField = 0; nField < 3; nField++ )
	{
		const char *pzAddresses = NULL;

		switch( nField )
		{
			case 0:
			{
				pzAddresses = m_cTo.c_str();
				break;
			}

			case 1:
			{
				pzAddresses = m_cCc.c_str();
				break;
			}

			case 2:
			{
				pzAddresses = m_cBcc.c_str();
				break;
			}
		}

		/* Break the string into multiple addresses, if there are any */
		if( strcmp( pzAddresses, "" ) == 0 )
			continue;

		char *pComma = NULL;
		const char *pStart = pzAddresses;
		bool bParse = true;
		while( bParse )
		{
			os::String cRecipiant;

			pComma = strstr( pStart, "," );

			if( NULL == pComma )
			{
				cRecipiant = pStart;
				bParse = false;
			}
			else
			{
				char *zRecipiant = (char*)calloc( 1, ( pComma - pStart ) + 1 );
				if( NULL == zRecipiant )
					return ENOMEM;

				zRecipiant = strncpy( zRecipiant, pStart, pComma - pStart );
				cRecipiant = zRecipiant;
				free( zRecipiant );

				pStart = pComma + 1;
			}

			/* Strip leading & trailing whitespace and check that we still have valid data */
			cRecipiant.Strip();
			if( cRecipiant.Length() == 0 )
				continue;

			m_vRecipiants.push_back( cRecipiant );
		}
	}

	/* Iterate the recipiants list & validate each entry.  Check for the following forms:

	   o user@example.com
	   o "user" user@example.com
	   o "user" <user@example.com>
	   
	   For all of these forms, take only the user@example.com part.  Validate that part;
	   check for one (& only one) @ symbol. */

	/* Note that the iterator gets incremented at the bottom of the loop instead of within the for()
	   itself.  This is because .erase() will increment i.  erase() followed by continue; increments
	   i twice, either skipping an entry or looping back to i.begin()  This is avoided by incrementing
	   at the bottom of the loop so that continue does not cause i to increment */

	std::list<os::String>::iterator i;
	for( i = m_vRecipiants.begin(); i != m_vRecipiants.end(); )
	{
		const char *pSymbol, *pStart, *pEnd;
		const char *pzRecipiant = (*i).c_str();
		os::String cAddress;

		/* Check for a quoted username */
		pSymbol = strstr( pzRecipiant, "\"" );
		if( NULL != pSymbol )
		{
			pEnd = strstr( pSymbol + 1, "\"" );
			if( NULL != pEnd )
			{
				/* Skip the quoted username */
				pStart = pEnd + 1;
			}
			else
			{
				/* Invalid; this address has one quote (") character */
				m_vInvalid.push_back( (*i) );
				i = m_vRecipiants.erase( i );

				continue;
			}
		}
		else
			pStart = pzRecipiant;

		/* Check for angeled brackets around address */
		pSymbol = strstr( pStart, "<" );
		if( NULL != pSymbol )
		{
			pStart = pSymbol + 1;
			pEnd = strstr( pStart, ">" );
			if( NULL != pEnd )
			{
				/* Closing bracket found, extract the address */
				ssize_t nLen = pEnd - pStart;
				char *zAddress = (char*)calloc( 1, nLen + 1);
				if( NULL == zAddress )
					continue;
				zAddress = (char*)strncpy( zAddress, pStart, nLen );
				cAddress = zAddress;
				free( zAddress );
				pStart = cAddress.c_str();
			}
			else
			{
				/* Invalid; only the opening bracket (<) was found */#
				m_vInvalid.push_back( (*i) );
				i = m_vRecipiants.erase( i );

				continue;
			}
		}
		else
		{
			/* This address does not have any enclosing brackets.  Ensure that
			   there is no "spare" closing bracket */
			if( strstr( pStart, ">" ) != NULL )
			{
				/* Invalid; closing bracket (>) but no opening bracket (<) was found */
				m_vInvalid.push_back( (*i) );
				i = m_vRecipiants.erase( i );

				continue;
			}
		}

		/* Ensure that there is one (and only one) @ symbol */
		pSymbol =  strstr( pStart, "@" );
		if( NULL == pSymbol )
		{
			/* Invalid; no @ symbol was found */
			m_vInvalid.push_back( (*i) );
			i = m_vRecipiants.erase( i );

			continue;
		}
		else if( strstr( pSymbol + 1, "@" ) != NULL )
		{
			/* Invalid; more than one @ symbol was found */
			m_vInvalid.push_back( (*i) );
			i = m_vRecipiants.erase( i );

			continue;
		}
		else
		{
			/* This address is valid; strip any remaining whitespace */
			cAddress = pStart;
			cAddress.Strip();
		}

		/* Put the validated address back into the list */
		(*i) = cAddress;
		i++;
	}

	if( m_vInvalid.size() > 0 )
		return false;
	else
		return true;
}

status_t Mailmessage::GetRecipiants( std::list<os::String> &vRecipiants, bool bValid )
{
	if( bValid )
		vRecipiants = m_vRecipiants;
	else
		vRecipiants = m_vInvalid;

	return EOK;
}

/* Add the file to the list of attachments */
status_t Mailmessage::Attach( os::String cFilename )
{
	m_vAttachments.push_back( cFilename );
	return EOK;
}

/* Generate an RFC2822 email from all of the information already in this message and store
   the raw mail in the buffer */
status_t Mailmessage::Compose( void )
{
	/* Generate a unique message id and set the current date & time */
	time_t nTime = time( NULL );
	tm *psTime = gmtime( &nTime );

	char zTime[16] = {0};
	strftime( zTime, 16, "%Y%m%d%H%S", psTime );

	char zHost[MAXHOSTNAMELEN] = {0};
	gethostname( zHost, MAXHOSTNAMELEN );

	char zRnd[32] = {0};
	srand( nTime );
	snprintf( zRnd, 32, "%i", rand() );

	m_cId = os::String( "<" ) +
			os::String( zTime ) +
			os::String( "." ) +
			os::String( zRnd ) +
			os::String( "@" ) +
			os::String( zHost ) +
			os::String( ">" );
	m_bHaveDate = true;

	char zDate[64] = {0};
	strftime( zDate, 64, "%a, %d %b %Y %H:%M:%S %z", psTime );

	m_cDate = zDate;
	m_bHaveDate = true;

	/* Create headers for all of the fields that are populated */
	os::String cBuffer = "";

	if( m_cTo != "" )
		cBuffer += os::String( "To: " ) + m_cTo + os::String( "\r\n" );

	if( m_cCc != "" )
		cBuffer += os::String( "Cc: " ) + m_cCc + os::String( "\r\n" );

	if( m_cBcc != "" )
		cBuffer += os::String( "Bcc: " ) + m_cBcc + os::String( "\r\n" );

	if( m_cReplyTo != "" )
		cBuffer += os::String( "Reply-To: " ) + m_cReplyTo + os::String( "\r\n" );

	if( m_cSubject != "" )
		cBuffer += os::String( "Subject: " ) + m_cSubject + os::String( "\r\n" );

	if( m_cInReplyTo != "" )
		cBuffer += os::String( "In-Reply-To: " ) + m_cInReplyTo + os::String( "\r\n" );

	if( m_cId != "" )
		cBuffer += os::String( "Message-Id: " ) + m_cId + os::String( "\r\n" );

	if( m_cFrom != "" )
		cBuffer += os::String( "From: " ) + m_cFrom + os::String( "\r\n" );

	if( m_cSender != "" )
		cBuffer += os::String( "Sender: " ) + m_cSender + os::String( "\r\n" );

	if( m_cDate != "" )
		cBuffer += os::String( "Date: " ) + m_cDate + os::String( "\r\n" );

	cBuffer += os::String( "User-Agent: " ) + g_cTitle + os::String( "\r\n" );

	/* Decide if this is a multipart message and set Content-Type etc. as appropriate */
	cBuffer += os::String( "MIME-Version: 1.0\r\n" );

	os::String cPartBoundary;
	if( m_vAttachments.size() == 0 )
	{
		/* No attachments, body only.  The Content-Transfer-Encoding header goes at the top of
		   the message */
		SetContent( "text/plain" );
		cBuffer += os::String( "Content-Type: " ) +
					m_cContent +
					os::String( ";\r\n\tcharset=utf-8\r\n" );

		SetEncoding( "8bit" );
		cBuffer += os::String( "Content-Transfer-Encoding: " ) + m_cEncoding + os::String( "\r\n\r\n" );

	}
	else
	{
		/* Type is multipart/mixed.  Generate a multipart boundary. */
		cPartBoundary = os::String( "----=_NextPart_" ) +
						os::String( zTime ) +
						os::String( "." ) +
						os::String( zRnd );

		os::String cContent = os::String( "multipart/mixed;\r\n\tboundary=\"" ) +
								cPartBoundary +
								os::String( "\"" );
		SetContent( cContent );

		cPartBoundary = os::String( "--" ) + cPartBoundary + os::String( "\r\n" );

		cBuffer += os::String( "Content-Type: " ) + m_cContent + os::String( "\r\n" );
		cBuffer += os::String( "\r\nThis is a multi-part message in MIME format.\r\n\r\n" );
		cBuffer += cPartBoundary;

		cBuffer += os::String( "Content-Type: text/plain;\r\n\tcharset=\"utf8\"\r\n" );
		cBuffer += os::String( "Content-Transfer-Encoding: 8bit\r\n\r\n" );
	}

	/* Insert the body.  The first step is to convert all single LF's at the end of each
	   line into CRLF pairs */
	char *pStart, *pEnd;
	int nLf = 0;

	pStart = m_pcBuffer;
	while( ( pEnd = strchr( pStart, '\n' ) ) != NULL )
	{
		nLf++;
		pStart = pEnd + 1;
	}

	char *pBody = (char*)calloc( 1, m_nSize + nLf + 1 );
	if( NULL == pBody )
		return ENOMEM;

	pBody = xstrncpy_to_inet( pBody, m_pcBuffer, sizeof( pBody ) );

	cBuffer += pBody;
	free( pBody );

	/* If there are attachments, encode and add to message */
	if( m_vAttachments.size() > 0 )
	{
		/* Emit multi-part boundary to terminate body text */
		cBuffer += os::String( "\r\n" ) + cPartBoundary;

		/* Encode & emit each part */
		os::RegistrarManager *pcRegistrar = NULL;
		pcRegistrar = os::RegistrarManager::Get();

		std::list <os::String>::iterator i;
		for( i = m_vAttachments.begin(); i != m_vAttachments.end(); i++ )
		{
			/* Generate Content- headers */

			/* XXXKV: We only need the type.  Registrar needs a seperate GetType() or GetMimeType() call */
			os::String cType, cMimeType;
			os::Image *pcIcon = new os::BitmapImage();
			os::Message cFileInfo;

			if( pcRegistrar->GetTypeAndIcon( (*i), os::Point( 1, 1 ), &cType, &pcIcon, &cFileInfo ) != EOK )
				continue;

			if( cFileInfo.FindString( "mimetype", &cMimeType ) != EOK )
				cType = "application/octet-stream";

			debug( "%s is %s\n", (*i).c_str(), cMimeType.c_str() );

			cBuffer += os::String( "Content-Type: " ) + cMimeType + os::String( "\r\n" );
			cBuffer += os::String( "Content-Transfer-Encoding: base64\r\n" );

			const char *pSep, *pLast;
			pLast = (*i).c_str();
			while( ( pSep = strchr( pLast, '/' ) ) != NULL )
				pLast = pSep + 1;

			debug( "%s filename is %s\n", (*i).c_str(), pLast );
			
			cBuffer += os::String( "Content-Disposition: attachment;\r\n\tfilename=\"" ) + os::String( pLast ) + os::String( "\"\r\n\r\n" );

			/* Read data & encode */
			try
			{
				ssize_t nSize;
				char *pcBuffer, *pcEncoded = NULL;

				os::File cFile( (*i) );
				nSize = cFile.GetSize();

				pcBuffer = (char*)calloc( 1, nSize + 1 );
				if( NULL == pcBuffer )
					break;

				if( cFile.Read( pcBuffer, nSize ) < nSize )
				{
					debug( "%s: Failed to read all data\n",  (*i).c_str() );
					free( pcBuffer );
					continue;
				}

				Base64Codec cCodec;
				if( cCodec.Encode( &pcEncoded, pcBuffer, nSize ) > 0 )
					cBuffer += os::String( pcEncoded ) + os::String( "\r\n\r\n" );

				if( NULL != pcEncoded )
					free( pcEncoded );
				free( pcBuffer );
			}
			catch( std::exception &e )
			{
				debug( "%s\n", e.what() );
			}

			/* Emit next multi-part boundary */
			cBuffer += cPartBoundary;
		}
		pcRegistrar->Put();
	}

	/* Terminate with \r\n.\r\n and store */
	cBuffer += "\r\n\r\n.\r\n";

	SetData( cBuffer.c_str(), cBuffer.Length() );

	return EOK;
}

/* Scan the data in pBuffer and find the header specified in cFind,
 * extract it to cHeader.
 */
status_t Mailmessage::FindHeader( os::String cFind, os::String &cHeader, const char *pBuffer, ssize_t nSize )
{
	os::String cSearch = os::String( "\n" ) + cFind + os::String( ": " );
	char *pStart, *pEnd, *zRawHeader, *zHeader;

	/* Look for the start of the header */
	pStart = strstr( pBuffer, cSearch.c_str() );
	if( NULL == pStart )
	{
		cHeader = "";
		return ESRCH;
	}

	/* Check that the header that has been found isn't outside of the search area */
	if( ( pStart - pBuffer ) > nSize )
	{
		cHeader = "";
		return ESRCH;
	}

	/* Skip past the header to the begining of the header data */
	pStart = strstr( pStart, ": " );
	if( NULL == pStart )
	{
		cHeader = "";
		return ESRCH;
	}
	pStart += 2;

	/* Find the end of the header data */
	pEnd = strstr( pStart, "\r" );
	if( NULL == pEnd )
	{
		cHeader = "";
		return ESRCH;
	}

	/* Conformant implementations may insert linebreaks in the header data, so not
	   all headers end at \r.  This will try and work out if the header continues
	   on the next line by looking at the character after the \n  If it's whitespace,
	   then we probably didn't find the end of the header yet. */
	while( *(pEnd + 2) == ' ' || *(pEnd + 2) == '\t' )
		pEnd = strstr( pEnd + 2, "\r" );
		if( NULL == pEnd )
		{
			cHeader = "";
			return ESRCH;
		}

	/* Sanity check */
	if( pEnd - pStart <= 0 )
	{
		cHeader = "";
		return EINVAL;
	}

	/* Extract the header data which is between pStart & pEnd */
	zRawHeader = (char*)calloc( 1, ( pEnd - pStart ) + 1 );
	if( NULL == zRawHeader )
	{
		cHeader = "";
		return ENOMEM;
	}

	zRawHeader = strncpy( zRawHeader, pStart, pEnd - pStart );

	/* Strip CR's, LF's & tabs */
	zHeader = (char*)calloc( 1, pEnd - pStart + 1 );
	if( NULL == zHeader )
	{
		cHeader = "";
		return ENOMEM;
	}

	zHeader = xstrnxcpy( zHeader, zRawHeader, pEnd - pStart, "\r\n\t" );
	free( zRawHeader );

	cHeader = zHeader;
	free( zHeader );

	return EOK;
}

/* Find all arguments specifiec in the header and split into into multiple parts */
status_t Mailmessage::ParseHeaderArgs( os::String cHeader, os::String cArgs[] )
{
	const char *pStart, *pEnd;
	char *pBuffer, *zArg;
	int nArg = 0;
	size_t nLen;

	/* We'll first need to strip all \r and \n characters from the header. 
	   See FindHeader() for why this is necasary */
	pBuffer = (char*)calloc( 1, cHeader.size() + 1 );
	if( NULL == pBuffer )
		return ENOMEM;

	pBuffer = xstrnxcpy( pBuffer, cHeader.c_str(), cHeader.size(), "\r\n" );

	/* Find all arguments.  What should be a simple series of strstr()'s is
	   complicated by the different ways mail clients encode this field.  To
	   capture the correct data we use a couple of loops and some pointers.
	   The idea is to skip any whitespace at the start of the field, then find
	   the field seperator ';', or the end of the entire field ('\0').  For each
	   part we find the data is copied into a buffer, upto a maximum of four buffers.
	   The first field is always the MIME type. */
	pEnd = pStart = pBuffer;
	while( nArg <= 3 )
	{
		while( *pStart == ' ' || *pStart == '\t' )
			pStart++;

		while( *pEnd != ';' && *pEnd != '\0' )
			pEnd++;

		if( pEnd - pStart <= 0 )
			break;

		nLen = pEnd - pStart;
		zArg = (char*)calloc( 1, nLen + 1 );
		if( NULL == zArg )
			break;

		zArg = strncpy( zArg, pStart, nLen );
		cArgs[nArg] = zArg;

		if( '\0' == *pEnd )
			break;

		nArg += 1;
		pEnd += 1;
		pStart = pEnd;
	}

	free( pBuffer );

	return EOK;
}

/* Look for the arg cName in every entry in cArgs[].  Parse the params into cParam */
status_t Mailmessage::GetHeaderArg( os::String cArgs[], int nCount, os::String cName, os::String &cParam )
{
	const char *pStart, *pEnd;
	char *pzParam = NULL;
	ssize_t nLen;

	for( int nArg = 0; nArg <= nCount; nArg++ )
	{
		const char *zArg = cArgs[nArg].c_str();
		if( NULL != zArg )
		{
			/* Some MUAs complicate matters by producing arguments in uppercase (fx. Pine), which is a real
			   pain if we're looking for the MIME boundary; you can't simply force everything into lowercase
			   because the boundary string must be found *exactly* as it is provided. */
			pStart = xstrcasestr( zArg, cName.c_str() );
			if( NULL == pStart )
				continue;
			pStart += cName.Length();

			/* Check for quotes.  Some arguments are quoted, others are not.  This checks
			   for both a start and end quote, in case there are broken mail clients out
			   there. */
			if( *pStart == '"' )
				pStart += 1;

			pEnd = strstr( pStart, "\"" );
			if( NULL != pEnd )
				nLen = pEnd - pStart;
			else
				nLen = strlen( zArg ) - 9;

			if( nLen <= 0 )
				break;

			pzParam = (char*)calloc( 1, nLen + 1 );
			if( NULL == pzParam )
				return ENOMEM;

			pzParam = strncpy( pzParam, pStart, nLen );
			break;
		}
	}

	if( NULL == pzParam )
		return EINVAL;

	cParam = pzParam;
	free( pzParam );

	return EOK;
}

/* Split the Content-Type: header into multiple parts, then parse
 * the MIME type into a super type and sub type.
 */
status_t Mailmessage::FindContentType( os::String cHeader, os::String cArgs[], MimeType &cMimeType )
{
	const char *pStart, *pEnd;
	char zMime[4096];
	size_t nLen;

	/* Get the arguments from the Content-Type: header */
	if( ParseHeaderArgs( cHeader, cArgs ) != EOK )
		return EINVAL;

	/* The first argument is always the MIME type.  Do a simple parse to find
	   the MIME super and sub types. */
	if( cArgs[0] == "" )
		return ESRCH;

	pStart = cArgs[0].c_str();
	pEnd = strstr( pStart, "/" );
	nLen = pEnd - pStart;
	if( NULL == pEnd || nLen <= 0 || nLen >= 4096 )
		return EINVAL;
	memset( zMime, 0, 4096 );
	strncpy( zMime, pStart, nLen );
	cMimeType.cSuperType = zMime;
	cMimeType.cSuperType = cMimeType.cSuperType.Lower();

	nLen = cArgs[0].Length() - ( pEnd - pStart );
	if( nLen >= 4096 )
		return EINVAL;
	memset( zMime, 0, 4096 );
	strncpy( zMime, pEnd + 1, nLen );
	cMimeType.cSubType = zMime;
	cMimeType.cSubType = cMimeType.cSubType.Lower();

	return EOK;
}

/* Find start of body in m_pcBuffer and create a part for it */
status_t Mailmessage::FindBody( void )
{
	char *pStart, *pEnd;
	Multipart *pcBodyPart = new Multipart();
	if( NULL == pcBodyPart )
		return ENOMEM;

	pStart = strstr( m_pcBuffer, "\r\n\r\n" );
	if( NULL == pStart )
		return EINVAL;
	pStart += 4;

	pEnd = strstr( pStart, "\r\n.\r\n" );
	if( NULL == pEnd )
		return EINVAL;

	if( pEnd - pStart <= 0 )
		return EINVAL;

	pcBodyPart->pStart = pcBodyPart->pBegin = pStart;
	pcBodyPart->pEnd = pEnd;
	pcBodyPart->m_nSize = pcBodyPart->pEnd - pcBodyPart->pStart;
	pcBodyPart->m_nLen = pcBodyPart->m_nSize;
	pcBodyPart->cMimeType = m_cMimeType;
	pcBodyPart->cEncoding = m_cEncoding;
	/* XXXKV: If a user send an email from Outlook that contains nothing but an attachment it does the wrong
	   thing and puts the base64 encoded attachment straight into the body, so we have to look out for
	   non-text message bodies and treat them as attachments. */
	if( m_cMimeType.cSuperType == "text" )
	{
		pcBodyPart->eDisposition = BODY;

		if( GetHeaderArg( m_cContentTypeArgs, 3, os::String( "charset=" ), pcBodyPart->cCharset ) != EOK )
			pcBodyPart->cCharset = "us-ascii";
	}
	else
	{
		pcBodyPart->eDisposition = ATTACHMENT;
		pcBodyPart->cFilename = "Untitled Attachment";

		if( m_bHaveDisposition )
		{
			os::String cDispositionArgs[4];
			if( ParseHeaderArgs( m_cDisposition, cDispositionArgs ) == EOK )
			{
				/* Attempt to find a suitable filename.  Prefer the filename= disposition arg over the content-type name and
				   fall back on a generic name of we can't find one */
				if( GetHeaderArg( cDispositionArgs, 3, os::String( "filename=" ), pcBodyPart->cFilename ) != EOK )
					if( GetHeaderArg( cDispositionArgs, 3, os::String( "name=" ), pcBodyPart->cFilename ) != EOK )
						pcBodyPart->cFilename = "Untitled Attachment";
			}
		}

		m_nAttachmentCount += 1;
	}

	m_vParts.push_back( pcBodyPart );
	m_nBodyPartIndex = 0;

	return EOK;
}

/* Scan m_pcBuffer looking for the MIME boundary specified in m_pzContentTypeArgs[1].
   Create a part for each boundary we find and push those parts into the vector */
status_t Mailmessage::FindMultiParts( const char *pcBuffer, std::vector <Multipart *> &vParts, os::String cBoundary )
{
	const char *pStart, *pBegin, *pEnd;
	uint nPart;
	Multipart *pcPart;

	/* Scan pcBuffer for every occurance of pzBoundary.  Create and push back a part
	   for every one we find, but don't parse the Content headers yet.  Skip the headers
	   in pcBuffer before we start looking for the parts. */
	pStart = strstr( pcBuffer, "\r\n\r\n" );
	while( NULL != pStart )
	{
		pStart = strstr( pStart, cBoundary.c_str() );
		if( NULL == pStart )
		{
			debug( "failed to find boundary \"%s\"\n", cBoundary.c_str() );
			break;
		}
		pStart += cBoundary.Length();	/* Skip the boundary marker */

		/* XXXKV: Check this to find out exactly what seperates the part headers
		   from the data.  This seems to work for all but one. */
		pBegin = strstr( pStart, "\r\n\r\n" );
		if( NULL == pBegin )
			break;
		pBegin += 4;	/* Skip the blank lines */

		pEnd = strstr( pBegin, cBoundary.c_str() );
		if( NULL == pEnd )
			break;
		pEnd -= 2;	/* Come back over the leading "--" */

		debug( "boundary found at offset %d\n", ( pStart - pcBuffer ) );
		debug( "this part is %d bytes\n", ( pEnd - pBegin ) );

		/* Create a new Multipart and fill in start, begin & end details.  The
		   type & encoding will be found after */
		pcPart = new Multipart();
		if( NULL == pcPart )
			break;

		pcPart->pStart = pStart;
		pcPart->pBegin = pBegin;
		pcPart->pEnd = pEnd;
		pcPart->m_nSize = pEnd - pStart;
		pcPart->m_nLen = pEnd - pBegin;
		pcPart->m_nId = GetNextId();
		vParts.push_back( pcPart );

		pStart = pEnd;
	}

	/* Get the content type, any content type args, encoding and
	   disposition (if included) for each part now in vParts */
	os::String cContent, cEncoding;
	for( nPart = 0; nPart < vParts.size(); nPart++ )
	{
		pcPart = vParts[nPart];

		if( FindHeader( "Content-Type", cContent, pcPart->pStart, pcPart->m_nSize ) == EOK )
			FindContentType( cContent, pcPart->m_cContentTypeArgs, pcPart->cMimeType );
		else
		{
			/* "application/octet-stream" may be safer, but some mailers (GNUS) produce parts with
			   no headers at all (RFC violation?)  Setting the default type to "text/plain" in combination
			   with the logic in IdentifyDisposition() catches the problem and forces at least the first
			   part to be the body, instead of an untitled attachment. */
			pcPart->cMimeType.cSuperType = "text";
			pcPart->cMimeType.cSubType = "plain";
		}

		if( FindHeader( "Content-Transfer-Encoding", cEncoding, pcPart->pStart, pcPart->m_nSize ) == EOK )
			pcPart->cEncoding = cEncoding.Lower();
		else
			pcPart->cEncoding = "7bit";

		if( FindHeader( "Content-Disposition", pcPart->cDisposition, pcPart->pStart, pcPart->m_nSize ) == EOK )
			if( ParseHeaderArgs( pcPart->cDisposition, pcPart->m_cDispositionArgs ) == EOK )
				pcPart->cDisposition = pcPart->m_cDispositionArgs[0];
	}

	/* Identify parts which are themselves multi-part.  Parse them down into individual parts and add
	   to the containing part as children */
	for( nPart = 0; nPart < vParts.size(); nPart++ )
	{
		pcPart = vParts[nPart];

		if( pcPart->cMimeType.cSuperType == "multipart" )
		{
			debug( "Multipart #%d is itself a multipart(/%s)\n", nPart, pcPart->cMimeType.cSubType.c_str() );

			os::String cMultipartBoundary;

			if( GetHeaderArg( pcPart->m_cContentTypeArgs, 3, os::String( "boundary=" ), cMultipartBoundary ) != EOK )
				continue;

			debug( "boundary for this multipart is %s\n", cMultipartBoundary.c_str() );

			char *pMultipartData = (char*)calloc( 1, pcPart->m_nSize + 1 );
			if( NULL == pMultipartData )
				continue;

			pMultipartData = strncpy( pMultipartData, pcPart->pStart, pcPart->m_nSize );

			std::vector <Multipart *> vMultiParts;
			if( FindMultiParts( pMultipartData, vMultiParts, cMultipartBoundary ) != EOK )
				continue;

			debug( "this part contains %d parts\n", vMultiParts.size() );

			IdentifyDisposition( vMultiParts );

			for( uint nMultiPart = 0; nMultiPart < vMultiParts.size(); nMultiPart++ )
				pcPart->AddChild( vMultiParts[nMultiPart] );
		}
	}

	return EOK;
}

/* For each part in m_vParts, identify it's disposition.  We recognise parts as either attachments, inline attachments,
   body text or alternative body text.  Also attempt to identify a suitable body.  Favour text/plain but
   allow text/html if we find it, although keep looking for text/plain.  Return the index of the part identified as the
   body. */
int Mailmessage::IdentifyDisposition( std::vector <Multipart *> &vParts )
{
	uint nPart;
	Multipart *pcPart;
	int nBodyPartIndex = -1;

	m_nAttachmentCount = 0;

	debug( "message has %d parts\n", vParts.size() );

	for( nPart = 0; nPart < vParts.size(); nPart++ )
	{
		pcPart = vParts[nPart];

		debug( "part %d is %s/%s\n", nPart, pcPart->cMimeType.cSuperType.c_str(), pcPart->cMimeType.cSubType.c_str() );

		if( pcPart->cDisposition != "" )
			debug( "%s\n", pcPart->cDisposition.c_str() );

		if( pcPart->cMimeType.cSuperType == "text" )
		{
			/* Get a charset */
			if( GetHeaderArg( pcPart->m_cContentTypeArgs, 3, os::String( "charset=" ), pcPart->cCharset ) != EOK )
				pcPart->cCharset = "us-ascii";
		}

		/* Set the correct disposition for this attachment and try to identify a body if we need one */
		/* XXXKV: Be careful of the order here! */
		if( pcPart->cMimeType.cSuperType == "multipart" )
			pcPart->eDisposition = MULTIPART;
		else if( pcPart->cMimeType.cSubType == "pgp-signature" )
			pcPart->eDisposition = ENCRYPTION_KEY;
		else if( pcPart->cDisposition == "inline" )
		{
			pcPart->eDisposition = INLINE;
			m_nAttachmentCount += 1;
		}
		else if( pcPart->cDisposition == "attachment" )
		{
			pcPart->eDisposition = ATTACHMENT;
			m_nAttachmentCount += 1;
		}
		else if( pcPart->cMimeType.cSubType == "plain" )
		{
			pcPart->eDisposition = BODY;
			if( nBodyPartIndex == -1 )
				nBodyPartIndex = nPart;
			continue;
		}
		else if( pcPart->cMimeType.cSubType == "html" )
		{
			/* BODY_ALTERNATIVE parts are hidden by default; we may unhide the first one later
			  (See below) if we don't find an acceptable BODY */
			pcPart->eDisposition = BODY_ALTERNATIVE_HIDDEN;
			continue;
		}
		else
		{
			/* By default make everything an attachment as the safe option */
			pcPart->eDisposition = ATTACHMENT;
			m_nAttachmentCount += 1;
		}

		/* Attempt to find a suitable filename.  Prefer the filename= disposition arg over the content-type name and
		   fall back on a generic name of we can't find one */
		if( GetHeaderArg( pcPart->m_cDispositionArgs, 3, os::String( "filename=" ), pcPart->cFilename ) != EOK )
			if( GetHeaderArg( pcPart->m_cContentTypeArgs, 3, os::String( "name=" ), pcPart->cFilename ) != EOK )
				pcPart->cFilename = "Untitled Attachment";

		debug( "Filename: %s\n", pcPart->cFilename.c_str() );
	}

	/* If the message is "multipart/signed" and we have a body, mark the body as BODY_SIGNED */
	if( m_cMimeType.cSubType == "signed" && nBodyPartIndex > -1 )
	{
		pcPart = vParts[nBodyPartIndex];
		pcPart->eDisposition = BODY_SIGNED;
	}

	/* If all we found were BODY_ALTERNATIVE_HIDDEN, mark the first as BODY_ALTERNATIVE */
	if( nBodyPartIndex == -1 )
		for( nPart = 0; nPart < vParts.size(); nPart++ )
		{
			pcPart = vParts[nPart];
			if( pcPart->eDisposition == BODY_ALTERNATIVE_HIDDEN )
			{
				pcPart->eDisposition = BODY_ALTERNATIVE;
				nBodyPartIndex = nPart;
			}
		}

	/* If the first part has a disposition of INLINE but a MIME super-type of "text", nominate it as BODY */
	if( nBodyPartIndex == -1 && vParts.size() > 0 )
	{
		pcPart = vParts[0];
		if( pcPart->eDisposition == INLINE && pcPart->cMimeType.cSuperType == "text" )
		{
			pcPart->eDisposition = BODY;
			nBodyPartIndex = 0;
			m_nAttachmentCount -= 1;
		}
	}
 
	return nBodyPartIndex;
}

/* Strip unwanted characters from the header; we don't want quotes (' or ") nor punctuation (; and ,) */
void Mailmessage::CleanHeader( os::String &cHeader )
{
	char *zHeader = (char*)calloc( 1, cHeader.Length() + 1 );
	zHeader = xstrnxcpy( zHeader, cHeader.c_str(), cHeader.Length(), ",;'\"" );
	cHeader = zHeader;
	free( zHeader );
}

