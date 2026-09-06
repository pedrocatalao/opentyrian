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
// platform_sdl.c — the standalone SDL2 implementation of platform.h, except
// for the window and renderer, which live in video_sdl.c.
#include "SDL.h"

#include "platform.h"

#include "loudness.h"
#include "video_sdl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---- lifecycle ------------------------------------------------------------

void plat_exit(int code)
{
	exit(code);
}

// ---- timing ---------------------------------------------------------------

Uint32 plat_ticks(void)
{
	return SDL_GetTicks();
}

void plat_delay(Uint32 ms)
{
	SDL_Delay(ms);
}

// ---- input ----------------------------------------------------------------

static bool mouseRelative;
static Sint32 mouseWindowXRelative;
static Sint32 mouseWindowYRelative;

// Mapping from CP437 to UCS for 0x80 to 0xA8.
static const Uint16 ucsMap[] =
{
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
	0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
	0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
	0x00BF,
};

static void handleTextInput(const char *text, size_t textSize)
{
	for (size_t i = 0; i < textSize; ++i)
	{
		// Decode codepoint from UTF-8.
		Uint16 cp = (unsigned char)text[i];
		if (cp == 0)
		{
			break;
		}
		else if (cp < 0x80)
		{
			// ASCII.
		}
		else if (cp < 0xC0)
		{
			// Invalid.
			continue;
		}
		else if (cp < 0xE0)
		{
			if (i + 1 >= textSize)
				continue;

			cp &= 0x1F;
			cp = (cp << 6) | (text[++i] & 0x3F);
		}
		else if (cp < 0xF0)
		{
			if (i + 2 >= textSize)
				continue;

			cp &= 0x0F;
			cp = (cp << 6) | (text[++i] & 0x3F);
			cp = (cp << 6) | (text[++i] & 0x3F);
		}
		else
		{
			// Outside the BMP.
			continue;
		}

		// Map codepoint to CP437 character.
		Uint8 ch = 0;
		if (cp < 0x80)
		{
			ch = cp;
		}
		else
		{
			for (size_t j = 0; j < COUNTOF(ucsMap); ++j)
			{
				if (cp == ucsMap[j])
				{
					ch = 0x80 + j;
					break;
				}
			}

			if (ch == 0)
				continue;
		}

		input_text(ch);
	}
}

void plat_pump(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_WINDOWEVENT:
				switch (ev.window.event)
				{
				case SDL_WINDOWEVENT_FOCUS_LOST:
					input_focus(false);
					break;

				case SDL_WINDOWEVENT_FOCUS_GAINED:
					input_focus(true);
					break;

				case SDL_WINDOWEVENT_RESIZED:
					video_on_win_resize();
					break;
				}
				break;

			case SDL_KEYDOWN:
				input_key_down((Scancode)ev.key.keysym.scancode, ev.key.keysym.mod, ev.key.repeat != 0);
				break;

			case SDL_KEYUP:
				input_key_up((Scancode)ev.key.keysym.scancode);
				break;

			case SDL_TEXTINPUT:
				handleTextInput(ev.text.text, COUNTOF(ev.text.text));
				break;

			case SDL_MOUSEMOTION:
			{
				Sint32 x = ev.motion.x, y = ev.motion.y;
				mapWindowPointToScreen(&x, &y);

				if (mouseRelative)
				{
					mouseWindowXRelative += ev.motion.xrel;
					mouseWindowYRelative += ev.motion.yrel;
				}

				input_mouse_motion(x, y, ev.motion.xrel != 0 || ev.motion.yrel != 0);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				Sint32 x = ev.button.x, y = ev.button.y;
				mapWindowPointToScreen(&x, &y);
				input_mouse_button(ev.button.button, ev.type == SDL_MOUSEBUTTONDOWN, x, y);
				break;
			}

			case SDL_QUIT:
				input_quit();
				break;
		}
	}
}

void plat_text_input(bool enable)
{
	if (enable)
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}

