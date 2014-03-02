/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control handler
 */

#ifndef PSCC_HANDLER_H
#define PSCC_HANDLER_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "pscc_msg.h"
#include "mpl_param.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * pscc_get_param_fp_t
 *
 * @connid - connection id
 * @parmaid - parameter id
 *
 * Returns: pointer to parameter element, NULL on error
 *
 **/
typedef mpl_param_element_t* (*pscc_get_param_fp_t)(int connid,pscc_paramid_t paramid);

/**
 * pscc_set_param_fp_t
 *
 * @connid - connection id
 * @param  - parameter to set
 *
 * Returns: 0 on sucess, -1 on error
 *
 **/
typedef int (*pscc_set_param_fp_t)(int connid, mpl_param_element_t *param);

/**
 * pscc_delete_param_fp_t
 *
 * @connid - connection id
 * @parmaid - parameter id
 *
 * Returns: 0 on sucess, -1 on error
 *
 **/
typedef void (*pscc_delete_param_fp_t)(int connid, pscc_paramid_t id);

/**
 * pscc_rc_t - struct for holding resource control parameters
 *
 * @data_p        Pointer do resource control data
 * @length        Lenght of resource control data
 * @seq_id        Sequence ID of the resource control
 *
 **/
typedef struct
{
  uint8_t*       data_p;
  uint16_t       length;
  uint8_t        seq_id;
} pscc_rc_t;

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * pscc_handler - handle client requests
 *
 * @req      request message, packed format
 * @reqlen   length of request message
 * @resp     responsen message to be sent back to client, packed format
 * @resplen  length of responsen message
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int pscc_handler(char *req, size_t reqlen, char *resp, size_t *resplen);

/**
 * pscc_get_param - get parameter
 *
 * @connid   connection id
 * @id       parameter id
 *
 * Returns: pointer to parameter element, NULL if parameter not found or on error
 *
 **/
mpl_param_element_t* pscc_get_param(int connid,pscc_paramid_t id);

#endif /* PSCC_HANDLER_H */
