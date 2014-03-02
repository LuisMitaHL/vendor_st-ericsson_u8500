/*
 * libthermal
 *
 * ThermalManager Interface
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <err.h>
#include <stdio.h>
#include <semaphore.h>

#include <socketmsgs.h>

#include "thermal_manager.h"

/* call the client's error callback if it is available to us */
#define ERROR_CB(x) {\
	if (client_callbacks.error_cb != NULL) {\
		client_callbacks.error_cb(x);	\
	}\
	}
/* Where we fail but pass back the error via other means */
#define GENERIC_FAIL (-1)

/* Identify that we have failed when allocating config data */
#define FAILED_ALLOCATING_CONFIG_DATA (-2)

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t recv_lock = PTHREAD_MUTEX_INITIALIZER;
static int initalized = 0;
static pthread_t recv_thread_pid;
static thermal_msgid_t wait_msgid;
static void *wait_data;
static sem_t wait_sem;
static int recv_fd = -1;
static STE_TM_callbacks_t client_callbacks = {
	.error_cb = NULL,
	.notification_cb = NULL,
};

static void *recv_thread(void *args);
static void recv_thread_sighandler(int sig);
static void send_thermal_msg_request(thermal_socket_msg_t *req, thermal_msgid_t id, void **resp);
static void handle_thermal_event(thermal_notification_event_msg_t *msg);

int STE_TM_init(STE_TM_callbacks_t *callbacks) {
	struct sockaddr_un remote;
	int ret = 0;
	int len;

	pthread_mutex_lock(&lock);

	if (initalized || callbacks == NULL) {
		ret = GENERIC_FAIL;
		goto end;
	}

	client_callbacks = *callbacks;

	/* open socket */
	if((recv_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		warn("Unable to create new socket");
		ret = errno;
		goto end;
	}

	remote.sun_family = AF_UNIX;
	strncpy(remote.sun_path, THERMAL_MANAGER_DEFAULT_SOCKET, strlen(THERMAL_MANAGER_DEFAULT_SOCKET) + 1);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family) + 1;

	if (connect(recv_fd, (struct sockaddr *) &remote, len) == -1) {
		warn("Unable to create unix socket");
		ERROR_CB(TM_UNABLE_TO_CONNECT_TO_THERMAL_MANAGER);
		ret = errno;
		goto end;
	}

	/* create recv thread */
	if (pthread_create(&recv_thread_pid, NULL, recv_thread, NULL)) {
		warn("Unable to create recv thread");
		ERROR_CB(TM_UNABLE_TO_CONNECT_TO_THERMAL_MANAGER);
		ret = errno;
	}

	initalized = 1;

end:
	pthread_mutex_unlock(&lock);
	return ret;
}

void STE_TM_deinit(void) {
	pthread_mutex_lock(&lock);

	if (initalized) {
		/* kill recv thread & wait for the all clear */
		pthread_kill(recv_thread_pid, SIGTERM);
		pthread_join(recv_thread_pid, NULL);

		/* close socket */
		close(recv_fd);
		recv_fd = -1;

		initalized = 0;
	}

	pthread_mutex_unlock(&lock);
}

int STE_TM_get_actions(char ***actions) {
	thermal_get_actions_req_msg_t req;
	thermal_get_actions_resp_msg_t *resp = NULL;
	int count;
	char **ret;
	int i = 0;
	int o = 0;

	pthread_mutex_lock(&lock);

	if (!initalized) {
		count = GENERIC_FAIL;
		ERROR_CB(TM_NOT_INITALISED);
		goto end;
	}

	/* build & send request */
	req.msg.len = sizeof(thermal_get_actions_req_msg_t);
	req.msg.id = GET_ACTIONS_REQ;
	send_thermal_msg_request((thermal_socket_msg_t *)&req, GET_ACTIONS_RESP, (void **) &resp);

	/* deal with response */
	if (resp == NULL) {
		count = GENERIC_FAIL;
		goto end;
	}

	count = resp->number_of_actions;
	resp->actions = ((char *)resp) + sizeof(thermal_get_actions_resp_msg_t);

	ret = malloc(sizeof(char *) * count);
	if (ret == NULL) {
		count = GENERIC_FAIL;
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		free(resp);
		goto end;
	}

	/* Copy out the received actions */
	for (i = 0; i < count; i++) {
		ret[i] = strdup(resp->actions + o);
		if (ret[i] == NULL) {
			int j = 0;
			/* Memory alloc failed clean up previous allocations*/
			for (j = 0; j < i; j++) {
				free(ret[j]);
			}
			/* And now free actions container */
			free(ret);
			free(resp);
			count = GENERIC_FAIL;
			goto end;
		}

		/* Move on to the start of the next action, adding one to */
		/* consider NULL terminator */
		o += strlen(ret[i]) + 1;
	}
	*actions = ret;
	free(resp);

end:
	pthread_mutex_unlock(&lock);
	return count;
}

