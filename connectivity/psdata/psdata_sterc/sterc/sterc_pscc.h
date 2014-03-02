/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Sterc PPP handler
 */
#ifndef STERC_PSCC_H
#define STERC_PSCC_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "stercd.h"
#include "sterc_handler.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * sterc_pscc_connected_fp_t
 *
 * @parentid - parentid id
 *
 * Returns: -
 *
 **/
typedef void (*sterc_pscc_connected_fp_t)(int parentid);

/**
 * sterc_pscc_disconnected_fp_t
 *
 * @parentid - parentid id
 *
 * Returns: -
 *
 **/
typedef void (*sterc_pscc_disconnected_fp_t)(int parentid);

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/*
 * sterc_pscc_init - init
 *
 * @tt           - transport type
 * @ctrl_addr_p  - pscc control address
 * @event_addr_p - pscc event address
 *
 * Returns: file descriptor, (-1) on error
 *
 **/
int sterc_pscc_init(sterc_transport_type_t tt, char *ctrl_addr_p,char *event_addr_p);

/*
 * sterc_pscc_deinit - de-inititialise
 *
 * Returns: -
 *
 **/
void sterc_pscc_deinit(void);

/*
 * sterc_pscc_handler - handle pscc event messages
 *
 * @fd      file descriptor
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int sterc_pscc_handler(int fd);

/*
 * sterc_pscc_create - create pscc instance
 *
 * @parentid         reference to caller
 * @get_param_cb     callback to get a parameter
 * @set_param_cb     callback to set a parameter
 * @delete_param_cb  callback to delete a previously set parameter
 *
 * Returns: handle, (-1) on error
 *
 **/
int sterc_pscc_create(int parentid,
                      sterc_get_param_fp_t get_param_cb,
                      sterc_set_param_fp_t set_param_cb,
                      sterc_delete_param_fp_t delete_param_cb);

/*
 * sterc_pscc_connect - connect pscc instance
 *
 * @handle           bearer instance
 * @connected_cb     connected callback
 * @disconnected_cb  disconnected callback
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int sterc_pscc_connect(int handle,
                       sterc_pscc_connected_fp_t connected_cb,
                       sterc_pscc_disconnected_fp_t disconnected_cb);

/*
 * sterc_pscc_disconnect - disconnect pscc instance
 *
 * @handle      bearer instance
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int sterc_pscc_disconnect(int handle);

/*
 * sterc_pscc_destroy - destroy pscc instance
 *
 * @handle      bearer instance
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int sterc_pscc_destroy(int handle);

#endif /* STERC_PSCC_H */
