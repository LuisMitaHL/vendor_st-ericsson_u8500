/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   afmcontroller.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "hst/misc/afmcontroller.nmf"
#include "OMX_Core.h"
#include "armnmf_dbc.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_hst_misc_afmcontroller_src_afmcontrollerTraces.h"
#endif

inline bool hst_misc_afmcontroller::isOutputPort(int idx) {
    if (mPortDirections & (1 << idx)) return true;
    else return false;
}

inline bool hst_misc_afmcontroller::isInputPort(int idx) {
    return !isOutputPort(idx);
}

/**
 * Send Idle state command to the binding(s) if not already in IdleState
 * \param portIdx
 * \return true if a sendCmd Idle has been sent to one or several bindings,
 * */
bool hst_misc_afmcontroller::dispatchSendCmdIdleOnBindings(OMX_STATETYPE state, t_uword portIdx) {
    int i;
    bool isOneCmdSent = false;

    if (portIdx == OMX_ALL) {
        mEventAppliedOnEachPort = true;
        for (i = 0; i < MAX_PORTS; i++) {
            if ((state != OMX_StateIdle) && !(binding[i].IsNullInterface())) {
                OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller:: send idleState to binding %d", i);
                binding[i].sendCommand(OMX_CommandStateSet, OMX_StateIdle);
	            waitCommandFromSlave(i, OMX_CommandStateSet, 1);
                isOneCmdSent = true;
            }
        }
    } else if (state != OMX_StateIdle && !(binding[portIdx].IsNullInterface())) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller:: send idleState to binding %d", portIdx);
        binding[portIdx].sendCommand(OMX_CommandStateSet, OMX_StateIdle);
	    waitCommandFromSlave(portIdx, OMX_CommandStateSet, 1);
        isOneCmdSent = true;
    }

    return isOneCmdSent;
}

bool hst_misc_afmcontroller::allBindingsStateCmdReceived(OMX_STATETYPE state, t_uword portIdx) {
    int i;

    if (portIdx == OMX_ALL) {

        for (i = 0; i < MAX_PORTS; i++) {
            if (state != OMX_StateIdle && !(binding[i].IsNullInterface())) {
                if (!(mBindingsCmdReceived & (1 << (i + 1)))) {
                    return false;
                }
            }
        }
    } else if (state != OMX_StateIdle && !(binding[portIdx].IsNullInterface())) {
        if (!(mBindingsCmdReceived & (1 << (portIdx + 1)))) {
            return false;
        }
    }

    return true;
}

// return true if one binding is instantiated on corresponding port
// if portIdx == OMX_ALL, return true if one binding is instantiated at least
// on one port
bool hst_misc_afmcontroller::isThereOneBindingInstantiated(t_uword portIdx) {
    int i;

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (!(binding[i].IsNullInterface())) {
                return true;
            }
        }
    } else if (!(binding[portIdx].IsNullInterface())) {
        return true;
    }

    return false;
}

void hst_misc_afmcontroller::enableBindingComponent(OMX_STATETYPE state, t_uword portIdx) {

    t_uint16 bindingPortsTunneled = ALL_PORTS_TUNNELED;

    if(binding[portIdx].IsNullInterface()) return;

    if(isOutputPort(portIdx)) {
        bindingPortsTunneled &= (mPortsTunneled & (1 << portIdx)) << 1;
    } else {
        bindingPortsTunneled &= (mPortsTunneled & (1 << portIdx)) << 0;
    }

    fsmInit_t init;
    init.portsDisabled = 0;
    init.portsTunneled = bindingPortsTunneled;
    init.traceInfoAddr = getTraceInfoPtr();    //  O: there's a check into fsmInit() of bindinginit
    if(mPortTypes & (1 << portIdx)) init.traceInfoAddr = (TraceInfo_t *)init.traceInfoAddr->dspAddr;
    init.id1 = portIdx;
    OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller:: send fsmInit on binding %d", portIdx);
    bindinginit[portIdx].fsmInit(init); // Will respond with a "OMX_CommandStateSet to Idle done".

    if (state != OMX_StateIdle) {
        OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller:: send setState(%d) on binding %d", state, portIdx);
        binding[portIdx].sendCommand(OMX_CommandStateSet, state);
        waitCommandFromSlave(portIdx, OMX_CommandStateSet, 2) ;
    } else {
        waitCommandFromSlave(portIdx, OMX_CommandStateSet, 1) ;
    }

    //forward StateIdle CmdReceived event to generic audio controller class (not sent following a fsmInit)
    hst_misc_audiocontroller::slavesEventHandler(OMX_EventCmdReceived, OMX_CommandStateSet, OMX_StateIdle, portIdx);
}

