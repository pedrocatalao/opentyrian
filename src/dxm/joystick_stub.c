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
// joystick_stub.c — the host has no joystick; keep the game's API satisfied.
#include "joystick.h"

#include <string.h>

int joystick_repeat_delay = 300;
bool joydown = false;
bool ignore_joystick = true;
int joysticks = 0;
Joystick *joystick = NULL;

int joystick_axis_reduce(int j, int value) { (void)j; return value; }
bool joystick_analog_angle(int j, float *angle) { (void)j; (void)angle; return false; }

void poll_joystick(int j) { (void)j; }
void poll_joysticks(void) { }

void push_key(Scancode key) { input_key_down(key, MOD_NONE, false); input_key_up(key); }
void push_joysticks_as_keyboard(void) { }

void init_joysticks(void) { }
void deinit_joysticks(void) { }

void reset_joystick_assignments(int j) { (void)j; }
bool load_joystick_assignments(Config *config, int j) { (void)config; (void)j; return false; }
bool save_joystick_assignments(Config *config, int j) { (void)config; (void)j; return false; }

void joystick_assignments_to_string(char *buffer, size_t buffer_len, const Joystick_assignment *assignments)
{
	(void)assignments;
	if (buffer_len > 0)
		buffer[0] = '\0';
}

bool detect_joystick_assignment(int j, Joystick_assignment *assignment) { (void)j; (void)assignment; return false; }

bool joystick_assignment_cmp(const Joystick_assignment *a, const Joystick_assignment *b)
{
	return memcmp(a, b, sizeof *a) == 0;
}
