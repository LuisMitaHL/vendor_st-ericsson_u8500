/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "sensors.h"
#include "socket.h"
#include "actions.h"
#include "hwmon.h"
#include "config.h"
#include "log.h"

#include "socketmsgs.h"

/* We use the Android wakelock's to prevent the platform from
 * going to sleep when the platform is transitioning between
 * thermal states
 */
#define WAKELOCK_KEY "thermald"
#define WAKELOCK_LOCKPATH "/sys/power/wake_lock"
#define WAKELOCK_UNLOCKPATH "/sys/power/wake_unlock"

// XXX ensure list is ordered

/* an action */
typedef struct sensor_notification_action_s {
	char *action;
	struct sensor_notification_action_s *next;
} sensor_notification_action_t;

/* a notification level */
typedef struct sensor_notification_level_s {
	int level;
	int min_alarm_value;
	int max_alarm_value;
	int will_shutdown;
	sensor_notification_action_t *actions;
	struct sensor_notification_level_s *next;
} sensor_notification_level_t;

/* a sensor */
typedef struct sensor_config_s {
	char *sensor_name;
	int notification_level_count;
	int current_level;
	sensor_notification_level_t *levels;
	struct sensor_config_s *next;
} sensor_config_t;

static sensor_config_t *sensor_get(const char *name);
static sensor_notification_level_t *sensor_get_notification_level(sensor_config_t *sensor, int level);
static sensor_notification_level_t *determine_current_notification_level(sensor_config_t *s, int value);
static void raise_socket_event(sensor_config_t *sensor, sensor_notification_level_t *level, sensor_event_t event);
static inline void wakelock(void);
static inline void wakeunlock(void);

static sensor_config_t *sensor_config = NULL;
static pthread_mutex_t sensor_lock;

/*
 * Bring up sensor "subsystem"
 */
void sensor_init(void) {
	pthread_mutex_init(&sensor_lock, NULL);
}

/*
 * Create a new sensor if need be
 */
int sensor_new(const char *name) {
	sensor_config_t *sensor;
	int ret = 0;

	if (sensor_exists(name)) {
		return 0;
	}

	DBG("adding new sensor %s\n", name);

	/* allocate & populate new sensor struct */
	sensor = (sensor_config_t *) malloc(sizeof(sensor_config_t));
	if (sensor == NULL) {
		ERR("unable to allocate memory\n");
		return -1;
	}

	sensor->sensor_name = strdup(name);
	if (sensor->sensor_name == NULL) {
		ERR("unable to allocate memory\n");
		free(sensor);
		ret = -1;
		goto error;
	}

	sensor->notification_level_count = 0;
	sensor->current_level = -1;
	sensor->levels = NULL;
	sensor->next = NULL;

	pthread_mutex_lock(&sensor_lock);

	/* stick into the main sensor linked list */
	if (sensor_config == NULL) {
		sensor_config = sensor;
	} else {
		sensor_config_t *nxt;

		nxt = sensor_config;
		while (nxt->next != NULL) {
			nxt = nxt->next;
		}

		nxt->next = sensor;
	}

error:
	pthread_mutex_unlock(&sensor_lock);
	return ret;
}

/*
 * Determines if a sensor exists
 */
int sensor_exists(const char *name) {
	sensor_config_t *nxt;

	pthread_mutex_lock(&sensor_lock);

	nxt = sensor_config;
	while (nxt != NULL) {
		if (strcmp(nxt->sensor_name, name) == 0) {
			pthread_mutex_unlock(&sensor_lock);
			return 1;
		}
		nxt = nxt->next;
	}

	pthread_mutex_unlock(&sensor_lock);

	return 0;
}

/*
 * Adds a new notification level with the specified activation value for
 * the specified sensor
 */
