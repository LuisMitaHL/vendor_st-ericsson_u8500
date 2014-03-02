/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   controller.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <misc/controller.nmf>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/generic/include/FSM.inl"
#include "fsm/component/include/OmxEvent.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_nmf_mpc_misc_controller_src_controllerTraces.h"
#endif

typedef struct {
    FsmEvent        fsmEvent;
    union {
        struct {
            OMX_STATETYPE   state;
        } setState;

        struct {
            int             portIdx;
        } cmd;
        
    } args;
} CtrlEvent;

static inline CtrlEvent_signal(const CtrlEvent *evt) { return evt->fsmEvent.signal; }

#define MAX_PORTS   10

#define BINDING_EVENT 1
#define MAIN_EVENT    2

#define ALL_PORTS_TUNNELED 0xFFFF

static void omxState(FSM *this, const CtrlEvent *);

static void setStateTransientState(FSM *this, const CtrlEvent *);
static void setStateTransientDisableState(FSM *this, const CtrlEvent *evt);
static void setStateTransientEnableState(FSM *this, const CtrlEvent *evt);

static void setStateTransientDisableStateWaitForBindingsCmdReceived(FSM *this, const CtrlEvent *evt);
static void setStateTransientEnableWaitForBindingsSetStateComplete(FSM *this, const CtrlEvent *evt);
static void setStateTransientStateWaitForBindingsCmdReceived(FSM *this, const CtrlEvent *evt);
static void setStateTransientStateWaitForMainCmdReceived(FSM *this, const CtrlEvent *evt);

////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////

typedef enum {
    CTRL_MAIN_SETSTATECOMPLETE_SIG = OMX_LAST_SIG,
    CTRL_SETSTATECOMPLETE_SIG,
    CTRL_DISABLEPORTCOMPLETE_SIG,
    CTRL_ENABLEPORTCOMPLETE_SIG,
    CTRL_MAIN_CMDRECEIVED_SIG,
    CTRL_BINDING_CMDRECEIVED_SIG
} ControllerSignal;

static FSM              mController;

static OMX_STATETYPE    mOmxState;
static int              mComponentsInTargetState;
static int              mComponentsCmdReceived;
static int              mPortDirections;
static int              mPortsTunneled;
static bool             mNotifyStateTransition;
static bool             mEventAppliedOnEachPort;
static int              mFlushEventReceived;

#ifndef NDBC
static bool             mConfigured;
#endif

#pragma noprefix
#pragma nopid
extern void * THIS;

static inline bool
isOutputPort(int idx) {
    if (mPortDirections & (1 << idx)) {
        return true;
    } else {
        return false;
    }
}

static inline bool
isInputPort(int idx) {
    return !isOutputPort(idx);
}

////////////////////////////////////////////////////////////

static void 
Controller_init() {
	int i;
	TRACE_t * this = (TRACE_t *)&mController;
	OstTraceFiltInst0 (TRACE_API, "AFM_MPC: Controller_init function");	

    // at initialization, the controller cannot switch into idle state 
    // before knowing the state of other components 
    // => start in transient state with OMX_StateIdle as target state
    mOmxState = OMX_StateIdle;
    mNotifyStateTransition = true;
    mEventAppliedOnEachPort = false;
    mComponentsInTargetState = 0;
    mComponentsCmdReceived = 0;

    FSM_init(&mController, (FSM_State) setStateTransientState);
}


// dispatch Flush command to right binding components
static void dispatchFlushCmd(int portIdx) {
    int i;

	maincomponent.sendCommand(OMX_CommandFlush, portIdx);

    if (portIdx == OMX_ALL) {
        mEventAppliedOnEachPort = true;
        for (i = 0; i < MAX_PORTS; i++) {
            if (!IS_NULL_INTERFACE(binding[i], sendCommand)) {
                binding[i].sendCommand(OMX_CommandFlush, OMX_ALL);
            }
        }
    } else if (!IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        binding[portIdx].sendCommand(OMX_CommandFlush, OMX_ALL);
    }
}

// return true if one binding is instantiated on corresponding port
// if portIdx == OMX_ALL, return true if one binding is instantiated at least
// on one port
static bool isThereOneBindingInstantiated(int portIdx) {
    int i;

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (!IS_NULL_INTERFACE(binding[i], sendCommand)) {
                return true;
            }
        }
    } else if (!IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        return true;
    }

    return false;
}

