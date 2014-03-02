/*
 * Thermal Service Manager
 *
 * HWMON abstraction
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <poll.h>
#include <signal.h>

#include "sensors.h"
#include "hwmon.h"
#include "log.h"

/* base path to the HWMON sysfs interface */
#define HWMON_ROOT "/sys/class/hwmon/"

/* defines total number of chips HWMON_ROOT/hwmon[n] to
 * attempt to find */
#define HWMON_MAX_CHIPS (4)

/* defines total number of chip sensors to search for */
#define HWMON_MAX_SENSORS (10)

/* maximum length of a sensors label */
#define HWMON_MAX_SENSOR_LABEL_LEN (32)

/* maximum string size for the an attribute node (ie max string length
 * from hwmon_sensor_attributes below */
#define HWMON_MAX_SENSOR_ATTRS_LEN (14)

/* maximum length of the string to be read from sysfs nodes */
#define HWMON_MAX_VALUE_LEN (32)

/* how long to wait before assuming the HWMON kernel subsystem
 * has had time to process and recalibrate it's sensor alarms */
#define HWMON_SYSFS_UPDATE_TIMEOUT (5)

/* size of buffer to be used to read values from the HWMON alarm nodes. */
#define POLL_BUF_LEN (16)

#define SENSOR_HAS_ATTRIBUTE(x, a) x->attr[a]

/* hwmon_sensor_attr_t & hwmon_sensor_attributes are linked and must be kept in
 * sync. The enum is used as the index for hwmon_sensor_attributes. */
typedef enum {
	HWMON_ATTR_INPUT,
	HWMON_ATTR_MAX,
	HWMON_ATTR_MIN,
	HWMON_ATTR_CRIT,
	HWMON_ATTR_MAX_ALARM,
	HWMON_ATTR_MIN_ALARM,
	HWMON_ATTR_CRIT_ALARM,
	HWMON_ATTR_COUNT
} hwmon_sensor_attr_t;

static char hwmon_sensor_attributes[HWMON_ATTR_COUNT][HWMON_MAX_SENSOR_ATTRS_LEN] = {
	"_input", "_max", "_min", "_crit", "_max_alarm",
	"_min_alarm", "_crit_alarm"
};

/* represents a sensor within the ThermalManager */
typedef struct hwmon_sensor_s {
	char *base_path; /* HWMONROOT/hwmon[ii]/device/temp[ii] */
	char *name; /* contents of the _label attribute */
	int attr[HWMON_ATTR_COUNT];
	struct hwmon_sensor_s *next;
} hwmon_sensor_t;

/* holds all hwmon alarm events to listen to */
typedef struct hwmon_sensor_eventlist_s {
	int fd;
	char *path;
	hwmon_sensor_attr_t attr;
	hwmon_sensor_t *sensor;
	struct hwmon_sensor_eventlist_s *next;
} hwmon_sensor_eventlist_t;

static hwmon_sensor_t *hwmon_sensors = (NULL);
static hwmon_sensor_eventlist_t *hwmon_eventlist = (NULL);
static int hwmon_event_count;
static pthread_t hwmon_event_pid;
static pthread_mutex_t hwmon_event_lock;

static int find_sensors(const char *path);
static int find_sensors_attributes(const char *name, const char *basepath, const int sensor_num);
static inline void remove_newline(char *str);
static hwmon_sensor_t *get_hwmon_sensor(const char *sensor);
static inline int attr_path(hwmon_sensor_t *s, hwmon_sensor_attr_t a, char **ret);
static int read_attr(hwmon_sensor_t *sensor, hwmon_sensor_attr_t attr, int *value);
static int write_attr(hwmon_sensor_t *sensor, hwmon_sensor_attr_t attr, int value);
static void *hwmon_event_thread(void *arg);
static void hwmon_event_cancel(hwmon_sensor_t *s, hwmon_sensor_attr_t attr);
static void hwmon_event_wait(hwmon_sensor_t *s, hwmon_sensor_attr_t attr);
static void hwmon_event_sighandler(int signo);
static inline sensor_event_t attr_to_sensor_event(hwmon_sensor_attr_t attr);

/*
 * Inits hwmon subsystem
 */
void hwmon_init(void) {
	hwmon_event_count = 0;
	pthread_mutex_init(&hwmon_event_lock, NULL);
	pthread_create(&hwmon_event_pid, NULL, hwmon_event_thread, NULL);
}

