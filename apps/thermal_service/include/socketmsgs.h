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
#ifndef __SOCKETMSGS_H__
#define __SOCKETMSGS_H__

/* as a rule of thumb, any pointer defined bellow will be updated
 * by the receiver to point to msg + sizeof(msg_type) (ie the end
 * of the message).
 * if there is more than 1 pointer, then an explanation of where
 * the message data is allocated will be provided.
 */

/**
 * Thermal message ids
 */
typedef enum {
	INVALID = -1, /*< Invalid message ID */
	THERMAL_NOTIFICATION_EVENT = 0, /*< A new notification has arrived */
	GET_ACTIONS_REQ, /*< Requests all actions in platform */
	GET_ACTIONS_RESP, /*< Response to all actions in platform request*/
	GET_SENSORS_REQ, /*< Requests all sensors in platform */
	GET_SENSORS_RESP, /*< Response to all sensors in platform request */
	GET_SENSOR_CONFIG_REQ, /*< Request a specific sensor configuration */
	GET_SENSOR_CONFIG_RESP /*< Sensor configuration result */
} thermal_msgid_t;

/**
 * Thermal message present in all messages to provide
 * a common place for message id and length
 */
typedef struct {
	int len; /*< Total length (in bytes) of message (including header) */
	thermal_msgid_t id; /*< Message ID of message */
} thermal_socket_msg_t;

/**
 * Request struct for GET_ACTIONS_REQ
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Common message header */
} thermal_get_actions_req_msg_t;

/**
 * Response struct to GET_ACTIONS_REQ message to return all the
 * actions currently defined within the system.
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Message header */
	int number_of_actions; /*< Number of action strings returned */
	char *actions; /*< Pseudo reference to actions strings. Upon receipt this field
			    must be updated to (msg + sizeof(thermal_get_actions_resp_msg_t) as
			    the string's are stored after the struct. Strings are store in a continuous
			    NULL terminated block. */
} thermal_get_actions_resp_msg_t;

/**
 * Request struct for GET_SENSORS_REQ
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Common message header */
} thermal_get_sensors_req_msg_t;


/**
 * Response struct to GET_SENSORS_REQ message to return all the actions
 * currently defined within the system.
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Message header */
	int number_of_sensors; /*< Number of sensor strings returned */
	char *sensors; /*< Pseudo reference to sensors strings. Upon receipt this field
			    must be updated to (msg + sizeof(thermal_get_sensors_resp_msg_t) as
			    the string's are stored after the struct. Strings are store in a continuous
			    NULL terminated block. */
} thermal_get_sensors_resp_msg_t;

/**
 * Sent with a GET_SENSOR_CONFIG_REQ message to retrieve a sensors current
 * configuration
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Message header */
	char *sensor; /*< Pseudo reference to sensor string. Upon receipt this field
			  must be updated to (msg + sizeof(thermal_get_sensor_config_req_msg_t) as
			  the string is stored after the struct. */
} thermal_get_sensor_config_req_msg_t;

/**
 * Used within the GET_SENSOR_CONFIG_RESP & SET_SENSOR_CONFIG_REQ to store
 * notification level data.
 */
typedef struct {
	int len; /*< Total length of this "notification level" including all the space
		    allocated for the action strings (and their NULL terminators) */
	int level; /*< Notification level */
	int min_alarm_value; /*< Notification minimum alarm value */
	int max_alarm_value; /*< Notification maximum alarm value */
	int action_count; /*< How many actions are associated (hence sent) with this
			     notification level */
	char *actions; /*< Pseudo reference to actions strings. Upon receipt this field
			    must be updated to (msg + sizeof(thermal_get_sensor_resp_msg_t) as
			    the string's are stored after the struct. Strings are storde in a continuous
			    NULL terminated block. */
} thermal_sensor_notification_level_t;

/**
 * Struct sent in response to GET_SENSOR_CONFIG_REQ message.
 *
 * If error is not 0 then none of the fields can be deemed as valid and the
 * appropriate error code will be set.
 *
 * The actual message received will be allocated as defined below:
 *
 * thermal_get_sensor_config_resp_msg_t
 *   thermal_sensor_notification_level_t
 *     action string space
 * ...
 *
 * Thus to move between action levels the caller must first do something like:
 *
 *    thermal_get_sensor_config_resp_msg_t *msg;
 *    thermal_sensor_notication_level_t *notification_level;
 *
 *    msg = RECEIVE_MESSAGE();
 *    msg->levels = (msg + sizeof(thermal_get_sensor_config_resp_msg_t);
 *    notification_level = levels;
 *
 * Now notification_level points to the first level returned, to move to the
 * next:
 *
 *   notification_level += notification_level->len;
 *
 * The called must make used of the notification_level_count field to prevent
 * a buffer overrun.
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Message header */
	int error; /*< Error code */
	int notification_level_count; /*< Number of notification levels in message */
	int current_level; /*< Notification level sensor is currently operating in */
	thermal_sensor_notification_level_t *levels; /**< See struct description for full explanation */
} thermal_get_sensor_config_resp_msg_t;

/**
 * Notification event
 */
typedef struct {
	thermal_socket_msg_t msg; /*< Common message header */
	char *sensor; /*< Pseudo pointer to sensor name, located at end of struct */
	int shutdown_timeout; /*< Timeout till platform shutdown (or 0 for no timeout) */
	int current_level; /*< New notification level for sensor */
	int previous_level; /*< Sensors previous notification level */
	int actions_taken_count; /*< How many mitigation actions must occur */
	char **actions_taken; /*< Array of mitigation actions */
} thermal_notification_event_msg_t;

#endif
