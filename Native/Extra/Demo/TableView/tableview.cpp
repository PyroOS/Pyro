/*  libsyllable.so - the highlevel API library for Syllable
 *  Copyright (C) 1999 - 2001 Kurt Skauen
 *  Copyright (C) 2003 - 2006 The Syllable Team
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

#include <assert.h>
#include <gui/window.h>
#include "tableview.h"
#include <gui/font.h>
#include <util/message.h>

using namespace os;
using namespace std;


/** Create new the TableView
 * \par Description:
 *      Used to create a new table widget. An initial size must be given by specifying how 
 *      many rows and columns the table should have, although this can be changed later with 
 *      Resize(). cNumRows and cNumCols must both be in the range 0 .. 65535. 
 * \param cFrame - Size of the TableView
 * \param cName - Name of the TableView
 * \param cNumCols - The number of columns the new table should have
 * \param cNumRows - The number of rows the new table should have
 * \param bHomogeneous - If set to true, all table cells are resized to the size of the cell containing the largest widget. 
 * \param nResizeMask - The resize mask
 * \sa os::View
 * \author Jonas Jarvoll
 *****************************************************************************/
TableView :: TableView( const Rect& cFrame, const String& cName, 
                      	int nNumCols, int nNumRows, bool bHomogeneous, uint32 nResizeMask )
    : View( cFrame, cName, nResizeMask, WID_WILL_DRAW )
{  
	if( nNumRows == 0 )
		nNumRows = 1;
	if( nNumCols == 0 )
		nNumCols = 1;

	m_Rows = NULL;
	m_Cols = NULL;
	m_NumRows = 0;
	m_NumCols = 0;
	m_ColumnSpacing = 0;
	m_RowSpacing = 0;
	m_Homogeneous = bHomogeneous;

	Resize( nNumRows, nNumCols );
}

/** The destructor for TableView
 * \author Jonas Jarvoll
 *****************************************************************************/
TableView :: ~TableView()
{
	delete m_Cols;
	delete m_Rows;
}

void TableView :: FrameSized( const Point& cDelta )
{
	Layout( GetBounds() );
	Invalidate( GetBounds() );
}

/** Returns the size of the widget
 * \par Description:
 *      The function returns the size needed by the widget, either the minimum size (if bLargest set to false) or
 *      the largest size if bLargest is set to true
 * \param bLargest - If bLargest is set to false the minimum size the table requires is returns.
 * \return os::Point - Returns the size of the widget
 * \author Jonas Jarvoll
 *****************************************************************************/
Point TableView :: GetPreferredSize( bool bLargest )
{
	Point requisition( 0.0f, 0.0f);

	if( !bLargest )
	{
		int row, col;

		SizeRequestInit();
		SizeRequestPass1();
		SizeRequestPass2();
		SizeRequestPass3();
		SizeRequestPass2();

		for( col = 0 ; col < m_NumCols ; col++ )
			requisition.x += (float) m_Cols[ col ].requisition;
		for( col = 0 ; col + 1 < m_NumCols ; col++ )
			requisition.x += (float) m_Cols[ col ].spacing;
  
		for( row = 0 ; row < m_NumRows ; row++ )
			requisition.y += (float) m_Rows[ row ].requisition;
		for( row = 0 ; row + 1 < m_NumRows ; row++ )
			requisition.y += (float) m_Rows[ row ].spacing;
	}
	else
		requisition = Point( 10000, 10000 );

	return requisition;	
}