/*
 * searches the sysfs nodes for the various hwmon chips
 */
int hwmon_find_sensors(void) {
	struct stat s;
	char *path;
	int pathlen;
	int i;

	/* allocate memory for string "HWMON_ROOT/hwmon[ii]\0" */
	pathlen = strlen(HWMON_ROOT);
	pathlen += strlen("hwmon");
	pathlen += 3; /* 2 digits and a \0 terminator */

	path = (char *) malloc(sizeof(char) * pathlen);
	if (path == NULL) {
		ERR("unable to allocate memory\n");
		return HWMON_ERROR;
	}

	/* check HWMON_ROOT/hwmon[ii] to see if it exists,
	 * if so we have a new bank of sensors we need to
	 * discover. If it fails there will be not be any
	 * i+1 nodes.
	 */
	for (i = 0; i < HWMON_MAX_CHIPS; i++) {
		snprintf(path, pathlen, "%shwmon%d", HWMON_ROOT, i);

		if (stat(path, &s) < 0) {
			goto allfound;
		}

		DBG("searching %s for sensors\n", path);

		if (find_sensors(path) < 0) {
			free(path);
			return HWMON_ERROR;
		}
	}

allfound:
	DBG("all HWMON sensors found\n");

	free(path);
	return HWMON_OK;
}

/*
 * Reads the current input value (if it exists) for the sensor
 * and returns to the caller
 */
int hwmon_get_current_value(const char *sensor) {
	hwmon_sensor_t *s;
	int value;
	int ret;

	s = get_hwmon_sensor(sensor);
	if (s == NULL) {
		ERR("unable to find hwmon sensor %s!\n", sensor);
		return HWMON_ERROR_UNKNOWN_SENSOR;
	}

	ret = read_attr(s, HWMON_ATTR_INPUT, &value);
	if (ret != HWMON_OK) {
		return ret;
	}

	return value;
}

/*
 * Sets a sensors alarm values & instructs the monitor thread to wait for
 * events on those fd's
 */
int hwmon_set_sensor_alarms(const char *sensor, int min_value, int max_value) {
	hwmon_sensor_t *s;

	DBG("setting alarm for %s min=%d max=%d\n", sensor, min_value, max_value);

	s = get_hwmon_sensor(sensor);
	if (s == NULL) {
		ERR("unable to find hwmon sensor %s\n", sensor);
		return HWMON_ERROR_UNKNOWN_SENSOR;
	}

	/*
	 * Simplest case using the _max & _min attributes to set a range
	 * that let's us move between different states
	 */
	if (SENSOR_HAS_ATTRIBUTE(s, HWMON_ATTR_MAX) &&
		SENSOR_HAS_ATTRIBUTE(s, HWMON_ATTR_MAX_ALARM) &&
		SENSOR_HAS_ATTRIBUTE(s, HWMON_ATTR_MIN) &&
		SENSOR_HAS_ATTRIBUTE(s, HWMON_ATTR_MIN_ALARM)) {
		/* cancel any ongoing alarm timers */
		hwmon_event_cancel(s, HWMON_ATTR_MAX_ALARM);
		hwmon_event_cancel(s, HWMON_ATTR_MIN_ALARM);

		/* set new alarm levels */
		write_attr(s, HWMON_ATTR_MAX, max_value);
		write_attr(s, HWMON_ATTR_MIN, min_value);

		/* we have to sleep here, to yield for enough time for the HWMON
		 * kernel subsystem to detect that a new value has been written
		 * and to recalibrate the sensor alarms. if we don't we get an
		 * endless stream of alarms for a few seconds before the alarm
		 * thresholds are detected.
		 *
		 * Right now this value has come from a bit of trial and error, it
		 * will probably need to be tweaked further at some point.
		 */
		sleep(HWMON_SYSFS_UPDATE_TIMEOUT);

		/* start waiting for new event */
		hwmon_event_wait(s, HWMON_ATTR_MAX_ALARM);
		hwmon_event_wait(s, HWMON_ATTR_MIN_ALARM);

		return HWMON_OK;
	}

	ERR("don't know how to set alarm for %s\n", sensor);
	return HWMON_ERROR;
}

