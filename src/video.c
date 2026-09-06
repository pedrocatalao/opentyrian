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
#include "video.h"

#include "logging.h"
#include "palette.h"

#include <stdlib.h>
#include <string.h>

const char *const scaling_mode_names[ScalingMode_MAX] = {
	"Center",
	"Integer",
	"Fit 8:5",
	"Fit 4:3",
};

int fullscreen_display;
ScalingMode scaling_mode = SCALE_INTEGER;

Surface *VGAScreen, *VGAScreenSeg;
Surface *VGAScreen2;
Surface *game_screen;

void init_video(void)
{
	// The software surfaces that the game renders to. These are all 320x200x8
	// regardless of what the platform ends up showing.
	VGAScreen = VGAScreenSeg = surface_create(vga_width, vga_height);
	VGAScreen2 = surface_create(vga_width, vga_height);
	game_screen = surface_create(vga_width, vga_height);

	if (VGAScreen == NULL || VGAScreen2 == NULL || game_screen == NULL)
	{
		logFatal("Failed to allocate screen surfaces.");
		plat_exit(EXIT_FAILURE);
	}

	JE_clr256(VGAScreen);

	plat_video_init();
}

void deinit_video(void)
{
	plat_video_quit();

	surface_free(VGAScreenSeg);
	surface_free(VGAScreen2);
	surface_free(game_screen);
	VGAScreen = VGAScreenSeg = VGAScreen2 = game_screen = NULL;
}

bool set_scaling_mode_by_name(const char *name)
{
	for (int i = 0; i < ScalingMode_MAX; ++i)
	{
		 if (strcmp(name, scaling_mode_names[i]) == 0)
		 {
			 scaling_mode = i;
			 return true;
		 }
	}
	return false;
}

void JE_clr256(Surface *screen)
{
	surface_fill(screen, 0);
}

void JE_showVGA(void)
{
	plat_present(VGAScreen, palette);
}
