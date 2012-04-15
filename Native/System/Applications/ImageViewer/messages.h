// ImageViewer (C)opyright 2008 Jonas Jarvoll
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

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

enum
{
	MSG_NEXT_IMAGE, 
	MSG_PREV_IMAGE, 
	MSG_DELETE,
	MSG_DELETED_FILE,
	MSG_FLIP_FULLSCREEN,
	MSG_WANTS_TO_QUIT,
	MSG_ROTATE_CCW,
	MSG_ROTATE_CW,
	MSG_SLIDESHOW_START,
	MSG_SLIDESHOW_STOP,
	MSG_ZOOM_SLIDER,
	MSG_TIMEOUT_SLIDER,
	MSG_VIEW_FIT,
	MSG_VIEW_100,
	MSG_SCROLLBAR,
	MSG_ZOOM_IN,
	MSG_ZOOM_OUT,
	MSG_OPEN,
	MSG_SET_DESKTOP,
	MSG_COPY_FINISHED
};

#endif

