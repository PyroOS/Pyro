#include "node.h"

GNode::GNode()
{
	m_bTrack = 0;
}

GNode::~GNode()
{
}

void GNode::SetPosition( const Point& cPos )
{
	m_cPosition = cPos;
}

Point GNode::GetPosition()
{
	return m_cPosition;
}

void GNode::SetSolution( const Point& cEndPoint )
{
	m_cEndPoint = cEndPoint;
}

bool GNode::HitTest( const Point& cPos )
{
	return Rect( m_cPosition.x - 9, m_cPosition.y - 9, m_cPosition.x + 9, m_cPosition.y + 9 ).DoIntersect( cPos );
}

void GNode::MouseDown()
{
	m_bTrack = true;
}

void GNode::MouseUp( const Point& cPos )
{
	m_bTrack = false;
	m_cPosition = cPos;
}

void GNode::MouseMove( const Point& cPos )
{
	m_cPosition = cPos;
}

void GNode::InitializeAnimation( int nSteps )
{
	m_cDelta.x = ( m_cEndPoint.x - m_cPosition.x ) / nSteps;
	m_cDelta.y = ( m_cEndPoint.y - m_cPosition.y ) / nSteps;
}

void GNode::Animate()
{
	m_cPosition.x += m_cDelta.x;
	m_cPosition.y += m_cDelta.y;
}
