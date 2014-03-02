/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiocontroller.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "audiocontroller.hpp"
#include "OMX_Core.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_misc_audiocontroller_audiocontrollerTraces.h"
#endif

AFM_API_EXPORT void hst_misc_audiocontroller::init(t_uint8 slaveCount, Ieventhandler *nmfitf) {
    OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::init");
    ARMNMF_DBC_PRECONDITION(nmfitf) ; // eventHandler interface must NOT be null

    mSlaveCount    = slaveCount;
    mEventHandler  = nmfitf;
    mOmxState = OMX_StateTransient ;
    mSlaveCountBinded = 0;

    mSlavesArray = new slaveInfo_t[mSlaveCount] ;

    if (mSlavesArray == 0) {
        FSM::init((FSM_State) &hst_misc_audiocontroller::invalidState);
        return ;
    }

    for (int i=0; i<mSlaveCount; i++) {
        mSlavesArray[i].itf = 0 ;
        mSlavesArray[i].isBufferSupplier = false;
        mSlavesArray[i].state = OMX_StateInvalid ;
        mSlavesArray[i].remainingCommands = 0 ;
        mSlavesArray[i].remainingReceivedCommands = 0 ;
    }

    // at initialization, the controller cannot switch into idle state
    // before knowing the state of all slaves
    // => start in transient state with OMX_StateIdle as target state
    mOmxState = OMX_StateIdle;
    mNotifyStateTransition = true;
    FSM::init((FSM_State) &hst_misc_audiocontroller::transientState);
}


AFM_API_EXPORT void hst_misc_audiocontroller::setSlave(t_uint8 slaveIndex, Isendcommand * slave, bool isBufferSupplier) {
    ARMNMF_DBC_PRECONDITION(slaveIndex < mSlaveCount) ;
    ARMNMF_DBC_PRECONDITION(slave) ;
    mSlavesArray[slaveIndex].itf = slave ;
    mSlavesArray[slaveIndex].isBufferSupplier = isBufferSupplier ;
    if(!mSlavesArray[slaveIndex].itf->IsNullInterface()) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::setSlave %d", slaveIndex);
        mSlaveCountBinded ++;
    }
}

AFM_API_EXPORT void hst_misc_audiocontroller::sendCommand(OMX_COMMANDTYPE cmd, t_uword param) {
    CtrlEvent ev;

    ev.args.cmd.cmd = cmd ;
    ev.args.cmd.param = param ;

    if (cmd == OMX_CommandStateSet) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::sendCommand OMX_CommandStateSet(state=%d)", param);
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_SETSTATE_FSMSIG;
    }
    else if (cmd == OMX_CommandFlush) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::sendCommand OMX_CommandFlush(portId=%d)", param);
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_FLUSH_FSMSIG;
    }
    else if (cmd == OMX_CommandPortDisable) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::sendCommand OMX_CommandPortDisable(portId=%d)", param);
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_DISABLE_PORT_FSMSIG;
    }
    else if (cmd == OMX_CommandPortEnable) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::sendCommand OMX_CommandPortEnable(portId=%d)", param);
        ev.fsmEvent.signal = (FSM_FSMSIGnal)OMX_ENABLE_PORT_FSMSIG;
    }
    else {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::sendCommand unknown cmd received cmd =%d (portId=%d)",cmd, param);
        ARMNMF_DBC_ASSERT(0);
    }

    //Reset remainingReceivedCommand for each slave before dispatching event
    for (int i=0; i<mSlaveCount; i++) {
        mSlavesArray[i].remainingReceivedCommands = 0 ;
    }


    //Reset remainingCommand for each slave before dispatching event
    for (int i=0; i<mSlaveCount; i++) {
        mSlavesArray[i].remainingCommands = 0 ;
    }

    dispatch((FsmEvent *) &ev);
}

AFM_API_EXPORT void hst_misc_audiocontroller::slavesEventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 id) {
    CtrlEvent ctrlEv ;
    OstTraceFiltInst2(TRACE_DEBUG, "AFM_HOST: hst_misc_audiocontroller::slavesEventHandler ev = %d, data1 = %d", ev, data1);
    OstTraceFiltInst2(TRACE_DEBUG, "AFM_HOST: hst_misc_audiocontroller::slavesEventHandler data2 = %d, id = %d", data2, id);

    ctrlEv.fsmEvent.signal = (FSM_FSMSIGnal) CTRL_EVENT_FSMSIG;

    ctrlEv.args.ev.event = ev ;
    ctrlEv.args.ev.data1 = data1 ;
    ctrlEv.args.ev.data2 = data2 ;
    ctrlEv.args.ev.id    = id ;

    dispatch((FsmEvent *) &ctrlEv) ;
}