// dispatch SetState commands to all binding components
static void dispatchSetStateCmdOnBindings(OMX_STATETYPE targetState, int portIdx) {
    int i;

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (!IS_NULL_INTERFACE(binding[i], sendCommand)) {
                binding[i].sendCommand(OMX_CommandStateSet, targetState);
            }
        }
    } else {
        binding[portIdx].sendCommand(OMX_CommandStateSet, targetState);
    }
}

/**
 * Send Idle state command to the binding(s) if not already in IdleState
 * \param portIdx
 * \return true if a sendCmd Idle has been sent to one or several bindings, 
 * */
static bool dispatchSendCmdIdleOnBindings(int portIdx) {
    int i;
    bool isOneCmdSent = false;

    if (portIdx == OMX_ALL) {
        mEventAppliedOnEachPort = true;
        for (i = 0; i < MAX_PORTS; i++) {
            if (mOmxState != OMX_StateIdle &&
                !IS_NULL_INTERFACE(binding[i], sendCommand)) {
                binding[i].sendCommand(OMX_CommandStateSet, OMX_StateIdle);
                isOneCmdSent = true;
            }
        }
    } else if (mOmxState != OMX_StateIdle &&
            !IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        binding[portIdx].sendCommand(OMX_CommandStateSet, OMX_StateIdle);
        isOneCmdSent = true;
    }

    return isOneCmdSent;
}

static void enableBindingComponent(int portIdx) {

	fsmInit_t tmpinit;
    t_uint16 bindingPortsTunneled = ALL_PORTS_TUNNELED;

    if (IS_NULL_INTERFACE(bindinginit[portIdx], fsmInit)) return;

    if(isOutputPort(portIdx)) {
        bindingPortsTunneled &= (mPortsTunneled & (1 << portIdx)) << 1;
    } else {
        bindingPortsTunneled &= (mPortsTunneled & (1 << portIdx)) << 0;
    }

	tmpinit.portsDisabled = 0;
	tmpinit.portsTunneled = bindingPortsTunneled;
	tmpinit.traceInfoAddr = mController.traceObject.mTraceInfoPtr; 
	tmpinit.id1 = portIdx;
    bindinginit[portIdx].fsmInit(tmpinit);
}

// dispatch a SetState command to binding component
static void dispatchEnablePortCmdOnBindings(int portIdx) {

    int i;

    if (portIdx == OMX_ALL) {
        mEventAppliedOnEachPort = true;
        for (i = 0; i < MAX_PORTS; i++) {
            enableBindingComponent(i);
        }
    } else {
        enableBindingComponent(portIdx);
    }
}

static bool allBindingsCmdReceived(void) {
    int i;

    for (i = 0; i < MAX_PORTS; i++) {
        if (IS_NULL_INTERFACE(binding[i], sendCommand)) {
            continue;
        }
        if (!(mComponentsCmdReceived & (1 << (i + 1)))) {
            return false;
        }
    }
    return true;
}

static bool allComponentsCmdReceived(void) {

    if (!(mComponentsCmdReceived & 0x1)) {
        return false;
    }

    return allBindingsCmdReceived();
}

static bool allComponentsInTargetState(void) {
    int i;

    if (!(mComponentsInTargetState & 0x1)) {
        return false;
    }

    for (i = 0; i < MAX_PORTS; i++) {
        if (IS_NULL_INTERFACE(binding[i], sendCommand)) {
            continue;
        }
        if (!(mComponentsInTargetState & (1 << (i + 1)))) {
            return false;
        }
    }
    return true;
}

static bool allBindingsStateCmdReceived(int portIdx) {
    int i;

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (mOmxState != OMX_StateIdle &&
                !IS_NULL_INTERFACE(binding[i], sendCommand)) {
                if (!(mComponentsCmdReceived & (1 << (i + 1)))) {
                    return false;
                }
            }
        }
    } else if (mOmxState != OMX_StateIdle &&
            !IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        if (!(mComponentsCmdReceived & (1 << (portIdx + 1)))) {
            return false;
        }
    }

    return true;
}

static bool isPortCommandReceived(int portIdx) {
    int i;

    if (!(mComponentsCmdReceived & 0x1)) {
        return false;
    }

    return allBindingsStateCmdReceived(portIdx);
}

