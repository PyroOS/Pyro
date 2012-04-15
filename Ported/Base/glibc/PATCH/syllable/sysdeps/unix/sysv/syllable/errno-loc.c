/* Thread-safe function to get address of errno variable

   Copyright (C) 1996, 1997, 1998, 1999, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <errno.h>
#include <tls.h>
#include <pyro/tld.h>

#if ! USE___THREAD
#undef errno
extern int errno;
#endif

int *
#if ! USE___THREAD
weak_const_function
#endif
__errno_location( void )
{
	int *errno_addr;
	__asm__( "movl %%gs:(%1),%0" : "=r" (errno_addr) : "r" (TLD_ERRNO_ADDR) );
	return( errno_addr );
}
libc_hidden_def(__errno_location);
