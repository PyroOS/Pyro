#ifndef __NODE_H__
#define __NODE_H__

#include <gui/window.h>
#include <gui/imageview.h>
#include <util/message.h>

using namespace os;

class GNode
{
	public:
	GNode();
	~GNode();

	void	SetPosition( const Point& cPos );
	Point	GetPosition();
	
	void	SetSolution( const Point& cEndPoint );

	bool	HitTest( const Point& cPos );

	void	MouseDown();
	void	MouseUp( const Point& cPos );
	void	MouseMove( const Point& cPos );

	void	InitializeAnimation( int nSteps );
	void	Animate();

	bool	IsTracking() const	{ return m_bTrack; }

	std::vector<int>	ConnectedNodes;

	private:
	Point			m_cPosition;
	Point			m_cDelta;
	Point			m_cEndPoint;
	bool			m_bTrack;
};

#endif // __NODE_H__