int sensor_new_notification_level(const char *name,
				const int notification_level,
				const int min_alarm_level,
				const int max_alarm_level) {
	sensor_config_t *s;
	sensor_notification_level_t *nl;

	pthread_mutex_lock(&sensor_lock);

	/* check if the sensor & notification level already exist */
	s = sensor_get(name);
	if (s == NULL) {
		goto error;
	}

	nl = sensor_get_notification_level(s, notification_level);
	if (nl != NULL) {
		ERR("notification level already set. Config file invalid\n");
		goto error;
	}

	DBG("sensor=%s level=%d min=%d max=%d\n", name, notification_level,
		min_alarm_level, max_alarm_level);

	/* allocate & populate struct */
	nl = (sensor_notification_level_t *) malloc(sizeof(sensor_notification_level_t));
	if (nl == NULL) {
		ERR("unable to allocate memory\n");
		goto error;
	}

	nl->level = notification_level;
	nl->min_alarm_value = min_alarm_level;
	nl->max_alarm_value = max_alarm_level;
	nl->will_shutdown = 0;
	nl->next = NULL;
	nl->actions = NULL;

	/* add it in the sensors notification level list */
	if (s->levels == NULL) {
		s->levels = nl;
	} else {
		sensor_notification_level_t *nxt = s->levels;

		while (nxt->next != NULL) {
			nxt = nxt->next;
		}

		nxt->next = nl;
	}

	if (notification_level != SENSOR_CRITICAL_NOTIFICATION_LEVEL) {
		s->notification_level_count++;
	}

	pthread_mutex_unlock(&sensor_lock);
	return 0;
error:
	pthread_mutex_unlock(&sensor_lock);
	return -1;
}

/*
 * Adds an action to a sensors notification level actions list
 */
int sensor_set_action_for_level(const char *name,
				const int notification_level,
				const char *action) {

	sensor_config_t *s;
	sensor_notification_level_t *nl;
	sensor_notification_action_t *a;

	DBG("sensor=%s level=%d action=%s\n",
		name, notification_level, action);

	if (name == NULL || action == NULL) {
		ERR("invalid parameters passed\n");
		return -1;
	}

	pthread_mutex_lock(&sensor_lock);

	/* check sensor & notification level */
	s = sensor_get(name);
	if (s == NULL) {
		goto error;
	}

	nl = sensor_get_notification_level(s, notification_level);
	if (nl == NULL) {
		ERR("notification level %d for sensor %s not defined. Config file invalid\n",
			notification_level, name);
		goto error;
	}

	actions_add(action);

	/* allocate & populate action struct */
	a = (sensor_notification_action_t *) malloc(sizeof(sensor_notification_action_t));
	if (a == NULL) {
		ERR("unable to allocate memory\n");
		goto error;
	}

	a->action = strdup(action);
	if (a->action == NULL) {
		ERR("unable to allocate memory\n");
		free(a);
		goto error;
	}
	a->next = NULL;

	/* if were adding the shutdown action, mark this level as a level
	 * that will cause the platform to shutdown */
	if (strcmp(a->action, ACTION_SHUTDOWN_NAME) == 0) {
		nl->will_shutdown = 1;
	}

	/* add it in the sensors notification action list */
	if (nl->actions == NULL) {
		nl->actions = a;
	} else {
		sensor_notification_action_t *nxt = nl->actions;

		while (nxt->next != NULL) {
			nxt = nxt->next;
		}

		nxt->next = a;
	}

	pthread_mutex_unlock(&sensor_lock);
	return 0;
error:
	pthread_mutex_unlock(&sensor_lock);
	return -1;
}

/*
 * Deletes a sensor from the sensor list.  NOTE: Caller needs to hold the
 * sensor_lock mutex.
 */
static void sensor_delete(sensor_config_t *sensor) {
	sensor_config_t *next = sensor->next;

	if (sensor_config == sensor) {
		sensor_config = next;
	} else {
		sensor_config_t *prev = sensor_config;

		while (prev->next && prev->next != sensor) {
			prev = prev->next;
		}

		if (!prev->next) {
			ERR("trying to remove non-existent sensor!");
			return;
		}

		prev->next = next;
	}

	free(sensor->sensor_name);
	free(sensor);
}

/*
 * Setup the initial sensor configuration
 */
