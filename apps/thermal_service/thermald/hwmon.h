/*
 * Thermal Service Manager
 *
 * Entry point for the Thermal Service
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __HWMON_H__
#define __HWMON_H__

/**
 * Represents a error within the HWMON subsystem
 */
typedef enum {
	HWMON_OK = 0, /*< No error */
	HWMON_ERROR = -1, /*< General error */
	HWMON_ERROR_UNKNOWN_SENSOR = -2, /*< Sensor request does not exist */
	HWMON_ERROR_NOT_SUPPORTED_FOR_SENSOR = -3 /*< Sensor does not support requested action */
} hwmon_errorcode_t;

/**
 * Brings up HWMON subsystem
 */
void hwmon_init(void);

/**
 * Tries to find all the hwmon sensors available in the system
 * @return HWMON error code
 */
int hwmon_find_sensors(void);

/**
 * Retrieves the value that is currently reported by a sensor. Value returned
 * is a +ve integer.
 * @param sensor Sensor to get current reading for
 * @return HWMON error code or current reading
 */
int hwmon_get_current_value(const char *sensor);

/**
 * Configures and starts monitoring to ensure a sensor does not stray between
 * the supplied min/max values
 * @param sensor Sensor to monitor
 * @param min_value Min value before alarm sounds.
 * @param max_value Max value before alarm sounds
 * @return HWMON error code
 */
int hwmon_set_sensor_alarms(const char *sensor, int min_value, int max_value);

/**
 * Requests that the critical alarm for a given sensor is monitored
 * @param sensor The sensor to monitor critical alarm from
 * @return HWMON error code
 */
int hwmon_critical_alarm(const char *sensor);

#endif
