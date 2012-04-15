/*
 * Whisper email client for Syllable
 *
 * Copyright (C) 2005-2007 Kristian Van Der Vliet
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef WHISPER_CODEC_H_
#define WHISPER_CODEC_H_

#include <unistd.h>

class Codec
{
	public:
		ssize_t Decode( const char *pEncoded, ssize_t nEncodedSize, char **ppDecoded );
		ssize_t Encode( char **ppEncoded, const char *pDecoded, ssize_t nDecodedSize );
};

#endif

