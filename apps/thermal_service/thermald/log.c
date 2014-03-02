/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "log.h"

int LOG_level;

void log_level(const int level) {
	if (level < 0) {
		LOG_level = LOG_LEVEL_DEFAULT;
	} else if (level > LOG_LEVEL_DEBUG) {
		LOG_level = LOG_LEVEL_DEBUG;
	} else {
		LOG_level = level;
	}
}