int STE_TM_get_sensors(char ***sensors) {
	thermal_get_sensors_req_msg_t req;
	thermal_get_sensors_resp_msg_t *resp = NULL;
	int count;
	char **ret;
	int i = 0;
	int o = 0;

	pthread_mutex_lock(&lock);

	if (initalized == 0) {
		count = GENERIC_FAIL;
		ERROR_CB(TM_NOT_INITALISED);
		goto sensor_mutex_unlock;
	}

	/* build and send request */
	req.msg.len = sizeof(thermal_get_sensors_req_msg_t);
	req.msg.id = GET_SENSORS_REQ;
	send_thermal_msg_request((thermal_socket_msg_t *)&req, GET_SENSORS_RESP, (void **) &resp);

	/* deal with response */
	if (resp == NULL) {
		count = GENERIC_FAIL;
		goto sensor_mutex_unlock;
	}

	count = resp->number_of_sensors;
	resp->sensors = ((char *)resp) + sizeof(thermal_get_sensors_resp_msg_t);

	ret = malloc(sizeof(char *) * count);
	if (ret == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		count = GENERIC_FAIL;
		free(resp);
		goto sensor_mutex_unlock;
	}

	/* Copy out the received sensors */
	for (i = 0; i < count; i++) {
		ret[i] = strdup(resp->sensors + o);

		if (ret[i] == NULL) {
			int j;

			/* Memory alloc failed clean up previous allocations*/
			for (j = 0; j < i; j++) {
				free(ret[j]);
			}
			/* And now free sensors container */
			free(ret);
			free(resp);
			count = GENERIC_FAIL;
			goto sensor_mutex_unlock;
		}

		/* Move on to the start of the next sensor by adding 1*/
		o += strlen(ret[i]) + 1;
	}
	*sensors = ret;
	free(resp);

sensor_mutex_unlock:
	pthread_mutex_unlock(&lock);
	return count;
}

void STE_TM_free_actions(int count, char ***actions) {
	int i;

	if ((actions == NULL) || (*actions == NULL) || (count < 0)) {
		return;
	}

	for (i = 0; i < count; i++) {
		free((*actions)[i]);
	}

	free(*actions);
	*actions = NULL;
}

