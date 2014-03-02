/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _THERMALUTILS_H
#define _THERMALUTILS_H

/* Error codes */
#define ERROR_INVALID_XML_CONFIG	-1
#define ERROR_SENSOR_NOT_FOUND		-2
#define ERROR_INVALID_INPUT		-3
#define ERROR_OUT_OF_MEMORY		-4

/*
 * Initialization function to be called
 * once during init
 */
void init_thermal_utils(void);

/*
 * Deinitialization function to be called
 * on exit to release resources
 */
void deinit_thermal_utils(void);

/*
 * Convert sensor temperature from
 * millivolts to degree celsius
 * name: sensor name read from sensor sysfs file
 * input: sensor temp in millivolts
 * output: temp in degree celsius
 * returns zero on success and error code otherwise
 */
int convert_sensor_mv_dc(const char *name, const int input, int *output);

/*
 * Convert sensor temperature from
 * degree celsius to millivolts
 * name: sensor name read from sensor sysfs file
 * input: sensor temp in degree celsius
 * output: temp in millivolts
 * returns zero on success and error code otherwise
 */
int convert_sensor_dc_mv(const char *name, const int input, int *output);

#endif /* _THERMALUTILS_H */
