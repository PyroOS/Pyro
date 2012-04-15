/* Copyright (C) 1991,1995,1996,1997,1998,2002 Free Software Foundation, Inc.
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
#include <unistd.h>
#include <sysdep.h>
#include <sys/syscall.h>

/* Fetch the effective group ID, real group ID, and saved-set group ID,
   of the calling process.  */
int
__getresgid (gid_t *egid, gid_t *rgid, gid_t *sgid)
{
  gid_t kegid, krgid, ksgid;
  int res;

  res = INLINE_SYSCALL(getresgid, 3, &krgid, &kegid, &ksgid);
  if( res == 0 )
  {
    *rgid = krgid;
    *egid = kegid;
    *sgid = ksgid;
  }
  return res;
}
libc_hidden_def (__getresgid)
weak_alias (__getresgid, getresgid)

