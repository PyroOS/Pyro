/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */

#include <messagenode.h>

#include <typeinfo>

/* XXXKV: This all needs to be re-thought.  A TreeViewMessageNode is currently only useful
   when it's used in a multi-column ListView or TreeView, with no indenting.  This means we
   can't create a threaded or grouped messages view.  Nor can we use TreeViewMessageNode in
   the folders list (Using a standard TreeViewStringNode in the folders list works but
   there is no way to change the font E.g. to make it bold when there are unread messages
   in that folder)

   Right now it's pretty obvious that TreeView isn't much use as it is, other than for
   very simple trees.  In our case we need far more functionality that TreeView & ListView
   are capable of; we've managed to work around some of the simplier limitations with
   this custom class but it's really the tip of the iceberg.

   We'll need a custom pair of treeview and treeview-row.  The use of multiple columns
   will have to be droped (What's a column when the row is indented?) so we'll need
   a different way to paint each row, retaining the concept of mixed icons and
   strings.  The *concept* of a column I.e. each individual icon & string that makes up a
   row can have a fixed width, should remain, as should the idea of an empty "column"
   E.g. the first and third icons may be missing but the string should still be rendered
   at the same position as if the icons were there.

   Once we're able to build a complex "multi-column" row as a single item, building a
   Tree will be much simplier.  However there will still need to be some way to attach
   child nodes to a parent; the current method requires us to know exactly which row
   to insert the "child" at as well as the exact indent, and there is no explicit
   relationship between the two.

   We may want to rethink the entire ListView model and make each row an individual
   View in it's own right, which would also make changing the colour, font etc.
   on individual rows easier.
*/

TreeViewMessageNode::TreeViewMessageNode()
{
	m_nTextFlags = DTF_IGNORE_FMT;
	m_pcFont = NULL;
}

TreeViewMessageNode::~TreeViewMessageNode()
{
	std::vector<MessageViewColumn *>::iterator i;
	for( i = m_vColumns.begin(); i != m_vColumns.end(); i++ )
		delete (*i);

	if( m_pcFont )
		m_pcFont->Release();
}

void TreeViewMessageNode::AttachToView( View* pcView, int nColumn )
{
	if( m_vColumns[nColumn]->m_pcImage != NULL )
	{
		m_vColumns[nColumn]->m_vWidth = m_vColumns[nColumn]->m_pcImage->GetSize().x + 5;
	}
	else
	{
		Point cSize = pcView->GetTextExtent( m_vColumns[nColumn]->m_cString );
		m_vColumns[nColumn]->m_vWidth = cSize.x + 5;
	}
}

void TreeViewMessageNode::SetRect( const Rect& cRect, int nColumn )
{
}

float TreeViewMessageNode::GetWidth( View* pcView, int nColumn )
{
	return( m_vColumns[nColumn]->m_vWidth );
}

float TreeViewMessageNode::GetHeight( View* pcView )
{
	Point cSize( 0, 0 );

	std::vector<MessageViewColumn *>::iterator i;
	for( i = m_vColumns.begin(); i != m_vColumns.end(); i++ )
	{
		if( (*i)->m_pcImage != NULL )
		{
			Point s = (*i)->m_pcImage->GetSize();
			if( s.y > cSize.y )
				cSize.y = s.y;
		}
		else
		{
			Point s = pcView->GetTextExtent( (*i)->m_cString, m_nTextFlags );
			if( s.y > cSize.y )
				cSize.y = s.y;
		}
	}

    return cSize.y;
}

