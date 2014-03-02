
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Fsm.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_Fsm_H_
#define _ENS_Fsm_H_

#include "ENS_Redefine_Class.h"

#include "ENS_macros.h"
#include "TraceObject.h"

#undef OMX_TRACE_UID
#define OMX_TRACE_UID 4
#include "OMX_Core.h"

class ENS_Fsm;
class ENS_FsmEvent;

typedef OMX_ERRORTYPE (ENS_Fsm::*FsmState) (const ENS_FsmEvent *);

// Abstract base class for Finite State Machines
class ENS_Fsm: public TraceObject
{
public:
    ENS_API_IMPORT
    ENS_Fsm();

    ENS_API_IMPORT
    virtual ~ENS_Fsm() = 0;
    
    ENS_API_IMPORT void
    init(FsmState initialState);
    
    ENS_API_IMPORT OMX_ERRORTYPE
    dispatch(const ENS_FsmEvent* event);
    
    FsmState
    getFsmState() const;
    
    void
    stateTransition(FsmState targetState);
    
private:
    FsmState mState;
};


inline FsmState
ENS_Fsm::getFsmState() const
{ return mState; }

inline void
ENS_Fsm::stateTransition(FsmState targetState)
{ mState = targetState; }

#endif // _ENS_Fsm_H_
