/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* brief   This file contains the methods needed to instantiate the ISPCTL DSP NMF FW component. It depends on NMF Component Manager API
 *       It :
 *           - builds the NMF network to communicate with ISPCTL
 *           - provides the interfaces needed to communicate with ISPCTL
 *           - implements the callback functions binded on ISPTCL Alert/infos/debug functions.
 */

#undef DBGT_LAYER
#define DBGT_LAYER 3
#undef DBGT_PREFIX
#define DBGT_PREFIX "SMGT"

#include "ext_omx_state_manager.h"

/* for traces */
#undef OMX_TRACE_UID
#define OMX_TRACE_UID 0x8

COmxStateMgr::COmxStateMgr()
{
    stateCounter = 0;
    state[0] = OMX_StateMax;
}

OMX_STATETYPE COmxStateMgr::updateState()
{
    OMX_STATETYPE returnedState;

    if(stateCounter>2) DBC_ASSERT(0); //this cannot ocur.


    if(state[0] != OMX_StateMax) {
        stateCounter--;


        returnedState = state[0];
        state[0] = OMX_StateMax;


        return returnedState;
    }
    else return OMX_StateMax;
}


OMX_ERRORTYPE COmxStateMgr::changeState(OMX_STATETYPE stateTobeUpdated)
{

    state[stateCounter] = stateTobeUpdated;
    stateCounter++;
    return OMX_ErrorNone;
}
