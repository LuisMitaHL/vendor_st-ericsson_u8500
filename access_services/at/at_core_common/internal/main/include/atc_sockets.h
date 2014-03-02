/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_SOCKETS_H
#define ATC_SOCKETS_H

int atc_socket_create(const char *path);
int atc_socket_accept(int fd);

#endif
