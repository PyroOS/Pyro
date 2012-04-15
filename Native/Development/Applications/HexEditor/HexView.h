                                                                                                                                                                                                       
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

#ifndef __HEXVIEW_H__
#define __HEXVIEW_H__

#include <gui/view.h>
#include <gui/scrollbar.h>
#include <util/string.h>
#include <util/handler.h>

using namespace os;

#define BYTES_PER_LINE 16

enum ColumnType
{
	CT_LINE_NUMBER,
	CT_HEX,
	CT_ASCII
};

enum FindType
{
	FT_ASCII,
	FT_HEX
};

class HexView : public View
{
	public:
		HexView(const Rect &cFrame, const String &cTitle);
		~HexView();
		
		Point GetPreferredSize(bool bLargest = false);
		void Paint(const Rect &cUpdateRect);
		
		void FrameSized(const Point &cDelta);
		
		void SetBuffer(uint8 *pBuffer, int nLength, bool bFreeOld = true);
		uint8 *GetBuffer(void) const;
		int GetBufferLength(void) const;
		const uint8 *GetCursor(void) const;
		
		void CursorMoved(void);
		void FileChanged(void);
		
		void Undo(void);
		void Redo(void);
		
		void SelectAll(void);
		void SelectNone(void);
		
		void Copy(void);
		
		void ClearLayout(void);
		void AddColumn(ColumnType nType);
	
		void SetController(Handler *pcHandler) { m_pcHandler = pcHandler; }
	
		void FindNext(FindType nType, const String &cText);
		void FindPrev(FindType nType, const String &cText);
	
		void SetMessage(const String &cText);
	
		class InnerView;
		
	private:
		Handler *m_pcHandler;
		
		ScrollBar *m_pcVScroll;
		ScrollBar *m_pcHScroll;
		InnerView *m_pcInner;
		
		void SizeChildren();
		void SizeHScrollBar(const Rect &cOrigFrame, Rect *pcFrame);
		void SizeVScrollBar(const Rect &cOrigFrame, Rect *pcFrame);
};

#endif





