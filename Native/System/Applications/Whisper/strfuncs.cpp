/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */

#include <strfuncs.h>

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

/* Copy nSize bytes from pFrom to pTo.  Exlude all characters specified in zExclude. */
char * xstrnxcpy( char *pTo, const char *pFrom, size_t nSize, const char *zExclude )
{
	char f, *p = pTo;
	const char *x;
	bool bCopy = true;

	while( ( '\0' != *pFrom ) && ( --nSize >= 0 ) )
	{
		f = *pFrom++;
		x = zExclude;
		bCopy = true;

		do
		{
			if( f == *x )
				bCopy = false;
		}
		while( *x++ );

		if( bCopy )
			*pTo++ = f;
	}

	return p;
}

/* Search for pStr2 inside pStr1, ignoring case. */
char * xstrncasestr( const char *pStr1, const char *pStr2, size_t nLen )
{
	while( *pStr1 && *pStr2 && nLen-- )
	{
		if( tolower( *pStr1 ) == tolower( *pStr2 ) )
			if( strncasecmp( pStr1, pStr2, strlen( pStr2 ) ) == 0 )
				return (char*)pStr1;

		pStr1++;
	}
	return NULL;
}

char * xstrcasestr( const char *pStr1, const char *pStr2 )
{
	return xstrncasestr( pStr1, pStr2, strlen( pStr1 ) );
}

/* Copy nSize bytes from pFrom to pTo.  Exluding carriage return (\r) */
char * xstrncpy_to_unix( char *pTo, const char *pFrom, size_t nSize )
{
	char *p = pTo;

	while( ( '\0' != *pFrom ) && ( nSize >= 0 ) )
	{
		if( '\r' != *pFrom )
		{
			*pTo++ = *pFrom++;
			--nSize;
		}
		else
			pFrom++;
	}

	return p;
}

/* Copy nSize bytes from pFrom to pTo.  Convert linefeeds (\n) to carriage return/linefeed (\r\n) pairs. */
char * xstrncpy_to_inet( char *pTo, const char *pFrom, size_t nSize )
{
	char *p = pTo;

	while( ( '\0' != *pFrom ) && ( --nSize >= 0 ) )
	{
		if( '\n' == *pFrom )
		{
			*pTo++ = '\r';
			--nSize;
		}
		*pTo++ = *pFrom++;
	}

	return p;
}

