                                                                                                                                                                                                       
// Hex Editor - Copyright 2007 Andrew Kennan
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "HexView.h"
#include "EditController.h"
#include "InnerView.h"

HexView::HexView(const Rect &cFrame, const String &cTitle)
	: View(cFrame, cTitle, CF_FOLLOW_ALL, WID_FULL_UPDATE_ON_RESIZE)
{
	m_pcInner = new InnerView(this);
	m_pcInner->AddColumn(CT_LINE_NUMBER);
	m_pcInner->AddColumn(CT_HEX);
	m_pcInner->AddColumn(CT_ASCII);
		
	m_pcVScroll = new ScrollBar(Rect(), "m_pcVScroll", new Message());
	m_pcVScroll->SetScrollTarget(m_pcInner);
	
	m_pcHScroll = new ScrollBar(Rect(), "m_pcHScroll", new Message(), 0, FLT_MAX, HORIZONTAL);
	m_pcHScroll->SetScrollTarget(m_pcInner);

	AddChild(m_pcInner);
	AddChild(m_pcHScroll);
	AddChild(m_pcVScroll);
		
	SizeChildren();
}

HexView::~HexView()
{
}

Point HexView::GetPreferredSize(bool bLargest)
{
	if( bLargest )
		return Point(INT_MAX, INT_MAX);
	
	Point cSize = m_pcInner->GetPreferredSize(false);
	if( m_pcVScroll->IsVisible() )
	{
		Point cVSBSize = m_pcVScroll->GetPreferredSize(false);		
		cSize.x += cVSBSize.x;
		cSize.y = cSize.y > cVSBSize.y?cSize.y:cVSBSize.y;
	}
	
	if( m_pcHScroll->IsVisible() )
	{
		Point cHSBSize = m_pcHScroll->GetPreferredSize(false);
		cSize.y += cHSBSize.y;
	}
	
	return cSize;
}
		
void HexView::FrameSized(const Point &cDelta)
{
	SizeChildren();
}

uint8 *HexView::GetBuffer(void) const
{
	return m_pcInner->GetBuffer();
}

int HexView::GetBufferLength(void) const
{
	return m_pcInner->GetBufferLength();
}

void HexView::SizeHScrollBar(const Rect &cOrigFrame, Rect *pcFrame)
{
	Point cIVSize = m_pcInner->GetPreferredSize(false);
	Point cHSBSize = m_pcHScroll->GetPreferredSize(false);
	
	if( cIVSize.x <= pcFrame->Width() )
	{
		if( m_pcHScroll->IsVisible() )
			m_pcHScroll->Hide();
	}
	else
	{
		m_pcHScroll->SetMinMax(0, cIVSize.x - pcFrame->Width());
		m_pcHScroll->SetProportion(pcFrame->Width() / cIVSize.x);
		m_pcHScroll->SetSteps(1, cIVSize.x / 10);
		if( ! m_pcHScroll->IsVisible() )
			m_pcHScroll->Show(true);

		if( pcFrame->Height() == cOrigFrame.Height() )
		{
			pcFrame->bottom -= cHSBSize.y;
			
			SizeVScrollBar(cOrigFrame, pcFrame);
		}
	}
}

void HexView::SizeVScrollBar(const Rect &cOrigFrame, Rect *pcFrame)
{	
	int nVisibleLines = (int)(pcFrame->Height() / m_pcInner->GetCharHeight());
	int nActualLines = GetBufferLength() / BYTES_PER_LINE;
		
	if( nVisibleLines >= nActualLines )
	{
		if( m_pcVScroll->IsVisible() )
			m_pcVScroll->Hide();
	}
	else 
	{
		m_pcVScroll->SetMinMax(0,nActualLines - nVisibleLines + 1);
		m_pcVScroll->SetProportion((float)nVisibleLines / (float)nActualLines);
		m_pcVScroll->SetSteps(1, nVisibleLines);
		
		if( ! m_pcVScroll->IsVisible() )
			m_pcVScroll->Show(true);

		if( pcFrame->Width() == cOrigFrame.Width() )
		{
			Point cVSBSize = m_pcVScroll->GetPreferredSize(false);
			pcFrame->right -= cVSBSize.x;
			
			SizeHScrollBar(cOrigFrame, pcFrame);
		}
	}
}

void HexView::SizeChildren(void)
{
	Rect cFrame = GetBounds();
	
	cFrame.left += 2;
	cFrame.top += 2;
	cFrame.right -= 2;
	cFrame.bottom -= 2;
	
	Rect cInnerFrame(cFrame);
	
	if( m_pcVScroll->IsVisible() )
		m_pcVScroll->Hide();
	if( m_pcHScroll->IsVisible() )
		m_pcHScroll->Hide();

	SizeVScrollBar(cFrame, &cInnerFrame);		
	SizeHScrollBar(cFrame, &cInnerFrame);
	
	m_pcInner->SetFrame(cInnerFrame);
	
	if( m_pcVScroll->IsVisible() )
		m_pcVScroll->SetFrame(Rect(cInnerFrame.right, cFrame.top, cFrame.right, cInnerFrame.bottom));
	if( m_pcHScroll->IsVisible() )
		m_pcHScroll->SetFrame(Rect(cFrame.left, cInnerFrame.bottom, cInnerFrame.right, cFrame.bottom));
	
	Invalidate();
	Flush();
}

void HexView::Paint(const Rect &cUpdateRect)
{
	Rect cBounds(GetBounds());
	EraseRect(cBounds);
	DrawFrame(cBounds, FRAME_RECESSED);
}

void HexView::CursorMoved(void)
{
	Messenger cMnger(m_pcHandler);
	cMnger.SendMessage(new Message(EV_CURSOR_MOVED));
}

void HexView::FileChanged(void)
{
	Messenger cMnger(m_pcHandler);
	cMnger.SendMessage(new Message(EV_FILE_CHANGED));
}

void HexView::Undo(void)
{
	m_pcInner->Undo();
}

void HexView::Redo(void)
{
	m_pcInner->Redo();
}

void HexView::SelectAll(void)
{
	m_pcInner->SelectAll();
}

void HexView::SelectNone(void)
{
	m_pcInner->SelectNone();
}

void HexView::Copy(void)
{
	m_pcInner->Copy();
}

void HexView::SetBuffer(uint8 *pBuffer, int nLength, bool bFreeOld)
{
	if( bFreeOld )
	{
		uint8 *pBuf = m_pcInner->GetBuffer();
		m_pcInner->SetBuffer(NULL, 0);
		free(pBuf);
	}
	
	m_pcInner->SetBuffer(pBuffer, nLength);
	
	SizeChildren();
}

const uint8 *HexView::GetCursor(void) const
{
	return m_pcInner->GetCursor();
}

void HexView::FindNext(FindType nType, const String &cText)
{
	m_pcInner->FindNext(nType, cText);
}

void HexView::FindPrev(FindType nType, const String &cText)
{
	m_pcInner->FindPrev(nType, cText);
}

void HexView::SetMessage(const String &cText)
{
	Message *pcMsg = new Message(EV_SHOW_MESSAGE);
	pcMsg->AddString(MESSAGE_TEXT_KEY, cText);
	Messenger cMnger(m_pcHandler);
	cMnger.SendMessage(pcMsg);
}



