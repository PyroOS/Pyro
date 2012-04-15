#include <mbox.h>
#include <messages.h>
#include <mail.h>

#include <util/message.h>
#include <gui/filerequester.h>
#include <storage/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

using namespace os;

IXPlugin * MboxNode::GetPlugin( void )
{
	return new MboxPlugin();
}

MboxPlugin::MboxPlugin( void )
{

}

MboxPlugin::~MboxPlugin( void )
{

}

status_t MboxPlugin::CheckFile( const os::String cFilename )
{
	status_t nError = EINVAL;

	FILE *hMbox = fopen( cFilename.c_str(), "r" );
	if( NULL == hMbox )
		return EIO;

	char pHead[15];
	const char pFromSeq[] = "\nFrom ";
	const char pEudoraSeq[] = { 0xa, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x53 };

	fgets( pHead, 15, hMbox );

	if( memcmp( pHead, &pFromSeq[1], 5 ) == 0 || memcmp( pHead, &pEudoraSeq[1], 14 ) == 0 )
		nError = EOK;

	fclose( hMbox );
	return nError;
}

status_t MboxPlugin::Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename )
{
	MboxImporter *pcImporter = new MboxImporter( pcParent, cFolder, cFilename );
	if( pcImporter == NULL )
		return ENOMEM;

	pcImporter->Start();

	return EOK;
}

status_t MboxPlugin::Export( os::Window *pcParent, Mailfolder *pcFolder, const os::String cFilename )
{
	MboxExporter *pcExporter = new MboxExporter( pcParent, pcFolder, cFilename );
	if( pcExporter == NULL )
		return ENOMEM;

	pcExporter->Start();

	return EOK;
}

uint64 MboxImporter::m_nCount = 1;

MboxImporter::MboxImporter( os::Window *pcParent, const os::String cFolder, const os::String cFilename ) : Thread( "mbox_importer" )
{
	m_pcParent = pcParent;
	m_cFolder = cFolder;
	m_cFilename = cFilename;
	m_pcMessenger = new Messenger( m_pcParent );
}

MboxImporter::~MboxImporter( void )
{

}

int32 MboxImporter::Run()
{
	std::cerr << "start import of " << m_cFilename.str() << std::endl;

	FILE *hMbox = fopen( m_cFilename.c_str(), "r" );
	if( NULL == hMbox )
		return EIO;

	/* Get the filesize */
	fseek( hMbox, 0, SEEK_END );
	m_nSize = ftello( hMbox );
	fseek( hMbox, 0, SEEK_SET );

	/* Auto-detect the mbox format and set the appropriate message delimiter */
	char pHead[15], pSeq[16];
	const char pFromSeq[] = "\nFrom ";
	const char pEudoraSeq[] = { 0xa, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x53 };
	String cDialogTitle = "Importing ";

	fgets( pHead, 15, hMbox );

	if( memcmp( pHead, &pFromSeq[1], 5 ) == 0 )
	{
		/* Standard UNIX mbox */
		memcpy( pSeq, pFromSeq, 6 );
		m_bDoCrLf = true;

		cDialogTitle += "UNIX mbox";

		std::cerr << "UNIX mbox format" << std::endl;
	}
	else if( memcmp( pHead, &pEudoraSeq[1], 14 ) == 0 )
	{
		/* Eudora mbox */
		memcpy( pSeq, pEudoraSeq, 15 );
		m_bDoCrLf = false;

		cDialogTitle += "Eudora mbox";

		std::cerr << "Eudora mbox format" << std::endl;
	}
	else
	{
		std::cerr << "unknown mbox format" << std::endl;

		fclose( hMbox );
		return EINVAL;
	}
	fseek( hMbox, 0, SEEK_SET );

	m_pcDialog = new IXPortProgressDialog( Rect( 50, 50, 300, 150 ), cDialogTitle + String( " file " ) + m_cFilename );
	m_pcDialog->CenterInWindow( m_pcParent );
	m_pcDialog->Go( new Invoker( new Message( M_IXPORT_CANCEL ), m_pcParent ) );

	/* Here be dragons

	   This code is stupid, because mbox is stupid.  Each message in the file is delimited by the sequence "From " at the
	   start of a line E.g. we need to look for the sequence "\nFrom "  The code reads the stream one byte at a time and
	   then compares the charcter against the sequence we are looking for, stored in the array pSeq.  If we match a
	   character, we place the byte into pBuf and increment the counter j to the next character in the sequence.  If the
	   character does *not* match but we have matches predeceding characters in the sequence, then we have to do a bit
	   of a dance.  The first thing we do is copy the contents of pBuf to the message buffer cMessage.  The last character
	   we read is ungetc()'d back into the stream, and the counter j is reset to the start of the sequence pSeq.

	   If we match all of the characters in pSeq against the input stream (j is 6), then we have found the sequence
	   "\nFrom " and have found the start of the next message.  The message is added, and then we have to seek through
	   the stream until the end of the line; the message body will begin directly after the newline.

	   If the character does not match pSeq, and we have not matched the first character in pSeq, then the character
	   if pushed into the message buffer.
	*/
 
	String cMessage;
	int c, j = 0, k = 0;
	int nSeqLen = strlen( pSeq );
	int pBuf[nSeqLen];

	/* The first message in the mbox will not begin with a newline, so we have to provide our own */
	ungetc( '\n', hMbox );

	m_nBytes = 0;
	m_bRun = true;
	m_nCount = 1;

	while( m_bRun )
	{
		/* Track the total number of bytes we've read so far */
		++m_nBytes;

		c = fgetc( hMbox );
		if( j > 0 && c != pSeq[j] )
		{
			/* We found part of the sequence, but this character didn't match.  Put the characters we did match into
			   cMessage and push the character we've just read back into the stream.  We do this in case c is a linefeed;
			   it may be the start of the sequence we're looking for, but if we don't put back into the stream we'll
			   never match the sequence properly */

			for( k = 0; k < j; k++ )
				cMessage += (char)pBuf[k];
			ungetc( c, hMbox );
			j = 0;
		}
		else if( c == pSeq[j] )
		{
			pBuf[j] = c;
			++j;
			if( j == nSeqLen )
			{
				/* Matched the sequence; add this message */
				AddMessage( cMessage );

				/* Read-ahead to the end of the line */
				while( true )
				{
					c = fgetc( hMbox );
					if( c == '\n' || c == EOF )
						break;
				}
				j = 0;
			}
		}
		else if( c == EOF )
		{
			AddMessage( cMessage );
			break;
		}
		else
			cMessage += (char)c;
	}

	fclose( hMbox );

	/* Close the progress dialog */
	m_pcDialog->Close();

	/* Let the application know that we're done */
	Message *pcMessage = new Message( M_IMPORT_COMPLETE );
	m_pcMessenger->SendMessage( pcMessage );

	return EOK;
}

