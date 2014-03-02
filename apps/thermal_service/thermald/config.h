/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

/**
 * Parses configuration file to load sensor configuration
 * params
 * @param configfile Full path to config file
 * @return 0 if success, <0 if not
 */
int parse_config(const char *configfile);

/**
 * Get shutdown timeout
 * @return Timeout in seconds
 */
int config_getshutdowntimeout(void);

#endif