// dispatch a SetState command to binding component
void hst_misc_afmcontroller::dispatchEnablePortCmdOnBindings(OMX_STATETYPE state, t_uword portIdx) {

    int i;

    if (portIdx == OMX_ALL) {
        mEventAppliedOnEachPort = true;
        for (i = 0; i < MAX_PORTS; i++) {
            enableBindingComponent(state, i);
        }
    } else {
        enableBindingComponent(state, portIdx);
    }
}

void hst_misc_afmcontroller::printMainEvents(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) {
    if (ev == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandStateSet(state=%d) received by main ", data2);
    }
    else if (ev == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandStateSet(state=%d) reached by main ", data2);
    } else if (ev == OMX_EventCmdReceived && data1 == OMX_CommandPortDisable) {
        if(data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortDisable received by all ports");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortDisable received by port %d", data2);
        }
    } else if (ev == OMX_EventCmdComplete && data1 == OMX_CommandPortDisable) {
        if(data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortDisable reached by all ports");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortDisable reached by port %d", data2);
        }
    } else if (ev == OMX_EventCmdReceived && data1 == OMX_CommandPortEnable) {
        if(data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortEnable received by all ports");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortEnable received by port %d", data2);
        }
    }
    else if (ev == OMX_EventCmdComplete && data1 == OMX_CommandPortEnable) {
        if(data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortEnable reached by all ports");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandPortEnable reached by port %d", data2);
        }
    }
	else if (ev == OMX_EventCmdReceived && data1 == OMX_CommandFlush) {
        if(data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandFlush received by all ports");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_CommandFlush received by port %d", data2);
        }
    }
    else if (ev == OMX_EventCmdComplete && data1 == OMX_CommandFlush) {
        if(data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler all ports are flushed");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler port %d is flushed", data2);
        }
    }
    else if (ev == OMX_EventBufferFlag) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_EventBufferFlag sent by port %d", data1);
    } else if (ev == OMX_EventError) {
        OstTraceFiltInst2 (TRACE_ERROR, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler OMX_EventError data1=%d, data2=%d", data1, data2);
    } else
    {
        OstTraceFiltInst1 (TRACE_ERROR, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler unknown main_event=%d", ev);
        ARMNMF_DBC_ASSERT_MSG(0, "hst_misc_afmcontroller::main_eventHandler unknown main_event!\n");
    }
}

void hst_misc_afmcontroller::printBindingEvents(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 collectionIndex) {
    if (ev == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::eventHandler OMX_CommandStateSet(state=%d) received by binding %d", data2, collectionIndex);
    }
    else if (ev == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::eventHandler OMX_CommandStateSet(state=%d) reached by binding %d", data2, collectionIndex);
    }
	else if (ev == OMX_EventCmdReceived && data1 == OMX_CommandFlush) {
        OstTraceFiltInst2(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::eventHandler OMX_CommandFlush(state=%d) reached by binding %d", data2, collectionIndex);
    }
    else if (ev == OMX_EventCmdComplete && data1 == OMX_CommandFlush) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::eventHandler binding %d is flushed", collectionIndex);
    }
    else if (ev == OMX_EventBufferFlag) {
        OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::eventHandler OMX_EventBufferFlag sent by binding %d", collectionIndex);
    }
    else if (ev == OMX_EventError) {
        OstTraceFiltInst2(TRACE_ERROR, "AFM_HOST: hst_misc_afmcontroller::eventHandler OMX_EventError data1=%d, data2=%d", data1, data2);
    }
    else {
        OstTraceFiltInst1 (TRACE_ERROR, "AFM_HOST: hst_misc_afmcontroller::eventHandler unknown main_event=%d", ev);
        ARMNMF_DBC_ASSERT_MSG(0, "hst_misc_afmcontroller::eventHandler unknown event!\n");
    }
}

/* ------------------------------------------------------------------ */
void METH(setParameter)(t_uint16 portDirections, t_uint16 portTypes) {
    mPortDirections = portDirections;
    mPortTypes = portTypes;
}


void METH(fsmInit)(fsmInit_t initFsm) {
    // trace init
    setTraceInfo(initFsm.traceInfoAddr, MAX_PORTS);

    init(MAX_PORTS + 1, &proxy) ;

    mPortsTunneled = initFsm.portsTunneled;

    setSlave(MAIN_COMPONENT_INDEX, &maincomponent, false) ;

    fsmInit_t tmpinit;
    tmpinit.portsDisabled = initFsm.portsDisabled;
    tmpinit.portsTunneled = ALL_PORTS_TUNNELED;
    tmpinit.traceInfoAddr = initFsm.traceInfoAddr;
    tmpinit.id1 = MAX_PORTS + 1;
    maincompinit.fsmInit(tmpinit);

    tmpinit.portsDisabled = 0;
    for (int i = 0; i < MAX_PORTS; i++) {
        setSlave(i, &binding[i], true) ;
        if (!binding[i].IsNullInterface()) {
            tmpinit.portsTunneled = ALL_PORTS_TUNNELED;
            tmpinit.id1 = i;
            if (mPortTypes & (1 << i)){
                // mpc bindings => change addr
                tmpinit.traceInfoAddr = (TraceInfo_t *)(initFsm.traceInfoAddr->dspAddr);
            } else {
                tmpinit.traceInfoAddr = initFsm.traceInfoAddr;
            }
            if(isOutputPort(i)) {
                tmpinit.portsTunneled &= (initFsm.portsTunneled & (1 << i)) << 1;
            } else {
                tmpinit.portsTunneled &= (initFsm.portsTunneled & (1 << i)) << 0;
            }
            bindinginit[i].fsmInit(tmpinit);
        }
    }

}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled) {
    if (portIdx >=0) {
        // Reset and update the field correponding to portIdx in mPortsTunneled
        mPortsTunneled &= ~(1 << portIdx);
        mPortsTunneled |= isTunneled & (1 << portIdx);
    } else {
        mPortsTunneled = isTunneled;
    }
}