void TreeViewMessageNode::Paint( const Rect& cFrame, View* pcView, uint nColumn, bool bSelected, bool bHighlighted, bool bHasFocus )
{
	TreeView *pcOwner = GetOwner();
	uint nIndentWidth = 10;
	bool bExp = false;

	if( pcOwner )
	{
		nIndentWidth = pcOwner->GetIndentWidth();
		bExp = pcOwner->HasChildren( this );	// FIXME: slow!
	}

	uint nIndent = GetIndent() * nIndentWidth;
	Rect cItemRect( cFrame );

	if( nIndent && !nColumn )
	{
		cItemRect.left += nIndent + 15;
		cItemRect.right += nIndent + 15;
	}
	else
		nIndent = 0;

	float vOffset = cItemRect.left + 3.0f;

	pcView->SetDrawingMode( DM_COPY );
	pcView->SetFgColor( 255, 255, 255 );
	pcView->FillRect( cFrame );
	
	if( bSelected || bHighlighted )
	{
		Rect cSelectFrame = cFrame;
		cSelectFrame.left = cItemRect.left;
		if( nColumn == 0 )
		{
			cSelectFrame.left += 2;
			cSelectFrame.top += 2;
			cSelectFrame.bottom -= 2;
		}
		if( bSelected )
			pcView->SetFgColor( 186, 199, 227 );
		else
			pcView->SetFgColor( 0, 50, 200 );
		pcView->FillRect( cSelectFrame );
	
		/* Round edges */
		if( nColumn == 0 )
		{
			pcView->DrawLine( os::Point( cSelectFrame.left + 2, cSelectFrame.top - 2 ), 
								os::Point( cSelectFrame.right, cSelectFrame.top - 2 ) );
			pcView->DrawLine( os::Point( cSelectFrame.left, cSelectFrame.top - 1 ), 
								os::Point( cSelectFrame.right, cSelectFrame.top - 1 ) );
			
			pcView->DrawLine( os::Point( cSelectFrame.left - 2, cSelectFrame.top + 2 ), 
								os::Point( cSelectFrame.left - 2, cSelectFrame.bottom - 2 ) );
			pcView->DrawLine( os::Point( cSelectFrame.left - 1, cSelectFrame.top ), 
								os::Point( cSelectFrame.left - 1, cSelectFrame.bottom ) );
								
			pcView->DrawLine( os::Point( cSelectFrame.left + 2, cSelectFrame.bottom + 2 ), 
								os::Point( cSelectFrame.right, cSelectFrame.bottom + 2 ) );
			pcView->DrawLine( os::Point( cSelectFrame.left, cSelectFrame.bottom + 1 ), 
								os::Point( cSelectFrame.right, cSelectFrame.bottom + 1 ) );
		} 
	}
 
	if( bHighlighted )
	{
		pcView->SetFgColor( 255, 255, 255 );
		pcView->SetBgColor( 0, 50, 200 );
	}
	else if( bSelected )
	{
		pcView->SetFgColor( 0, 0, 0 );
		pcView->SetBgColor( 186, 199, 227 );
	}
	else
	{
		pcView->SetBgColor( 255, 255, 255 );
		pcView->SetFgColor( 0, 0, 0 );
	}

	if( nColumn <= m_vColumns.size() )
	{
		Image *pcImage = GetIcon( nColumn );
		if( pcImage != NULL )
		{
			/* Draw Image */
			float vHeight = pcImage->GetSize().y;

			pcView->SetDrawingMode( DM_BLEND );
			pcImage->Draw( Point( cItemRect.left + 2, cItemRect.top + (cItemRect.Height() / 2 - vHeight / 2) ), pcView );
		}
		else
		{
			Font *pcOldFont = NULL;

			/* Draw String */
			if( m_pcFont != NULL )
			{
				pcOldFont = pcView->GetFont();
				pcOldFont->AddRef();
				pcView->SetFont( m_pcFont );
			}

			Rect cTextRect( vOffset, cItemRect.top, cItemRect.right, cItemRect.bottom );
			pcView->DrawText( cTextRect, GetString( nColumn ), m_nTextFlags );
			pcView->Flush();

			if( pcOldFont != NULL )
			{
				pcView->SetFont( pcOldFont );
				pcOldFont->Release();
				pcView->Flush();
			}
		}
	}

	/* Draw Trunk (a.k.a connecting lines) */
	if( nColumn == 0 && pcOwner && pcOwner->GetDrawTrunk() )
	{
		pcView->SetFgColor( 0, 0, 0 );

		uint32 bits = _GetLinePositions();
		int i = 0;
		for( ; bits ; bits >>= 1, i++ )
		{
			if( bits & 1 )
			{
				if( !( bits >> 1 ) && IsLastSibling() )
				{
					pcView->DrawLine( Point( cFrame.left + ( i + .5 ) * (float)nIndentWidth, cFrame.top ),
									  Point( cFrame.left + ( i + .5 ) * (float)nIndentWidth, cFrame.top + cFrame.Height()/2 ) );
				}
				else
				{
					pcView->DrawLine( Point( cFrame.left + ( i + .5 ) * (float)nIndentWidth, cFrame.top ),
									  Point( cFrame.left + ( i + .5 ) * (float)nIndentWidth, cFrame.bottom ) );
				}
			}
		}
		
		if( i )
		{
			if( bExp )
			{
				pcView->DrawLine( Point( cFrame.left + ( i - .5 ) * (float)nIndentWidth, cFrame.top + cFrame.Height()/2 ),
								  Point( _ExpanderCrossPos( cFrame ).left-1, cFrame.top + cFrame.Height()/2 ) );
			}
			else
			{
				pcView->DrawLine( Point( cFrame.left + ( i - .5 ) * (float)nIndentWidth, cFrame.top + cFrame.Height()/2 ),
								  Point( cItemRect.left, cFrame.top + cFrame.Height()/2 ) );
			}
		}
	}

	if( nIndent && bExp )
	{
		Rect cExpCr( _ExpanderCrossPos( cFrame ) );

	    _DrawExpanderCross( pcView, cExpCr + Point( cFrame.left, cFrame.top ) );

		if( IsExpanded() && pcOwner && pcOwner->GetDrawTrunk() )
		{
			pcView->DrawLine( Point( nIndent + cFrame.left + nIndentWidth/2, cExpCr.bottom + cFrame.top ),
							  Point( nIndent + cFrame.left + nIndentWidth/2, cFrame.bottom ) );
		}
	}
}

