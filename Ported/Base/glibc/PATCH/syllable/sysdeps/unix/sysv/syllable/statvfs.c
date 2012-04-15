/* Return information about the filesystem on which FILE resides.
   Copyright (C) 1998, 2002 Free Software Foundation, Inc.
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
#include <sys/statvfs.h>
#include <stddef.h>
#include <fcntl.h>

/* Return information about the filesystem on which FILE resides.  */
int
__statvfs (const char *file, struct statvfs *buf)
{
  int save_errno, error, fd;
  
  fd = __open(file,O_RDONLY);
  if( fd < 0 )
    return fd;

  /* fstatvfs does the real work */
  error = fstatvfs(fd,buf);

  /* Don't clobber errno when we close fd */
  save_errno = errno;
  __close(fd);
  __set_errno(save_errno);

  return error;
}
weak_alias (__statvfs, statvfs)
libc_hidden_weak (statvfs)