int STE_TM_get_sensor_config(const char *sensor, STE_TM_sensor_config_t **config) {
	thermal_get_sensor_config_req_msg_t *req;
	thermal_get_sensor_config_resp_msg_t *resp;
	int len;
	int i, j;
	int ret = 0;
	STE_TM_sensor_config_t *conf;
	thermal_sensor_notification_level_t *t;
	STE_TM_notification_level_t *level;

	if (sensor == NULL) {
		ERROR_CB(TM_INVALID_PARAMETER);
		ret = GENERIC_FAIL;
		goto end;
	}

	pthread_mutex_lock(&lock);

	if (initalized == 0) {
		ret = GENERIC_FAIL;
		ERROR_CB(TM_NOT_INITALISED);
		goto unlock;
	}

	len = sizeof(thermal_get_sensor_config_req_msg_t) + strlen(sensor) + 1;
	req = (thermal_get_sensor_config_req_msg_t *) malloc(len * sizeof(char));

	if (req == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		ret = GENERIC_FAIL;
		goto unlock;
	}

	req->msg.len = len;
	req->msg.id = GET_SENSOR_CONFIG_REQ;
	req->sensor = ((char *) req) + sizeof(thermal_get_sensor_config_req_msg_t);
	strncpy(req->sensor, sensor, strlen(sensor) + 1);

	send_thermal_msg_request((thermal_socket_msg_t *)req, GET_SENSOR_CONFIG_RESP, (void **) &resp);

	/* deal with response */
	if (resp == NULL) {
		ret = GENERIC_FAIL;
		goto unlock;
	}

	if (resp->error) {
		ret = resp->error;
		goto freereqresp;
	}

	conf = (STE_TM_sensor_config_t *) malloc(sizeof(STE_TM_sensor_config_t));
	if (conf == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		ret = GENERIC_FAIL;
		goto freereqresp;
	}

	conf->name = strdup(sensor);
	conf->no_notification_levels = resp->notification_level_count;
	conf->current_notification_level = resp->current_level;

	conf->notification_levels = malloc(sizeof(STE_TM_notification_level_t *) * conf->no_notification_levels);

	if (conf->notification_levels == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		ret = GENERIC_FAIL;
		goto freereqresp;
	}

	t = (thermal_sensor_notification_level_t *)(((char *) resp) + sizeof(thermal_get_sensor_config_resp_msg_t));
	i = 0;
	while (i < resp->notification_level_count) {
		char *str;

		level = malloc(sizeof(STE_TM_notification_level_t));
		if (level == NULL) {
			ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
			ret = FAILED_ALLOCATING_CONFIG_DATA;
			goto freeconfigdata;
		}

		// build up level struct
		level->notification_level = t->level;
		level->min_alarm_level = t->min_alarm_value;
		level->max_alarm_level = t->max_alarm_value;
		level->number_of_actions = t->action_count;

		level->actions = malloc(sizeof(char *) * level->number_of_actions);
		if (level->actions == NULL) {
			ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
			ret = FAILED_ALLOCATING_CONFIG_DATA;
			goto freeconfigdata;
		}

		// now spin through the avaliable actions
		str = (((char *) t) + sizeof(thermal_sensor_notification_level_t));
		for (j = 0; j < level->number_of_actions; j++) {
			level->actions[j] = strdup(str);
			if (level->actions[j] == NULL) {
				ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
				ret = FAILED_ALLOCATING_CONFIG_DATA;
				goto freeconfigdata;
			}

			str = str + strlen(str) + 1;
		}

		// return to client
		conf->notification_levels[i] = level;

		t = (thermal_sensor_notification_level_t *)(((char *) t) + t->len);
		i++;
	}

freeconfigdata:

	if (ret == FAILED_ALLOCATING_CONFIG_DATA) {

		/* Free up notification levels and actions as we failed allocating */
		STE_TM_free_sensor_config(&conf);
		ret = -1;
	}

	*config = conf;
freereqresp:
	free(resp);
	free(req);
unlock:
	pthread_mutex_unlock(&lock);
end:
	return ret;

}

void STE_TM_free_sensors(int count, char ***sensors) {
	int i;

	if ((sensors == NULL) || (*sensors == NULL)) {
		return;
	}

	for (i = 0; i < count; i++) {
		if ((*sensors)[i] != NULL)
		{
			free((*sensors)[i]);
		}
	}

	free(*sensors);
	*sensors = NULL;
}

void STE_TM_free_sensor_config(STE_TM_sensor_config_t **sensor) {
	STE_TM_notification_level_t *level;
	int i, j;

	if (*sensor == NULL) {
		return;
	}

	for (i = 0; i < (*sensor)->no_notification_levels; i++) {
		level = (*sensor)->notification_levels[i];

		if (level == NULL) { 
			/* This notification level does not exist check next one */
			continue;
		}

		if (level->actions == NULL) { 
			/* This notification level exists but not the action container */
			free(level);
			continue;
		}

		for (j = 0; j < level->number_of_actions; j++) {
			if (level->actions[j] == NULL) { 
				/* This action does not exist check next one */
				continue;
			}
			free(level->actions[j]);
		}
		free(level->actions);
		free(level);
	}
	free((*sensor)->notification_levels);
	free((*sensor)->name);
	free(*sensor);
	*sensor = NULL;
}

STE_TM_notification_t *STE_TM_clone_notification(STE_TM_notification_t *original) {
	STE_TM_notification_t *clone;
	int i;

	if (original == NULL) {
		return NULL;
	}

	clone = (STE_TM_notification_t *) malloc(sizeof(STE_TM_notification_t));
	if (clone == NULL) {
		return NULL;
	}

	clone->current_level = original->current_level;
	clone->previous_level = original->previous_level;
	clone->shutdown_in = original->shutdown_in;
	clone->number_of_actions_taken = original->number_of_actions_taken;

	clone->sensor = strdup(original->sensor);
	if (clone->sensor == NULL) {
		free(clone);
		return NULL;
	}

	clone->actions_taken = (char **) malloc(sizeof(char *) *
						clone->number_of_actions_taken);
	if (clone->actions_taken == NULL) {
		free(clone->sensor);
		free(clone);
		return NULL;
	}

	for (i = 0; i < clone->number_of_actions_taken; i++) {
		clone->actions_taken[i] = strdup(original->actions_taken[i]);
		if (clone->actions_taken[i] == NULL) {
			int j;

			for (j = 0; j < i; j++) {
				/* free any actions that have been allocated */
				free(clone->actions_taken[j]);
			}
			return NULL;
		}
	}

	return clone;
}

