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
// platform_dxm.c — platform.h implemented on the DOS ex Machina host
// (dxm_host).  Links no SDL, opens no device, owns no thread: everything
// comes from the shell through the host table bound in dxm_core_main().
#include "dxm_core.h"

#include "platform.h"

#include "loudness.h"
#include "video.h"

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static const dxm_host *H;
static double tickOrigin;
static jmp_buf exitTarget;
static bool exitArmed;

static const dxm_mode MODE_13H = { 320, 200, DXM_FB_INDEX8, 5, 6, 400 };

void dxm_adapter_bind(const dxm_host *h)
{
	H = h;
	tickOrigin = h->now();
}

jmp_buf *dxm_adapter_exit_target(void)
{
	exitArmed = true;
	return &exitTarget;
}

// ---- lifecycle ------------------------------------------------------------

void plat_exit(int code)
{
	(void)code;
	if (exitArmed)
		longjmp(exitTarget, 1);
	// Not armed: nothing to unwind to.  Spin until the shell gives up on us
	// rather than take the host process down.
	for (;;)
		H->sleep_ms(100);
}

// ---- timing ---------------------------------------------------------------

Uint32 plat_ticks(void)
{
	return (Uint32)((H->now() - tickOrigin) * 1000.0);
}

void plat_delay(Uint32 ms)
{
	H->sleep_ms((int)ms);
}

// ---- video ----------------------------------------------------------------

void plat_video_init(void) { }
void plat_video_quit(void) { }

int plat_display_count(void)
{
	return 1;
}

void plat_present(const Surface *screen, const Color palette[256])
{
	static uint8_t pal[256 * 3];
	for (int i = 0; i < 256; ++i)
	{
		pal[i * 3 + 0] = palette[i].r;
		pal[i * 3 + 1] = palette[i].g;
		pal[i * 3 + 2] = palette[i].b;
	}

	dxm_frame frame = { &MODE_13H, screen->pixels, pal };
	H->present(&frame);
}

// ---- input ----------------------------------------------------------------

// XT set 1 make codes, which is what the host reports, to the game's
// scancodes.  Keypad codes that double as arrows/navigation on an 84-key
// board map to the navigation keys, since those are what the game binds by
// default.
static const Scancode xtToScancode[0x60] =
{
	[0x01] = SCANCODE_ESCAPE,
	[0x02] = SCANCODE_1, [0x03] = SCANCODE_2, [0x04] = SCANCODE_3,
	[0x05] = SCANCODE_4, [0x06] = SCANCODE_5, [0x07] = SCANCODE_6,
	[0x08] = SCANCODE_7, [0x09] = SCANCODE_8, [0x0A] = SCANCODE_9,
	[0x0B] = SCANCODE_0,
	[0x0C] = SCANCODE_MINUS, [0x0D] = SCANCODE_EQUALS,
	[0x0E] = SCANCODE_BACKSPACE, [0x0F] = SCANCODE_TAB,
	[0x10] = SCANCODE_Q, [0x11] = SCANCODE_W, [0x12] = SCANCODE_E,
	[0x13] = SCANCODE_R, [0x14] = SCANCODE_T, [0x15] = SCANCODE_Y,
	[0x16] = SCANCODE_U, [0x17] = SCANCODE_I, [0x18] = SCANCODE_O,
	[0x19] = SCANCODE_P,
	[0x1A] = SCANCODE_LEFTBRACKET, [0x1B] = SCANCODE_RIGHTBRACKET,
	[0x1C] = SCANCODE_RETURN, [0x1D] = SCANCODE_LCTRL,
	[0x1E] = SCANCODE_A, [0x1F] = SCANCODE_S, [0x20] = SCANCODE_D,
	[0x21] = SCANCODE_F, [0x22] = SCANCODE_G, [0x23] = SCANCODE_H,
	[0x24] = SCANCODE_J, [0x25] = SCANCODE_K, [0x26] = SCANCODE_L,
	[0x27] = SCANCODE_SEMICOLON, [0x28] = SCANCODE_APOSTROPHE,
	[0x29] = SCANCODE_GRAVE, [0x2A] = SCANCODE_LSHIFT,
	[0x2B] = SCANCODE_BACKSLASH,
	[0x2C] = SCANCODE_Z, [0x2D] = SCANCODE_X, [0x2E] = SCANCODE_C,
	[0x2F] = SCANCODE_V, [0x30] = SCANCODE_B, [0x31] = SCANCODE_N,
	[0x32] = SCANCODE_M,
	[0x33] = SCANCODE_COMMA, [0x34] = SCANCODE_PERIOD, [0x35] = SCANCODE_SLASH,
	[0x36] = SCANCODE_RSHIFT, [0x37] = SCANCODE_KP_MULTIPLY,
	[0x38] = SCANCODE_LALT, [0x39] = SCANCODE_SPACE, [0x3A] = SCANCODE_CAPSLOCK,
	[0x3B] = SCANCODE_F1, [0x3C] = SCANCODE_F2, [0x3D] = SCANCODE_F3,
	[0x3E] = SCANCODE_F4, [0x3F] = SCANCODE_F5, [0x40] = SCANCODE_F6,
	[0x41] = SCANCODE_F7, [0x42] = SCANCODE_F8, [0x43] = SCANCODE_F9,
	[0x44] = SCANCODE_F10,
	[0x45] = SCANCODE_NUMLOCKCLEAR, [0x46] = SCANCODE_SCROLLLOCK,
	[0x47] = SCANCODE_HOME, [0x48] = SCANCODE_UP, [0x49] = SCANCODE_PAGEUP,
	[0x4A] = SCANCODE_KP_MINUS,
	[0x4B] = SCANCODE_LEFT, [0x4C] = SCANCODE_KP_5, [0x4D] = SCANCODE_RIGHT,
	[0x4E] = SCANCODE_KP_PLUS,
	[0x4F] = SCANCODE_END, [0x50] = SCANCODE_DOWN, [0x51] = SCANCODE_PAGEDOWN,
	[0x52] = SCANCODE_INSERT, [0x53] = SCANCODE_DELETE,
	[0x57] = SCANCODE_F11, [0x58] = SCANCODE_F12,
};

