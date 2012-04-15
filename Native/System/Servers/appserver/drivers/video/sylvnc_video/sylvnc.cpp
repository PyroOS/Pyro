
/*
 *  The AtheOS application server
 *  Copyright (C) 1999 - 2001 Kurt Skauen
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <atheos/types.h>
#include <atheos/isa_io.h>

#include <atheos/kernel.h>
#include <atheos/vesa_gfx.h>
#include <atheos/areas.h>

#include "../../../server/bitmap.h"
#include "../../../server/sprite.h"

#include <gui/bitmap.h>

#include "sylvnc.h"

using namespace os;

#define	MAX_MODEINFO_NAME			79

#define	MIF_PALETTE	0x0001	/* palettized screen mode       */

static area_id g_nFrameBufArea = -1;
static FILE * g_pfKbdDev;
static FILE * g_pfMouseDev;
static bool g_bDidMouseOrKey;
static void (* defaultPtrAddEvent) (int buttonMask,int x,int y,rfbClientPtr cl);

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

SylVNC::SylVNC()
:	m_cRFBLock("SylVNC_RFB_lock")
{
	rfbScreen = NULL;
	m_bIsInitiated = true;
	return;
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

SylVNC::~SylVNC()
{
}


//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

bool SylVNC::InitModes( void )
{
	Vesa_Info_s sVesaInfo;
	VESA_Mode_Info_s sModeInfo;
	uint16 anModes[1024];
	int nModeCount;

	int i = 0;

	strcpy( sVesaInfo.VesaSignature, "VBE2" );

	nModeCount = get_vesa_info( &sVesaInfo, anModes, 1024 );

	if( nModeCount <= 0 )
	{
		dbprintf( "Error: SylVNC::InitModes() no VESA20 modes found\n" );
		return ( false );
	}

//    dbprintf( "Found %d vesa modes\n", nModeCount );

	int nPagedCount = 0;
	int nPlanarCount = 0;
	int nBadCount = 0;

	for( i = 0; i < nModeCount; ++i )
	{
		get_vesa_mode_info( &sModeInfo, anModes[i] );

		if( sModeInfo.PhysBasePtr == 0 )
		{		// We must have linear frame buffer
			nPagedCount++;
			continue;
		}
		if( sModeInfo.BitsPerPixel < 8 )
		{
			nPlanarCount++;
			continue;
		}
		if( sModeInfo.NumberOfPlanes != 1 )
		{
			nPlanarCount++;
			continue;
		}

		if( sModeInfo.BitsPerPixel != 15 && sModeInfo.BitsPerPixel != 16 && sModeInfo.BitsPerPixel != 32 )
		{
			nBadCount++;
			continue;
		}

		if( sModeInfo.RedMaskSize == 0 && sModeInfo.GreenMaskSize == 0 && sModeInfo.BlueMaskSize == 0 && sModeInfo.RedFieldPosition == 0 && sModeInfo.GreenFieldPosition == 0 && sModeInfo.BlueFieldPosition == 0 )
		{
			m_cModeList.push_back( VNCMode( sModeInfo.XResolution, sModeInfo.YResolution, sModeInfo.BytesPerScanLine, CS_CMAP8, 60.0f, anModes[i] | 0x4000, sModeInfo.PhysBasePtr ) );
		}
		else if( sModeInfo.RedMaskSize == 5 && sModeInfo.GreenMaskSize == 5 && sModeInfo.BlueMaskSize == 5 && sModeInfo.RedFieldPosition == 10 && sModeInfo.GreenFieldPosition == 5 && sModeInfo.BlueFieldPosition == 0 )
		{
			m_cModeList.push_back( VNCMode( sModeInfo.XResolution, sModeInfo.YResolution, sModeInfo.BytesPerScanLine, CS_RGB15, 60.0f, anModes[i] | 0x4000, sModeInfo.PhysBasePtr ) );
		}
		else if( sModeInfo.RedMaskSize == 5 && sModeInfo.GreenMaskSize == 6 && sModeInfo.BlueMaskSize == 5 && sModeInfo.RedFieldPosition == 11 && sModeInfo.GreenFieldPosition == 5 && sModeInfo.BlueFieldPosition == 0 )
		{
			m_cModeList.push_back( VNCMode( sModeInfo.XResolution, sModeInfo.YResolution, sModeInfo.BytesPerScanLine, CS_RGB16, 60.0f, anModes[i] | 0x4000, sModeInfo.PhysBasePtr ) );
		}
		else if( sModeInfo.BitsPerPixel == 32 && sModeInfo.RedMaskSize == 8 && sModeInfo.GreenMaskSize == 8 && sModeInfo.BlueMaskSize == 8 && sModeInfo.RedFieldPosition == 16 && sModeInfo.GreenFieldPosition == 8 && sModeInfo.BlueFieldPosition == 0 )
		{
			m_cModeList.push_back( VNCMode( sModeInfo.XResolution, sModeInfo.YResolution, sModeInfo.BytesPerScanLine, CS_RGB32, 60.0f, anModes[i] | 0x4000, sModeInfo.PhysBasePtr ) );
		}
		else
		{
			dbprintf( "Found unsupported video mode: %dx%d %d BPP %d BPL - %d:%d:%d, %d:%d:%d\n", sModeInfo.XResolution, sModeInfo.YResolution, sModeInfo.BitsPerPixel, sModeInfo.BytesPerScanLine, sModeInfo.RedMaskSize, sModeInfo.GreenMaskSize, sModeInfo.BlueMaskSize, sModeInfo.RedFieldPosition, sModeInfo.GreenFieldPosition, sModeInfo.BlueFieldPosition );
		}
#if 0
		dbprintf( "Mode %04x: %dx%d %d BPP %d BPL - %d:%d:%d, %d:%d:%d (%p)\n", anModes[i], sModeInfo.XResolution, sModeInfo.YResolution, sModeInfo.BitsPerPixel, sModeInfo.BytesPerScanLine, sModeInfo.RedMaskSize, sModeInfo.GreenMaskSize, sModeInfo.BlueMaskSize, sModeInfo.RedFieldPosition, sModeInfo.GreenFieldPosition, sModeInfo.BlueFieldPosition, ( void * )sModeInfo.PhysBasePtr );
#endif
	}
	dbprintf( "Found total of %d VESA modes. Valid: %d, Paged: %d, Planar: %d, Bad: %d\n", nModeCount, m_cModeList.size(), nPagedCount, nPlanarCount, nBadCount );
	return ( true );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

area_id SylVNC::Open( void )
{
	if( InitModes() )
	{
		rfbLogEnable(0);
		m_nFrameBufferSize = 1024 * 1024 * 4;
		m_pFrameBuffer = NULL;
		g_nFrameBufArea = create_area( "vesa_io", /*NULL */(void**) &m_pFrameBuffer, m_nFrameBufferSize,
			AREA_FULL_ACCESS, AREA_NO_LOCK );
		dbprintf("m_pFrameBuffer = %08x\n", (uint32)m_pFrameBuffer);
		dbprintf("m_pFrameBufferSize = %08x\n", m_nFrameBufferSize);
		dbprintf("Frame Buffer End = %08x\n", (uint32)m_pFrameBuffer + (uint32)m_nFrameBufferSize);
		return ( g_nFrameBufArea );
	}
	return ( -1 );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

void SylVNC::Close( void )
{
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

int SylVNC::GetScreenModeCount()
{
	return ( m_cModeList.size() );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

bool SylVNC::GetScreenModeDesc( int nIndex, screen_mode * psMode )
{
	if( nIndex >= 0 && nIndex < int ( m_cModeList.size() ) )
	{
		*psMode = m_cModeList[nIndex];
		return ( true );
	}
	else
	{
		return ( false );
	}
}


//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

int SylVNC::SetScreenMode( screen_mode sMode )
{
	int nBytesPerPixel = 0;
	m_nCurrentMode = -1;

	for( int i = GetScreenModeCount() - 1; i >= 0; --i )
	{
		if( m_cModeList[i].m_nWidth == sMode.m_nWidth && m_cModeList[i].m_nHeight == sMode.m_nHeight && m_cModeList[i].m_eColorSpace == sMode.m_eColorSpace )
		{
			m_nCurrentMode = i;
			break;
		}

	}

	if( m_nCurrentMode >= 0 )
	{
//		remap_area( g_nFrameBufArea, ( void * )( m_cModeList[m_nCurrentMode].m_nFrameBuffer & PAGE_MASK ) );
//		m_nFrameBufferOffset = m_cModeList[m_nCurrentMode].m_nFrameBuffer & ~PAGE_MASK;
//		if( SetVNCMode( m_cModeList[m_nCurrentMode].m_nVNCMode ) )
		if( true )
		{
			switch (sMode.m_eColorSpace)
			{
			case CS_RGBA15:
			case CS_RGB15:
			case CS_RGB16:
				nBytesPerPixel=2;
				break;
			case CS_RGB24:
				nBytesPerPixel=3;
				break;
			case CS_RGBA32:
			case CS_RGB32:
				nBytesPerPixel=4;
				break;
			default:
				nBytesPerPixel=4;
				break;
			}
			if( rfbScreen == NULL )
			{
				// ACQUIRE RFB lock
				m_cRFBLock.Lock();
				
				rfbScreen = rfbGetScreen( NULL, NULL, sMode.m_nWidth, sMode.m_nHeight, 8, 3, nBytesPerPixel);
				dbprintf("rfbGetScreen Got Screen: sMode.m_nWidth = %d, sMode.m_nHeight = %d, nBytesPerPixel = %d.\n", sMode.m_nWidth, sMode.m_nHeight, nBytesPerPixel); 
				rfbScreen->frameBuffer = (char*)m_pFrameBuffer;
				rfbInitServer(rfbScreen);
				dbprintf("rfbInitServer initted\n");

				// RELEASE RFB lock
				m_cRFBLock.Unlock();

				g_pfKbdDev = fopen("/dev/keybd", "wb");
				g_pfMouseDev = fopen("/dev/input/sylvnc_mouse", "wb");
				rfbScreen->kbdAddEvent = RFBKbdEventHandler;
				defaultPtrAddEvent = rfbScreen->ptrAddEvent;
				rfbScreen->ptrAddEvent = RFBMouseEventHandler;
				rfbEventThread = spawn_thread( "RFB Event Thread", (void*)RFBEventThread, DISPLAY_PRIORITY, 0, this);
				resume_thread( rfbEventThread );			
//				rfbProcessEvents(rfbScreen, 0);
			}
			return ( 0 );
		}
	}
	return ( -1 );
}

void SylVNC::RFBEventThread( SylVNC * pcDriver )
{
	dbprintf("RFBEventThread successfully spawned.\n");
	loop: for( ;; )
	{
		g_bDidMouseOrKey = true;
		// ACQUIRE RFB & FB locks
		pcDriver->m_cRFBLock.Lock();
		while ( g_bDidMouseOrKey == true )
		{
			g_bDidMouseOrKey = false;
			rfbProcessEvents(pcDriver->rfbScreen, 0);
		}
		// RELEASE RFB & FB locks
		pcDriver->m_cRFBLock.Unlock();
		fflush( g_pfKbdDev );
		fflush( g_pfMouseDev);
		snooze( 100 );
	}
	dbprintf("oops, RFBEventThread loop escaped somehow...");
	goto loop;
}

void SylVNC::RFBKbdEventHandler( rfbBool down, rfbKeySym key, _rfbClientRec *)
{
	uint8 translatedKey = 0;
	if (key >=0)
	{
		translatedKey = TranslateKeyCode( key );
		if (translatedKey != 0)
		{
			if ( !down )
			{
				translatedKey = translatedKey | 0x80;
			}
			fwrite( &translatedKey, sizeof(uint8), 1, g_pfKbdDev);
		}
		g_bDidMouseOrKey = true;
	}
	
}

void SylVNC::RFBMouseEventHandler( int nButtonMask, int nXMotion, int nYMotion, _rfbClientRec * Client)
{
	int sig = SYLVNC_MOUSE_EVENT_SIG;
	SylVNCMouseEvent sEvent = {sig, nButtonMask, nXMotion, nYMotion};
	defaultPtrAddEvent(nButtonMask, nXMotion, nYMotion , Client); //default LibVNCServer mouse event handler: needed for client-side mouse handling
	fwrite( &sEvent, sizeof (SylVNCMouseEvent), 1, g_pfMouseDev);
	g_bDidMouseOrKey = true;
}


screen_mode SylVNC::GetCurrentScreenMode()
{
	return ( m_cModeList[m_nCurrentMode] );
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

bool SylVNC::IntersectWithMouse( const IRect & cRect )
{
//  if ( NULL != m_pcMouse ) {
//    return( cRect.DoIntersect( m_pcMouse->GetFrame() ) );
//  } else {
	return ( false );
//  }
}

//----------------------------------------------------------------------------
// NAME:
// DESC:
// NOTE:
// SEE ALSO:
//----------------------------------------------------------------------------

bool SylVNC::SetVNCMode( uint32 nMode )
{
	
	struct RMREGS rm;

	memset( &rm, 0, sizeof( struct RMREGS ) );

	rm.EAX = 0x4f02;
	rm.EBX = nMode;

	realint( 0x10, &rm );
	int nResult = rm.EAX & 0xffff;

	memset( &rm, 0, sizeof( struct RMREGS ) );
	rm.EBX = 0x01;		// Get display offset.
	rm.EAX = 0x4f07;
	realint( 0x10, &rm );

	memset( &rm, 0, sizeof( struct RMREGS ) );
	rm.EBX = 0x00;
	rm.EAX = 0x4f07;
	realint( 0x10, &rm );

	memset( &rm, 0, sizeof( struct RMREGS ) );
	rm.EBX = 0x01;		// Get display offset.
	rm.EAX = 0x4f07;
	realint( 0x10, &rm );
	
	return ( nResult );
}

bool SylVNC::DrawLine(SrvBitmap* pcBitmap, const os::IRect& cClipRect,
	const os::IPoint& cPnt1, const os::IPoint& cPnt2,
	const os::Color32_s& sColor, int nMode)
{
	if(!pcBitmap->m_bVideoMem)
	{
		/* Off-screen */
		return DisplayDriver::DrawLine(pcBitmap, cClipRect, cPnt1, cPnt2,
										sColor, nMode);
	}

	int x1 = cPnt1.x;
	int y1 = cPnt1.y;
	int x2 = cPnt2.x;
	int y2 = cPnt2.y;
	int xTmp = 0;
	int yTmp = 0;

	if(!DisplayDriver::ClipLine(cClipRect, &x1, &y1, &x2, &y2))
	{
		return false;
	}

	if(x1 > x2)
	{
		xTmp = x1;
		x1 = x2;
		x2 = xTmp;
		xTmp = 0;
	}

	if(y1 > y2)
	{
		yTmp = y1;
		y1 = y2;
		y2 = yTmp;
		yTmp = 0;
	}

	DisplayDriver::DrawLine(pcBitmap, cClipRect, cPnt1, cPnt2, sColor, nMode);

	// ACQUIRE RFB lock
	m_cRFBLock.Lock();
		
	rfbMarkRectAsModified( rfbScreen, x1, y1, x2 + 1, y2 + 1 );

	// RELEASE RFB lock
	m_cRFBLock.Unlock();

	resume_thread( rfbEventThread );
//	rfbProcessEvents(rfbScreen, 0);

	return true;
}

bool SylVNC::FillRect(SrvBitmap *pcBitmap, const IRect& cRect,
	const Color32_s& sColor)
{
	if(!pcBitmap->m_bVideoMem)
	{
		/* Off-screen */
		return DisplayDriver::FillRect(pcBitmap, cRect, sColor);
	}

	int nColor;

	if (pcBitmap->m_eColorSpc == CS_RGB32)
		nColor = COL_TO_RGB32(sColor);
	else
		nColor = COL_TO_RGB16(sColor);

	DisplayDriver::FillRect(pcBitmap, cRect, sColor);

	// ACQUIRE RFB lock
	m_cRFBLock.Lock();
	
	rfbMarkRectAsModified( rfbScreen, cRect.left, cRect.top, cRect.right + 1, cRect.bottom + 1);

	// RELEASE RFB lock
	m_cRFBLock.Unlock();

	resume_thread( rfbEventThread );
//	rfbProcessEvents(rfbScreen, 0);
	
	return true;
}


bool SylVNC::BltBitmap(SrvBitmap *pcDstBitmap, SrvBitmap *pcSrcBitmap,
	IRect cSrcRect, IPoint cDstPos, int nMode)
{
	if((!pcDstBitmap->m_bVideoMem) && (!pcSrcBitmap->m_bVideoMem))
	{
		// Off-screen to off-screen
		return DisplayDriver::BltBitmap(pcDstBitmap, pcSrcBitmap, cSrcRect,
										cDstPos, nMode);
	}

	int srcX1 = cSrcRect.left;
	int srcY1 = cSrcRect.top;
	int srcX2 = cSrcRect.right;
	int srcY2 = cSrcRect.bottom;
	int dstX1 = cDstPos.x;
	int dstY1 = cDstPos.y;
	int dstX2 = cDstPos.x + cSrcRect.Width();
	int dstY2 = cDstPos.y + cSrcRect.Height();

	if((pcDstBitmap->m_bVideoMem) && (pcSrcBitmap->m_bVideoMem))
	{
		if(nMode == DM_COPY)
//			dbprintf("SylVNC::BltBitmap() - Screen to screen DM_COPY\n");
		;
		else if(nMode == DM_OVER)
			dbprintf("SylVNC::BltBitmap() - Screen to screen DM_OVER\n");
		else if(nMode == DM_BLEND)
			dbprintf("SylVNC::BltBitmap() - Screen to screen DM_BLEND\n");
		else
			dbprintf("SylVNC::BltBitmap() - Unknown nMode = %d\n", nMode);
	}

	DisplayDriver::BltBitmap(pcDstBitmap, pcSrcBitmap, cSrcRect, cDstPos, nMode);

	if(pcDstBitmap->m_bVideoMem)
	{
	        // ACQUIRE RFB lock
	        m_cRFBLock.Lock();

		rfbMarkRectAsModified( rfbScreen, dstX1, dstY1, dstX2 + 1, dstY2 + 1);

	        // RELEASE RFB lock
	        m_cRFBLock.Unlock();
	}

	if(pcSrcBitmap->m_bVideoMem)
	{
                // ACQUIRE RFB lock
		m_cRFBLock.Lock();

		rfbMarkRectAsModified( rfbScreen, srcX1, srcY1, srcX2 + 1, srcY2 + 1);

		// RELEASE RFB lock
		m_cRFBLock.Unlock();
	}
	resume_thread( rfbEventThread );
//	rfbProcessEvents(rfbScreen, 0);

	return true;
}

void SylVNC::RenderGlyph(SrvBitmap *pcBitmap, Glyph* pcGlyph,
	const os::IPoint& cPos, const os::IRect& cClipRect,
	const os::Color32_s& sFgColor)
{
	if(!pcBitmap->m_bVideoMem)
	{
		DisplayDriver::RenderGlyph(pcBitmap, pcGlyph, cPos, cClipRect, sFgColor);
		return;
	}

	DisplayDriver::RenderGlyph(pcBitmap, pcGlyph, cPos, cClipRect, sFgColor);

	// ACQUIRE RFB lock
	m_cRFBLock.Lock();

	rfbMarkRectAsModified( rfbScreen, cClipRect.left, cClipRect.top, cClipRect.right + 1, cClipRect.bottom + 1);

	// RELEASE RFB lock
	m_cRFBLock.Unlock();

	resume_thread( rfbEventThread );
//	rfbProcessEvents(rfbScreen, 0);

}

void SylVNC::RenderGlyphBlend(SrvBitmap *pcBitmap, Glyph* pcGlyph,
	const os::IPoint& cPos, const os::IRect& cClipRect,
	const os::Color32_s& sFgColor)
{
	if(!pcBitmap->m_bVideoMem)
	{
		DisplayDriver::RenderGlyphBlend(pcBitmap, pcGlyph, cPos, cClipRect,
										sFgColor);
		return;
	}

	DisplayDriver::RenderGlyphBlend(pcBitmap, pcGlyph, cPos, cClipRect, sFgColor);

	// ACQUIRE RFB lock
	m_cRFBLock.Lock();

	rfbMarkRectAsModified( rfbScreen, cClipRect.left, cClipRect.top, cClipRect.right + 1, cClipRect.bottom + 1);

	// RELEASE RFB lock
	m_cRFBLock.Unlock();

	resume_thread( rfbEventThread );
//	rfbProcessEvents(rfbScreen, 0);

}

void SylVNC::RenderGlyph(SrvBitmap *pcBitmap, Glyph* pcGlyph,
	const os::IPoint& cPos, const os::IRect& cClipRect, const uint32* anPallette)
{
	if(!pcBitmap->m_bVideoMem)
	{
		DisplayDriver::RenderGlyph(pcBitmap, pcGlyph, cPos, cClipRect, anPallette);
		return;
	}

	DisplayDriver::RenderGlyph(pcBitmap, pcGlyph, cPos, cClipRect, anPallette);

	// ACQUIRE RFB lock
	m_cRFBLock.Lock();

	rfbMarkRectAsModified( rfbScreen, cClipRect.left, cClipRect.top, cClipRect.right + 1, cClipRect.bottom + 1);	

	// RELEASE RFB lock
	m_cRFBLock.Unlock();

	resume_thread( rfbEventThread );
//	rfbProcessEvents(rfbScreen, 0);
}

extern "C" DisplayDriver* init_gfx_driver()
{
	try
	{
		SylVNC * pcDriver = new SylVNC();
		if( pcDriver->IsInitiated() )
		{
			return pcDriver;
		}
		return NULL;
	}
	catch(std::exception& cExc)
	{
		dbprintf("SylVNC - Got exception trying to init\n");
		return NULL;
	}
}

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


uint8 SylVNC::TranslateKeyCode( rfbKeySym XKeySym )
{
    uint8 nDstCode = 0x00;
    switch ( XKeySym )
    {
	case XK_VoidSymbol:
	{
		nDstCode = 0x00;
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
		nDstCode = 0x1e;
		break;
	}

	case XK_Tab:
	{
		nDstCode = 0x26;
		break;
	}

	case XK_Linefeed:
	{
		nDstCode = 0x47;
		break;
	}

	case XK_Clear:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Return:
	{
		nDstCode = 0x47;
		break;
	}

	case XK_Pause:
	{
		nDstCode = 0x10;
		break;
	}

	case XK_Scroll_Lock:
	{
		nDstCode = 0x0F;
		break;
	}

	case XK_Sys_Req:
	{
		nDstCode = 0x7e;
		break;
	}

	case XK_Escape:
	{
		nDstCode = 0x01;
		break;
	}

	case XK_Delete:
	{
		nDstCode = 0x34;
		break;
	}



/* International & multi-key character composition */


	case XK_Multi_key:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_SingleCandidate:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_MultipleCandidate:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_PreviousCandidate:
	{
		nDstCode = 0x00;
		break;
	}
/* Cursor control & motion */


	case XK_Home:
	{
		nDstCode = 0x20;
		break;
	}

	case XK_Left:
	{
		nDstCode = 0x61;
		break;
	}

	case XK_Up:
	{
		nDstCode = 0x57;
		break;
	}

	case XK_Right:
	{
		nDstCode = 0x63;
		break;
	}

	case XK_Down:
	{
		nDstCode = 0x62;
		break;
	}

	case XK_Page_Up:
	{
		nDstCode = 0x21;
		break;
	}

	case XK_Page_Down:
	{
		nDstCode = 0x36;
		break;
	}

	case XK_End:
	{
		nDstCode = 0x35;
		break;
	}

	case XK_Begin:
	{
		nDstCode = 0x00;
		break;
	}


/* Misc Functions */


	case XK_Select:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Print:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Execute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Insert:
	{
		nDstCode = 0x1f;
		break;
	}

	case XK_Undo:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Redo:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Menu:
	{
		nDstCode = 0x68;
		break;
	}

	case XK_Find:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Cancel:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Help:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Break:
	{
		nDstCode = 0x7f;
		break;
	}

	case XK_Mode_switch:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Num_Lock:
	{
		nDstCode = 0x22;
		break;
	}

/* Keypad Functions, keypad numbers cleverly chosen to map to ascii */


	case XK_KP_Space:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_KP_Tab:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_KP_Enter:
	{
		nDstCode = 0x5b;
		break;
	}

	case XK_KP_F1:
	{
		nDstCode = 0x02;
		break;
	}

	case XK_KP_F2:
	{
		nDstCode = 0x03;
		break;
	}

	case XK_KP_F3:
	{
		nDstCode = 0x04;
		break;
	}

	case XK_KP_F4:
	{
		nDstCode = 0x05;
		break;
	}

	case XK_KP_Home:
	{
		nDstCode = 0x37;
		break;
	}

	case XK_KP_Left:
	{
		nDstCode = 0x48;
		break;
	}

	case XK_KP_Up:
	{
		nDstCode = 0x38;
		break;
	}

	case XK_KP_Right:
	{
		nDstCode = 0x4a;
		break;
	}

	case XK_KP_Down:
	{
		nDstCode = 0x59;
		break;
	}

	case XK_KP_Page_Up:
	{
		nDstCode = 0x39;
		break;
	}

	case XK_KP_Page_Down:
	{
		nDstCode = 0x5a;
		break;
	}

	case XK_KP_End:
	{
		nDstCode = 0x58;
		break;
	}

	case XK_KP_Begin:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_KP_Insert:
	{
		nDstCode = 0x64;
		break;
	}

	case XK_KP_Delete:
	{
		nDstCode = 0x65;
		break;
	}

	case XK_KP_Equal:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_KP_Multiply:
	{
		nDstCode = 0x24;
		break;
	}

	case XK_KP_Add:
	{
		nDstCode = 0x3a;
		break;
	}

	case XK_KP_Separator:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_KP_Subtract:
	{
		nDstCode = 0x25;
		break;
	}

	case XK_KP_Decimal:
	{
		nDstCode = 0x65;
		break;
	}

	case XK_KP_Divide:
	{
		nDstCode = 0x23;
		break;
	}


	case XK_KP_0:
	{
		nDstCode = 0x64;
		break;
	}

	case XK_KP_1:
	{
		nDstCode = 0x58;
		break;
	}

	case XK_KP_2:
	{
		nDstCode = 0x59;
		break;
	}

	case XK_KP_3:
	{
		nDstCode = 0x5a;
		break;
	}

	case XK_KP_4:
	{
		nDstCode = 0x48;
		break;
	}

	case XK_KP_5:
	{
		nDstCode = 0x49;
		break;
	}

	case XK_KP_6:
	{
		nDstCode = 0x4a;
		break;
	}

	case XK_KP_7:
	{
		nDstCode = 0x37;
		break;
	}

	case XK_KP_8:
	{
		nDstCode = 0x38;
		break;
	}

	case XK_KP_9:
	{
		nDstCode = 0x39;
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
		nDstCode = 0x02;
		break;
	}

	case XK_F2:
	{
		nDstCode = 0x03;
		break;
	}

	case XK_F3:
	{
		nDstCode = 0x04;
		break;
	}

	case XK_F4:
	{
		nDstCode = 0x05;
		break;
	}

	case XK_F5:
	{
		nDstCode = 0x06;
		break;
	}

	case XK_F6:
	{
		nDstCode = 0x07;
		break;
	}

	case XK_F7:
	{
		nDstCode = 0x08;
		break;
	}

	case XK_F8:
	{
		nDstCode = 0x09;
		break;
	}

	case XK_F9:
	{
		nDstCode = 0x0a;
		break;
	}

	case XK_F10:
	{
		nDstCode = 0x0b;
		break;
	}

	case XK_F11:
	{
		nDstCode = 0x0c;
		break;
	}

	case XK_F12:
	{
		nDstCode = 0x0d;
		break;
	}

	case XK_F13:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F14:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F15:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F16:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F17:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F18:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F19:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F20:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F21:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F22:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F23:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F24:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F25:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F26:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F27:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F28:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F29:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F30:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F31:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F32:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F33:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F34:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_F35:
	{
		nDstCode = 0x00;
		break;
	}

/* Modifiers */


	case XK_Shift_L:
	{
		nDstCode = 0x4b;
		break;
	}

	case XK_Shift_R:
	{
		nDstCode = 0x56;
		break;
	}

	case XK_Control_L:
	{
		nDstCode = 0x5c;
		break;
	}

	case XK_Control_R:
	{
		nDstCode = 0x60;
		break;
	}

	case XK_Caps_Lock:
	{
		nDstCode = 0x3b;
		break;
	}

	case XK_Shift_Lock:
	{
		nDstCode = 0x3b;
		break;
	}


	case XK_Meta_L:
	{
		nDstCode = 0x5d;
		break;
	}

	case XK_Meta_R:
	{
		nDstCode = 0x5f;
		break;
	}

	case XK_Alt_L:
	{
		nDstCode = 0x5d;
		break;
	}

	case XK_Alt_R:
	{
		nDstCode = 0x5f;
		break;
	}

	case XK_Super_L:
	{
		nDstCode = 0x66;
		break;
	}

	case XK_Super_R:
	{
		nDstCode = 0x67;
		break;
	}

	case XK_Hyper_L:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Hyper_R:
	{
		nDstCode = 0x00;
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
		nDstCode = 0x5e;
		break;
	}

	case XK_exclam:
	{
		nDstCode = 0x12;
		break;
	}

	case XK_quotedbl:
	{
		nDstCode = 0x46;
		break;
	}

	case XK_numbersign:
	{
		nDstCode = 0x14;
		break;
	}

	case XK_dollar:
	{
		nDstCode = 0x15;
		break;
	}

	case XK_percent:
	{
		nDstCode = 0x16;
		break;
	}

	case XK_ampersand:
	{
		nDstCode = 0x18;
		break;
	}

	case XK_apostrophe:
	{
		nDstCode = 0x46;
		break;
	}

	case XK_parenleft:
	{
		nDstCode = 0x1a;
		break;
	}

	case XK_parenright:
	{
		nDstCode = 0x1b;
		break;
	}

	case XK_asterisk:
	{
		nDstCode = 0x19;
		break;
	}

	case XK_plus:
	{
		nDstCode = 0x1d;
		break;
	}

	case XK_comma:
	{
		nDstCode = 0x53;
		break;
	}

	case XK_minus:
	{
		nDstCode = 0x1c;
		break;
	}

	case XK_period:
	{
		nDstCode = 0x54;
		break;
	}

	case XK_slash:
	{
		nDstCode = 0x55;
		break;
	}

	case XK_0:
	{
		nDstCode = 0x1b;
		break;
	}

	case XK_1:
	{
		nDstCode = 0x12;
		break;
	}

	case XK_2:
	{
		nDstCode = 0x13;
		break;
	}

	case XK_3:
	{
		nDstCode = 0x14;
		break;
	}

	case XK_4:
	{
		nDstCode = 0x15;
		break;
	}

	case XK_5:
	{
		nDstCode = 0x16;
		break;
	}

	case XK_6:
	{
		nDstCode = 0x17;
		break;
	}

	case XK_7:
	{
		nDstCode = 0x18;
		break;
	}

	case XK_8:
	{
		nDstCode = 0x19;
		break;
	}

	case XK_9:
	{
		nDstCode = 0x1a;
		break;
	}

	case XK_colon:
	{
		nDstCode = 0x45;
		break;
	}

	case XK_semicolon:
	{
		nDstCode = 0x45;
		break;
	}

	case XK_less:
	{
		nDstCode = 0x53;
		break;
	}

	case XK_equal:
	{
		nDstCode = 0x1d;
		break;
	}

	case XK_greater:
	{
		nDstCode = 0x54;
		break;
	}

	case XK_question:
	{
		nDstCode = 0x55;
		break;
	}

	case XK_at:
	{
		nDstCode = 0x13;
		break;
	}

	case XK_A:
	{
		nDstCode = 0x3c;
		break;
	}

	case XK_B:
	{
		nDstCode = 0x50;
		break;
	}

	case XK_C:
	{
		nDstCode = 0x4e;
		break;
	}

	case XK_D:
	{
		nDstCode = 0x3e;
		break;
	}

	case XK_E:
	{
		nDstCode = 0x29;
		break;
	}

	case XK_F:
	{
		nDstCode = 0x3f;
		break;
	}

	case XK_G:
	{
		nDstCode = 0x40;
		break;
	}

	case XK_H:
	{
		nDstCode = 0x41;
		break;
	}

	case XK_I:
	{
		nDstCode = 0x2e;
		break;
	}

	case XK_J:
	{
		nDstCode = 0x42;
		break;
	}

	case XK_K:
	{
		nDstCode = 0x43;
		break;
	}

	case XK_L:
	{
		nDstCode = 0x44;
		break;
	}

	case XK_M:
	{
		nDstCode = 0x52;
		break;
	}

	case XK_N:
	{
		nDstCode = 0x51;
		break;
	}

	case XK_O:
	{
		nDstCode = 0x2f;
		break;
	}

	case XK_P:
	{
		nDstCode = 0x30;
		break;
	}

	case XK_Q:
	{
		nDstCode = 0x27;
		break;
	}

	case XK_R:
	{
		nDstCode = 0x2a;
		break;
	}

	case XK_S:
	{
		nDstCode = 0x3d;
		break;
	}

	case XK_T:
	{
		nDstCode = 0x2b;
		break;
	}

	case XK_U:
	{
		nDstCode = 0x2d;
		break;
	}

	case XK_V:
	{
		nDstCode = 0x4f;
		break;
	}

	case XK_W:
	{
		nDstCode = 0x28;
		break;
	}

	case XK_X:
	{
		nDstCode = 0x4d;
		break;
	}

	case XK_Y:
	{
		nDstCode = 0x2c;
		break;
	}

	case XK_Z:
	{
		nDstCode = 0x4c;
		break;
	}

	case XK_bracketleft:
	{
		nDstCode = 0x31;
		break;
	}

	case XK_backslash:
	{
		nDstCode = 0x33;
		break;
	}

	case XK_bracketright:
	{
		nDstCode = 0x32;
		break;
	}

	case XK_asciicircum:
	{
		nDstCode = 0x17;
		break;
	}

	case XK_underscore:
	{
		nDstCode = 0x1c;
		break;
	}

	case XK_grave:
	{
		nDstCode = 0x11;
		break;
	}

	case XK_a:
	{
		nDstCode = 0x3c;
		break;
	}

	case XK_b:
	{
		nDstCode = 0x50;
		break;
	}

	case XK_c:
	{
		nDstCode = 0x4e;
		break;
	}

	case XK_d:
	{
		nDstCode = 0x3e;
		break;
	}

	case XK_e:
	{
		nDstCode = 0x29;
		break;
	}

	case XK_f:
	{
		nDstCode = 0x3f;
		break;
	}

	case XK_g:
	{
		nDstCode = 0x40;
		break;
	}

	case XK_h:
	{
		nDstCode = 0x41;
		break;
	}

	case XK_i:
	{
		nDstCode = 0x2e;
		break;
	}

	case XK_j:
	{
		nDstCode = 0x42;
		break;
	}

	case XK_k:
	{
		nDstCode = 0x43;
		break;
	}

	case XK_l:
	{
		nDstCode = 0x44;
		break;
	}

	case XK_m:
	{
		nDstCode = 0x52;
		break;
	}

	case XK_n:
	{
		nDstCode = 0x51;
		break;
	}

	case XK_o:
	{
		nDstCode = 0x2f;
		break;
	}

	case XK_p:
	{
		nDstCode = 0x30;
		break;
	}

	case XK_q:
	{
		nDstCode = 0x27;
		break;
	}

	case XK_r:
	{
		nDstCode = 0x2a;
		break;
	}

	case XK_s:
	{
		nDstCode = 0x3d;
		break;
	}

	case XK_t:
	{
		nDstCode = 0x2b;
		break;
	}

	case XK_u:
	{
		nDstCode = 0x2d;
		break;
	}

	case XK_v:
	{
		nDstCode = 0x4f;
		break;
	}

	case XK_w:
	{
		nDstCode = 0x28;
		break;
	}

	case XK_x:
	{
		nDstCode = 0x4d;
		break;
	}

	case XK_y:
	{
		nDstCode = 0x2c;
		break;
	}

	case XK_z:
	{
		nDstCode = 0x4c;
		break;
	}

	case XK_braceleft:
	{
		nDstCode = 0x31;
		break;
	}

	case XK_bar:
	{
		nDstCode = 0x33;
		break;
	}

	case XK_braceright:
	{
		nDstCode = 0x32;
		break;
	}

	case XK_asciitilde:
	{
		nDstCode = 0x11;
		break;
	}


	case XK_nobreakspace:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_exclamdown:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_cent:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_sterling:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_currency:
	{
		nDstCode = 0x15;
		break;
	}

	case XK_yen:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_brokenbar:
	{
		nDstCode = 0x33;
		break;
	}

	case XK_section:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_diaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_copyright:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ordfeminine:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_guillemotleft:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_notsign:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_hyphen:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_registered:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_macron:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_degree:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_plusminus:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_twosuperior:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_threesuperior:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_acute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_mu:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_paragraph:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_periodcentered:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_cedilla:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_onesuperior:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_masculine:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_guillemotright:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_onequarter:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_onehalf:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_threequarters:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_questiondown:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Agrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Aacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Acircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Atilde:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Adiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Aring:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_AE:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ccedilla:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Egrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Eacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ecircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ediaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Igrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Iacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Icircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Idiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Eth:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ntilde:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ograve:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Oacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ocircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Otilde:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Odiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_multiply:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ooblique:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ugrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Uacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Ucircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Udiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Yacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_Thorn:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ssharp:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_agrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_aacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_acircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_atilde:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_adiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_aring:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ae:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ccedilla:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_egrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_eacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ecircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ediaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_igrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_iacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_icircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_idiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_eth:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ntilde:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ograve:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_oacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ocircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_otilde:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_odiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_division:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_oslash:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ugrave:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_uacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ucircumflex:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_udiaeresis:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_yacute:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_thorn:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ydiaeresis:
	{
		nDstCode = 0x00;
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
		nDstCode = 0x00;
		break;
	}

	case XK_soliddiamond:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_checkerboard:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ht:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_ff:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_cr:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_lf:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_nl:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_vt:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_lowrightcorner:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_uprightcorner:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_upleftcorner:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_lowleftcorner:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_crossinglines:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_horizlinescan1:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_horizlinescan3:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_horizlinescan5:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_horizlinescan7:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_horizlinescan9:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_leftt:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_rightt:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_bott:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_topt:
	{
		nDstCode = 0x00;
		break;
	}

	case XK_vertbar:
	{
		nDstCode = 0x00;
		break;
	}
#endif /* XK_SPECIAL */
	default:
	{
		nDstCode = 0x00;
		break;
	}
    }
    return nDstCode;
}

