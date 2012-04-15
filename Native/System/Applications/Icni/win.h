#ifndef __TESTWIN_H__
#define __TESTWIN_H__

#include <gui/window.h>
#include <gui/imageview.h>
#include <gui/checkmenu.h>
#include <util/message.h>
#include "node.h"
#include "gameview.h"

using namespace os;

class Win : public os::Window
{
public:
    Win( const os::Rect& cRect );
    ~Win();

	void TimerTick( int nID );

	bool OkToQuit();

	void 	HandleMessage( Message * pcMessage );

	void	UpdateLevelMenu();

private:
	Menu* _CreateMenuBar();
	
	GameView*		m_pcGameView;
	CheckMenu*		m_pcLevels[3];
	int				m_nLevel;
};

#endif // __TESTTWIN_H__
