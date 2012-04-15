/*
 *  The AtheOS kernel
 *  Copyright (C) 1999  Kurt Skauen
 *  Copyright (C) 2002  Kristian Van Der Vliet (vanders@users.sourceforge.net)
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

#include <posix/errno.h>
#include <posix/stat.h>
#include <posix/fcntl.h>
#include <posix/dirent.h>

#include <pyro/types.h>
#include <pyro/isa_io.h>

#include <pyro/kernel.h>
#include <pyro/device.h>
#include <pyro/irq.h>
#include <pyro/smp.h>
#include <pyro/threads.h>

#include <macros.h>

typedef struct
{
    uint32	nFlags;
} SylVNC_Mouse_FileCookie_s;

typedef struct
{
    thread_id hWaitThread;
    char	    zBuffer[ 256 ];
    atomic_t	    nOutPos;
    atomic_t	    nInPos;
    atomic_t	    nBytesReceived;
    atomic_t	    nOpenCount;
    int	    nDevNum;
    int	    nIrqHandle;
} SylVNC_Mouse_Volume_s;

static SylVNC_Mouse_Volume_s	g_sVolume;

static int g_nDeviceNode = 0;

/*****************************************************************************
 * NAME:
 * DESC:
 * NOTE:
 * SEE ALSO:
 ****************************************************************************/

static int SylVNC_Mouse_Open( void* pNode, uint32 nFlags, void **ppCookie )
{
    int	nError;
	
//    if ( 0 != atomic_read( &g_sVolume.nOpenCount ) )
//    {
//	nError = -EBUSY;
//	printk( "ERROR : Attempt to reopen SylVNC Mouse device\n" );
//    }
//    else
    {
	SylVNC_Mouse_FileCookie_s* psCookie = kmalloc( sizeof( SylVNC_Mouse_FileCookie_s ), MEMF_KERNEL | MEMF_CLEAR );
		
	if ( NULL != psCookie )
	{
	    *ppCookie = psCookie;
	    psCookie->nFlags = nFlags;
		
	    atomic_inc( &g_sVolume.nOpenCount );
	    nError = 0;
	}
	else
	{
	    nError = -ENOMEM;
	}
    }
    return( nError );
}

/*****************************************************************************
 * NAME:
 * DESC:
 * NOTE:
 * SEE ALSO:
 ****************************************************************************/

static int SylVNC_Mouse_Close( void* pNode, void* pCookie )
{
    SylVNC_Mouse_FileCookie_s* 	psCookie = pCookie;
	
    kassertw( NULL != psCookie );
	
    atomic_dec( &g_sVolume.nOpenCount );

    kfree( psCookie );
	
    return( 0 );
}


/*****************************************************************************
 * NAME:
 * DESC:
 * NOTE:
 * SEE ALSO:
 ****************************************************************************/

static int SylVNC_Mouse_Read( void* pNode, void* pCookie, off_t nPos, void* pBuf, size_t nLen )
{
    SylVNC_Mouse_Volume_s* 	psVolume = &g_sVolume;
    int		nError;
    if ( 0 == nLen ) {
	return( 0 );
    }
	
    for ( ;; )
    {
    	if ( atomic_read( &psVolume->nBytesReceived ) > 0 )
	{
	    int	nSize = min( nLen, atomic_read( &psVolume->nBytesReceived ) );
	    int	i;
	    char*	pzBuf = pBuf;

	    for ( i = 0 ; i < nSize ; ++i ) {
		pzBuf[ i ] = psVolume->zBuffer[ atomic_inc_and_read( &psVolume->nOutPos ) & 0xff ];
	    }
	    atomic_sub( &g_sVolume.nBytesReceived, nSize );
			
	    nError = nSize;
	}
	else
	{
	    int	nEFlg = cli();

	    if ( -1 != psVolume->hWaitThread )
	    {
		nError = -EBUSY;
		printk( "ERROR : two threads attempted to read from keyboard device!\n" );
	    }
	    else
	    {
		if (  (( SylVNC_Mouse_FileCookie_s *)pCookie)->nFlags & O_NONBLOCK )
		{
		    nError = -EWOULDBLOCK;
		}
		else
		{
		    psVolume->hWaitThread = sys_get_thread_id(NULL);
		    nError = suspend();
		    psVolume->hWaitThread = -1;
		}
	    }
	    put_cpu_flags( nEFlg );
	}
	if ( 0 != nError ) {
	    break;
	}
    }
    return( nError );
}

static int  SylVNC_Mouse_Write( void* pNode, void* pCookie, off_t nPosition, const void* pBuffer, size_t nSize )
{
	int i;
	for ( i = 0 ; i < nSize ; ++i )
	{
		g_sVolume.zBuffer[ atomic_inc_and_read( &g_sVolume.nInPos ) & 0xff ] = ((char *)pBuffer)[i];
    
		atomic_inc( &g_sVolume.nBytesReceived );

		if ( -1 != g_sVolume.hWaitThread )
		{
			wakeup_thread( g_sVolume.hWaitThread, false );
		}
	}
	return(nSize);
}
/*****************************************************************************
 * NAME:
 * DESC:
 * NOTE:
 * SEE ALSO:
 ****************************************************************************/

DeviceOperations_s g_sOperations = {
    SylVNC_Mouse_Open,
    SylVNC_Mouse_Close,
    NULL,
    SylVNC_Mouse_Read,
    SylVNC_Mouse_Write		/* For Sylvnc to put in mouse movements */
};


/*****************************************************************************
 * NAME:
 * DESC:
 * NOTE:
 * SEE ALSO:
 ****************************************************************************/

status_t device_init( int nDeviceID )
{
    int nError;
    int nHandle;
    printk( "SylVNC_Mouse: device_init() called\n" );

    g_sVolume.hWaitThread    = -1;
    atomic_set( &g_sVolume.nInPos, 0 );
    atomic_set( &g_sVolume.nOutPos, 0 );
    atomic_set( &g_sVolume.nBytesReceived, 0 );
    atomic_set( &g_sVolume.nOpenCount, 0 );
    
	nHandle = register_device( "", "system" );
	claim_device( nDeviceID, nHandle , "SylVNC Mouse", DEVICE_INPUT );
    nError = g_nDeviceNode = create_device_node( nDeviceID, nHandle, "input/sylvnc_mouse", &g_sOperations, NULL );
    
    return( nError );
}

/*****************************************************************************
 * NAME:
 * DESC:
 * NOTE:
 * SEE ALSO:
 ****************************************************************************/

status_t device_uninit( int nDeviceID )
{
    int nError;
    
    printk( "SylVNC Mouse: device_uninit() called\n" );
    nError = delete_device_node( g_nDeviceNode );
    return( 0 );
}