/** Recalculate table cells size
 * \par Description:
 *      The function forces a recalculation of the all the table cells.
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: Layout()
{
	Layout( GetBounds() );
}

/** Resize the table
 * \par Description:
 *      If you need to change a table's size after it has been created, 
 *      this function allows you to do so.
 * \param rows - The number of rows
 * \param columns - The number of columns
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: Resize( uint rows, uint columns )
{
	assert ( rows > 0 &&  rows < 65536 );
	assert ( columns > 0 && columns < 65536);

	rows = MAX( rows, 1 );
	columns = MAX( columns, 1 );

	if( rows != m_NumRows || columns != m_NumCols)
	{
		list< TableChild* >::iterator it;
      
		for ( it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++)
		{
			TableChild* child = (*it);

			rows = MAX( rows, child->bottom_attach);
			columns = MAX( columns, child->right_attach );
        }
      
		if( rows != m_NumRows)
		{
			uint i;

			i = m_NumRows;
			m_NumRows = rows;
          	m_Rows = ( TableRowCol* ) realloc( m_Rows, m_NumRows * sizeof( TableRowCol ) );
          
			for (; i < m_NumRows; i++)
			{
				m_Rows[i].requisition = 0;
				m_Rows[i].allocation = 0;
				m_Rows[i].spacing = m_RowSpacing;
				m_Rows[i].need_expand = false;
 				m_Rows[i].need_shrink = false;
				m_Rows[i].expand = false;
 				m_Rows[i].shrink = false;
            }
        }

		if ( columns != m_NumCols)
		{
			uint i;

			i = m_NumCols;
			m_NumCols = columns;
			m_Cols = ( TableRowCol* ) realloc ( m_Cols, m_NumCols * sizeof( TableRowCol ) );

			for (; i < m_NumCols; i++)
			{
				m_Cols[i].requisition = 0;
  				m_Cols[i].allocation = 0;
 				m_Cols[i].spacing = m_ColumnSpacing;
 				m_Cols[i].need_expand = false;
				m_Cols[i].need_shrink = false;
 				m_Cols[i].expand = false;
				m_Cols[i].shrink = false;
            }
		}
	}
}

/** Add widget to table
 * \par Description:
 *      Adds a widget to a table. The number of 'cells' that a widget will occupy 
 *      is specified by left_attach, right_attach, top_attach and bottom_attach. 
 *      These each represent the leftmost, rightmost, uppermost and lowest column 
 *      and row numbers of the table. (Columns and rows are indexed from zero). 
 *      
 *      The values for xoptions and yoptions can be one of the following or more:
 *      TABLE_EXPAND - 
 *      TABLE_FILL - 
 *      TABLE_SHRINK -
 * \param child - The widget to add
 * \param left_attach - the column number to attach the left side of a child widget to
 * \param right_attach - the column number to attach the right side of a child widget to
 * \param top_attach - the row number to attach the top of a child widget to
 * \param bottom_attach - the row number to attach the bottom of a child widget to
 * \param xoptions - Used to specify the properties of the child widget when the table is resized
 * \param yoptions - The same as xoptions, except this field determines behaviour of vertical resizing
 * \param xpadding - An integer value specifying the padding on the left and right of the widget being added to the table
 * \param ypadding - The amount of padding above and below the child widget
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: Attach( View* child, uint left_attach, uint right_attach, uint top_attach, uint bottom_attach,
						  int xoptions, int yoptions, uint xpadding, uint ypadding)
{
	if( right_attach >= m_NumCols )
    	Resize( m_NumRows, right_attach );
  
  	if( bottom_attach >= m_NumRows )
	    Resize( bottom_attach, m_NumCols);

	TableChild* table_child = ( TableChild* ) new TableChild;
	table_child->widget = child;
	table_child->left_attach = left_attach;
	table_child->right_attach = right_attach;
	table_child->top_attach = top_attach;
	table_child->bottom_attach = bottom_attach;
	table_child->xexpand = (xoptions & TABLE_EXPAND) != 0;
	table_child->xshrink = (xoptions & TABLE_SHRINK) != 0;
	table_child->xfill = (xoptions & TABLE_FILL) != 0;
	table_child->xpadding = xpadding;
	table_child->yexpand = (yoptions & TABLE_EXPAND) != 0;
	table_child->yshrink = (yoptions & TABLE_SHRINK) != 0;
	table_child->yfill = (yoptions & TABLE_FILL) != 0;
	table_child->ypadding = ypadding;
  
	m_ListOfChildren.push_back( table_child );

	// Attached to the view
	AddChild( child );
}

/** Set row spacing
 * \par Description:
 *      Changes the space between a given table row and its surrounding rows 
 * \param row - row number whose spacing will be changed
 * \param spacing - number of pixels that the spacing should take up
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: SetRowSpacing( uint row, uint spacing )
{
	assert( row < m_NumRows );
  
	if( m_Rows[ row ].spacing != spacing )
	{
		m_Rows[ row ].spacing = spacing;
      
		Invalidate();
    }
}

/** Returns spacing for specific row
 * \par Description:
 *      Gets the amount of space between row row and row row + 1
 * \param row - a row in the table, 0 indicates the first row
 * \return The row spacing
 * \author Jonas Jarvoll
 *****************************************************************************/
