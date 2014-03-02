/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "socketmsgs.h"

/**
 * Notification level used to represent a Critical level
 */
#define SENSOR_CRITICAL_NOTIFICATION_LEVEL (-1234)

/**
 * What sensor attribute caused the event
 */
typedef enum {
	SENSOR_EVENT_MIN, /*< Min level in set range */
	SENSOR_EVENT_MAX, /*< Max level in set range */
	SENSOR_EVENT_CRITICAL, /*< Critical sensor */
	SENSOR_EVENT_UNKNOWN, /*< Unknown attribute */
} sensor_event_t;

/**
 * Bring up the sensors subsystem
 */
void sensor_init(void);

/**
 * Creates (if needed) a sensor configuration with in the sensor
 * subsystem
 * @param name Name of sensor to create
 * @return 0 if success
 */
int sensor_new(const char *name);

/**
 * Checks to see if the configuration for a sensor exists
 * @param name Name of sensor to search for
 * @return 0 if it exists
 */
int sensor_exists(const char *name);

/**
 * Adds a new notification level configuration to a sensor
 * @param name Name of sensor to add notification config level to
 * @param notification_level Notification level to define (only +ve numbers)
 * @param min_alarm_level Sensor reading at which to trigger min alarm
 * @param max_alarm_level Sensor reading at which to trigger max alarm
 * @return 0 if success.
 */
int sensor_new_notification_level(const char *name,
				  const int notification_level,
				const int min_alarm_level, const int max_alarm_level);

/**
 * Adds a mitigation action to a notification level
 * @param name Name of sensor to add mitigation action to
 * @param notification_level Notification level to add action to
 * @param action Action string
 */
int sensor_set_action_for_level(const char *name,
				const int notification_level,
				const char *action);
/**
 * Used to do the initial configure of all the platform sensors
 */
int sensor_configure_all(void);

/**
 * Returns in *list a string of all sensors in the form
 *  SENSOR1\0SENSOR2\0SENSOR3\0.
 * Called must free allocate memory
 * @param List List of strings
 * @param count Number of strings contained in list
 * @return 0 if error, or total length of allocated memory for list
 */
int sensor_getlist(char **list, int *count);

/**
 * Returns a GET_SENSOR_CONFIG_RESP struct for use in the
 * socket server
 * @param sensor Name of sensor to get config for
 * @param outmsg Message that is allocated
 * @return 0 for error or Length of message allocated.
 */
int sensor_getsensorresp(char *sensor,
			thermal_get_sensor_config_resp_msg_t **outmsg);

/**
 * Called when the hwmon subsystem detects a sensor event so the
 * sensor subsystem can deal with the event.
 * @param sensor Name of sensor that caused the event
 * @param event Sensor attribute that caused the event to occur
 */
void sensor_event(char *sensor, sensor_event_t event);

#endif