static bool allBindingsStateIdleCmdCompleted(int portIdx) {
    int i;

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (mOmxState != OMX_StateIdle &&
                !IS_NULL_INTERFACE(binding[i], sendCommand)) {
                if (!(mComponentsInTargetState & (1 << (i + 1)))) {
                    return false;
                }
            }
        }
    } else if (mOmxState != OMX_StateIdle &&
            !IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        if (!(mComponentsInTargetState & (1 << (portIdx + 1)))) {
            return false;
        }
    }

    return true;
}

static bool isEnablePortCommandCompleted(int portIdx) {
    int i;

    if (!(mComponentsInTargetState & 0x1)) {
        return false;
    }

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (IS_NULL_INTERFACE(binding[i], sendCommand)) {
                continue;
            }
            if (!(mComponentsInTargetState & (1 << (i + 1)))) {
                return false;
            }
        }
    } else if (!IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        if (!(mComponentsInTargetState & (1 << (portIdx + 1)))) {
            return false;
        }
    }

    return true;
}

static bool isDisablePortCommandCompleted(int portIdx) {
    int i;

    if (!(mComponentsInTargetState & 0x1)) {
        return false;
    }

    return allBindingsStateIdleCmdCompleted(portIdx);
}

static bool
allFlushEventReceived(int portIdx) {
    int i;

    if (!(mFlushEventReceived & 0x1)) {
        return false;
    }

    if (portIdx == OMX_ALL) {
        for (i = 0; i < MAX_PORTS; i++) {
            if (!IS_NULL_INTERFACE(binding[i], sendCommand)) {
                if (!(mFlushEventReceived & (1 << (i + 1)))) {
                    return false;
                }
            }
        }
    } else if (!IS_NULL_INTERFACE(binding[portIdx], sendCommand)) {
        if (!(mFlushEventReceived & (1 << (portIdx + 1)))) {
            return false;
        }
    }

    return true;
}

// controller in one of the OMX states
static void 
omxState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
		    OstTraceFiltInst1(TRACE_FLOW, "AFM_MPC: controller omxstate FSM_ENTRY_SIG %d",(unsigned int)mOmxState);
            mComponentsInTargetState = 0;
            mComponentsCmdReceived = 0;
            if(mNotifyStateTransition) {
                proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, mOmxState);
            } else {
                mNotifyStateTransition = true;
            }
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller omxstate FSM_EXIT_SIG");
			break;

        case OMX_SETSTATE_SIG:
            mOmxState = evt->args.setState.state;
            
            if(mOmxState  == OMX_StateIdle){
                //In case of transition to idle state, bindings must receive setStateIdle before the main 
                //component in order to stop buffer transmissions. For that, we are waitting for 
                //CTRL_BINDING_CMDRECEIVED_SIG before sending setStateIdle to the main component
                if(isThereOneBindingInstantiated(OMX_ALL) == true) {
                    dispatchSetStateCmdOnBindings(mOmxState, OMX_ALL);
                    FSM_TRANSITION(setStateTransientStateWaitForBindingsCmdReceived);
                } else {
                    maincomponent.sendCommand(OMX_CommandStateSet, mOmxState);
                    FSM_TRANSITION(setStateTransientState);
                }
            } else {
                //In other cases, we are sending setState command to the main component and on the 
                //CTRL_MAIN_CMDRECEIVED_SIG reception, we will send (if needed) setState commands to the
                //bindings
                maincomponent.sendCommand(OMX_CommandStateSet, mOmxState);
                if(isThereOneBindingInstantiated(OMX_ALL) == true) {
                    FSM_TRANSITION(setStateTransientStateWaitForMainCmdReceived);                    
                } else {
                    FSM_TRANSITION(setStateTransientState);
                }
            }

            break;
            
        case OMX_DISABLE_PORT_SIG:
            //In case of disablePort command, bindings must receive setStateIdle before sending
            //the disablePort command to the main component in order to stop buffer transmission.
            if(dispatchSendCmdIdleOnBindings(evt->args.cmd.portIdx) == true) {
                FSM_TRANSITION(setStateTransientDisableStateWaitForBindingsCmdReceived);
            } else {
                maincomponent.sendCommand(OMX_CommandPortDisable, evt->args.cmd.portIdx);
                FSM_TRANSITION(setStateTransientDisableState);
            }
            break;

        case OMX_ENABLE_PORT_SIG:
            //In case of enablePort command, the main component must receive the enablePort command before 
            //sending the setStateIdle to the bindings in order to be ready to receive buffers.
            maincomponent.sendCommand(OMX_CommandPortEnable, evt->args.cmd.portIdx);
            FSM_TRANSITION(setStateTransientEnableState);
            break;

		case OMX_FLUSH_SIG:
            mFlushEventReceived = 0;
            mEventAppliedOnEachPort = false;
			dispatchFlushCmd(evt->args.cmd.portIdx);
			break;

        case CTRL_SETSTATECOMPLETE_SIG:
            //A binding component has been instantiated!
            break;

        default:
            ASSERT(0);
    }
}