int hwmon_critical_alarm(const char *sensor) {
	hwmon_sensor_t *s;

	DBG("monitoring critical alarm for %s\n", sensor);

	s = get_hwmon_sensor(sensor);
	if (s == NULL) {
		ERR("unable to find hwmon sensor %s\n", sensor);
		return HWMON_ERROR_UNKNOWN_SENSOR;
	}

	if (!SENSOR_HAS_ATTRIBUTE(s, HWMON_ATTR_CRIT_ALARM)) {
		return HWMON_ERROR;
	}

	hwmon_event_cancel(s, HWMON_ATTR_CRIT_ALARM);
	hwmon_event_wait(s, HWMON_ATTR_CRIT_ALARM);

	return HWMON_OK;
}

/*
 * called by hwmon_find_sensors to search within a hwmon chip
 * sysfs node for the various temp sensors
 */
static int find_sensors(const char *path) {
	char *sensor;
	int sensorlen;
	int i;

	/* allocate memory for "path/device/temp[i]_label\0" */
	sensorlen = strlen(path);
	sensorlen += strlen("/device/temp_label");
	sensorlen += 3; /* 2 digits and a \0 terminator */

	sensor = (char *) malloc(sizeof(char) * sensorlen);
	if (sensor == NULL) {
		ERR("unable to allocate memory\n");
		return HWMON_ERROR;
	}

	/* as all sensors will have a temp[i]_label sysfs
	 * file we will try and stat it and then attempt to
	 * discover it's other attributes
	 */
	for (i = 1; i < HWMON_MAX_SENSORS; i++) {
		char *label;
		FILE *file;

		snprintf(sensor, sensorlen, "%s/device/temp%i_label", path, i);

		/* try to open & read the sensor label */
		file = fopen(sensor, "r");
		if (file == NULL) {
			goto allfound;
		}

		label = (char *) malloc(sizeof(char) * HWMON_MAX_SENSOR_LABEL_LEN);
		if (label == NULL) {
			ERR("unable to allocate memory\n");
			fclose(file);
			free(sensor);
			return HWMON_ERROR;
		}

		if (fgets(label, HWMON_MAX_SENSOR_LABEL_LEN, file) == NULL) {
			ERR("unable to read sensor label\n");
			fclose(file);
			continue;
		}

		/* yay! we've found a sensor, let's discover it's attributes */
		remove_newline(label);
		fclose(file);
		if (find_sensors_attributes(label, path, i) < 0) {
			free(sensor);
			free(label);
			return HWMON_ERROR;
		}

		free(label);
	}

allfound:
	free(sensor);

	return HWMON_OK;
}

/*
 * called from find_sensors() once we've determined a sensor actual exists.
 * this function does the majority of the work of building up the hwmon
 * sensor struct and determining the various alarms etc that are exposed
 */
static int find_sensors_attributes(const char *name, const char *basepath,
				   const int sensor_num) {
	hwmon_sensor_t *hwmon_sensor;
	char *test;
	int testlen;
	int i;

	INF("found sensor \"%s\"\n", name);

	/* allocate hwmon struct and setup */
	hwmon_sensor = (hwmon_sensor_t *) malloc(sizeof(hwmon_sensor_t));
	if (hwmon_sensor == NULL) {
		ERR("unable to allocate memory\n");
		return HWMON_ERROR;
	}

	hwmon_sensor->name = strdup(name);
	if (hwmon_sensor->name == NULL) {
		ERR("unable to copy string\n");
		free(hwmon_sensor);
		return HWMON_ERROR;
	}

	hwmon_sensor->next = NULL;

	/* allocate space for attribute:
	 *    HWMON_ROOT/hwmon[ii]/device/temp[ii]<_attr>\0 */
	testlen = strlen(basepath);
	testlen += strlen("/device/temp");
	testlen += 3; /* 2 digits & \0 */
	testlen += HWMON_MAX_SENSOR_ATTRS_LEN;

	test = (char *) malloc(sizeof(char) * testlen);
	if (test == NULL) {
		ERR("unable to allocate memory\n");
		free(hwmon_sensor->name);
		free(hwmon_sensor);
		return HWMON_ERROR;
	}

	/* work out which of the various HWMON attributes the
	 * sensor has
	 */
	for (i = 0; i < HWMON_ATTR_COUNT; i++) {
		struct stat s;

		snprintf(test, testlen, "%s/device/temp%d%s", basepath,
			sensor_num, hwmon_sensor_attributes[i]);

		if (stat(test, &s) < 0) {
			hwmon_sensor->attr[i] = 0; /* attribute doesn't exist */
		} else {
			DBG("%s has %s\n", name, hwmon_sensor_attributes[i]);
			hwmon_sensor->attr[i] = 1; /* attribute does exist */
		}
	}

	/* setup the base_path attribute (it's done here to make
	 * use of the space already allocated for the test path)
	 */
	snprintf(test, testlen, "%s/device/temp%d", basepath, sensor_num);
	hwmon_sensor->base_path = strdup(test);
	if (hwmon_sensor->base_path == NULL) {
		ERR("unable to copy string\n");
		free(hwmon_sensor->name);
		free(hwmon_sensor);
		free(test);
		return HWMON_ERROR;
	}

	/* finally add the sensor to our list */
	if (hwmon_sensors == NULL) {
		hwmon_sensors = hwmon_sensor;
	} else {
		hwmon_sensor_t *nxt = hwmon_sensors;
		while (nxt->next != NULL) {
			nxt = nxt->next;
		}
		nxt->next = hwmon_sensor;
	}

	free(test);

	return HWMON_OK;
}

