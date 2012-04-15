/*
 * yuv2rgb.c, Software YUV to RGB coverter
 *
 *  Copyright (C) 1999, Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *  All Rights Reserved.
 *
 *  Functions broken out from display_x11.c and several new modes
 *  added by H�kan Hjort <d95hjort@dtek.chalmers.se>
 *
 *  15 & 16 bpp support by Franck Sicard <Franck.Sicard@solsoft.fr>
 *
 *  This file is part of mpeg2dec, a free MPEG-2 video decoder
 *
 *  mpeg2dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  mpeg2dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * MMX/MMX2 Template stuff from Michael Niedermayer (michaelni@gmx.at) (needed for fast movntq support)
 * 1,4,8bpp support by Michael Niedermayer (michaelni@gmx.at)
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mplayercomp.h"
//#include "config.h"
//#include "video_out.h"
#include "rgb2rgb.h"
#include "cpudetect.h"
#include "mangle.h"
//#include "../mp_msg.h"

#ifdef HAVE_MLIB
#include "yuv2rgb_mlib.c"
#endif

#define DITHER1XBPP // only for mmx

#ifdef ARCH_X86
#define CAN_COMPILE_X86_ASM
#endif

const uint8_t  __attribute__((aligned(8))) dither_2x2_4[2][8]={
{  1,   3,   1,   3,   1,   3,   1,   3, },
{  2,   0,   2,   0,   2,   0,   2,   0, },
};

const uint8_t  __attribute__((aligned(8))) dither_2x2_8[2][8]={
{  6,   2,   6,   2,   6,   2,   6,   2, },
{  0,   4,   0,   4,   0,   4,   0,   4, },
};

const uint8_t  __attribute__((aligned(8))) dither_8x8_32[8][8]={
{ 17,   9,  23,  15,  16,   8,  22,  14, },
{  5,  29,   3,  27,   4,  28,   2,  26, },
{ 21,  13,  19,  11,  20,  12,  18,  10, },
{  0,  24,   6,  30,   1,  25,   7,  31, },
{ 16,   8,  22,  14,  17,   9,  23,  15, },
{  4,  28,   2,  26,   5,  29,   3,  27, },
{ 20,  12,  18,  10,  21,  13,  19,  11, },
{  1,  25,   7,  31,   0,  24,   6,  30, },
};

#if 0
const uint8_t  __attribute__((aligned(8))) dither_8x8_64[8][8]={
{  0,  48,  12,  60,   3,  51,  15,  63, },
{ 32,  16,  44,  28,  35,  19,  47,  31, },
{  8,  56,   4,  52,  11,  59,   7,  55, },
{ 40,  24,  36,  20,  43,  27,  39,  23, },
{  2,  50,  14,  62,   1,  49,  13,  61, },
{ 34,  18,  46,  30,  33,  17,  45,  29, },
{ 10,  58,   6,  54,   9,  57,   5,  53, },
{ 42,  26,  38,  22,  41,  25,  37,  21, },
};
#endif

const uint8_t  __attribute__((aligned(8))) dither_8x8_73[8][8]={
{  0,  55,  14,  68,   3,  58,  17,  72, },
{ 37,  18,  50,  32,  40,  22,  54,  35, },
{  9,  64,   5,  59,  13,  67,   8,  63, },
{ 46,  27,  41,  23,  49,  31,  44,  26, },
{  2,  57,  16,  71,   1,  56,  15,  70, },
{ 39,  21,  52,  34,  38,  19,  51,  33, },
{ 11,  66,   7,  62,  10,  65,   6,  60, },
{ 48,  30,  43,  25,  47,  29,  42,  24, },
};

#if 0
const uint8_t  __attribute__((aligned(8))) dither_8x8_128[8][8]={
{ 68,  36,  92,  60,  66,  34,  90,  58, },
{ 20, 116,  12, 108,  18, 114,  10, 106, },
{ 84,  52,  76,  44,  82,  50,  74,  42, },
{  0,  96,  24, 120,   6, 102,  30, 126, },
{ 64,  32,  88,  56,  70,  38,  94,  62, },
{ 16, 112,   8, 104,  22, 118,  14, 110, },
{ 80,  48,  72,  40,  86,  54,  78,  46, },
{  4, 100,  28, 124,   2,  98,  26, 122, },
};
#endif

#if 1
const uint8_t  __attribute__((aligned(8))) dither_8x8_220[8][8]={
{117,  62, 158, 103, 113,  58, 155, 100, },
{ 34, 199,  21, 186,  31, 196,  17, 182, },
{144,  89, 131,  76, 141,  86, 127,  72, },
{  0, 165,  41, 206,  10, 175,  52, 217, },
{110,  55, 151,  96, 120,  65, 162, 107, },
{ 28, 193,  14, 179,  38, 203,  24, 189, },
{138,  83, 124,  69, 148,  93, 134,  79, },
{  7, 172,  48, 213,   3, 168,  45, 210, },
};
#elif 1
// tries to correct a gamma of 1.5
const uint8_t  __attribute__((aligned(8))) dither_8x8_220[8][8]={
{  0, 143,  18, 200,   2, 156,  25, 215, },
{ 78,  28, 125,  64,  89,  36, 138,  74, },
{ 10, 180,   3, 161,  16, 195,   8, 175, },
{109,  51,  93,  38, 121,  60, 105,  47, },
{  1, 152,  23, 210,   0, 147,  20, 205, },
{ 85,  33, 134,  71,  81,  30, 130,  67, },
{ 14, 190,   6, 171,  12, 185,   5, 166, },
{117,  57, 101,  44, 113,  54,  97,  41, },
};
#elif 1
// tries to correct a gamma of 2.0
const uint8_t  __attribute__((aligned(8))) dither_8x8_220[8][8]={
{  0, 124,   8, 193,   0, 140,  12, 213, },
{ 55,  14, 104,  42,  66,  19, 119,  52, },
{  3, 168,   1, 145,   6, 187,   3, 162, },
{ 86,  31,  70,  21,  99,  39,  82,  28, },
{  0, 134,  11, 206,   0, 129,   9, 200, },
{ 62,  17, 114,  48,  58,  16, 109,  45, },
{  5, 181,   2, 157,   4, 175,   1, 151, },
{ 95,  36,  78,  26,  90,  34,  74,  24, },
};
#else
// tries to correct a gamma of 2.5
const uint8_t  __attribute__((aligned(8))) dither_8x8_220[8][8]={
{  0, 107,   3, 187,   0, 125,   6, 212, },
{ 39,   7,  86,  28,  49,  11, 102,  36, },
{  1, 158,   0, 131,   3, 180,   1, 151, },
{ 68,  19,  52,  12,  81,  25,  64,  17, },
{  0, 119,   5, 203,   0, 113,   4, 195, },
{ 45,   9,  96,  33,  42,   8,  91,  30, },
{  2, 172,   1, 144,   2, 165,   0, 137, },
{ 77,  23,  60,  15,  72,  21,  56,  14, },
};
#endif

#ifdef CAN_COMPILE_X86_ASM

/* hope these constant values are cache line aligned */
uint64_t __attribute__((aligned(8))) mmx_80w = 0x0080008000800080;
uint64_t __attribute__((aligned(8))) mmx_10w = 0x1010101010101010;
uint64_t __attribute__((aligned(8))) mmx_00ffw = 0x00ff00ff00ff00ff;
uint64_t __attribute__((aligned(8))) mmx_Y_coeff = 0x253f253f253f253f;

