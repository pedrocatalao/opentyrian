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
#include "surface.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

Surface *surface_create(int w, int h)
{
	assert(w > 0 && h > 0);

	Surface *surface = malloc(sizeof *surface);
	if (surface == NULL)
		return NULL;

	surface->w = w;
	surface->h = h;
	surface->pitch = w;
	surface->pixels = calloc((size_t)w * h, 1);
	if (surface->pixels == NULL)
	{
		free(surface);
		return NULL;
	}

	return surface;
}

void surface_free(Surface *surface)
{
	if (surface == NULL)
		return;

	free(surface->pixels);
	free(surface);
}

void surface_fill(Surface *surface, Uint8 color)
{
	memset(surface->pixels, color, (size_t)surface->pitch * surface->h);
}

void surface_fill_rect(Surface *surface, const Rect *rect, Uint8 color)
{
	if (rect == NULL)
	{
		surface_fill(surface, color);
		return;
	}

	int x1 = MAX(rect->x, 0);
	int y1 = MAX(rect->y, 0);
	int x2 = MIN(rect->x + rect->w, surface->w);
	int y2 = MIN(rect->y + rect->h, surface->h);

	for (int y = y1; y < y2; ++y)
		memset(surface->pixels + (size_t)y * surface->pitch + x1, color, (size_t)(x2 - x1));
}

void surface_copy(Surface *dst, const Surface *src)
{
	assert(dst->w == src->w && dst->h == src->h);

	if (dst->pitch == src->pitch)
	{
		memcpy(dst->pixels, src->pixels, (size_t)src->pitch * src->h);
		return;
	}

	for (int y = 0; y < src->h; ++y)
		memcpy(dst->pixels + (size_t)y * dst->pitch, src->pixels + (size_t)y * src->pitch, (size_t)src->w);
}