status_t MboxImporter::AddMessage( os::String &cMessage )
{
	if( cMessage != "" )
	{
		if( m_bDoCrLf )
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
		}

		/* Add an RFC2822 terminating sequence */
		cMessage += "\r\n.\r\n";

		/* Create a Mailmessage and send it back down to the application along with the relevent data */
		Mailmessage *pcImport = new Mailmessage( cMessage.c_str(), cMessage.size() );

		Message *pcImportMessage = new Message( M_IMPORT_NEW );
		pcImportMessage->AddPointer( "message", pcImport );
		pcImportMessage->AddString( "folder", m_cFolder );

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

MboxExporter::MboxExporter( os::Window *pcParent, Mailfolder *pcFolder, const os::String cFilename ) : Thread( "mbox_exporter" )
{
	m_pcParent = pcParent;
	m_pcFolder = pcFolder;
	m_cFilename = cFilename;
}

MboxExporter::~MboxExporter( void )
{

}

int32 MboxExporter::Run()
{
	std::cerr << "starting export to " << m_cFilename.c_str() << std::endl;

	File *pcMbox;
	try
	{
		pcMbox = new File( m_cFilename, O_CREAT|O_WRONLY);
	}
	catch( std::exception &e )
	{
		std::cerr << "failed to create output file: " << e.what() << std::endl;
		return EIO;
	}

	IXPortProgressDialog *pcDialog = new IXPortProgressDialog( Rect( 50, 50, 300, 150 ), String( "Exporting folder to mbox file " ) + m_cFilename );
	pcDialog->CenterInWindow( m_pcParent );
	pcDialog->Go( new Invoker( new Message( M_IXPORT_CANCEL ), m_pcParent ) );

	m_pcFolder->Lock();

	Mailsummery cSummery;
	Mailmessage *pcMailMessage;
	status_t nError;
	uint64 nCount = 0;
	bool bRun = true;
	while( ( nError = m_pcFolder->GetNextEntry( &cSummery ) ) != ENOENT && bRun )
	{
		if( EISDIR == nError )
			continue;

		pcMailMessage = new Mailmessage();
		m_pcFolder->Read( cSummery.cReference, pcMailMessage );
		pcMailMessage->Parse();

		char *pzBody, *pzRawBody = pcMailMessage->GetData();
		size_t nSize = pcMailMessage->GetDataSize();

		/* Strip carriage returns from the body text */
		char *pIn, *pOut;
		pzBody = pOut = (char*)calloc( 1, nSize );
		if( NULL == pzBody )
		{
			std::cerr << "out of memory" << std::endl;
			return ENOMEM;
		}
		pIn = pzRawBody;
		nSize = 0;
		do
		{
			if( *pIn != '\r' )
			{
				*pOut++ = *pIn;
				nSize++;
			}
		}
		while( *pIn++ != '\0' );

		/* Come back over trailing CRLF.CRLF sequence */
		nSize -= 5;

		String cFrom = String( "From " ) + pcMailMessage->GetFrom() + String( " " ) + pcMailMessage->GetDate() + String( "\n" );
		pcMbox->Write( cFrom.c_str(), cFrom.size() );
		pcMbox->Write( pzBody, nSize );
		String cSeperator = String( "\n" );
		pcMbox->Write( cSeperator.c_str(), cSeperator.size() );

		delete( pcMailMessage );
		free( pzBody );

		/* Update progress dialog */
		char zMessage[64] = { '\0' };
		snprintf( zMessage, 64, "Exporting message #%Ld", nCount++ );

		pcDialog->Lock();
		pcDialog->SetMessage( zMessage );
		bRun = !pcDialog->IsCancelled();
		pcDialog->Unlock();
	}

	m_pcFolder->Unlock();
	delete( pcMbox );

	/* Close the progress dialog */
	pcDialog->Close();

	return EOK;
}

extern "C"
{
	MboxNode * get_ix_node( void )
	{
		return new MboxNode();
	}
}

