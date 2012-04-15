#include <util/application.h>
#include <gui/image.h>
#include <gui/requesters.h>
#include "gameview.h"
#include <iostream>

#include "win.h"
#include "ids.h"

using namespace os;

Win::Win( const Rect& cRect )
 : Window( cRect, "Win", "Icni" )
{
	Menu* pcMenu = _CreateMenuBar();

	pcMenu->SetFrame( Rect( 0, 0, GetBounds().right, pcMenu->GetPreferredSize(false).y ) );

	AddChild( pcMenu );

	Rect cFrame( 0, pcMenu->GetFrame().bottom, 499, pcMenu->GetFrame().bottom+499 );

	m_pcGameView = new GameView( cFrame, "m_pcGameView" );
	AddChild( m_pcGameView );
	AddTimer( this, 1, 20000, false );
	m_pcGameView->MakeFocus();

	cFrame.top = 0;
	Rect cWinFrame( GetFrame() );
	cWinFrame.right = cWinFrame.left + cFrame.right;
	cWinFrame.bottom = cWinFrame.top + cFrame.bottom;
	SetFrame( cWinFrame );
	
	m_nLevel = 1;
	UpdateLevelMenu();
}

// This method gets called every 20 ms.
void Win::TimerTick( int nID )
{
	m_pcGameView->TimerTick();
}

Win::~Win()
{
}
	
bool Win::OkToQuit()
{
    Application::GetInstance()->PostMessage( M_QUIT );
    return true;
}

void Win::HandleMessage( Message * pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case ID_APP_QUIT:
			{
				OkToQuit();
				break;
			}
		case ID_APP_SURRENDER:
			{
				m_pcGameView->Surrender();
				break;
			}
		case ID_LEVEL_CHANGE:
			{
				int32 i;
				void* p = NULL;
				CheckMenu* pcStateMenu;
				pcMessage->FindInt32( "level", &i );
				pcMessage->FindPointer( "source", &p );
				pcStateMenu = (CheckMenu*)p;
				m_nLevel = i;
				UpdateLevelMenu();
				m_pcGameView->Setup( m_nLevel );
				m_pcGameView->StartLevel( m_nLevel );
				break;
			}
		case ID_DONE:
			{
				if( m_pcGameView->Check() ) {
					switch( rand()%12 ) {
						case 0:		m_pcGameView->ShowHint( "\33cGood job!", 50*4 ); break;
						case 1:		m_pcGameView->ShowHint( "\33cSplendid!", 50*4 ); break;
						case 2:		m_pcGameView->ShowHint( "\33cImpressive!", 50*4 ); break;
						case 3:		m_pcGameView->ShowHint( "\33cWonderful!", 50*4 ); break;
						case 4:		m_pcGameView->ShowHint( "\33cYippie!", 50*4 ); break;
						case 5:		m_pcGameView->ShowHint( "\33cCool!", 50*4 ); break;
						case 6:		m_pcGameView->ShowHint( "\33cYo! You rock, man!", 50*4 ); break;
						case 7:		m_pcGameView->ShowHint( "\33cJolly good, old chap!", 50*4 ); break;
						case 8:		m_pcGameView->ShowHint( "\33cAll right, now try this one!", 50*4 ); break;
						case 9:		m_pcGameView->ShowHint( "\33cSo, was it too simple?!", 50*4 ); break;
						case 10:	m_pcGameView->ShowHint( "\33cGood!", 50*4 ); break;
						case 11:	m_pcGameView->ShowHint( "\33cGreat!", 50*4 ); break;
						default: m_pcGameView->ShowHint( "\33cOops! The programmer made a mistake!", 50*4 ); break;	// This one should never show... :)
					}
					m_nLevel++;
					m_pcGameView->Setup( m_nLevel );
				} else {
					Alert *pcAlert = new Alert( "Hey!",
						"Don't try to cheat! You have some crossing lines left!",
						Alert::ALERT_WARNING, 0,
						"D'oh!", NULL);
					pcAlert->CenterInWindow( this );
					pcAlert->Go( new Invoker( 0 ) );
				}
				break;
			}
		default:
			Window::HandleMessage( pcMessage );
			break;
	}
}

void Win::UpdateLevelMenu()
{
	for( int i = 0; i < 3; i++ ) {
		int32 nTemp = 0;
		Message* pcMsg = m_pcLevels[i]->GetMessage();
		pcMsg->FindInt32( "level", &nTemp );
		m_pcLevels[i]->SetChecked( nTemp == m_nLevel );
	}
}

Menu* Win::_CreateMenuBar()
{
	Message *pcMsg;

	Menu* pcMenuBar = new Menu( Rect(), "main_menu", ITEMS_IN_ROW, CF_FOLLOW_LEFT | CF_FOLLOW_RIGHT | CF_FOLLOW_TOP, WID_FULL_UPDATE_ON_H_RESIZE );

	Menu *pcFileMenu = new Menu( Rect( 0, 0, 1, 1 ), "Game", ITEMS_IN_COLUMN );

	pcFileMenu->AddItem( "I give up!", new Message( ID_APP_SURRENDER ), "" );
	pcFileMenu->AddItem(new MenuSeparator);
	pcFileMenu->AddItem( "Quit", new Message( ID_APP_QUIT ), "CTRL+Q" );

	Menu *pcLevelMenu = new Menu( Rect( 0, 0, 1, 1 ), "Level", ITEMS_IN_COLUMN );

	pcMsg = new Message( ID_LEVEL_CHANGE );
	pcMsg->AddInt32( "level", 1 );
	pcLevelMenu->AddItem( ( m_pcLevels[0] = new CheckMenu( "Ridiculously easy", pcMsg ) ) );

	pcMsg = new Message( ID_LEVEL_CHANGE );
	pcMsg->AddInt32( "level", 10 );
	pcLevelMenu->AddItem( ( m_pcLevels[1] = new CheckMenu( "Slightly challenging", pcMsg ) ) );

	pcMsg = new Message( ID_LEVEL_CHANGE );
	pcMsg->AddInt32( "level", 20 );
	pcLevelMenu->AddItem( ( m_pcLevels[2] = new CheckMenu( "Irritatingly impossible", pcMsg ) ) );

	pcMenuBar->AddItem( pcFileMenu );
	pcMenuBar->AddItem( pcLevelMenu );

	pcMenuBar->SetTargetForItems( this );

	return pcMenuBar;
}
