/*
 * libthermal API
 *
 * ThermalManager Interface
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef __THERMAL_MANAGER_H__
#define __THERMAL_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
* Full path to the Unix socket used for communication with the ThermalManager
* process
*/
#define THERMAL_MANAGER_DEFAULT_SOCKET "/tmp/thermal.skt"

/**
* A Thermal Notification, containing all information that is stored witin
* the ThermalManager relating to the change in Thermal level for a given 
* sensor
*/

typedef struct {
	char *sensor; /*< Sensor name */
	int current_level; /*< Current notification level */
	int previous_level; /*< Previous notification level */
	int shutdown_in; /*< Time, in seconds, until platform shuts down due to this notification. If 0 then platform will not shutdown */
	char **actions_taken; /*< Array of actions that have been/should be performed due to this event. */
	int number_of_actions_taken; /*< Size of the actions_taken array */

} STE_TM_notification_t;

/**
* Error codes returned by the error callback
*/
typedef enum {
	TM_OK, /*< No error */
	TM_NOT_INITALISED, /*< STE_TM_init() has not yet been called! */
	TM_UNABLE_TO_CONNECT_TO_THERMAL_MANAGER, /*< Unable to communicate with ThermalManager */
	TM_INVALID_DATA_RECEIVED, /*< The ThermalManager has send invalid information */
	TM_CONNECTION_DROPPED, /*< The ThermalManager connection has died unexpectedly */
	TM_UNABLE_TO_ALLOCATE_MEMORY, /*< No more memory can be allocated by libthermal */
	TM_INVALID_PARAMETER /*< Invalid parameter passed into libthermal */
} STE_TM_error_t;

/**
* Callback registered with library that will be called when a new notification
* arrives
* @param notification Pointer to notification object. This pointer is only valid
* during the life of this function call. If a copy is required use:
* STE_TM_clone_notification()
*/
typedef void (*STE_TM_notification_cb_t)(STE_TM_notification_t *notification);

/**
* Callback registered with library that will be called when an error occurs
* @param error Error condition that has occurred
*/
typedef void (*STE_TM_error_cb_t)(STE_TM_error_t error);

/**
* Callback struct registered with library
*/
typedef struct {
	STE_TM_notification_cb_t notification_cb;
	STE_TM_error_cb_t error_cb;
} STE_TM_callbacks_t;

/**
* A Notification level
*
* Each STE_TM_sensor_config_t can contain 1 or more notification levels max_alarm_level;
* which details what will occur when thermal sensors transition between number_of_actions;
* different states
*/
typedef struct {
	int notification_level; /*< The notification level that is being described */
	int min_alarm_level; /*< The min value of the alarm range. 0 for disabled */
	int max_alarm_level; /*< The max value of the alarm range. 0 for disabled */
	int number_of_actions; /*< Number of actions stored within actions array */
	char **actions; /*< Array of actions that will be execute when this level is entered */
} STE_TM_notification_level_t;

/**
* Sensor configuration struct
*/
typedef struct {
	char *name; /*< Sensor name */
	int no_notification_levels; /*< Number of notification levels in array */
	int current_notification_level; /*< The level sensor is currently in */
	STE_TM_notification_level_t **notification_levels; /*< Array of notification levels */
} STE_TM_sensor_config_t;

/**
 * Initalize the Thermal Manager
 * @param cb Callback functions
 * @return 0 on success, -1 on failure
 */
int STE_TM_init(STE_TM_callbacks_t *callbacks);

/**
* Get the actions that are available in the thermal management system.
* @param actions A pointer to an array of strings that will be populated
* by the library.
* @return number items in allocated array or -1 for failure
*/
int STE_TM_get_actions(char ***actions);

/**
* Free memory associated to STE_TM_get_actions when it is no longer required.
* @param count Number of items with in the string array.
* @param actions Pointer to a string array allocated by the library
*/
void STE_TM_free_actions(int count, char ***actions);

/**
* Get the sensors that are available in the thermal management system.
* @param sensors A pointer to an array of strings that will be populated by
* the library.
* @return Number of items in sensors array or -1 for failure
*/
int STE_TM_get_sensors(char ***sensors);

/**
* Free memory associated to STE_TM_get_sensors when it is no longer required.
* @param count Number of items within the string array
* @param sensors Pointer to a string array allocated by the library
*/
void STE_TM_free_sensors(int count, char ***sensors);

/**
* Retrieves configuration of a particular sensor
* @param sensor name of sensor.
* @param config configuration details of the sensor in question.
*               Caller must free returned memory.
*/
int STE_TM_get_sensor_config(const char *sensor, STE_TM_sensor_config_t **config);

/**
 * Frees memory associated to a given sensor configuration.
 * @param sensor the sensor data to free.
 */
void STE_TM_free_sensor_config(STE_TM_sensor_config_t **sensor);

/**
* Clones a notification. You must explicitly free a cloned notification
* with STE_TM_free_cloned_notification()
* @param original Notification to clone
* @return Pointer to the cloned notification
*/
STE_TM_notification_t *STE_TM_clone_notification(STE_TM_notification_t *original);

/**
* Frees a cloned notification
* @param notification Notification object to free
*/
void STE_TM_free_cloned_notification(STE_TM_notification_t **notification);

/**
 * Clean up the thermal manager and close the socket connection.
 */
void STE_TM_deinit(void);

#ifdef __cplusplus
}
#endif

#endif


