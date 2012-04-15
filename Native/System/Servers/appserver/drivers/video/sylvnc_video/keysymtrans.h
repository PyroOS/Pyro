#ifndef KEYSYMTRANS_H


/* $XConsortium: keysym.h,v 1.15 94/04/17 20:10:55 rws Exp $ */

/***********************************************************

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $TOG: keysymdef.h /main/25 1997/06/21 10:54:51 kaleb $ */

/***********************************************************
Copyright (c) 1987, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
#include <rfb/rfb.h>
#include <rfb/keysym.h>

static void TranslateKeySymToUSKeyScanCode( rfbKeySym XKeySym, uint8 * pDstCode );

static void TranslateKeySymToUSKeyScanCode( rfbKeySym XKeySym, uint8 * pDstCode )
{
    switch ( XKeySym )
    {
	case XK_VoidSymbol:
	{
		*pDstCode = 0x00;
		break;
	}

#ifdef XK_MISCELLANY
/*
 * TTY Functions, cleverly chosen to map to ascii, for convenience of
 * programming, but could have been arbitrary (at the cost of lookup
 * tables in client code.
 */


	case XK_BackSpace:
	{
		*pDstCode = 0x1e;
		break;
	}

	case XK_Tab:
	{
		*pDstCode = 0x26;
		break;
	}

	case XK_Linefeed:
	{
		*pDstCode = 0x47;
		break;
	}

	case XK_Clear:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Return:
	{
		*pDstCode = 0x47;
		break;
	}

	case XK_Pause:
	{
		*pDstCode = 0x10;
		break;
	}

	case XK_Scroll_Lock:
	{
		*pDstCode = 0x0F;
		break;
	}

	case XK_Sys_Req:
	{
		*pDstCode = 0x7e;
		break;
	}

	case XK_Escape:
	{
		*pDstCode = 0x01;
		break;
	}

	case XK_Delete:
	{
		*pDstCode = 0x34;
		break;
	}



/* International & multi-key character composition */


	case XK_Multi_key:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_SingleCandidate:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_MultipleCandidate:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_PreviousCandidate:
	{
		*pDstCode = 0x00;
		break;
	}
/* Cursor control & motion */


	case XK_Home:
	{
		*pDstCode = 0x20;
		break;
	}

	case XK_Left:
	{
		*pDstCode = 0x61;
		break;
	}

	case XK_Up:
	{
		*pDstCode = 0x57;
		break;
	}

	case XK_Right:
	{
		*pDstCode = 0x63;
		break;
	}

	case XK_Down:
	{
		*pDstCode = 0x62;
		break;
	}

	case XK_Page_Up:
	{
		*pDstCode = 0x21;
		break;
	}

	case XK_Page_Down:
	{
		*pDstCode = 0x36;
		break;
	}

	case XK_End:
	{
		*pDstCode = 0x35;
		break;
	}

	case XK_Begin:
	{
		*pDstCode = 0x00;
		break;
	}


/* Misc Functions */


	case XK_Select:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Print:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Execute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Insert:
	{
		*pDstCode = 0x1f;
		break;
	}

	case XK_Undo:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Redo:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Menu:
	{
		*pDstCode = 0x68;
		break;
	}

	case XK_Find:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Cancel:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Help:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Break:
	{
		*pDstCode = 0x7f;
		break;
	}

	case XK_Mode_switch:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Num_Lock:
	{
		*pDstCode = 0x22;
		break;
	}