void METH(eventHandler)(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 collectionIndex) {

    t_uword portIdx;

    printBindingEvents(ev, data1, data2, collectionIndex);

    if (ev == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        mBindingsCmdReceived |= 1 << (collectionIndex + 1);
        if(mEventAppliedOnEachPort == true) {
            portIdx = OMX_ALL;
        } else {
            portIdx = collectionIndex;
        }
        OMX_STATETYPE currentState = getState() ;
        if (mIsMainWaittingForDisablePortCmd == true)
        {
            if(allBindingsStateCmdReceived(currentState, portIdx) == true) {
                OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller:: send OMX_CommandPortDisable(portId=%d) to main component", portIdx);
                maincomponent.sendCommand(OMX_CommandPortDisable, portIdx);
                mIsMainWaittingForDisablePortCmd = false;
            }
        }

        //forward event to generic audio controller class
        hst_misc_audiocontroller::slavesEventHandler(ev, data1, data2, collectionIndex);

    }
	else if (ev == OMX_EventCmdReceived && data1 == OMX_CommandFlush) {
		mSentCommandsToBindings--;
		if(!mSentCommandsToBindings) {
			/*
			Main component should receive the command after bindings have received it.
			so that there is no deadlock between Main and Binding regarding buffer because
			of Preemptive SMP scheduler.
			*/
			if(mEventAppliedOnEachPort == true) {
				portIdx = OMX_ALL;
			} else {
				portIdx = collectionIndex;
			}
			OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller:: send OMX_CommandFlush(portId=%d) to main component", portIdx);
			maincomponent.sendCommand(OMX_CommandFlush, portIdx);
		}

		// forward event to generic audio controller class
        // We are sending this to audiocontroller only to decrement the counter: remainingReceivedCommands
        hst_misc_audiocontroller::slavesEventHandler(ev, data1, data2, collectionIndex);

	}
    else if (ev == OMX_EventBufferFlag) {
        if (isOutputPort(collectionIndex)) {
            // forward event to proxy if output port
            OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::eventHandler send OMX_EventBufferFlag to proxy (portId=%d)", collectionIndex);
            proxy.eventHandler(ev, collectionIndex, data2);
        }
    }
    else {
        //forward event to generic audio controller class
        hst_misc_audiocontroller::slavesEventHandler(ev, data1, data2, collectionIndex);
    }
}


