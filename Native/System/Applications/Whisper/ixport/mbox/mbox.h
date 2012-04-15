#ifndef WHISPER_MBOX_IXPORT_H_
#define WHISPER_MBOX_IXPORT_H_

#include <gui/window.h>
#include <util/thread.h>
#include <util/messenger.h>

#include <ixport.h>
#include <dialogs.h>

class MboxNode : public IXNode
{
	public:
		MboxNode( void ){};
		~MboxNode( void ){};

		os::String GetIdentifier( void ){ return "UNIX mbox"; };
		uint64 GetCaps( void ){ return ( IMPORT | EXPORT ); };

		IXPlugin * GetPlugin( void );
};

class MboxPlugin : public IXPlugin
{
	public:
		MboxPlugin( void );
		~MboxPlugin( void );

		status_t CheckFile( const os::String cFilename );

		status_t Import( os::Window *pcParent, const os::String cFolder, const os::String cFilename );
		status_t Export( os::Window *pcParent, Mailfolder *pcFolder, const os::String cFilename );
};

class MboxImporter : public os::Thread
{
	public:
		MboxImporter( os::Window *pcParent, const os::String cFolder, const os::String cFilename );
		~MboxImporter( void );

		int32 Run();

	private:
		status_t AddMessage( os::String &cMessage );

		os::Window *m_pcParent;
		os::String m_cFolder;
		os::String m_cFilename;
		os::Messenger *m_pcMessenger;

		IXPortProgressDialog *m_pcDialog;

		off_t m_nSize;
		size_t m_nBytes;

		bool m_bRun;
		bool m_bDoCrLf;

		static uint64 m_nCount;
};

class MboxExporter : public os::Thread
{
	public:
		MboxExporter( os::Window *pcParent, Mailfolder *pcFolder, const os::String cFilename );
		~MboxExporter( void );

		int32 Run();

	private:
		os::Window *m_pcParent;
		Mailfolder *m_pcFolder;
		os::String m_cFilename;
};

#endif

