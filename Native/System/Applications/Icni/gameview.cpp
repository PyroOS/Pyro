#include "gameview.h"
#include "ids.h"
#include <util/locale.h>

using namespace os;

GameView::GameView( Rect cFrame, const String& cName )
:ImageView( cFrame, cName, NULL, ImageView::NORMAL, CF_FOLLOW_NONE )
{
	m_nAnimate = 0;
	m_pcBackground = new BitmapImage();
	m_pcNodeIcon = new BitmapImage();
	m_pcSelectedNodeIcon = new BitmapImage();
	m_pcImage = new BitmapImage();

	m_pcImage->ResizeCanvas( Point( GetBounds().Width() + 1, GetBounds().Height() + 1 ) );
	SetImage( m_pcImage );

	View* pcView = m_pcImage->GetView();

    os::font_properties fp( "LCD2", "Normal", 0, 20.0f, 0.0f, 0.0f );
    fp.m_nFlags |= FPF_SMOOTHED;
	Font* pcFont = new Font(fp);
	pcFont->SetSize( 15 );
	pcView->SetFont( pcFont );
	pcFont->Release();

	Point cLevelSize = pcView->GetTextExtent( "LEVEL: 000" );
	m_cLevelTextRect.left = 20;
	m_cLevelTextRect.top = 460;
	m_cLevelTextRect.right = m_cLevelTextRect.left + cLevelSize.x;
	m_cLevelTextRect.bottom = m_cLevelTextRect.top + cLevelSize.y;
	m_cButtonRect.right = 499 - m_cLevelTextRect.left;
	m_cButtonRect.left = m_cButtonRect.right - cLevelSize.x;
	m_cButtonRect.top = m_cLevelTextRect.top;
	m_cButtonRect.bottom = m_cLevelTextRect.top + cLevelSize.y;

	Locale l;
	StreamableIO* pcBackground = l.GetLocalizedResourceStream( "bluepatchwork.png" );
	m_pcBackground->Load(pcBackground);
	delete pcBackground;

	StreamableIO* pcNodeIcon = l.GetLocalizedResourceStream( "normalnode.png" );
	m_pcNodeIcon->Load(pcNodeIcon);
	delete pcNodeIcon;

	StreamableIO* pcSelectedNodeIcon = l.GetLocalizedResourceStream( "selectednode.png" );
	m_pcSelectedNodeIcon->Load(pcSelectedNodeIcon);
	delete pcSelectedNodeIcon;

	Setup( 1 );
	m_nLevel = 1;
	m_bShowHint = false;
	m_nGiveUp = 0;
	m_nTotalMoves = 0;
	m_tStartTime = 0;
}

void GameView::StartLevel( int nLevel )
{
	m_nTotalMoves = 0;
	m_tStartTime = 0;
	m_nStartLevel = nLevel;
	m_bSurrendered = false;
}

