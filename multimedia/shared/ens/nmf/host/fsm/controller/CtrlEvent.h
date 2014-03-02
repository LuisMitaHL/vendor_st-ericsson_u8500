/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CtrlEvent.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CTRL_EVENT_H_
#define _CTRL_EVENT_H_

#include "ENS_Redefine_Class.h"

#include "OmxEvent.h"

#include "omxcommand.idt.h"
#include "omxevent.idt.h"

typedef enum {
    CTRL_EVENT_FSMSIG = OMX_LAST_FSMSIG ,
    CTRL_NEIGHBOUR_EVENT_FSMSIG,
    CTRL_LAST_FSM_SIG
} CtrlSignal;

typedef struct {
	FsmEvent    fsmEvent;
    
	union {
		struct {
			OMX_COMMANDTYPE  cmd;
			t_uword param ; 
		} cmd ; // defined if signal != CTRL_EVENT_FSMSIG ;

		struct {
			OMX_EVENTTYPE  event; 
			t_uint32 	   data1;
			t_uint32       data2;
			t_uint8        id;
		} ev ; // defined if signal == CTRL_EVENT_FSMSIG ;
	} args ;
} CtrlEvent;

static inline int CtrlEvent_FSMSIGnal(const CtrlEvent *evt) { return evt->fsmEvent.signal; }
#endif // _CTRL_EVENT_H_