/* hope these constant values are cache line aligned */
uint64_t __attribute__((aligned(8))) mmx_U_green = 0xf37df37df37df37d;
uint64_t __attribute__((aligned(8))) mmx_U_blue = 0x4093409340934093;
uint64_t __attribute__((aligned(8))) mmx_V_red = 0x3312331233123312;
uint64_t __attribute__((aligned(8))) mmx_V_green = 0xe5fce5fce5fce5fc;

/* hope these constant values are cache line aligned */
uint64_t __attribute__((aligned(8))) mmx_redmask = 0xf8f8f8f8f8f8f8f8;
uint64_t __attribute__((aligned(8))) mmx_grnmask = 0xfcfcfcfcfcfcfcfc;

uint64_t __attribute__((aligned(8))) M24A=   0x00FF0000FF0000FFLL;
uint64_t __attribute__((aligned(8))) M24B=   0xFF0000FF0000FF00LL;
uint64_t __attribute__((aligned(8))) M24C=   0x0000FF0000FF0000LL;

// the volatile is required because gcc otherwise optimizes some writes away not knowing that these
// are read in the asm block
volatile uint64_t __attribute__((aligned(8))) b5Dither;
volatile uint64_t __attribute__((aligned(8))) g5Dither;
volatile uint64_t __attribute__((aligned(8))) g6Dither;
volatile uint64_t __attribute__((aligned(8))) r5Dither;

