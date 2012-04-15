#include "mainwindow.h"
#include "messages.h"
#include <util/event.h>
#include <gui/button.h>
#include <storage/path.h>

void MainWindow::GetEvents( os::Event* pcEvent, os::String zID, int nLevel )
{
	//printf( "Event %s\n", zID.c_str() );
	
	/* Create node */
	os::TreeViewStringNode* pcNode = new os::TreeViewStringNode();
	pcNode->SetIndent( nLevel );
	pcNode->AppendString( ( zID == "/" ) ? zID : os::Path( zID ).GetLeaf() );
	pcNode->AppendString( "" );
	pcNode->AppendString( "" );	
	pcNode->AppendString( "" );
	m_pcTree->InsertNode( pcNode, false );
	
	std::vector<os::String> cChildList;
	int nIndex = 0;
	while( pcEvent->SetToRemote( zID, nIndex ) == 0 )
	{
		proc_id hProcess;
		port_id hPort;
		os::String zDesc;
		pcEvent->GetRemoteInfo( &hProcess, &hPort, NULL, &zDesc );
		//printf( "Index %i %i %i %s\n", nIndex, hProcess, hPort, zDesc.c_str() );
		nIndex++;
		pcNode = new os::TreeViewStringNode();
		pcNode->SetIndent( nLevel + 1 );
		pcNode->AppendString( "Event" );
		char zTemp[20];
		sprintf( zTemp, "%i", hProcess );
		pcNode->AppendString( zTemp );
		sprintf( zTemp, "%i", hPort );
		pcNode->AppendString( zTemp );	
		pcNode->AppendString( zDesc );
		m_pcTree->InsertNode( pcNode, false );
	}
	pcEvent->Unset();
	pcEvent->SetToRemote( zID, -1 );
	
	if( pcEvent->GetRemoteChildren( &cChildList ) != 0 )
	{
		return;
	}
	for( uint i = 0; i < cChildList.size(); i++ )
	{
		GetEvents( pcEvent, zID + cChildList[i] + "/", nLevel + 1 );
	}
}

MainWindow::MainWindow() : os::Window( os::Rect( 0, 0, 600, 400 ), "main_wnd", "Event viewer" )
{
	/* Create gui */
	os::LayoutView* pcView = new os::LayoutView( GetBounds(), "layout", NULL, os::CF_FOLLOW_ALL );
	os::VLayoutNode* pcVNode = new os::VLayoutNode( "v_layout" );
	pcVNode->SetBorders( os::Rect( 5, 5, 5, 5 ) );
	
	m_pcTree = new os::TreeView( os::Rect(), "tree", os::ListView::F_NO_AUTO_SORT | os::ListView::F_RENDER_BORDER );
	m_pcTree->InsertColumn( "ID", (int)GetBounds().Width() * 3 / 8 );
	m_pcTree->InsertColumn( "Proc", (int)GetBounds().Width() / 8 );
	m_pcTree->InsertColumn( "Port", (int)GetBounds().Width() / 8 );	
	m_pcTree->InsertColumn( "Description", (int)GetBounds().Width() * 3 / 8 );		
	m_pcTree->SetDrawTrunk( true );
	m_pcTree->SetDrawExpanderBox( true );
	
	pcVNode->AddChild( m_pcTree );
	
	os::Button* pcButton = new os::Button( os::Rect(), "refresh", "Refresh", new os::Message( M_REFRESH ) );
	pcVNode->AddChild( new os::VLayoutSpacer( "", 5.0f, 5.0f ) );
	pcVNode->AddChild( pcButton, 0.0f );
	
	pcView->SetRoot( pcVNode );
	AddChild( pcView );
	
	
	/* Get event */
	os::Event* pcEvent = new os::Event();
	GetEvents( pcEvent, "/", 1 );
	delete( pcEvent );
	
	/* Set Icon */
	os::Resources cCol( get_image_id() );
	os::ResStream *pcStream = cCol.GetResourceStream( "icon48x48.png" );
	os::BitmapImage *pcIcon = new os::BitmapImage( pcStream );
	SetIcon( pcIcon->LockBitmap() );
	delete( pcIcon );
}

void MainWindow::HandleMessage( os::Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_REFRESH:
		{
			m_pcTree->Clear();
			/* Get event */
			os::Event* pcEvent = new os::Event();
			GetEvents( pcEvent, "/", 1 );
			delete( pcEvent );
			m_pcTree->Invalidate( true );
			m_pcTree->Flush();
		}
		break;
		case M_APP_QUIT:
		{
			PostMessage( os::M_QUIT );
		}
		break;
	}
}
bool MainWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return( true );
}










