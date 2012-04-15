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

#ifndef WHISPER_MESSAGE_VIEW_H_
#define WHISPER_MESSAGE_VIEW_H_

#include <gui/view.h>
#include <gui/control.h>
#include <gui/rect.h>
#include <gui/scrollbar.h>
#include <gui/stringview.h>
#include <gui/image.h>
#include <gui/imageview.h>
#include <gui/menu.h>
#include <gui/filerequester.h>
#include <gui/button.h>
#include <util/string.h>
#include <util/message.h>
#include <pyro/time.h>

#include <mail.h>
#include <icon.h>

#include <vector>
#include <list>

using namespace os;

class ChildView : public View
{
	public:
		ChildView( Rect cFrame, char *pzText, uint32 nReziseMask ) : View( cFrame, pzText, nReziseMask ){};
		~ChildView(){};
};

class FlowView : public ChildView
{
	public:
		FlowView( Rect cFrame, char *pzText, const Settings *pcVisualSettings );
		~FlowView();

		virtual void AttachedToWindow( void );
	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void FrameSized( const Point &cDelta );
		virtual void Paint( const Rect &cUpdateRect );

		virtual void HandleMessage( Message *pcMessage );

		virtual void MouseMove( const Point &cNewPos, int nCode, uint32 nButtons, Message *pcData );
		virtual void MouseDown( const Point &cPosition, uint32 nButtons );
		virtual void MouseUp( const Point &cPosition, uint32 nButtons, Message *pcData );

	private:
		void _OpenMenu( const Point &cPosition );
		void _EnableEdit( bool bEnable = true );

		char *m_pzText;
		Point m_cSize;

		bool m_bIBeamActive;
		bool m_bSawClick;

		bool m_bSelect;
		IPoint m_cSel1, m_cSel2;
		IPoint m_cOldSel;

		Menu *m_pcMenu;
		MenuItem *m_pcCopy;
		Messenger *m_pcMessenger;
};

class MessageView;

class InlineView : public ChildView
{
	public:
		InlineView( Rect cFrame, Multipart &cPart, const Handler *pcAppWindow, const Settings *pcVisualSettings );
		~InlineView();

		virtual void AttachedToWindow( void );
		void AllAttached();
	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void FrameSized( const Point &cDelta );
		virtual void Paint( const Rect &cUpdateRect );
		virtual void HandleMessage( Message *pcMessage );

	private:
		void _Layout( void );
		size_t _Decode( Multipart *pcPart, char **ppDecoded );

		Multipart m_cPart;

		Point m_cSize, m_cHeaderSize;

		StringView *m_pcAttachmentView;
		StringView *m_pcFilenameView;

		Button *m_pcSaveButton;

		FlowView *m_pcInlineText;
		ImageView *m_pcInlineImage;
		MessageView *m_pcInlineMessage;

		bool m_bDisplay;
};

enum signed_modes
{
	SIGNED_OK,
	SIGNED_UNKNOWN
};

class SignedView : public ChildView
{
	public:
		SignedView( Rect cFrame, Multipart &cPart, int nMode, const Handler *pcAppWindow, const Settings *pcVisualSettings );
		~SignedView();

		virtual void AttachedToWindow( void );
		void AllAttached();
	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void FrameSized( const Point &cDelta );
		virtual void Paint( const Rect &cUpdateRect );

	private:
		void _Layout( void );
		size_t _Decode( Multipart *pcPart, char **ppDecoded );

		Multipart m_cPart;
		int m_nMode;

		Point m_cSize, m_cHeaderSize;

		StringView *m_pcSignedView;
		StringView *m_pcKeyView;

		FlowView *m_pcInlineText;
		MessageView *m_pcInlineMessage;

		bool m_bDisplay;
};

class AttachmentView : public ChildView
{
	public:
		AttachmentView( Rect cFrame );
		~AttachmentView();

		virtual void AttachedToWindow( void );
		virtual void MouseMove( const Point &cNewPos, int nCode, uint32 nButtons, Message *pcData );
		virtual void MouseDown( const Point &cPosition, uint32 nButtons );
		virtual void MouseUp( const Point& cPosition, uint32 nButtons, Message* pcData );

	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void FrameSized( const Point &cDelta );
		virtual void Paint( const Rect &cUpdateRect );
		virtual void HandleMessage( Message *pcMessage );

