#include "mainwindow.h"
#include "messages.h"


#include <gui/button.h>
#include <gui/layoutview.h>
#include <gui/stringview.h>
#include <gui/textview.h>

MainWindow::MainWindow() : os::Window( os::Rect( 0, 0, 350, 300 ), "main_wnd", "Random Test App",WND_NOT_RESIZABLE )
{
	/* Set Icon */
	os::Resources cCol( get_image_id() );
	os::ResStream *pcStream = cCol.GetResourceStream( "icon48x48.png" );
	os::BitmapImage *pcIcon = new os::BitmapImage( pcStream );
	SetIcon( pcIcon->LockBitmap() );
	delete( pcIcon );
	
	os::LayoutView* pcView = new os::LayoutView(GetBounds(),"layout",NULL);
	os::HLayoutNode* pcRoot = new os::HLayoutNode("root");
	
	os::VLayoutNode* pcViewNode = new os::VLayoutNode("rand_view");
	m_pcRandView = new RandView();
	m_pcRandView->SetRandomBackColor(true);
	m_pcRandView->SetRandomFont(true);
	m_pcRandView->SetRandomFontColor(true);
	pcViewNode->AddChild(m_pcRandView);
	

	os::VLayoutNode* pcConfigNode = new os::VLayoutNode("");
	
	os::HLayoutNode* pcIntNode = new os::HLayoutNode("int_node");
	pcIntNode->AddChild(rand_int = new os::StringView(os::Rect(),"random_int","Random Integer:"));
	pcIntNode->AddChild(new os::HLayoutSpacer("",10,10));
	pcIntNode->AddChild(rand_int_text = new os::StringView(os::Rect(),"int",os::String().Format("%d",cRandom.AsInt())));
	pcIntNode->AddChild(new os::HLayoutSpacer("",2,2));
	pcConfigNode->AddChild(pcIntNode);
	
	
	os::HLayoutNode* pcFltNode = new os::HLayoutNode("flt_node");
	pcFltNode->AddChild(rand_float = new os::StringView(os::Rect(),"random_float","Random Float:"));
	pcFltNode->AddChild(new os::HLayoutSpacer("",10,10));
	pcFltNode->AddChild(rand_float_text = new os::StringView(os::Rect(),"float",os::String().Format("%f",cRandom.AsFloat(0.00,2000.00))));
	pcFltNode->AddChild(new os::HLayoutSpacer("",2,2));
	pcConfigNode->AddChild(pcFltNode);
	
	pcConfigNode->SameWidth("random_int","random_float",NULL);
	
	pcConfigNode->AddChild(new os::Button(Rect(),"refresh","Refresh",new os::Message(M_REFRESH)));
	
	
	pcRoot->AddChild(pcViewNode);
	pcRoot->AddChild(new HLayoutSpacer("",2,5));
	pcRoot->AddChild(pcConfigNode);
	
	
	pcView->SetRoot(pcRoot);
	pcView->ResizeTo(pcView->GetPreferredSize(false));
	ResizeTo(pcView->GetPreferredSize(false));
	
	AddChild(pcView);
}

void MainWindow::HandleMessage( os::Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_APP_QUIT:
		{
			PostMessage( os::M_QUIT );
			break;
		}
		
		case M_REFRESH:
		{
			m_pcRandView->Refresh();
			rand_float_text->SetString(os::String().Format("%f",cRandom.AsFloat(0.00,2000.00)));
			rand_int_text->SetString(os::String().Format("%d",cRandom.AsInt()));
			break;
		}
	}
}
bool MainWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return( true );
}