void METH(main_eventHandler)(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2) {

    printMainEvents(ev, data1, data2);

    if ((ev == OMX_EventError) && ((data1 == (t_uint32)OMX_ErrorUnderflow) || (data1 == (t_uint32)OMX_ErrorOverflow))) {
        proxy.eventHandler(ev, data1, data2);

    } else if (ev == OMX_EventCmdReceived && data1 == OMX_CommandPortEnable)
    {
        if (isThereOneBindingInstantiated(data2) == true) {
            OMX_STATETYPE currentState = getState() ;
            dispatchEnablePortCmdOnBindings(currentState, data2);
        }
        //forward event to generic audio controller class
        hst_misc_audiocontroller::slavesEventHandler(ev, data1, data2, MAIN_COMPONENT_INDEX);
    } else if (ev == OMX_EventBufferFlag)
    {
        int portIndex = data1;
        if ((binding[portIndex].IsNullInterface()) || (isInputPort(portIndex))) {
            // forward event to proxy if no bindings or if input port
            OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::main_eventHandler send OMX_EventBufferFlag to proxy (portId=%d)", data1);
            proxy.eventHandler(ev, data1, data2);
        }
    } else
    {
        //forward event to generic audio controller class
        hst_misc_audiocontroller::slavesEventHandler(ev, data1, data2, MAIN_COMPONENT_INDEX);
    }
}


/* ------------------------------------------------------------------ */

//specific implementation for disablePort, enablePort and flushPort commands
void hst_misc_afmcontroller::forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) {

    mEventAppliedOnEachPort = false;
    mBindingsCmdReceived = 0;
    mEnablePortCmdOngoing = false;
    mIsMainWaittingForDisablePortCmd = false;

    switch(cmd) {
        case OMX_CommandPortDisable:
            {
                //In case of disablePort command, bindings must receive setStateIdle before sending
                //the disablePort command to the main component in order to stop buffer transmission.
                OMX_STATETYPE currentState = getState();
                if(dispatchSendCmdIdleOnBindings(currentState, id) == true) {
                    mIsMainWaittingForDisablePortCmd = true;
                } else {
                    OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::forwardPortCommand send OMX_CommandPortDisable(portId=%d) to main component", id);
                    maincomponent.sendCommand(OMX_CommandPortDisable, id);
                }
	            waitCommandFromSlave(MAIN_COMPONENT_INDEX, OMX_CommandPortDisable, 1);
            }
            break ;

        case OMX_CommandPortEnable:
            {
                OstTraceFiltInst1(TRACE_FLOW, "AFM_HOST: hst_misc_afmcontroller::forwardPortCommand send OMX_CommandPortEnable(portId=%d) to main component", id);

                //In case of enablePort command, the main component must receive the enablePort command before
                //sending the setStateIdle to the bindings in order to be ready to receive buffers.
                mEnablePortCmdOngoing = true;
                maincomponent.sendCommand(OMX_CommandPortEnable, id);
                waitCommandFromSlave(MAIN_COMPONENT_INDEX, OMX_CommandPortEnable, 1);
            }
            break ;

        case OMX_CommandFlush:
            {
                int start_id, end_id, i;

                /* We will forward this flush command only to bindings
                   Once we receive OMX_EventCmdReceived for this command
                   then we will forward it to main component in eventHandler() */

                if (id == OMX_ALL) {
                    start_id = 0;
                    end_id = MAX_PORTS;
                    mEventAppliedOnEachPort = true;
                }
                else {
                    mEventAppliedOnEachPort = false;
                    start_id = id;
                    end_id = id + 1;
                }

                mSentCommandsToBindings = 0;

                for (i = start_id; i < end_id; i++) {
                    if (!binding[i].IsNullInterface()) {
                        binding[i].sendCommand(cmd, OMX_ALL);
                        waitCommandFromSlave(i, cmd, 1) ;
                        mSentCommandsToBindings++;
                    }
                }
                if(mSentCommandsToBindings == 0)
                {
                    maincomponent.sendCommand(cmd, id);
                }
                waitCommandFromSlave(MAIN_COMPONENT_INDEX, cmd, 1) ;
            }
            break ;
        default:
            break;
    }
}

