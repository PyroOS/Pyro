                                                                                                                                                                                                       
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

#include "Strings.h"

// Returns true if the supplied character is a valid hex char.
bool is_valid_hex_char(char c)
{
	return (c >= '0' && c <= '9') 
		|| (c >= 'a' && c <= 'f');
}

// Returns the value of a hex char.
uint8 get_key_val(char c)
{
	if(c >= '0' && c <= '9') 
		return c - '0';
	else if(c >= 'a' && c <= 'f');
		return c - 'W';
	return 0;
}

// Gets the hex character for half of a byte.
char get_nibble_char(uint8 nByte, bool bHigh)
{
	if( bHigh )
		nByte >>= 4;
	nByte &= 0x0f;
	if( nByte < 10 )
		return (char)('0' + nByte);
	return (char)('7' + nByte);	
}

// Lighten one component of a colour
#define LIGHTEN(c) c > 204?255:(int)((float)c / 0.8)

Color32_s lighten_color(const Color32_s &sCol)
{
	return Color32_s(
		LIGHTEN(sCol.red),
		LIGHTEN(sCol.green),
		LIGHTEN(sCol.blue)
	);
}

// Column showing line numbers.
class LineNumberColumn : public HexViewColumn
{
	public:
		LineNumberColumn(HexView::InnerView *pcOwner, float vX)
			: HexViewColumn(pcOwner, vX)
		{ }
		~LineNumberColumn() { }
		
		bool ReadOnly(void) { return true; }
		
		float GetWidth(void) const { return 4 + (GetCharWidth() * 8); }
		
		void RenderLine(uint8 *pStart, float xOff, float vLineTop, float vLineBottom, int nCursor, int nSelStart, int nSelEnd, View *pcView, bool bSelected)
		{
			static char zLine[9];
			sprintf(zLine, "%08x", (uint32)(pStart - GetBuffer()));
			Rect cRect(xOff + GetX() + 2, vLineTop, xOff + GetX() + GetWidth(), vLineBottom);
			pcView->DrawText(cRect, zLine);
		}		
};

// Column showing ASCII characters.
class AsciiColumn : public HexViewColumn
{
	public:
		AsciiColumn(HexView::InnerView *pcOwner, float vX)
			: HexViewColumn(pcOwner, vX)	
		{ }
		~AsciiColumn() { }
		
		float GetWidth(void) const { return 4 + (GetCharWidth() * BYTES_PER_LINE); }
		
		uint8 *GetCursorAt(const Point &cPos)
		{
			int x = (int)(cPos.x / GetCharWidth());
			int y = (int)(cPos.y / GetCharHeight()) + GetScrollLine();
			if( x < 0 || x >= BYTES_PER_LINE )
				return NULL;
			uint8 *pCursor = GetBuffer() + (y * BYTES_PER_LINE) + x;
			if( pCursor >= GetBuffer() + GetBufferLength() )
				return NULL;
			return pCursor;
		}
		
		void RenderLine(uint8 *pStart, float xOff, float vLineTop, float vLineBottom, int nCursor, int nSelStart, int nSelEnd, View *pcView, bool bSelected)
		{			
			Color32_s sCursCol(40,40,40);
			Color32_s sCursLineCol(255,235,186);
			
			if( ! bSelected )
			{
				sCursCol = lighten_color(sCursCol);
				sCursLineCol = lighten_color(sCursLineCol);
			}
		
			float vCharWidth = GetCharWidth();
			float vCharHeight = GetCharHeight();
			
			uint8 *pEnd = GetBuffer() + GetBufferLength();
			static char zLine[(BYTES_PER_LINE) + 1];
			for( int x = 0; x < BYTES_PER_LINE; x++ )
			{
				uint8 *p = pStart + x;
				zLine[x] = p >= pEnd || *p < 32 || *p > 126?'.':*p;
			}
					
			Rect cRect(xOff + GetX() + 2, vLineTop, xOff + GetX() + GetWidth() - 2, vLineBottom);
			
			pcView->SetFgColor(Color32_s(0,0,0));
			if( nSelStart < 0 )
			{				
				if( nCursor >= 0 )
					pcView->FillRect(cRect, sCursLineCol);
				
				pcView->DrawText(cRect, zLine);
	
				if( nCursor >= 0 )			
				{
					float vCursorX = cRect.left + (vCharWidth * nCursor);
					pcView->SetFgColor(sCursCol);
					pcView->DrawLine(Point(vCursorX, vLineTop), Point(vCursorX, vLineBottom));
					vCursorX++;
					pcView->DrawLine(Point(vCursorX, vLineTop), Point(vCursorX, vLineBottom));
				}
			}
			else
			{
				IPoint cSel1((int)(nSelStart * vCharWidth), (int)vCharHeight);
				IPoint cSel2((int)((nSelEnd * vCharWidth) + 1), (int)vCharHeight);
				pcView->DrawSelectedText(cRect, zLine, cSel1, cSel2, SEL_CHAR);
			}
		}
		
