                                                                                                                                                                                                       
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

#include "InnerView.h"
#include "Strings.h"

#include "Column.cpp"

HexView::InnerView::InnerView(HexView *pcOwner)
	: View(Rect(), "HexView::InnerView", CF_FOLLOW_ALL, WID_WILL_DRAW|WID_CLEAR_BACKGROUND)
	, m_pBuffer(NULL)
	, m_nLength(0)
	, m_pCursor(NULL)
	, m_pSelStart(NULL)
	, m_pSelEnd(NULL)
	, m_vCharWidth(0)
	, m_vCharHeight(0)
	, m_pcRenderView(NULL)
	, m_pcRenderBitmap(NULL)
	, m_bSelecting(false)
	, m_pcOwner(pcOwner)
{		
	// Get the dimensions of the system fixed width font.
	font_properties sProp;
	Font::GetDefaultFont(DEFAULT_FONT_FIXED, &sProp);
	Font *pcFont = new Font(sProp);
	SetFont(pcFont);
	
	m_vCharWidth = pcFont->GetStringWidth("W", 1);
	
	font_height sHeight;
	pcFont->GetHeight(&sHeight);
	m_vCharHeight = sHeight.ascender + sHeight.descender + sHeight.line_gap;
	
	pcFont->Release();
		
	// Initialise the undo buffer.
	m_cUndoPos = m_cUndoBuffer.end();
	
	UpdateDisplay();
}

HexView::InnerView::~InnerView()
{
	if( m_pcRenderView != NULL )
	{
		delete(m_pcRenderView);
	}
	
	if( m_pcRenderBitmap != NULL )
	{
		delete(m_pcRenderBitmap);
	}
	
	ClearLayout();
}


void HexView::InnerView::ClearLayout(void)
{
	for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
		 i != m_cColumns.end();
		 i++ )
		delete(*i);
	m_cColumns.clear();
}

void HexView::InnerView::AddColumn(ColumnType nType)
{
	float x = 0;
	for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
		 i != m_cColumns.end();
		 i++ )
		x += (*i)->GetWidth();
		
	HexViewColumn *pcCol = NULL;
	switch( nType )
	{
		case CT_LINE_NUMBER:
			pcCol = new LineNumberColumn(this, x);
			break;
		case CT_HEX:
			pcCol = new HexColumn(this, x);
			break;
		case CT_ASCII:
			pcCol = new AsciiColumn(this, x);
			break;
		default:
			break;
	}
	if( pcCol != NULL )
	{
		m_cColumns.push_back(pcCol);
		m_pcEditCol = pcCol;
	}
}

void HexView::InnerView::MoveCursorBy(int nDelta)
{
	if( m_pBuffer == NULL )
		return;
		
	m_pCursor += nDelta;
	if( m_pCursor < m_pBuffer )
		m_pCursor = m_pBuffer;
	else if( m_pCursor >= m_pBuffer + m_nLength )
		m_pCursor = m_pBuffer + m_nLength - 1;
		
	m_pcOwner->CursorMoved();
}

void HexView::InnerView::MoveCursorTo(uint8 *pNewPos)
{
	if( m_pBuffer == NULL )
		return;
		
	if( pNewPos == NULL )
		return;
		
	m_pCursor = pNewPos;
	if( m_pCursor < m_pBuffer )
		m_pCursor = m_pBuffer;
	else if( m_pCursor >= m_pBuffer + m_nLength )
		m_pCursor = m_pBuffer + m_nLength - 1;
		
	m_pcOwner->CursorMoved();
}

int HexView::InnerView::GetScrollLine(void)
{
	ScrollBar *pcVScroll = GetVScrollBar();
	if( pcVScroll == NULL )
		return 0;
	return pcVScroll->GetValue().AsInt32();
}

Point HexView::InnerView::GetPreferredSize(bool bLargest)
{
	float x = 2;
	for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
		 i != m_cColumns.end();
		 i++ )
		x += (*i)->GetWidth();
	return Point(x, bLargest ? INT_MAX : m_nLength / BYTES_PER_LINE);
}

void HexView::InnerView::ViewScrolled(const Point &cDelta)
{
	UpdateDisplay();
}

void HexView::InnerView::FrameSized(const Point &cDelta)
{
	Rect cFrame = GetBounds();
			
	// Need to recreate the back buffer.
	if( m_pcRenderView != NULL )
	{
		delete(m_pcRenderView);
		m_pcRenderView = NULL;
	}
	
	if( m_pcRenderBitmap != NULL )
	{
		delete(m_pcRenderBitmap);
		m_pcRenderBitmap = NULL;
	}
	
	UpdateDisplay();
}

