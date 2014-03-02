/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Component_Fsm.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_Index.h"
#include "ENS_Component_Fsm.h"
#include "ENS_Port.h"
#include "ProcessingComponent.h"
#define OMXCOMPONENT "ENS_COMPONENT_FSM"

#include "ENS_Index.h"

#include "osi_trace.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ens_proxy_ENS_Fsm_ENS_Component_FsmTraces.h"
#endif //OST_TRACE_COMPILER_IN_USE

static const unsigned int OMX_ErrorPreemptResourcesRequest = (unsigned int)OMX_ErrorVendorStartUnused;

ENS_Component_Fsm::ENS_Component_Fsm(ENS_Component &ensComp)
    : mEnsComponent(ensComp),
      mTargetState(OMX_StateLoaded)
{
    init(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoaded));
    mTransientState = OMX_StateInvalid;
    
#ifndef BACKWARD_COMPATIBILTY_MODE
    BUFFER_SHARING_ENABLED = true;
#else
    BUFFER_SHARING_ENABLED = false;
#endif
}

OMX_ERRORTYPE
ENS_Component_Fsm::suspendResources(void)
{
    if (mEnsComponent.resourcesSuspended()==true)
        return OMX_ErrorNone;

    OMX_ERRORTYPE err = mEnsComponent.suspendResources();
    if (err == OMX_ErrorNone) {
        mEnsComponent.resourcesSuspended(true);
    }
    return err;
}

OMX_ERRORTYPE
ENS_Component_Fsm::unsuspendResources(void)
{
    if (mEnsComponent.resourcesSuspended()==false)
        return OMX_ErrorNone;

    OMX_ERRORTYPE err = mEnsComponent.unsuspendResources();
    if (err == OMX_ErrorNone) {
        mEnsComponent.resourcesSuspended(false);
    }
    return err;
}

ENS_API_EXPORT OMX_STATETYPE ENS_Component_Fsm::getOMXState()
{   
    if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoaded) ||
        getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleHOST) ||
        getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleDSP) ||
        getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToTestForResources) ||
        getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateCheckRscAvailability)) {
        return OMX_StateLoaded;
    }
    else if ((getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle)) ||
             (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleSuspended))) {
        return OMX_StateIdle;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleToLoaded)) {
        // Check what is the target state
        if (mTargetState==OMX_StateIdle) {
            // Means the transition to Idle as failed maybe due to RM issue
            return OMX_StateLoaded;
        }
        return OMX_StateIdle;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting)) {
        return OMX_StateExecuting;
    }
    else if ((getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause)) ||
             (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePauseSuspended))) {
        return OMX_StatePause;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTestForResources)) {
        return OMX_StateTestForResources;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateWaitForResources)) {
        return OMX_StateWaitForResources;
    }
    else if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient)) {
        OMX_STATETYPE previousState, targetState;
        translateTransientState(&previousState, &targetState);
        return previousState;
    }

    return OMX_StateInvalid;
}

ENS_API_EXPORT OMX_STATETYPE ENS_Component_Fsm::getState()
{
    return this->getState(this->getFsmState());
}


OMX_STATETYPE
ENS_Component_Fsm::getState(const FsmState& st) const
{   
    if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoaded)) {
        return OMX_StateLoaded;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleHOST)) {
        return OMX_StateLoadedToIdleHOST;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleDSP)) {
        return OMX_StateLoadedToIdleDSP;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateCheckRscAvailability)) {
        return OMX_StateCheckRscAvailability;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToTestForResources)) {
        return OMX_StateLoadedToTestForResources;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTestForResources)) {
        return OMX_StateTestForResources;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateWaitForResources)) {
        return OMX_StateWaitForResources;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle)) {
        return OMX_StateIdle;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleSuspended)) {
        return OMX_StateIdleSuspended;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleToLoaded)) {
        return OMX_StateIdleToLoaded;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting)) {
        return OMX_StateExecuting;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause)) {
        return OMX_StatePause;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePauseSuspended)) {
        return OMX_StatePauseSuspended;
    }
    else if (st == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient)) {
        return OMX_StateTransient;
    }
    else {
        return OMX_StateInvalid;
    }
}


//-----------------------------------------------------------------------------
//! \brief method that computes the state of the component from its ports' states 
//!
//! This method updates the state of the component FSM from the current states of 
//! the component's ports
//! 
//! \param currentState the current state that the component FSM is in 
//!
//-----------------------------------------------------------------------------
void ENS_Component_Fsm::updateState(FsmState currentState)
{
    if (currentState == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateInvalid)) {
        // we are already in invalid state. We stay in this state
        // this could be the case if the component is in invalid state, but none port is
        return;
    }
    
    FsmState newState = currentState;
    OMX_U32 loadedPorts = 0;
    OMX_U32 idlePorts = 0;
    OMX_U32 executingPorts = 0;
    OMX_U32 pausedPorts = 0;
    OMX_U32 loadedToIdleHOSTPorts = 0;
    OMX_U32 loadedToIdleDSPPorts = 0;
    OMX_U32 loadedToTestForResources = 0;
    OMX_U32 testForResources = 0;
    OMX_U32 checkRscAvailabilityPorts = 0;
    OMX_U32 waitForResources = 0;
    OMX_U32 idleToLoadedPorts = 0;
    OMX_U32 transientPorts = 0;
    OMX_U32 invalidPorts = 0;    
    OMX_U32 portCount = mEnsComponent.getPortCount();

    for(unsigned int i = 0; i < portCount; i++) {
        switch(portGetState(i)) {
            case OMX_StateLoaded:
                loadedPorts++;
                break;
            case OMX_StateLoadedToIdleHOST:
                loadedToIdleHOSTPorts++;
                break;
            case OMX_StateLoadedToIdleDSP:
                loadedToIdleDSPPorts++;
                break;
            case OMX_StateCheckRscAvailability:
                checkRscAvailabilityPorts++;
                break;
            case OMX_StateIdleToLoaded:
                idleToLoadedPorts++;
                break;
            case OMX_StateIdle:
                idlePorts++;
                break;
            case OMX_StateExecuting:
                executingPorts++;
                break;
            case OMX_StatePause:
                pausedPorts++;
                break;
            case OMX_StateTransient:
                transientPorts++;
                break;
            case OMX_StateInvalid:
                invalidPorts++;
                break;
            case OMX_StateLoadedToTestForResources:
                loadedToTestForResources++;
                break;
            case OMX_StateTestForResources:
                testForResources++;
                break;
            case OMX_StateWaitForResources:
                waitForResources++;
                break;
            default:
                DBC_ASSERT(0);
                break;
        }
    }

    if (executingPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting);
    }
    else if (pausedPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause);
    }
    else if (idlePorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle);
    }
    else if (testForResources == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTestForResources);
    }
    else if (waitForResources == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateWaitForResources);
    }
    else if (loadedToTestForResources == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToTestForResources);
    }
    else if (loadedPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoaded);
    }
    else if (checkRscAvailabilityPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateCheckRscAvailability);
    }
    else if (loadedPorts + loadedToTestForResources == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoaded);
    }
    else if (loadedToTestForResources + testForResources == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToTestForResources);
    }
    else if (loadedToIdleDSPPorts == portCount ||
            idlePorts + loadedToIdleDSPPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleDSP);
    }
    else if (loadedToIdleDSPPorts + loadedToIdleHOSTPorts == portCount ||
            idlePorts + loadedToIdleHOSTPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleHOST);
    }
    else if (checkRscAvailabilityPorts + loadedToIdleHOSTPorts == portCount ) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleHOST);
    }
    else if (checkRscAvailabilityPorts + loadedToIdleDSPPorts == portCount ) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleDSP);
    }
    else if (loadedPorts + idleToLoadedPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleToLoaded);
    }
    else if (transientPorts == portCount) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient);
    }
    else if (invalidPorts != 0) {
        newState = static_cast<FsmState>(&ENS_Component_Fsm::OmxStateInvalid);
    }

    if (newState != currentState) {
        stateTransition(newState);
    }
}