/* Keypad Functions, keypad numbers cleverly chosen to map to ascii */


	case XK_KP_Space:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_KP_Tab:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_KP_Enter:
	{
		*pDstCode = 0x5b;
		break;
	}

	case XK_KP_F1:
	{
		*pDstCode = 0x02;
		break;
	}

	case XK_KP_F2:
	{
		*pDstCode = 0x03;
		break;
	}

	case XK_KP_F3:
	{
		*pDstCode = 0x04;
		break;
	}

	case XK_KP_F4:
	{
		*pDstCode = 0x05;
		break;
	}

	case XK_KP_Home:
	{
		*pDstCode = 0x37;
		break;
	}

	case XK_KP_Left:
	{
		*pDstCode = 0x48;
		break;
	}

	case XK_KP_Up:
	{
		*pDstCode = 0x38;
		break;
	}

	case XK_KP_Right:
	{
		*pDstCode = 0x4a;
		break;
	}

	case XK_KP_Down:
	{
		*pDstCode = 0x59;
		break;
	}

	case XK_KP_Page_Up:
	{
		*pDstCode = 0x39;
		break;
	}

	case XK_KP_Page_Down:
	{
		*pDstCode = 0x5a;
		break;
	}

	case XK_KP_End:
	{
		*pDstCode = 0x58;
		break;
	}

	case XK_KP_Begin:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_KP_Insert:
	{
		*pDstCode = 0x64;
		break;
	}

	case XK_KP_Delete:
	{
		*pDstCode = 0x65;
		break;
	}

	case XK_KP_Equal:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_KP_Multiply:
	{
		*pDstCode = 0x24;
		break;
	}

	case XK_KP_Add:
	{
		*pDstCode = 0x3a;
		break;
	}

	case XK_KP_Separator:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_KP_Subtract:
	{
		*pDstCode = 0x25;
		break;
	}

	case XK_KP_Decimal:
	{
		*pDstCode = 0x65;
		break;
	}

	case XK_KP_Divide:
	{
		*pDstCode = 0x23;
		break;
	}


	case XK_KP_0:
	{
		*pDstCode = 0x64;
		break;
	}

	case XK_KP_1:
	{
		*pDstCode = 0x58;
		break;
	}

	case XK_KP_2:
	{
		*pDstCode = 0x59;
		break;
	}

	case XK_KP_3:
	{
		*pDstCode = 0x5a;
		break;
	}

	case XK_KP_4:
	{
		*pDstCode = 0x48;
		break;
	}

	case XK_KP_5:
	{
		*pDstCode = 0x49;
		break;
	}

	case XK_KP_6:
	{
		*pDstCode = 0x4a;
		break;
	}

	case XK_KP_7:
	{
		*pDstCode = 0x37;
		break;
	}

	case XK_KP_8:
	{
		*pDstCode = 0x38;
		break;
	}

	case XK_KP_9:
	{
		*pDstCode = 0x39;
		break;
	}



/*
 * Auxilliary Functions; note the duplicate definitions for left and right
 * function keys;  Sun keyboards and a few other manufactures have such
 * function key groups on the left and/or right sides of the keyboard.
 * We've not found a keyboard with more than 35 function keys total.
 */


	case XK_F1:
	{
		*pDstCode = 0x02;
		break;
	}

	case XK_F2:
	{
		*pDstCode = 0x03;
		break;
	}

	case XK_F3:
	{
		*pDstCode = 0x04;
		break;
	}

	case XK_F4:
	{
		*pDstCode = 0x05;
		break;
	}

	case XK_F5:
	{
		*pDstCode = 0x06;
		break;
	}

	case XK_F6:
	{
		*pDstCode = 0x07;
		break;
	}

	case XK_F7:
	{
		*pDstCode = 0x08;
		break;
	}

	case XK_F8:
	{
		*pDstCode = 0x09;
		break;
	}

	case XK_F9:
	{
		*pDstCode = 0x0a;
		break;
	}

	case XK_F10:
	{
		*pDstCode = 0x0b;
		break;
	}

	case XK_F11:
	{
		*pDstCode = 0x0c;
		break;
	}

	case XK_F12:
	{
		*pDstCode = 0x0d;
		break;
	}

	case XK_F13:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F14:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F15:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F16:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F17:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F18:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F19:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F20:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F21:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F22:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F23:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F24:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F25:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F26:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F27:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F28:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F29:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F30:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F31:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F32:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F33:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F34:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_F35:
	{
		*pDstCode = 0x00;
		break;
	}

