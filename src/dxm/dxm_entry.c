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
// dxm_entry.c — what makes this game a DOS ex Machina core (PORTING.md §4):
// the info block and the three fixed-name exports.
#include "dxm_core.h"

#include "opentyr.h"

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

void dxm_adapter_audio(int16_t *out, int frames);   // platform_dxm.c

static const dxm_mode MODE_13H = { 320, 200, DXM_FB_INDEX8, 5, 6, 400 };

static const dxm_core_info INFO = {
	DXM_ABI,
	"tyrian", "TYRIAN.EXE", "Tyrian", "Epic MegaGames", 1995,
	&MODE_13H, 1, "tyrian1.lvl"
};

DXM_EXPORT const dxm_core_info *dxm_core_get_info(void)
{
	return &INFO;
}

DXM_EXPORT void dxm_core_audio(int16_t *out, int frames)
{
	dxm_adapter_audio(out, frames);
}

DXM_EXPORT int dxm_core_main(const dxm_host *host, const char *data_dir)
{
	dxm_adapter_bind(host);

	if (setjmp(*dxm_adapter_exit_target()))   // PORTING.md §3.1: plat_exit() lands here
		return 0;

	// The data directory is wherever the shell installed the game; hand it
	// over the same way a user would on the command line.
	char dataArg[1100];
	snprintf(dataArg, sizeof dataArg, "--data=%s", data_dir);
	char *argv[] = { "opentyrian", dataArg, NULL };

	return opentyrian_main(2, argv);
}
