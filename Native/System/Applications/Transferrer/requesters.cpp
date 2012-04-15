/* This file contains various GUI dialogs and requester windows. */
#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/stringview.h>
#include <gui/textview.h>
#include <gui/button.h>
#include <util/message.h>

#include "requesters.h"
#include "messages.h"

/* TODO:
	- more options in overwrite dialog
*/


/**** Delete confirmation dialog *****/

DeleteConfirmDialog::DeleteConfirmDialog( std::vector< RemoteNode >* pacNodes, Handler* pcTarget )
	: Window( Rect(0,0,300,60), "delete_dialog", "Delete files" )
{
	m_pcTarget = pcTarget;
	m_pacNodes = pacNodes;
	
	HLayoutNode* pcRoot = new HLayoutNode( "root" );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer1", 3.0f, 3.0f ) );

	VLayoutNode* pcLayout = new VLayoutNode( "vnode" );
	String zTmp;
	if( pacNodes->size() == 1 ) {
		zTmp = "Are you sure you want to delete this file?";
	} else {
		zTmp.Format( "Are you sure you want to delete these %i files?", pacNodes->size() );
	}
	StringView* pcStringView = new StringView( Rect(), "stringview", zTmp );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer1", 3.0f ) );
	pcLayout->AddChild( pcStringView );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer2", 3.0f ) );
	HLayoutNode* pcButtonsNode = new HLayoutNode( "hnode" );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer2" ) );
	Button* pcYes = new Button( Rect(), "yesbutton", "_Yes", new Message( M_DELETE_CONFIRMED ) );
	pcYes->SetTarget( this );
	pcButtonsNode->AddChild( pcYes, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer3" ) );
	Button* pcNo = new Button( Rect(), "nobutton", "_No", new Message( M_QUIT ) );
	pcNo->SetTarget( this );
	pcButtonsNode->AddChild( pcNo, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer4" ) );
	pcLayout->AddChild( pcButtonsNode );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer3", 3.0f ) );
	pcRoot->AddChild( pcLayout );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer2", 3.0f, 3.0f ) );	
	
	LayoutView* pcView = new LayoutView( GetFrame(), "layoutview", pcRoot );
	AddChild( pcView );
	
	pcYes->SetTabOrder();
	pcNo->SetTabOrder();
	SetDefaultButton( pcYes );
	pcNo->SetShortcut( ShortcutKey( 27 ) );	/* Escape */
	
	ResizeTo( pcView->GetPreferredSize( false ) );
}

DeleteConfirmDialog::~DeleteConfirmDialog()
{
	if( m_pacNodes ) delete( m_pacNodes );
}

void DeleteConfirmDialog::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_DELETE_CONFIRMED:
		{
			pcMessage->AddPointer( "files", m_pacNodes );
			Messenger cMessenger( m_pcTarget );
			cMessenger.SendMessage( pcMessage );
			m_pacNodes = NULL;	/* We don't want to delete it */
			Quit();
			break;
		}
		default:
			Window::HandleMessage( pcMessage );
	}
}

/**** Rename dialog ****/