/* Modifiers */


	case XK_Shift_L:
	{
		*pDstCode = 0x4b;
		break;
	}

	case XK_Shift_R:
	{
		*pDstCode = 0x56;
		break;
	}

	case XK_Control_L:
	{
		*pDstCode = 0x5c;
		break;
	}

	case XK_Control_R:
	{
		*pDstCode = 0x60;
		break;
	}

	case XK_Caps_Lock:
	{
		*pDstCode = 0x3b;
		break;
	}

	case XK_Shift_Lock:
	{
		*pDstCode = 0x3b;
		break;
	}


	case XK_Meta_L:
	{
		*pDstCode = 0x5d;
		break;
	}

	case XK_Meta_R:
	{
		*pDstCode = 0x5f;
		break;
	}

	case XK_Alt_L:
	{
		*pDstCode = 0x5d;
		break;
	}

	case XK_Alt_R:
	{
		*pDstCode = 0x5f;
		break;
	}

	case XK_Super_L:
	{
		*pDstCode = 0x66;
		break;
	}

	case XK_Super_R:
	{
		*pDstCode = 0x67;
		break;
	}

	case XK_Hyper_L:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Hyper_R:
	{
		*pDstCode = 0x00;
		break;
	}
#endif /* XK_MISCELLANY */

/*
 *  Latin 1
 *  Byte 3 = 0
 */
