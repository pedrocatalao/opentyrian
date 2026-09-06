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
#ifndef LOGGING_H
#define LOGGING_H

#include "platform.h"

#if defined(__GNUC__) || defined(__clang__)
#define LOG_PRINTF(fmtarg) __attribute__((format(printf, fmtarg, fmtarg + 1)))
#else
#define LOG_PRINTF(fmtarg)
#endif

void logDebug(const char *fmt, ...) LOG_PRINTF(1);
void logInfo(const char *fmt, ...) LOG_PRINTF(1);
void logWarn(const char *fmt, ...) LOG_PRINTF(1);
void logError(const char *fmt, ...) LOG_PRINTF(1);
void logFatal(const char *fmt, ...) LOG_PRINTF(1);

#endif /* LOGGING_H */