/*
 * searches for and removes the first newline at the end of
 * the passed in string
 */
static inline void remove_newline(char *str) {
	int i;

	if (str == NULL) {
		return;
	}

	i = strlen(str);

	for ( ; i > 0; i--) {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
	}
}

/*
 * Fetches the appropriate sensor struct
 */
static hwmon_sensor_t *get_hwmon_sensor(const char *sensor) {
	hwmon_sensor_t *nxt;

	nxt = hwmon_sensors;
	while (nxt != NULL) {
		if (strcmp(nxt->name, sensor) == 0) {
			return nxt;
		}
		nxt = nxt->next;
	}
	return NULL;
}

/*
 * Allocates memory and builds path to the SYFS node for a sensor
 * attribute. Caller must free() allocated memory.
 */
static inline int attr_path(hwmon_sensor_t *s, hwmon_sensor_attr_t a,
			    char **ret) {
	char *path;
	int path_len;

	if (s == NULL || s->base_path == NULL) {
		*ret = NULL;
		return HWMON_ERROR;
	}

	path_len = strlen(s->base_path);
	path_len += strlen(hwmon_sensor_attributes[a]);
	path_len += 1; /* for \0 */

	path = (char *) malloc(sizeof(char) * path_len);
	if (path == NULL) {
		ERR("unable to allocate memory\n");
		*ret = NULL;
		return HWMON_ERROR;
	}

	snprintf(path, path_len, "%s%s", s->base_path,
		hwmon_sensor_attributes[a]);

	*ret = path;

	return HWMON_OK;
}

/*
 * Reads a value from an attribute for a given sensor
 */
static int read_attr(hwmon_sensor_t *sensor, hwmon_sensor_attr_t attr,
		int *value) {
	char *sensor_value;
	char *path;
	int fd;

	/* sanity checks */
	if (sensor == NULL) {
		return HWMON_ERROR;
	}

	if (!sensor->attr[attr]) {
		return HWMON_ERROR_NOT_SUPPORTED_FOR_SENSOR;
	}

	if (attr_path(sensor, attr, &path) != HWMON_OK) {
		return HWMON_ERROR;
	}

	sensor_value = (char *) malloc(sizeof(char) * HWMON_MAX_VALUE_LEN);
	if (sensor_value == NULL) {
		ERR("unable to allocate memory\n");
		return HWMON_ERROR;
	}

	/* open the fd & read out the data */
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		ERR("failed to open sensor input\n");
		free(sensor_value);
		free(path);
		return HWMON_ERROR;
	}

	if (read(fd, sensor_value, HWMON_MAX_VALUE_LEN) == -1) {
		ERR("failed to read attribute");
		free(sensor_value);
		free(path);
		close(fd);
		return HWMON_ERROR;
	}

	free(path);
	close(fd);

	*value = strtol(sensor_value, NULL, 10);

	free(sensor_value);

	return HWMON_OK;
}

/*
 * Sets attribute for a sensor
 */