uint64_t __attribute__((aligned(8))) dither4[2]={
	0x0103010301030103LL,
	0x0200020002000200LL,};

uint64_t __attribute__((aligned(8))) dither8[2]={
	0x0602060206020602LL,
	0x0004000400040004LL,};

#undef HAVE_MMX
#undef ARCH_X86

//MMX versions
#undef RENAME
#define HAVE_MMX
#undef HAVE_MMX2
#undef HAVE_3DNOW
#define ARCH_X86
#define RENAME(a) a ## _MMX
#include "yuv2rgb_template.c"

//MMX2 versions
#undef RENAME
#define HAVE_MMX
#define HAVE_MMX2
#undef HAVE_3DNOW
#define ARCH_X86
#define RENAME(a) a ## _MMX2
#include "yuv2rgb_template.c"

#endif // CAN_COMPILE_X86_ASM

uint32_t matrix_coefficients = 6;

const int32_t Inverse_Table_6_9[8][4] = {
    {117504, 138453, 13954, 34903}, /* no sequence_display_extension */
    {117504, 138453, 13954, 34903}, /* ITU-R Rec. 709 (1990) */
    {104597, 132201, 25675, 53279}, /* unspecified */
    {104597, 132201, 25675, 53279}, /* reserved */
    {104448, 132798, 24759, 53109}, /* FCC */
    {104597, 132201, 25675, 53279}, /* ITU-R Rec. 624-4 System B, G */
    {104597, 132201, 25675, 53279}, /* SMPTE 170M */
    {117579, 136230, 16907, 35559}  /* SMPTE 240M (1987) */
};

void *yuv2rgb_c_init (unsigned bpp, int mode, void *table_rV[256], void *table_gU[256], int table_gV[256], void *table_bU[256]);

yuv2rgb_fun yuv2rgb= NULL;

static void (* yuv2rgb_c_internal) (uint8_t *, uint8_t *,
				    uint8_t *, uint8_t *,
				    void *, void *, int, int);

static void yuv2rgb_c (void * dst, uint8_t * py,
		       uint8_t * pu, uint8_t * pv,
		       unsigned h_size, unsigned v_size,
		       unsigned rgb_stride, unsigned y_stride, unsigned uv_stride)
{
    v_size >>= 1;

    while (v_size--) {
	yuv2rgb_c_internal (py, py + y_stride, pu, pv, dst, dst + rgb_stride,
			    h_size, v_size<<1);

	py += 2 * y_stride;
	pu += uv_stride;
	pv += uv_stride;
	dst += 2 * rgb_stride;
    }
}

void * table_rV[256];
void * table_gU[256];
int table_gV[256];
void * table_bU[256];

