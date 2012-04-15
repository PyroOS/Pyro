                                                                                                                                                                                                       
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

#ifndef __INNER_VIEW_H__
#define __INNER_VIEW_H__

#include <gui/view.h>
#include <gui/font.h>
#include <gui/guidefines.h>
#include <gui/bitmap.h>
#include <util/message.h>
#include <util/clipboard.h>

#include <vector>

#include "HexView.h"

// Defines a column in the viwe such as line numbers or ASCII.
class HexViewColumn;

// This class does the actual rendering and editing of the buffer.
class HexView::InnerView : public View
{
	public:
		InnerView(HexView *pcOwner);
		~InnerView();
		
		void Paint(const Rect &cUpdateRect);
	
		Point GetPreferredSize(bool bLargest = false);
		
		void FrameSized(const Point &cDelta);
		
		void MouseDown(const Point &cPosition, uint32 nButtons);
		void MouseMove(const Point &cNewPos, int nCode, uint32 nButtons, Message *pData);

		void ViewScrolled(const Point &cDelta);
		void WheelMoved(const Point &cDelta);
		void KeyDown(const char *pzString, const char *pzRawString, uint32 nQualifiers);
		
		void SetBuffer(uint8 *pBuffer, int nLength);
		uint8 *GetBuffer(void) const { return m_pBuffer; }
		int GetBufferLength(void) const { return m_nLength; }
		
		float GetCharHeight(void) const { return m_vCharHeight; }
		float GetCharWidth(void) const { return m_vCharWidth; }

		void Undo(void);
		void Redo(void);
		
		void SelectAll(void);
		void SelectNone(void);
		
		void Copy(void);		
		
		const uint8 *GetCursor(void) const { return m_pCursor; }
				
		void ClearLayout(void);
		void AddColumn(ColumnType nType);
				
		void MoveCursorBy(int nDelta);
		void MoveCursorTo(uint8 *pNewPos);
		
		int GetScrollLine(void);
		
		void AddUndoEntry(HexViewColumn *pcCol, uint8 nPos, uint8 *pCursor, uint8 nOldVal, uint8 nNewVal);
		
		void FindNext(FindType nType, const String &cText);
		void FindPrev(FindType nType, const String &cText);
		void SetMessage(const String &cText) { m_pcOwner->SetMessage(cText); }
		
	private:
		uint8 *m_pBuffer; 		// Pointer to buffer being edited.
		int m_nLength; 			// Length of edit buffer.
		
		uint8 *m_pCursor; 		// Cursor position.
		uint8 *m_pSelStart; 	// Selection start.
		uint8 *m_pSelEnd; 		// Selection end.
		
		float m_vCharWidth; 	// Width of chars in current font.
		float m_vCharHeight; 	// Height of lines in current font.
			
		View *m_pcRenderView; 	// View used for double buffered rendering.
		Bitmap *m_pcRenderBitmap;	// Bitmap used for double buffered rendering.
		
		bool m_bSelecting;		// Indicates whether or not a selection is in progress.
				
		HexView *m_pcOwner;		// The main HexView that owns this view. 
		
		std::vector<HexViewColumn *> m_cColumns; // List of columns to display.
		HexViewColumn *m_pcEditCol; // Pointer to currently selected column.
		
		// Records a change made to the buffer.
		struct undo_entry
		{
			public:
				HexViewColumn *pcColumn;
				uint8 nPos;
				uint8 *pCursor;
				uint8 nOldVal;
				uint8 nNewVal;
				
				undo_entry(HexViewColumn *col, uint8 pos, uint8 *cursor, uint8 oldVal, uint8 newVal)
					: pcColumn(col)
					, nPos(pos)
					, pCursor(cursor)
					, nOldVal(oldVal)
					, nNewVal(newVal)
				{
				}
		};
		
		std::vector<undo_entry> m_cUndoBuffer;	// History of undo entries.
		std::vector<undo_entry>::iterator m_cUndoPos; // Current position in the undo buffer.
		
		void EnsureRenderBuffer(void);
		void UpdateDisplay(void);
		uint8 *GetBufferAt(const Point &cPosition);
		void ClearSelection(void);
		void ScrollToCursor(void);
};


class HexViewColumn
{
	public:
		HexViewColumn(HexView::InnerView *pcOwner, float vX)
			: m_pcOwner(pcOwner)
			, m_vX(vX)
		{ }
		
		virtual ~HexViewColumn(void) { }
		
		// Indicates whether or not the column is read only.
		virtual bool ReadOnly(void) { return false; }
		
		// Gets the X coordinate of the start of the column.
		float GetX(void) const { return m_vX; }
		
		// Gets the width of the column.
		virtual float GetWidth(void) const = 0;
		
		// Gets a pointer to the position in the buffer pointed to by cPos, or NULL.
		virtual uint8 *GetCursorAt(const Point &cPos) { return NULL; }
		
		// Render a line of the column.
		virtual void RenderLine(uint8 *pStart, float xOff, float vLineTop, float vLineBottom, 
			int nCursor, int nSelStart, int nSelEnd, View *pcView, bool bSelected) = 0;
		
		// Gets the view containing this column.
		HexView::InnerView *GetOwner(void) const { return m_pcOwner; }
		
		// Write a byte to the buffer.
		virtual bool WriteByte(uint8 *pCursor, uint8 nValue, uint8 nRawValue) { return false; }
		
		// Hook called after an undo.
		virtual void OnUndo(uint8 nPos) { }
		
		// Hook called after a redo.
		virtual void OnRedo(uint8 nPos) { }
		
		// Add an entry to the undo buffer.
		void AddUndoEntry(uint8 nPos, uint8 *pCursor, uint8 nOldVal, uint8 nNewVal)
		{
			GetOwner()->AddUndoEntry(this, nPos, pCursor, nOldVal, nNewVal);
		}
		
		// Find the next occurance of a string and return a pointer to it.
		virtual uint8 *FindNext(const String &cText, int *pnLen) { *pnLen = 0; return NULL; }
		
		// Find the previous occurance of a string and return a pointer to it.
		virtual uint8 *FindPrev(const String &cText, int *pnLen) { *pnLen = 0; return NULL; }
		
		// Gets a pointer to the start of the edit buffer.
		uint8 *GetBuffer(void) const { return GetOwner()->GetBuffer(); }
		
		// Gets the length of the edit buffer.
		int GetBufferLength(void) const { return GetOwner()->GetBufferLength(); }
		
		// Gets a pointer to the cursor.
		const uint8 *GetCursor(void) const { return GetOwner()->GetCursor(); }
		
		float GetCharWidth(void) const { return GetOwner()->GetCharWidth(); }
		
		float GetCharHeight(void) const { return GetOwner()->GetCharHeight(); }
		
		int GetScrollLine(void) const { return GetOwner()->GetScrollLine(); }	
		
		void SetMessage(const String &cText) { GetOwner()->SetMessage(cText); }
	private:
		HexView::InnerView *m_pcOwner;
		float m_vX;
};


#endif


