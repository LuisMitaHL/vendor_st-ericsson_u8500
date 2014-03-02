/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   FSM.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _FSM_H_
#define _FSM_H_

#include "ENS_Redefine_Class.h"

#include "ENS_macros.h"
#include "OMX_Types.h"
#include "TraceObject.h"

#include <stdbool.h>
#include <armnmf_dbc.h>
#include "FsmEvent.h"

class FSM ;
typedef void (FSM::*FSM_State)(const FsmEvent *);

class FSM: public TraceObject
{

public:
	ENS_API_IMPORT FSM() ;
	ENS_API_IMPORT virtual ~FSM() = 0 ;
	
	ENS_API_IMPORT void init(FSM_State initialState);
	ENS_API_IMPORT void dispatch(const FsmEvent *evt);


	FSM_State getState() { 
		ARMNMF_DBC_PRECONDITION(!this->dispatching);
		return this->state;
	} /* getState */


#ifndef NDBC
	bool isDispatching() { return this->dispatching ; }
#endif
	
protected:
	FSM_State state;

private:
#ifndef NDBC
    bool dispatching;
#endif

}; /* FSM */

#endif // _FSM_H_