OMX_ERRORTYPE ENS_Component_Fsm::dispatchEvtOnPorts(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error;

    for(unsigned int i = 0; i < mEnsComponent.getPortCount(); i++) {
        error = getPortFsm(i)->dispatch(evt);
        if (error != OMX_ErrorNone) {
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: dispatchEvtOnPorts error: %d", error);
            // OMX_ErrorSameState and OMX_ErrorIncorrectStateTransition must be returned using an EventHandler
            if (error==OMX_ErrorSameState || error==OMX_ErrorIncorrectStateTransition) {
                eventHandlerCB(OMX_EventError, error, 0);
                return OMX_ErrorNone;
            } else {
                return error;
            }
        }
    }
    
    return OMX_ErrorNone;
}

ENS_API_EXPORT void ENS_Component_Fsm::dspEventHandler(
        OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2)
{
    OMX_ERRORTYPE error;
    switch (ev) {
        case OMX_EventCmdComplete :
        {     
            MSG1("EventCmdComplete from ProcessingComponent : h=0x%x\n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: EventCmdComplete");

            switch (data1) {
                case OMX_CommandStateSet:
                {
                    MSG1("  ---> OMX_CommandStateSet, state=%d\n", data2);
                    OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandStateSet, state=%d", data2);
                    ENS_CmdSetStateCompleteEvt evt((OMX_STATETYPE) data2);
                    //Dispatch SetStateComplete event on each port.
                    dispatchEvtOnPorts(&evt);
                    //Dispatch SetStateComplete event on component.
                    dispatch(&evt);
                    return;
                }
                case OMX_CommandPortDisable:
                {
                    if(data2 == OMX_ALL) {
                        for(OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++) {
                            //Dispatch disablePortComplete event on each port.
                            MSG1("  ---> OMX_CommandPortDisable, port=%d\n", i);
                            OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandPortDisable, port=%d", i);
                            ENS_FsmEvent evtPort(OMX_DISABLEPORTCOMPLETE_SIG);
                            getPortFsm(i)->dispatch(&evtPort);
                        }
                    } else {
                        MSG1("  ---> OMX_CommandPortDisable, port=%d\n", data2);
                        OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandPortDisable, port=%d", data2);
                        ENS_FsmEvent evtPort(OMX_DISABLEPORTCOMPLETE_SIG);
                        //Dispatch disablePortComplete event on port of index given by data2.
                        ENS_Port_Fsm *portfsm = getPortFsm(data2);
                        DBC_ASSERT(portfsm);
                        portfsm->dispatch(&evtPort);
                    }
                    return;
                }
                case OMX_CommandPortEnable:
                {
                    if(data2 == OMX_ALL) {
                        for(OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++) {
                            //Dispatch enablePortComplete event on each port.
                            MSG1("  ---> OMX_CommandPortEnable, port=%d\n", i);
                            OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandPortEnable, port=%d", i);
                            ENS_FsmEvent evt(OMX_ENABLEPORTCOMPLETE_SIG);
                            getPortFsm(i)->dispatch(&evt);
                        }
                    } else {
                        MSG1("  ---> OMX_CommandPortEnable, port=%d\n", data2);
                        OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandPortEnable, port=%d", data2);

                        ENS_FsmEvent evt(OMX_ENABLEPORTCOMPLETE_SIG);
                        //Dispatch enablePortComplete event on port of index given by data2.
                        ENS_Port_Fsm *portfsm = getPortFsm(data2);
                        DBC_ASSERT(portfsm);
                        portfsm->dispatch(&evt);
                    }
                    return;
                }
                case OMX_CommandFlush:
                {
                    if(data2 == OMX_ALL) {
                        for(OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++) {
                            //Dispatch flushPortComplete event on each port.
                            MSG1("  ---> OMX_CommandFlush, port=%d\n", i);
                            OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandFlush, port=%d", i);
                            ENS_FsmEvent evt(OMX_FLUSHPORTCOMPLETE_SIG);
                            getPortFsm(i)->dispatch(&evt);
                        }
                    } else {
                        MSG1("  ---> OMX_CommandFlush, port=%d\n", data2);
                        OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_CommandFlush, port=%d", data2);
                        ENS_FsmEvent evt(OMX_FLUSHPORTCOMPLETE_SIG);
                        //Dispatch flushPortComplete event on port of index given by data2.
                        ENS_Port_Fsm *portfsm = getPortFsm(data2);
                        DBC_ASSERT(portfsm);
                        portfsm->dispatch(&evt);
                    }
                    return;
                }
                default:
                break;
            }
            break;
        }
        case OMX_EventCmdReceived :
        {
            OstTraceFiltInst2(TRACE_OMX_API, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_EventCmdReceived data1=%d data2=%d", data1, data2);
            if((data1==OMX_CommandPortDisable) || (data1==OMX_CommandPortEnable)) {
                //Special case for disablePort/enablePort to manage OMX_ALL + standardTunneling/nonTunneling cases.                
                if(data2 == OMX_ALL) {
                    //Send one CmdReceived event for each port
                    for(OMX_U32 i = 0; i < mEnsComponent.getPortCount(); i++) {
                        if(mEnsComponent.getPort(i)->useProprietaryCommunication()) {
                            //Only forward event to ENSWrapper in case of proprietary tunneling.
                            //Otherwise this event has already been sent by proxy during OMX_SendCommand itself.
                            error = eventHandlerCB(ev, data1, i);
                            if(error != OMX_ErrorNone){
                                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: case OMX_EventCmdReceived error in  eventHandlerCB error=%d", error);
                            }
                        }
                    }
                } else if (mEnsComponent.getPort(data2)->useProprietaryCommunication()){
                    //Only forward event to ENSWrapper in case of proprietary tunneling.
                    //Otherwise this event has already been sent by proxy during OMX_SendCommand itself.
                    error = eventHandlerCB(ev, data1, data2);
                    if(error != OMX_ErrorNone){
                        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: case OMX_EventCmdReceived error2 in  eventHandlerCB error=%d", error);
                    }
                }
            } else if (data1==OMX_CommandFlush){
                //Don't forward OMX_EventCmdReceived event for OMX_CommandFlush to ENSWrapper
                return;
            } else {
                error = eventHandlerCB(ev, data1, data2);
                if(error != OMX_ErrorNone){
                    OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: case OMX_EventCmdReceived error3 in  eventHandlerCB error=%d", error);
                }
            }
            return;
        }
        case  OMX_EventError:
        {  
            MSG1("  ---> OMX_EventError, error=0x%x\n", data1);
            OstTraceFiltInst1(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm:  ---> OMX_EventError, error=0x%x", data1);
            data1 |= 0x80000000;
            break;
        }
        default:
        break;
    }

    error = eventHandlerCB(ev, data1, data2);
    if(error != OMX_ErrorNone){
        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: case OMX_EventCmdReceived error4 in  eventHandlerCB error=%d", error);
    }
}
       

void ENS_Component_Fsm::setTransientState(FsmState currentState, OMX_STATETYPE targetState)
{
    if (currentState == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle)) {
        if (targetState == OMX_StateExecuting) {
            mTransientState = OMX_StateIdleToExecuting;
        } else if (targetState == OMX_StatePause) {
            mTransientState = OMX_StateIdleToPause;
        } else {
            OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: setTransientState from Idle state assert");
            DBC_ASSERT(0);
        }
    } else if (currentState == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting)) {
        if (targetState == OMX_StateIdle) {
            mTransientState = OMX_StateExecutingToIdle;
        } else if (targetState == OMX_StatePause) {
            mTransientState = OMX_StateExecutingToPause;
        } else {
            OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: setTransientState from Executing state assert");
            DBC_ASSERT(0);
        }
    } else if (currentState == static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause)) {
        if (targetState == OMX_StateIdle) {
            mTransientState = OMX_StatePauseToIdle;
        } else if (targetState == OMX_StateExecuting) {
            mTransientState = OMX_StatePauseToExecuting;
        } else {
            OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: setTransientState from Pause state assert");
            DBC_ASSERT(0);
        }
    } else {
        OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: setTransientState assert");
        DBC_ASSERT(0);
    }
}

