/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Per Persson per.xb.persson@stericsson.com for
 * ST-Ericsson.
 *
 * License terms:
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <linux/types.h>
#include <errno.h>      /* Errors */
#include <stdarg.h>
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#ifdef ANDROID
#include <utils/Log.h>
#endif
#include "../include/hdmi_service_api.h"
#include "../include/hdmi_service_local.h"

pthread_t thread_sockclient;
#ifdef HDMI_SERVICE_USE_CALLBACK_FN
pthread_t thread_sockserver;
#endif /*HDMI_SERVICE_USE_CALLBACK_FN*/
int clientsocket = -1;
int listensocket = -1;
int serversocket = -1;
int no_return_msg = -1;

int listensocket_set(int sock)
{
	listensocket = sock;
	return 0;
}

int listensocket_get(void)
{
	return listensocket;
}

static int clientsocket_set(int sock)
{
	clientsocket = sock;
	return 0;
}

int clientsocket_get(void)
{
	return clientsocket;
}

/* Client socket thread. Handles incoming socket messages */
static void thread_sockclient_fn(int *arg)
{
	int bytes = 0;
	char buffer[SOCKET_DATA_MAX];
	struct cmd_data cmd_data;
	int cont = 1;
	int sock;
	int buf_index = 0;

	LOGHDMILIB("%s begin", __func__);

	sock = *arg;
	if (clientsocket_get() < 0)
		clientsocket_set(sock);
	LOGHDMILIB("clisock:%d", sock);

	while (cont) {
		if (bytes < CMDBUF_OFFSET) {
			/* Read from socket */
			bytes += read(sock, buffer + buf_index,
					SOCKET_DATA_MAX - buf_index);
			if (bytes <= 0) {
				LOGHDMILIBE("clisocket closed:%d", bytes);
				goto thread_sockclient_fn_end;
			}

			LOGHDMILIB("clisockread:%d", bytes);

			if (bytes < CMDBUF_OFFSET)
				/* Not enough data */
				continue;
		}

		/* Valid command */
		cmd_data.cmd = (__u32)buffer[buf_index + CMD_OFFSET];
		cmd_data.cmd_id = (__u32)buffer[buf_index + CMDID_OFFSET];
		cmd_data.data_len = (__u32)buffer[buf_index + CMDLEN_OFFSET];
		memcpy(cmd_data.data, &buffer[buf_index + CMDBUF_OFFSET],
					cmd_data.data_len);
		cmd_data.next = NULL;

		/* Remaining bytes to handle */
		bytes -= (CMDBUF_OFFSET + cmd_data.data_len);
		buf_index += (CMDBUF_OFFSET + cmd_data.data_len);
		if (bytes < 0)
			bytes = 0;
		if (bytes < CMDBUF_OFFSET) {
			memcpy(buffer, buffer + buf_index, bytes);
			buf_index = bytes;
		}

		/* Add to list */
		cmd_add(&cmd_data);

		/* Signal */
		hdmi_event(HDMIEVENT_CMD);

		if (cmd_data.cmd == HDMI_EXIT)
			cont = 0;
	}

thread_sockclient_fn_end:
	close(sock);
	if (clientsocket_get() < 0)
		clientsocket_set(-1);

	LOGHDMILIB("%s end: %d", __func__, bytes);
	pthread_exit(NULL);
}

int clientsocket_send(__u8 *buf, int len)
{
	int sock;
	int sent = -1;
	int res = -1;

	if (no_return_msg == 1)
		return 0;

	sock = clientsocket_get();
	if (sock >= 0) {
		sent = write(sock, buf, len);
		LOGHDMILIB("%s written %d bytes on sock", __func__, sent);
	}

	if (sent == len)
		res = 0;
	return res;
}

/* Socket listen thread.
 * Creates a listen socket.
 * Listens for incoming connection.
 * At connection attempt, creates a client socket in client thread.
 */