void GameView::Setup( int nLevel )
{
	m_nLevel = nLevel;
	m_nGiveUp = 0;
	m_nNumNodes = nLevel * 2 + 4;
	if( m_nNumNodes > MAX_NODES )
		m_nNumNodes = MAX_NODES;
 
	for( int i = 0; i < m_nNumNodes; i++ ) {
		m_caNodes[ i ].ConnectedNodes.clear();
	}

	Rect cBounds( GetBounds() );

	int perrow = (int)sqrt( m_nNumNodes ) + 1;
	Point cDelta;
	cDelta.x = cBounds.Width() / ( perrow + 1 );
	cDelta.y = cBounds.Height() / ( perrow );
	int x = 0, y = 0;
	for( int i = 0; i < m_nNumNodes; i++ ) {
		int right, down, diagonal;
		right = x + 1;
		if( right >= perrow ) right = -1;
		else right += y * perrow;
		if( right >= m_nNumNodes) right = -1;
		down = x + ( y + 1 ) * perrow;
		if( down >= m_nNumNodes ) down = -1;
		if( x + 1 < perrow ) {
			diagonal = x + 1 + ( y - 1 ) * perrow;
			if( diagonal < 0 ) diagonal = -1;
		} else {
			diagonal = -1;
		}
		if( right != -1 ) {
			m_caNodes[ i ].ConnectedNodes.push_back( right );
			m_caNodes[ right ].ConnectedNodes.push_back( i );
		}
		if( down != -1 ) {
			m_caNodes[ i ].ConnectedNodes.push_back( down );
			m_caNodes[ down ].ConnectedNodes.push_back( i );
		}
		if( diagonal != -1 ) {
			m_caNodes[ i ].ConnectedNodes.push_back( diagonal );
			m_caNodes[ diagonal ].ConnectedNodes.push_back( i );
		}

		m_caNodes[ i ].SetSolution( Point( cDelta.x * ( x + 1 ) + ( rand() % (int)( 0.75 * cDelta.x ) ) - cDelta.x * 0.5 * 0.75,
			cDelta.y * ( y + 1 ) + ( rand() % (int)( 0.75 * cDelta.y ) ) - cDelta.y * 0.5 * 0.75 ) );
		m_caNodes[ i ].SetPosition( Point( rand() % (int)cBounds.Width(), rand() % (int)cBounds.Height() ) );
		x++;
		if( x == perrow ) {
			x = 0;
			y++;
		}
	}
	
	m_bNeedRefresh = true;
	m_pcTracking = NULL;
}

GameView::~GameView()
{
	delete m_pcImage;
	delete m_pcBackground;
	delete m_pcNodeIcon;
	delete m_pcSelectedNodeIcon;
}

void GameView::MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData )
{
	if( m_pcTracking ) {
		m_pcTracking->MouseMove( cNewPos );
		m_bNeedRefresh = true;
	}
}

void GameView::MouseDown( const Point& cPosition, uint32 nButtons )
{
	if( nButtons == 1 ) {
		for( int i = 0; i < m_nNumNodes; i++ ) {
			if( m_caNodes[i].HitTest( cPosition ) ) {
				m_pcTracking = &m_caNodes[i];
				m_pcTracking->MouseDown();
				m_bNeedRefresh = true;
				return;
			}
		}
		if( m_cButtonRect.DoIntersect( cPosition ) ) {
//			GetWindow()->PostMessage( ID_DONE );
			Message* m = new Message( ID_DONE );
			GetWindow()->HandleMessage( m );
			delete m;
		}
		if( m_tStartTime == 0 ) {
			m_tStartTime = time( NULL );
		}
	}
}

void GameView::MouseUp( const Point& cPosition, uint32 nButtons, Message* pcData )
{
	if( m_pcTracking ) {
		m_pcTracking->MouseUp( cPosition );
		m_pcTracking = NULL;
		m_bNeedRefresh = true;

		if( m_nLevel == 1 && m_bShowHint == false ) {
			if( Check() ) {
				ShowHint( "\33rClick here when you're done >>>", 50*30 );
			}
		}
		
		if( m_nGiveUp++ == 30 ) {
			// tease the user a bit... :)
			ShowHint( "\33cIt's never too late to give up!", 50*15 );
		}
		
		m_nTotalMoves++;
	}
}

void GameView::TimerTick()
{
	static bool bDisplayBuffer = false;

	if( bDisplayBuffer ) {
		bDisplayBuffer = false;
		// 1. In case the last frame hasn't finished rendering yet, we'll have to
		//    extend the 20 ms frame delay until it is finished. 
		m_pcImage->Sync();
		// 2. Now we can display the background buffer. We must sync afterwards to make
		//    sure that we don't start changing the background buffer before it has been
		//    copied to the graphics card's frame buffer.
		Refresh();
		Sync();
	}
	
	if( m_bNeedRefresh ) {
		m_bNeedRefresh = false;		// RenderFrame() may trigger a new refresh, if animating

		// Time to render the next frame. The Flush() is asynchronous, so we can go on
		// doing other things while the AppServer is busy compositing our new frame.
		RenderFrame( m_pcImage->GetView() );
		m_pcImage->Flush();
		bDisplayBuffer = true;
	}
	
	if( m_nHintCounter ) {
		m_nHintCounter--;
	} else {
		m_bShowHint = false;
	}
}

