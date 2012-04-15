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

#ifndef WHISPER_DIALOGS_H_
#define WHISPER_DIALOGS_H_

#include <gui/window.h>
#include <gui/textview.h>
#include <gui/layoutview.h>
#include <gui/stringview.h>
#include <gui/progressbar.h>
#include <gui/checkbox.h>
#include <util/message.h>
#include <util/string.h>

#include <mailbox.h>

using namespace os;

enum dialog_messages
{
	ID_DIALOG_OK = 0,
	ID_DIALOG_CANCEL,
	ID_DIALOG_PRIVATE = 100
};

class FolderNameDialog : public Window
{
	public:
		FolderNameDialog( const Rect &cFrame, String cTitle, String cFolderName );
		~FolderNameDialog();

		void Go( Invoker *pcInvoker );
		void HandleMessage( Message *pcMessage );
	private:
		Invoker *m_pcInvoker;

		LayoutView *m_pcLayoutView;
		TextView *m_pcTextView;
};

class IXPortProgressDialog : public Window
{
	public:
		IXPortProgressDialog( const Rect &cFrame, String cTitle );
		~IXPortProgressDialog();

		void Go( Invoker *pcInvoker );
		void HandleMessage( Message *pcMessage );

		void SetMessage( String cMessage ){ m_pcStringView->SetString( cMessage ); };
		void SetProgress( float vValue ){ m_pcProgressBar->SetProgress( vValue ); };

		bool IsCancelled( void ){ return m_bCancelled; };

	private:
		Invoker *m_pcInvoker;

		bool m_bCancelled;

		LayoutView *m_pcLayoutView;
		StringView *m_pcStringView;
		ProgressBar *m_pcProgressBar;
};

enum properties_dialog_messages
{
	ID_PROPERTIES_DIALOG_CHECKBOX = ID_DIALOG_PRIVATE
};


class PropertiesDialog : public Window
{
	public:
		PropertiesDialog( const Rect &cFrame, String cTitle, FolderProperties *pcProperties );
 		~PropertiesDialog();

		void Go( Invoker *pcInvoker );
		void HandleMessage( Message *pcMessage );

	private:
		Invoker *m_pcInvoker;
		FolderProperties *m_pcProperties;

		LayoutView *m_pcLayoutView;
		CheckBox *m_pcCheckBox;
		TextView *m_pcTextView;
};


#endif

