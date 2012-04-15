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

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#define HOSTNAME_FILE "/etc/hostname"
#define NONAME "*noname*"

int __gethostname( char *name, size_t len )
{
  if( len > 0 )
  {
    int i, file;

    memset( name, '\0', len );

    file = open( HOSTNAME_FILE, O_RDONLY );
    if( file < 0 )
    {
      if( strlen( NONAME ) >= len )
      {
        __set_errno(EINVAL);
        return -1;
      }
      strncpy( name, NONAME, strlen(NONAME) );
    }
    else
    {
      read( file, name, len );
      close( file );
      for( i=0; i < len; i++ )
        if( isspace(name[i]) )
        {
          name[i]='\0';
          break;
        }
    }

    return 0;
  }
  else
  {
    __set_errno(EINVAL);
    return -1;
  }
}
weak_alias(__gethostname,gethostname)

