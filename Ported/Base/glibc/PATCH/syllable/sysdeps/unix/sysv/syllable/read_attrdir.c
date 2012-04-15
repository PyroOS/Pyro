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
#include <dirent.h>
#include <string.h>
#include <sysdep.h>
#include <sys/syscall.h>

#include <pyro/kernel.h>

struct dirent* __read_attrdir( DIR *dir )
{
  struct kernel_dirent kdir;
  int error;
  
  if( NULL == dir )
  {
  	__set_errno(EINVAL);
    return NULL;
  }   

  error = INLINE_SYSCALL(read_attrdir, 3, dir->fd, &kdir, sizeof(kdir));
  if( 1 == error )
  {
    struct dirent *entry = (struct dirent*)dir->data;

    entry->d_ino = kdir.d_ino;
    entry->d_off = 0;
    entry->d_namlen = kdir.d_namlen;
    entry->d_reclen = sizeof(struct dirent);
    entry->d_type = DT_UNKNOWN;
    memcpy(entry->d_name, kdir.d_name, kdir.d_namlen);
    entry->d_name[kdir.d_namlen] = '\0';

    return entry;
  }
  else if( 0 == error )
    __set_errno( 0 );	/* Should this be ENOENT instead? */
  else
    __set_errno(ESYSCFAILED);	/* Syscall doesn't provide a useful errno */

  return NULL;
}
weak_alias(__read_attrdir,read_attrdir)




