/* Copyright (C) 1996, 1997, 1998, 1999, 2002 Free Software Foundation, Inc.
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
#include <sysdep.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

/* lseek() is 64bit already on Pyro so lseek64() is identical.  The
   same for off_t/off64_t */

off64_t
__libc_lseek64 (int fd, off64_t offset, int whence)
{
  return (loff_t)__lseek( fd, (off_t)offset, whence);
}
weak_alias (__libc_lseek64, __lseek64)
weak_alias (__libc_lseek64, lseek64)

