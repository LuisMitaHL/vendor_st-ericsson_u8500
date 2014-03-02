/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "sensors.h"
#include "log.h"

#define DEFAULT_PLATFORM_HARD_SHUTDOWN_TIMEOUT_SECS (15)
#define CONFIG_PLATFORM_SHUTDOWN_VARIABLE "PLATFORMSHUTDOWN"
#define CONFIG_CRITICAL_LEVEL "CRITICAL"

static int parse_config_line(const char *line);

static int platform_shutdown = DEFAULT_PLATFORM_HARD_SHUTDOWN_TIMEOUT_SECS;

/*
 * parse the config file by opening and looping through each
 * line passing on the actual parse to 'parse_config_line'
 */
int parse_config(const char *configfile) {
	char *buf = NULL;
	size_t len;
	FILE *conf;
	int ret = 0;

	INF("parsing config file %s\n", configfile);

	if (configfile == NULL) {
		return -1;
	}

	/* open up the config file */
	conf = fopen(configfile, "r");
	if (conf == NULL) {
		ERR("unable to open config file\n");
		return -errno;
	}

	/* read 1 line at a time and parse! */
#ifdef ANDROID_BUILD
	while ((buf = fgetln(conf, &len)) != NULL) {
#else
	while (getline(&buf, &len, conf) > 0) {
#endif
		/* skip comment lines or empty lines*/
		if (*buf == '#' || *buf == '\n') {
			continue;
		}

		if (parse_config_line(buf) < 0) {
			ret = -1;
			goto end;
		}
	}
	end:
	fclose(conf);

	if (buf == NULL) {
		free(buf);
	}

	return ret;
}

/*
 * does the actual config line parsing. Config line syntax is described
 * in docs/configfileformat.txt
 */
static int parse_config_line(const char *line) {
	char *sensor, *p, *q, *curpos, *action;
	int notification_level, min_alarm_level, max_alarm_level;

	p = (char *) line;
	q = p; /* save start of word in q */

	if (*p == '"') {
		q++; p++; /* skip over first " */
		for (; (*p != '"' && *p != '\n'); p++); /* keep searching until we find the matching " */
	} else {
		for (; (*p != ' ' && *p != '\n'); p++); /* keep searching till we find the space */
	}

	/* get the sensor name */
	sensor = strndup(q, p-q);
	if (sensor == NULL) {
		ERR("unable to allocate memory\n");
		return -1;
	}

	/* skip over the last " if need be  & move to the next field*/
	if (*p == '"') {
		p++;
	}
	p++;

	/* check if this not a platform shutdown directive */
	if (strncmp(sensor, CONFIG_PLATFORM_SHUTDOWN_VARIABLE, strlen(CONFIG_PLATFORM_SHUTDOWN_VARIABLE)) == 0) {
		curpos = p;
		platform_shutdown = strtol(p, &p, 10);

		/* check strtol found a valid field */
		if (curpos == p) {
			ERR("platform shutdown config invalid\n");
			goto error;
		}

		INF("setting platform shutdown timeout to %d secs\n", platform_shutdown);

		free(sensor);
		return 0;
	}

	if (strncmp(p, CONFIG_CRITICAL_LEVEL, strlen(CONFIG_CRITICAL_LEVEL)) == 0) {
		notification_level = SENSOR_CRITICAL_NOTIFICATION_LEVEL;
		min_alarm_level = max_alarm_level = 0;
		p += strlen(CONFIG_CRITICAL_LEVEL) + 1;
	} else {
		/* read in notification level */
		curpos = p;
		notification_level = strtol(p, &p, 10);

		/* check strtol found a valid field */
		if (curpos == p)
			goto error;

		p++;

		/* read in min alarm level */
		curpos = p;
		min_alarm_level = strtol(p, &p, 10);

		/* check strtol found a valid field */
		if (curpos == p)
			goto error;

		p++;

		/* read in max alarm level */
		curpos = p;
		max_alarm_level = strtol(p, &p, 10);

		/* check strtol found a valid field */
		if (curpos == p)
			goto error;

		p++;
	}

	sensor_new(sensor);

	if (sensor_new_notification_level(sensor, notification_level,
						min_alarm_level,
						max_alarm_level) < 0) {
		ERR("failed to create notification level\n");
		free(sensor);
		return -1;
	}

	/* discover the actions for each level */
	do {
		/* search for next '\n' or ' ' char then copy the string */
		for (q = p; ((*p != '\n') && (*p != ' ')); p++);
		action = strndup(q, p - q);
		if (action == NULL) {
			ERR("Unable to allocate memory\n");
			free(sensor);
			return -1;
		}

		/* if it's a space char that caused us to stop increment to the
		 * next char so we don't get stuck in a loop */
		if (*p == ' ') {
			p++;
		}

		if (sensor_set_action_for_level(sensor, notification_level, action) < 0) {
			ERR("failed to add action to notification level\n");
			free(sensor);
			free(action);
			return -1;
		}

		free(action);
	} while ((*p != '\n'));

	free(sensor);

	return 0;

error:
	ERR("Invalid configuration for sensor %s\n", sensor);
	free(sensor);

	return -1;
}

int config_getshutdowntimeout(void) {
	return platform_shutdown;
}
