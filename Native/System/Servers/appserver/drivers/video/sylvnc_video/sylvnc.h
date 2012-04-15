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

#ifndef	__F_SYLVNCDRV_H__
#define	__F_SYLVNCDRV_H__

#include <signal.h>

#include <pyro/areas.h>
#include <pyro/kernel.h>
#include <pyro/pci.h>
#include <pyro/types.h>

#include <gui/rect.h>

#include "../../../server/ddriver.h"
#include "../../../server/sprite.h"

#include <rfb/rfbconfig.h>
#include <rfb/rfb.h>

#define SYLVNC_MOUSE_EVENT_SIG 0x564E434D /* VNCM */

struct SylVNCMouseEvent
{
	int nSignature;
	int nButtons;
	int x;
	int y;
};

struct VNCMode : public os::screen_mode
 {
    VNCMode( int w, int h, int bbl, os::color_space cs, float rf, int mode, uint32 fb ) : os::screen_mode( w,h,bbl,cs, rf ) { m_nVNCMode = mode; m_nFrameBuffer = fb; }
    int	   m_nVNCMode;
    uint32 m_nFrameBuffer;
};

class SylVNC : public DisplayDriver
{
public:

    SylVNC();
    ~SylVNC();

    area_id	Open();
    void	Close();

    virtual int	 GetScreenModeCount();
    virtual bool GetScreenModeDesc( int nIndex, os::screen_mode* psMode );
  
    int		SetScreenMode( os::screen_mode sMode );
    os::screen_mode GetCurrentScreenMode();
   
    virtual int		    GetFramebufferOffset() { return( m_nFrameBufferOffset ); }

	bool		IntersectWithMouse( const os::IRect& cRect );

/* Added the following so libvncserver can get updates... */
	virtual bool DrawLine(SrvBitmap* psBitMap, const os::IRect& cClipRect, const os::IPoint& cPnt1, const os::IPoint& cPnt2, const os::Color32_s& sColor, int nMode);
	virtual bool FillRect(SrvBitmap* psBitMap, const os::IRect& cRect, const os::Color32_s& sColor);
	virtual bool BltBitmap(SrvBitmap* pcDstBitMap, SrvBitmap* pcSrcBitMap, os::IRect cSrcRect, os::IPoint cDstPos, int nMode);

	virtual void RenderGlyph(SrvBitmap *pcBitmap, Glyph* pcGlyph, const os::IPoint& cPos, const os::IRect& cClipRect, const os::Color32_s& sFgColor);
	virtual void RenderGlyphBlend(SrvBitmap *pcBitmap, Glyph* pcGlyph, const os::IPoint& cPos, const os::IRect& cClipRect, const os::Color32_s& sFgColor);
	virtual void RenderGlyph(SrvBitmap *pcBitmap, Glyph* pcGlyph, const os::IPoint& cPos, const os::IRect& cClipRect, const uint32* anPallette);

	bool IsInitiated() const
	{
		return(m_bIsInitiated);
	}
/* End libvncserver additions for now */


private:

/* Added the following so libvncserver can get updates... */
	bool m_bIsInitiated;
	os::Locker m_cRFBLock;
	rfbScreenInfoPtr rfbScreen;
	thread_id rfbEventThread;
	static void SylVNC::RFBEventThread( SylVNC * pDriver );
	void * m_pFrameBuffer;
	static void SylVNC::RFBKbdEventHandler(rfbBool down, rfbKeySym key, _rfbClientRec * );
	static void SylVNC::RFBMouseEventHandler( int nButtonMask, int nXMotion, int nYMotion, _rfbClientRec *);
	static uint8 SylVNC::TranslateKeyCode( rfbKeySym XKeySym );
/* End libvncserver additions for now */

    bool		InitModes();
    bool		SetVNCMode( uint32 nMode );

    int	   m_nCurrentMode;
    uint32 m_nFrameBufferSize;
    int	   m_nFrameBufferOffset;
    std::vector<VNCMode> m_cModeList;
};

#endif // __F_SYLVNCDRV_H__
