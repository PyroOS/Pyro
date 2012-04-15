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

#include <qp_codec.h>

#include <posix/errno.h>
#include <malloc.h>

ssize_t QpCodec::Decode( const char *pEncoded, ssize_t nEncodedSize, char **ppDecoded )
{
	if( NULL == pEncoded || 0 == nEncodedSize )
	{
		*ppDecoded = NULL;
		return 0;
	}

	char nHex[2] = {0,0};
	char *pBuffer;
	ssize_t nEncoded, nDecoded;

	pBuffer = (char*)calloc( 1, nEncodedSize );
	if( NULL == pBuffer )
		return 0;

	for( nEncoded = nDecoded = 0; nEncoded < nEncodedSize; nEncoded++ )
	{
		if( pEncoded[nEncoded] == '=' )
		{
			nHex[0] = pEncoded[++nEncoded];
			nHex[1] = pEncoded[++nEncoded];

			if( '\r' == nHex[0] && '\n' == nHex[1] )
				continue;	/* Soft line break */

			/* Convert the two values to a printable character and add to decoded data */
			pBuffer[nDecoded++] = HexToChar( nHex );
		}
		else
			pBuffer[nDecoded++] = pEncoded[nEncoded];
	}

	*ppDecoded = pBuffer;
	return nEncodedSize;
}

ssize_t QpCodec::Encode( char **ppEncoded, const char *pDecoded, ssize_t nDecodedSize )
{
	/* XXXKV: Implement */
	return 0;
}

char QpCodec::HexToChar( const char nHex[] )
{
	int nOne = 0, nTwo = 0;

	if( nHex[0] >= '0' && nHex[0] <= '9' )
		nOne = nHex[0] - 48;
	else if( nHex[0] >= 'A' && nHex[0] <= 'F' )
		nOne = nHex[0] - 55;

	if( nHex[1] >= '0' && nHex[1] <= '9' )
		nTwo = nHex[1] - 48;
	else if( nHex[1] >= 'A' && nHex[1] <= 'F' )
		nTwo = nHex[1] - 55;

	return( ( nOne * 16 ) + nTwo );
}

