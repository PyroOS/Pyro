#include <pst.h>
#include <messages.h>
#include <mail.h>
#include <timeconv.h>
#include <strfunc.h>

#include <util/message.h>
#include <gui/filerequester.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>

#include <iostream>
#include <algorithm>

using namespace os;

IXPlugin * PstNode::GetPlugin( void )
{
	return new PstPlugin();
}

PstPlugin::PstPlugin( void )
{

}

PstPlugin::~PstPlugin( void )
{

}

status_t PstPlugin::CheckFile( const os::String cFilename )
{
	status_t nError;
	pst_file sPstfile;

	nError = pst_open( &sPstfile, (char*)cFilename.c_str(), "r" );
	if( nError != 0 )
		return EINVAL;

	nError = pst_load_index( &sPstfile );
	if( nError != 0 )
		nError = EINVAL;

	pst_close( &sPstfile );
	return nError;
}

status_t PstPlugin::Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename )
{
	PstImporter *pcImporter = new PstImporter( pcParent, cFolder, cFilename );
	if( pcImporter == NULL )
		return ENOMEM;

	pcImporter->Start();

	return EOK;
}

PstImporter::PstImporter( os::Window *pcParent, const os::String cFolder, const os::String cFilename ) : Thread( "pst_importer" )
{
	m_pcParent = pcParent;
	m_cFolder = cFolder;
	m_cFilename = cFilename;
	m_pcMessenger = new Messenger( m_pcParent );
}

PstImporter::~PstImporter( void )
{

}

int32 PstImporter::Run()
{
	std::cerr << "start import of " << m_cFilename.str() << std::endl;

	m_pcDialog = new IXPortProgressDialog( Rect( 50, 50, 300, 150 ), String( "Importing Microsoft Outlook PST file " ) + m_cFilename );
	m_pcDialog->CenterInWindow( m_pcParent );
	m_pcDialog->Go( new Invoker( new Message( M_IXPORT_CANCEL ), m_pcParent ) );

	/* Get the filesize */
	struct stat sInfo;
	if( stat( m_cFilename.c_str(), &sInfo ) == 0 )
		m_nSize = sInfo.st_size;
	else
		m_nSize = 1;

	/* Open & verify PST file */
	pst_file sPstfile;
	pst_item *item = NULL;
	pst_desc_ll *d_ptr;
	FILE *hMbox;

	status_t nError = pst_open( &sPstfile, (char*)m_cFilename.c_str(), "r" );
	if( nError != 0 )
	{
		std::cerr << "failed to open PST file " << m_cFilename.str() << std::endl;
		goto error;
	}

	nError = pst_load_index( &sPstfile );
	if( nError != 0 )
	{
		std::cerr << "failed to load index" << std::endl;
		goto error1;
	}

	pst_load_extended_attributes( &sPstfile );

	/* Get the total size of the file */
	hMbox = fopen( m_cFilename.c_str(), "r" );
	if( NULL == hMbox )
		return EIO;

	/* Find the root message store */
	d_ptr = sPstfile.d_head; // first record is main record
	item = (pst_item*)_pst_parse_item( &sPstfile, d_ptr);
	if( NULL == item || item->message_store == NULL )
	{
		std::cerr << "failed to find root record" << std::endl;
		goto error1;
	}

	d_ptr = pst_getTopOfFolders( &sPstfile, item );
	if( NULL == d_ptr )
	{
		std::cerr << "could not find top of folder record" << std::endl;
		goto error1;
	}

	if( item != NULL )
	{
		_pst_freeItem(item);
		item = NULL;
	}
	d_ptr = d_ptr->child;

	/* d_ptr now points to the first item within the root message store; start processing */
	m_bRun = true;
	nError = ProcessFolder( &sPstfile, d_ptr, 0, m_cFolder );
	if( nError != EOK )
		printf( "process_folder() failed\n" );

error1:
	pst_close( &sPstfile );
error:
	/* Close the progress dialog */
	m_pcDialog->Close();

	/* Let the application know that we're done */
	Message *pcMessage = new Message( M_IMPORT_COMPLETE );
	m_pcMessenger->SendMessage( pcMessage );
	return EOK;
}

int32 PstImporter::skip_count = 0;
int32 PstImporter::email_count = 0;
uint64 PstImporter::m_nCount = 0;

