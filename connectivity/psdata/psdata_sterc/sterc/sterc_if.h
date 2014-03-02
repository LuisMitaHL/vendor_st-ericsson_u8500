/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control, Abstraction Layer for Sterc interface
 */

#ifndef STERC_IF_H
#define STERC_IF_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "sterc_handler.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * sterc_if_create_fp_t - function prototype for creating a sterc interface
 *
 * @parentid - parentid id
 * @get_param_cb - callback function to get a parameter
 * @set_param_cb - callback function to set a parameter
 * @delete_param_cb - callback function to delete a parameter
 *
 * Returns: Handle to interface
 *
 **/
typedef int (*sterc_if_create_fp_t)(int parentid,
                                    sterc_get_param_fp_t get_param_cb,
                                    sterc_set_param_fp_t set_param_cb,
                                    sterc_delete_param_fp_t delete_param_cb);


/*
 * sterc_if_destroy_fp_t - Funtion prototype to destroy the interface
 *
 * @handle
 *
 * Returns: -
 *
 **/
typedef void (*sterc_if_destroy_fp_t) (int handle);


/*
 * sterc_if_destroy_fp_t - Funtion prototype to destroy the interface
 *
 * @handle
 * @connected_cb - callback function
 * @disconnected_cb - callback function
 *
 * Returns: (-1) on error
 *
 **/
typedef int (*sterc_if_connect_fp_t)(int handle,
                                     sterc_if_connected_fp_t connected_cb,
                                     sterc_if_disconnected_fp_t disconnected_cb);


/*
 * sterc_if_destroy_fp_t - Funtion prototype to disconnect the interface
 *
 * @handle
 *
 * Returns: -
 *
 **/
typedef void (*sterc_if_disconnect_fp_t) (int handle);

/*
 * sterc_if_resume- prototype for reporting that the if can resume its actions
 *
 * @handle   if handler
 * @result   resultcode
 *
 * Returns: -
 *
 **/
typedef void (*sterc_if_resume_fp_t)(int handle, sterc_result_t result);


/*
 * Control block that holds the function pointers for a sterc interface
 *
 **/
typedef struct
{
  sterc_if_create_fp_t create;
  sterc_if_destroy_fp_t destroy;
  sterc_if_connect_fp_t connect;
  sterc_if_disconnect_fp_t disconnect;
  sterc_if_resume_fp_t resume;
} sterc_if_ctrl_block_t;




#endif /* STERC_IF_H */
