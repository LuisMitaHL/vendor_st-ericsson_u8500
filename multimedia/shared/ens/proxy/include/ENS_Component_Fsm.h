/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Component_Fsm.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef ENS_COMPONENT_FSM_H
#define ENS_COMPONENT_FSM_H

#include "ENS_Redefine_Class.h"

#include "OMX_Component.h"
#include "OMX_Core.h"
#include "ENS_Component.h"
#include "ENS_Port_Fsm.h"
#include "ENS_Fsm.h"
#include "ENS_FsmEvent.h"

class ENS_Component_Fsm: public ENS_Fsm {
    public:
        ENS_Component_Fsm(ENS_Component &ensComp);

        ENS_API_IMPORT  void dspEventHandler(OMX_EVENTTYPE ev, OMX_U32 data1, OMX_U32 data2);

        ENS_API_IMPORT  OMX_STATETYPE getOMXState();

        ENS_API_IMPORT  OMX_STATETYPE getState();

    private:
        ENS_Component & mEnsComponent;
        OMX_STATETYPE   mTransientState;
        OMX_STATETYPE   mTargetState;
        
        // default behavior for all states
        OMX_ERRORTYPE defaultBehavior(const ENS_FsmEvent *);
        
        // Main OMX IL States
        OMX_ERRORTYPE OmxStateLoaded(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateIdle(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateExecuting(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStatePause(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateInvalid(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateTestForResources(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateWaitForResources(const ENS_FsmEvent *);
 
       // OMX IL RM Sub-States
        OMX_ERRORTYPE OmxStateIdleSuspended(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStatePauseSuspended(const ENS_FsmEvent *);

        // Transient States
        OMX_ERRORTYPE OmxStateLoadedToIdleHOST(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateLoadedToIdleDSP(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateIdleToLoaded(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateTransient(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateLoadedToTestForResources(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateCheckRscAvailability(const ENS_FsmEvent *);

        OMX_ERRORTYPE setState(const ENS_FsmEvent *, FsmState currentState);
        OMX_ERRORTYPE dispatchEvtAndUpdateState(const ENS_FsmEvent *, FsmState currentState);
        void updateState(FsmState currentState);
        void setTransientState(FsmState currentState, OMX_STATETYPE targetState);
        void translateTransientState(OMX_STATETYPE *previousState, OMX_STATETYPE *targetState);
        OMX_ERRORTYPE goIdleToLoaded(void);
        OMX_ERRORTYPE dispatchEvtOnPorts(const ENS_FsmEvent *evt);
        
        inline OMX_ERRORTYPE eventHandlerCB(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) const;

        inline OMX_BOOL isPortEnabled(const OMX_U32 portidx) const;

        inline ENS_Port_Fsm * getPortFsm(const OMX_U32 portIdx) const;

        inline OMX_STATETYPE portGetState(const OMX_U32 portIdx) const;

        OMX_STATETYPE
        getState(const FsmState& st) const;

 
     public:
	//Note: This flag will be removed in future
	bool BUFFER_SHARING_ENABLED;
	void bufferSharingEnabled(){
	    BUFFER_SHARING_ENABLED = true;
	}

private:
        OMX_ERRORTYPE
        suspendResources(void);
 
        OMX_ERRORTYPE
        unsuspendResources(void);
};

#include "ENS_Component_Fsm.inl"

#endif // ENS_COMPONENT_FSM_H
