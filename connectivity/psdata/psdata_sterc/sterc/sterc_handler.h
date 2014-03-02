/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Routing Control
 */

#ifndef STERC_HANDLER_H
#define STERC_HANDLER_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "mpl_param.h"
#include "sterc_msg.h"
#include "libstecom.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

#define STERC_HANDLER_STATES                  \
  STERC_HANDLER_STATE_ELEMENT(idle)           \
  STERC_HANDLER_STATE_ELEMENT(created)        \
  STERC_HANDLER_STATE_ELEMENT(connecting)     \
  STERC_HANDLER_STATE_ELEMENT(connected)      \
  STERC_HANDLER_STATE_ELEMENT(disconnecting)

#define STERC_HANDLER_STATE_ELEMENT(STATE) sterc_handler_##STATE,
typedef enum
{
  STERC_HANDLER_STATES
  sterc_handler_number_of_states
} sterc_handler_state_t;
#undef STERC_HANDLER_STATE_ELEMENT

#define STERC_HANDLER_STATE_ELEMENT(STATE) #STATE,
static const char* sterc_handler_state_names[] =
{
  STERC_HANDLER_STATES
};
#undef STERC_HANDLER_STATE_ELEMENT

/**
 * sterc_get_param_fp_t
 *
 * @parentid   - parent id
 * @parmid     - parameter id
 *
 * Returns: pointer to parameter element, NULL on error
 *
 **/
typedef mpl_param_element_t* (*sterc_get_param_fp_t)(int parentid, sterc_paramid_t paramid, char* key);

/**
 * sterc_set_param_fp_t
 *
 * @parentid    - parent id
 * @param       - parameter to set
 *
 * Returns: 0 on sucess, -1 on error
 *
 **/
typedef int (*sterc_set_param_fp_t)(int parentid, mpl_param_element_t *param);

/**
 * sterc_delete_param_fp_t
 *
 * @parentid - parentid id
 * @parmaid  - parameter id
 *
 * Returns: 0 on sucess, -1 on error
 *
 **/
typedef void (*sterc_delete_param_fp_t)(int parentid, sterc_paramid_t paramid);

/**
 * sterc_if_connected_fp_t
 *
 * @parentid - parentid id
 *
 * Returns: -
 *
 **/
typedef void (*sterc_if_connected_fp_t)(int parentid);
/**
 * sterc_if_disconnected_fp_t
 *
 * @parentid - parentid id
 *
 * Returns: -
 *
 **/
typedef void (*sterc_if_disconnected_fp_t)(int parentid);

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * sterc_handler - handle client requests
 *
 * @req      request message, packed format
 * @reqlen   length of request message
 * @resp     responsen message to be sent back to client, packed format
 * @resplen  length of responsen message
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int sterc_handler(struct stec_ctrl *ctrl, char *req, size_t reqlen, char *resp, size_t *resplen);


/**
 * sterc_handler_connect_wan -function that informs the handler that the wan can be connected
 *
 * @handler  sterc_handler id
 *
 * Returns: -
 *
 **/
void sterc_handler_connect_wan(int handler);


#endif /* STERC_HANDLER_H */
