/*
 * OpenTyrian: A modern cross-platform port of Tyrian
 * Copyright (C) The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef OPENTYR_H
#define OPENTYR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// SDL's names for the fixed-width types are used throughout.  SDL defines
// them identically; when a platform file includes SDL.h it must do so before
// this header so C99 does not see a second typedef.
#ifndef SDL_stdinc_h_
typedef int8_t Sint8;
typedef uint8_t Uint8;
typedef int16_t Sint16;
typedef uint16_t Uint16;
typedef int32_t Sint32;
typedef uint32_t Uint32;
typedef int64_t Sint64;
typedef uint64_t Uint64;
#endif

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define OT_BIG_ENDIAN 1
#else
#define OT_BIG_ENDIAN 0
#endif

static inline Uint16 swap16(Uint16 x) { return (Uint16)((x << 8) | (x >> 8)); }
static inline Uint32 swap32(Uint32 x) { return (x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24); }
#if OT_BIG_ENDIAN
#define swapLE16(x) swap16(x)
#define swapLE32(x) swap32(x)
#define swapBE16(x) (x)
#else
#define swapLE16(x) (x)
#define swapLE32(x) (x)
#define swapBE16(x) swap16(x)
#endif

#ifndef COUNTOF
#define COUNTOF(x) (sizeof(x) / sizeof *(x))  // use only on arrays!
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifndef M_PI
#define M_PI    3.14159265358979323846  // pi
#endif
#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923  // pi/2
#endif
#ifndef M_PI_4
#define M_PI_4  0.78539816339744830962  // pi/4
#endif


// BSD strlcpy: copies up to size-1 bytes, always terminates, returns strlen(src).
static inline size_t ot_strlcpy(char *dst, const char *src, size_t size)
{
	size_t len = 0;
	while (src[len] != '\0')
		++len;
	if (size > 0)
	{
		size_t n = len < size - 1 ? len : size - 1;
		for (size_t i = 0; i < n; ++i)
			dst[i] = src[i];
		dst[n] = '\0';
	}
	return len;
}

typedef unsigned int uint;
typedef unsigned long ulong;

// Pascal types, yuck.
typedef Sint32 JE_longint;
typedef Sint16 JE_integer;
typedef Sint8  JE_shortint;
typedef Uint16 JE_word;
typedef Uint8  JE_byte;
typedef bool   JE_boolean;
typedef char   JE_char;
typedef float  JE_real;

extern const char *opentyrian_str;
extern const char *opentyrian_version;

void setupMenu(void);

// The whole game, from the intro to the exit.  main() (standalone) or the
// core entry point (DXM) call this.
int opentyrian_main(int argc, char *argv[]);

#endif /* OPENTYR_H */