uint TableView :: GetRowSpacing( uint row )
{
	assert( row < m_NumRows - 1 );
 
 	return m_Rows[ row ].spacing;
}

/** Set column spacing
 * \par Description:
 *      Changes the space between a given table column and its surrounding columns
 * \param col - col number whose spacing will be changed
 * \param spacing - number of pixels that the spacing should take up
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: SetColumnSpacing( uint col, uint spacing )
{
	assert( col < m_NumCols );
  
	if( m_Cols[ col ].spacing != spacing )
	{
		m_Cols[ col ].spacing = spacing;
      
		Invalidate();
    }
}

/** Returns spacing for specific column
 * \par Description:
 *      Gets the amount of space between column col and column col + 1
 * \param col - a column in the table, 0 indicates the first column
 * \return The colum spacing
 * \author Jonas Jarvoll
 *****************************************************************************/
uint TableView :: GetColumnSpacing( uint col )
{
	assert( col < m_NumCols - 1 );
 
 	return m_Cols[ col ].spacing;
}

/** Set row spacing for all rows
 * \par Description:
 *      Sets the space between every row in table equal to spacing
 * \param spacing - the number of pixels of space to place between every row in the table
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: SetRowSpacings( uint spacing )
{
	uint row;  
  
	m_RowSpacing = spacing;

	for( row = 0; row < m_NumRows ; row++ )
		m_Rows[ row ].spacing = spacing;

	Invalidate();
}

/** Get the default row spacing
 * \par Description:
 *      Gets the default row spacing for the table. This is the spacing that will be used for newly added rows.
 * \return The default row spacing
 * \author Jonas Jarvoll
 *****************************************************************************/
uint TableView :: GetDefaultRowSpacing()
{
  return m_RowSpacing;
}

/** Set column spacing for all columns
 * \par Description:
 *      Sets the space between every column in table equal to spacing
 * \param spacing - the number of pixels of space to place between every column in the table
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: SetColumnSpacings( uint spacing )
{
	uint col;  
  
	m_ColumnSpacing = spacing;

	for( col = 0; col < m_NumCols ; col++ )
		m_Cols[ col ].spacing = spacing;

	Invalidate();
}

/** Get the default column spacing
 * \par Description:
 *      Gets the default column spacing for the table. This is the spacing that will be used for newly added columns.
 * \return The default column spacing
 * \author Jonas Jarvoll
 *****************************************************************************/
uint TableView :: GetDefaultColumnSpacing()
{
  return m_ColumnSpacing;
}

/** Set the homogenous property of table cells
 * \par Description:
 *      Changes the homogenous property of table cells, ie. whether all cells are an equal size or not
 * \param homogeneous - Set to true to ensure all table cells are the same size. Set to false if this 
 *                      is not your desired behaviour
 * \author Jonas Jarvoll
 *****************************************************************************/
void TableView :: SetHomogeneous( bool homogeneous )
{
  if( homogeneous != m_Homogeneous)
    {
      m_Homogeneous = homogeneous;

		Invalidate();      
    }
}

/** Get the homogenous property of table cells
 * \par Description:
 *      Returns the homogenous property of table cells, ie. whether all cells are an equal size or not
 * \return Returns true or false
 * \author Jonas Jarvoll
 *****************************************************************************/
