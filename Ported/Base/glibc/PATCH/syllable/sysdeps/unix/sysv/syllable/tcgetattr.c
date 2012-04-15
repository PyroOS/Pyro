/* Copyright (C) 1992, 1995, 1997, 1998, 2003 Free Software Foundation, Inc.
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
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/debug.h>
#include <pyro/types.h>

/* Put the state of FD into *TERMIOS_P.  */
int
__tcgetattr (fd, termios_p)
      int fd;
      struct termios *termios_p;
{
	if (NULL == termios_p)
	{
		dbprintf ("tcgetattr() called with termios_p == NULL\n");
		__set_errno (EINVAL);
		return (-1);
	}
	return (ioctl (fd, TCGETA, termios_p ) );
}
weak_alias (__tcgetattr, tcgetattr)