#ifdef XK_LATIN1

	case XK_space:
	{
		*pDstCode = 0x5e;
		break;
	}

	case XK_exclam:
	{
		*pDstCode = 0x12;
		break;
	}

	case XK_quotedbl:
	{
		*pDstCode = 0x46;
		break;
	}

	case XK_numbersign:
	{
		*pDstCode = 0x14;
		break;
	}

	case XK_dollar:
	{
		*pDstCode = 0x15;
		break;
	}

	case XK_percent:
	{
		*pDstCode = 0x16;
		break;
	}

	case XK_ampersand:
	{
		*pDstCode = 0x18;
		break;
	}

	case XK_apostrophe:
	{
		*pDstCode = 0x46;
		break;
	}

	case XK_parenleft:
	{
		*pDstCode = 0x1a;
		break;
	}

	case XK_parenright:
	{
		*pDstCode = 0x1b;
		break;
	}

	case XK_asterisk:
	{
		*pDstCode = 0x19;
		break;
	}

	case XK_plus:
	{
		*pDstCode = 0x1d;
		break;
	}

	case XK_comma:
	{
		*pDstCode = 0x53;
		break;
	}

	case XK_minus:
	{
		*pDstCode = 0x1c;
		break;
	}

	case XK_period:
	{
		*pDstCode = 0x54;
		break;
	}

	case XK_slash:
	{
		*pDstCode = 0x55;
		break;
	}

	case XK_0:
	{
		*pDstCode = 0x1b;
		break;
	}

	case XK_1:
	{
		*pDstCode = 0x12;
		break;
	}

	case XK_2:
	{
		*pDstCode = 0x13;
		break;
	}

	case XK_3:
	{
		*pDstCode = 0x14;
		break;
	}

	case XK_4:
	{
		*pDstCode = 0x15;
		break;
	}

	case XK_5:
	{
		*pDstCode = 0x16;
		break;
	}

	case XK_6:
	{
		*pDstCode = 0x17;
		break;
	}

	case XK_7:
	{
		*pDstCode = 0x18;
		break;
	}

	case XK_8:
	{
		*pDstCode = 0x19;
		break;
	}

	case XK_9:
	{
		*pDstCode = 0x1a;
		break;
	}

	case XK_colon:
	{
		*pDstCode = 0x45;
		break;
	}

	case XK_semicolon:
	{
		*pDstCode = 0x45;
		break;
	}

	case XK_less:
	{
		*pDstCode = 0x53;
		break;
	}

	case XK_equal:
	{
		*pDstCode = 0x1d;
		break;
	}

	case XK_greater:
	{
		*pDstCode = 0x54;
		break;
	}

	case XK_question:
	{
		*pDstCode = 0x55;
		break;
	}

	case XK_at:
	{
		*pDstCode = 0x13;
		break;
	}

	case XK_A:
	{
		*pDstCode = 0x3c;
		break;
	}

	case XK_B:
	{
		*pDstCode = 0x50;
		break;
	}

	case XK_C:
	{
		*pDstCode = 0x4e;
		break;
	}

	case XK_D:
	{
		*pDstCode = 0x3e;
		break;
	}

	case XK_E:
	{
		*pDstCode = 0x29;
		break;
	}

	case XK_F:
	{
		*pDstCode = 0x3f;
		break;
	}

	case XK_G:
	{
		*pDstCode = 0x40;
		break;
	}

	case XK_H:
	{
		*pDstCode = 0x41;
		break;
	}

	case XK_I:
	{
		*pDstCode = 0x2e;
		break;
	}

	case XK_J:
	{
		*pDstCode = 0x42;
		break;
	}

	case XK_K:
	{
		*pDstCode = 0x43;
		break;
	}

	case XK_L:
	{
		*pDstCode = 0x44;
		break;
	}

	case XK_M:
	{
		*pDstCode = 0x52;
		break;
	}

	case XK_N:
	{
		*pDstCode = 0x51;
		break;
	}

	case XK_O:
	{
		*pDstCode = 0x2f;
		break;
	}

	case XK_P:
	{
		*pDstCode = 0x30;
		break;
	}

	case XK_Q:
	{
		*pDstCode = 0x27;
		break;
	}

	case XK_R:
	{
		*pDstCode = 0x2a;
		break;
	}

	case XK_S:
	{
		*pDstCode = 0x3d;
		break;
	}

	case XK_T:
	{
		*pDstCode = 0x2b;
		break;
	}

	case XK_U:
	{
		*pDstCode = 0x2d;
		break;
	}

	case XK_V:
	{
		*pDstCode = 0x4f;
		break;
	}

	case XK_W:
	{
		*pDstCode = 0x28;
		break;
	}

	case XK_X:
	{
		*pDstCode = 0x4d;
		break;
	}

	case XK_Y:
	{
		*pDstCode = 0x2c;
		break;
	}

	case XK_Z:
	{
		*pDstCode = 0x4c;
		break;
	}

	case XK_bracketleft:
	{
		*pDstCode = 0x31;
		break;
	}

	case XK_backslash:
	{
		*pDstCode = 0x33;
		break;
	}

	case XK_bracketright:
	{
		*pDstCode = 0x32;
		break;
	}

	case XK_asciicircum:
	{
		*pDstCode = 0x17;
		break;
	}

	case XK_underscore:
	{
		*pDstCode = 0x1c;
		break;
	}

	case XK_grave:
	{
		*pDstCode = 0x11;
		break;
	}

	case XK_a:
	{
		*pDstCode = 0x3c;
		break;
	}

	case XK_b:
	{
		*pDstCode = 0x50;
		break;
	}

	case XK_c:
	{
		*pDstCode = 0x4e;
		break;
	}

	case XK_d:
	{
		*pDstCode = 0x3e;
		break;
	}

	case XK_e:
	{
		*pDstCode = 0x29;
		break;
	}

	case XK_f:
	{
		*pDstCode = 0x3f;
		break;
	}

	case XK_g:
	{
		*pDstCode = 0x40;
		break;
	}

	case XK_h:
	{
		*pDstCode = 0x41;
		break;
	}

	case XK_i:
	{
		*pDstCode = 0x2e;
		break;
	}

	case XK_j:
	{
		*pDstCode = 0x42;
		break;
	}

	case XK_k:
	{
		*pDstCode = 0x43;
		break;
	}

	case XK_l:
	{
		*pDstCode = 0x44;
		break;
	}

	case XK_m:
	{
		*pDstCode = 0x52;
		break;
	}

	case XK_n:
	{
		*pDstCode = 0x51;
		break;
	}

	case XK_o:
	{
		*pDstCode = 0x2f;
		break;
	}

	case XK_p:
	{
		*pDstCode = 0x30;
		break;
	}

	case XK_q:
	{
		*pDstCode = 0x27;
		break;
	}

	case XK_r:
	{
		*pDstCode = 0x2a;
		break;
	}

	case XK_s:
	{
		*pDstCode = 0x3d;
		break;
	}

	case XK_t:
	{
		*pDstCode = 0x2b;
		break;
	}

	case XK_u:
	{
		*pDstCode = 0x2d;
		break;
	}

	case XK_v:
	{
		*pDstCode = 0x4f;
		break;
	}

	case XK_w:
	{
		*pDstCode = 0x28;
		break;
	}

	case XK_x:
	{
		*pDstCode = 0x4d;
		break;
	}

	case XK_y:
	{
		*pDstCode = 0x2c;
		break;
	}

	case XK_z:
	{
		*pDstCode = 0x4c;
		break;
	}

	case XK_braceleft:
	{
		*pDstCode = 0x31;
		break;
	}

	case XK_bar:
	{
		*pDstCode = 0x33;
		break;
	}

	case XK_braceright:
	{
		*pDstCode = 0x32;
		break;
	}

	case XK_asciitilde:
	{
		*pDstCode = 0x11;
		break;
	}


	case XK_nobreakspace:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_exclamdown:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_cent:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_sterling:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_currency:
	{
		*pDstCode = 0x15;
		break;
	}

	case XK_yen:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_brokenbar:
	{
		*pDstCode = 0x33;
		break;
	}

	case XK_section:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_diaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_copyright:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ordfeminine:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_guillemotleft:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_notsign:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_hyphen:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_registered:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_macron:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_degree:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_plusminus:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_twosuperior:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_threesuperior:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_acute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_mu:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_paragraph:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_periodcentered:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_cedilla:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_onesuperior:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_masculine:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_guillemotright:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_onequarter:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_onehalf:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_threequarters:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_questiondown:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Agrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Aacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Acircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Atilde:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Adiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Aring:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_AE:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ccedilla:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Egrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Eacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ecircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ediaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Igrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Iacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Icircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Idiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Eth:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ntilde:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ograve:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Oacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ocircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Otilde:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Odiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_multiply:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ooblique:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ugrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Uacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Ucircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Udiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Yacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_Thorn:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ssharp:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_agrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_aacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_acircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_atilde:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_adiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_aring:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ae:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ccedilla:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_egrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_eacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ecircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ediaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_igrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_iacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_icircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_idiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_eth:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ntilde:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ograve:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_oacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ocircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_otilde:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_odiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_division:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_oslash:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ugrave:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_uacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ucircumflex:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_udiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_yacute:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_thorn:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ydiaeresis:
	{
		*pDstCode = 0x00;
		break;
	}
#endif /* XK_LATIN1 */

/*
 *  Special
 *  Byte 3 = 9
 */

#ifdef XK_SPECIAL

	case XK_blank:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_soliddiamond:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_checkerboard:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ht:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_ff:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_cr:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_lf:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_nl:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_vt:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_lowrightcorner:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_uprightcorner:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_upleftcorner:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_lowleftcorner:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_crossinglines:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_horizlinescan1:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_horizlinescan3:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_horizlinescan5:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_horizlinescan7:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_horizlinescan9:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_leftt:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_rightt:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_bott:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_topt:
	{
		*pDstCode = 0x00;
		break;
	}

	case XK_vertbar:
	{
		*pDstCode = 0x00;
		break;
	}
#endif /* XK_SPECIAL */
	default:
	{
		*pDstCode = 0x00;
		break;
	}
}

#endif