RenameRequester::RenameRequester( const String& zOldPath, Handler* pcTarget )
	: Window( Rect(0,0,300,60), "rename_dialog", "Rename file" )
{
	m_pcTarget = pcTarget;
	m_zOldPath = zOldPath;
	String zFilename;
	size_t nTmp = m_zOldPath.str().find_last_of( '/' );
	if( nTmp == std::string::npos ) {
		/* Path does not contain '/' */
		m_zDirectory = "";
		zFilename = zOldPath;
	} else {
		m_zDirectory = zOldPath.substr( 0, nTmp + 1 );	/* Up to and including the last '/' */
		zFilename = zOldPath.substr( nTmp + 1 );	/* Everything after the last '/' */
	}
	
	HLayoutNode* pcRoot = new HLayoutNode( "root" );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer1", 3.0f, 3.0f ) );

	VLayoutNode* pcLayout = new VLayoutNode( "vnode" );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer1", 3.0f ) );
	StringView* pcStringView = new StringView( Rect(), "stringview", "Enter a new name for the file:" );
	pcLayout->AddChild( pcStringView );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer2", 3.0f ) );
	
	m_pcTextView = new TextView( Rect(), "rename_input", zFilename.c_str() );
	m_pcTextView->SetMultiLine( false );
	m_pcTextView->SelectAll( false );
	pcLayout->AddChild( m_pcTextView );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer3", 3.0f ) );
	
	HLayoutNode* pcButtonsNode = new HLayoutNode( "hnode" );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer2" ) );
	Button* pcOK = new Button( Rect(), "okbutton", "Rename", new Message( M_RENAME_CONFIRMED ) );
	pcOK->SetTarget( this );
	pcButtonsNode->AddChild( pcOK, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer3" ) );
	Button* pcCancel = new Button( Rect(), "cancelbutton", "Cancel", new Message( M_QUIT ) );
	pcCancel->SetTarget( this );
	pcButtonsNode->AddChild( pcCancel, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer4" ) );
	pcLayout->AddChild( pcButtonsNode );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer4", 3.0f ) );
	pcRoot->AddChild( pcLayout );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer1", 3.0f, 3.0f ) );

	
	LayoutView* pcView = new LayoutView( GetFrame(), "layoutview", pcRoot );
	AddChild( pcView );
	
	m_pcTextView->SetTabOrder(NEXT_TAB_ORDER);
	m_pcTextView->MakeFocus( true );
	pcOK->SetTabOrder();
	pcCancel->SetTabOrder();
	pcCancel->SetShortcut( ShortcutKey( 27 ) );	/* Escape */
	SetDefaultButton( pcOK );

	ResizeTo( pcView->GetPreferredSize( false ) );
}

RenameRequester::~RenameRequester()
{
}

void RenameRequester::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_RENAME_CONFIRMED:
		{
			pcMessage->AddString( "old_path", m_zOldPath );
			String zNewPath = m_zDirectory;
			zNewPath += m_pcTextView->GetValue().AsString();
			pcMessage->AddString( "new_path", zNewPath );
			Messenger cMessenger( m_pcTarget );
			cMessenger.SendMessage( pcMessage );
			Quit();
			break;
		}
		default:
			Window::HandleMessage( pcMessage );
	}
}


/**** MkDir dialog ****/

MkDirRequester::MkDirRequester( const String& zBasePath, Handler* pcTarget )
	: Window( Rect(0,0,300,60), "mkdir_dialog", "Create new directory" )
{
	m_pcTarget = pcTarget;
	m_zBasePath = zBasePath;

	HLayoutNode* pcRoot = new HLayoutNode( "root" );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer1", 3.0f, 3.0f ) );
	
	VLayoutNode* pcLayout = new VLayoutNode( "vnode" );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer1", 3.0f ) );
	StringView* pcStringView = new StringView( Rect(), "stringview", "Enter a name for the new directory:" );
	pcLayout->AddChild( pcStringView );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer2", 3.0f ) );
	
	m_pcTextView = new TextView( Rect(), "mkdir_input", "New directory" );
	m_pcTextView->SetMultiLine( false );
	m_pcTextView->SelectAll( false );
	pcLayout->AddChild( m_pcTextView );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer3", 3.0f ) );
	
	HLayoutNode* pcButtonsNode = new HLayoutNode( "hnode" );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer2" ) );
	Button* pcOK = new Button( Rect(), "okbutton", "Create", new Message( M_MKDIR_CONFIRMED ) );
	pcOK->SetTarget( this );
	pcButtonsNode->AddChild( pcOK, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer3" ) );
	Button* pcCancel = new Button( Rect(), "cancelbutton", "Cancel", new Message( M_QUIT ) );
	pcCancel->SetTarget( this );
	pcButtonsNode->AddChild( pcCancel, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer4" ) );

	pcLayout->AddChild( pcButtonsNode );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer4", 3.0f ) );

	pcRoot->AddChild( pcLayout );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer2", 3.0f, 3.0f ) );

	
	LayoutView* pcView = new LayoutView( GetFrame(), "layoutview", pcRoot );
	AddChild( pcView );
	
	m_pcTextView->SetTabOrder(NEXT_TAB_ORDER);
	m_pcTextView->MakeFocus( true );
	pcOK->SetTabOrder();
	pcCancel->SetTabOrder();
	SetDefaultButton( pcOK );
	pcCancel->SetShortcut( ShortcutKey( 27 ) );	/* Escape */

	ResizeTo( pcView->GetPreferredSize( false ) );
}

MkDirRequester::~MkDirRequester()
{
}

void MkDirRequester::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_MKDIR_CONFIRMED:
		{
			String zTmp = m_zBasePath;
			zTmp += "/";
			zTmp += m_pcTextView->GetValue().AsString();
			pcMessage->AddString( "remotepath", zTmp );
			Messenger cMessenger( m_pcTarget );
			cMessenger.SendMessage( pcMessage );
			Quit();
			break;
		}
		default:
			Window::HandleMessage( pcMessage );
	}
}