void ENS_Component_Fsm::translateTransientState(OMX_STATETYPE *previousState, OMX_STATETYPE *targetState)
{
    *previousState = OMX_StateInvalid;
    *targetState = OMX_StateInvalid;
    switch(mTransientState){
        case OMX_StateIdleToExecuting:
            *previousState = OMX_StateIdle;
            *targetState = OMX_StateExecuting;
            break;
        case OMX_StateIdleToPause:
            *previousState = OMX_StateIdle;
            *targetState = OMX_StatePause;
            break;
        case OMX_StateExecutingToIdle:
            *previousState = OMX_StateExecuting;
            *targetState = OMX_StateIdle;
            break;
        case OMX_StateExecutingToPause:
            *previousState = OMX_StateExecuting;
            *targetState = OMX_StatePause;
            break;
        case OMX_StatePauseToIdle:
            *previousState = OMX_StatePause;
            *targetState = OMX_StateIdle;
            break;
        case OMX_StatePauseToExecuting:
            *previousState = OMX_StatePause;
            *targetState = OMX_StateExecuting;
            break;
        default:
            *previousState = OMX_StateInvalid;
            *targetState = OMX_StateInvalid;
            break;
    }
}

OMX_ERRORTYPE ENS_Component_Fsm::setState(const ENS_FsmEvent *evt, FsmState currentState)
{
    // Store the target state
    ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *) evt;
    mTargetState = setStateEvt->targetState();

    return dispatchEvtAndUpdateState(evt,currentState);
}


OMX_ERRORTYPE ENS_Component_Fsm::dispatchEvtAndUpdateState(const ENS_FsmEvent *evt, FsmState currentState)
{
    OMX_ERRORTYPE error;
    
    // Dispatch the evt
    error = dispatchEvtOnPorts(evt);
    if (error != OMX_ErrorNone) {
        OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: dispatchEvtAndUpdateState error: %d", error);
        return error;
    }

    // Update the state and the transient state
    updateState(currentState);

    if (getFsmState() == static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient)){
        //store transient State
        setTransientState(currentState, mTargetState);
    } else {
        mTransientState = OMX_StateInvalid;
    }

    return OMX_ErrorNone;
}