status_t PstImporter::ProcessFolder( pst_file *psPstfile, pst_desc_ll *d_ptr, int32 nItemCount, os::String cFolder )
{
	status_t nError = EOK;
	pst_item *item = NULL;

	while( d_ptr != NULL && m_bRun )
	{
		std::cerr << "New item record" << std::endl;

		if( d_ptr->desc == NULL )
			break;

		item = (pst_item*)_pst_parse_item( psPstfile, d_ptr );
		if( NULL == item )
			goto next;

		if( item->message_store != NULL )
		{
			/* There should only be one message_store, and we have already done it */
			std::cerr << "A second message_store has been found. Sorry, this must be an error." << std::endl;
			return EINVAL;
		}

		/* Process the record according to its type */
		if( item->folder != NULL )
		{
			/* Create a child folder */
			String cChild = String( item->file_as );
			String cPath = cFolder + String( "/" ) + cChild;

			std::cerr << "M_IMPORT: found a child folder \"" << cChild.str() << "\"" << std::endl;

			Message *pcMessage = new Message( M_IMPORT_CREATE_FOLDER );
			pcMessage->AddString( "parent", cFolder );
			pcMessage->AddString( "name", cChild );
			m_pcMessenger->SendMessage( pcMessage );

			nError = ProcessFolder( psPstfile, d_ptr->child, item->folder->email_count, cPath );
		}
		else if( item->email != NULL && ( item->type == PST_TYPE_NOTE || item->type == PST_TYPE_REPORT ) )
		{
			/* An email */
			std::cerr << "found a mail message" << std::endl;

			nError = ProcessMessage( psPstfile, item, cFolder );
			if( nError != EOK )
			{
				std::cerr << "failed to process message" << std::endl;
				skip_count++;
			}
			else
				email_count++;
		}
		else if( item->contact != NULL )
		{
			/* A contact record (Ignored) */
			std::cerr << "found a contact record" << std::endl;
			skip_count++;
		}
		else if( item->type == PST_TYPE_JOURNAL )
		{
			/* A journal record (Ignored) */
			std::cerr << "found a journal record" << std::endl;
			skip_count++;
		}

next:
		if( item != NULL)
		{
			_pst_freeItem(item);
			item = NULL;
		}

		d_ptr = d_ptr->next;
	}

	return nError;
}