// controller has sent a SetState command to all components
// => waiting for CmdComplete events from all components
static void 
setStateTransientState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientState FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientState FSM_EXIT_SIG");
			break;

        case CTRL_BINDING_CMDRECEIVED_SIG:        
        case CTRL_MAIN_CMDRECEIVED_SIG:
            if (allComponentsCmdReceived()) {
                proxy.eventHandler(OMX_EventCmdReceived, OMX_CommandStateSet, mOmxState);
            }
            break;

        case CTRL_MAIN_SETSTATECOMPLETE_SIG:
        case CTRL_SETSTATECOMPLETE_SIG: 
            if (allComponentsInTargetState()) {
                FSM_TRANSITION(omxState);
            }
            break;

        default:
            ASSERT(0);
    }
}

// Following a SetStateIdle command, controller has sent a SetStateIdle command to all binding components
// => waitting here for CmdReceived from all bindings before sending the SetStateIdle command to the main 
// component
static void setStateTransientStateWaitForBindingsCmdReceived(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientStateWaitForBindingsCmdReceived FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientStateWaitForBindingsCmdReceived FSM_EXIT_SIG");
			break;
        
        case CTRL_BINDING_CMDRECEIVED_SIG:
            if (allBindingsCmdReceived()) {
                maincomponent.sendCommand(OMX_CommandStateSet, mOmxState);
                FSM_TRANSITION(setStateTransientState);
            }
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
            break;

        default:
            ASSERT(0);
    }
}

// Following a SetState command, controller has sent a SetState command to the main component
// => waitting here for CmdReceived from the main component before sending the SetState command to the
// bindings (if needed)
static void setStateTransientStateWaitForMainCmdReceived(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientStateWaitForMainCmdReceived FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientStateWaitForMainCmdReceived FSM_EXIT_SIG");
			break;
        
        case CTRL_MAIN_CMDRECEIVED_SIG:
            dispatchSetStateCmdOnBindings(mOmxState, OMX_ALL);
            FSM_TRANSITION(setStateTransientState);
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
            break;

        default:
            ASSERT(0);
    }
}


// Following a disablePort command, controller has sent a SetState Idle to all binding components
// => waiting for CmdReceived from all binding components before sending the disablePort cmd to the
// main component
static void setStateTransientDisableStateWaitForBindingsCmdReceived(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientDisableStateWaitForBindingsCmdReceived FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientDisableStateWaitForBindingsCmdReceived FSM_EXIT_SIG");
			break;

        case CTRL_BINDING_CMDRECEIVED_SIG:
            if (allBindingsStateCmdReceived(evt->args.cmd.portIdx)) {
                maincomponent.sendCommand(OMX_CommandPortDisable, evt->args.cmd.portIdx);
                FSM_TRANSITION(setStateTransientDisableState);
            }
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
            break;

        default:
            ASSERT(0);
    }
}

// controller has sent a disable port command to main component
// and a SetState Idle to all binding components
// => waiting for CmdReceived and CmdComplete events from all components
static void 
setStateTransientDisableState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientDisableState FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientDisableState FSM_EXIT_SIG");
			break;

        case CTRL_MAIN_CMDRECEIVED_SIG:
            if (isPortCommandReceived(evt->args.cmd.portIdx)) {
                proxy.eventHandler(OMX_EventCmdReceived, OMX_CommandPortDisable, evt->args.cmd.portIdx);
            }
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
        case CTRL_DISABLEPORTCOMPLETE_SIG:
            if (isDisablePortCommandCompleted(evt->args.cmd.portIdx)) {
                mNotifyStateTransition = false;
                mEventAppliedOnEachPort = false;
                FSM_TRANSITION(omxState);
                proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortDisable, evt->args.cmd.portIdx);
            }
            break;

        default:
            ASSERT(0);
    }
}

