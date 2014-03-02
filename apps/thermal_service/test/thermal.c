/*
 * Implements Thermal test interface
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <thermal_manager.h>

#include "test.h"

static pthread_mutex_t event_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t event_recvd = PTHREAD_COND_INITIALIZER;

static void THERMAL_notificationcb(STE_TM_notification_t *notification) {
	int i;

	pthread_mutex_lock(&event_lock);

	printf("Thermal Event on %s occurred (%d->%d)\n",
		notification->sensor, notification->previous_level,
		notification->current_level);

	for (i = 0; i < notification->number_of_actions_taken; i++) {
		printf("+ action: %s\n", notification->actions_taken[i]);
	}

	pthread_cond_signal(&event_recvd);
	pthread_mutex_unlock(&event_lock);
}

static void THERMAL_errorcb(STE_TM_error_t error) {
	fprintf(stderr, "ERROR: ");
	switch (error) {
	case TM_OK:
		/* ignore! */
		return;

	case TM_NOT_INITALISED:
		fprintf(stderr, "Connection to thermal manager is not up.\n");
		return;

	case TM_UNABLE_TO_CONNECT_TO_THERMAL_MANAGER:
		fprintf(stderr,
			"Unable to connect to thermal manager. Is it running?\n");
		break;

	case TM_INVALID_DATA_RECEIVED:
		fprintf(stderr,
			"ThermalManger sent us bad data!\n");
		break;

	case TM_CONNECTION_DROPPED:
		fprintf(stderr,
			"Connection to ThermalManager died unexpectedly.\n");
		break;

	case TM_UNABLE_TO_ALLOCATE_MEMORY:
		fprintf(stderr,
			"Failed to allocated memory!\n");
		break;

	default:
		fprintf(stderr, "Unknown error! %d\n", error);
		break;
	}

	exit(-1);
}

int THERMAL_init(int argc, char **argv) {
	STE_TM_callbacks_t cb = {
		.notification_cb = THERMAL_notificationcb,
		.error_cb = THERMAL_errorcb
	};

	STE_TM_init(&cb);

	return TEST_OK;
}

int THERMAL_deinit(int argc, char **argv){
	STE_TM_deinit();

	return TEST_OK;
}

int THERMAL_getactions(int argc, char **argv) {
	int count, i;
	char **actions;

	count = STE_TM_get_actions(&actions);
	if (count == -1) {
		fprintf(stderr, "failed to retrieve actions\n");
		return TEST_FAILED;
	}

	printf("%d actions available in system\n", count);
	for (i = 0; i < count; i++) {
		printf("+ %s\n", actions[i]);
	}

	STE_TM_free_actions(count, &actions);

	return TEST_OK;
}
int THERMAL_getsensors(int argc, char **argv) {
	char **sensors;
	int count, i;

	count = STE_TM_get_sensors(&sensors);
	if (count == -1) {
		fprintf(stderr, "failed to retrieve sensors\n");
		return TEST_FAILED;
	}

	printf("%d sensors available in the system\n", count);
	for (i = 0; i < count; i++) {
		printf("+ %s\n", sensors[i]);
	}
	STE_TM_free_sensors(count, &sensors);

	return TEST_OK;
}

int THERMAL_getsensorconfig(int argc, char **argv) {
	STE_TM_sensor_config_t *sensor_config;
	STE_TM_notification_level_t *level;
	int i, j, result;

	if (argc != 3) {
		fprintf(stderr, "getsensorconfig <sensorname>\n");
		return TEST_FAILED;
	}

	result = STE_TM_get_sensor_config(argv[1], &sensor_config);

	printf("sensor=%s current_level=%d available_levels=%d\n",
		sensor_config->name, sensor_config->current_notification_level,
		sensor_config->no_notification_levels);

	for (i = 0; i < sensor_config->no_notification_levels; i++) {
		level = sensor_config->notification_levels[i];
		printf("+ level=%d min=%d max=%d actions=%d\n",
			level->notification_level,
			level->min_alarm_level, level->max_alarm_level,
			level->number_of_actions);

		for (j = 0; j < level->number_of_actions; j++) {
			printf("++ mitigation action: %s\n", level->actions[j]);
		}
	}

	STE_TM_free_sensor_config(&sensor_config);

	return TEST_OK;
}

int THERMAL_waitfornotification(int argc, char **argv) {
	pthread_mutex_lock(&event_lock);
	pthread_cond_wait(&event_recvd, &event_lock);
	pthread_mutex_unlock(&event_lock);

	return TEST_OK;
}

// register all test tables
TEST_TABLE_BEGIN(THERMAL)
TEST_TABLE_CMD("init", "Initialises connection to thermal manager", THERMAL_init)
TEST_TABLE_CMD("deinit", "Closes down connection to thermal manager", THERMAL_deinit)
TEST_TABLE_CMD("getactions", "Gets all actions in the system", THERMAL_getactions)
TEST_TABLE_CMD("getsensors", "Gets all sensors in the system", THERMAL_getsensors)
TEST_TABLE_CMD("getsensorconfig", "Get sensor configuration", THERMAL_getsensorconfig)
TEST_TABLE_CMD("waitfornotification", "Blocks until a new notification arrives", THERMAL_waitfornotification)
TEST_TABLE_END
