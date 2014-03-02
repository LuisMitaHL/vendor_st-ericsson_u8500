/*
 * Thermal Service Manager
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __ACTIONS_H__
#define __ACTIONS_H__

/**
 * The name of the action that will cause the platform
 * to shutdown.
 */
#define ACTION_SHUTDOWN_NAME "SHUTDOWN"

/**
 * Brings up the actions subsystem
 */
void actions_init(void);

/**
 * Adds a new Android handled action to the system
 * @param action Action name
 */
void actions_add(const char *action);

/**
 * Retrieves a list of all actions in the system.
 * @param list Will be updated to a char array that will be allocated to hold the list
 * @param count Will be updated with the list length
 * @return 0 if success, -1 if not
 */
int actions_getlist(char **list, int *count);

/**
 * Executes an action (if it exists)
 * @param action Name of action to execute
 * @return 0 if success, -1 if not
 */
int actions_execute(const char *action);

#endif
