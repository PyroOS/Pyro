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

#ifndef WHISPER_SYLLABLE__SETTINGS_VIEWS_H_
#define WHISPER_SYLLABLE__SETTINGS_VIEWS_H_

#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/frameview.h>
#include <gui/scrollbar.h>
#include <gui/textview.h>
#include <gui/checkbox.h>
#include <gui/dropdownmenu.h>
#include <gui/listview.h>
#include <gui/button.h>
#include <gui/spinner.h>
#include <gui/tabview.h>
#include <util/string.h>
#include <util/settings.h>
#include <util/message.h>

#include <identity.h>
#include <inputview.h>

#include <vector>

#define IL_SCROLL_WIDTH		16

class SettingsView : public View
{
	public:
		SettingsView( Rect cFrame, String cTitle, Identity *pcIdentity, Handler *pcMainWindow = NULL ) : View( cFrame, cTitle, CF_FOLLOW_ALL ){};
		virtual ~SettingsView(){};

		virtual void AttachedToWindow(){View::AttachedToWindow();};
		virtual void HandleMessage( Message *pcMessage ){View::HandleMessage( pcMessage );};

		virtual status_t Store( Identity *pcIdentity ){return ENOSYS;};
		virtual status_t Update( Identity *pcIdentity ){return ENOSYS;};

		virtual status_t Save( void ){return ENOSYS;};
		virtual status_t Apply( void ){return ENOSYS;};
		virtual status_t Cancel( void ){return ENOSYS;};
};

/* Views that will be attached within a TabView I.e. for Filters */
class SettingsTab : public View
{
	public:
		SettingsTab( Rect cFrame, String cName, Identity *pcIdentity, Handler *pcMainWindow = NULL ) : View( cFrame, cName, CF_FOLLOW_ALL ){};
		virtual ~SettingsTab(){};

		virtual void AttachedToWindow(){View::AttachedToWindow();};
		virtual void HandleMessage( Message *pcMessage ){View::HandleMessage( pcMessage );};

		virtual status_t Store( Identity *pcIdentity ){return ENOSYS;};
		virtual status_t Update( Identity *pcIdentity ){return ENOSYS;};

		virtual status_t Save( void ){return ENOSYS;};
		virtual status_t Apply( void ){return ENOSYS;};
		virtual status_t Cancel( void ){return ENOSYS;};
};

#define INPUT_HEIGHT	22

class IdentityView : public SettingsView
{
	public:
		IdentityView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow );
		~IdentityView();

		status_t Store( Identity *pcIdentity );

	private:
		Identity *m_pcIdentity;

		View *m_pcIdentityView;

		InputView *m_pcNameInput;
		InputView *m_pcEmailInput;
};

enum signature_messages
{
	ID_SIGNATURE_SAVE,
	ID_SIGNATURE_NEW,
	ID_SIGNATURE_DELETE,
	ID_SIGNATURE_SELECT
};

class SignatureView : public SettingsView
{
	public:
		SignatureView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow );
		~SignatureView();

		void AllAttached();
		void HandleMessage( Message *pcMessage );

		status_t Store( Identity *pcIdentity );

	private:
		Identity *m_pcIdentity;

		View *m_pcSignatureView;

		InputView *m_pcTitleInput;
		InputView *m_pcTextInput;

		Button *m_pcSave;

		LayoutView *m_pcLayoutView;
		VLayoutNode *m_pcLayoutRoot;

		HLayoutNode *m_pcInstancesLayout;

		ListView *m_pcInstances;
		VLayoutNode *m_pcButtonsLayout;
		Button *m_pcNewInstance;
		Button *m_pcDeleteInstance;

		std::vector <std::pair <os::String, os::String> > m_vSignatures;

		bool m_bNew;
};

enum outbound_messages
{
	ID_REQUIRES_AUTH,
	ID_POP3_AUTH
};

class OutboundView : public SettingsView
{
	public:
		OutboundView( Rect cFrame,Identity *pcIdentity, Handler *pcMainWindow );
		~OutboundView();

		void AttachedToWindow();
		void HandleMessage( Message *pcMessage );

		status_t Store( Identity *pcIdentity );
		status_t Update( Identity *pcIdentity );

	private:
		Identity *m_pcIdentity;

		View *m_pcOutboundView;

		InputView *m_pcServerInput;
		CheckBox *m_pcRequiresAuth;
		InputView *m_pcUsernameInput;
		InputView *m_pcPasswordInput;
		Spinner *m_pcPort;
		CheckBox *m_pcPopBeforeSmtp;
		DropdownMenu *m_pcPop3Account;
};

enum inbound_messages
{
	ID_INBOUND_SAVE,
	ID_INBOUND_NEW,
	ID_INBOUND_DELETE,
	ID_INBOUND_SELECT,
	ID_INBOUND_DELETE_MAIL
};

class InboundView : public SettingsView
{
	public:
		InboundView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow );
		~InboundView();

		void AllAttached();
		void HandleMessage( Message *pcMessage );

		status_t Store( Identity *pcIdentity );

	private:
		Identity *m_pcIdentity;

		View *m_pcPop3View;

		InputView *m_pcServerInput;
		InputView *m_pcUsernameInput;
		InputView *m_pcPasswordInput;

		Spinner *m_pcPort;
		Button *m_pcSave;
		CheckBox *m_pcDeleteMail;

		LayoutView *m_pcLayoutView;
		VLayoutNode *m_pcLayoutRoot;

		HLayoutNode *m_pcInstancesLayout;

		ListView *m_pcInstances;
		VLayoutNode *m_pcButtonsLayout;
		Button *m_pcNewInstance;
		Button *m_pcDeleteInstance;

		std::vector <Server> m_vServers;

		bool m_bNew;
};

class FiltersView : public SettingsView
{
	public:
		FiltersView( Rect cFrame, Identity *pcIdentity, Handler *pcMainWindow );
		~FiltersView();

		status_t Store( Identity *pcIdentity );

		status_t Save( void );
	private:
		Identity *m_pcIdentity;

		TabView *m_pcFiltersTabView;
};

#endif