void STE_TM_free_cloned_notification(STE_TM_notification_t **notification) {
	int i;
	if (notification == NULL || *notification == NULL) {
		return;
	}

	for (i = 0; i < (*notification)->number_of_actions_taken; i++) {
		free((*notification)->actions_taken[i]);
	}

	free((*notification)->actions_taken);
	free((*notification)->sensor);
	free(*notification);
	*notification = NULL;
}

static void recv_thread_sighandler(int sig) {
	pthread_exit(NULL);
}

static void *recv_thread(void *arg) {
	struct sigaction sig;

	/* Handle SIGTERM here to ensure thread exits instead of process */
	sig.sa_handler = recv_thread_sighandler;
	sig.sa_flags = 0;
	sigaction(SIGTERM, &sig, NULL);

	while (1) {
		thermal_socket_msg_t m;
		thermal_socket_msg_t *msg;
		int len;

		len = recv(recv_fd, &m, sizeof(thermal_socket_msg_t), MSG_PEEK);
		if (len <= 0 || len != sizeof(thermal_socket_msg_t)) {
			ERROR_CB(TM_INVALID_DATA_RECEIVED);
			return NULL;
		}

		msg = (thermal_socket_msg_t *) malloc(m.len);
		if (msg == NULL) {
			ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
			continue;
		}

		len = recv(recv_fd, msg, m.len, MSG_WAITALL);
		if (len <= 0 || len != m.len) {
			ERROR_CB(TM_INVALID_DATA_RECEIVED);
			return NULL;
		}

		if (msg->id == THERMAL_NOTIFICATION_EVENT) {
			handle_thermal_event((thermal_notification_event_msg_t *) msg);
			free(msg);
		} else {
			pthread_mutex_lock(&recv_lock);
			if (wait_msgid == msg->id) {
				wait_data = msg;
				sem_post(&wait_sem);
			} else {
				free(msg);
			}
			pthread_mutex_unlock(&recv_lock);
		}
	}
	return NULL;
}

static void send_thermal_msg_request(thermal_socket_msg_t *req, thermal_msgid_t id, void **resp) {
	if (resp != NULL) {
		pthread_mutex_lock(&recv_lock);
		sem_init(&wait_sem, 0, 0);
		wait_msgid = id;
		pthread_mutex_unlock(&recv_lock);
	}

	write(recv_fd, req, req->len);

	sem_wait(&wait_sem);

	pthread_mutex_lock(&recv_lock);
	*resp = wait_data;
	wait_data = NULL;
	wait_msgid = INVALID;
	pthread_mutex_unlock(&recv_lock);
}

static void handle_thermal_event(thermal_notification_event_msg_t *msg) {
	STE_TM_notification_t *notification;
	char *actions;
	int i;

	notification = (STE_TM_notification_t *) malloc(sizeof(STE_TM_notification_t));
	if (notification == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		return;
	}

	notification->current_level = msg->current_level;
	notification->previous_level = msg->previous_level;
	notification->shutdown_in = msg->shutdown_timeout;
	notification->number_of_actions_taken = msg->actions_taken_count;

	msg->sensor = (((char *) msg) + sizeof(thermal_notification_event_msg_t));
	notification->sensor = strdup(msg->sensor);

	if (notification->sensor == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		goto notsensmemfail;
	}

	notification->actions_taken = (char **) malloc(sizeof(char *) * notification->number_of_actions_taken);

	if (notification->actions_taken == NULL) {
		ERROR_CB(TM_UNABLE_TO_ALLOCATE_MEMORY);
		goto notactmemfail;
	}

	actions = msg->sensor + strlen(msg->sensor) + 1;
	for (i = 0; i < notification->number_of_actions_taken; i++) {
		notification->actions_taken[i] = strdup(actions);
		actions = actions + strlen(actions) + 1;
	}

	if (client_callbacks.notification_cb != NULL) {
		client_callbacks.notification_cb(notification);
	}

	for (i = 0; i < notification->number_of_actions_taken; i++) {
		free(notification->actions_taken[i]);
	}

	free(notification->actions_taken);
notactmemfail:
	free(notification->sensor);
notsensmemfail:
	free(notification);
}