void GameView::Surrender()
{
	if( GetMoves() < 10 ) {
		ShowHint( "You can't give up already!!", 250 );
	} else {
		for( int i = 0; i < m_nNumNodes; i++ ) {
			m_caNodes[ i ].InitializeAnimation( 150 );
		}
	
		m_nAnimate = 150;
		m_bNeedRefresh = true;
		m_bSurrendered = true;
		
		ShowHint( "Look! Wasn't it simple?!", 250 );
	}
}

void GameView::RenderFrame( View* pcView )
{
	if( m_nAnimate ) {
		m_nAnimate--;
		for( int i = 0; i < m_nNumNodes; i++ ) {
			m_caNodes[ i ].Animate();
		}
		m_bNeedRefresh = true;
	}

	pcView->SetDrawingMode( DM_COPY );

	m_pcBackground->Draw( Point( 0, 0 ), pcView );

	for( int i = 0; i < m_nNumNodes; i++ ) {
		std::vector<int>::iterator ci;
		for( ci = m_caNodes[ i ].ConnectedNodes.begin(); ci != m_caNodes[ i ].ConnectedNodes.end(); ci++ ) {
			if( *(ci) > i ) {
				DrawConnection( pcView, i, *ci );
			}
		}
	}

	for( int i = 0; i < m_nNumNodes; i++ ) {
		DrawNode( pcView, i );
	}

	pcView->SetDrawingMode( DM_OVER );	
	pcView->SetFgColor( 0xFFFF0000 );
	pcView->SetBgColor( 0xFF000000 );

	String sDone( "\33rDONE!" );

	if( !m_bSurrendered ) {
		String sLevel = String().Format( "LEVEL: %d", m_nLevel );
		pcView->DrawText( m_cLevelTextRect, sLevel );

	/*	pcView->MovePenTo( m_cButtonRect.LeftTop() );
		pcView->DrawLine( m_cButtonRect.RightBottom() );*/
		pcView->DrawText( m_cButtonRect, sDone );
	}

	if( m_bShowHint ) {
		if( ( m_nHintCounter % 25 ) > 10 ) {
			pcView->SetFgColor( 0xFFFFFF00 );
			if( m_cHintText[1] == 'r' ) {
				// If the text is right-aligned, we need to move it really close to the right edge...
				Point cSize = pcView->GetTextExtent( sDone );
				pcView->DrawText( Rect( m_cLevelTextRect.right+20, m_cLevelTextRect.top, m_cButtonRect.right-cSize.x-10, m_cLevelTextRect.bottom ), m_cHintText );
			} else {
				pcView->DrawText( Rect( m_cLevelTextRect.right+20, m_cLevelTextRect.top, m_cButtonRect.left-20, m_cLevelTextRect.bottom ), m_cHintText );
			}
		}
		m_bNeedRefresh = true;
	} else {
		if( m_bSurrendered ) {
			int score = m_nLevel*m_nLevel*1000 - GetTime() * 50 - GetMoves() * 150;
			String sSummary = String().Format( "\33cC O N G R A T U A L T I O N S !\n\n"
				"You made it all the way to level: %d\n\n"
				"It only took you %d seconds and %d moves.\n\n\nSCORE: %d", m_nLevel, GetTime(), GetMoves(), score );
			pcView->SetFgColor( 0xFFFF0000 );
			pcView->DrawText( pcView->GetBounds(), sSummary );
		}
	}
}