		// Find the next occurance of a string and return a pointer to it.
		uint8 *FindNext(const String &cText, int *pnLen)
		{			
			*pnLen = cText.size();	
	
			const uint8 *pStart = GetBuffer();
			const uint8 *pCurs = GetCursor();
			const uint8 *pLook = pCurs + 1;
			const uint8 *pEnd = pStart + GetBufferLength() - *pnLen;
			
			while( pLook != pCurs )
			{
				bool bFound = true;
				for( uint32 i = 0; i < cText.size(); i++ )
					if( *(pLook + i) != (uint8)cText[i] )
					{
						bFound = false;
						break;
					}
				if( bFound )
					break;
					
				pLook++;
				if( pLook >= pEnd )
				{
					pLook = pStart;
					SetMessage(STR_MSG_SEARCH_CONTINUE_TOP);
				}
			}
			
			return (uint8 *)pLook;
		}
		
		// Find the previous occurance of a string and return a pointer to it.
		uint8 *FindPrev(const String &cText, int *pnLen)
		{
			*pnLen = cText.size();
			
			const uint8 *pStart = GetBuffer();
			const uint8 *pCurs = GetCursor();
			const uint8 *pLook = pCurs - 1;
			const uint8 *pEnd = pStart + GetBufferLength() - *pnLen;
			
			while( pLook != pCurs )
			{
				bool bFound = true;
				for( uint32 i = 0; i < cText.size(); i++ )
					if( *(pLook + i) != (uint8)cText[i] )
					{
						bFound = false;
						break;
					}
				if( bFound )
					break;
										
				pLook--;
				if( pLook < pStart )
				{
					pLook = pEnd;
					SetMessage(STR_MSG_SEARCH_CONTINUE_BOTTOM);
				}
			}
			
			return (uint8 *)pLook;
		}
				
		bool WriteByte(uint8 *pCursor, uint8 nValue, uint8 nRawValue)
		{
			AddUndoEntry(0, pCursor, *pCursor, nValue);
			*pCursor = nValue;
			GetOwner()->MoveCursorBy(1);
				
			return true;
		}	
};


// Column providing a hexadecimal view of the data.
class HexColumn : public HexViewColumn
{
	public:
		HexColumn(HexView::InnerView *pcOwner, float vX)
			: HexViewColumn(pcOwner, vX)
			, m_bSecondChar(false)			
		{ }
		~HexColumn() { }
		
		float GetWidth(void) const { return 4 + (GetCharWidth() * 3 * BYTES_PER_LINE); }
		
		uint8 *GetCursorAt(const Point &cPos)
		{
			int x = (int)(cPos.x / (GetCharWidth() * 3));
			int y = (int)(cPos.y / GetCharHeight()) + GetScrollLine();
			if( x < 0 || x >= BYTES_PER_LINE )
				return NULL;
			uint8 *pCursor = GetBuffer() + (y * BYTES_PER_LINE) + x;
			if( pCursor >= GetBuffer() + GetBufferLength() )
				return NULL;
			return pCursor;
		}
		
		void RenderLine(uint8 *pStart, float xOff, float vLineTop, float vLineBottom, int nCursor, int nSelStart, int nSelEnd, View *pcView, bool bSelected)
		{			
			Color32_s sCursCol(40,40,40);
			Color32_s sCursLineCol(255,235,186);
			
			if( ! bSelected )
			{
				sCursCol = lighten_color(sCursCol);
				sCursLineCol = lighten_color(sCursLineCol);
			}
		
			float vCharWidth = GetCharWidth();
			float vCharHeight = GetCharHeight();
			
			uint8 *pEnd = GetBuffer() + GetBufferLength();
			static char zLine[(3 * BYTES_PER_LINE) + 1];
			uint8 *p = pStart;
			for( int x = 0; x < 3 * BYTES_PER_LINE; x+=3 )
			{
				if( p < pEnd )
				{
					zLine[x] = get_nibble_char(*p, true);
					zLine[x + 1] = get_nibble_char(*p, false);
					p++;
				} else {
					zLine[x] = ' ';
					zLine[x + 1] = ' ';
				}
				zLine[x + 2] = ' ';
			}
					
			Rect cRect(xOff + GetX() + 2, vLineTop, xOff + GetX() + GetWidth() - 2, vLineBottom);
			
			pcView->SetFgColor(Color32_s(0,0,0));
							
			if( nSelStart < 0 )
			{				
				if( nCursor >= 0 )
					pcView->FillRect(cRect, sCursLineCol);
				
				pcView->DrawText(cRect, zLine);

				if( nCursor >= 0 )
				{
					float vCursorX = cRect.left + (3 * vCharWidth * nCursor);
					if( m_bSecondChar )
						vCursorX += vCharWidth;
					pcView->SetFgColor(sCursCol);
					pcView->DrawLine(Point(vCursorX, vLineTop), Point(vCursorX, vLineBottom));
					vCursorX++;
					pcView->DrawLine(Point(vCursorX, vLineTop), Point(vCursorX, vLineBottom));
				}
			}
			else
			{
				IPoint cSel1((int)(nSelStart * 3 * vCharWidth), (int)vCharHeight);
				IPoint cSel2((int)((nSelEnd * 3 * vCharWidth) + (vCharWidth * 2)), (int)vCharHeight);
				pcView->DrawSelectedText(cRect, zLine, cSel1, cSel2, SEL_CHAR);
			}
		}
		