//-----------------------------------------------------------------------------
//! \brief Default Behavior for all states of the component FSM
//!
//! This method implements default behavior for all states of the component FSM.
//! It is intended to be called in the default case of each state handler method.
//! State handlers should handle events only when the behavior is different.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::defaultBehavior(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        case ENTRY_SIG:
            return OMX_ErrorNone;

        case EXIT_SIG:
            return OMX_ErrorNone;

        case OMX_GETPARAMETER_SIG:
            {
                ENS_CmdGetParamEvt *getParamEvt = (ENS_CmdGetParamEvt *) evt;
                return mEnsComponent.getParameter(getParamEvt->getIndex(), getParamEvt->getStructPtr());
            }

        case OMX_SETCONFIG_SIG:
            {
                ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
                OMX_ERRORTYPE err;
                err = mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
                if(err != OMX_ErrorNone) {
                    OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: defaultBehavior SETCONFIG_SIG error: %d", err);
                    return err;
                }
                if ((unsigned long)setConfigEvt->getIndex() == (unsigned long)ENS_IndexConfigTraceSetting) {
                    return OMX_ErrorNone; // do not propagate such a config
                }
                return mEnsComponent.getProcessingComponent().applyConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
            }

        case OMX_GETCONFIG_SIG:
            {
                ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
                OMX_ERRORTYPE err;
                err = mEnsComponent.getProcessingComponent().retrieveConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
                if(err == OMX_ErrorUnsupportedIndex) {
                    return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
                }
                if (err != OMX_ErrorNone) {
                    OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: defaultBehavior GETCONFIG_SIG error: %d", err);
                }
                return err;
            }

        case OMX_GETEXTENSIONIDX_SIG:
            {
                ENS_CmdGetExtIdxEvt *getExtIdxEvt = (ENS_CmdGetExtIdxEvt *) evt;
                return mEnsComponent.getExtensionIndex(getExtIdxEvt->getParamName(), getExtIdxEvt->getIndexTypePtr());
            }

        case OMX_USE_BUFFER_SIG:
            {
                ENS_CmdUseBufferEvt *useBufferEvt = (ENS_CmdUseBufferEvt *) evt;
                ENS_Port_Fsm *portfsm = getPortFsm(useBufferEvt->getPortIdx());
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);
            }

        case OMX_ALLOCATE_BUFFER_SIG:
            {
                ENS_CmdAllocBufferEvt *allocBufferEvt = (ENS_CmdAllocBufferEvt *) evt;
                ENS_Port_Fsm *portfsm = getPortFsm(allocBufferEvt->getPortIdx());
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);
            }

        case OMX_FREE_BUFFER_SIG:
            {
                ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;
                ENS_Port_Fsm *portfsm = getPortFsm(freeBufferEvt->getPortIdx());
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);
            }

        case OMX_EMPTY_THIS_BUFFER_SIG:
            {
                ENS_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ENS_CmdEmptyThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
                ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nInputPortIndex);
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);
            }

        case OMX_FILL_THIS_BUFFER_SIG:
            {
                ENS_CmdFillThisBufferEvt *fillThisBufferEvt = (ENS_CmdFillThisBufferEvt *) evt;
                OMX_BUFFERHEADERTYPE *pBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
                ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nOutputPortIndex);
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);            
            }

       case OMX_EMPTY_BUFFER_DONE_SIG:
            {
                ENS_CmdEmptyBufferDoneEvt *emptyBufferDoneEvt = (ENS_CmdEmptyBufferDoneEvt *) evt;
                OMX_BUFFERHEADERTYPE *pBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
                ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nInputPortIndex);
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);
            }

        case OMX_FILL_BUFFER_DONE_SIG:
            {
                ENS_CmdFillBufferDoneEvt *fillBufferDoneEvt = (ENS_CmdFillBufferDoneEvt *) evt;
                OMX_BUFFERHEADERTYPE *pBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
                ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nOutputPortIndex);
                if (!portfsm) {
                    return OMX_ErrorBadPortIndex;
                }
                return portfsm->dispatch(evt);            
            }

        case OMX_RESRESERVATIONPROCESSED_SIG:
            {
                ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
                // Can occur if component resources were suspended by disabling/enabling ports
                if (mEnsComponent.resourcesSuspended()==true) {
                    if (OMX_TRUE == ackRscEvt->resGranted()) {
                        // Inform RM 
                        error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), 
                                                              mEnsComponent.omxState() ,
                                                              RM_RS_SATISFIED,0);
                        if(error != OMX_ErrorNone) return error;
                        // Let component restore suspended resources
                        return this->unsuspendResources();
                    } else {
                        eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorInsufficientResources, 0);
                        return OMX_ErrorInsufficientResources;
                    }
                } else {
                    // ?? unexpected event
                    OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: defaultBehavior error: incorrect event received");
                    return OMX_ErrorIncorrectStateOperation;
                }
            }
        default:
        {
            OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: defaultBehavior error: incorrect state operation");
            return OMX_ErrorIncorrectStateOperation;
        }
    }
}