#include <rfctime.h>
#include <debug.h>
#include <resources/Whisper.h>

bool TreeViewMessageNode::IsLessThan( const ListViewRow* pcOther, uint nColumn ) const
{
	bool bRet = true;

	if( typeid( *pcOther ) == typeid( *this ) )
	{
		/* XXXKV: This is a bit hokus-pokus. We're essentially hard coding the number, order and type of the
		   columns directly into the logic of the row itself. If the row configuration ever changes, this will
		   cease working and will also need to be updated. There doesn't appear to be any other way to do it
		   and still get the functionality I'm after, though. */

		switch( nColumn )
		{
			case 0: /* Flag */
			{
				/* It would be nice to sort on the flag itself, but this class has no visibility of what the status
				   actually is: just that it is an Image. So we'll fall through and just sort on the basis of "Is there
				   and icon?" */

				/* FALL THROUGH */
			}

			case 1:	/* Status */
			case 2:	/* Attachment */
			{
				bRet = ( m_vColumns[nColumn]->m_pcImage < ((TreeViewMessageNode*)pcOther)->m_vColumns[nColumn]->m_pcImage );
				break;
			}

			case 3:	/* Subject */
			case 4:	/* Sender */
			case 5: /* Date. It would be nice to properly sort by date, but we'd have to convert the String back to a date etc. */
			{
				bRet = ( GetString( nColumn ) < ((TreeViewMessageNode*)pcOther)->GetString( nColumn ) );
				break;
			}
		}
	}

	return bRet;
}

void TreeViewMessageNode::AppendString( const String& cString )
{
	MessageViewColumn *pcColumn = new MessageViewColumn();

	pcColumn->m_cString = cString;
	m_vColumns.push_back( pcColumn );
}

void TreeViewMessageNode::SetString( int nIndex, const String& cString )
{
	if( m_vColumns[nIndex]->m_pcImage != NULL )
		delete( m_vColumns[nIndex]->m_pcImage );

	m_vColumns[nIndex]->m_pcImage = NULL;
	m_vColumns[nIndex]->m_cString = cString;
}

const String& TreeViewMessageNode::GetString( int nIndex ) const
{
	return m_vColumns[nIndex]->m_cString;
}

void TreeViewMessageNode::AppendIcon( Image *pcIcon )
{
	MessageViewColumn *pcColumn = new MessageViewColumn();

	pcColumn->m_pcImage = pcIcon;
	m_vColumns.push_back( pcColumn );
}

void TreeViewMessageNode::SetIcon( int nIndex, Image *pcIcon )
{
	if( m_vColumns[nIndex]->m_pcImage != NULL )
		delete( m_vColumns[nIndex]->m_pcImage );

	m_vColumns[nIndex]->m_pcImage = pcIcon;
	m_vColumns[nIndex]->m_cString = "";
}

Image * TreeViewMessageNode::GetIcon( int nIndex )
{
	return m_vColumns[nIndex]->m_pcImage;
}

void TreeViewMessageNode::SetFont( Font *pcFont )
{
	m_pcFont = pcFont;
}

uint32 TreeViewMessageNode::GetTextFlags() const
{
	return m_nTextFlags;
}

void TreeViewMessageNode::SetTextFlags( uint32 nTextFlags )
{
	m_nTextFlags = nTextFlags;
}

