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
#include "scancode.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

// Generated from SDL_GetScancodeName() of the official SDL 2.32.10 build, for
// every scancode, so key names in configuration files match upstream's.
static const char *const scancodeNames[SCANCODE_COUNT] =
{
	[4] = "A",
	[5] = "B",
	[6] = "C",
	[7] = "D",
	[8] = "E",
	[9] = "F",
	[10] = "G",
	[11] = "H",
	[12] = "I",
	[13] = "J",
	[14] = "K",
	[15] = "L",
	[16] = "M",
	[17] = "N",
	[18] = "O",
	[19] = "P",
	[20] = "Q",
	[21] = "R",
	[22] = "S",
	[23] = "T",
	[24] = "U",
	[25] = "V",
	[26] = "W",
	[27] = "X",
	[28] = "Y",
	[29] = "Z",
	[30] = "1",
	[31] = "2",
	[32] = "3",
	[33] = "4",
	[34] = "5",
	[35] = "6",
	[36] = "7",
	[37] = "8",
	[38] = "9",
	[39] = "0",
	[40] = "Return",
	[41] = "Escape",
	[42] = "Backspace",
	[43] = "Tab",
	[44] = "Space",
	[45] = "-",
	[46] = "=",
	[47] = "[",
	[48] = "]",
	[49] = "\\",
	[50] = "#",
	[51] = ";",
	[52] = "'",
	[53] = "`",
	[54] = ",",
	[55] = ".",
	[56] = "/",
	[57] = "CapsLock",
	[58] = "F1",
	[59] = "F2",
	[60] = "F3",
	[61] = "F4",
	[62] = "F5",
	[63] = "F6",
	[64] = "F7",
	[65] = "F8",
	[66] = "F9",
	[67] = "F10",
	[68] = "F11",
	[69] = "F12",
	[70] = "PrintScreen",
	[71] = "ScrollLock",
	[72] = "Pause",
	[73] = "Insert",
	[74] = "Home",
	[75] = "PageUp",
	[76] = "Delete",
	[77] = "End",
	[78] = "PageDown",
	[79] = "Right",
	[80] = "Left",
	[81] = "Down",
	[82] = "Up",
	[83] = "Numlock",
	[84] = "Keypad /",
	[85] = "Keypad *",
	[86] = "Keypad -",
	[87] = "Keypad +",
	[88] = "Keypad Enter",
	[89] = "Keypad 1",
	[90] = "Keypad 2",
	[91] = "Keypad 3",
	[92] = "Keypad 4",
	[93] = "Keypad 5",
	[94] = "Keypad 6",
	[95] = "Keypad 7",
	[96] = "Keypad 8",
	[97] = "Keypad 9",
	[98] = "Keypad 0",
	[99] = "Keypad .",
	[101] = "Application",
	[102] = "Power",
	[103] = "Keypad =",
	[104] = "F13",
	[105] = "F14",
	[106] = "F15",
	[107] = "F16",
	[108] = "F17",
	[109] = "F18",
	[110] = "F19",
	[111] = "F20",
	[112] = "F21",
	[113] = "F22",
	[114] = "F23",
	[115] = "F24",
	[116] = "Execute",
	[117] = "Help",
	[118] = "Menu",
	[119] = "Select",
	[120] = "Stop",
	[121] = "Again",
	[122] = "Undo",
	[123] = "Cut",
	[124] = "Copy",
	[125] = "Paste",
	[126] = "Find",
	[127] = "Mute",
	[128] = "VolumeUp",
	[129] = "VolumeDown",
	[133] = "Keypad ,",
	[134] = "Keypad = (AS400)",
	[153] = "AltErase",
	[154] = "SysReq",
	[155] = "Cancel",
	[156] = "Clear",
	[157] = "Prior",
	[158] = "Return",
	[159] = "Separator",
	[160] = "Out",
	[161] = "Oper",
	[162] = "Clear / Again",
	[163] = "CrSel",
	[164] = "ExSel",
	[176] = "Keypad 00",
	[177] = "Keypad 000",
	[178] = "ThousandsSeparator",
	[179] = "DecimalSeparator",
	[180] = "CurrencyUnit",
	[181] = "CurrencySubUnit",
	[182] = "Keypad (",
	[183] = "Keypad )",
	[184] = "Keypad {",
	[185] = "Keypad }",
	[186] = "Keypad Tab",
	[187] = "Keypad Backspace",
	[188] = "Keypad A",
	[189] = "Keypad B",
	[190] = "Keypad C",
	[191] = "Keypad D",
	[192] = "Keypad E",
	[193] = "Keypad F",
	[194] = "Keypad XOR",
	[195] = "Keypad ^",
	[196] = "Keypad %",
	[197] = "Keypad <",
	[198] = "Keypad >",
	[199] = "Keypad &",
	[200] = "Keypad &&",
	[201] = "Keypad |",
	[202] = "Keypad ||",
	[203] = "Keypad :",
	[204] = "Keypad #",
	[205] = "Keypad Space",
	[206] = "Keypad @",
	[207] = "Keypad !",
	[208] = "Keypad MemStore",
	[209] = "Keypad MemRecall",
	[210] = "Keypad MemClear",
	[211] = "Keypad MemAdd",
	[212] = "Keypad MemSubtract",
	[213] = "Keypad MemMultiply",
	[214] = "Keypad MemDivide",
	[215] = "Keypad +/-",
	[216] = "Keypad Clear",
	[217] = "Keypad ClearEntry",
	[218] = "Keypad Binary",
	[219] = "Keypad Octal",
	[220] = "Keypad Decimal",
	[221] = "Keypad Hexadecimal",
	[224] = "Left Ctrl",
	[225] = "Left Shift",
	[226] = "Left Alt",
	[227] = "Left GUI",
	[228] = "Right Ctrl",
	[229] = "Right Shift",
	[230] = "Right Alt",
	[231] = "Right GUI",
	[257] = "ModeSwitch",
	[258] = "AudioNext",
	[259] = "AudioPrev",
	[260] = "AudioStop",
	[261] = "AudioPlay",
	[262] = "AudioMute",
	[263] = "MediaSelect",
	[264] = "WWW",
	[265] = "Mail",
	[266] = "Calculator",
	[267] = "Computer",
	[268] = "AC Search",
	[269] = "AC Home",
	[270] = "AC Back",
	[271] = "AC Forward",
	[272] = "AC Stop",
	[273] = "AC Refresh",
	[274] = "AC Bookmarks",
	[275] = "BrightnessDown",
	[276] = "BrightnessUp",
	[277] = "DisplaySwitch",
	[278] = "KBDIllumToggle",
	[279] = "KBDIllumDown",
	[280] = "KBDIllumUp",
	[281] = "Eject",
	[282] = "Sleep",
	[283] = "App1",
	[284] = "App2",
	[285] = "AudioRewind",
	[286] = "AudioFastForward",
	[287] = "SoftLeft",
	[288] = "SoftRight",
	[289] = "Call",
	[290] = "EndCall",
};