static bool xtDown[0x60];
static bool textInputEnabled;

static Uint16 currentMods(void)
{
	Uint16 mod = MOD_NONE;
	if (xtDown[0x2A]) mod |= MOD_LSHIFT;
	if (xtDown[0x36]) mod |= MOD_RSHIFT;
	if (xtDown[0x1D]) mod |= MOD_LCTRL;
	if (xtDown[0x38]) mod |= MOD_LALT;
	return mod;
}

void plat_pump(void)
{
	if (H->should_quit())
		input_quit();

	// Drain the BIOS-style queue; key state below is what the game reads.
	while (H->getch() != 0)
		;

	for (int xt = 1; xt < (int)COUNTOF(xtToScancode); ++xt)
	{
		bool down = H->key_down(xt) != 0;
		if (down == xtDown[xt])
			continue;
		xtDown[xt] = down;

		Scancode scancode = xtToScancode[xt];
		if (scancode == SCANCODE_UNKNOWN)
			continue;

		if (down)
		{
			Uint16 mod = currentMods();
			input_key_down(scancode, mod, false);

			if (textInputEnabled)
			{
				char ch = scancode_to_char(scancode, (mod & MOD_SHIFT) != 0);
				if (ch >= 32 && ch < 127)
					input_text((Uint8)ch);
			}
		}
		else
		{
			input_key_up(scancode);
		}
	}
}

void plat_text_input(bool enable)
{
	textInputEnabled = enable;
}

void plat_mouse_show(bool show) { (void)show; }
void plat_mouse_relative(bool enable) { (void)enable; }

void plat_mouse_relative_motion(Sint32 *out_x, Sint32 *out_y)
{
	*out_x = 0;
	*out_y = 0;
}

// ---- audio ----------------------------------------------------------------

#define DXM_AUDIO_RATE 44100

static bool audioOpen;

bool plat_audio_open(int *rate)
{
	*rate = DXM_AUDIO_RATE;
	audioOpen = true;
	return true;
}

void plat_audio_close(void)
{
	audioOpen = false;
}

void plat_audio_lock(void)
{
	if (H != NULL && H->lock != NULL)
		H->lock();
}

void plat_audio_unlock(void)
{
	if (H != NULL && H->unlock != NULL)
		H->unlock();
}

// Called by the shell from its audio callback: stereo s16 at 44100 Hz.
void dxm_adapter_audio(int16_t *out, int frames)
{
	static Sint16 mono[4096];

	while (frames > 0)
	{
		int n = frames < (int)COUNTOF(mono) ? frames : (int)COUNTOF(mono);

		if (audioOpen && !audio_disabled)
			audio_render(mono, n);
		else
			memset(mono, 0, (size_t)n * sizeof mono[0]);

		for (int i = 0; i < n; ++i)
		{
			out[i * 2] = mono[i];
			out[i * 2 + 1] = mono[i];
		}

		out += n * 2;
		frames -= n;
	}
}

// ---- logging --------------------------------------------------------------

void plat_log(LogLevel level, const char *message)
{
	static const char *const prefixes[] = { "", "", "warning: ", "error: ", "fatal: " };
	char line[4200];
	snprintf(line, sizeof line, "%s%s", prefixes[level], message);
	H->log(line);
}

// ---- paths ----------------------------------------------------------------

const char *plat_base_path(void)
{
	// The data directory is given to the game explicitly (see dxm_entry.c);
	// there is no executable to be relative to.
	return NULL;
}

const char *plat_pref_path(void)
{
	return H->pref_dir;
}
