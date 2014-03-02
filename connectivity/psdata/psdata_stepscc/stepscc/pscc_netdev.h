/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control - net device
 */

#ifndef PSCC_NETDEV_H
#define PSCC_NETDEV_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "pscc_handler.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * pscc_netdev_destroyed_fp_t
 *
 * @connid - connection id
 *
 * Returns: none
 *
 **/
typedef void (*pscc_netdev_destroyed_fp_t)(int connid);

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * pscc_netdev_init - initialize net device
 *
 * Returns: file descriptor, -1 on error

 **/
int pscc_netdev_init(void);

/**
 * pscc_netdev_shutdown - deinitialize net device
 *
 * Returns: void
 *
 **/
void pscc_netdev_shutdown(void);

/**
 * pscc_netdev_handle_message - Message Handler for net device
 *
 * @fd              - file descriptor
 *
 * Returns: 0 on handled, -1 if unhandled
 *
 **/
int pscc_netdev_handle_message(int fd);

/**
 * pscc_netdev_create - Create net device
 *
 * @connid              - connection ID
 * @param_get_cb        - Get parameter callback fp
 * @param_set_cb        - Set parameter callback fp
 * @param_delete_cb     - Delete parameter callback fp
 * @netdev_destroyed_cb - Destroyed netdev callback fp
 *
 * Returns: dev_handle on success, -1 on error
 *
 **/
int pscc_netdev_create(
    int                         connid,
    pscc_get_param_fp_t         param_get_cb,
    pscc_set_param_fp_t         param_set_cb,
    pscc_delete_param_fp_t      param_delete_cb,
    pscc_netdev_destroyed_fp_t  netdev_destroyed_cb
);

/**
 * pscc_netdev_destroy - Destroy net device
 *
 * @dev_handle          - Handle of the device to be deleted
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_netdev_destroy(int dev_handle);

#endif //PSCC_NETDEV_H