status_t PstImporter::ProcessMessage( pst_file *psPstfile, pst_item *item, os::String cFolder )
{
	time_t em_time;
	char *c_time;
	char *boundary = NULL, *b1, *b2;
	char *enc;
	bool base64_body = false, convert_to_multipart = false;

	/* Convert the sent date if it exists, or set it to a fixed date */
	if( item->email->sent_date != NULL )
	{
		em_time = fileTimeToUnixTime( item->email->sent_date, 0 );
		c_time = ctime(&em_time);
		if( c_time != NULL )
			c_time[strlen( c_time ) - 1] = '\0'; /* remove last newline */
		else
			c_time = "Fri Dec 28 12:06:21 2001";
	}
	else
		c_time = "Fri Dec 28 12:06:21 2001";

	/* Get as much useful data as possible from the headers */
	if( item->email->header != NULL )
	{
	  /* See if there is a multipart boundary already. Also, check to find out if we are looking
		 at the boundary associated with content-type, and that the content type really is "multipart" */

		b2 = pst_stristr( item->email->header, "boundary=" );
		if( b2 != NULL)
		{
			b2 += strlen( "boundary=" );	/* Move to first char of boundary marker */
			if( *b2 == '"' )
			{
				b2++;					/* Skip opening quote */
				b1 = strchr( b2, '"' );	/* Find terminating quote */
			}
			else
			{
				b1 = b2;
				while( isgraph( *b1 ) )	/* Find first char that isn't part of boundary */
					b1++;
			}
	    
			boundary = (char*)calloc( 1, ( b1-b2 ) +1 );
			strncpy( boundary, b2, b1-b2 );
			b1 = b2 = boundary;

			/* Strip CRs and tabs */
			while( *b2 != '\0' )
			{
				if(*b2 != '\n' && *b2 != '\r' && *b2 != '\t' )
				{
					*b1 = *b2;
					b1++;
				}
				b2++;
			}
			*b1 = '\0';

			std::cerr << "multipart boundary is \"" << boundary << "\"" << std::endl;
		}
		else
			std::cerr << "multipart boundary not found" << std::endl;

		/* Find transfer encoding */
		b2 = pst_stristr( item->email->header, "Content-Transfer-Encoding:" );
		if( b2 != NULL )
		{
			if((b2 = strchr(b2, ':')) != NULL)
			{
				b2++; // skip to the : at the end of the string
		
				while (*b2 == ' ' || *b2 == '\t')
					b2++;

				if( pst_strincmp( b2, "base64", 6 ) == 0 )
				{
					printf( "body is base64 encoded\n" );
					base64_body = true;
				}
			}
		}
		else
			std::cerr << "did not find content-transfer-encoding" << std::endl;
	}

	/* If the message has a body type which will be output as an attachment but is not a multipart
	   message already, we'll have to create our own multipart data */
	if( boundary == NULL && ( item->attach || ( item->email->body && item->email->htmlbody ) ) )
#if 0
	|| item->email->rtf_compressed || item->email->encrypted_body || item->email->encrypted_htmlbody
#endif
	{
		boundary = (char*)calloc( 1, 64 );
		snprintf( boundary, 64, "--=_NextPart-pst-import-%i_-_-", rand() );
		convert_to_multipart = true;
		std::cerr << "created our own boundary \"" << boundary << "\"" << std::endl;
	}

	String cMessage;
	char *temp;

	/* Process the headers */
	if( item->email->header != NULL && convert_to_multipart == false )
	{
		/* Some of the headers we get from the file are not properly defined; they can contain some
		   email stuff too.  We will cut off the header when we see a \n\n or \r\n\r\n */
		removeCR( item->email->header );

		temp = strstr( item->email->header, "\n\n" );
		if( temp != NULL )
		{
			temp += 2;	/* Advance past the \n\n */
			*temp = '\0';
		}
	    char *soh = skip_header_prologue( item->email->header );
		cMessage = soh;
	}
	else
	{
		/* We'll have to create our own RFC2822 headers from the data available */

		/* From */
		temp = item->email->outlook_sender;
		if( temp == NULL )
			temp = "";
		cMessage = String( "From: \"" ) + String( item->email->outlook_sender_name ) + String( "\" <" ) + String( temp ) + String( ">\n" );

		/* Subject */
		cMessage += String( "Subject: " );
		if( item->email->subject != NULL )
			cMessage += String( item->email->subject->subj );
		cMessage += String( "\n" );

		/* To */
		cMessage += String( "To: " ) + String( item->email->sentto_address ) + String( "\n" );

		/* Cc */
		if( item->email->cc_address != NULL )
			cMessage += String( "CC: " ) + String( item->email->cc_address ) + String( "\n" );

		/* Date */
		if( item->email->sent_date != NULL )
		{
			c_time = (char*)xmalloc( C_TIME_SIZE );
			strftime( c_time, C_TIME_SIZE, "%a, %d %b %Y %H:%M:%S %z", gmtime( &em_time ) );
			cMessage += String( "Date: " ) + String( c_time )  + String( "\n" );
			free(c_time);
		}

		/* MIME headers */
		cMessage += String( "MIME-Version: 1.0\n" );
		if( item->attach != NULL || convert_to_multipart )
			cMessage += String( "Content-Type: multipart/mixed;\n\tboundary=\"" ) + String( boundary ) + String( "\"\n" );
		else if( item->email->htmlbody && item->email->body )
			cMessage += String( "Content-Type: multipart/alternative;\n\tboundary=\"" ) + String( boundary ) + String( "\"\n" );
		else if( item->email->body && !item->email->htmlbody )
			cMessage += String( "Content-Type: text/plain\n" );
		else if( item->email->htmlbody )
			cMessage += String( "Content-Type: text/html\n" );

		/* Blank line to delimit headers from body */
		cMessage += "\n";
	}

	/* Add the body, with appropriate multipart boundaries as required.  This importer currently
	   ignores RTF and encrypted bodies */
	if( item->email->body != NULL )
	{
		if( boundary )
		{
			cMessage += String( "\n--" ) + String( boundary ) + String( "\nContent-Type: text/plain\n" );
			if (base64_body)
				cMessage += String( "Content-Transfer-Encoding: base64\n" );
		}
		cMessage += String( "\n" );

		removeCR( item->email->body );
		if( base64_body )
		{
			enc = base64_encode( item->email->body, strlen( item->email->body ) );
			cMessage += String( enc );
		}
		else
			cMessage += item->email->body;
	}

	if (item->email->htmlbody != NULL)
	{
		if( boundary )
		{
			cMessage += String( "\n--" ) + String( boundary ) + String( "\nContent-Type: text/html\n" );
			if (base64_body)
				cMessage += String( "Content-Transfer-Encoding: base64\n" );
		}
		cMessage += String( "\n" );

		removeCR( item->email->htmlbody );
		if( base64_body )
		{
			enc = base64_encode( item->email->body, strlen( item->email->body ) );
			cMessage += String( enc );
		}
		else
			cMessage += item->email->body;
	}

	/* Attachments */
	item->current_attach = item->attach;
	while( item->current_attach != NULL )
	{
		std::cerr << "Starting attachment encoding" << std::endl;

		if( item->current_attach->data == NULL )
		{
			std::cerr << "Data of attachment is NULL! Size is supposed to be " << item->current_attach->size << std::endl;
			item->current_attach = item->current_attach->next;
			continue;
		}
		std::cerr << "Attachment size is " << item->current_attach->size << std::endl;

		enc = base64_encode( item->current_attach->data, item->current_attach->size );
		if( enc == NULL)
		{
			std::cerr << "ERROR base64_encode returned NULL. Must have failed" << std::endl;
			item->current_attach = item->current_attach->next;
			continue;
		}

		if( boundary )
		{
			cMessage += String( "\n--" ) + String( boundary ) + String( "\n" );
			if( item->current_attach->mimetype == NULL )
				cMessage += String( "Content-Type: " ) + String( "application/octet-stream" );
			else
				cMessage += String( "Content-Type: " ) + String( item->current_attach->mimetype );
			cMessage += String( "\nContent-Transfer-Encoding: base64\n" );
			if( item->current_attach->filename2 == NULL )
				cMessage += String( "Content-Disposition: inline\n\n" );
			else
				cMessage += String( "Content-Disposition: attachment; filename=\"" ) + String( item->current_attach->filename2 ) + String( "\"\n\n" );
		}

		if( item->current_attach->data != NULL )
			cMessage += enc;
		else
		{
#if 0
			/* XXXKV: Need an alternative here! */
			pst_attach_to_file_base64(&pstfile, item->current_attach, f->output);
			fprintf(f->output, "\n\n");
#else
			std::cerr << "item->current_attach->data is NULL  and we have no way to encode the attachment yet!" << std::endl;
#endif
		}

		/* Next */
		item->current_attach = item->current_attach->next;
	}

	/* Terminate the message */
	if( boundary )
		cMessage += String( "\n--" ) + String( boundary ) + String( "\n" );
	cMessage += String( "\n.\n" );

	m_nBytes = std::max( (int64)m_nBytes, (int64)( ftello( psPstfile->fp ) ) );
	return AddMessage( cMessage, cFolder );
}