//-----------------------------------------------------------------------------
//! \brief State handler for OMX Loaded state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Loaded state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateLoaded(const ENS_FsmEvent *evt) 
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    mEnsComponent.doSpecificInternalState(evt);

    switch (evt->signal()) {
        
    case ENTRY_SIG:
        {
#ifdef HOST_TRACE
            MSG1("OmxStateLoaded ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
#else
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoaded ENTRY_SIG");
#endif
            // Notify the RM
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateLoaded,
                                                  RM_RS_NONE,0);
            if(error != OMX_ErrorNone) DBC_ASSERT(0);
            
            // Notify the client
            // TODO: use previousState info instead 
            if (mTargetState!=OMX_StateIdle) {
                // No notification if target state was Idle and never reached
                error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateLoaded);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);
            }
            
            //Reset the preemption state
            mEnsComponent.setPreemptionState(ENS_Component::Preempt_None);
            return OMX_ErrorNone;
        }

        case EXIT_SIG:
        {
            if(BUFFER_SHARING_ENABLED){
                if(mEnsComponent.isBufferSharingEnabled()) {
                    mEnsComponent.createBufferSharingGraph();
                }
                else {
                    mEnsComponent.disableBufferSharing();
                }
            }                
            return OMX_ErrorNone;
        }

        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (setStateEvt->targetState() == OMX_StateIdle) {
                //send OMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning from loaded to idle state.
                error = eventHandlerCB(OMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle);
                if(error != OMX_ErrorNone) return error;
            }
            return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoaded));
        }

        case OMX_SETPARAMETER_SIG:
        {
            ENS_CmdSetParamEvt *setParamEvt = (ENS_CmdSetParamEvt *) evt;
            return mEnsComponent.setParameter(setParamEvt->getIndex(), setParamEvt->getStructPtr());
        }

        case OMX_SETCONFIG_SIG:
        {
            ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
            return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
        }

        case OMX_GETCONFIG_SIG:
        {
            ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
            return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
        }
            
        case OMX_SET_CALLBACKS_SIG:
        {   
            ENS_CmdSetCbEvt * setCbEvt = (ENS_CmdSetCbEvt *) evt;
            return mEnsComponent.registerILClientCallbacks(setCbEvt->getCbStructPtr(), setCbEvt->getAppDataPtr());
        }
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Idle state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Idle state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateIdle(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error= OMX_ErrorNone;

    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
#ifdef HOST_TRACE
            MSG1("OmxStateIdle ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
#else
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateIdle ENTRY_SIG");
#endif
 
            // Case Idle preempted
            if (mEnsComponent.getPreemptionState() == ENS_Component::Preempt_Total) {
                error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateIdle,
                                                      RM_RS_SUSPENDED,0);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);
                
                error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);

                // The component has been totally preempted by RM, it shall move in loaded state
                // Send the evt OMX_ErrorResourcesLost to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesLost,0);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);
                
                // Move  component to loaded
                ENS_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
                error =  setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
                if(error != OMX_ErrorNone) DBC_ASSERT(0);

                return OMX_ErrorNone;
            }

            // Notify RM
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateIdle,
                                                  RM_RS_SATISFIED,0);
            if(error != OMX_ErrorNone) DBC_ASSERT(0);

            // Notify the client
            ENS_EntryEvt* entryEvt = (ENS_EntryEvt *)evt;
            OMX_STATETYPE prevState = this->getState(entryEvt->previousState());
            if (prevState != OMX_StateIdleSuspended) {
                error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);
            }
            return OMX_ErrorNone;
        }
        
        case EXIT_SIG:
        {
            OMX_STATETYPE fsmState = this->getState();
            if (fsmState == OMX_StateLoaded || fsmState == OMX_StateIdleToLoaded) {
                if ((mEnsComponent.getEnsWrapperCB() != 0) && (mEnsComponent.getEnsWrapperCB()->DestroyENSProcessingPartAsync != 0)) {
                    // Delegate destruction processing to wrapper if possible
                    mEnsComponent.getEnsWrapperCB()->DestroyENSProcessingPartAsync(mEnsComponent.getOMXHandle());
                } else {
                    error = mEnsComponent.getProcessingComponent().destroy();
                    if (error != OMX_ErrorNone){
                        eventHandlerCB(OMX_EventError, (OMX_U32)error, 0);
                        DBC_ASSERT(0);
                    }
                }
            }
            return OMX_ErrorNone;
        }

        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (OMX_StateIdleSuspended == setStateEvt->targetState()) {
                // Move to sub state idle-suspended
                stateTransition(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleSuspended));
                return OMX_ErrorNone;
            }

            if (mEnsComponent.getPreemptionState() == ENS_Component::Preempt_Total) {
                // Component can go in loaded state only
                if (OMX_StateLoaded == setStateEvt->targetState()) {
                    return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
                } else {
                    return OMX_ErrorComponentSuspended;
                }
            } else {
                return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
            }
        }

        case OMX_RELEASERESOURCE_SIG:
        {
            // The component is preempted (fully or partially) by RME
            ENS_CmdReleaseResourceEvt *releaseResourceEvt = (ENS_CmdReleaseResourceEvt *) evt;
            if (OMX_FALSE == releaseResourceEvt->resSuspend()){ 
                // Full Preemption
                // Send evt to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesLost,0);
                if(error != OMX_ErrorNone) return error;
                // Change state to Loaded
                ENS_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
                return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
            } else {
                // Suspension
                // Send evt to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorComponentSuspended,0);
                if(error != OMX_ErrorNone) return error;
                // Synchronous call to release resources
                OMX_ERRORTYPE err = this->suspendResources();
                if (err != OMX_ErrorNone) {
                    return err;
                }
                // Move state to idle-suspended
                ENS_CmdSetStateEvt setStateEvt(OMX_StateIdleSuspended);
                return this->dispatch(&setStateEvt);
            }
        }

        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Executing state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Executing state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateExecuting(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
#ifdef HOST_TRACE
            MSG1("OmxStateExecuting ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());            
#else
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateExecuting ENTRY_SIG");
#endif
             // Notify the RM
             error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateExecuting,
                                                   RM_RS_SATISFIED,0);
             if(error != OMX_ErrorNone) DBC_ASSERT(0);

             // Notify the client
             error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting);
             if(error != OMX_ErrorNone) DBC_ASSERT(0);
             return OMX_ErrorNone;
        }
            
        case OMX_SETSTATE_SIG:
        {
            return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting));
        }

        case OMX_RELEASERESOURCE_SIG:
        {
            ENS_CmdReleaseResourceEvt *releaseResourceEvt = (ENS_CmdReleaseResourceEvt *) evt;
            if (OMX_FALSE == releaseResourceEvt->resSuspend()) { 
                // Full Preemption
                if (mEnsComponent.getPreemptionPolicy()) {
                    // Send preemption request event to the client
                    error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorPreemptResourcesRequest,0);
                    return error;
                } else {
                    // Send evt to the client
                    error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesPreempted,0);
                    if(error != OMX_ErrorNone) return error;

                    // Change state to idle preempted
                    ENS_CmdSetStateEvt setStateEvt(OMX_StateIdle);
                    return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting));
                } 
           } else {
                // Partial suspension
                // Send evt to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorComponentSuspended,0);
                if(error != OMX_ErrorNone) return error;
                // Synchronous call to release resources
                OMX_ERRORTYPE err = this->suspendResources();
                if (err != OMX_ErrorNone) {
                    return err;
                }
                // Move state to pause then pause-suspended        
                ENS_CmdSetStateEvt setStateEvt(OMX_StatePause);
                return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateExecuting));
            }
        }

        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Pause state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Pause state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStatePause(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            MSG1("OmxStatePause ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStatePause ENTRY_SIG");
          
            // Notify RM
            if (mEnsComponent.getPreemptionState() == ENS_Component::Preempt_None) {
                error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StatePause,
                                                      RM_RS_SATISFIED,0);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);
            }

            // Notify the client
            ENS_EntryEvt* entryEvt = (ENS_EntryEvt *)evt;
            OMX_STATETYPE prevState = this->getState(entryEvt->previousState());
            if (prevState != OMX_StatePauseSuspended && prevState != OMX_StatePause) {
                error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StatePause);
                if(error != OMX_ErrorNone) DBC_ASSERT(0);
            }

            if (mEnsComponent.getPreemptionState() == ENS_Component::Preempt_Partial) {
                // Move state to pause-suspended
                ENS_CmdSetStateEvt setStateEvt(OMX_StatePauseSuspended);
                return this->dispatch(&setStateEvt);
            }
            return OMX_ErrorNone;
        }

        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (mEnsComponent.getPreemptionState() == ENS_Component::Preempt_Total) {
                // Can only move to state Idle
                if (OMX_StateIdle == setStateEvt->targetState()) {
                    return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause));
                } else {
                    return OMX_ErrorComponentSuspended;
                }
            }

            if (OMX_StatePauseSuspended == setStateEvt->targetState()) {
                // Move to sub state pause-suspended
                stateTransition(static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePauseSuspended));
                return OMX_ErrorNone;
            }
            return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause));
        }

        case OMX_RELEASERESOURCE_SIG:
        {
            // The component is suspended or preempted
            ENS_CmdReleaseResourceEvt *releaseResourceEvt = (ENS_CmdReleaseResourceEvt *) evt;
            if (OMX_FALSE == releaseResourceEvt->resSuspend()){ 
                // Full suspension
                if (mEnsComponent.getPreemptionPolicy()) {
                    // Send preemption request event to the client
                    return eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorPreemptResourcesRequest,0);
                } else {
                    // Send evt to the client
                    error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesPreempted,0);
                    if(error != OMX_ErrorNone) return error;
                    // Change state to idle preempted
                    ENS_CmdSetStateEvt setStateEvt(OMX_StateIdle);
                    return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause));
                } 
            } else {
                // Partial suspension
                // Send evt to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorComponentSuspended,0);
                if(error != OMX_ErrorNone) return error;
                // Synchronous call to release resources
                OMX_ERRORTYPE err = this->suspendResources();
                if (err != OMX_ErrorNone) {
                    return err;
                }
                // Move state to pause-suspended
                ENS_CmdSetStateEvt setStateEvt(OMX_StatePauseSuspended);
                return this->dispatch(&setStateEvt);
            }
        }

        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Invalid state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Invalid state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateInvalid(const ENS_FsmEvent *evt)
{    
    switch (evt->signal()) {
        
         case ENTRY_SIG:
         {
             MSG1("OmxStateInvalid ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
             OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateInvalid ENTRY_SIG");
             return eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorInvalidState, 0);
         } 
         
         case OMX_FREE_BUFFER_SIG:
         {
             ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;
             ENS_Port_Fsm *portfsm = getPortFsm(freeBufferEvt->getPortIdx());
             if (!portfsm) {
                 return OMX_ErrorBadPortIndex;
             }
             return portfsm->dispatch(evt);
         }
         
         default:
         {
             OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateInvalid error");
             return OMX_ErrorInvalidState;
         }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for the first transient state from Loaded to Idle 
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from Loaded to Idle. This transient state waits for calls
//! to OMX_AllocateBuffer or OMX_UseBuffer on all non-supplier ports of the component.
//! It then transitions to OmxStateLoadedToIdleDsp.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateLoadedToIdleHOST(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error;
    
    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG1("OmxStateLoadedToIdleHOST ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToIdleHOST ENTRY_SIG");
            return OMX_ErrorNone;
        }

        case OMX_SETCONFIG_SIG:
        {
            ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
            return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
        }

        case OMX_GETCONFIG_SIG:
        {
            ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
            return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
        }
        
        case OMX_USE_BUFFER_SIG:
        {
            ENS_CmdUseBufferEvt *useBufferEvt = (ENS_CmdUseBufferEvt *) evt;

            ENS_Port_Fsm *portfsm = getPortFsm(useBufferEvt->getPortIdx());
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToIdleHOST USE_BUFFER_SIG error: %d", error);
                return error;
            }

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleHOST));

            return OMX_ErrorNone;
        }
        
        case OMX_ALLOCATE_BUFFER_SIG:
        {
            ENS_CmdAllocBufferEvt *allocBufferEvt = (ENS_CmdAllocBufferEvt *) evt;

            ENS_Port_Fsm *portfsm = getPortFsm(allocBufferEvt->getPortIdx());
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToIdleHOST ALLOCATE_BUFFER_SIG error: %d", error);
                return error;
            }

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleHOST));

            return OMX_ErrorNone;
        }
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for the second transient state from Loaded to Idle 
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from Loaded to Idle. The component will transition to this
//! state when all its ports are populated. On entry to this state, the processing 
//! component's construct method is called. When the callback from the processing 
//! component is called to tell that the processing component is in Idle state, the 
//! component FSM transitions to OmxStateIdle.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateLoadedToIdleDSP(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error = OMX_ErrorNone;

    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            MSG1("OmxStateLoadedToIdleDSP ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToIdleDSP ENTRY_SIG");

            if ((mEnsComponent.getEnsWrapperCB() != 0) && (mEnsComponent.getEnsWrapperCB()->ConstructENSProcessingPartAsync != 0)) {
                // Delegate construction processing to wrapper if possible
                mEnsComponent.getEnsWrapperCB()->ConstructENSProcessingPartAsync(mEnsComponent.getOMXHandle());
            } else {
                error = mEnsComponent.getProcessingComponent().construct();
                if (error != OMX_ErrorNone){
                    eventHandlerCB(OMX_EventError, (OMX_U32)error, 0);
                    DBC_ASSERT(0);
                }
            }
            return OMX_ErrorNone;
        }
            
        case OMX_SETSTATECOMPLETE_SIG:
        {
            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToIdleDSP));
            return OMX_ErrorNone;
        }

        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for transient state from Idle to Loaded
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from Idle to Loaded and the component has some non-supplier ports.
//! When the component FSM transitions to this state, the processing component's 
//! destroy method has already been called. This state then waits for calls to OMX_FreeBuffer
//! on all non-supplier ports. It then transitions to Loaded state.
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateIdleToLoaded(const ENS_FsmEvent *evt) {

    OMX_ERRORTYPE error;
    
    switch (evt->signal()) {

        case ENTRY_SIG:
        {
            MSG1("OmxStateIdleToLoaded ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateIdleToLoaded ENTRY_SIG");
            return OMX_ErrorNone;
        }
        
        case OMX_FREE_BUFFER_SIG:
        {
            ENS_CmdFreeBufferEvt *freeBufferEvt = (ENS_CmdFreeBufferEvt *) evt;

            ENS_Port_Fsm *portfsm = getPortFsm(freeBufferEvt->getPortIdx());
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateIdleToLoaded FREE_BUFFER_SIG error: %d", error);
                return error;
            }

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdleToLoaded));

            return OMX_ErrorNone;
        }
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for all other transient states
//!
//! This method implements the behavior of the component FSM when the component
//! is transitionning from one state to another and this transition only implies  
//! forwarding the command to the processing component and waiting for the CmdComplete
//! callback
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateTransient(const ENS_FsmEvent *evt) {

    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            OMX_STATETYPE previousState, targetState;
            MSG1("OmxStateTransient ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateTransient ENTRY_SIG");
            translateTransientState(&previousState, &targetState);

            // TODO: ENTRY_SIG shall not return error
            return mEnsComponent.getProcessingComponent().sendCommand(OMX_CommandStateSet, targetState);
        }
            
        case OMX_SETSTATECOMPLETE_SIG:
        {
            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient));
            return OMX_ErrorNone;
        }

        case OMX_EMPTY_THIS_BUFFER_SIG:
        {
            ENS_CmdEmptyThisBufferEvt *emptyThisBufferEvt = (ENS_CmdEmptyThisBufferEvt *) evt;
            OMX_BUFFERHEADERTYPE *pBuffer = emptyThisBufferEvt->getOMXBufferHdrPtr();
            OMX_ERRORTYPE error;

            ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nInputPortIndex);
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateTransient EMPTY_THIS_BUFFER_SIG error: %d", error);
                return error;
            }

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient));

            return OMX_ErrorNone;
        }

        case OMX_FILL_THIS_BUFFER_SIG:
        {
            ENS_CmdFillThisBufferEvt *fillThisBufferEvt = (ENS_CmdFillThisBufferEvt *) evt;
            OMX_BUFFERHEADERTYPE *pBuffer = fillThisBufferEvt->getOMXBufferHdrPtr();
            OMX_ERRORTYPE error;

            ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nOutputPortIndex);
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateTransient FILL_THIS_BUFFER_SIG error: %d", error);
                return error;
            }

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient));

            return OMX_ErrorNone;
        }

        case OMX_EMPTY_BUFFER_DONE_SIG:
        {
            ENS_CmdEmptyBufferDoneEvt *emptyBufferDoneEvt = (ENS_CmdEmptyBufferDoneEvt *) evt;
            OMX_BUFFERHEADERTYPE *pBuffer = emptyBufferDoneEvt->getOMXBufferHdrPtr();
            OMX_ERRORTYPE error;

            ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nInputPortIndex);
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) return error;

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient));

            return OMX_ErrorNone;
        }

        case OMX_FILL_BUFFER_DONE_SIG:
        {
            ENS_CmdFillBufferDoneEvt *fillBufferDoneEvt = (ENS_CmdFillBufferDoneEvt *) evt;
            OMX_BUFFERHEADERTYPE *pBuffer = fillBufferDoneEvt->getOMXBufferHdrPtr();
            OMX_ERRORTYPE error;

            ENS_Port_Fsm *portfsm = getPortFsm(pBuffer->nOutputPortIndex);
            if (!portfsm) {
                return OMX_ErrorBadPortIndex;
            }
            error = portfsm->dispatch(evt);
            if (error != OMX_ErrorNone) return error;

            updateState(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTransient));

            return OMX_ErrorNone;
        }

        default:
        {
            return defaultBehavior(evt);
        }
    }
}

