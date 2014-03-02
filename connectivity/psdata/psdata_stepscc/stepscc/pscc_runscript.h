/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control run script
 */


#ifndef PSCC_RUNSCRIPT_H
#define PSCC_RUNSCRIPT_H

/********************************************************************************
 * Include Files
 ********************************************************************************/
#include "pscc_object.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
extern char* pscc_runscript_path;

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * pscc_runscript_call
 *
 * @obj_p - pointer to connectin object
 *
 * Returns: 0 on success, -1 on error
 *
 **/
int pscc_runscript_call(pscc_object_t* obj_p);

#endif
