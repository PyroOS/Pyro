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

#include <rfctime.h>
#include <resources/Whisper.h>

/* Convert the RFC822 date to time_t */
time_t convert_date( os::String cDate )
{
	struct tm sTime;
	time_t nEpoch = 0;
	const char *pStart;
	char *pEnd;

	pStart = cDate.c_str();
	pEnd = strptime( pStart, "%a, %d %b %Y %H:%M:%S %z", &sTime );
	if( pEnd > pStart )
		nEpoch = mktime( &sTime );

	return nEpoch;
}

/* Convert time_t to a displayable (non-RFC822) string */
os::String display_date( time_t nTime )
{
	char zDate[64] = {0};
	struct tm *psTime = gmtime( &nTime );

	strftime( zDate, 64, MSG_MAINWND_MSGLST_DISPLAY_DATE.c_str(), psTime );

	return zDate;
}

