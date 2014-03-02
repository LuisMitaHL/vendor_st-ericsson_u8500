/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   OmxEvent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _OMX_EVENT_H_
#define _OMX_EVENT_H_

#include "ENS_Redefine_Class.h"

#include "FsmEvent.h"

#include <inc/typedef.h>
#include <armnmf_buffer.idt.h>
#include <omxstate.idt.h>

typedef enum {
    OMX_PROCESS_FSMSIG = FSM_USER_FSMSIG,
    OMX_RETURNBUFFER_FSMSIG,
    OMX_FLUSH_FSMSIG,
    OMX_SETSTATE_FSMSIG,
    OMX_DISABLE_PORT_FSMSIG,
    OMX_ENABLE_PORT_FSMSIG,
    OMX_LAST_FSMSIG
} OmxSignal;

typedef struct {
    FsmEvent    fsmEvent;
    union {
        struct {
            OMX_BUFFERHEADERTYPE_p    buffer;
            int         portIdx;
        } returnBuffer;

        struct {
            OMX_STATETYPE   state;
        } setState;

        struct {
            unsigned  int  portIdx;
        } portCmd;
    } args;
} OmxEvent;


static inline int OmxEvent_FSMSIGnal(const OmxEvent *evt) { return evt->fsmEvent.signal; }
#endif // _OMX_EVENT_H_
