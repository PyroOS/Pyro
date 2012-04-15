/* libsmtp 0.4 -:-  (C)opyright 2001 - 2007 Kristian Van Der Vliet
/
/  This library is free software; you can redistribute it and/or
/  modify it under the terms of the GNU Library General Public
/  License as published by the Free Software Foundation; either
/  version 2 of the License, or (at your option) any later version.
/
/  This library is distributed in the hope that it will be useful,
/  but WITHOUT ANY WARRANTY; without even the implied warranty of
/  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/  Library General Public License for more details.
/
/  You should have received a copy of the GNU Library General Public
/  License along with this library; if not, write to the Free
/  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
/  MA 02111-1307, USA
*/

#include <base64.h>
#include <unistd.h>
#include <malloc.h>

ssize_t base64_decode( const char *encoded, size_t encoded_size, char **decoded )
{
	if( NULL == encoded || 0 == encoded_size )
	{
		*decoded = NULL;
		return 0;
	}

	/* You might need to think backwards to understand this table.  It is a table
	   of all 64 characters in the base64 alphabet, placed into the table according
	   to their ASCII value.  The data in each entry is the value assigned to the
	   character E.g. '+' = 62.  The loop can then simply use the byte taken from
	   pEncoded and use it as an index into this table, which gives it an integer
	   value. */
	char dtable[256];
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

	/* Calculate & allocate buffer.  Decoding base64 produces three output bytes
	   for every four input bytes. */
	int buffer_size = (int)( ( encoded_size / 4.0 ) * 3.0 );
	buffer_size += 3 - ( buffer_size % 3 );

	char *buffer = (char*)calloc( 1, buffer_size + 1 );

	if( NULL == buffer )
	{
		*decoded = NULL;
		return 0;
	}

	/* I was going to re-write the entire code using "sensible" variable names,
	 * but that just made the code messy and hard to read.  The one-character
	 * variables are used as follows:
	 *
	 * a[]  : Four raw bytes from encoded
	 * b[]  : Values for the four bytes from encoded, from dtable
	 * c	: Single byte from encoded, put into a[] & b[] if valid
	 * o[]	: Three bytes, decoded from the four bytes in b[]
	 * i, j : Loop counters
	 * in	: Current byte in encoded
	 * out	: Current byte in buffer
	 */

	int j, c = 0;
	size_t in = 0, out = 0;

    while( in < encoded_size )
	{
        unsigned char a[4], b[4], o[3];

		/* Junk in the buffers is bad */
		a[0] = a[1] = a[2] = a[3] = '=';
		b[0] = b[1] = b[2] = b[3] = 0;

        for (i = 0; i < 4; i++)
		{
			/* Pull the next byte from encoded, but skip any ASCII control
			   characters E.g. /r, whitespace etc. */
			while( ( c = encoded[in++] ) <= ' ' && in < encoded_size )
				;

			/* If we ran out of data we'd best stop now */
			if( in == encoded_size )
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

		/* Put into buffer */
		for( j = 0; j < i; j++ )
			buffer[out++] = o[j];

        if (i < 3)
            break;
    }

	/* Set decoded & return size */
	*decoded = buffer;
	return out;
}

ssize_t base64_encode( char **encoded, const char *decoded, size_t decoded_size )
{
	if( NULL == decoded || 0 == decoded_size )
	{
		*encoded = NULL;
		return 0;
	}

	char dtable[256];
	int i;

	/* Setup dtable.  See base64_decode() for more information */
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

	/* Calculate & allocate buffer.  Encoding base64 produces four output bytes
	   for every three input bytes.  The output is padded, so the buffer must be a round multiple of 4 */
	int buffer_size = (int)( ( decoded_size / 3.0 ) * 4.0 );
	buffer_size += 4 - ( buffer_size % 4 );

	/* The output buffer also needs to allow for one CRLF pair every 76 characters */
	buffer_size += (( buffer_size / 76 ) + 1 ) * 2;

	char *buffer = (char*)calloc( 1, buffer_size + 1 );
	if( NULL == buffer )
	{
		*encoded = NULL;
		return 0;
	}

	size_t in = 0, out = 0;
	int r = 0;

	while ( in < decoded_size )
	{
		unsigned char igroup[3], ogroup[4];
		int n;

		igroup[0] = igroup[1] = igroup[2] = 0;

		/* Pull the next group of 3 bytes from decoded */
		for (n = 0; n < 3 && in < decoded_size; n++)
			igroup[n] = decoded[in++];

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

			/* Put into buffer.  Wrap the output every 76 characters. */
			for (i = 0; i < 4; i++, r++)
			{
				if( r > 76 )
				{
					buffer[out++] = '\r';
					buffer[out++] = '\n';
					r = 0;
				}
				buffer[out++] = ogroup[i];
			}
		}
	}

	/* Set encoded & return size */
	*encoded = buffer;
	return out;
}