void yuv2rgb_init (unsigned bpp, int mode)
{
    if(yuv2rgb) return;
#ifdef CAN_COMPILE_X86_ASM
    if(gCpuCaps.hasMMX2)
    {
	if (yuv2rgb == NULL /*&& (config.flags & VO_MMX_ENABLE)*/) {
		yuv2rgb = yuv2rgb_init_MMX2 (bpp, mode);
		if (yuv2rgb != NULL)
			mp_msg(MSGT_SWS,MSGL_INFO,"Using MMX2 for colorspace transform\n");
		else
			mp_msg(MSGT_SWS,MSGL_WARN,"Cannot init MMX2 colorspace transform\n");
	}
    }
    else if(gCpuCaps.hasMMX)
    {
	if (yuv2rgb == NULL /*&& (config.flags & VO_MMX_ENABLE)*/) {
		yuv2rgb = yuv2rgb_init_MMX (bpp, mode);
		if (yuv2rgb != NULL)
			mp_msg(MSGT_SWS,MSGL_INFO,"Using MMX for colorspace transform\n");
		else
			mp_msg(MSGT_SWS,MSGL_WARN,"Cannot init MMX colorspace transform\n");
	}
    }
#endif
#ifdef HAVE_MLIB
    if (yuv2rgb == NULL /*&& (config.flags & VO_MLIB_ENABLE)*/) {
	yuv2rgb = yuv2rgb_init_mlib (bpp, mode);
	if (yuv2rgb != NULL)
	    mp_msg(MSGT_SWS,MSGL_INFO,"Using mlib for colorspace transform\n");
    }
#endif
    if (yuv2rgb == NULL) {
	mp_msg(MSGT_SWS,MSGL_INFO,"No accelerated colorspace conversion found\n");
	yuv2rgb_c_init (bpp, mode, table_rV, table_gU, table_gV, table_bU);
	yuv2rgb = (yuv2rgb_fun)yuv2rgb_c;
    }
}

#define RGB(i)					\
	U = pu[i];				\
	V = pv[i];				\
	r = table_rV[V];			\
	g = table_gU[U] + table_gV[V];		\
	b = table_bU[U];

#define DST1(i)					\
	Y = py_1[2*i];				\
	dst_1[2*i] = r[Y] + g[Y] + b[Y];	\
	Y = py_1[2*i+1];			\
	dst_1[2*i+1] = r[Y] + g[Y] + b[Y];

#define DST2(i)					\
	Y = py_2[2*i];				\
	dst_2[2*i] = r[Y] + g[Y] + b[Y];	\
	Y = py_2[2*i+1];			\
	dst_2[2*i+1] = r[Y] + g[Y] + b[Y];

#define DST1RGB(i)							\
	Y = py_1[2*i];							\
	dst_1[6*i] = r[Y]; dst_1[6*i+1] = g[Y]; dst_1[6*i+2] = b[Y];	\
	Y = py_1[2*i+1];						\
	dst_1[6*i+3] = r[Y]; dst_1[6*i+4] = g[Y]; dst_1[6*i+5] = b[Y];

#define DST2RGB(i)							\
	Y = py_2[2*i];							\
	dst_2[6*i] = r[Y]; dst_2[6*i+1] = g[Y]; dst_2[6*i+2] = b[Y];	\
	Y = py_2[2*i+1];						\
	dst_2[6*i+3] = r[Y]; dst_2[6*i+4] = g[Y]; dst_2[6*i+5] = b[Y];

#define DST1BGR(i)							\
	Y = py_1[2*i];							\
	dst_1[6*i] = b[Y]; dst_1[6*i+1] = g[Y]; dst_1[6*i+2] = r[Y];	\
	Y = py_1[2*i+1];						\
	dst_1[6*i+3] = b[Y]; dst_1[6*i+4] = g[Y]; dst_1[6*i+5] = r[Y];

#define DST2BGR(i)							\
	Y = py_2[2*i];							\
	dst_2[6*i] = b[Y]; dst_2[6*i+1] = g[Y]; dst_2[6*i+2] = r[Y];	\
	Y = py_2[2*i+1];						\
	dst_2[6*i+3] = b[Y]; dst_2[6*i+4] = g[Y]; dst_2[6*i+5] = r[Y];

static void yuv2rgb_c_32 (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint32_t * r, * g, * b;
    uint32_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
    }
}

