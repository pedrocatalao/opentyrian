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
#ifndef VIDEO_H
#define VIDEO_H

#include "platform.h"

#include <stdbool.h>

#define vga_width 320
#define vga_height 200

typedef enum {
	SCALE_CENTER,
	SCALE_INTEGER,
	SCALE_ASPECT_8_5,
	SCALE_ASPECT_4_3,
	ScalingMode_MAX
} ScalingMode;

extern const char *const scaling_mode_names[ScalingMode_MAX];

extern int fullscreen_display; // -1 means windowed
extern ScalingMode scaling_mode;

extern Surface *VGAScreen, *VGAScreenSeg;
extern Surface *game_screen;
extern Surface *VGAScreen2;

// Shared (video.c): the 320x200 surfaces the game draws into.
void init_video(void);
void deinit_video(void);

void JE_clr256(Surface *);
void JE_showVGA(void);

// Display setup, implemented by the platform: the SDL window (video_sdl.c)
// or no-ops for a host that owns the display (dxm/video_stub.c).
// The software scalers the platform offers, by index; the DXM core has one.
extern uint scaler;
extern const uint scalers_count;
const char *scaler_name(uint i);
void set_scaler_by_name(const char *name);

void reinit_fullscreen(int new_display);
void toggle_fullscreen(void);
bool init_scaler(unsigned int new_scaler);
bool set_scaling_mode_by_name(const char *name);

#endif /* VIDEO_H */