int sensor_configure_all(void) {
	sensor_config_t *nxt;
	sensor_config_t *tmp;

	pthread_mutex_lock(&sensor_lock);

	nxt = sensor_config;
	while (nxt != NULL) {
		sensor_notification_level_t *n;
		int value;

		/* first up check to see if the sensor has a critical
		 * alarm configured */
		n = sensor_get_notification_level(nxt, SENSOR_CRITICAL_NOTIFICATION_LEVEL);
		if (n != NULL) {
			hwmon_critical_alarm(nxt->sensor_name);
			if (nxt->notification_level_count == 0) {
				goto end;
			}
		}

		/* get sensor value */
		value = hwmon_get_current_value(nxt->sensor_name);
		switch (value) {
		case HWMON_ERROR:
			ERR("unknown error\n");
			pthread_mutex_unlock(&sensor_lock);
			return -1;

		case HWMON_ERROR_UNKNOWN_SENSOR:
			ERR("sensor %s does not exist, removing it and continuing\n", nxt->sensor_name);
			tmp = nxt;
			nxt = tmp->next;
			sensor_delete(tmp);
			continue;

		case HWMON_ERROR_NOT_SUPPORTED_FOR_SENSOR:
			/* if sensor does not report current value, then set the
			 * alarm at the lowest notification level
			 */
			n = sensor_get_notification_level(nxt,
							nxt->notification_level_count - 1);
			INF("sensor %s value is unknown. assuming level %d\n",
				nxt->sensor_name, n->level);
			break;

		default:
			/* any other value is a real reading! */
			n = determine_current_notification_level(nxt, value);

			INF("sensor %s currently reporting %d (level %d)\n",
				nxt->sensor_name, value,
				(n == NULL) ? -1 : n->level);
			break;
		}

		if (n != NULL) {
			nxt->current_level = n->level;
			hwmon_set_sensor_alarms(nxt->sensor_name,
						n->min_alarm_value,
						n->max_alarm_value);
		} else {
			ERR("unable to determine current notification level\n");
			nxt->current_level = -1;
		}

	end:
		nxt = nxt->next;
	}

	pthread_mutex_unlock(&sensor_lock);
	return 0;
}

/*
 * Returns list of sensors known to system
 */
int sensor_getlist(char **list, int *count) {
	sensor_config_t *sensor;
	int i, len, l;
	char *str;

	len = i = 0;
	str = NULL;

	pthread_mutex_lock(&sensor_lock);

	sensor = sensor_config;
	while (sensor != NULL) {
		char *tmp;
		l = strlen(sensor->sensor_name) + 1; /* for \0 */
		tmp = realloc(str, len + l); /* current length + space for new string */
		if (tmp == NULL) {
			ERR("unable to allocate memory\n");
			if (str != NULL) {
				free(str);
			}
			i = len = 0;
			goto end;
		}

		str = tmp;

		strncpy((str + len), sensor->sensor_name, l);
		len += l; /* total length of current string is add to len */
		i++; /* increment count */

		sensor = sensor->next;
	}

end:
	*list = str;
	*count = i;

	pthread_mutex_unlock(&sensor_lock);

	return len;
}

/*
 * Builds a GET_SENSOR_RESP message
 */
int sensor_getsensorresp(char *sensor,
			thermal_get_sensor_config_resp_msg_t **outmsg) {
	sensor_config_t *s;
	sensor_notification_level_t *n;
	sensor_notification_action_t *a;
	thermal_get_sensor_config_resp_msg_t *msg;

	/* build the basic return struct */
	msg = (thermal_get_sensor_config_resp_msg_t *)
		malloc(sizeof(thermal_get_sensor_config_resp_msg_t));
	if (msg == NULL) {
		ERR("unable to allocate memory\n");
		*outmsg = NULL;
		return -1;
	}

	msg->msg.len = sizeof(thermal_get_sensor_config_resp_msg_t);
	msg->msg.id = GET_SENSOR_CONFIG_RESP;
	msg->error = 0;

	/* find the appropriate sensor */
	if (!sensor_exists(sensor)) {
		msg->error = 1;
		*outmsg = msg;
		return msg->msg.len;
	}

	s = sensor_get(sensor);

	/* fill in stuff from the sensor struct */
	msg->notification_level_count = s->notification_level_count;
	msg->current_level = s->current_level;

	/* now for each notification level */
	n = s->levels;
	while (n != NULL) {
		thermal_sensor_notification_level_t *level;
		thermal_get_sensor_config_resp_msg_t *tmp;

		level = NULL;

		/* build the struct */
		level = (thermal_sensor_notification_level_t *) malloc(sizeof(thermal_sensor_notification_level_t));
		if (level == NULL) {
			ERR("unable to allocate memory\n");
			msg->error = 1;
			*outmsg = msg;
			return msg->msg.len;
		}

		level->len = sizeof(thermal_sensor_notification_level_t);
		level->level = n->level;
		level->min_alarm_value = n->min_alarm_value;
		level->max_alarm_value = n->max_alarm_value;
		level->action_count = 0;

		/* now search through each action sticking it at the end
		 * of the allocated data */
		a = n->actions;
		while (a != NULL) {
			thermal_sensor_notification_level_t *tmplevel;
			int len;

			len = strlen(a->action) + 1;
			tmplevel = realloc(level, level->len + len);
			if (tmplevel == NULL) {
				ERR("unable to allocate memory\n");
				if (level != NULL) {
					free(level);
				}
				msg->error = 1;
				*outmsg = msg;
				return msg->msg.len;
			}
			level = tmplevel;

			strncpy((((char *) level) + level->len),
				a->action,
				len);

			level->len += len;
			level->action_count++;
			a = a->next;
		}

		/* stick it at the end of the allocated message & update
		 * total length */
		tmp = realloc(msg, msg->msg.len + level->len);
		if (tmp == NULL) {
			ERR("unable to allocate memory\n");
			if (msg != NULL) {
				msg->error = 1;
				*outmsg = msg;
				return msg->msg.len;
			} else {
				*outmsg = NULL;
				return 0;
			}
		}

		msg = tmp;

		memcpy((((char *) msg) + msg->msg.len), level, level->len);
		msg->msg.len += level->len;

		if (level != NULL) {
			free(level);
		}

		n = n->next;
	}

	*outmsg = msg;
	return msg->msg.len;
}