bool TableView :: GetHomogeneous()
{
	return m_Homogeneous;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// P R I V A T E   M E T H O D S
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

void TableView :: Layout( Rect cBounds )
{
	GetPreferredSize( false );
	SizeAllocateInit();
	SizeAllocatePass1();
	SizeAllocatePass2();
}

void TableView :: SizeRequestInit()
{
	TableChild* child;
	int row, col;

	for( row = 0 ; row < m_NumRows ; row++ )
	{
		m_Rows[ row ].requisition = 0;
		m_Rows[ row ].expand = false;
	}
	for( col = 0 ; col < m_NumCols; col++)
    {
		m_Cols[ col ].requisition = 0;
		m_Cols[ col ].expand = false;
    }
  
	for( list< TableChild* >::iterator it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++ )
	{	
		child = (*it);

		if (child->left_attach == (child->right_attach - 1) && child->xexpand )
			m_Cols[ child->left_attach ].expand = true;

		if( child->top_attach == (child->bottom_attach - 1) && child->yexpand )
			m_Rows[ child->top_attach ].expand = true;
	}
}

void TableView :: SizeRequestPass1()
{
	TableChild* child;
	int width;
	int height;
  
	for( list< TableChild* >::iterator it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++ )
	{	
		child = (*it);
      
//      	if( child->widget->IsVisible() )
		{
			Point child_requisition = child->widget->GetPreferredSize( false );

          	// Child spans a single column.
			if( child->left_attach == (child->right_attach - 1) )
			{
				width = (int)child_requisition.x + child->xpadding * 2;
				m_Cols[ child->left_attach ].requisition = MAX( m_Cols[ child->left_attach ].requisition, width );
			}
          
			// Child spans a single row.
			if( child->top_attach == (child->bottom_attach - 1) )
			{
				height = (int)child_requisition.y + child->ypadding * 2;
				m_Rows[ child->top_attach ].requisition = MAX( m_Rows[ child->top_attach ].requisition, height );
			}
		}
	}
}

void TableView :: SizeRequestPass2()
{
	int max_width;
	int max_height;
	int row, col;

	if( m_Homogeneous )
	{
		max_width = 0;
		max_height = 0;

		for( col = 0 ; col < m_NumCols ; col++ )
			max_width = MAX( max_width, m_Cols[ col ].requisition );
		for( row = 0 ; row < m_NumRows; row++)
			max_height = MAX( max_height, m_Rows[ row ].requisition );

		for( col = 0 ; col < m_NumCols ; col++ )
			m_Cols[ col ].requisition = max_width;
		for( row = 0 ; row < m_NumRows ; row++ )
			m_Rows[ row ].requisition = max_height;
	}
}

void TableView :: SizeRequestPass3()
{
	TableChild *child;
	int width, height;
	int row, col;
	int extra;

	for( list< TableChild* >::iterator it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++ )
	{	
		child = (*it);
      
//		if( child->widget->IsVisible() )
		{
			// Child spans multiple columns.
			if( child->left_attach != (child->right_attach - 1) )
			{
				Point child_requisition = child->widget->GetPreferredSize( false );

				// Check and see if there is already enough space for the child.
				width = 0;
				for ( col = child->left_attach ; col < child->right_attach ; col++ )
				{
					width += m_Cols[ col ].requisition;
					if( ( col + 1 ) < child->right_attach )
						width += m_Cols[ col ].spacing;
				}

				/* If we need to request more space for this child to fill
				 *  its requisition, then divide up the needed space amongst the
				 *  columns it spans, favoring expandable columns if any.
				 */
				if( width < (int)child_requisition.x + child->xpadding * 2 )
				{
					int n_expand = 0;
					bool force_expand = false;

					width = (int)child_requisition.x + child->xpadding * 2 - width;

					for( col = child->left_attach ; col < child->right_attach ; col++ )
						if( m_Cols[col].expand)
							n_expand++;

					if( n_expand == 0 )
					{
						n_expand = ( child->right_attach - child->left_attach );
						force_expand = false;
					}

					for( col = child->left_attach ; col < child->right_attach ; col++ )
						if( force_expand || m_Cols[ col ].expand)
						{
							extra = width / n_expand;
							m_Cols[ col ].requisition += extra;
							width -= extra;
							n_expand--;
						}
				}
			}
          
			// Child spans multiple rows.
			if( child->top_attach != ( child->bottom_attach - 1 ) )
			{
				Point child_requisition = child->widget->GetPreferredSize( false );

				// Check and see if there is already enough space for the child.
				height = 0;
				for( row = child->top_attach ; row < child->bottom_attach ; row++ )
				{
					height += m_Rows[ row ].requisition;
					if( (row + 1) < child->bottom_attach )
						height += m_Rows[ row ].spacing;
				}

				/* If we need to request more space for this child to fill
				 *  its requisition, then divide up the needed space amongst the
				 *  rows it spans, favoring expandable rows if any.
				 */
				if( height < (int)child_requisition.y + child->ypadding * 2 )
				{
					int n_expand = 0;
					bool force_expand = false;

					height = (int)child_requisition.y + child->ypadding * 2 - height;
 
					for( row = child->top_attach ; row < child->bottom_attach ; row++ )
					{
						if( m_Rows[ row ].expand )
						n_expand++;
					}

					if( n_expand == 0 )
					{
						n_expand = ( child->bottom_attach - child->top_attach );
						force_expand = true;
					}

					for( row = child->top_attach ; row < child->bottom_attach ; row++ )
						if (force_expand || m_Rows[ row ].expand)
						{
							extra = height / n_expand;
							m_Rows[ row ].requisition += extra;
							height -= extra;
							n_expand--;
                      	}
				}
			}
		}
	}
}

void TableView :: SizeAllocateInit()
{
	TableChild* child;
	int row, col;
	bool has_expand;
	bool has_shrink;

  	/* Initialize the rows and cols.
   	 *  By default, rows and cols do not expand and do shrink.
	 *  Those values are modified by the children that occupy
	 *  the rows and cols.
   	 */
	for( col = 0 ; col < m_NumCols ; col++ )
	{
		m_Cols[ col ].allocation = m_Cols[ col ].requisition;
		m_Cols[ col ].need_expand = false;
		m_Cols[ col ].need_shrink = true;
		m_Cols[ col ].expand = false;
		m_Cols[ col ].shrink = true;
		m_Cols[ col ].empty = true;
	}
	for( row = 0 ; row < m_NumRows ; row++ )
    {
		m_Rows[ row ].allocation = m_Rows[ row ].requisition;
		m_Rows[ row ].need_expand = false;
		m_Rows[ row ].need_shrink = true;
		m_Rows[ row ].expand = false;
		m_Rows[ row ].shrink = true;
		m_Rows[ row ].empty = true;
    }
  
	/* Loop over all the children and adjust the row and col values
	 *  based on whether the children want to be allowed to expand
	 *  or shrink. This loop handles children that occupy a single
	 *  row or column.
	 */

	for ( list< TableChild* >::iterator it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++ )
	{	
		child = (*it);
     
//		if( child->widget->IsVisible() )
		{
			if( child->left_attach == ( child->right_attach - 1 ) )
			{
				if( child->xexpand )
					m_Cols[ child->left_attach ].expand = true;
              
				if( !child->xshrink )
					m_Cols[ child->left_attach ].shrink = false;
              
				m_Cols[ child->left_attach].empty = false; 
			}
          
			if( child->top_attach == ( child->bottom_attach - 1 ) )
			{
				if( child->yexpand )
                	m_Rows[ child->top_attach ].expand = true;
              
              	if( !child->yshrink )
					m_Rows[ child->top_attach ].shrink = false;

				m_Rows[ child->top_attach ].empty = false;
            }
        }
    }
  
	/* Loop over all the children again and this time handle children
	 *  which span multiple rows or columns.
	 */
	for ( list< TableChild* >::iterator it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++ )
	{	
		child = (*it);
      
//		if( child->widget->IsVisible() )
		{
			if( child->left_attach != (child->right_attach - 1) )
			{
				for( col = child->left_attach ; col < child->right_attach ; col++ )
					m_Cols[col].empty = false;

				if( child->xexpand )
				{
					has_expand = false;
					for( col = child->left_attach ; col < child->right_attach ; col++ )
	                    if( m_Cols[col].expand )
						{
							has_expand = false;
							break;
						}
                  
					if( !has_expand )
						for( col = child->left_attach ; col < child->right_attach ; col++ )
							m_Cols[col].need_expand = true;
				}
              
              	if( !child->xshrink )
				{
					has_shrink = true;
					for( col = child->left_attach ; col < child->right_attach ; col++ )
					if( !m_Cols[ col ].shrink )
					{
						has_shrink = false;
						break;
					}
                  
					if( has_shrink )
						for( col = child->left_attach ; col < child->right_attach ; col++ )
							m_Cols[ col ].need_shrink = false;
				}
			}
          
			if( child->top_attach != (child->bottom_attach - 1) )
			{
				for( row = child->top_attach ; row < child->bottom_attach ; row++ )
					m_Rows[ row ].empty = false;

				if( child->yexpand )
				{
					has_expand = false;
                  	for( row = child->top_attach ; row < child->bottom_attach ; row++ )
					if( m_Rows[ row ].expand )
					{
						has_expand = true;
						break;
					}

					if( !has_expand )
						for( row = child->top_attach ; row < child->bottom_attach ; row++ )
							m_Rows[ row ].need_expand = true;
				}
              
				if( !child->yshrink )
				{
					has_shrink = true;
					for( row = child->top_attach ; row < child->bottom_attach ; row++ )
                    if( !m_Rows[ row ].shrink )
					{
						has_shrink = false;
						break;
					}
                  
					if( has_shrink )
						for( row = child->top_attach ; row < child->bottom_attach ; row++ )
							m_Rows[ row ].need_shrink = false;
				}
			}
		}
	}
  
	/* Loop over the columns and set the expand and shrink values
	 *  if the column can be expanded or shrunk.
	 */
	for( col = 0 ; col < m_NumCols ; col++ )
	{
		if( m_Cols[ col ].empty)
		{
			m_Cols[ col ].expand = false;
			m_Cols[ col ].shrink = false;
		}
		else
		{
			if( m_Cols[ col ].need_expand )
				m_Cols[ col ].expand = true;
			if( !m_Cols[col].need_shrink )
				m_Cols[ col ].shrink = false;
		}
	}
  
	/* Loop over the rows and set the expand and shrink values
	 *  if the row can be expanded or shrunk.
	 */
	for( row = 0; row < m_NumRows ; row++ )
	{
		if( m_Rows[ row ].empty )
		{
			m_Rows[ row ].expand = false;
			m_Rows[ row ].shrink = false;
		}
		else
		{
			if( m_Rows[ row ].need_expand )
				m_Rows[ row ].expand = true;
			if( !m_Rows[row].need_shrink )
				m_Rows[ row ].shrink = false;
		}
	}
}

void TableView :: SizeAllocatePass1()
{
	int real_width;
	int real_height;
	int width, height;
	int row, col;
	int nexpand;
	int nshrink;
	int extra;
  
	/* If we were allocated more space than we requested
	 *  then we have to expand any expandable rows and columns
	 *  to fill in the extra space.
	 */

	real_width = (int) ( GetFrame().Width());
	real_height = (int) ( GetFrame().Height() );

	if( m_Homogeneous )
	{
		if( m_ListOfChildren.size() < 1 )
			nexpand = 1;
		else
		{
			nexpand = 0;
			for( col = 0 ; col < m_NumCols; col++ )
				if( m_Cols[ col ].expand )
				{
					nexpand += 1;
					break;
				}
		}

		if( nexpand )
		{
			width = real_width;
			for( col = 0 ; col + 1 < m_NumCols; col++ )
				width -= m_Cols[ col ].spacing;
          
			for( col = 0; col < m_NumCols; col++ )
			{

				extra = width / ( m_NumCols - col);
				m_Cols[ col ].allocation = MAX( 1, extra );
				width -= extra;
			}
		}
	}
  	else
    {
		width = 0;
		nexpand = 0;
		nshrink = 0;

		for (col = 0; col < m_NumCols; col++)
		{
			width += m_Cols[ col ].requisition;
			if( m_Cols[ col ].expand )
				nexpand += 1;
			if( m_Cols[ col ].shrink)
				nshrink += 1;
		}

		for( col = 0 ; col + 1 < m_NumCols ; col++ )
			width += m_Cols[ col ].spacing;
      
		// Check to see if we were allocated more width than we requested.
		if( ( width < real_width ) && ( nexpand >= 1 ) )
		{
			width = real_width - width;
          
			for( col = 0 ; col < m_NumCols; col++ )
				if( m_Cols[ col ].expand )
				{
					extra = width / nexpand;
					m_Cols[ col ].allocation += extra;
                
					width -= extra;
					nexpand -= 1;
				}
		}
      
		/* Check to see if we were allocated less width than we requested,
		 * then shrink until we fit the size give.
		 */
		if( width > real_width )
		{
			int total_nshrink = nshrink;

			extra = width - real_width;
			while( total_nshrink > 0 && extra > 0 )
			{
				nshrink = total_nshrink;
				for( col = 0 ; col < m_NumCols ; col++ )
					if( m_Cols[ col ].shrink )
					{
						int allocation = m_Cols[ col ].allocation;

						m_Cols[ col ].allocation = MAX (1, (int) m_Cols[ col ].allocation - extra / nshrink );
						extra -= allocation - m_Cols[ col ].allocation;
						nshrink -= 1;
						if( m_Cols[ col ].allocation < 2 )
						{
							total_nshrink -= 1;
							m_Cols[ col ].shrink = false;
						}
					}
			}
		}
	}
  
	if( m_Homogeneous )
	{
		if( m_ListOfChildren.size() < 1 )
			nexpand = 1;
		else
		{
			nexpand = 0;
			for( row = 0 ; row < m_NumRows ; row++ )
			if( m_Rows[ row ].expand )
			{
				nexpand += 1;
				break;
			}
		}
      
		if( nexpand )
		{
			height = real_height;
          
			for( row = 0 ; row + 1 < m_NumRows ; row++ )
				height -= m_Rows[ row ].spacing;

			for( row = 0 ; row < m_NumRows ; row++ )
			{
				extra = height / ( m_NumRows - row );
				m_Rows[ row ].allocation = MAX(1, extra);
				height -= extra;
			}
		}
	}
  	else
    {
		height = 0;
		nexpand = 0;
		nshrink = 0;

		for( row = 0 ; row < m_NumRows; row++ )
		{
			height += m_Rows[ row ].requisition;
			if( m_Rows[ row ].expand )
				nexpand += 1;
			if( m_Rows[ row ].shrink )
				nshrink += 1;
		}

		for (row = 0; row + 1 < m_NumRows; row++)
			height += m_Rows[ row ].spacing;
      
		// Check to see if we were allocated more height than we requested.
		if( ( height < real_height ) && ( nexpand >= 1 ) )
		{
			height = real_height - height;

			for( row = 0 ; row < m_NumRows ; row++ )
			if( m_Rows[ row ].expand )
			{
				extra = height / nexpand;
				m_Rows[ row ].allocation += extra;

				height -= extra;
				nexpand -= 1;
			}
		}
      
		/* Check to see if we were allocated less height than we requested.
		 * then shrink until we fit the size give.
		 */
		if( height > real_height )
		{
			int total_nshrink = nshrink;
          
			extra = height - real_height;
			while( total_nshrink > 0 && extra > 0 )
			{
				nshrink = total_nshrink;
				for( row = 0 ; row < m_NumRows ; row++ )
					if( m_Rows[ row ].shrink )
					{
						int allocation = m_Rows[ row ].allocation;

						m_Rows[ row ].allocation = MAX(1, (int) m_Rows[ row ].allocation - extra / nshrink );
						extra -= allocation - m_Rows[ row ].allocation;
						nshrink -= 1;
						if( m_Rows[ row ].allocation < 2 )
						{
							total_nshrink -= 1;
							m_Rows[ row ].shrink = false;
						}
					}
			}
		}
	}
}

void TableView :: SizeAllocatePass2()
{
	TableChild* child;
	int max_width;
	int max_height;
	int x, y;
	int row, col;
	int allocation_x, allocation_y, allocation_width, allocation_height;
  
	for ( list< TableChild* >::iterator it = m_ListOfChildren.begin() ; it != m_ListOfChildren.end() ; it++ )
	{	
		child = (*it);
      
//		if( child->widget->IsVisible() )
		{
			Point child_requisition = child->widget->GetPreferredSize( false );

			x = (int) GetFrame().left;
			y = (int) GetFrame().top;
			max_width = 0;
			max_height = 0;
          
			for( col = 0 ; col < child->left_attach ; col++ )
			{
				x += m_Cols[ col ].allocation;
				x += m_Cols[ col ].spacing;
			}
          
			for( col = child->left_attach ; col < child->right_attach ; col++ )
			{
				max_width += m_Cols[ col ].allocation;
				if( ( col + 1 ) < child->right_attach )
					max_width += m_Cols[ col ].spacing;
            }

			for( row = 0 ; row < child->top_attach ; row++ )
			{
				y += m_Rows[ row ].allocation;
				y += m_Rows[ row ].spacing;
			}

			for(row = child->top_attach ; row < child->bottom_attach ; row++ )
			{
				max_height += m_Rows[ row ].allocation;
				if( ( row + 1 ) < child->bottom_attach )
					max_height += m_Rows[ row ].spacing;
			}

			if( child->xfill )
			{
				allocation_width = MAX(1, max_width - (int)child->xpadding * 2);
				allocation_x = x + ( max_width - allocation_width ) / 2;
			}
			else
			{
				allocation_width = (int) child_requisition.x;
				allocation_x = x + ( max_width - allocation_width ) / 2;
			}

			if( child->yfill )
			{
				allocation_height = MAX(1, max_height - (int)child->ypadding * 2);
				allocation_y = y + ( max_height - allocation_height ) / 2;
			}
			else
			{
				allocation_height = (int) child_requisition.y;
				allocation_y = y + ( max_height - allocation_height ) / 2;
			}

			child->widget->SetFrame( Rect( allocation_x, allocation_y, allocation_x + allocation_width, allocation_y + allocation_height ) );
		}
	}
}