		// Find the next occurance of a string and return a pointer to it.
		uint8 *FindNext(const String &cText, int *pnLen)
		{
			uint8 *pBytes = ParseFindText(cText, pnLen);
			if( pBytes == NULL )
				return NULL;
				
			const uint8 *pStart = GetBuffer();
			const uint8 *pCurs = GetCursor();
			const uint8 *pLook = pCurs + 1;
			const uint8 *pEnd = pStart + GetBufferLength() - *pnLen;
			
			while( pLook != pCurs )
			{
				if( Match(pLook, pBytes, *pnLen) )
					break;
					
				pLook++;
				if( pLook >= pEnd )
				{
					pLook = pStart;
					SetMessage(STR_MSG_SEARCH_CONTINUE_TOP);
				}
			}
			
			free(pBytes);
			
			return (uint8 *)pLook;
		}
		
		// Find the previous occurance of a string and return a pointer to it.
		uint8 *FindPrev(const String &cText, int *pnLen)
		{
			uint8 *pBytes = ParseFindText(cText, pnLen);
			if( pBytes == NULL )
				return NULL;
				
			const uint8 *pStart = GetBuffer();
			const uint8 *pCurs = GetCursor();
			const uint8 *pLook = pCurs - 1;
			const uint8 *pEnd = pStart + GetBufferLength() - *pnLen;
			
			while( pLook != pCurs )
			{
				if( Match(pLook, pBytes, *pnLen) )
					break;
					
				pLook--;
				if( pLook < pStart )
				{
					pLook = pEnd;
					SetMessage(STR_MSG_SEARCH_CONTINUE_BOTTOM);
				}
			}
			
			free(pBytes);
			
			return (uint8 *)pLook;
		}
		
		bool Match(const uint8 *pBuf, const uint8 *pLook, int nLen) const
		{
			for( int i = 0; i < nLen; i++ )
				if( *(pBuf + i) != *(pLook + i) )
					return false;
			return true;
		}
		
		uint8 *ParseFindText(const String &cText, int *pnLength)
		{
			// Allow spaces or valid hex chars.
			std::vector<uint8> cBytes;
			bool bHigh = true;
			bool bErr = false;
			uint8 nTmp = 0;
			for( uint32 i = 0; i < cText.size(); i++ )
				if( is_valid_hex_char(cText[i] ) )
					if( bHigh )
					{
						nTmp = get_key_val(cText[i]) << 4;
						bHigh = false;
					} else {
						nTmp |= get_key_val(cText[i]);
						bHigh = true;
						cBytes.push_back(nTmp);
					}
				else if( cText[i] != ' ' )
				{
					bErr = true;
					break;
				}
				
			if( bErr )
			{
				SetMessage(STR_MSG_INVALID_HEX_STRING);
				return NULL;
			}
			
			uint8 *pnBytes = (uint8 *)malloc(cBytes.size());
			*pnLength = cBytes.size();
			for( int i = 0; i < *pnLength; i++ )
				*(pnBytes + i) = cBytes[i];
			return pnBytes;
		}
		
		bool WriteByte(uint8 *pCursor, uint8 nValue, uint8 nRawValue)
		{
			if( ! is_valid_hex_char(nRawValue) )
				return false;
				
			nValue = get_key_val(nRawValue);
				
			if( m_bSecondChar )
			{
				nValue = (*pCursor & 0xF0) + nValue;
				AddUndoEntry(1, pCursor, *pCursor, nValue);
				*pCursor = nValue;
				m_bSecondChar = false;
				GetOwner()->MoveCursorBy(1);
			} else {
				nValue = (*pCursor & 0x0F) + (nValue << 4);
				AddUndoEntry(0, pCursor, *pCursor, nValue);
				*pCursor = nValue;
				m_bSecondChar = true;
			}
				
			return true;
		}	
		
		void OnUndo(uint8 nPos)
		{
			m_bSecondChar = nPos > 0;
		}
		
		void OnRedo(uint8 nPos)
		{
			m_bSecondChar = nPos > 0;
		}
		
	private:
		bool m_bSecondChar;
};