static int write_attr(hwmon_sensor_t *sensor, hwmon_sensor_attr_t attr, int value) {
	char *sensor_value;
	char *path;
	int fd;

	/* sanity checks */
	if (sensor == NULL) {
		return HWMON_ERROR;
	}

	if (!sensor->attr[attr]) {
		return HWMON_ERROR_NOT_SUPPORTED_FOR_SENSOR;
	}

	if (attr_path(sensor, attr, &path) != HWMON_OK) {
		return HWMON_ERROR;
	}

	sensor_value = (char *) malloc(sizeof(char) * HWMON_MAX_VALUE_LEN);
	if (sensor_value == NULL) {
		ERR("unable to allocate memory\n");
		return HWMON_ERROR;
	}

	/* open & write */
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		ERR("failed to open sensor input");
		free(path);
		free(sensor_value);
		return HWMON_ERROR;
	}

	snprintf(sensor_value, HWMON_MAX_VALUE_LEN, "%d\n", value);
	if (write(fd, sensor_value, strnlen(sensor_value, HWMON_MAX_VALUE_LEN)) == -1) {
		ERR("failed to write attribute");
		free(path);
		free(sensor_value);
		close(fd);
		return HWMON_ERROR;
	}

	/* we need to force the HWMON driver to actually parse and reset
	 * the alarm
	 */
	fsync(fd);

	free(path);
	free(sensor_value);
	close(fd);

	return HWMON_OK;
}

/*
 * A dummy function that does nothing but allow the hwmon poll() event
 * thread to receive SIGUSR1 without exiting. SIGUSR1 is used to interrupt
 * the ongoing poll() so the fd set can be rebuilt
 */
static void hwmon_event_sighandler(int signo) {
	/* do nothing */
}

/*
 * Receives events on the different sensors
 */
static void *hwmon_event_thread(void *arg) {
	struct sigaction act;

	/* setup signal handler */
	act.sa_handler = hwmon_event_sighandler;
	act.sa_flags = 0;
	sigaction(SIGUSR1, &act, NULL);

	while (1) {
		hwmon_sensor_eventlist_t *event;
		int event_occured; /* flag set when an event occurs */
		int nfds; /* number of fds to monitor in poll() */
		int err;
		struct pollfd *fds = NULL;
		struct pollfd *currentpfd;

		pthread_mutex_lock(&hwmon_event_lock);

		DBG("rebuilding poll fds...\n");

		if (fds != NULL) {
			free(fds);
			fds = NULL;
		}

		/* allocate memory for fds */
		fds = (struct pollfd *) malloc(sizeof(struct pollfd) * hwmon_event_count);
		if (fds == NULL) {
			ERR("unable to allocate memory... exiting.");
			pthread_mutex_unlock(&hwmon_event_lock);
			return NULL;
		}

		nfds = 0;
		currentpfd = fds;
		event_occured = 0;

		/* loop through all the events */
		event = hwmon_eventlist;

		while (event != NULL) {
			char buf[POLL_BUF_LEN];

			/* if the fd is open, close it now */
			if (event->fd != -1) {
				close(event->fd);
				event->fd = -1;
			}

			/* open the alarm path */
			event->fd = open(event->path, O_RDONLY);
			if (event->fd == -1) {
				ERR("unable to open alarm %s", event->path);
				pthread_mutex_unlock(&hwmon_event_lock);
				return NULL;
			}

			/* check to see if an alarm has occurred, if so flag it and jump
			 * out and service it.
			 */
			if (read(event->fd, &buf, POLL_BUF_LEN) > 0) {
				/* All STE HWMON sensors return a 1 if an alarm is set,
				 * Hopefully all the others do to. */
				if (buf[0] == '1') {
					int val;

					val = hwmon_get_current_value(event->sensor->name);
					switch(val) {
					case HWMON_ERROR:
					case HWMON_ERROR_UNKNOWN_SENSOR:
						ERR("event occured on unknown sensor?");
						break;

					case HWMON_ERROR_NOT_SUPPORTED_FOR_SENSOR:
						DBG("alarm on %s\n", event->sensor->name);
						break;

					default:
						DBG("alarm on %s reporting %d\n", event->sensor->name,
							hwmon_get_current_value(event->sensor->name));
						break;
					}
					event_occured = 1;
					goto event_occured;
				}
			}

			/* build pollfd struct */
			currentpfd->fd = event->fd;
			currentpfd->events = POLLERR | POLLPRI;
			currentpfd->revents = 0;

			nfds++;

			/* move on to next item */
			currentpfd++;
			event = event->next;
		}
	event_occured:
		pthread_mutex_unlock(&hwmon_event_lock);

		/* if an event occurred inform the sensor subsystem and go back to the top
		 * to check if any other events have occurred. We need todo this because:
		 * a) We need to use the same mutex to reconfigure the event regions
		 *    from the sensor subsystem otherwise we will get a hang.
		 * b) There is probably a range change so we need to go back and rebuild
		 *    the fd set.
		 */
		if (event_occured) {
			sensor_event(event->sensor->name,
				attr_to_sensor_event(event->attr));
			event_occured = 0;
			continue;
		}

		/* now do actual wait */
		DBG("waiting for alarm...\n");
		err = poll(fds, nfds, -1);
		if (err > 0) {
			DBG("detected new sensor event!\n");
		} else if (err == -1) {
			if (errno != EINTR) {
				ERR("failed to monitor sensor alarms");
			}
		}
	}

	return NULL;
}

