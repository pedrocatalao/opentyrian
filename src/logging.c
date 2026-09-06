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
#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

static void logMessage(LogLevel level, const char *fmt, va_list ap)
{
	char buffer[4096];
	vsnprintf(buffer, sizeof buffer, fmt, ap);
	plat_log(level, buffer);
}

#define LOG_FUNCTION(name, level) \
	void name(const char *fmt, ...) \
	{ \
		va_list ap; \
		va_start(ap, fmt); \
		logMessage(level, fmt, ap); \
		va_end(ap); \
	}

LOG_FUNCTION(logDebug, LOG_DEBUG)
LOG_FUNCTION(logInfo, LOG_INFO)
LOG_FUNCTION(logWarn, LOG_WARN)
LOG_FUNCTION(logError, LOG_ERROR)
LOG_FUNCTION(logFatal, LOG_FATAL)
