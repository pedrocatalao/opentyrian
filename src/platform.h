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
#ifndef PLATFORM_H
#define PLATFORM_H

// The seam between the game and whatever hosts it.  Game code includes this
// and nothing platform-specific.  Two implementations exist: the standalone
// SDL2 build (platform_sdl.c, video_sdl.c) and the DOS ex Machina core
// (dxm/platform_dxm.c), which links no SDL at all.

#include "opentyr.h"
#include "scancode.h"
#include "surface.h"

#include <stdbool.h>

// ---- lifecycle ------------------------------------------------------------

// Leave the game.  Standalone: exit().  Core: unwind to the host, which
// survives.  Game code MUST call this rather than exit().
void plat_exit(int code);

// ---- timing ---------------------------------------------------------------

Uint32 plat_ticks(void);      // milliseconds, monotonic
void plat_delay(Uint32 ms);

// ---- video ----------------------------------------------------------------

void plat_video_init(void);  // called by init_video() after the surfaces exist
void plat_video_quit(void);

// Show the 320x200 screen through the palette.
void plat_present(const Surface *screen, const Color palette[256]);
int plat_display_count(void);

// ---- input ----------------------------------------------------------------

// Pump host events.  Delivers them through the input_* callbacks below,
// which keyboard.c implements.
void plat_pump(void);

void plat_text_input(bool enable);
void plat_mouse_show(bool show);
void plat_mouse_relative(bool enable);
// Relative motion accumulated since the last call, in screen units; resets.
void plat_mouse_relative_motion(Sint32 *out_x, Sint32 *out_y);

void input_key_down(Scancode scancode, Uint16 mod, bool repeat);
void input_key_up(Scancode scancode);
void input_text(Uint8 ch);  // CP437
void input_mouse_motion(Sint32 x, Sint32 y, bool moved);  // screen coordinates
void input_mouse_button(Uint8 button, bool down, Sint32 x, Sint32 y);
void input_focus(bool focused);
void input_quit(void);

// ---- audio ----------------------------------------------------------------

// Open the output device.  It pulls mono signed 16-bit samples from
// audio_render() (loudness.c).  Returns false if unavailable; the actual
// sample rate comes back in *rate.
bool plat_audio_open(int *rate);
void plat_audio_close(void);
void plat_audio_lock(void);
void plat_audio_unlock(void);

// ---- logging --------------------------------------------------------------

typedef enum LogLevel
{
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL,
} LogLevel;

void plat_log(LogLevel level, const char *message);

// ---- paths ----------------------------------------------------------------

// Directory of the executable (Contents/Resources in a macOS bundle), with a
// trailing separator, or NULL.
const char *plat_base_path(void);

// Writable per-user directory for configuration and saves, with a trailing
// separator, or NULL to use the current directory.
const char *plat_pref_path(void);

#endif /* PLATFORM_H */
