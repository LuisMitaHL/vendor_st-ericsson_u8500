/*
 * Thermal Service Manager
 *
 * Abstracts socket server/client setup and deals with creating
 * & dealing with messages received from the client.
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "socket.h"
#include "sensors.h"
#include "actions.h"
#include "log.h"

#include "socketmsgs.h"

#include <thermal_manager.h>

#define SOCKET_MAX_CLIENTS (5)

typedef struct socket_client_s {
	int fd;
	pthread_t pid;
	struct socket_client_s *next;
} socket_client_t;

static void *socket_server_thread(void *arg);
static void *socket_client_thread(void *arg);
static inline void socket_handle_message(int fd, void *msg);
static inline void socket_send_msg(int fd, thermal_socket_msg_t *msg);
static inline void handle_get_actions_req(int fd);
static inline void handle_get_sensors_req(int fd);
static inline void handle_get_sensor_config_req(int fd, thermal_get_sensor_config_req_msg_t *msg);

static pthread_t socket_server_pid;
static int socket_server_fd;
static socket_client_t *socket_clients = NULL;
static pthread_mutex_t socket_clients_lock;
static char *unix_socket_path = NULL;

/*
 * Starts the receiving socket thread
 */
int socket_init(void) {
	pthread_mutex_init(&socket_clients_lock, NULL);

	if (unix_socket_path == NULL) {
		unix_socket_path = THERMAL_MANAGER_DEFAULT_SOCKET;
	}

	if (pthread_create(&socket_server_pid, NULL, socket_server_thread, NULL) != 0) {
		ERR("unable to create socket thread\n");
		return -1;
	}

	return 0;
}

/*
 * Set's the unix socket's path
 */
void socket_setunixsocket(char *path) {
	unix_socket_path = path;
}

/*
 * Blocking call that waits for the server thread to shutdown
 */
int socket_wait(void) {
	return pthread_join(socket_server_pid, NULL);
}

void socket_event(thermal_notification_event_msg_t *msg) {
	socket_client_t *c;

	pthread_mutex_lock(&socket_clients_lock);

	DBG("--> THERMAL_NOTIFICATION_EVENT\n");

	c = socket_clients;
	while (c != NULL) {
		socket_send_msg(c->fd, (thermal_socket_msg_t *) msg);
		c = c->next;
	}

	pthread_mutex_unlock(&socket_clients_lock);
}

/*
 * Main receive thread setup to handle new incoming
 * clients
 */
static void *socket_server_thread(void *arg) {
	struct sockaddr_un srv;
	int len;

	INF("starting socket thread...\n");

	/* try and remove old unix socket if needed */
	if (unlink(unix_socket_path) != 0) {
		if (errno != ENOENT) {
			ERR("unable to remove old unix socket\n");
			return NULL;
		}
	}

	/* create socket and start listening for clients */
	socket_server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_server_fd < 0) {
		ERR("unable to create new unix socket\n");
		return NULL;
	}

	srv.sun_family = AF_UNIX;
	strncpy(srv.sun_path, unix_socket_path, UNIX_PATH_MAX);
	len = strlen(srv.sun_path) + sizeof(srv.sun_family) + 1;

	if (bind(socket_server_fd, (struct sockaddr *) &srv, len) < 0) {
		ERR("unable to bind socket\n");
		return NULL;
	}

	if (listen(socket_server_fd, SOCKET_MAX_CLIENTS) < 0) {
		ERR("unable to accept %d clients\n", SOCKET_MAX_CLIENTS);
		return NULL;
	}

	INF("socket init complete. awaiting new clients...\n");

	while (1) {
		struct sockaddr_un client;
		socket_client_t *sclient;
		socklen_t l = sizeof(struct sockaddr_un);
		int fd;

		/* wait for a new client to arrived & build our local struct */
		fd = accept(socket_server_fd, (struct sockaddr *) &client, &l);
		if (fd < 0) {
			ERR("failed to accept new client\n");
			return NULL;
		}

		sclient = (socket_client_t *) malloc(sizeof(socket_client_t));
		if (sclient == NULL) {
			ERR("unable to allocate memory\n");
			return NULL;
		}

		sclient->fd = fd;
		sclient->next = NULL;

		/* add client to internal list */
		pthread_mutex_lock(&socket_clients_lock);
		if (socket_clients == NULL) {
			socket_clients = sclient;
		} else {
			socket_client_t *nxt = socket_clients;

			while (nxt->next != NULL)
				nxt = nxt->next;

			nxt->next = sclient;
		}
		pthread_mutex_unlock(&socket_clients_lock);

		/* start up client recv thread */
		if (pthread_create(&sclient->pid, NULL, socket_client_thread, (void *) sclient) != 0) {
			ERR("unable to start client thread\n");
			return NULL;
		}
	}

	return NULL;
}

/*
 * Manages communication with the client
 */