AFM_API_EXPORT hst_misc_audiocontroller::~hst_misc_audiocontroller() {
    if (mSlavesArray != 0) delete [] mSlavesArray ;
}

//ctrl dispatch setState command on all slaves
AFM_API_EXPORT void hst_misc_audiocontroller::forwardStateChange(const OMX_STATETYPE targetState) {
    for (int i=0; i < mSlaveCount; i++) {
        if (!mSlavesArray[i].itf->IsNullInterface()) {
            mSlavesArray[i].remainingReceivedCommands = 1;
            mSlavesArray[i].itf->sendCommand(OMX_CommandStateSet, targetState) ;
        }
    }
}

//register a command for a given slave
AFM_API_EXPORT void hst_misc_audiocontroller::waitCommandFromSlave(t_uint8 slaveIndex, OMX_COMMANDTYPE command, t_uint8 count) {
    OstTraceFiltInst3(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitCommandFromSlave slaveIndex = %d, command = %d, remainingCommandsCount = %d", slaveIndex, command, count);

    ARMNMF_DBC_PRECONDITION(slaveIndex < mSlaveCount) ;

    mSlavesArray[slaveIndex].remainingCommands = count ;
    mSlavesArray[slaveIndex].remainingReceivedCommands = count ;
    mSlavesArray[slaveIndex].currentCommand = command ;

}

//ctrl dispatch setState command on all buffer User slaves
void hst_misc_audiocontroller::forwardStateChangeToBufferUser(const OMX_STATETYPE targetState) {
    for (int i=0; i < mSlaveCount; i++) {
        if ((!mSlavesArray[i].itf->IsNullInterface()) &&
                (mSlavesArray[i].isBufferSupplier==false)) {
            mSlavesArray[i].remainingReceivedCommands = 1;
            mSlavesArray[i].itf->sendCommand(OMX_CommandStateSet, targetState) ;
        }
    }
}

//ctrl dispatch setState command on all buffer Supplier slaves
void hst_misc_audiocontroller::forwardStateChangeToBufferSupplier(const OMX_STATETYPE targetState) {
    for (int i=0; i < mSlaveCount; i++) {
        if ((!mSlavesArray[i].itf->IsNullInterface()) &&
                (mSlavesArray[i].isBufferSupplier==true)) {
            mSlavesArray[i].remainingReceivedCommands = 1;
            mSlavesArray[i].itf->sendCommand(OMX_CommandStateSet, targetState) ;
        }
    }
}

//return true if all binded slaves are in targetState
bool hst_misc_audiocontroller::allSlavesInTargetState(const OMX_STATETYPE targetState) {
    int i = 0 ;
    while
        (
         (i<mSlaveCount) &&
         (
          (mSlavesArray[i].itf->IsNullInterface()) ||
          (mSlavesArray[i].state == targetState)
         )
        ) i++ ;
    if (i == mSlaveCount) {
       return true;
    }
    return false;
}

//return true if all binded slaves have executed all the commands
bool hst_misc_audiocontroller::allSlaveCmdExecuted(void) {
    int i = 0 ;
    while ((i<mSlaveCount) && (mSlavesArray[i].remainingCommands == 0)) i++;
    if (i == mSlaveCount) {
       return true;
    }
    return false;
}

//return true if all binded slaves have received all the commands
bool hst_misc_audiocontroller::allSlaveCmdReceived(void) {
    int i = 0 ;
    while ((i<mSlaveCount) && (mSlavesArray[i].remainingReceivedCommands == 0)) i++;
    if (i == mSlaveCount) {
       return true;
    }
    return false;
}

//return true if all buffer User slaves have received OMX_CommandStateSet
bool hst_misc_audiocontroller::allBufferUserSlavesReceivedSetStateCmd(void) {
    int i = 0 ;
    while
        (
         (i<mSlaveCount) &&
         (
          (mSlavesArray[i].itf->IsNullInterface()) ||
          (mSlavesArray[i].isBufferSupplier == true) ||
          (mSlavesArray[i].remainingReceivedCommands == 0)
         )
        ) i++ ;
    if (i == mSlaveCount) {
        return true;
    }
    return false;
}

//return true if all buffer Supplier slaves have received OMX_CommandStateSet
bool hst_misc_audiocontroller::allBufferSupplierSlavesReceivedSetStateCmd(void) {
    int i = 0 ;
    while
        (
         (i<mSlaveCount) &&
         (
          (mSlavesArray[i].itf->IsNullInterface()) ||
          (mSlavesArray[i].isBufferSupplier == false) ||
          (mSlavesArray[i].remainingReceivedCommands == 0)
         )
        ) i++ ;
    if (i == mSlaveCount) {
        return true;
    }
    return false;
}

/* ---------------------- */

void hst_misc_audiocontroller::omxState(const CtrlEvent *evt) {
    switch(CtrlEvent_FSMSIGnal(evt)) {

        case FSM_ENTRY_FSMSIG:
            OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::omxState FSM_ENTRY_FSMSIG Enter state %d", mOmxState);
            if(mNotifyStateTransition) {
                mEventHandler->eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, mOmxState);
            } else {
                mNotifyStateTransition = true;
            }
            break;

        case FSM_EXIT_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::omxState FSM_EXIT_SIG");
            break;

        case OMX_SETSTATE_FSMSIG:
            {
                OMX_STATETYPE currentState = mOmxState;
                mOmxState = (OMX_STATETYPE)evt->args.cmd.param;
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::omxstate OMX_SETSTATE_FSMSIG currentState=%d nextState=%d", currentState, mOmxState);

                if ((currentState == OMX_StateIdle) && (mOmxState == OMX_StateExecuting)){
                    //In case of transition from idle to executing state, buffer user components must receive setStateExec
                    //before buffer supplier components.
                    if(mSlaveCountBinded > 1) {
                        forwardStateChangeToBufferUser(mOmxState);
                        this->state = (FSM_State)&hst_misc_audiocontroller::waitForBufferUserSetStateReceived;
                    } else {
                        forwardStateChange(mOmxState) ;
                        this->state = (FSM_State)&hst_misc_audiocontroller::transientState;
                    }
                } else if ((currentState == OMX_StateExecuting) && (mOmxState == OMX_StateIdle)) {
                    //In case of transition from executing to idle state, buffer supplier components must receive setStateIdle
                    //before buffer user components in order to stop buffer transmission
                    if(mSlaveCountBinded > 1) {
                        forwardStateChangeToBufferSupplier(mOmxState);
                        this->state = (FSM_State)&hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived;
                    } else {
                        forwardStateChange(mOmxState) ;
                        this->state = (FSM_State)&hst_misc_audiocontroller::transientState;
                    }
                } else {
                    forwardStateChange(mOmxState) ;
                    this->state = (FSM_State)&hst_misc_audiocontroller::transientState;
                }
            }
            break;

        case OMX_DISABLE_PORT_FSMSIG:
        case OMX_ENABLE_PORT_FSMSIG:
        case OMX_FLUSH_FSMSIG:
            forwardPortCommand(evt->args.cmd.cmd, evt->args.cmd.param);
            {
                //check if ctrl must go in waitForCommandComplete state
                int i = 0 ;
                while ((i<mSlaveCount) && (mSlavesArray[i].remainingCommands == 0)) i++;
                if (i < mSlaveCount) {
                    //Save context
                    mStandbyInfo.command = evt->args.cmd.cmd ;
                    mStandbyInfo.id = evt->args.cmd.param ;
                    mNotifyStateTransition = false;
                    this->state = (FSM_State)&hst_misc_audiocontroller::waitForCommandCompleteState ;
                }
            }
            break;

        case CTRL_EVENT_FSMSIG:
            if (evt->args.ev.event == OMX_EventError) {
                OstTraceFiltInst3(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::omxState OMX_EventError data1=%d, data2=%d sent by slave %d", evt->args.ev.data1, evt->args.ev.data2, evt->args.ev.id);
                mEventHandler->eventHandler(evt->args.ev.event, evt->args.ev.data1, evt->args.ev.data2);
            }else {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::omxState CTRL_EVENT_FSMSIG unexpected event=%d in state=%d",evt->args.ev.event, mOmxState);
                this->state = (FSM_State) &hst_misc_audiocontroller::invalidState ;
            }


        default:
            OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::omxState Unexpected event event=%d in state=%d",evt->args.ev.event, mOmxState);
            this->state = (FSM_State) &hst_misc_audiocontroller::invalidState ;
            break;
    }
}

void hst_misc_audiocontroller::transientState(const CtrlEvent *evt) {
    switch(CtrlEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::transientState FSM_ENTRY_FSMSIG");
            break;
        case FSM_EXIT_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::transientState FSM_EXIT_FSMSIG");
            break;

        case CTRL_EVENT_FSMSIG:
            if (
                    (evt->args.ev.event == OMX_EventCmdComplete) &&
                    (evt->args.ev.data1 == OMX_CommandStateSet) &&
                    (evt->args.ev.data2 == (unsigned int)mOmxState)
               ) {
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::transientState -> slave %d in state %d", evt->args.ev.id, mOmxState);
                mSlavesArray[evt->args.ev.id].state = mOmxState ;
                if(allSlavesInTargetState(mOmxState)) {
                    this->state = (FSM_State) &hst_misc_audiocontroller::omxState;
                }
            } else if (
                    (evt->args.ev.event == OMX_EventCmdReceived) &&
                    (evt->args.ev.data1 == OMX_CommandStateSet) &&
                    (evt->args.ev.data2 == (unsigned int)mOmxState)
                    ) {
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::transientState -> slave %d has received OMX_CommandStateSet(%d)", evt->args.ev.id, mOmxState);
                mSlavesArray[evt->args.ev.id].remainingReceivedCommands=0;
                if(allSlaveCmdReceived()) {
                    mEventHandler->eventHandler(OMX_EventCmdReceived, OMX_CommandStateSet, mOmxState);
                }
            } else if (evt->args.ev.event == OMX_EventError) {
                OstTraceFiltInst3(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::transientState OMX_EventError data1=%d, data2=%d sent by slave %d", evt->args.ev.data1, evt->args.ev.data2, evt->args.ev.id);
                mEventHandler->eventHandler(evt->args.ev.event, evt->args.ev.data1, evt->args.ev.data2);
            } else {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::transientState CTRL_EVENT_FSMSIG unexpected event=%d in state=%d",evt->args.ev.event, mOmxState);
                this->state = (FSM_State) &hst_misc_audiocontroller::invalidState ;
            }
            break;

        default:
            OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::transientState Unexpected event event=%d in state=%d",evt->args.ev.event, mOmxState);
            ARMNMF_DBC_ASSERT(0);
            break;
    }
}

void hst_misc_audiocontroller::waitForBufferUserSetStateReceived(const CtrlEvent *evt) {

    switch(CtrlEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived FSM_ENTRY_FSMSIG");
            break;
        case FSM_EXIT_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived FSM_EXIT_FSMSIG");
            break;

        case CTRL_EVENT_FSMSIG:
            if (
                    (evt->args.ev.event == OMX_EventCmdReceived) &&
                    (evt->args.ev.data1 == OMX_CommandStateSet) &&
                    (evt->args.ev.data2 == (unsigned int)mOmxState)
               ) {
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived -> slave %d has received OMX_CommandStateSet(%d)", evt->args.ev.id, mOmxState);
                mSlavesArray[evt->args.ev.id].remainingReceivedCommands=0;
                if(allBufferUserSlavesReceivedSetStateCmd()) {
                    forwardStateChangeToBufferSupplier(mOmxState);
                    if(allSlaveCmdReceived()) {
                        mEventHandler->eventHandler(OMX_EventCmdReceived, OMX_CommandStateSet, mOmxState);
                    }
                    this->state = (FSM_State) &hst_misc_audiocontroller::transientState;
                }
            } else if  (
                    (evt->args.ev.event == OMX_EventCmdComplete) &&
                    (evt->args.ev.data1 == OMX_CommandStateSet) &&
                    (evt->args.ev.data2 == (unsigned int)mOmxState)
               ) {
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived -> slave %d in state %d", evt->args.ev.id, mOmxState);
                mSlavesArray[evt->args.ev.id].state = mOmxState ;
            } else if (evt->args.ev.event == OMX_EventError) {
                OstTraceFiltInst3(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived OMX_EventError data1=%d, data2=%d sent by slave %d", evt->args.ev.data1, evt->args.ev.data2, evt->args.ev.id);
                mEventHandler->eventHandler(evt->args.ev.event, evt->args.ev.data1, evt->args.ev.data2);
            } else {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived CTRL_EVENT_FSMSIG unexpected event=%d in state=%d",evt->args.ev.event, mOmxState);
                this->state = (FSM_State) &hst_misc_audiocontroller::invalidState ;
            }
            break;

        default:
            OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForBufferUserSetStateReceived Unexpected event event=%d in state=%d",evt->args.ev.event, mOmxState);
            ARMNMF_DBC_ASSERT(0);
            break;
    }
}

void hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived(const CtrlEvent *evt) {

    switch(CtrlEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived FSM_ENTRY_FSMSIG");
            break;
        case FSM_EXIT_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived FSM_EXIT_FSMSIG");
            break;

        case CTRL_EVENT_FSMSIG:
            if (
                    (evt->args.ev.event == OMX_EventCmdReceived) &&
                    (evt->args.ev.data1 == OMX_CommandStateSet) &&
                    (evt->args.ev.data2 == (unsigned int)mOmxState)
               ) {
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived -> slave %d has received OMX_CommandStateSet(%d)", evt->args.ev.id, mOmxState);
                mSlavesArray[evt->args.ev.id].remainingReceivedCommands=0;
                if(allBufferSupplierSlavesReceivedSetStateCmd()) {
                    forwardStateChangeToBufferUser(mOmxState);
                    if(allSlaveCmdReceived()) {
                        mEventHandler->eventHandler(OMX_EventCmdReceived, OMX_CommandStateSet, mOmxState);
                    }
                    this->state = (FSM_State) &hst_misc_audiocontroller::transientState;
                }
            } else if  (
                    (evt->args.ev.event == OMX_EventCmdComplete) &&
                    (evt->args.ev.data1 == OMX_CommandStateSet) &&
                    (evt->args.ev.data2 == (unsigned int)mOmxState)
               ) {
                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived -> slave %d in state %d", evt->args.ev.id, mOmxState);
                mSlavesArray[evt->args.ev.id].state = mOmxState ;
            } else if (evt->args.ev.event == OMX_EventError) {
                OstTraceFiltInst3(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived OMX_EventError data1=%d, data2=%d sent by slave %d", evt->args.ev.data1, evt->args.ev.data2, evt->args.ev.id);
                mEventHandler->eventHandler(evt->args.ev.event, evt->args.ev.data1, evt->args.ev.data2);
            } else {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived CTRL_EVENT_FSMSIG unexpected event=%d in state=%d",evt->args.ev.event, mOmxState);
                this->state = (FSM_State) &hst_misc_audiocontroller::invalidState ;
            }
            break;

        default:
            OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForBufferSupplierSetStateReceived Unexpected event event=%d in state=%d",evt->args.ev.event, mOmxState);
            ARMNMF_DBC_ASSERT(0);
            break;
    }
}

void hst_misc_audiocontroller::waitForCommandCompleteState(const CtrlEvent *evt) {
    switch(CtrlEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForCommandCompleteState FSM_ENTRY_FSMSIG");
            break;
        case FSM_EXIT_FSMSIG:
            OstTraceFiltInst0(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForCommandCompleteState FSM_EXIT_FSMSIG");
            break;

        case CTRL_EVENT_FSMSIG:
            if ((evt->args.ev.event == OMX_EventCmdComplete) &&
                    (evt->args.ev.data1 == (unsigned int)mSlavesArray[evt->args.ev.id].currentCommand)) {

                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForCommandCompleteState -> slave %d complete command %d", evt->args.ev.id, evt->args.ev.data1);

                mSlavesArray[evt->args.ev.id].remainingCommands--;

                if(allSlaveCmdExecuted()) {
                    forwardPortEvent(evt->args.ev.event, mStandbyInfo.command, mStandbyInfo.id) ;
                    this->state = (FSM_State) &hst_misc_audiocontroller::omxState;
                }
            } else if ((evt->args.ev.event == OMX_EventCmdReceived) &&
                    (evt->args.ev.data1 == (unsigned int)mSlavesArray[evt->args.ev.id].currentCommand)) {

                OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_audiocontroller::waitForCommandCompleteState -> command %d received by slave %d", evt->args.ev.data1, evt->args.ev.id);

                mSlavesArray[evt->args.ev.id].remainingReceivedCommands--;

                if(allSlaveCmdReceived()) {
                    forwardPortEvent(evt->args.ev.event, mStandbyInfo.command, mStandbyInfo.id) ;
                }
            } else if (evt->args.ev.event == OMX_EventError) {
                OstTraceFiltInst3(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForCommandState OMX_EventError data1=%d, data2=%d sent by slave %d", evt->args.ev.data1, evt->args.ev.data2, evt->args.ev.id);
                mEventHandler->eventHandler(evt->args.ev.event, evt->args.ev.data1, evt->args.ev.data2);
            } else {
                OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForCommandState CTRL_EVENT_FSMSIG unexpected event=%d in state=%d",evt->args.ev.event, mOmxState);
                this->state = (FSM_State) &hst_misc_audiocontroller::omxState;
            }
            break;

        default:
            OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::waitForCommandState Unexpected event event=%d in state=%d",evt->args.ev.event, mOmxState);
            ARMNMF_DBC_ASSERT(0);
            break;
    }
}

void hst_misc_audiocontroller::invalidState(const CtrlEvent * evt) {
    switch(CtrlEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:
            OstTraceFiltInst0(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::invalidState FSM_ENTRY_FSMSIG");
            mEventHandler->eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInvalidState, 0);
            break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "AFM_HOST: hst_misc_audiocontroller::invalidState default");
            break ;
    }
}