/**** Overwrite dialog ****/
OverwriteRequester::OverwriteRequester( const String& zPath, int nJobID, bool bMultiJob, Handler* pcTarget )
	: Window( Rect( 0,0,300,60 ), "overwrite_dialog", "Overwrite file?" )
{
	m_nJobID = nJobID;
	m_pcTarget = pcTarget;

	HLayoutNode* pcRoot = new HLayoutNode( "root" );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer1", 3.0f, 3.0f ) );

	VLayoutNode* pcLayout = new VLayoutNode( "vnode" );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer1", 3.0f ) );
	String zMessage;
	zMessage.Format( "The file %s already exists.\nDo you want to overwrite it?", zPath.c_str() );
	StringView* pcStringView = new StringView( Rect(), "stringview", zMessage );
	pcLayout->AddChild( pcStringView );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer2", 3.0f ) );

	if( bMultiJob ) {
		m_pcCheckbox = new CheckBox( Rect(), "checkbox", "Do this for all remaining files", NULL );
		pcLayout->AddChild( m_pcCheckbox );
		pcLayout->AddChild( new VLayoutSpacer( "vspacer3", 3.0f ) );
	} else {
		m_pcCheckbox = NULL;
	}
	
	HLayoutNode* pcButtonsNode = new HLayoutNode( "hnode" );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer2" ) );
	Button* pcYes = new Button( Rect(), "yesbutton", "_Overwrite", new Message( M_YES ) );
	pcYes->SetTarget( this );
	pcButtonsNode->AddChild( pcYes, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer2" ) );
	Button* pcNo = new Button( Rect(), "skipbutton", "_Skip this file", new Message( M_NO ) );
	pcNo->SetTarget( this );
	pcButtonsNode->AddChild( pcNo, 2.0f );
	pcButtonsNode->AddChild( new HLayoutSpacer( "spacer3" ) );
	pcLayout->AddChild( pcButtonsNode );
	pcLayout->AddChild( new VLayoutSpacer( "vspacer4", 3.0f ) );

	pcRoot->AddChild( pcLayout );
	pcRoot->AddChild( new HLayoutSpacer( "rootspacer2", 3.0f, 3.0f ) );

	LayoutView* pcView = new LayoutView( GetFrame(), "layoutview", pcRoot );
	AddChild( pcView );

	SetDefaultButton( pcYes );
	pcYes->SetTabOrder();
	pcNo->SetTabOrder();
	pcNo->SetShortcut( ShortcutKey( 27 ) );	/* Escape */

	ResizeTo( pcView->GetPreferredSize( false ) );
}

OverwriteRequester::~OverwriteRequester()
{
}

bool OverwriteRequester::OkToQuit()
{
	/* User has pressed 'Skip' or cancelled the window. Send cancel message. */
	Message cMsg( M_GUI_OVERWRITE_REPLY );
	cMsg.AddInt32( "id", m_nJobID );
	cMsg.AddInt32( "userResponse", RESPONSE_NO );
	if( m_pcCheckbox ) cMsg.AddBool( "persistent", m_pcCheckbox->GetValue().AsBool() );
	Messenger cMessenger( m_pcTarget );
	cMessenger.SendMessage( &cMsg );
	
	return( true );
}

void OverwriteRequester::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_YES:
		{
			Message cMsg( M_GUI_OVERWRITE_REPLY );
			cMsg.AddInt32( "id", m_nJobID );
			cMsg.AddInt32( "userResponse", RESPONSE_YES );
			if( m_pcCheckbox ) cMsg.AddBool( "persistent", m_pcCheckbox->GetValue().AsBool() );
			Messenger cMessenger( m_pcTarget );
			cMessenger.SendMessage( &cMsg );
			Quit();
			break;
		}
		case M_NO:
		{
			OkToQuit();
			Quit();
			break;
		}
	}
}