uint8 *HexView::InnerView::GetBufferAt(const Point &cPosition)
{
	if( m_pBuffer == NULL )
		return NULL;
		
	// Get a pointer to the location indicated by the supplied position.
	
	Point cPos(cPosition);
	cPos += GetScrollOffset();
	
	for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
		 i != m_cColumns.end();
		 i++ )
		if( cPos.x >= (*i)->GetX() && cPos.x <= (*i)->GetX() + (*i)->GetWidth() )
		{
			cPos.x -= (*i)->GetX();
			return (*i)->GetCursorAt(cPos);
			break;
		}
	
	return NULL;
} 

void HexView::InnerView::ClearSelection(void)
{
	m_bSelecting = false;
	m_pSelStart = NULL;
	m_pSelEnd = NULL;
}

void HexView::InnerView::MouseDown(const Point &cPosition, uint32 nButtons)
{
	MakeFocus();

	if( nButtons & MOUSE_BUT_LEFT )
	{
		Point cPos(cPosition);
		cPos.x -= GetScrollOffset().x;
		MoveCursorTo(GetBufferAt(cPos));
		
		for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
			 i != m_cColumns.end();
			 i++ )
			if( cPos.x >= (*i)->GetX() && cPos.x <= (*i)->GetX() + (*i)->GetWidth() )
			{
				if( ! (*i)->ReadOnly() )
					m_pcEditCol = *i;
				break;
			}
		
		ClearSelection();
		UpdateDisplay();
	}
}

void HexView::InnerView::MouseMove(const Point &cNewPos, int nCode, uint32 nButtons, Message *pData)
{
	if( nButtons & MOUSE_BUT_LEFT )
	{
		// Handle drag selection with the mouse.	
		
		Point cPos(cNewPos);
		cPos.x -= GetScrollOffset().x;
		
		uint8 *p = GetBufferAt(cPos);
		
		if( p != NULL ) 
		{
			if( ! m_bSelecting )
			{
				m_bSelecting = true;
				m_pSelStart = p;
			}
			m_pSelEnd = p;
			UpdateDisplay();
		}
	}
}

void HexView::InnerView::WheelMoved(const Point &cDelta)
{
	if( m_pBuffer == NULL )
		return;
		
	ScrollBar *pcVScroll = GetVScrollBar();
	if( pcVScroll != NULL )
	{
		// Scroll the display.
		
		Rect cFrame = GetBounds();
		int nVal = pcVScroll->GetValue().AsInt32() + (int)(cDelta.y * 2);
		int nMin = 0;
		int nMax = (m_nLength / BYTES_PER_LINE) - (int)(cFrame.Height() / m_vCharHeight) + 1;
		
		if( nVal < nMin )
			nVal = nMin;
		if( nVal > nMax )
			nVal = nMax;
		
		pcVScroll->SetValue(nVal);
		UpdateDisplay();
	}
}
 
