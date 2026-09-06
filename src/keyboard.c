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
#include "keyboard.h"

#include "joystick.h"
#include "mouse.h"
#include "network.h"
#include "nortsong.h"
#include "opentyr.h"
#include "video.h"

#include <assert.h>

#define POLL_INTERVAL 10

JE_boolean ESCPressed;

bool windowHasFocus;

bool keysactive[SCANCODE_COUNT];

const Scancode lordKeyScancodes[] = { SCANCODE_L, SCANCODE_O, SCANCODE_R, SCANCODE_D };
bool lordKeySymsDown[4] = { 0 };

static KeyboardInput keyboardInputs[32];
static size_t keyboardInputsFront;
static size_t keyboardInputsBack;
static size_t keyboardInputsCount;

Sint32 mouseX;
Sint32 mouseY;
Uint8 mouseButtonsDown;

static MouseInput mouseInputs[4];
static size_t mouseInputsFront;
static size_t mouseInputsBack;
static size_t mouseInputsCount;
static bool mouseHasMotionInput;

static bool mouseRelativeEnabled;

void init_keyboard(void)
{
	plat_text_input(false);

	plat_mouse_show(false);
}

bool keyboardHasInput(void)
{
	return keyboardInputsCount > 0;
}

bool keyboardGetInput(KeyboardInput *out_input)
{
	if (keyboardInputsCount > 0)
	{
		assert(keyboardInputsFront < COUNTOF(keyboardInputs));
		if (out_input != NULL)
			*out_input = keyboardInputs[keyboardInputsFront];
		keyboardInputsFront = keyboardInputsFront == COUNTOF(keyboardInputs) - 1 ? 0 : keyboardInputsFront + 1;
		keyboardInputsCount -= 1;
		return true;
	}

	return false;
}

void keyboardClearInput(void)
{
	keyboardInputsFront = 0;
	keyboardInputsBack = 0;
	keyboardInputsCount = 0;
}

bool mouseHasInput(InputFlags flags)
{
	return mouseInputsCount > 0 || ((flags & INPUT_NO_MOTION) == 0 && mouseHasMotionInput);
}

bool mouseGetInput(InputFlags flags, MouseInput *out_input)
{
	if (mouseInputsCount > 0)
	{
		assert(mouseInputsFront < COUNTOF(mouseInputs));
		if (out_input != NULL)
			*out_input = mouseInputs[mouseInputsFront];
		mouseInputsFront = mouseInputsFront == COUNTOF(mouseInputs) - 1 ? 0 : mouseInputsFront + 1;
		mouseInputsCount -= 1;
		return true;
	}

	if ((flags & INPUT_NO_MOTION) == 0 && mouseHasMotionInput)
	{
		if (out_input != NULL)
		{
			*out_input = (MouseInput)
			{
				.x = mouseX,
				.y = mouseY,
				.button = 0,
			};
		}
		mouseHasMotionInput = false;
		return true;
	}

	return false;
}

void mouseClearInput(void)
{
	mouseInputsFront = 0;
	mouseInputsBack = 0;
	mouseInputsCount = 0;

	mouseHasMotionInput = false;
}

void mouseSetRelative(bool enable)
{
	plat_mouse_relative(enable && windowHasFocus);

	mouseRelativeEnabled = enable;

	Sint32 x, y;
	plat_mouse_relative_motion(&x, &y);  // discard
}

void mouseGetRelativePosition(Sint32 *const out_x, Sint32 *const out_y)
{
	plat_mouse_relative_motion(out_x, out_y);
}

static void pushKeyboardInput(Sint32 sym, Uint16 scancode, Uint16 mod, Uint8 ch)
{
	if (keyboardInputsCount < COUNTOF(keyboardInputs))
	{
		assert(keyboardInputsBack < COUNTOF(keyboardInputs));
		KeyboardInput *const input = &keyboardInputs[keyboardInputsBack];
		input->sym = sym;
		input->scancode = scancode;
		input->mod = mod;
		input->ch = ch;
		keyboardInputsBack = keyboardInputsBack == COUNTOF(keyboardInputs) - 1 ? 0 : keyboardInputsBack + 1;
		keyboardInputsCount += 1;
	}
}

void input_focus(bool focused)
{
	windowHasFocus = focused;

	mouseSetRelative(mouseRelativeEnabled);
}

