/* Webster (C)opyright	2008 Kristian Van Der Vliet
 * 						2004-2007 Arno Klenke
 *						2001 Kurt Skauen
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef BROWSER_MESSAGES_H
#define BROWSER_MESSAGES_H

enum browser_messages
{
	ID_URL_CHANGED = 1,
	ID_SET_STATUS_BAR_TEXT,
	ID_CLEAR_STATUS_BAR_TEXT,
	ID_CREATE_WINDOW,
	ID_CREATE_WINDOW_REPLY,
	ID_CREATE_TAB,
	ID_CREATE_TAB_REPLY,
	ID_WINDOW_OPENED,
	ID_WINDOW_CLOSED,
	ID_BUTTON_BACK,
	ID_BUTTON_FORWARD,
	ID_BUTTON_RELOAD,
	ID_BUTTON_STOP,
	ID_BUTTON_HOME,
	ID_MENU_APP_ABOUT,
	ID_MENU_WIN_NEW_TAB,
	ID_MENU_WIN_CLOSE_TAB,
	ID_MENU_EDIT_CUT,
	ID_MENU_EDIT_COPY,
	ID_MENU_EDIT_PASTE,
	ID_MENU_EDIT_DELETE,
	ID_MENU_SETTINGS_CONFIGURE,
	ID_MENU_BOOKMARKS_ADD,
	ID_MENU_BOOKMARKS_MANAGE,
	ID_BOOKMARK_GO,
	ID_TAB_CHANGED,
	ID_WEBVIEW_SET_TITLE,
	ID_WEBVIEW_LOAD_STARTED,
	ID_WEBVIEW_LOAD_FINISHED,
	ID_SETTINGS_SELECT,
	ID_SETTINGS_SAVE,
	ID_SETTINGS_APPLY,
	ID_SETTINGS_CANCEL,
	ID_SETTINGS_SCROLL_V

};

#endif

