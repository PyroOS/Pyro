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

#ifndef _SYS_SYSINFO_H
#define _SYS_SYSINFO_H 1

#include <pyro/kernel.h>

/* External functions */
__BEGIN_DECLS

/* pyro/kernel.h defines a get_system_info macro wrapping get_system_info_v() with the current SYS_INFO_VERSION. */
#undef get_system_info

extern status_t get_system_info( system_info* info ) __THROW;

extern status_t get_system_info_v( system_info* info, int nVersion ) __THROW;


/* Now redefine the get_system_info macro */
#define get_system_info( psInfo ) get_system_info_v( psInfo, SYS_INFO_VERSION )

__END_DECLS

#endif /* _SYS_SYSINFO_H */