OMX_ERRORTYPE ENS_Component_Fsm::OmxStateTestForResources(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            MSG1("OmxStateTestForResources ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateTestForResources ENTRY_SIG");
            // Notify the RM
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateTestForResources,
                                                  RM_RS_SATISFIED,0);
            if(error != OMX_ErrorNone) DBC_ASSERT(0);

            // Notify the client           
            error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateTestForResources);
            if(error != OMX_ErrorNone) DBC_ASSERT(0);

            return OMX_ErrorNone;
        }
            
        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (setStateEvt->targetState() == OMX_StateIdle) {
                //send OMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning from TestForResources to idle state.
                error = eventHandlerCB(OMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle);
                if(error != OMX_ErrorNone) return error;
            }
            return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTestForResources));
        }

        case OMX_SETCONFIG_SIG:
        {
            ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
            return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
        }

        case OMX_RELEASERESOURCE_SIG:
        {
            ENS_CmdReleaseResourceEvt *releaseResourceEvt = (ENS_CmdReleaseResourceEvt *) evt;
            if (OMX_FALSE == releaseResourceEvt->resSuspend()){
                // Full suspension
                // Send evt to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesLost,0);
                if(error != OMX_ErrorNone) return error;
                // Change state to loaded
                ENS_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
                return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateTestForResources));
            }
            return OMX_ErrorNone;
        }

        case OMX_GETCONFIG_SIG:
        {
            ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
            return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
        }

        case OMX_USE_BUFFER_SIG:
        case OMX_ALLOCATE_BUFFER_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateTransient error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}

