#ifndef WHISPER_PST_IXPORT_H_
#define WHISPER_PST_IXPORT_H_

#include <gui/window.h>
#include <util/thread.h>
#include <util/messenger.h>

#include <ixport.h>
#include <dialogs.h>
#include <libpst.h>

class PstNode : public IXNode
{
	public:
		PstNode( void ){};
		~PstNode( void ){};

		os::String GetIdentifier( void ){ return "Microsoft Outlook PST"; };
		uint64 GetCaps( void ){ return ( IMPORT ); };

		IXPlugin * GetPlugin( void );
};

class PstPlugin : public IXPlugin
{
	public:
		PstPlugin( void );
		~PstPlugin( void );

		status_t CheckFile( const os::String cFilename );

		status_t Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename );
};

#define C_TIME_SIZE 500

class PstImporter : public os::Thread
{
	public:
		PstImporter( os::Window *pcParent, const os::String cFolder, const os::String cFilename );
		~PstImporter( void );

		int32 Run();

	private:
		status_t ProcessFolder( pst_file *psPstfile, pst_desc_ll *d_ptr, int32 nItemCount, os::String cFolder );
		status_t ProcessMessage( pst_file *psPstfile, pst_item *item, os::String cFolder );
		status_t AddMessage( os::String &cMessage, os::String cFolder );

		char * pst_stristr( char *haystack, char *needle );
		char * removeCR( char *c );
		char * skip_header_prologue( char *headers );

		os::Window *m_pcParent;
		os::String m_cFolder;
		os::String m_cFilename;
		os::Messenger *m_pcMessenger;

		IXPortProgressDialog *m_pcDialog;

		bool m_bRun;

		static uint64 m_nCount;
		off_t m_nSize;
		size_t m_nBytes;

		static int32 skip_count;
		static int32 email_count;
};

#endif

