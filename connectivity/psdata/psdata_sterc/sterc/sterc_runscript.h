/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Routing Control run script
 */

#ifndef STERC_RUNSCRIPT_H
#define STERC_RUNSCRIPT_H

/********************************************************************************
 * Include Files
 ********************************************************************************/
#include "sterc_handler.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
extern char* sterc_runscript_path_connect;
extern char* sterc_runscript_path_disconnect;

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * sterc_runscript_call
 *
 * @parentid
 * @get_param_cb 
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int sterc_runscript_call(int parentid,
                         sterc_get_param_fp_t get_param_cb,
                         sterc_handler_state_t state);

#endif