// This is very near from the yuv2rgb_c_32 code
static void yuv2rgb_c_24_rgb (uint8_t * py_1, uint8_t * py_2,
			      uint8_t * pu, uint8_t * pv,
			      void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	RGB(0);
	DST1RGB(0);
	DST2RGB(0);

	RGB(1);
	DST2RGB(1);
	DST1RGB(1);

	RGB(2);
	DST1RGB(2);
	DST2RGB(2);

	RGB(3);
	DST2RGB(3);
	DST1RGB(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 24;
	dst_2 += 24;
    }
}

// only trivial mods from yuv2rgb_c_24_rgb
static void yuv2rgb_c_24_bgr (uint8_t * py_1, uint8_t * py_2,
			      uint8_t * pu, uint8_t * pv,
			      void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	RGB(0);
	DST1BGR(0);
	DST2BGR(0);

	RGB(1);
	DST2BGR(1);
	DST1BGR(1);

	RGB(2);
	DST1BGR(2);
	DST2BGR(2);

	RGB(3);
	DST2BGR(3);
	DST1BGR(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 24;
	dst_2 += 24;
    }
}

// This is exactly the same code as yuv2rgb_c_32 except for the types of
// r, g, b, dst_1, dst_2
static void yuv2rgb_c_16 (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint16_t * r, * g, * b;
    uint16_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
    }
}

// This is exactly the same code as yuv2rgb_c_32 except for the types of
// r, g, b, dst_1, dst_2
static void yuv2rgb_c_8  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
    }
}

// r, g, b, dst_1, dst_2
static void yuv2rgb_c_8_ordered_dither  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	const uint8_t *d32= dither_8x8_32[v_pos&7];
	const uint8_t *d64= dither_8x8_73[v_pos&7];
#define DST1bpp8(i,o)					\
	Y = py_1[2*i];				\
	dst_1[2*i] = r[Y+d32[0+o]] + g[Y+d32[0+o]] + b[Y+d64[0+o]];	\
	Y = py_1[2*i+1];			\
	dst_1[2*i+1] = r[Y+d32[1+o]] + g[Y+d32[1+o]] + b[Y+d64[1+o]];

#define DST2bpp8(i,o)					\
	Y = py_2[2*i];				\
	dst_2[2*i] =  r[Y+d32[8+o]] + g[Y+d32[8+o]] + b[Y+d64[8+o]];	\
	Y = py_2[2*i+1];			\
	dst_2[2*i+1] =  r[Y+d32[9+o]] + g[Y+d32[9+o]] + b[Y+d64[9+o]];


	RGB(0);
	DST1bpp8(0,0);
	DST2bpp8(0,0);

	RGB(1);
	DST2bpp8(1,2);
	DST1bpp8(1,2);

	RGB(2);
	DST1bpp8(2,4);
	DST2bpp8(2,4);

	RGB(3);
	DST2bpp8(3,6);
	DST1bpp8(3,6);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
    }
}


// This is exactly the same code as yuv2rgb_c_32 except for the types of
// r, g, b, dst_1, dst_2
static void yuv2rgb_c_4  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
        int acc;
#define DST1_4(i)					\
	Y = py_1[2*i];				\
	acc = r[Y] + g[Y] + b[Y];	\
	Y = py_1[2*i+1];			\
        acc |= (r[Y] + g[Y] + b[Y])<<4;\
	dst_1[i] = acc; 

#define DST2_4(i)					\
	Y = py_2[2*i];				\
	acc = r[Y] + g[Y] + b[Y];	\
	Y = py_2[2*i+1];			\
	acc |= (r[Y] + g[Y] + b[Y])<<4;\
	dst_2[i] = acc; 
	
        RGB(0);
	DST1_4(0);
	DST2_4(0);

	RGB(1);
	DST2_4(1);
	DST1_4(1);

	RGB(2);
	DST1_4(2);
	DST2_4(2);

	RGB(3);
	DST2_4(3);
	DST1_4(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 4;
	dst_2 += 4;
    }
}

static void yuv2rgb_c_4_ordered_dither  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	const uint8_t *d64= dither_8x8_73[v_pos&7];
	const uint8_t *d128=dither_8x8_220[v_pos&7];
        int acc;