/*
 * Stops waiting for a sensor alarm
 */
static void hwmon_event_cancel(hwmon_sensor_t *s, hwmon_sensor_attr_t attr) {
	hwmon_sensor_eventlist_t *event = NULL;

	/* remove event monitoring from main list */
	pthread_mutex_lock(&hwmon_event_lock);

	if (hwmon_eventlist == NULL) {
		goto found;
	}

	if (hwmon_eventlist->sensor == s && hwmon_eventlist->attr == attr) {
		event = hwmon_eventlist;
		hwmon_eventlist = hwmon_eventlist->next;
		hwmon_event_count--;
	} else {
		hwmon_sensor_eventlist_t *nxt;
		hwmon_sensor_eventlist_t *previous;

		previous = hwmon_eventlist;
		nxt = hwmon_eventlist->next;

		while (nxt != NULL) {
			if (nxt->sensor == s && nxt->attr == attr) {
				event = nxt;
				previous->next = nxt->next;
				hwmon_event_count--;
				goto found;
			}

			previous = nxt;
			nxt = nxt->next;
		}
	}

found:
	pthread_mutex_unlock(&hwmon_event_lock);

	if (event == NULL) {
		return;
	}

	/* instruct the hwmon event list to restart */
	pthread_kill(hwmon_event_pid, SIGUSR1);

	/* cleanup the event struct */
	if (event->fd != -1) {
		close(event->fd);
	}

	if (event->path != NULL) {
		free(event->path);
	}

	free(event);
}

/*
 * Waits for a sensor alarm
 */
static void hwmon_event_wait(hwmon_sensor_t *s, hwmon_sensor_attr_t attr) {
	hwmon_sensor_eventlist_t *event;

	/* check the sensor has the required attribute */
	if (!SENSOR_HAS_ATTRIBUTE(s, attr)) {
		ERR("not supported by sensor\n");
		return;
	}

	/* allocate our struct to store event details for the event thread */
	event = (hwmon_sensor_eventlist_t *) malloc(sizeof(hwmon_sensor_eventlist_t));
	if (event == NULL) {
		ERR("unable to allocate memory\n");
		return;
	}

	/* populate the struct */
	event->fd = -1;
	event->sensor = s;
	event->attr = attr;
	event->next = NULL;

	if (attr_path(event->sensor, event->attr, &(event->path)) != HWMON_OK) {
		ERR("failed to find sensor attribute path\n");
		free(event);
		return;
	}

	/* stick it at the end of the list */
	pthread_mutex_lock(&hwmon_event_lock);
	if (hwmon_eventlist == NULL) {
		hwmon_eventlist = event;
	} else {
		hwmon_sensor_eventlist_t *nxt = hwmon_eventlist;

		while (nxt->next != NULL) {
			nxt = nxt->next;
		}

		nxt->next = event;
	}

	hwmon_event_count++;
	pthread_mutex_unlock(&hwmon_event_lock);

	/* signal event thread to reconfigure itself */
	pthread_kill(hwmon_event_pid, SIGUSR1);
}

static inline sensor_event_t attr_to_sensor_event(hwmon_sensor_attr_t attr) {
	switch (attr) {
	case HWMON_ATTR_MIN_ALARM:
		return SENSOR_EVENT_MIN;

	case HWMON_ATTR_MAX_ALARM:
		return SENSOR_EVENT_MAX;

	case HWMON_ATTR_CRIT_ALARM:
		return SENSOR_EVENT_CRITICAL;

	default:
		return SENSOR_EVENT_UNKNOWN;
	}
}
