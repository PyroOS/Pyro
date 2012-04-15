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

#ifndef WHISPER_SYLLABLE__SETTINGS_WINDOW_H_
#define WHISPER_SYLLABLE__SETTINGS_WINDOW_H_

#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/frameview.h>
#include <util/string.h>
#include <util/settings.h>
#include <util/message.h>

#include <icon.h>
#include <identity.h>
#include <inputview.h>
#include <settings_views.h>

#include <vector>

using namespace os;

class IconList : public View
{
	public:
		IconList( Rect cFrame, int nFlags, Handler *pcHandler );
		~IconList();

		virtual void AttachedToWindow( void );
	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void Paint( const Rect &cUpdateRect );
		virtual void WheelMoved( const Point &cDelta );
		virtual void FrameSized( const Point &cDelta );
		virtual void HandleMessage( Message *pcMessage );

		virtual void AddIcon( Image *pcImage, String cName );
		virtual status_t Select( int nIndex, bool bNotify = true );

	private:
		class InnerView : public View
		{
			public:
				InnerView( Rect cFrame );
				~InnerView();

				virtual void AttachedToWindow( void );
				virtual void MouseDown( const Point &cPosition, uint32 nButtons );
			    virtual Point GetPreferredSize( bool bLargest ) const;
				virtual void Paint( const Rect &cUpdateRect );
				virtual void FrameSized( const Point &cDelta );
				virtual void HandleMessage( Message *pcMessage );

				virtual void AddIcon( Image *pcImage, String cName );

				status_t Select( int nIndex, bool bNotify = true );

			private:
				void _Layout( void );

				std::vector <Icon *> m_vIcons;
				int32 m_nSelected;
				Point m_cSize;
		};

		void _Layout( void );

		Handler *m_pcHandler;

		InnerView *m_pcInnerView;
		ScrollBar *m_pcVScrollBar;
};

class SettingsWindow : public Window
{
	public:
		SettingsWindow( const Rect &cFrame, const String cTitle, Handler *pcParent, Settings *pcGuiSettings, Identity *pcIdentity );
		~SettingsWindow();

		void FrameSized( const Point &cDelta );

		void HandleMessage( Message *pcMessage );
		bool OkToQuit( void );

	private:
		void _Save( int nCode );
		void _SaveGui( void );

		Handler *m_pcParent;
		Settings *m_pcGuiSettings;

		LayoutView *m_pcLayoutView;
		IconList *m_pcIconList;
		FrameView *m_pcFrameView;

		std::vector<SettingsView*> m_vViews;
		SettingsView *m_pcSelected;
};

#endif