// controller has sent a enable port command to main component
// and a SetState command to all binding components
// => waiting for CmdComplete events from all components
static void 
setStateTransientEnableState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientEnableState FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientEnableState FSM_EXIT_SIG");
			break;

        case CTRL_MAIN_CMDRECEIVED_SIG:
            if (isThereOneBindingInstantiated(evt->args.cmd.portIdx) == true) {
                dispatchEnablePortCmdOnBindings(evt->args.cmd.portIdx);
                if(mOmxState == OMX_StateIdle) { 
                    //Ctrl can already send EnablePort CmdReceived event to proxy
                    proxy.eventHandler(OMX_EventCmdReceived, OMX_CommandPortEnable, evt->args.cmd.portIdx);
                }
            } else {
                proxy.eventHandler(OMX_EventCmdReceived, OMX_CommandPortEnable, evt->args.cmd.portIdx);
            }
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
        case CTRL_ENABLEPORTCOMPLETE_SIG:
            if (isEnablePortCommandCompleted(evt->args.cmd.portIdx)) {
                if((mOmxState != OMX_StateIdle) && (isThereOneBindingInstantiated(evt->args.cmd.portIdx) == true)){
                    //Ctrl must send setState command to all the bindings (to be in same state than main component)
                    mComponentsCmdReceived = 1; //main port is already enabled
                    mComponentsInTargetState = 1;
                    dispatchSetStateCmdOnBindings(mOmxState, evt->args.cmd.portIdx);
                    FSM_TRANSITION(setStateTransientEnableWaitForBindingsSetStateComplete);
                } else {
                    mNotifyStateTransition = false;
                    mEventAppliedOnEachPort = false;
                    FSM_TRANSITION(omxState);
                    proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortEnable, evt->args.cmd.portIdx);
                }
            }
            break;

        default:
            ASSERT(0);
    }
}

// Following a enablePort command, controller has sent SetState command to all binding components
// => waiting for CmdComplete events from all components
static void 
setStateTransientEnableWaitForBindingsSetStateComplete(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientEnableWaitForBindingsSetStateComplete FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
			OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: controller setStateTransientEnableWaitForBindingsSetStateComplete FSM_EXIT_SIG");
			break;

        case CTRL_BINDING_CMDRECEIVED_SIG:
            if (isPortCommandReceived(evt->args.cmd.portIdx) == true) {
                proxy.eventHandler(OMX_EventCmdReceived, OMX_CommandPortEnable, evt->args.cmd.portIdx);
            }
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
        case CTRL_ENABLEPORTCOMPLETE_SIG:
            if (isEnablePortCommandCompleted(evt->args.cmd.portIdx)) {
                mNotifyStateTransition = false;
                mEventAppliedOnEachPort = false;
                FSM_TRANSITION(omxState);
                proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortEnable, evt->args.cmd.portIdx);
            }
            break;

        default:
            ASSERT(0);
    }
}

////////////////////////////////////////////////////////////
// Provided Interfaces 
////////////////////////////////////////////////////////////

void METH(setParameter)(t_uint16 portDirections) {
    mConfigured = true;
    mPortDirections = portDirections;
}