#define DST1bpp4(i,o)					\
	Y = py_1[2*i];				\
	acc = r[Y+d128[0+o]] + g[Y+d64[0+o]] + b[Y+d128[0+o]];	\
	Y = py_1[2*i+1];			\
	acc |= (r[Y+d128[1+o]] + g[Y+d64[1+o]] + b[Y+d128[1+o]])<<4;\
        dst_1[i]= acc;

#define DST2bpp4(i,o)					\
	Y = py_2[2*i];				\
	acc =  r[Y+d128[8+o]] + g[Y+d64[8+o]] + b[Y+d128[8+o]];	\
	Y = py_2[2*i+1];			\
	acc |=  (r[Y+d128[9+o]] + g[Y+d64[9+o]] + b[Y+d128[9+o]])<<4;\
        dst_2[i]= acc;


	RGB(0);
	DST1bpp4(0,0);
	DST2bpp4(0,0);

	RGB(1);
	DST2bpp4(1,2);
	DST1bpp4(1,2);

	RGB(2);
	DST1bpp4(2,4);
	DST2bpp4(2,4);

	RGB(3);
	DST2bpp4(3,6);
	DST1bpp4(3,6);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 4;
	dst_2 += 4;
    }
}

// This is exactly the same code as yuv2rgb_c_32 except for the types of
// r, g, b, dst_1, dst_2
static void yuv2rgb_c_4b  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	RGB(0);
	DST1(0);
	DST2(0);

	RGB(1);
	DST2(1);
	DST1(1);

	RGB(2);
	DST1(2);
	DST2(2);

	RGB(3);
	DST2(3);
	DST1(3);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
    }
}

static void yuv2rgb_c_4b_ordered_dither  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int U, V, Y;
    uint8_t * r, * g, * b;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;

    while (h_size--) {
	const uint8_t *d64= dither_8x8_73[v_pos&7];
	const uint8_t *d128=dither_8x8_220[v_pos&7];

#define DST1bpp4b(i,o)					\
	Y = py_1[2*i];				\
	dst_1[2*i] = r[Y+d128[0+o]] + g[Y+d64[0+o]] + b[Y+d128[0+o]];	\
	Y = py_1[2*i+1];			\
	dst_1[2*i+1] = r[Y+d128[1+o]] + g[Y+d64[1+o]] + b[Y+d128[1+o]];

#define DST2bpp4b(i,o)					\
	Y = py_2[2*i];				\
	dst_2[2*i] =  r[Y+d128[8+o]] + g[Y+d64[8+o]] + b[Y+d128[8+o]];	\
	Y = py_2[2*i+1];			\
	dst_2[2*i+1] =  r[Y+d128[9+o]] + g[Y+d64[9+o]] + b[Y+d128[9+o]];


	RGB(0);
	DST1bpp4b(0,0);
	DST2bpp4b(0,0);

	RGB(1);
	DST2bpp4b(1,2);
	DST1bpp4b(1,2);

	RGB(2);
	DST1bpp4b(2,4);
	DST2bpp4b(2,4);

	RGB(3);
	DST2bpp4b(3,6);
	DST1bpp4b(3,6);

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 += 8;
	dst_2 += 8;
    }
}

static void yuv2rgb_c_1_ordered_dither  (uint8_t * py_1, uint8_t * py_2,
			  uint8_t * pu, uint8_t * pv,
			  void * _dst_1, void * _dst_2, int h_size, int v_pos)
{
    int Y;
    uint8_t * g;
    uint8_t * dst_1, * dst_2;

    h_size >>= 3;
    dst_1 = _dst_1;
    dst_2 = _dst_2;
    g= table_gU[128] + table_gV[128];

    while (h_size--) {
	const uint8_t *d128=dither_8x8_220[v_pos&7];
	char out_1=0, out_2=0;

#define DST1bpp1(i,o)					\
	Y = py_1[2*i];				\
	out_1+= out_1 + g[Y+d128[0+o]];	\
	Y = py_1[2*i+1];			\
	out_1+= out_1 + g[Y+d128[1+o]];

#define DST2bpp1(i,o)					\
	Y = py_2[2*i];				\
	out_2+= out_2 + g[Y+d128[8+o]];	\
	Y = py_2[2*i+1];			\
	out_2+= out_2 + g[Y+d128[9+o]];

	DST1bpp1(0,0);
	DST2bpp1(0,0);

	DST2bpp1(1,2);
	DST1bpp1(1,2);

	DST1bpp1(2,4);
	DST2bpp1(2,4);

	DST2bpp1(3,6);
	DST1bpp1(3,6);
	
	dst_1[0]= out_1;
	dst_2[0]= out_2;

	pu += 4;
	pv += 4;
	py_1 += 8;
	py_2 += 8;
	dst_1 ++;
	dst_2 ++;
    }
}