/*
 * Called when an HWMON event occurs. We put a wakelock in place to prevent
 * the system going to sleep while we change cpufreq etc
 */
void sensor_event(char *sensor, sensor_event_t event) {
	sensor_notification_action_t *actions;
	sensor_config_t *s;
	sensor_notification_level_t *nl;
	int value;

	wakelock();

	INF("thermal event occurred on %s\n", sensor);

	pthread_mutex_lock(&sensor_lock);

	s = sensor_get(sensor);
	if (s == NULL) {
		ERR("unknown sensor %s caused event\n", sensor);
		goto error;
	}

	/* we need to handle alarms a little different to the critical alarm, as
	 * with the critical alarm's there is nothing extra to reconfigure.
	 */
	switch (event) {
	case SENSOR_EVENT_MIN:
	case SENSOR_EVENT_MAX:
		value = hwmon_get_current_value(sensor);
		switch (value) {
		case HWMON_ERROR:
		case HWMON_ERROR_UNKNOWN_SENSOR:
			ERR("failed to read sensor %s\n", sensor);
			goto error;
			break;

		case HWMON_ERROR_NOT_SUPPORTED_FOR_SENSOR:
			if (event == SENSOR_EVENT_MIN) {
				nl = sensor_get_notification_level(s,
								s->current_level + 1);
			} else {
				nl = sensor_get_notification_level(s,
								s->current_level - 1);
			}
			break;

		default:
			nl = determine_current_notification_level(s, value);
			break;
		}

		if (nl == NULL) {
			ERR("unable to determine new sensor level\n");
			goto error;
		}

		INF("Alarm set: sensor %s, current level %d, new level %d: min %d, max %d\n",
			sensor, s->current_level, nl->level, nl->min_alarm_value, nl->max_alarm_value);
		if (hwmon_set_sensor_alarms(sensor, nl->min_alarm_value,
					    nl->max_alarm_value) != HWMON_OK) {
			ERR("alarm for %s failed to set.\n", sensor);
			goto error;
		}
		break;

	case SENSOR_EVENT_CRITICAL:
		INF("critical event detected!\n");

		nl = sensor_get_notification_level(s, SENSOR_CRITICAL_NOTIFICATION_LEVEL);
		if (nl == NULL) {
			ERR("unable to determine sensor level\n");
			goto error;
		}
		break;

	default:
		ERR("unknown event occurred!\n");
		goto error;
	}

	/* only raise an event (and run actions) if this is an
	 * actual level change (ie new level != old level) or this
	 * is from a critical level update
	 */
	if (nl->level == SENSOR_CRITICAL_NOTIFICATION_LEVEL ||
	    s->current_level != nl->level) {
		raise_socket_event(s, nl, event);

		/* run the appropriate actions for this level change */
		actions = nl->actions;
		while (actions != NULL) {
			actions_execute(actions->action);
			actions = actions->next;
		}

		/* now update the current level */
		if (nl->level != SENSOR_CRITICAL_NOTIFICATION_LEVEL) {
			s->current_level = nl->level;
		}
	}

error:
	pthread_mutex_unlock(&sensor_lock);
	wakeunlock();
}


/*
 * Creates a socket event that can be passed out to the thermal clients
 */
