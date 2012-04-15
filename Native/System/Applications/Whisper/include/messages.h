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

#ifndef WHISPER_MESSAGES_H_
#define WHISPER_MESSAGES_H_

enum messages
{
	M_SELECT = 1,
	M_FOLDER_SELECT,
	M_CREATE_FOLDER,
	M_CREATE_FOLDER_DIALOG,
	M_RENAME_FOLDER,
	M_RENAME_FOLDER_DIALOG,
	M_DELETE_FOLDER,
	M_PROPERTIES_FOLDER,
	M_PROPERTIES_FOLDER_DIALOG,
	M_POST_FOLDER,
	M_SELECT_ALL_MESSAGES,
	M_CUT_MESSAGE,
	M_COPY_MESSAGE,
	M_PASTE_MESSAGE,
	M_DELETE_MESSAGE,
	M_FLAG_MESSAGE,
	M_MARK_MESSAGE,
	M_VIEW_MESSAGE,
	M_VIEW_MESSAGE_DATA,
	M_ALERT_DONE,
	M_STATUS_UPDATE,
	M_STATUS_CLEAR,
	M_CHECK_FOR_MAIL,
	M_NEW_MAIL,
	M_NEW_MAIL_COMPLETE,
	M_COMPOSE_MESSAGE,
	M_COMPOSE_MESSAGE_COMPLETE,
	M_COMPOSE_REPLY,
	M_COMPOSE_REPLY_ALL,
	M_COMPOSE_FORWARD,
	M_MAIL_SENT,
	M_SETTINGS_CONFIGURE,
	M_SETTINGS_CONFIGURE_APPLY,
	M_SETTINGS_CONFIGURE_SAVE,
	M_SETTINGS_SAVE_OC,
	M_SETTINGS_SAVE_NOW,
	M_APP_QUIT,
	M_APP_ABOUT,
	M_DO_IMPORT,
	M_IMPORT_FILE,
	M_DO_EXPORT,
	M_EXPORT_FILE,
	M_IMPORT_NEW,
	M_IMPORT_CREATE_FOLDER,
	M_IMPORT_COMPLETE,
	M_IXPORT_CANCEL,
	M_EMPTY_TRASH,
	M_EDIT_COPY,
	M_ENABLE_EDIT_COPY,
	M_GET_FOLDER_LIST		/* Used by the Rule Filter */
};

enum events
{
	M_FIRST_EVENT = 4096,
	M_MAIL_SEND,			/* internet/Mail/Send */
	M_MAIL_GET_COUNT,		/* internet/Mail/GetNewCount */
	M_MAIL_CHECK			/* internet/Mail/Check */
};

enum composer_messages
{
	M_ADDR_LOOKUP_TO,
	M_ADDR_LOOKUP_CC,
	M_ADDR_LOOKUP_BCC,
	M_COMPOSER_MESSAGE_SEND,
	M_COMPOSER_MESSAGE_SAVE,
	M_COMPOSER_MESSAGE_CLOSE,
	M_COMPOSER_EDIT_CUT,
	M_COMPOSER_EDIT_COPY,
	M_COMPOSER_EDIT_PASTE,
	M_COMPOSER_VIEW_TO,
	M_COMPOSER_VIEW_CC,
	M_COMPOSER_VIEW_BCC,
	M_COMPOSER_INSERT_SIG,
	M_COMPOSER_INSERT_FILE,
	M_COMPOSER_REMOVE_FILE
};

enum settings_messages
{
	M_SETTINGS_SELECT,
	M_SETTINGS_SAVE,
	M_SETTINGS_APPLY,
	M_SETTINGS_CANCEL,
	M_SCROLL_V
};

#endif