void HexView::InnerView::KeyDown(const char *pzString, const char *pzRawString, uint32 nQualifiers)
{
	Rect cFrame = GetBounds();
	// Lines on screen.
	int nPageSize = (int)(cFrame.Height() / m_vCharHeight);
	// X position of cursor 
	int x = ((m_pCursor - m_pBuffer) % BYTES_PER_LINE);
	
	uint8 *pOldCursor = m_pCursor;
	bool bShift = (nQualifiers & QUAL_SHIFT) != 0;
	bool bCtrl = (nQualifiers & QUAL_CTRL) != 0;
	bool bAlt = (nQualifiers & QUAL_ALT) != 0;
	
	if( bAlt && pzRawString[0] >= '0' && pzRawString[0] <= '9' )
	{
		// Select a column using ALT + number
		uint nCol = (int)(pzRawString[0] - '0');
		if( nCol < m_cColumns.size() && !m_cColumns[nCol]->ReadOnly() )
		{
			m_pcEditCol = m_cColumns[nCol];
			UpdateDisplay();
		}		
	} else if( bCtrl && pzRawString[0] && pzRawString[0] != VK_HOME && pzRawString[0] != VK_END )
	{
		// Pass to base class for handling of shortcuts.
		View::KeyDown(pzString, pzRawString, nQualifiers);

	} else if( m_pBuffer != NULL && ! bAlt ) 
	{	
		bool bUpdateSelection = false;
		switch( pzRawString[0] )
		{
			case VK_LEFT_ARROW:
				bUpdateSelection = bShift;
				MoveCursorBy(-1);
				break;
			case VK_RIGHT_ARROW:
				bUpdateSelection = bShift;
				MoveCursorBy(1);	
				break;
			case VK_UP_ARROW:
				bUpdateSelection = bShift;
				MoveCursorBy(-BYTES_PER_LINE);
				break;
			case VK_DOWN_ARROW:
				bUpdateSelection = bShift;
				MoveCursorBy(BYTES_PER_LINE);
				break;
			case VK_HOME:
				bUpdateSelection = bShift;
				if( bCtrl )
					MoveCursorTo(m_pBuffer);
				else
					MoveCursorBy(-x);
				break;
			case VK_END:
				bUpdateSelection = bShift;
				if( bCtrl )
					MoveCursorTo(m_pBuffer + m_nLength - 1);
				else
					MoveCursorBy(BYTES_PER_LINE - x - 1);
				break;
			case VK_PAGE_UP: {
				bUpdateSelection = bShift;
				uint8 *pNewPos = m_pCursor - (BYTES_PER_LINE * nPageSize);
				if( pNewPos < m_pBuffer )
					pNewPos = m_pBuffer + x;
				MoveCursorTo(pNewPos);
				break; }
			case VK_PAGE_DOWN: {
				bUpdateSelection = bShift;
				uint8 *pNewPos = m_pCursor + (BYTES_PER_LINE * nPageSize);
				if( pNewPos >= m_pBuffer + m_nLength )
				{
					pNewPos = m_pBuffer + m_nLength - (m_nLength % BYTES_PER_LINE);
					if( pNewPos >= m_pBuffer + m_nLength )
						pNewPos = m_pBuffer + m_nLength - 1;
				}
				MoveCursorTo(pNewPos);
				break; }
			case VK_BACKSPACE:
			case VK_ENTER:
			case VK_TAB:
			case VK_ESCAPE:
			case VK_INSERT:
			case VK_DELETE:
			case VK_FUNCTION_KEY:
				// Keys that have no effect.
				break;
			default:
				// Key press that alters the buffer.
				if( m_pcEditCol->WriteByte(m_pCursor, (uint8)*pzString, (uint8)*pzRawString) )
					m_pcOwner->FileChanged();
				break;
		}
		
		if( bUpdateSelection && bShift )
		{
			// Update selection with keys.
			if( ! m_bSelecting )
			{
				m_bSelecting = true;
				m_pSelStart = pOldCursor;
			}
			m_pSelEnd = m_pCursor;
		} else {
			ClearSelection();
		}
		
		ScrollToCursor();			
		UpdateDisplay();
	}
}
 
void HexView::InnerView::ScrollToCursor(void)
{
	Rect cFrame = GetBounds();
	// Lines on screen.
	int nPageSize = (int)(cFrame.Height() / m_vCharHeight);
	
	ScrollBar *pcVScroll = GetVScrollBar();
	int nStartByte = (BYTES_PER_LINE * pcVScroll->GetValue().AsInt32());
	int nEndByte = nStartByte + (BYTES_PER_LINE * (nPageSize - 1));
	
	if( m_pCursor <= m_pBuffer + nStartByte )
		pcVScroll->SetValue((int)((m_pCursor - m_pBuffer) / BYTES_PER_LINE));
	if( m_pCursor > m_pBuffer + nEndByte )
		pcVScroll->SetValue((int)((m_pCursor - m_pBuffer) / BYTES_PER_LINE) - nPageSize + 1);
}
 
void HexView::InnerView::AddUndoEntry(HexViewColumn *pcCol, uint8 nPos, uint8 *pCursor, uint8 nOldVal, uint8 nNewVal)
{
	if( m_cUndoPos != m_cUndoBuffer.end() )
		m_cUndoBuffer.erase(m_cUndoPos, m_cUndoBuffer.end());
		
	m_cUndoBuffer.push_back(undo_entry(pcCol, nPos, pCursor, nOldVal, nNewVal));

	m_cUndoPos = m_cUndoBuffer.end();
}
 
void HexView::InnerView::Undo(void)
{
	if( m_cUndoBuffer.size() > 0 && m_cUndoPos != m_cUndoBuffer.begin() )
	{
		m_cUndoPos--;
		
		MoveCursorTo((*m_cUndoPos).pCursor);
		*m_pCursor = (*m_cUndoPos).nOldVal;
		m_pcEditCol = (*m_cUndoPos).pcColumn;
		m_pcEditCol->OnUndo((*m_cUndoPos).nPos);
		
		UpdateDisplay();
	}
}