void GameView::ShowHint( const String& cText, int nTime )
{
	m_cHintText = cText;
	m_nHintCounter = nTime;
	m_bShowHint = true;
	m_bNeedRefresh = true;
}

void GameView::DrawConnection( View* pcView, int from, int to )
{
	Point cFrom = m_caNodes[ from ].GetPosition();
	Point cTo = m_caNodes[ to ].GetPosition();
	bool bTracking = m_caNodes[ from ].IsTracking() || m_caNodes[ to ].IsTracking();
	
	if( bTracking ) {
		pcView->SetFgColor( 0xFFFFFF00 );
	} else {
		pcView->SetFgColor( 0xFF888800 );
	}
	pcView->MovePenTo( cFrom );
	pcView->DrawLine( cTo );
}

void GameView::DrawNode( View* pcView, int node )
{
	Point cCentre = m_caNodes[ node ].GetPosition();
	Point cSize = m_pcNodeIcon->GetSize();
	Point cTopLeft( cCentre.x - cSize.x/2, cCentre.y - cSize.y/2 );
	pcView->SetDrawingMode( DM_BLEND );
	if( m_caNodes[ node ].IsTracking() ) {
		m_pcSelectedNodeIcon->Draw( cTopLeft, pcView );
	} else {
		m_pcNodeIcon->Draw( cTopLeft, pcView );
	}
}

// Line intersect algorithm (isLeft and LinesIntersect) borrowed from:

// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

inline float
isLeft( Point P0, Point P1, Point P2 )
{
    return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}

// test for existence of an intersect point
bool LinesIntersect( Point& a1, Point& a2, Point& b1, Point& b2 )
{
    float lsign, rsign;
    Point s1lP, s1rP, s2lP, s2rP;
    if( a1.x > a2.x ) { s1rP = a1; s1lP = a2; }
	else { s1rP = a2; s1lP = a1; }
    if( b1.x > b2.x ) { s2rP = b1; s2lP = b2; }
	else { s2rP = b2; s2lP = b1; }
    lsign = isLeft(s1lP, s1rP, s2lP);    // s2 left point sign
    rsign = isLeft(s1lP, s1rP, s2rP);    // s2 right point sign
    if (lsign * rsign > 0) // s2 endpoints have same sign relative to s1
        return false;      // => on same side => no intersect is possible
    lsign = isLeft(s2lP, s2rP, s1lP);    // s1 left point sign
    rsign = isLeft(s2lP, s2rP, s1rP);    // s1 right point sign
    if (lsign * rsign > 0) // s1 endpoints have same sign relative to s2
        return false;      // => on same side => no intersect is possible
    // the segments s1 and s2 straddle each other
    return true;           // => an intersect exists
}

bool GameView::Check()
{
	// This could be made a lot more effective, but it's sufficently fast for this game...
	for( int i = 0; i < m_nNumNodes; i++ ) {
		std::vector<int>::iterator ci;
		for( ci = m_caNodes[ i ].ConnectedNodes.begin(); ci != m_caNodes[ i ].ConnectedNodes.end(); ci++ ) {
			if( *(ci) > i ) {
				for( int j = 0; j < m_nNumNodes; j++ ) {
					std::vector<int>::iterator cj;
					for( cj = m_caNodes[ j ].ConnectedNodes.begin(); cj != m_caNodes[ j ].ConnectedNodes.end(); cj++ ) {
						if( *(cj) > j && i != j && i != *(cj) && j != *(ci) && *(cj) != *(ci) ) {
							Point a1, a2, b1, b2;
							a1 = m_caNodes[ i ].GetPosition();
							a2 = m_caNodes[ *(ci) ].GetPosition();
							b1 = m_caNodes[ j ].GetPosition();
							b2 = m_caNodes[ *(cj) ].GetPosition();
							
							if( LinesIntersect( a1, a2, b1, b2 ) ) {
								return false;
							}
						}
					}
				}
			}
		}
	}
	return true;
}
