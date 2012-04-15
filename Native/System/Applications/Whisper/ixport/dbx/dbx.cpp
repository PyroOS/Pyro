#include <dbx.h>
#include <messages.h>
#include <mail.h>

#include <util/message.h>
#include <storage/path.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

using namespace os;

IXPlugin * DbxNode::GetPlugin( void )
{
	return new DbxPlugin();
}

DbxPlugin::DbxPlugin( void )
{

}

DbxPlugin::~DbxPlugin( void )
{

}

status_t DbxPlugin::CheckFile( const os::String cFilename )
{
	status_t nError = EINVAL;
	DBX *hDbx;

	hDbx = dbx_open( cFilename.c_str() );
	if( hDbx != NULL )
	{
		if( hDbx->type == DBX_TYPE_FOLDER || hDbx->type == DBX_TYPE_EMAIL )
			nError = EOK;
		dbx_close( hDbx );
	}

	return nError;
}

status_t DbxPlugin::Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename )
{
	DbxImporter *pcImporter = new DbxImporter( pcParent, cFolder, cFilename );
	if( pcImporter == NULL )
		return ENOMEM;

	pcImporter->Start();

	return EOK;
}

uint64 DbxImporter::m_nTotal = 1;

DbxImporter::DbxImporter( os::Window *pcParent, const os::String cFolder, const os::String cFilename ) : Thread( "dbx_importer" )
{
	m_pcParent = pcParent;
	m_cFolder = cFolder;
	m_cFilename = cFilename;
	m_pcMessenger = new Messenger( m_pcParent );
}

DbxImporter::~DbxImporter( void )
{

}

int32 DbxImporter::Run()
{
	std::cerr << "start import of " << m_cFilename.str() << std::endl;

	DBX *hDbx;
	status_t nError;

	hDbx = dbx_open( m_cFilename.c_str() );
	if( hDbx == NULL )
	{
		std::cerr << "failed to open \"" << m_cFilename.str() << "\"" << std::endl;
		return EINVAL;
	}

	/* We need to know the path to the containing directory */
	const char *pIn = m_cFilename.c_str() + m_cFilename.size();
	size_t nLen = m_cFilename.size();
	for( ; pIn >= m_cFilename.c_str(); pIn--, nLen-- )
		if( *pIn == '/' )
		{
			nLen += 1;
			break;
		}
	char *zPath = (char*)calloc( 1, nLen + 1 );
	zPath = (char*)strncpy( zPath, m_cFilename.c_str(), nLen );
	m_cPath = zPath;
	free( zPath );

	/* Create and display a progress dialog */
	m_pcDialog = new IXPortProgressDialog( Rect( 50, 50, 300, 150 ), String( "Importing Outlook Express file " ) + m_cFilename );
	m_pcDialog->CenterInWindow( m_pcParent );
	m_pcDialog->Go( new Invoker( new Message( M_IXPORT_CANCEL ), m_pcParent ) );

	if( hDbx->type == DBX_TYPE_FOLDER )
		nError = ProcessFolder( hDbx );
	else if( hDbx->type == DBX_TYPE_EMAIL )
		nError = ProcessMail( hDbx, m_cFolder );
	else
		nError = EINVAL;

	dbx_close( hDbx );

	/* Close the progress dialog */
	m_pcDialog->Close();

	/* Let the application know that we're done */
	Message *pcMessage = new Message( M_IMPORT_COMPLETE );
	m_pcMessenger->SendMessage( pcMessage );

	return nError;
}

status_t DbxImporter::ProcessFolder( DBX *hDbx )
{
	status_t nError = EOK;
	DBXFOLDER *psFolder = NULL;

	for( int i = hDbx->indexCount-1; i >= 0; i-- )
	{
		dbx_free( hDbx, psFolder );
		psFolder=(DBXFOLDER*)dbx_get( hDbx, i, 0 );
    
		if( dbx_errno != DBX_NOERROR )
		{
			std::cerr << "failed to read folder" << std::endl;
			nError = EIO;
			break;
		}
    
		if( NULL == psFolder )
			continue;

		if( psFolder->fname != NULL )
		{
			String cPath = m_cPath + String( psFolder->fname );

			DBX *psChild = dbx_open( cPath.c_str() );
			if( NULL == psChild )
			{
				std::cerr << "failed to open child folder" << std::endl;
				continue;
			}

			if( psChild->type != DBX_TYPE_EMAIL )
			{
				std::cerr << "Folder " << psFolder->fname << " doesn't contain emails like I expect" << std::endl;
				continue;
			}

			/* Create sub-folder */
			String cChild = String( psFolder->name );
			String cTarget = m_cFolder + String( "/" ) + cChild;

			Message *pcMessage = new Message( M_IMPORT_CREATE_FOLDER );
			pcMessage->AddString( "parent", m_cFolder );
			pcMessage->AddString( "name", cChild );
			m_pcMessenger->SendMessage( pcMessage );

			/* Add all messages to the sub-folder */
			nError = ProcessMail( psChild, cTarget );
			dbx_close( psChild );
			if( m_bRun == false )
				break;
		}
	}

	return nError;
}

status_t DbxImporter::ProcessMail( DBX *hDbx, const String &cFolder )
{
	status_t nError = EOK;
	DBXEMAIL *psMessage = NULL;

	m_nCount = hDbx->indexCount;

	for( int i = hDbx->indexCount-1; i >= 0; i-- )
	{
		dbx_free( hDbx, psMessage );
		psMessage = (DBXEMAIL*)dbx_get( hDbx, i, DBX_FLAG_BODY );
		if( dbx_errno != DBX_NOERROR )
		{
			std::cerr << "failed to read message" << std::endl;
			nError = EIO;
			break;
		}

		if( psMessage && psMessage->email != NULL )
		{
			String cMessage = psMessage->email;
			nError = AddMessage( cMessage, cFolder );
			if( m_bRun == false )
				break;
		}
		else if( NULL == psMessage )
			printf("DBX returned a NULL email\n");
		else
			printf("Email has no body\n");

		m_nCount++;
	}

	return nError;
}

status_t DbxImporter::AddMessage( String &cMessage, const String &cFolder )
{
	if( cMessage != "" )
	{
		/* Add an RFC2822 terminating sequence */
		cMessage += "\r\n.\r\n";

		/* Create a Mailmessage and send it back down to the application along with the relevent data */
		Mailmessage *pcImport = new Mailmessage( cMessage.c_str(), cMessage.size() );

		Message *pcImportMessage = new Message( M_IMPORT_NEW );
		pcImportMessage->AddPointer( "message", pcImport );
		pcImportMessage->AddString( "folder", cFolder );

		m_pcMessenger->SendMessage( pcImportMessage );

		/* Update the progress dialog & check if the user canceled */
		//float vProgress = ( (double)m_nBytes / (double)m_nSize );
		float vProgress = ( (double)m_nTotal / (double)m_nCount );

		char zMessage[64] = { '\0' };
		snprintf( zMessage, 64, "Importing message #%Ld", m_nTotal++ );

		m_pcDialog->Lock();
		m_pcDialog->SetMessage( zMessage );
		m_pcDialog->SetProgress( vProgress );
		m_bRun = !m_pcDialog->IsCancelled();
		m_pcDialog->Unlock();
	}

	cMessage = "";
	return EOK;
}

extern "C"
{
	DbxNode * get_ix_node( void )
	{
		return new DbxNode();
	}
}

