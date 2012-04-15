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

#include <base64_codec.h>

#include <posix/errno.h>
#include <stdlib.h>

ssize_t Base64Codec::Decode( const char *pEncoded, ssize_t nEncodedSize, char **ppDecoded )
{
	if( NULL == pEncoded || 0 == nEncodedSize )
	{
		*ppDecoded = NULL;
		return 0;
	}

	/* You might need to think backwards to understand this table.  It is a table
	   of all 64 characters in the base64 alphabet, placed into the table according
	   to their ASCII value.  The data in each entry is the value assigned to the
	   character E.g. '+' = 62.  The loop can then simply use the byte taken from
	   pEncoded and use it as an index into this table, which gives it an integer
	   value. */
    int i;

    for (i = 0; i < 255; i++)
        dtable[i] = 0x80;

    for (i = 'A'; i <= 'Z'; i++)
        dtable[i] = 0 + (i - 'A');

    for (i = 'a'; i <= 'z'; i++)
        dtable[i] = 26 + (i - 'a');

    for (i = '0'; i <= '9'; i++)
        dtable[i] = 52 + (i - '0');

	/* Cast to avoid warning from g++  Darn strict typing */
    dtable[(int)'+'] = 62;
    dtable[(int)'/'] = 63;
    dtable[(int)'='] = 0;

	/* Calculate & allocate pBuffer.  Decoding base64 produces three output bytes
	   for every four input bytes. */
	int nBufferSize = (int)( ( nEncodedSize / 4.0 ) * 3.0 );
	nBufferSize += 3 - ( nBufferSize % 3 );

	char *pBuffer = (char*)calloc( 1, nBufferSize + 1 );

	if( NULL == pBuffer )
	{
		*ppDecoded = NULL;
		return 0;
	}

	/* I was going to re-write the entire code using "sensible" variable names,
	 * but that just made the code messy and hard to read.  The one-character
	 * variables are used as follows:
	 *
	 * a[]  : Four raw bytes from pEncoded
	 * b[]  : Values for the four bytes from pEncoded, from dtable
	 * c	: Single byte from pEncoded, put into a[] & b[] if valid
	 * o[]	: Three bytes, decoded from the four bytes in b[]
	 * i, j : Loop counters
	 * in	: Current byte in pEncoded
	 * out	: Current byte in pBuffer
	 */

	int j, c = 0;
	ssize_t in = 0, out = 0;

    while( in < nEncodedSize )
	{
        unsigned char a[4], b[4], o[3];

		/* Junk in the buffers is bad */
		a[0] = a[1] = a[2] = a[3] = '=';
		b[0] = b[1] = b[2] = b[3] = 0;

        for (i = 0; i < 4; i++)
		{
			/* Pull the next byte from pEncoded, but skip any ASCII control
			   characters E.g. /r, whitespace etc. */
			while( ( c = pEncoded[in++] ) <= ' ' && in < nEncodedSize )
				;

			/* If we ran out of data we'd best stop now */
			if( in == nEncodedSize )
				break;

			/* Check for but ignore any character which is not
			   in the Base64 alphabet */
            if (dtable[c] & 0x80)
			{
                i--;
                continue;
            }

            a[i] = c;
            b[i] = (char)dtable[c];
        }
        o[0] = (b[0] << 2) | (b[1] >> 4);
        o[1] = (b[1] << 4) | (b[2] >> 2);
        o[2] = (b[2] << 6) | b[3];
        i = a[2] == '=' ? 1 : (a[3] == '=' ? 2 : 3);

		/* Put into pBuffer */
		for( j = 0; j < i; j++ )
			pBuffer[out++] = o[j];

        if (i < 3)
            break;
    }

	/* Set ppDecoded & return size */
	*ppDecoded = pBuffer;
	return out;
}

ssize_t Base64Codec::Encode( char **ppEncoded, const char *pDecoded, ssize_t nDecodedSize )
{
	if( NULL == pDecoded || 0 == nDecodedSize )
	{
		*ppEncoded = NULL;
		return 0;
	}

	int i;

	/* Setup dtable.  See Decode() for more information */
	for (i = 0; i < 9; i++)
	{
		dtable[i] = 'A' + i;
		dtable[i + 9] = 'J' + i;
		dtable[26 + i] = 'a' + i;
		dtable[26 + i + 9] = 'j' + i;
	}
	for (i = 0; i < 8; i++)
	{ 
		dtable[i + 18] = 'S' + i;
		dtable[26 + i + 18] = 's' + i;
	}
	for (i = 0; i < 10; i++)
		dtable[52 + i] = '0' + i;
	dtable[62] = '+';
	dtable[63] = '/';

	/* Calculate & allocate pBuffer.  Encoding base64 produces four output bytes
	   for every three input bytes.  The output is padded, so the buffer must be a round multiple of 4 */
	int nBufferSize = (int)( ( nDecodedSize / 3.0 ) * 4.0 );
	nBufferSize += 4 - ( nBufferSize % 4 );

	/* The output buffer also needs to allow for one CRLF pair every 76 characters */
	nBufferSize += (( nBufferSize / 76 ) + 1 ) * 2;

	char *pBuffer = (char*)calloc( 1, nBufferSize + 1 );
	if( NULL == pBuffer )
	{
		*ppEncoded = NULL;
		return 0;
	}

	ssize_t in = 0, out = 0;
	int r = 0;

	while ( in < nDecodedSize )
	{
		unsigned char igroup[3], ogroup[4];
		int n;

		igroup[0] = igroup[1] = igroup[2] = 0;

		/* Pull the next group of 3 bytes from pDecoded */
		for (n = 0; n < 3 && in < nDecodedSize; n++)
			igroup[n] = pDecoded[in++];

		if( n > 0 )
		{
			/* Encode */
			ogroup[0] = dtable[igroup[0] >> 2];
			ogroup[1] = dtable[((igroup[0] & 3) << 4) | (igroup[1] >> 4)];
			ogroup[2] = dtable[((igroup[1] & 0xF) << 2) | (igroup[2] >> 6)];
			ogroup[3] = dtable[igroup[2] & 0x3F];

			/* Replace characters in output stream with "=" pad
			   characters if fewer than three characters were
			   read from the end of the input stream. */

			if (n < 3)
			{
				ogroup[3] = '=';
				if (n < 2)
					ogroup[2] = '=';
			}

			/* Put into pBuffer.  Wrap the output every 76 characters. */
			for (i = 0; i < 4; i++, r++)
			{
				if( r > 76 )
				{
					pBuffer[out++] = '\r';
					pBuffer[out++] = '\n';
					r = 0;
				}
				pBuffer[out++] = ogroup[i];
			}
		}
	}

	/* Set ppEncoded & return size */
	*ppEncoded = pBuffer;
	return out;
}