void plat_mouse_show(bool show)
{
	SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void plat_mouse_relative(bool enable)
{
	SDL_SetRelativeMouseMode(enable);

	mouseRelative = enable;
}

void plat_mouse_relative_motion(Sint32 *out_x, Sint32 *out_y)
{
	scaleWindowDistanceToScreen(&mouseWindowXRelative, &mouseWindowYRelative);
	*out_x = mouseWindowXRelative;
	*out_y = mouseWindowYRelative;

	mouseWindowXRelative = 0;
	mouseWindowYRelative = 0;
}

// ---- audio ----------------------------------------------------------------

static SDL_AudioDeviceID audioDevice = 0;

static void audioCallback(void *userdata, Uint8 *stream, int size)
{
	(void)userdata;

	audio_render((Sint16 *)stream, size / (int)sizeof (Sint16));
}

bool plat_audio_open(int *rate)
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL audio: %s", SDL_GetError());
		return false;
	}

	SDL_AudioSpec ask, got;
	memset(&ask, 0, sizeof ask);
	ask.freq = *rate;
	ask.format = AUDIO_S16SYS;
	ask.channels = 1;
	ask.samples = 256 * (*rate / 11025); // ~23 ms
	ask.callback = audioCallback;

	int allowedChanges = SDL_AUDIO_ALLOW_FREQUENCY_CHANGE;
#if SDL_VERSION_ATLEAST(2, 0, 9)
	allowedChanges |= SDL_AUDIO_ALLOW_SAMPLES_CHANGE;
#endif
	audioDevice = SDL_OpenAudioDevice(/*device*/ NULL, /*iscapture*/ 0, &ask, &got, allowedChanges);

	if (audioDevice == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open audio device: %s", SDL_GetError());
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return false;
	}

	*rate = got.freq;

	SDL_PauseAudioDevice(audioDevice, 0); // unpause

	return true;
}

void plat_audio_close(void)
{
	if (audioDevice != 0)
	{
		SDL_PauseAudioDevice(audioDevice, 1); // pause
		SDL_CloseAudioDevice(audioDevice);
		audioDevice = 0;
	}

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void plat_audio_lock(void)
{
	if (audioDevice != 0)
		SDL_LockAudioDevice(audioDevice);
}

void plat_audio_unlock(void)
{
	if (audioDevice != 0)
		SDL_UnlockAudioDevice(audioDevice);
}

// ---- logging --------------------------------------------------------------

void plat_log(LogLevel level, const char *message)
{
	switch (level)
	{
	case LOG_DEBUG:
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
		break;
	case LOG_INFO:
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
		break;
	case LOG_WARN:
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
		break;
	case LOG_ERROR:
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
		break;
	case LOG_FATAL:
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, NULL);
		break;
	}
}

// ---- paths ----------------------------------------------------------------

const char *plat_base_path(void)
{
	static char *basePath = NULL;
	static bool tried = false;

	if (!tried)
	{
		tried = true;
		basePath = SDL_GetBasePath();  // never freed; lives as long as the process
	}

	return basePath;
}

const char *plat_pref_path(void)
{
	static char *prefPath = NULL;
	static bool tried = false;

	if (tried)
		return prefPath;
	tried = true;

#ifdef TARGET_WIN32
	const char *appData = getenv("APPDATA");
	if (appData != NULL)
	{
		size_t size = strlen(appData) + strlen("/OpenTyrian") + 1;
		prefPath = malloc(size);
		snprintf(prefPath, size, "%s/OpenTyrian", appData);
		return prefPath;
	}
#else
	const char *xdgConfigHome = getenv("XDG_CONFIG_HOME");
	if (xdgConfigHome != NULL)
	{
		size_t size = strlen(xdgConfigHome) + strlen("/opentyrian") + 1;
		prefPath = malloc(size);
		snprintf(prefPath, size, "%s/opentyrian", xdgConfigHome);
		return prefPath;
	}

	const char *home = getenv("HOME");
	if (home != NULL)
	{
		size_t size = strlen(home) + strlen("/.config/opentyrian") + 1;
		prefPath = malloc(size);
		snprintf(prefPath, size, "%s/.config/opentyrian", home);
		return prefPath;
	}
#endif

	return NULL;
}
