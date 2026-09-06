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
// video_stub.c — display setup for a host that owns the display: there is
// no window, no fullscreen and exactly one "scaler".
#include "video.h"

uint scaler = 0;
const uint scalers_count = 1;

const char *scaler_name(uint i)
{
	(void)i;
	return "None";
}

void set_scaler_by_name(const char *name)
{
	(void)name;
}

void reinit_fullscreen(int new_display)
{
	(void)new_display;
	fullscreen_display = -1;
}

void toggle_fullscreen(void) { }

bool init_scaler(unsigned int new_scaler)
{
	(void)new_scaler;
	return true;
}
