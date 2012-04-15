/* Copyright (C) 1996, 1997, 2002 Free Software Foundation, Inc.
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
#include <time.h>

#include <pyro/time.h>

/* Pause execution for a number of nanoseconds.  */
int
__libc_nanosleep (const struct timespec *requested_time,
		  struct timespec *remaining)
{
  int ret;

  ret = __snooze( ((bigtime_t)requested_time->tv_sec * 1000000LL) + ((bigtime_t)requested_time->tv_nsec) / 1000L );
  if( NULL != remaining )
    remaining->tv_sec = remaining->tv_nsec = 0;
  return ret;
}
weak_alias (__libc_nanosleep, __nanosleep)
libc_hidden_def (__nanosleep)
weak_alias (__libc_nanosleep, nanosleep)