void HexView::InnerView::Redo(void)
{
	if( m_cUndoPos != m_cUndoBuffer.end() )
	{
		MoveCursorTo((*m_cUndoPos).pCursor);
		*m_pCursor = (*m_cUndoPos).nNewVal;
		m_pcEditCol = (*m_cUndoPos).pcColumn;
		m_pcEditCol->OnRedo((*m_cUndoPos).nPos);

		m_cUndoPos++;		

		UpdateDisplay();
	}
}
 
void HexView::InnerView::SelectAll(void)
{
	m_pSelStart = m_pBuffer;
	m_pSelEnd = m_pBuffer + m_nLength - 1;
	m_bSelecting = true;
	UpdateDisplay();
}

void HexView::InnerView::SelectNone(void)
{
	ClearSelection();
	UpdateDisplay();
}
 
void HexView::InnerView::Copy(void)
{
	int nLen = m_pSelEnd - m_pSelStart;
	if( nLen > 0 )
	{
		char zClip[nLen + 1];
		uint8 *p = m_pSelStart;
		for( int i = 0; i < nLen; i++, p++ )
			zClip[i] = *p < 32 || *p > 126?'.':*p;
		zClip[nLen] = '\0';
		
		Clipboard cClip;
		cClip.Lock();
		cClip.Clear();
		Message *pcData = cClip.GetData();
		pcData->AddString("text/plain", zClip);
		cClip.Commit();
		cClip.Unlock();
		
	}
}
 
void HexView::InnerView::Paint(const Rect &cUpdateRect)
{		
	if( m_pcRenderBitmap != NULL )
		DrawBitmap(m_pcRenderBitmap, m_pcRenderBitmap->GetBounds(), GetBounds());
}

void HexView::InnerView::EnsureRenderBuffer(void)
{
	if( m_pcRenderView == NULL )
	{	
		Rect cFrame = GetFrame();
		float h = cFrame.Height();
		float w = cFrame.Width();
		if( w < 32 )
			w = 32;
		if( h < 32 )
			h = 32;
		// Create the view and bitmap used for rendering.
		m_pcRenderView = new View(Rect(0,0,w,h), "RenderView");
		m_pcRenderBitmap = new Bitmap((int)w + 1, (int)h + 1, CS_RGB32, Bitmap::ACCEPT_VIEWS|Bitmap::SHARE_FRAMEBUFFER);
		m_pcRenderBitmap->AddChild(m_pcRenderView);
		
		font_properties sProp;
		Font::GetDefaultFont(DEFAULT_FONT_FIXED, &sProp);
		Font *pcFont = new Font(sProp);
		m_pcRenderView->SetFont(pcFont);
				
		pcFont->Release();
	}
}

