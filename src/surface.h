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
#ifndef SURFACE_H
#define SURFACE_H

#include "opentyr.h"

// An 8-bit paletted software surface.  The game draws into these directly
// through pixels/pitch; the platform layer turns the one it is shown into
// whatever the display wants.
typedef struct Surface
{
	int w, h;
	int pitch;      // bytes per row; equals w
	Uint8 *pixels;  // h rows of pitch bytes
} Surface;

typedef struct Color
{
	Uint8 r, g, b;
} Color;

typedef struct Rect
{
	int x, y, w, h;
} Rect;

Surface *surface_create(int w, int h);
void surface_free(Surface *surface);

void surface_fill(Surface *surface, Uint8 color);
void surface_fill_rect(Surface *surface, const Rect *rect, Uint8 color);  // clipped
void surface_copy(Surface *dst, const Surface *src);  // same dimensions

#endif /* SURFACE_H */
