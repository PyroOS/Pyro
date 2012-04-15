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

#ifndef WHISPER_SYLLABLE__MESSAGENODE_H_
#define WHISPER_SYLLABLE__MESSAGENODE_H_

#include <gui/treeview.h>
#include <gui/font.h>

#include <vector>

using namespace os;

class MessageViewColumn
{
	public:
		MessageViewColumn()
		{
			m_pcImage = NULL;
			m_cString = "";
			m_vWidth = 0.0f;
		};
		~MessageViewColumn()
		{
			if( m_pcImage )
				delete( m_pcImage );
		};

		Image *m_pcImage;
		String m_cString;
		float m_vWidth;
};

class TreeViewMessageNode : public TreeViewNode
{
	public:
		TreeViewMessageNode();
		virtual ~TreeViewMessageNode();

		void AttachToView( View* pcView, int nColumn );
		void SetRect( const Rect& cRect, int nColumn );
		virtual float GetWidth( View* pcView, int nColumn );
		virtual float GetHeight( View* pcView );
		virtual void Paint( const Rect& cFrame, View* pcView, uint nColumn, bool bSelected, bool bHighlighted, bool bHasFocus );
		virtual bool IsLessThan( const ListViewRow* pcOther, uint nColumn ) const;

		void AppendString( const String& cString );
		void SetString( int nIndex, const String& cString );
		const String& GetString( int nIndex ) const;

		void AppendIcon( Image *pcIcon );
		void SetIcon( int nIndex, Image *pcIcon );
		Image * GetIcon( int nIndex );

		void SetFont( Font *pcFont );

		uint32 GetTextFlags() const;
		void SetTextFlags( uint32 nTextFlags );

	private:
		std::vector<MessageViewColumn *> m_vColumns;
		uint32 m_nTextFlags;
		Font *m_pcFont;
};

#endif

