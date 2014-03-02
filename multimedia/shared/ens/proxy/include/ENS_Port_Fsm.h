/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Port_Fsm.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef ENS_PORT_FSM_H
#define ENS_PORT_FSM_H

#include "ENS_Redefine_Class.h"

#include "ENS_Port.h" 
#include "ENS_Fsm.h"
#include "ENS_FsmEvent.h"

class ENS_Port_Fsm: public ENS_Fsm {
    public:
        ENS_Port_Fsm(ENS_Port &port) : mPort(port), mDisabledState(OMX_StateInvalid),
            mWaitBuffersFromNeighbor(OMX_FALSE), mWaitBuffersToNeighbor(OMX_FALSE), mEventAppliedOnEachPorts(OMX_FALSE) {
            init(static_cast<FsmState>(&ENS_Port_Fsm::OmxStateLoaded));

            #ifndef BACKWARD_COMPATIBILTY_MODE
                BUFFER_SHARING_ENABLED = true;
            #else
                BUFFER_SHARING_ENABLED = false;
            #endif

        }

        ENS_API_IMPORT OMX_STATETYPE getState(void);

        ENS_API_IMPORT void emptybufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
        ENS_API_IMPORT void fillbufferdone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

    private:
        ENS_Port            &mPort;
        OMX_STATETYPE       mDisabledState;
        OMX_BOOL            mWaitBuffersFromNeighbor;
        OMX_BOOL            mWaitBuffersToNeighbor;
        OMX_BOOL            mEventAppliedOnEachPorts;
        
        // default behavior for all states of the FSM
        OMX_ERRORTYPE defaultBehavior(const ENS_FsmEvent *);

        // Main OMX IL States
        OMX_ERRORTYPE OmxStateLoaded(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateIdle(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateExecuting(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStatePause(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateInvalid(const ENS_FsmEvent *);

        // Transient States
        OMX_ERRORTYPE OmxStateLoadedToIdleHOST(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateLoadedToIdleDSP(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateIdleToLoaded(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateTransientToIdle(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateTransient(const ENS_FsmEvent *);

        OMX_ERRORTYPE OmxStateTransientToDisableWaitBuffersFromNeighbor(const ENS_FsmEvent *evt);
        OMX_ERRORTYPE OmxStateTransientToDisableWaitForFreeBuffers(const ENS_FsmEvent *evt);
		OMX_ERRORTYPE OmxStateTransientToDisableDSP(const ENS_FsmEvent *evt);
        OMX_ERRORTYPE OmxStateTransientToEnableHOST(const ENS_FsmEvent *evt);
        OMX_ERRORTYPE OmxStateTransientToEnableDSP(const ENS_FsmEvent *evt);
        OMX_ERRORTYPE OmxStateDisabled(const ENS_FsmEvent *evt);
        
        OMX_ERRORTYPE OmxStateLoadedToTestForResources(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateTestForResources(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateWaitForResources(const ENS_FsmEvent *);
        OMX_ERRORTYPE OmxStateCheckRscAvailability(const ENS_FsmEvent *);

        OMX_ERRORTYPE goLoadedToIdle(void);
        OMX_ERRORTYPE goIdleToLoaded(void);
        OMX_ERRORTYPE goToEnable(OMX_U32 portIndex);
        OMX_ERRORTYPE goToDisable(OMX_STATETYPE currentState, OMX_U32 portIndex);
        FsmState getFsmParentState(OMX_STATETYPE parentState);

        /// Standard Tunneling Initialization :
        /// Each supplier port shall allocate and pass its buffers to the non-
        /// supplier port it is tunneling.
        /// @return OMX_ERRORTYPE
        OMX_ERRORTYPE checkStandardTunnelingInit(void);

        /// Standard Tunneling De-Initialization :
        /// Each supplier port shall call FreeBuffer method on its tunneled Component 
        /// and frees its buffers.
        /// @return OMX_ERRORTYPE
        OMX_ERRORTYPE checkStandardTunnelingDeInit(void);

        /// Copies OMX_BUFFERHEADERTYPE fields from source buffer to destination buffer.
        /// It is used in buffer sharing in which we need to map buffers between two connected ports.
        void copyOMXBufferHeaderFields(OMX_BUFFERHEADERTYPE *pSrcBuffer,OMX_BUFFERHEADERTYPE *pDstBuffer);

        inline OMX_ERRORTYPE eventHandlerCB(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2){
            ENS_Component &enscomp = mPort.getENSComponent();
            return enscomp.eventHandlerCB(ev, data1, data2, 0);
        }
        
        ProcessingComponent & getProcessingComponent();

     public:
        //Note: This flag will be removed in future
        bool BUFFER_SHARING_ENABLED;
        void bufferSharingEnabled(){
                BUFFER_SHARING_ENABLED = true;
        }


};

#endif // ENS_PORT_FSM_H