OMX_ERRORTYPE ENS_Component_Fsm::OmxStateWaitForResources(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            MSG1("OmxStateWaitForResources ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateWaitForResources ENTRY_SIG");
            // Notify the RM
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateWaitForResources,
                                                  RM_RS_SATISFIED,0);
            if(error != OMX_ErrorNone) DBC_ASSERT(0);
            // Notify the client
            error = eventHandlerCB(OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateWaitForResources);
            if(error != OMX_ErrorNone) DBC_ASSERT(0);
            
            return OMX_ErrorNone;
        }
            
        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (setStateEvt->targetState() == OMX_StateIdle) {
                //send OMX_EventCmdReceived to ENSWrapper : will not be sent by processingComponent when transitionning from OmxStateWaitForResources to idle state.
                error = eventHandlerCB(OMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle);
                if(error != OMX_ErrorNone) return error;
            }
            return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateWaitForResources));
        }

        case OMX_SETCONFIG_SIG:
        {
            ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
            return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
        }

        case OMX_GETCONFIG_SIG:
        {
            ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
            return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
        }
            
        case OMX_USE_BUFFER_SIG:
        case OMX_ALLOCATE_BUFFER_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateWaitForResources error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}


OMX_ERRORTYPE ENS_Component_Fsm::OmxStateLoadedToTestForResources(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            MSG1("OmxStateLoadedToTestForResources ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToTestForResources ENTRY_SIG");
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), 
                                                  mTargetState,
                                                  RM_RS_REQUIRED,
                                                  0);
            if (error != OMX_ErrorNone) DBC_ASSERT(0);
            return error;
        }
            
        case OMX_RESRESERVATIONPROCESSED_SIG:
        {
            OMX_ERRORTYPE err = dispatchEvtAndUpdateState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateLoadedToTestForResources));
            if (err != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToTestForResources RESRESERVATIONPROCESSED_SIG error: %d", err);
                return err;
            }
            
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            if (OMX_FALSE == ackRscEvt->resGranted()) {
                return eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorInsufficientResources, 0);
            }
            return OMX_ErrorNone;
        }

        case OMX_SETCONFIG_SIG:
        {
            ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
            return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
        }

        case OMX_GETCONFIG_SIG:
        {
            ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
            return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
        }
            
        case OMX_USE_BUFFER_SIG:
        case OMX_ALLOCATE_BUFFER_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateLoadedToTestForResources error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}


OMX_ERRORTYPE ENS_Component_Fsm::OmxStateCheckRscAvailability(const ENS_FsmEvent *evt)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    mEnsComponent.doSpecificInternalState(evt);

    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
            MSG1("OmxStateCheckRscAvailability ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateCheckRscAvailability ENTRY_SIG");
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), 
                                                         OMX_StateLoaded,
                                                         RM_RS_REQUIRED,
                                                         0);
            if (error != OMX_ErrorNone) DBC_ASSERT(0);
            return error;
        }
            
        case OMX_RESRESERVATIONPROCESSED_SIG:
        {
            OMX_ERRORTYPE err = dispatchEvtAndUpdateState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateCheckRscAvailability));
            if (err != OMX_ErrorNone) {
                OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateCheckRscAvailability RESRESERVATIONPROCESSED_SIG error: %d", err);
                return err;
            }
            
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            if (OMX_FALSE == ackRscEvt->resGranted()) {
                return eventHandlerCB(OMX_EventError, (OMX_U32)OMX_ErrorInsufficientResources, 0);
            }
            return OMX_ErrorNone;
        }

        case OMX_SETCONFIG_SIG:
        {
            ENS_CmdSetConfigEvt *setConfigEvt = (ENS_CmdSetConfigEvt *) evt;
            return mEnsComponent.setConfig(setConfigEvt->getIndex(), setConfigEvt->getStructPtr());
        }

        case OMX_GETCONFIG_SIG:
        {
            ENS_CmdGetConfigEvt *getConfigEvt = (ENS_CmdGetConfigEvt *) evt;
            return mEnsComponent.getConfig(getConfigEvt->getIndex(), getConfigEvt->getStructPtr());
        }
        
        case OMX_USE_BUFFER_SIG:
        case OMX_ALLOCATE_BUFFER_SIG:
        case OMX_FREE_BUFFER_SIG:
        case OMX_EMPTY_THIS_BUFFER_SIG:
        case OMX_FILL_THIS_BUFFER_SIG:
        case OMX_EMPTY_BUFFER_DONE_SIG:
        case OMX_FILL_BUFFER_DONE_SIG:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateCheckRscAvailability error: incorrect state operation %d", evt->signal());
            return OMX_ErrorIncorrectStateOperation;
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}

