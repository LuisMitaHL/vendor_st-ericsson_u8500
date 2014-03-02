/*                               -*- Mode: C -*-
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: SIM Phonebook service socket info
 *
 * Author: Mats Bengtsson <mats.m.bengtson@stericsson.com>
 *
 */

#ifndef __simpbd_socket_h__
#define __simpbd_socket_h__ (1)

#define SIMPBD_NUM_SOCKET_SERVERS (1)
#define SIMPBD_SOCKET_MAX_CLIENTS (10)

#ifdef __i386
static const char simpbd_socket_name[] = "/tmp/socket/simpbd";
#else
static const char simpbd_socket_name[]   = "/dev/socket/simpbd";
#endif

#endif /* __simpbd_socket_h__ */
