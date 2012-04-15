#include "view.h"
#include "util/random.h"

RandView::RandView() : View(os::Rect(0,0,450,300),"RandView")
{
}

void RandView::Paint(const os::Rect&)
{
	os::Random rand;
	
	
	if (m_bFont)
	{
		m_pcFont = new Font(rand.AsFont(true,true));
		SetFont(m_pcFont);
	}
	
	
	if (m_bFontColor)
		m_sFontColor = rand.AsColor(true);
	else
		m_sFontColor = Color32_s(255,255,255,255);
			
	
	if (m_bBackColor)
		m_sBackColor = rand.AsColor(false);
	else
		m_sBackColor = os::Color32_s(0,0,0,255);	
	
	
	FillRect(GetBounds(),m_sBackColor);
	
	
	SetFgColor(m_sFontColor);
	DrawText(GetBounds(),rand.AsString(m_nStringType,20));
	
}

os::Point RandView::GetPreferredSize(bool) const
{
	return os::Point(450,300);
}

void RandView::Refresh()
{
	Invalidate();
}

void RandView::SetRandomBackColor(bool color)
{
	m_bBackColor = color;
}

void RandView::SetRandomFont(bool bFont)
{
	m_bFont = bFont;
}

void RandView::SetRandomFontColor(bool bFntColor)
{
	m_bFontColor = bFntColor;
}

void RandView::SetRandomTextType(uint nType)
{
	m_nStringType = nType;
}

bool RandView::GetRandBackColor()
{
	return m_bBackColor;
}

bool RandView::GetRandomFont()
{
	return m_bFont;
}

bool RandView::GetRandomFontColor()
{
	return m_bFontColor;
}

uint RandView::GetStringType()
{
	return m_nStringType;
}
