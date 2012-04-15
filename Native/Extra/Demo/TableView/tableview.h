/*  libsyllable.so - the highlevel API library for Syllable
 *  Copyright (C) 1999 - 2001 Kurt Skauen
/*  Copyright (C) 2003 - 2004 Syllable Team
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 *  MA 02111-1307, USA
 */

/*

 The algorithm for TableView is based on the one used in GTK.
 More information about GTK can be found at www.gtk.org. 
 The orginal algorithm is found in GtkTable which source code 
 can be downloaded from ftp://ftp.gtk.org/pub/gtk/

 It was "syllablized" by Jonas Jarvoll in 2006 

*/

#ifndef __F_GUI_TABLEVIEW_H__
#define __F_GUI_TABLEVIEW_H__

#include <list>
#include <gui/view.h>

namespace os
{
#if 0	// Fool Emacs auto-indent
}
#endif

typedef struct _TableChild   TableChild;
typedef struct _TableRowCol  TableRowCol;

struct _TableChild
{
	os::View* widget;
	uint16 left_attach;
	uint16 right_attach;
	uint16 top_attach;
	uint16 bottom_attach;
	uint16 xpadding;
	uint16 ypadding;
	bool xexpand;
	bool yexpand;
	bool xshrink;
	bool yshrink;
	bool xfill;
	bool yfill;
};

struct _TableRowCol
{
	uint16 requisition;
	uint16 allocation;
	uint16 spacing;
	bool need_expand;
	bool need_shrink;
	bool expand;
	bool shrink;
	bool empty;
};

class	TableView : public View
{
public:
	#define MAX( x, y ) ( x<y?y:x )
	#define TABLE_EXPAND 	0x01
	#define TABLE_FILL 	 	0x02
	#define TABLE_SHRINK 	0x04

	TableView( const os::Rect& cFrame, const os::String& cName, int nWidth, int nHeight,
			   bool homogenous, uint32 nResizeMask = CF_FOLLOW_LEFT | CF_FOLLOW_TOP );
	~TableView();

	void FrameSized( const os::Point& cDelta );
	os::Point GetPreferredSize( bool bLargest );

	void Layout();

	void Resize( uint rows, uint columns );
	
	void Attach( os::View* child, uint left_attach, uint right_attach, uint top_attach, uint bottom_attach,
				 int xoptions = ( TABLE_EXPAND | TABLE_FILL ), int yoptions = ( TABLE_EXPAND | TABLE_FILL ), 
				 uint xpadding = 0, uint ypadding = 0);

	void SetRowSpacing( uint row, uint spacing );
	uint GetRowSpacing( uint row );
	
	void SetColumnSpacing( uint colum, uint spacing );
	uint GetColumnSpacing( uint colum );

	void SetRowSpacings( uint spacing );
	uint GetDefaultRowSpacing();

	void SetColumnSpacings( uint spacing );
	uint GetDefaultColumnSpacing();

	void SetHomogeneous( bool homogeneous );
	bool GetHomogeneous();
   
private:
	void Layout( os::Rect cBounds );
	void SizeRequestInit();
	void SizeRequestPass1();
	void SizeRequestPass2();
	void SizeRequestPass3();
	void SizeAllocateInit();
	void SizeAllocatePass1();
	void SizeAllocatePass2();

	std::list<TableChild*> m_ListOfChildren;

	TableRowCol* m_Rows;
	TableRowCol* m_Cols;
	uint16 m_NumRows;
	uint16 m_NumCols;
	uint16 m_ColumnSpacing;
	uint16 m_RowSpacing;
	bool m_Homogeneous;
};

}

#endif // __F_GUI_TABLEVIEW_H__

