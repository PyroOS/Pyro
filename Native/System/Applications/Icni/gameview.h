#ifndef __GAMEVIEW_H__
#define __GAMEVIEW_H__

#include <gui/button.h>
#include <gui/window.h>
#include <gui/bitmap.h>
#include <gui/font.h>

#include <gui/image.h>

#include "node.h"

#define MAX_NODES	40

class GameView : public ImageView
{
	public:
	GameView( Rect cFrame, const String& cName );
	~GameView();

    virtual void	MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData );
    virtual void	MouseDown( const Point& cPosition, uint32 nButtons );
    virtual void	MouseUp( const Point& cPosition, uint32 nButtons, Message* pcData );

	void TimerTick();

	void RenderFrame( View* pcView );

	void DrawConnection( View* pcView, int from, int to );
	void DrawNode( View* pcView, int node );
	
	void Setup( int nLevel );
	void ShowHint( const String& cText, int nTime );
	void Surrender();
	bool Check();
	void StartLevel( int nLevel );

	time_t GetTime() const { return time(NULL) - m_tStartTime; }
	int GetMoves() const { return m_nTotalMoves; }

	private:
	BitmapImage*	m_pcImage;
	GNode			m_caNodes[ MAX_NODES ];
	int				m_nNumNodes;
	bool			m_bNeedRefresh;
	Point			m_cMousePos;
	GNode*			m_pcTracking;

	BitmapImage*	m_pcBackground;
	BitmapImage*	m_pcNodeIcon;
	BitmapImage*	m_pcSelectedNodeIcon;
	
	int				m_nAnimate;
	int				m_nLevel;
	
	Rect			m_cLevelTextRect;
	Rect			m_cButtonRect;
	
	bool			m_bShowHint;
	String			m_cHintText;
	int				m_nHintCounter;
	int				m_nGiveUp;

	int				m_nTotalMoves;
	time_t			m_tStartTime;
	int				m_nStartLevel;
	bool			m_bSurrendered;
};

#endif /* __GAMEVIEW_H__ */