static void raise_socket_event(sensor_config_t *sensor,
			sensor_notification_level_t *level, sensor_event_t event) {
	thermal_notification_event_msg_t *msg;
	sensor_notification_action_t *a;
	char *end;
	int len;

	len = sizeof(thermal_notification_event_msg_t);
	len += strlen(sensor->sensor_name) + 1; /* for \0 */

	msg = (thermal_notification_event_msg_t *) malloc(sizeof(char) * len);
	if (msg == NULL) {
		ERR("unable to allocate memory\n");
		return;
	}

	msg->msg.id = THERMAL_NOTIFICATION_EVENT;
	msg->msg.len = len;

	msg->sensor = (((char *) msg) + sizeof(thermal_notification_event_msg_t));
	strncpy(msg->sensor, sensor->sensor_name, strlen(sensor->sensor_name) + 1);

	if (level->will_shutdown) {
		msg->shutdown_timeout = config_getshutdowntimeout();
	} else {
		msg->shutdown_timeout = 0;
	}

	msg->current_level = level->level;
	msg->previous_level = sensor->current_level;
	msg->actions_taken_count = 0;

	a = level->actions;
	while (a != NULL) {
		thermal_notification_event_msg_t *tmp;

		/* allocate space */
		len = strlen(a->action) + 1; // for \0
		tmp = realloc(msg, msg->msg.len + len);

		if (tmp == NULL) {
			ERR("unable to allocate memory\n");
			len = 0;
			if (msg != NULL) {
				free(msg);
			}
			goto fail;
		}

		msg = tmp;

		/* copy string */
		end = (((char *) msg) + msg->msg.len);
		strncpy(end, a->action, len);
		msg->actions_taken_count++;

		msg->msg.len += len;
		a = a->next;
	}

	socket_event(msg);
	return;
fail:
	ERR("failed to raise socket event!!!\n");
}

/*
 * Fetches sensor struct for the given name
 */
static sensor_config_t *sensor_get(const char *name) {
	sensor_config_t *nxt;

	nxt = sensor_config;
	while (nxt != NULL) {
		if (strcmp(nxt->sensor_name, name) == 0) {
			return nxt;
		}
		nxt = nxt->next;
	}

	return NULL;
}

/*
 * Fetches a sensors notification level
 */
static sensor_notification_level_t
*sensor_get_notification_level(sensor_config_t *sensor, int level) {
	sensor_notification_level_t *nxt;

	if (sensor == NULL) {
		return NULL;
	}

	if (level != SENSOR_CRITICAL_NOTIFICATION_LEVEL &&
		sensor->notification_level_count == 0) {
		return NULL;
	}

	nxt = sensor->levels;
	while (nxt != NULL) {
		if (nxt->level == level) {
			return nxt;
		}

		nxt = nxt->next;
	}

	return NULL;
}

/*
 * Determines notification level based on value, returns NULL if it's
 * before any alarm triggers.
 */
static sensor_notification_level_t
*determine_current_notification_level(sensor_config_t *s, int value) {
	sensor_notification_level_t *nxt;

	nxt = s->levels;

	while (nxt != NULL) {
		if (nxt->max_alarm_value == 0) {
			if (value >= nxt->min_alarm_value) {
				DBG("value=%d level=%d min=%d max=*\n", value,
					nxt->level, nxt->min_alarm_value);
				return nxt;
			}
		} else {
			if (value >= nxt->min_alarm_value &&
				value < nxt->max_alarm_value) {
				DBG("value=%d level=%d min=%d max=%d\n", value,
					nxt->level, nxt->min_alarm_value,
					nxt->max_alarm_value);
				return nxt;
			}
		}
		nxt = nxt->next;
	}

	return NULL;
}

/*
 * Creates a thermald wakelock to prevent the platform from sleeping
 * during the processing of thermal events
 */
static inline void wakelock(void) {
	int fd;

	DBG("enabling thermal wakelock\n");

	fd = open(WAKELOCK_LOCKPATH, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open wakelock sysfs\n");
		return;
	}

	if (write(fd, WAKELOCK_KEY, strlen(WAKELOCK_KEY) + 1) == -1) {
		ERR("failed to enable wakelock\n");
	}

	close(fd);
}

/*
 * Removes the thermald wakelock so normal power management can return
 */
static inline void wakeunlock(void) {
	int fd;

	DBG("disabling thermal wakelock\n");

	fd = open(WAKELOCK_UNLOCKPATH, O_WRONLY);
	if (fd < 0) {
		ERR("unable to open wakelock sysfs\n");
		return;
	}

	if (write(fd, WAKELOCK_KEY, strlen(WAKELOCK_KEY) + 1) == -1) {
		ERR("failed to disable wakelock\n");
	}

	close(fd);
}
