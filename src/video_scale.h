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
#ifndef VIDEO_SCALE_H
#define VIDEO_SCALE_H

#include "SDL.h"

#include "opentyr.h"
#include "surface.h"
#include "video.h"

typedef void (*ScalerFunction)(Surface *src, SDL_Texture *dst);

// Current palette for the scalers, maintained by video_sdl.c.
extern Uint32 rgb_palette[256], yuv_palette[256];

struct Scalers
{
	int width, height;
	ScalerFunction scaler16, scaler32;
	const char *name;
};

extern const struct Scalers scalers[];

#endif /* VIDEO_SCALE_H */