		virtual void AddIcon( Image *pcImage, String cName, uint64 nPartId );
	private:
		void _Layout( void );
		void _AddToSelection( int nIndex );
		void _Select( int nIndex );
		void _OpenMenu( const Point &cPosition );

		class AttachmentIcon : public Icon
		{
			public:
				AttachmentIcon( Rect cFrame, Image *pcImage, String cName, uint64 nPartId );

				uint64 GetId( void )
				{
					return m_nPartId;
				};
				bigtime_t GetSelectionTime( void )
				{
					return m_nSelectionTime;
				};
				void SetSelectionTime( bigtime_t nSelectionTime )
				{
					m_nSelectionTime = nSelectionTime;
				};
			private:
				uint64 m_nPartId;
				bigtime_t m_nSelectionTime;
		};

		std::vector <AttachmentIcon *> m_vAttachments;
		std::list <int> m_vSelected;

		Point m_cSize;
		Rect m_cParentBounds;

		Menu *m_pcMenu;
		Messenger *m_pcMessenger;

		bool m_bMouseDown;
		bool m_bDragging;
};

enum message_view_messages
{
	ID_MV_SCROLL_V = 1,
	ID_ATTACHMENT_SAVE,
	ID_ATTACHMENT_OPEN,
	ID_ATTACHMENT_BEGIN_DRAG,
	ID_EDIT_COPY
};

enum message_view_timers
{
	AUTOSCROLL_TIMER
};

enum scroll_direction
{
	AUTOSCROLL_NONE,
	AUTOSCROLL_UP,
	AUTOSCROLL_DOWN
};

class MessageView : public View
{
	public:
		MessageView( Rect cFrame, const char *pzName, const Handler *pcAppWindow, const Settings *pcVisualSettings );
		~MessageView();

	    virtual Point GetPreferredSize( bool bLargest ) const;
		virtual void FrameSized( const Point &cDelta );
		virtual void Paint( const Rect &cUpdateRect );
		virtual void WheelMoved( const Point &cDelta );

		virtual void MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData );
		virtual void MouseUp( const Point &cPosition, uint32 nButtons, Message *pcData );

		virtual void TimerTick( int nId );

		status_t Display( Mailmessage *pcMessage );

		void Clear( void );

		void Copy( void );
	private:
		class InnerView : public View
		{
			public:
				InnerView( Rect cFrame, const Handler *pcAppWindow, const Settings *pcVisualSettings );
				~InnerView();

				void AttachedToWindow( const View *pcParent );

			    virtual Point GetPreferredSize( bool bLargest ) const;
				virtual void FrameSized( const Point &cDelta );
				virtual void Paint( const Rect &cUpdateRect );
				virtual void HandleMessage( Message *pcMessage );

				virtual void MouseMove( const Point& cNewPos, int nCode, uint32 nButtons, Message* pcData );

				void Display( Mailmessage *pcMessage );

				void AddBody( Multipart &cPart );
				void AddAttachment( Multipart &cPart );
				void AddInline( Multipart &cPart );
				void AddSigned( Multipart &cPart );
				void AddMultipart( Multipart &cPart );

				void Clear( void );

				void Copy( void );

			private:
				void _Layout( void );
				void _SaveAttachment( uint64 nId );
				void _DoSaveAttachment( uint64 nId, const char *pzFilename );
				void _OpenAttachment( uint64 nId );
				status_t _SaveAttachmentTmp( uint64 nId, os::String &cFilename );

				Point m_cSize;
				float m_vContentHeight, m_vDefualtFontHeight;
				Rect m_vSeperatorFrame;

				StringView *m_pcSubjectView;
				StringView *m_pcFromView;
				StringView *m_pcToView;

				Mailmessage *m_pcMessage;

				bool m_bDrawSeperator;
				bool m_bIBeamActive;

				/* XXXKV: Replace with a list? */
				std::vector <ChildView *> m_vViews;

				/* We only need one AttachmentView per. message */
				AttachmentView *m_pcAttachmentView;

				const Settings *m_pcVisualSettings;

				const Handler *m_pcAppWindow;
				Messenger *m_pcMessenger;
		};

		InnerView *m_pcInnerView;
		ScrollBar *m_pcVScrollBar;

		scroll_direction m_eScrollDirection;

		void _Layout();

		void _StartScroll( scroll_direction eDirection );
		void _StopScroll( void );
};

#define MV_SCROLL_WIDTH		16

#endif

