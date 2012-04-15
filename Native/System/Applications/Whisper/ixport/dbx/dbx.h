#ifndef WHISPER_DBX_IXPORT_H_
#define WHISPER_DBX_IXPORT_H_

#include <gui/window.h>
#include <util/thread.h>
#include <util/messenger.h>

#include <ixport.h>
#include <dialogs.h>
#include <libdbx.h>

class DbxNode : public IXNode
{
	public:
		DbxNode( void ){};
		~DbxNode( void ){};

		os::String GetIdentifier( void ){ return "Microsoft Outlook Express DBX"; };
		uint64 GetCaps( void ){ return ( IMPORT ); };

		IXPlugin * GetPlugin( void );
};

class DbxPlugin : public IXPlugin
{
	public:
		DbxPlugin( void );
		~DbxPlugin( void );

		status_t CheckFile( const os::String cFilename );

		status_t Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename );
};

class DbxImporter : public os::Thread
{
	public:
		DbxImporter( os::Window *pcParent, const os::String cFolder, const os::String cFilename );
		~DbxImporter( void );

		int32 Run();

	private:
		status_t ProcessFolder( DBX *hDbx );
		status_t ProcessMail( DBX *hDbx, const os::String &cFolder );
		status_t AddMessage( os::String &cMessage, const os::String &cFolder );

		os::Window *m_pcParent;
		os::String m_cFolder;
		os::String m_cFilename;
		os::String m_cPath;
		os::Messenger *m_pcMessenger;

		IXPortProgressDialog *m_pcDialog;

		bool m_bRun;

		uint64 m_nIndex;
		uint64 m_nCount;
		static uint64 m_nTotal;
};

#endif