void METH(fsmInit) (fsmInit_t initFsm) {
    int i;
	fsmInit_t tmpinit; // to overwrite some parameters
	TRACE_t *this = (TRACE_t *)&mController;

    mPortsTunneled = initFsm.portsTunneled;

	// trace init
    FSM_traceInit(&mController, initFsm.traceInfoAddr, MAX_PORTS);

	OstTraceFiltInst0(TRACE_FLOW, "AFM_MPC: this component stands for mpc_controller");

    Controller_init();

	tmpinit.portsDisabled = initFsm.portsDisabled; 
	tmpinit.portsTunneled = ALL_PORTS_TUNNELED;
	tmpinit.traceInfoAddr = initFsm.traceInfoAddr;
	tmpinit.id1 = MAX_PORTS + 1;

    maincompinit.fsmInit(tmpinit);

	tmpinit.portsDisabled = 0; 

    for (i = 0; i < MAX_PORTS; i++) {
        if (!IS_NULL_INTERFACE(bindinginit[i], fsmInit)) {
			tmpinit.portsTunneled = ALL_PORTS_TUNNELED;
			tmpinit.id1 = i;
			if(isOutputPort(i)) {
                tmpinit.portsTunneled &= (initFsm.portsTunneled & (1 << i)) << 1;
                
            } else {
                tmpinit.portsTunneled &= (initFsm.portsTunneled & (1 << i)) << 0;
            }
            bindinginit[i].fsmInit(tmpinit);
        }
    }
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    if (portIdx >=0) {
        // Reset and update the field correponding to portIdx in mPortsTunneled
        mPortsTunneled &= ~(1 << portIdx); 
        mPortsTunneled |= isTunneled & (1 << portIdx);
    } else {
        mPortsTunneled = isTunneled;
    }
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    CtrlEvent ev;

    if (cmd == OMX_CommandStateSet) {
        ev.fsmEvent.signal = OMX_SETSTATE_SIG;
        ev.args.setState.state = (OMX_STATETYPE) param;
    } 
    else if (cmd == OMX_CommandPortDisable) {
        ev.fsmEvent.signal = OMX_DISABLE_PORT_SIG;
        ASSERT((param==OMX_ALL) || (param<MAX_PORTS));
        ev.args.cmd.portIdx = param;
    }
    else if (cmd == OMX_CommandPortEnable) {
        ev.fsmEvent.signal = OMX_ENABLE_PORT_SIG;
        ASSERT((param==OMX_ALL) || (param<MAX_PORTS));
        ev.args.cmd.portIdx = param;
    } 
	else if (cmd == OMX_CommandFlush) {
        ev.fsmEvent.signal = OMX_FLUSH_SIG;
        ASSERT((param==OMX_ALL) || (param<MAX_PORTS));
        ev.args.cmd.portIdx = param;		
	}
    else { ASSERT(0); }
    
    FSM_dispatch(&mController, &ev);
}

void METH(main_eventHandler)(
        OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    CtrlEvent ev;
	TRACE_t * this = (TRACE_t *)&mController;    

    ASSERT(mConfigured);

    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_CommandStateSet(state=%d) reached by main", data2);        
        ev.fsmEvent.signal = CTRL_MAIN_SETSTATECOMPLETE_SIG;
        ASSERT((OMX_STATETYPE) data2 == mOmxState);
        mComponentsInTargetState |= 0x1;
        FSM_dispatch(&mController, &ev);
    } 
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_CommandStateSet(state=%d) received by main ", data2);                
        ev.fsmEvent.signal = CTRL_MAIN_CMDRECEIVED_SIG;
        ASSERT((OMX_STATETYPE) data2 == mOmxState);
        mComponentsCmdReceived |= 0x1;
        FSM_dispatch(&mController, &ev);
    } 
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandPortDisable) {
        if((int)data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_CommandPortDisable received by all ports");
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_CommandPortDisable received by port %d", data2);
        }
        ev.fsmEvent.signal = CTRL_MAIN_CMDRECEIVED_SIG;
        ev.args.cmd.portIdx = data2;
        mComponentsCmdReceived |= 0x1;
        FSM_dispatch(&mController, &ev);
    }
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortDisable) {
        if((int)data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler all ports are disabled");
        } else {            
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler port %d is disabled", data2);
        }
        ev.fsmEvent.signal = CTRL_DISABLEPORTCOMPLETE_SIG;
        ev.args.cmd.portIdx = data2;
        mComponentsInTargetState |= 0x1;
        FSM_dispatch(&mController, &ev);
    }
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandPortEnable) {
        if((int)data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_CommandPortEnable received by all ports");            
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_CommandPortEnable received by port %d", data2);
        }
        ev.fsmEvent.signal = CTRL_MAIN_CMDRECEIVED_SIG;
        ev.args.cmd.portIdx = data2;
        mComponentsCmdReceived |= 0x1;
        FSM_dispatch(&mController, &ev);
    }
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortEnable) {
        if((int)data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler all ports are enabled");  
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler port %d is enabled", data2);
        }
        ev.fsmEvent.signal = CTRL_ENABLEPORTCOMPLETE_SIG;
        ev.args.cmd.portIdx = data2;
        mComponentsInTargetState |= 0x1;
        FSM_dispatch(&mController, &ev);
    }
	else if (event == OMX_EventCmdComplete && data1 == OMX_CommandFlush) {
        if((int)data2 == OMX_ALL) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler all ports are flushed");                 
        } else {
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler port %d is flushed", data2);     
        }
        mFlushEventReceived |= 0x1;
        if(allFlushEventReceived((int)data2) == true) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler forward FlushCompleteEvent to proxy");                        
		    proxy.eventHandler(event, data1, data2);
        }
	}
    else if (event == OMX_EventBufferFlag) {
        int portIndex = data1;
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler OMX_EventBufferFlag sent by port %d", data1);                
        if (IS_NULL_INTERFACE(binding[portIndex], sendCommand) ||
                isInputPort(portIndex)) {
            // forward event to proxy if no bindings or if input port
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller main_eventHandler forward OMX_EventBufferFlag to proxy (portId=%d)", data1);            
            proxy.eventHandler(event, data1, data2);
        } 
    }
    else if (event == OMX_EventError) {
        OstTraceFiltInst2 (TRACE_ERROR, "AFM_MPC: controller main_eventHandler OMX_EventError data1=%d, data2=%d", data1, data2);                
        proxy.eventHandler(event, data1, data2);
    }
    else if (event == OMX_DSP_EventIndexSettingChanged){
        proxy.eventHandler(event, data1, data2);
    }
    else {
        ASSERT(0);
    }
}

