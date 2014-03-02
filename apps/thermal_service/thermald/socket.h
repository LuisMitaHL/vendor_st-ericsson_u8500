/*
 * Thermal Service Manager
 *
 * Entry point for the Thermal Service
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <socketmsgs.h>

/**
 * Bring up socket subsystem
 * @return -1 if failure
 */
int socket_init(void);

/**
 * Set the path to the unix socket to create or use
 * the default provided in thermal_manager.h
 * @param path Full path to unix socket
 */
void socket_setunixsocket(char *path);

/**
 * Wait's for the server thread to die. If this returns
 * we've had a catastrophic error!
 */
int socket_wait(void);

/**
 * Called from the sensors subsystem to raise a notification
 * event for all connected clients
 * @param msg ThermalNotification message to send
 */
void socket_event(thermal_notification_event_msg_t *msg);

#endif
