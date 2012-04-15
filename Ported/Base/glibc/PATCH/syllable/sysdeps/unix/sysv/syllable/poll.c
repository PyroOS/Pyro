/* Poll system call, with emulation if it is not available.
   Copyright (C) 1997,1998,1999,2000,2001,2002 Free Software Foundation, Inc.
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
#include <sys/poll.h>

#include <sys/syscall.h>
#include <sysdep.h>

#if defined __NR_poll

extern int __syscall_poll (struct pollfd *__unbounded fds,
			   unsigned int nfds, int timeout);

static int __emulate_poll (struct pollfd *fds, nfds_t nfds,
			   int timeout) internal_function;

/* The real implementation.  */
int
__poll (fds, nfds, timeout)
     struct pollfd *fds;
     unsigned long int nfds;
     int timeout;
{
	static int must_emulate;

	if(!must_emulate) {
		int ret, errno_saved = errno;

		ret = INLINE_SYSCALL(poll, 3, fds, nfds, timeout);
		if (ret >= 0 || errno != -ENOSYS) {
			return (ret);
		} else {
			__set_errno(errno_saved);
			must_emulate = 1;
		}
	}

	ret = __emulate_poll (fds, nfds, timeout);
	return (ret);
}
libc_hidden_def (__poll)
weak_alias (__poll, poll)
strong_alias (__poll, __libc_poll)

/* Get the emulation code.  */
# define __poll(fds, nfds, timeout) \
  static internal_function __emulate_poll (fds, nfds, timeout)
#endif
#include <sysdeps/unix/bsd/poll.c>
