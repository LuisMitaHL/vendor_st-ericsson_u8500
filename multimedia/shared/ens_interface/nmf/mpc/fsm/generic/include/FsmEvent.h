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

typedef enum {
    // specific signals
    FSM_ENTRY_SIG,
    FSM_EXIT_SIG,
    // start of user defined signals
    FSM_USER_SIG
} FSM_Signal;

typedef struct {
    FSM_Signal signal;
} FsmEvent;

#endif // _FSM_EVENT_H_