//-----------------------------------------------------------------------------
//! \brief State handler for OMX Idle-Suspended state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Idle Suspended state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStateIdleSuspended(const ENS_FsmEvent *evt) {
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
#ifdef HOST_TRACE
            MSG1("OmxStateIdleSuspended ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
#else
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStateIdleSuspended ENTRY_SIG");
#endif
            // Notify RM
            error =  ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StateIdle,
                                                  RM_RS_SUSPENDED,0);
            // This state is reachable from Idle state only.
            // No need to notify client
            if (error != OMX_ErrorNone) DBC_ASSERT(0);
            return error;
        }

        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (OMX_StateIdle == setStateEvt->targetState()) {
                // Move back to idle
                stateTransition(static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
                return OMX_ErrorNone;
            }

            // Component not allowed to go in executing state if suspended
            if (OMX_StateExecuting != setStateEvt->targetState()) {
                return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
            } else {
                OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStateIdleSuspended error: Component suspended");
                return OMX_ErrorComponentSuspended;
            }
        }

        case OMX_RELEASERESOURCE_SIG:
        {
            ENS_CmdReleaseResourceEvt *releaseResourceEvt = (ENS_CmdReleaseResourceEvt *) evt;
            if (OMX_FALSE == releaseResourceEvt->resSuspend()) { 
                // Full suspension. The suspended component is now preempted by RME
                // Send evt to the client
                error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesLost,0);
                if(error != OMX_ErrorNone) return error;
                // Change state to loaded
                ENS_CmdSetStateEvt setStateEvt(OMX_StateLoaded);
                return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStateIdle));
            } else {
                // Component already suspended. Should not occur
                return OMX_ErrorComponentSuspended;
            }
        }

        case OMX_RESRESERVATIONPROCESSED_SIG:
        {
            // The component is unsuspended by RME
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            if (OMX_TRUE == ackRscEvt->resGranted()) {
                // Reset the suspension flag
                mEnsComponent.setPreemptionState(ENS_Component::Preempt_None);
                OMX_ERRORTYPE err = this->unsuspendResources();
                if (err != OMX_ErrorNone) {
                    return err;
                }
                // Notify the client and send OMX_EventComponentResumed event
                error = eventHandlerCB(OMX_EventComponentResumed, 0, 0);
                if(error != OMX_ErrorNone) return error;
                ENS_CmdSetStateEvt setStateEvt(OMX_StateIdle);
                return this->dispatch(&setStateEvt);
            }
            return OMX_ErrorNone;
        }
        
        default:
        {
            return defaultBehavior(evt);
        }
    }
}


//-----------------------------------------------------------------------------
//! \brief State handler for OMX Pause-Suspended state
//!
//! This method implements the behavior of the component FSM when the component
//! is in the OMX Pause-Suspended state. 
//!
//! \param evt the event to process 
//! \return OMX_ERRORTYPE
//!
//-----------------------------------------------------------------------------
OMX_ERRORTYPE ENS_Component_Fsm::OmxStatePauseSuspended(const ENS_FsmEvent *evt) {
    OMX_ERRORTYPE error = OMX_ErrorNone;
    switch (evt->signal()) {
        
        case ENTRY_SIG:
        {
#ifdef HOST_TRACE
            MSG1("OmxStatePauseSuspended ENTRY_SIG h=0x%x \n", mEnsComponent.getOMXHandle());
#else
            OstTraceFiltInst0(TRACE_FLOW, "ENS_PROXY: ENS_Component_Fsm: OmxStatePauseSuspended ENTRY_SIG");
#endif
 
            // Notify the RM
            error = ENS_HwRm::HWRM_Notify_NewStatesOMXRES(mEnsComponent.getOMXHandle(), OMX_StatePause,
                                                  RM_RS_SUSPENDED,0);
            
            // This state is reachable from Pause state only.
            // No need to notify client
            if (error != OMX_ErrorNone) DBC_ASSERT(0);
            return error;
        }
            
        case OMX_SETSTATE_SIG:
        {
            ENS_CmdSetStateEvt *setStateEvt = (ENS_CmdSetStateEvt *)evt;
            if (OMX_StatePause == setStateEvt->targetState()) {
                // Move back to pause
                stateTransition(static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause));
                return OMX_ErrorNone;
            }

            // Component not allowed to move from suspended to executing
            if (OMX_StateExecuting == setStateEvt->targetState()) {
                OstTraceFiltInst0(TRACE_ERROR, "ENS_PROXY: ENS_Component_Fsm: OmxStatePauseSuspended error: Component suspended");
                return OMX_ErrorComponentSuspended;
            } else {
                return setState(evt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause));
            }
        }

        case OMX_RELEASERESOURCE_SIG:
        {
            ENS_CmdReleaseResourceEvt *releaseResourceEvt = (ENS_CmdReleaseResourceEvt *) evt;
            if (OMX_FALSE == releaseResourceEvt->resSuspend()) { 
                // Full suspension. The suspended component is now preempted by RME
                if (mEnsComponent.getPreemptionPolicy()) {
                    // Send preemption request event to the client
                    return eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorPreemptResourcesRequest,0);
                } else {
                    // Change state to idle preempted
                    error = eventHandlerCB(OMX_EventError,(OMX_U32)OMX_ErrorResourcesPreempted,0);
                    if(error != OMX_ErrorNone) return error;
                    ENS_CmdSetStateEvt setStateEvt(OMX_StateIdle);
                    return setState(&setStateEvt, static_cast<FsmState>(&ENS_Component_Fsm::OmxStatePause));
                } 
            } else {
                // Component already suspended. Should not occur
                return OMX_ErrorComponentSuspended;
            }
        }

        case OMX_RESRESERVATIONPROCESSED_SIG:
        {
            // The component is unsuspended by RME
            ENS_CmdResReservationProcessedEvt *ackRscEvt = (ENS_CmdResReservationProcessedEvt *) evt;
            if (OMX_TRUE == ackRscEvt->resGranted()) {
                // Reset the suspension flag
                mEnsComponent.setPreemptionState(ENS_Component::Preempt_None);
                // Re-acquire lost resources
                OMX_ERRORTYPE err = this->unsuspendResources();
                if (err != OMX_ErrorNone) {
                    return err;
                }
                // Notify the client and send OMX_EventComponentResumed event
                error = eventHandlerCB(OMX_EventComponentResumed, 0, 0);
                if(error != OMX_ErrorNone) return error;
                ENS_CmdSetStateEvt setStateEvt(OMX_StatePause);
                return this->dispatch(&setStateEvt);
            }
            return OMX_ErrorNone;
        }

        default:
        {
            return defaultBehavior(evt);
        }
    }
}
