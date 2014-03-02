/*
 * Positioning Manager
 *
 * test_engine.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef __TEST_STELP_SOCKET_H__
#define __TEST_STELP_SOCKET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>

#include <agpsosa.h>

#include <test_engine_int.h>
#include <test_engine_process.h>



#define  INADDR_STELP_AMIL_SERVER_HOST             0x7f000001  //inet_addr("127.0.0.1") localhost

#define  PORT_STELP_AMIL_SERVER_HOST               3333      /* A random port selected which is unused according to IANA*/

typedef struct {

    int fd_stelp_socket;
    int stelp_socket_data_read;
    int SocketCreated;

} Lbsstelp_Socket;


Lbsstelp_Socket socket_conn;

TSTENG_socket_msg_t msg;



void LBSSTELP_socket_close_down(void);
void LBSSTELP_socket_init(void);

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "Stelp"


#endif