void thread_socklisten_fn(void *arg)
{
	int socknew;
	socklen_t clilen;
	struct sockaddr_un serv_addr;
	struct sockaddr_un cli_addr;
	int res;
	int sockl;

	LOGHDMILIB("%s begin", __func__);

	/* Create listen socket */
	sockl = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockl < 0) {
		LOGHDMILIBE("%s", "socket create fail");
		goto thread_socklisten_fn_end;
	}
	listensocket_set(sockl);
	LOGHDMILIB2("Listen socket create:%d", sockl);

	/* Remove any old path */
	unlink(SOCKET_LISTEN_PATH);

	/* Bind to path */
	memset((char *) &serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, SOCKET_LISTEN_PATH);
	res = bind(sockl, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (res < 0) {
		LOGHDMILIBE("socket bind fail:%d", res);
		close(sockl);
		goto thread_socklisten_fn_end;
	}

	LOGHDMILIB2("Listen socket bind: %d", res);

	/* while loop is breaked by shutdown on listen socket */
	while (1) {
		/* Listen for incoming connection */
		if (listen(sockl, SOCKET_MAX_CONN) != 0) {
			LOGHDMILIBE("%s listen error", __func__);
			close(sockl);
			goto thread_socklisten_fn_end;
		}

		/* Establish a client connection */
		clilen = sizeof(cli_addr);
		socknew = accept(sockl, (struct sockaddr *) &cli_addr, &clilen);
		if (socknew < 0) {
			LOGHDMILIBE("socket accept fail:%d", socknew);
			close(sockl);
			goto thread_socklisten_fn_end;
		}
		LOGHDMILIB2("socket accept:%d", socknew);

		if (socknew >= 0)
			/* Create a client thread */
			pthread_create(&thread_sockclient, NULL,
				(void *)thread_sockclient_fn, (void *)&socknew);
	}

thread_socklisten_fn_end:
	/* Remove any old path */
	unlink(SOCKET_LISTEN_PATH);

	LOGHDMILIB("%s end", __func__);
	pthread_exit(NULL);
}

static int serversocket_set(int sock)
{
	serversocket = sock;
	return 0;
}

#ifdef HDMI_SERVICE_USE_CALLBACK_FN
/* Server socket thread. Handles outgoing socket messages */
static void thread_sockserver_fn(void *arg)
{
	int res;
	char buffer[SOCKET_DATA_MAX];
	struct cmd_data cmd_data;
	int cont = 1;
	int sock;
	cb_fn callback;

	LOGHDMILIB("%s begin", __func__);

	sock = (int)arg;

	while (cont) {
		memset(buffer, 0, SOCKET_DATA_MAX);
		res = read(sock, buffer, SOCKET_DATA_MAX);
		if (res <= 0) {
			LOGHDMILIB("servsocket closed:%d", res);
			goto thread_sockserver_fn_end;
		}

		LOGHDMILIB("servsockread res:%d", res);

		if (cmd_data.cmd == HDMI_EXIT) {
			cont = 0;
		} else {
			cmd_data.cmd = (__u32)buffer[CMD_OFFSET];
			cmd_data.cmd_id = (__u32)buffer[CMDID_OFFSET];
			cmd_data.data_len = (__u32)buffer[CMDLEN_OFFSET];
			memcpy(cmd_data.data, &buffer[CMDBUF_OFFSET],
						cmd_data.data_len);
			cmd_data.next = NULL;

			/* Send through callback fn */
			callback = hdmi_service_callback_get();
			LOGHDMILIB("callback:%p", callback);
			if (callback)
				callback(cmd_data.cmd, cmd_data.data_len,
							cmd_data.data);
		}
	}

thread_sockserver_fn_end:
	close(sock);

	LOGHDMILIB("%s end res:%d", __func__, res);
	pthread_exit(NULL);
}
#endif /*HDMI_SERVICE_USE_CALLBACK_FN*/

int serversocket_create(int avoid_return_msg)
{
	int sock;
	int len;
	struct sockaddr_un addr;
	int n;

	LOGHDMILIB("%s begin", __func__);

	no_return_msg = avoid_return_msg;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	LOGHDMILIB("sock:%d", sock);
	if (sock < 0) {
		LOGHDMILIBE("%s %s", __func__, "socket create fail");
		goto serversocket_create_end;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_LISTEN_PATH);
	len = sizeof(addr);

	n = connect(sock, (struct sockaddr *)&addr, len);
	LOGHDMILIB("connect:%d", n);

	if (n < 0) {
		LOGHDMILIBE("%s socket connect err:%d", __func__, n);
		close(sock);
		sock = -1;
	}

serversocket_create_end:
	if (sock >= 0) {
		serversocket_set(sock);
		LOGHDMILIB("servsock:%d", sock);

#ifdef HDMI_SERVICE_USE_CALLBACK_FN
		/* Create a server thread */
		pthread_create(&thread_sockserver, NULL,
			(void *)thread_sockserver_fn, (void *)sock);
#endif /*HDMI_SERVICE_USE_CALLBACK_FN*/
	}
	LOGHDMILIB("%s end", __func__);
	return sock;
}

static int serversocket_get(void)
{
	return serversocket;
}

int serversocket_write(int len, __u8 *data)
{
	int n;
	int sock;

	sock = serversocket_get();
	n = write(sock, data, len);
	LOGHDMILIB("write socket len res:%d %d %d", sock, len, n);

	return n;
}

int serversocket_close(void)
{
	int sock;

	sock = serversocket_get();
	return close(sock);
}