void METH(eventHandler)(
        OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, t_uint8 index)
{
    CtrlEvent ev;
	TRACE_t * this = (TRACE_t *)&mController;    

    ASSERT(mConfigured);

    if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst2 (TRACE_FLOW, "AFM_MPC: controller eventHandler OMX_CommandStateSet(state=%d) received by binding %d", data2, index);        
        ev.fsmEvent.signal = CTRL_BINDING_CMDRECEIVED_SIG;
        if(mEventAppliedOnEachPort == true) {
            ev.args.cmd.portIdx = OMX_ALL;
        } else {
            ev.args.cmd.portIdx = index;
        }
        mComponentsCmdReceived |= 1 << (index + 1);
        FSM_dispatch(&mController, &ev);
    }
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        OstTraceFiltInst2 (TRACE_FLOW, "AFM_MPC: controller eventHandler OMX_CommandStateSet(state=%d) reached by binding %d", data2, index);                
        ev.fsmEvent.signal = CTRL_SETSTATECOMPLETE_SIG;
        if(mEventAppliedOnEachPort == true) {
            ev.args.cmd.portIdx = OMX_ALL;
        } else {
            ev.args.cmd.portIdx = index;
        }
        mComponentsInTargetState |= 1 << (index + 1);
        FSM_dispatch(&mController, &ev);
    } 
	else if (event == OMX_EventCmdComplete && data1 == OMX_CommandFlush) {
        int portIdx;
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller eventHandler binding %d is flushed", index);                                
        if(mEventAppliedOnEachPort == true) {
            portIdx = OMX_ALL;
        } else {
            portIdx = index;
        }
        mFlushEventReceived |= 1 << (index + 1);
        if(allFlushEventReceived(portIdx) == true) {
            OstTraceFiltInst0 (TRACE_FLOW, "AFM_MPC: controller eventHandler forward FlushCompleteEvent to proxy");                                    
		    proxy.eventHandler(event, data1, portIdx);
        }
	}
    else if (event == OMX_EventBufferFlag) {
        OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller eventHandler OMX_EventBufferFlag sent by binding %d", index);                
        if (isOutputPort(index)) {
            // forward event to proxy if output port
            OstTraceFiltInst1 (TRACE_FLOW, "AFM_MPC: controller eventHandler forward OMX_EventBufferFlag to proxy (portId=%d)", index);            
            proxy.eventHandler(event, index, data2);
        }
    }
    else if (event == OMX_EventError) {
        OstTraceFiltInst2 (TRACE_ERROR, "AFM_MPC: controller eventHandler OMX_EventError data1=%d, data2=%d", data1, data2);                        
        proxy.eventHandler(OMX_EventError, data1, data2);
    } 
    else if (event == OMX_DSP_EventIndexSettingChanged) {
        proxy.eventHandler(event, data1, data2);
    }
    else {
        ASSERT(0);
    }
}