status_t PstImporter::AddMessage( os::String &cMessage, os::String cFolder )
{
	if( cMessage != "" )
	{
		/* Convert linefeeds to CRLF pairs */
		const char *pStart, *pEnd;
		int nLf = 0;

		pStart = cMessage.c_str();
		while( ( pEnd = strchr( pStart, '\n' ) ) != NULL )
		{
			nLf++;
			pStart = pEnd + 1;
		}

		char *pMessage = (char*)calloc( 1, cMessage.size() + nLf + 1 );
		if( NULL == pMessage )
			return ENOMEM;

		const char *pIn;
		char *pOut;

		pIn = cMessage.c_str();
		pOut = pMessage;

		while( '\0' != *pIn )
		{
			if( '\n' == *pIn )
				*pOut++ = '\r';
			*pOut++ = *pIn++;
		}
		cMessage = pMessage;
		free( pMessage );

		/* Create a Mailmessage and send it back down to the application along with the relevent data */
		Mailmessage *pcImport = new Mailmessage( cMessage.c_str(), cMessage.size() );

		Message *pcImportMessage = new Message( M_IMPORT_NEW );
		pcImportMessage->AddPointer( "message", pcImport );
		pcImportMessage->AddString( "folder", cFolder );

		m_pcMessenger->SendMessage( pcImportMessage );

		/* Update the progress dialog & check if the user canceled */
		float vProgress = ( (double)m_nBytes / (double)m_nSize );

		char zMessage[64] = { '\0' };
		snprintf( zMessage, 64, "Importing message #%Ld", m_nCount++ );

		m_pcDialog->Lock();
		m_pcDialog->SetMessage( zMessage );
		m_pcDialog->SetProgress( vProgress );
		m_bRun = !m_pcDialog->IsCancelled();
		m_pcDialog->Unlock();
	}

	cMessage = "";
	return EOK;
}

/* case-insensitive strstr() */
char * PstImporter::pst_stristr( char *haystack, char *needle )
{
	char *x=haystack, *y=needle, *z = NULL;

	if (haystack == NULL || needle == NULL)
		return NULL;

	while (*y != '\0' && *x != '\0')
	{
		if( tolower( *y ) == tolower( *x ) )
		{
			y++;
			if( z == NULL )
				z = x;
		}
		else
		{
			y = needle;
			z = NULL;
		}
		x++;
	}
	return z;
}

/* Convert CRLF to LF */
char * PstImporter::removeCR( char *c )
{
	char *a, *b;
	a = b = c;
	while (*a != '\0')
	{
		*b = *a;
		if (*a != '\r')
			b++;
		a++;
	}
	*b = '\0';
	return c;
}

/* Skip the pseudo-header prologue that Microsoft Outlook inserts at the beginning of the
   internet email headers for emails stored in their "Personal Folders" files. */
char * PstImporter::skip_header_prologue( char *headers )
{
	const char *bad = "Microsoft Mail Internet Headers";
	size_t len = strlen( bad );

	if( strncmp( headers, bad, len ) == 0 )
	{
		char *pc = strchr(headers, '\n');
		return pc + 1;
	}
	return headers;
}

extern "C"
{
	PstNode * get_ix_node( void )
	{
		return new PstNode();
	}
}