// US layout, for platforms with no text input of their own and for the
// keycode-like 'sym' the game uses for typed cheat words.
char scancode_to_char(Scancode scancode, bool shift)
{
	if (scancode >= SCANCODE_A && scancode <= SCANCODE_Z)
		return (char)((shift ? 'A' : 'a') + (scancode - SCANCODE_A));
	if (scancode >= SCANCODE_1 && scancode <= SCANCODE_0)
	{
		static const char digits[] = "1234567890";
		static const char shifted[] = "!@#$%^&*()";
		return (shift ? shifted : digits)[scancode - SCANCODE_1];
	}
	if (scancode >= SCANCODE_KP_1 && scancode <= SCANCODE_KP_0)
		return "1234567890"[scancode - SCANCODE_KP_1];

	switch (scancode)
	{
	case SCANCODE_SPACE:        return ' ';
	case SCANCODE_RETURN:
	case SCANCODE_KP_ENTER:     return '\r';
	case SCANCODE_ESCAPE:       return 27;
	case SCANCODE_BACKSPACE:    return '\b';
	case SCANCODE_TAB:          return '\t';
	case SCANCODE_MINUS:        return shift ? '_' : '-';
	case SCANCODE_EQUALS:       return shift ? '+' : '=';
	case SCANCODE_LEFTBRACKET:  return shift ? '{' : '[';
	case SCANCODE_RIGHTBRACKET: return shift ? '}' : ']';
	case SCANCODE_BACKSLASH:    return shift ? '|' : '\\';
	case SCANCODE_SEMICOLON:    return shift ? ':' : ';';
	case SCANCODE_APOSTROPHE:   return shift ? '"' : '\'';
	case SCANCODE_GRAVE:        return shift ? '~' : '`';
	case SCANCODE_COMMA:        return shift ? '<' : ',';
	case SCANCODE_PERIOD:       return shift ? '>' : '.';
	case SCANCODE_SLASH:        return shift ? '?' : '/';
	case SCANCODE_KP_DIVIDE:    return '/';
	case SCANCODE_KP_MULTIPLY:  return '*';
	case SCANCODE_KP_MINUS:     return '-';
	case SCANCODE_KP_PLUS:      return '+';
	case SCANCODE_KP_PERIOD:    return '.';
	default:                    return 0;
	}
}

const char *scancode_name(Scancode scancode)
{
	if (scancode < 0 || scancode >= SCANCODE_COUNT || scancodeNames[scancode] == NULL)
		return "";
	return scancodeNames[scancode];
}

Scancode scancode_from_name(const char *name)
{
	if (name == NULL || name[0] == '\0')
		return SCANCODE_UNKNOWN;
	for (int i = 0; i < SCANCODE_COUNT; ++i)
		if (scancodeNames[i] != NULL && strcasecmp(scancodeNames[i], name) == 0)
			return (Scancode)i;
	return SCANCODE_UNKNOWN;
}
