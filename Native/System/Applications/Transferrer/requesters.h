#ifndef REQUESTERS_H
#define REQUESTERS_H

#include <gui/window.h>
#include <gui/textview.h>
#include <gui/checkbox.h>

#include <vector>

#include "remotenode.h"

#ifndef DEBUG
extern bool g_bDebug;
#define DEBUG( arg... ) do { if( g_bDebug ) printf( arg ); } while( 0 )
#endif

using namespace os;

class RenameRequester : public Window
{
public:
	RenameRequester( const String& zOldPath, Handler* pcTarget );
	~RenameRequester();
	
	void HandleMessage( Message* pcMessage );
private:
	Handler* m_pcTarget;
	String m_zOldPath;
	String m_zDirectory;
	
	os::TextView* m_pcTextView;
};

class DeleteConfirmDialog : public Window
{
public:
	DeleteConfirmDialog( std::vector< RemoteNode >* pacNodes, Handler* pcTarget );	
	~DeleteConfirmDialog();

	void HandleMessage( Message* pcMessage );
private:
	Handler* m_pcTarget;
	std::vector< RemoteNode >* m_pacNodes;
};

class MkDirRequester : public Window
{
public:
	MkDirRequester( const String& zBasePath, Handler* pcTarget );
	~MkDirRequester();

	void HandleMessage( Message* pcMessage );
private:
	Handler* m_pcTarget;
	String m_zBasePath;
	
	os::TextView* m_pcTextView;
};

class OverwriteRequester : public Window
{
public:
	OverwriteRequester( const String& zPath, int nJobID, bool bSoloJob, Handler* pcTarget );
	~OverwriteRequester();
	
	void HandleMessage( Message* pcMessage );
	bool OkToQuit();
private:
	int m_nJobID;
	os::CheckBox* m_pcCheckbox;
	Handler* m_pcTarget;
};

#endif	/* REQUESTERS_H */