static int div_round (int dividend, int divisor)
{
    if (dividend > 0)
	return (dividend + (divisor>>1)) / divisor;
    else
	return -((-dividend + (divisor>>1)) / divisor);
}

void *yuv2rgb_c_init (unsigned bpp, int mode, void *table_rV[256], void *table_gU[256], int table_gV[256], void *table_bU[256])
{  
    int i;
    uint8_t table_Y[1024];
    uint32_t *table_32 = 0;
    uint16_t *table_16 = 0;
    uint8_t *table_8 = 0;
    uint8_t *table_332 = 0;
    uint8_t *table_121 = 0;
    uint8_t *table_1 = 0;
    int entry_size = 0;
    void *table_r = 0, *table_g = 0, *table_b = 0;
    void *table_start;

    int crv = Inverse_Table_6_9[matrix_coefficients][0];
    int cbu = Inverse_Table_6_9[matrix_coefficients][1];
    int cgu = -Inverse_Table_6_9[matrix_coefficients][2];
    int cgv = -Inverse_Table_6_9[matrix_coefficients][3];

    for (i = 0; i < 1024; i++) {
	int j;

	j = (76309 * (i - 384 - 16) + 32768) >> 16;
	j = (j < 0) ? 0 : ((j > 255) ? 255 : j);
	table_Y[i] = j;
    }

    switch (bpp) {
    case 32:
	yuv2rgb_c_internal = yuv2rgb_c_32;

	table_start= table_32 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint32_t));

	entry_size = sizeof (uint32_t);
	table_r = table_32 + 197;
	table_b = table_32 + 197 + 685;
	table_g = table_32 + 197 + 2*682;

	for (i = -197; i < 256+197; i++)
	    ((uint32_t *)table_r)[i] = table_Y[i+384] << ((mode==MODE_RGB) ? 16 : 0);
	for (i = -132; i < 256+132; i++)
	    ((uint32_t *)table_g)[i] = table_Y[i+384] << 8;
	for (i = -232; i < 256+232; i++)
	    ((uint32_t *)table_b)[i] = table_Y[i+384] << ((mode==MODE_RGB) ? 0 : 16);
	break;

    case 24:
//	yuv2rgb_c_internal = (mode==MODE_RGB) ? yuv2rgb_c_24_rgb : yuv2rgb_c_24_bgr;
	yuv2rgb_c_internal = (mode!=MODE_RGB) ? yuv2rgb_c_24_rgb : yuv2rgb_c_24_bgr;

	table_start= table_8 = malloc ((256 + 2*232) * sizeof (uint8_t));

	entry_size = sizeof (uint8_t);
	table_r = table_g = table_b = table_8 + 232;

	for (i = -232; i < 256+232; i++)
	    ((uint8_t * )table_b)[i] = table_Y[i+384];
	break;

    case 15:
    case 16:
	yuv2rgb_c_internal = yuv2rgb_c_16;

	table_start= table_16 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint16_t));

	entry_size = sizeof (uint16_t);
	table_r = table_16 + 197;
	table_b = table_16 + 197 + 685;
	table_g = table_16 + 197 + 2*682;

	for (i = -197; i < 256+197; i++) {
	    int j = table_Y[i+384] >> 3;

	    if (mode == MODE_RGB)
		j <<= ((bpp==16) ? 11 : 10);

	    ((uint16_t *)table_r)[i] = j;
	}
	for (i = -132; i < 256+132; i++) {
	    int j = table_Y[i+384] >> ((bpp==16) ? 2 : 3);

	    ((uint16_t *)table_g)[i] = j << 5;
	}
	for (i = -232; i < 256+232; i++) {
	    int j = table_Y[i+384] >> 3;

	    if (mode == MODE_BGR)
		j <<= ((bpp==16) ? 11 : 10);

	    ((uint16_t *)table_b)[i] = j;
	}
	break;

    case 8:
	yuv2rgb_c_internal = yuv2rgb_c_8_ordered_dither; //yuv2rgb_c_8;

	table_start= table_332 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint8_t));

	entry_size = sizeof (uint8_t);
	table_r = table_332 + 197;
	table_b = table_332 + 197 + 685;
	table_g = table_332 + 197 + 2*682;

	for (i = -197; i < 256+197; i++) {
	    int j = (table_Y[i+384 - 16] + 18)/36;

	    if (mode == MODE_RGB)
		j <<= 5;

	    ((uint8_t *)table_r)[i] = j;
	}
	for (i = -132; i < 256+132; i++) {
	    int j = (table_Y[i+384 - 16] + 18)/36;

	    if (mode == MODE_BGR)
		j <<= 1;

	    ((uint8_t *)table_g)[i] = j << 2;
	}
	for (i = -232; i < 256+232; i++) {
	    int j = (table_Y[i+384 - 37] + 43)/85;

	    if (mode == MODE_BGR)
		j <<= 6;

	    ((uint8_t *)table_b)[i] = j;
	}
	break;
    case 4:
    case 4|128:
        if(bpp==4)
	    yuv2rgb_c_internal = yuv2rgb_c_4_ordered_dither; //yuv2rgb_c_4;
        else
	    yuv2rgb_c_internal = yuv2rgb_c_4b_ordered_dither; //yuv2rgb_c_4;

	table_start= table_121 = malloc ((197 + 2*682 + 256 + 132) * sizeof (uint8_t));

	entry_size = sizeof (uint8_t);
	table_r = table_121 + 197;
	table_b = table_121 + 197 + 685;
	table_g = table_121 + 197 + 2*682;

	for (i = -197; i < 256+197; i++) {
	    int j = table_Y[i+384 - 110] >> 7;

	    if (mode == MODE_RGB)
		j <<= 3;

	    ((uint8_t *)table_r)[i] = j;
	}
	for (i = -132; i < 256+132; i++) {
	    int j = (table_Y[i+384 - 37]+ 43)/85;

	    ((uint8_t *)table_g)[i] = j << 1;
	}
	for (i = -232; i < 256+232; i++) {
	    int j =table_Y[i+384 - 110] >> 7;

	    if (mode == MODE_BGR)
		j <<= 3;

	    ((uint8_t *)table_b)[i] = j;
	}
	break;

    case 1:
	yuv2rgb_c_internal = yuv2rgb_c_1_ordered_dither;

	table_start= table_1 = malloc (256*2 * sizeof (uint8_t));

	entry_size = sizeof (uint8_t);
	table_g = table_1;
	table_r = table_b = NULL;

	for (i = 0; i < 256+256; i++) {
	    int j = table_Y[i + 384 - 110]>>7;

	    ((uint8_t *)table_g)[i] = j;
	}
	break;

    default:
	table_start= NULL;
	mp_msg(MSGT_SWS,MSGL_ERR,"%ibpp not supported by yuv2rgb\n", bpp);
	//exit (1);
    }

    for (i = 0; i < 256; i++) {
	table_rV[i] = table_r + entry_size * div_round (crv * (i-128), 76309);
	table_gU[i] = table_g + entry_size * div_round (cgu * (i-128), 76309);
	table_gV[i] = entry_size * div_round (cgv * (i-128), 76309);
	table_bU[i] = table_b + entry_size * div_round (cbu * (i-128), 76309);
    }

    return table_start; 
}

