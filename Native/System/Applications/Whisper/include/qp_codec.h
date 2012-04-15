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

#ifndef WHISPER_QP_CODEC_H_
#define WHISPER_QP_CODEC_H_

#include <codec.h>

class QpCodec : public Codec
{
	public:
		ssize_t Decode( const char *pEncoded, ssize_t nEncodedSize, char **ppDecoded );
		ssize_t Encode( char **ppEncoded, const char *pDecoded, ssize_t nDecodedSize );

	private:
		char HexToChar( const char nHex[] );
};

#endif

