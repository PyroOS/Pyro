/*
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Image.h"

#include "BitmapImage.h"
#include "FloatRect.h"
#include "PlatformString.h"
#include "GraphicsContext.h"
#include "NotImplemented.h"
#include "SyllableDebug.h"

#include <gui/bitmap.h>
#include <gui/view.h>
#include <util/application.h>

#include <math.h>


#include <stdio.h>



// This function loads resources from WebKit
Vector<char> loadResourceIntoArray(const char*);

namespace WebCore {

void FrameData::clear()
{
    if (m_frame) {
    	if( os::Application::GetInstance() != NULL )
	    	delete( m_frame );
        m_frame = 0;
        m_duration = 0.0f;
        m_hasAlpha = true;
    }
}

// ================================================
// Image Class
// ================================================



Image* Image::loadPlatformResource(const char* name)
{
    Vector<char> arr = loadResourceIntoArray(name);
    Image* img = new BitmapImage();
    RefPtr<SharedBuffer> buffer = new SharedBuffer(arr.data(), arr.size());
    img->setData(buffer, true);
    return img;
}


void BitmapImage::initPlatformData()
{
}

void BitmapImage::invalidatePlatformData()
{
}

// Drawing Routines
void BitmapImage::draw(GraphicsContext* ctxt, const FloatRect& dst,
                 const FloatRect& src, CompositeOperator op)
{
    os::Bitmap* image = nativeImageForCurrentFrame();
    if (!image) // If it's too early we won't have an image yet.
        return;
        
    if (mayFillWithSolidColor()) {
        fillWithSolidColor(ctxt, dst, solidColor(), op);
        return;
    }

	//DEBUG( "Image::draw() %f %f\n", image->GetBounds().Width(), image->GetBounds().Height() );

    IntSize selfSize = size();
    FloatRect srcRect(src);
    FloatRect dstRect(dst);

    ctxt->save();

    // Set the compositing operation.
    ctxt->setCompositeOperation(op);
    
    os::View* view = ctxt->platformContext();
    
    os::Rect cSrc = srcRect;
    os::Rect cDst = dstRect;
    cSrc.Resize( 0, 0, -1, -1 );
    cDst.Resize( 0, 0, -1, -1 );
    if( !image->GetBounds().Includes( cSrc ) )
    	DEBUG( "Error: Image::draw(): Source size larger than bitmap\n" );
    view->DrawBitmap( image, cSrc & image->GetBounds(), cDst );

#if 0
    QPainter* painter(ctxt->platformContext());

    // Test using example site at
    // http://www.meyerweb.com/eric/css/edge/complexspiral/demo.html    
    painter->drawImage(dst, *image, src);
#endif
    ctxt->restore();

    startAnimation();
}


void Image::drawPattern(GraphicsContext* ctxt, const FloatRect& tileRect, const AffineTransform& patternTransform,
                              const FloatPoint& srcPoint, CompositeOperator op, const FloatRect& dstRect)
{
    os::Bitmap* image = nativeImageForCurrentFrame();
    if (!image) // If it's too early we won't have an image yet.
        return;

    os::Rect cDst = dstRect;
    os::Point cCurrentPos = cDst.LeftTop();
 
    ctxt->save();

    // Set the compositing operation.
    ctxt->setCompositeOperation(op);
    
    IntSize intrinsicImageSize = size();
   
    //DEBUG( "TILED %i %i\n", (int)tileSize.width(), (int)tileSize.height() );
    //DEBUG( "SRC %i %i\n", (int)srcPoint.x(), (int)srcPoint.y() );
    //DEBUG( "IMAGE %i %i\n", (int)intrinsicImageSize.width(), (int)intrinsicImageSize.height() );
   // DEBUG( "DST %i %i %i %i\n", (int)dstRect.x(), (int)dstRect.y(), (int)dstRect.right(), (int)dstRect.bottom() );
   	
	int nSrcX = ( (int)srcPoint.x() ) % intrinsicImageSize.width();
	int nSrcY = ( (int)srcPoint.y() ) % intrinsicImageSize.height();
	int nCurrentHeight = 0;
	
	while( nSrcX < 0 )
		nSrcX += intrinsicImageSize.width();
	if( nSrcY < 0 )
		nSrcY += intrinsicImageSize.height();
		
	
    while( 1 )
    {
    	int nHLeft = int( cDst.right - cCurrentPos.x );
    	int nVLeft = int( cDst.bottom - cCurrentPos.y );
    	
    	nHLeft = std::min( nHLeft, (int)tileRect.width() );
		nHLeft = std::min( nHLeft, intrinsicImageSize.width() - nSrcX + 1 );
		nVLeft = std::min( nVLeft, (int)tileRect.height() );
		nVLeft = std::min( nVLeft, intrinsicImageSize.height() - nSrcY + 1 );
		
		
		
    	if( nHLeft <= 0 )
    	{
    		/* Next line */
    		if( nCurrentHeight <= 0 )
    			break;
    		cCurrentPos.x = cDst.left;
    		cCurrentPos.y += nCurrentHeight;
    		nSrcX = 0;    		
    		nSrcY += nCurrentHeight;

			if( nSrcY >= intrinsicImageSize.height() )
				nSrcY = 0;
			continue;
    	}
    	
    	if( nVLeft <= 0 )
    	{
    		break; // Finished
    	}

//		DEBUG( "%i %i %i %i %i\n", nSrcX, nSrcY, nHLeft, nVLeft, nCurrentHeight );    		
    	nCurrentHeight = nVLeft;
    		

		
		//DEBUG( "-> %i %i %i %i\n", (int)cCurrentPos.x, (int)cCurrentPos.y, nHLeft, nVLeft );
		os::Rect cSrcBlit = os::Rect( nSrcX, nSrcY, nSrcX + nHLeft - 1, nSrcY + nVLeft - 1 )  & image->GetBounds();
		os::Rect cDstBlit = os::Rect( cCurrentPos, cCurrentPos + os::Point( nHLeft - 1, nVLeft - 1 )  );
		
		
		cCurrentPos.x += nHLeft;
		nSrcX += nHLeft;
		if( nSrcX >= intrinsicImageSize.width() )
			nSrcX = 0;
			

		os::View* view = ctxt->platformContext();
		if( cSrcBlit.IsValid() && cDstBlit.IsValid() )		
			view->DrawBitmap( image, cSrcBlit & image->GetBounds(), cDstBlit );
    
    }
    
    ctxt->restore();

   startAnimation();
    

}

void BitmapImage::checkForSolidColor()
{
    // FIXME: It's easy to implement this optimization. Just need to check the RGBA32 buffer to see if it is 1x1.
    m_isSolidColor = false;
}


os::Bitmap* BitmapImage::getPixmap() const
{
    return const_cast<BitmapImage*>(this)->frameAtIndex(0);
}

}

// vim: ts=4 sw=4 et











































