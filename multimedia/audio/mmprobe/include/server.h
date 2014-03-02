/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef	_SOCKET_SERVER_H
#define	_SOCKET_SERVER_H	1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define BACKLOG 10	 	// how many pending connections queue will hold.
#define PORT 	30000	// default port for the socket server.

/**
 * Initiates the probe server by initializing a socket and a FIFO pipe.
 */
int init_server(const int port, int *sockfd);

/**
 * Starts the probe server.
 */
int start_server(const int sockfd, char* enabledProbes, int logDecoderFormat);

/**
 * Stops the probe server.
 */
int stop_server(const int sockfd);

#endif /* socket_server.h  */
