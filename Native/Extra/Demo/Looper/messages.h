// EFileBrowser	 (C)opyright 2006 Jonas Jarvoll
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef __MESSAGES_H_
#define __MESSAGES_H_

enum Messages
{
	// Buttons
	MSG_BTN_START,
	MSG_BTN_STOP,

	// To looper
	MSG_TOLOOPER_START,
	MSG_TOLOOPER_STOP,
	MSG_TOLOOPER_ACK,

	// From looper
	MSG_FROMLOOPER_NEW_MESSAGE,

	// Used where no message is required
	MSG_VOID
};

#endif