static void *socket_client_thread(void *arg) {
	socket_client_t *me = (socket_client_t *) arg;

	INF("client connected\n");

	while (1) {
		thermal_socket_msg_t msgheader;
		void *msg;
		int len;

		/* read the message header to determine the size of
		 * memory required to read the entire message
		 */
		len = recv(me->fd, &msgheader, sizeof(thermal_socket_msg_t), MSG_PEEK);
		if (len != sizeof(thermal_socket_msg_t)) {
			goto cleanup;
		}

		msg = malloc(msgheader.len);
		if (msg == NULL) {
			ERR("unable to allocate memory\n");
			goto cleanup;
		}

		/* read the entire message now */
		len = recv(me->fd, msg, msgheader.len, 0);
		if (len != msgheader.len) {
			ERR("invalid msg length read!\n");
			free(msg);
			goto cleanup;
		}

		socket_handle_message(me->fd, msg);

		free(msg);
		msg = NULL;
	}

cleanup:
	INF("client disconnected\n");
	return NULL;
}

/*
 * Parses incoming messages and perform appropriate calls into
 * sub modules
 */
static inline void socket_handle_message(int fd, void *msg) {
	thermal_socket_msg_t *msgheader;

	msgheader = (thermal_socket_msg_t *) msg;

	switch (msgheader->id) {
	case GET_ACTIONS_REQ:
		INF("--> GET_ACTIONS_REQ\n");
		handle_get_actions_req(fd);
		break;

	case GET_SENSORS_REQ:
		INF("--> GET_SENSORS_REQ\n");
		handle_get_sensors_req(fd);
		break;

	case GET_SENSOR_CONFIG_REQ:
		INF("--> GET_SENSOR_CONFIG_REQ\n");
		handle_get_sensor_config_req(fd, msg);
		break;

	/* handle invalid messages (ie ones we should never receive) */
	case THERMAL_NOTIFICATION_EVENT:
	case GET_ACTIONS_RESP:
	case GET_SENSORS_RESP:
	case GET_SENSOR_CONFIG_RESP:
	default:
		ERR("invalid message received\n");
		break;
	}
}

/*
 * Sends a message to a client
 */
static inline void socket_send_msg(int fd, thermal_socket_msg_t *msg) {
	int sendlen;

	if (fd < 0 || msg == NULL || msg->len == 0) {
		ERR("invalid parameters\n");
		return;
	}

	DBG("sending %d bytes to client\n", msg->len);

	sendlen = send(fd, msg, msg->len, 0);
	if (sendlen < 0) {
		switch (errno) {
		case EPIPE:
			ERR("remote end disconnected\n");
			break;
		}
		ERR("unable to send message\n");
	} else if (sendlen != msg->len) {
		ERR("sent only partial message\n");
	}
}

/*
 * Deals with the GET_ACTIONS_REQ message
 */
static inline void handle_get_actions_req(int fd) {
	int actions_len;
	int actions_count;
	char *actions_list;
	thermal_get_actions_resp_msg_t *msg;
	int len;

	len = sizeof(thermal_get_actions_resp_msg_t);

	actions_len = actions_getlist(&actions_list, &actions_count);
	len += actions_len;

	msg = (thermal_get_actions_resp_msg_t *) malloc(len);
	if (msg == NULL) {
		ERR("unable to allocate memory\n");
		free(actions_list);
		return;
	}

	msg->msg.len = len;
	msg->msg.id = GET_ACTIONS_RESP;
	msg->number_of_actions = actions_count;
	msg->actions = ((char *) msg) + sizeof(thermal_get_actions_resp_msg_t);

	memcpy(msg->actions, actions_list, actions_len);

	INF("--> GET_ACTIONS_RESP\n");
	socket_send_msg(fd, (thermal_socket_msg_t *) msg);

	free(msg);
	free(actions_list);
}

/*
 * Deals with the GET_SENSORS_REQ message
 */
static inline void handle_get_sensors_req(int fd) {
	int sensors_len;
	int sensors_count;
	char *sensors_list;
	thermal_get_sensors_resp_msg_t *msg;
	int len;

	len = sizeof(thermal_get_sensors_resp_msg_t);

	sensors_len = sensor_getlist(&sensors_list, &sensors_count);
	len += sensors_len;

	msg = (thermal_get_sensors_resp_msg_t *) malloc(len);
	if (msg == NULL) {
		ERR("unable to allocate memory\n");
		free(sensors_list);
		return;
	}

	msg->msg.len = len;
	msg->msg.id = GET_SENSORS_RESP;
	msg->number_of_sensors = sensors_count;
	msg->sensors = ((char *) msg) + sizeof(thermal_get_sensors_resp_msg_t);

	memcpy(msg->sensors, sensors_list, sensors_len);

	INF("--> GET_SENSORS_RESP\n");
	socket_send_msg(fd, (thermal_socket_msg_t *) msg);

	free(msg);
	free(sensors_list);
}

/*
 * Deals with the GET_SENSOR_CONFIG_REQ message
 */
static inline void handle_get_sensor_config_req(int fd,
						thermal_get_sensor_config_req_msg_t *inmsg) {
	thermal_get_sensor_config_resp_msg_t *msg;
	int len;

	inmsg->sensor = ((char *) inmsg) + sizeof(thermal_get_sensor_config_req_msg_t);

	INF("requesting configuration for sensor \"%s\"\n", inmsg->sensor);

	len = sensor_getsensorresp(inmsg->sensor, &msg);

	if (len == 0) {
		ERR("failed to find sensor config\n");
		return;
	}

	INF("--> GET_SENSOR_CONFIG_RESP\n");
	socket_send_msg(fd, (thermal_socket_msg_t *) msg);

	free(msg);
}

