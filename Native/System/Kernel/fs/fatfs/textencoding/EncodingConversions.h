#ifndef __F_ENCODINGCONVERSIONS_H__
#define __F_ENCODINGCONVERSIONS_H__

#include <atheos/types.h>
status_t convert_to_utf8( uint32 srcEncoding, const char *src, int32 *srcLen, char *dst, int32 *dstLen, int32 *state, char substitute = '?' );
status_t convert_from_utf8( uint32 dstEncoding, const char *src, int32 *srcLen, char *dst, int32 *dstLen, int32 *state, char substitute = '?' );

#endif // __F_ENCODINGCONVERSIONS_H__