void HexView::InnerView::UpdateDisplay(void)
{
	Rect cFrame(0,0,GetFrame().Width(),GetFrame().Height());
	float xOff = GetScrollOffset().x;
	
	EnsureRenderBuffer();
		
	m_pcRenderView->EraseRect(cFrame);

	// Render boxes
	for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
		 i != m_cColumns.end();
		 i++ )
	{
		Rect cBox(xOff + (*i)->GetX(), cFrame.top, xOff + (*i)->GetX() + (*i)->GetWidth(), cFrame.bottom);
		m_pcRenderView->DrawFrame(cBox, *i == m_pcEditCol?FRAME_RAISED:FRAME_FLAT);
		cBox = Rect(cBox.left + 2, cBox.top, cBox.right - 2, cBox.bottom);
		if( (*i)->ReadOnly() )
			m_pcRenderView->FillRect(cBox, Color32_s(220,220,250));			
		else
			m_pcRenderView->FillRect(cBox, Color32_s(250,250,255));	
	}
	
	int nScrollPos = GetScrollLine();
				
	int nStartByte = (BYTES_PER_LINE * nScrollPos);
	
	// Maximum number of lines to render.
	int nMaxLines = (int)(cFrame.Height() / m_vCharHeight) + 1;
	// Maximum number of bytes to render.
	int nMaxByte = nMaxLines * BYTES_PER_LINE;
	if( nStartByte + nMaxByte > m_nLength )
		nMaxByte = m_nLength - nStartByte;
		
	// Pointer to current byte.
	uint8 *p = m_pBuffer + nStartByte;
	// Pointer to last byte.
	uint8 *pEnd = p + nMaxByte;
	int y = 0;
	float vLineTop = 0;
	float vLineBottom = 0;

	static char zHex[1 + (BYTES_PER_LINE * 3)];
	static char zAscii[1 + BYTES_PER_LINE];
	
	zHex[BYTES_PER_LINE * 3] = '\0';
	zAscii[BYTES_PER_LINE] = '\0';

	if( m_pBuffer != NULL )
	{

		// Get start and end of selection.
		uint8 *pSelStart = m_pSelStart;
		uint8 *pSelEnd = m_pSelEnd;
		if( pSelEnd < pSelStart )
		{
			uint8 *pTmp = pSelStart;
			pSelStart = pSelEnd;
			pSelEnd = pTmp;
		}
		
		while( p < pEnd )
		{
			m_pcRenderView->SetFgColor(Color32_s(0,0,0));
			
			// Calculate the top and bottom pixels of the line of text.
			vLineTop = cFrame.top + 2 + (y * m_vCharHeight);
			vLineBottom = vLineTop + m_vCharHeight;
			
			int nSelStart = -1;
			int nSelEnd = -1;
			int nCursor = -1;
			// For each byte in a line.
			for( int x = 0; x < BYTES_PER_LINE; x++ )
			{
				uint8 *p2 = p + x;
				if( nSelStart == -1 )
				{
					// Calculate selection start.
					if( p2 >= pSelStart && p2 <= pSelEnd )
					{
						nSelStart = x;
						if( p2 == pSelEnd )
							nSelEnd = x;
						else
							nSelEnd = BYTES_PER_LINE;
					}
				}
				else
				{
					// Find selection end.
					if( p2 == pSelEnd )
						nSelEnd = x;
				}
				
				// Record cursor position.
				if( p2 == m_pCursor )
					nCursor = x;
			}
			
			for( std::vector<HexViewColumn *>::iterator i = m_cColumns.begin();
					 i != m_cColumns.end();
					 i++ )
				(*i)->RenderLine(p, xOff, vLineTop, vLineBottom, nCursor, nSelStart, nSelEnd, m_pcRenderView, *i == m_pcEditCol);
				
			p += BYTES_PER_LINE;
			y++;
		}
	}
	
	m_pcRenderView->Sync();
	
	Invalidate();
	Flush();
}

void HexView::InnerView::SetBuffer(uint8 *pBuffer, int nLength)
{
	ClearSelection();
	m_pCursor = pBuffer;
	m_cUndoBuffer.clear();
	m_pBuffer = pBuffer;
	m_nLength = nLength;
	ScrollBar *m_pcVScroll = GetVScrollBar();
	if( m_pcVScroll != NULL )
		m_pcVScroll->SetValue(0);
		
	UpdateDisplay();
}

HexViewColumn *get_finder(FindType nType, HexView::InnerView *pcOwner)
{
	switch(nType)
	{
		case FT_ASCII:
			return new AsciiColumn(pcOwner, 0);
		case FT_HEX:
			return new HexColumn(pcOwner, 0);
		default:
			return NULL;
	}
}

void HexView::InnerView::FindNext(FindType nType, const String &cText)
{
	HexViewColumn *pcCol = get_finder(nType, this);
	if( pcCol != NULL )
	{
		int nLen = 0;
		uint8 *p = pcCol->FindNext(cText, &nLen);
		if( p != NULL )
		{
			if( p == m_pCursor )
				SetMessage(STR_MSG_SEARCH_NOT_FOUND);
			else
			{
				m_pCursor = p;
				m_pSelStart = p;
				m_pSelEnd = p + nLen - 1;
				ScrollToCursor();
				UpdateDisplay();
			}
		}
		delete(pcCol);
	}
}

void HexView::InnerView::FindPrev(FindType nType, const String &cText)
{
	HexViewColumn *pcCol = get_finder(nType, this);
	if( pcCol != NULL )
	{
		int nLen = 0;
		uint8 *p = pcCol->FindPrev(cText, &nLen);
		if( p != NULL )
		{
			if( p == m_pCursor )
				SetMessage(STR_MSG_SEARCH_NOT_FOUND);
			else
			{
				m_pCursor = p;
				m_pSelStart = p;
				m_pSelEnd = p + nLen - 1;
				ScrollToCursor();
				UpdateDisplay();
			}
		}
		delete(pcCol);
	}
}




