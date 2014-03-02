/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FsmEvent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _FSM_EVENT_H_
#define _FSM_EVENT_H_

#include "ENS_Redefine_Class.h"

typedef enum {
    // specific signals
    FSM_ENTRY_FSMSIG,
    FSM_EXIT_FSMSIG,
    // start of user defined signals
    FSM_USER_FSMSIG
} FSM_FSMSIGnal;

typedef struct {
    FSM_FSMSIGnal signal;
} FsmEvent;


#endif // _FSM_EVENT_H_