void input_key_down(Scancode scancode, Uint16 mod, bool repeat)
{
	if (scancode < 0 || scancode >= SCANCODE_COUNT)
		return;

	if (mod & MOD_ALT && scancode == SCANCODE_RETURN)
	{
		toggle_fullscreen();
		return;
	}

	if (!repeat)
		keysactive[scancode] = true;

	for (size_t i = 0; i < COUNTOF(lordKeyScancodes); ++i)
		lordKeySymsDown[i] |= scancode == lordKeyScancodes[i];

	pushKeyboardInput(scancode_to_char(scancode, false), scancode, mod, 0);

	mouseInactive = true;
}

void input_key_up(Scancode scancode)
{
	if (scancode < 0 || scancode >= SCANCODE_COUNT)
		return;

	keysactive[scancode] = false;

	for (size_t i = 0; i < COUNTOF(lordKeyScancodes); ++i)
		lordKeySymsDown[i] &= scancode != lordKeyScancodes[i];
}

void input_text(Uint8 ch)
{
	if (ch == 0)
		return;

	// Text; not a key.
	pushKeyboardInput(-1, (Uint16)-1, MOD_NONE, ch);
}

void input_mouse_motion(Sint32 x, Sint32 y, bool moved)
{
	mouseX = x;
	mouseY = y;

	mouseHasMotionInput = true;

	// Show system mouse pointer if outside screen.
	plat_mouse_show(mouseX < 0 || mouseX >= vga_width ||
	                mouseY < 0 || mouseY >= vga_height);

	if (moved)
		mouseInactive = false;
}

void input_mouse_button(Uint8 button, bool down, Sint32 x, Sint32 y)
{
	if (down)
	{
		if (mouseInputsCount < COUNTOF(mouseInputs))
		{
			assert(mouseInputsBack < COUNTOF(mouseInputs));
			MouseInput *const input = &mouseInputs[mouseInputsBack];
			input->button = button;
			input->x = x;
			input->y = y;
			mouseInputsBack = mouseInputsBack == COUNTOF(mouseInputs) - 1 ? 0 : mouseInputsBack + 1;
			mouseInputsCount += 1;
		}

		mouseButtonsDown |= MOUSE_BUTTON_MASK(button);

		mouseInactive = false;
	}
	else
	{
		mouseButtonsDown &= ~MOUSE_BUTTON_MASK(button);
	}
}

void input_quit(void)
{
	plat_exit(0);
}

void handleInputEvents(void)
{
	plat_pump();
}

bool hasInput(InputFlags flags)
{
	return keyboardHasInput() || mouseHasInput(flags);
}

bool getInput(void)
{
	return keyboardGetInput(NULL) || mouseGetInput(INPUT_NO_MOTION, NULL);
}

void waitUntilHasInput(InputFlags flags)
{
	while (true)
	{
		NETWORK_KEEP_ALIVE();

		push_joysticks_as_keyboard();
		handleInputEvents();

		if (hasInput(flags))
			return;

		plat_delay(POLL_INTERVAL);
	}
}

void waitUntilGetInput(void)
{
	while (true)
	{
		NETWORK_KEEP_ALIVE();

		push_joysticks_as_keyboard();
		handleInputEvents();

		if (getInput())
			return;

		plat_delay(POLL_INTERVAL);
	}
}

void waitUntilElapsed(void)
{
	while (true)
	{
		NETWORK_KEEP_ALIVE();

		push_joysticks_as_keyboard();
		handleInputEvents();

		Uint32 delay = getFrameCountTicks();
		if (delay == 0)
			return;

		plat_delay(MIN(delay, POLL_INTERVAL));
	}
}

bool waitUntilHasInputOrElapsed(void)
{
	while (true)
	{
		NETWORK_KEEP_ALIVE();

		push_joysticks_as_keyboard();
		handleInputEvents();

		if (hasInput(INPUT_NO_MOTION))
			return true;

		Uint32 delay = getFrameCountTicks();
		if (delay == 0)
			return false;

		plat_delay(MIN(delay, POLL_INTERVAL));
	}
}

bool waitUntilGetInputOrElapsed(void)
{
	while (true)
	{
		NETWORK_KEEP_ALIVE();

		push_joysticks_as_keyboard();
		handleInputEvents();

		if (getInput())
			return true;

		Uint32 delay = getFrameCountTicks();
		if (delay == 0)
			return false;

		plat_delay(MIN(delay, POLL_INTERVAL));
	}
}
