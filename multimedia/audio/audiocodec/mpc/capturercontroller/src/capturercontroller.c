/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   capturercontroller.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <audiocodec/mpc/capturercontroller.nmf>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/generic/include/FSM.inl"
#include "fsm/component/include/OmxEvent.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_capturercontroller_src_capturercontrollerTraces.h"
#endif

#define OMX_PORT_IDX            (0)
#define VOLCTRL_IDX             (1)


typedef struct {
    FsmEvent        fsmEvent;
    union {
        struct {
            OMX_STATETYPE   state;
        } setState;

        struct {
            int             portIdx;
        } portCmd;

        struct {
            OMX_STATETYPE   state;
            int             portIdx;
        } cmdComplete;
    } args;
} CtrlEvent;

static bool isPortEnabled = true;

static inline CtrlEvent_signal(const CtrlEvent *evt) { return evt->fsmEvent.signal; }

#define MAX_PORTS  1
#define ALL_PORTS_TUNNELED 0xFFFF

static void omxState(FSM *this, const CtrlEvent *);
static void setStateTransientState(FSM *this, const CtrlEvent *);
static void setStateTransientDisableState(FSM *this, const CtrlEvent *evt);
static void setStateTransientEnableState(FSM *this, const CtrlEvent *evt);

////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////

typedef enum {
    CTRL_CAPTURER_SETSTATECOMPLETE_SIG = OMX_LAST_SIG,
    CTRL_SETSTATECOMPLETE_SIG,
    CTRL_DISABLEPORTCOMPLETE_SIG,
    CTRL_ENABLEPORTCOMPLETE_SIG
} ControllerSignal;

static FSM              mCapturerController;

static OMX_STATETYPE    mOmxState;
static bool             mIsDmaRunning  = false;  
static t_uint32         inputInTargetState;


////////////////////////////////////////////////////////////

static void 
CapturerController_init() {
    // at initialization, the controller cannot switch into idle state 
    // before knowing the state of other components 
    // => start in transient state with OMX_StateIdle as target state
    mOmxState = OMX_StateIdle;
    inputInTargetState = 0;
    mIsDmaRunning  = false;  
    FSM_init(&mCapturerController, (FSM_State) setStateTransientState);
}

static bool
isDmaReadyForStateChange(void) {
    int i;
    bool answer = true;

    if(mOmxState == OMX_StateIdle){
        if(mIsDmaRunning){
            answer = false;
        }
    }
   
    return answer;
}

static bool
allComponentsInTargetState(void) {
    if (inputInTargetState) {
        return true;
    } else {
        return false;
    }
}

// controller in one of the OMX states
static void 
omxState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            break;

        case FSM_EXIT_SIG:
            break;

        case OMX_SETSTATE_SIG:
            inputInTargetState = 0;

            mOmxState = evt->args.setState.state;
            inputblk_sendcommand.sendCommand(OMX_CommandStateSet, mOmxState);
            if((mOmxState==OMX_StateExecuting) && isPortEnabled ){
                dma_state.enablePort();                                    
            }
            // to avoid calling disablePort() twice check if it was done through OMX_DISABLE_PORT
            else if ((mOmxState==OMX_StateIdle) && isPortEnabled ){
                dma_state.disablePort();                
            }
            FSM_TRANSITION(setStateTransientState);
            break;

        case OMX_DISABLE_PORT_SIG:
            PRECONDITION(evt->args.portCmd.portIdx == OMX_PORT_IDX);
            isPortEnabled = false;

            inputblk_sendcommand.sendCommand(OMX_CommandPortDisable, OMX_ALL);
            
            // to avoid calling disablePort() twice check if it was done through OMX_SETSTATE to idle

            if(mOmxState==OMX_StateExecuting){
                dma_state.disablePort();
            }

            FSM_TRANSITION(setStateTransientDisableState);
            break;

        case OMX_ENABLE_PORT_SIG:
            PRECONDITION(evt->args.portCmd.portIdx == OMX_PORT_IDX);
            isPortEnabled = true;
            
            inputblk_sendcommand.sendCommand(OMX_CommandPortEnable, OMX_ALL);
            if(mOmxState==OMX_StateExecuting){
                dma_state.enablePort();
            }
            
            FSM_TRANSITION(setStateTransientEnableState);
            break;
        case CTRL_SETSTATECOMPLETE_SIG: 
            // NOTHING TO DO
            // This is the event from portIsRunning interface
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
            break;

        case FSM_EXIT_SIG:
            break;

        case CTRL_SETSTATECOMPLETE_SIG: 
            if (allComponentsInTargetState() && isDmaReadyForStateChange()) {
                FSM_TRANSITION(omxState);
                proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, mOmxState);
            }
            break;

        default:
            ASSERT(0);
    }
}

// controller has sent a disable port command to either emitter or volctrl
static void 
setStateTransientDisableState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            break;
        case FSM_EXIT_SIG:
            break;

        case CTRL_DISABLEPORTCOMPLETE_SIG:
            ASSERT(evt->args.cmdComplete.portIdx == OMX_PORT_IDX);

            proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortDisable, OMX_PORT_IDX);

            FSM_TRANSITION(omxState);
            break;
        case CTRL_SETSTATECOMPLETE_SIG: 
            // NOTHING TO DO
            // This is the event from portIsStopped interface
            break;
        default:
            ASSERT(0);
    }
}

// controller has sent a enable port command either emitter or volctrl
static void 
setStateTransientEnableState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            break;

        case FSM_EXIT_SIG:
            break;

        case CTRL_ENABLEPORTCOMPLETE_SIG:
            ASSERT(evt->args.cmdComplete.portIdx == OMX_PORT_IDX);

            proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortEnable,  OMX_PORT_IDX);

            FSM_TRANSITION(omxState);
            break;
        case CTRL_SETSTATECOMPLETE_SIG: 
            // NOTHING TO DO
            // This is the event from portIsRunning interface
            break;
        default:
            ASSERT(0);
    }
}


////////////////////////////////////////////////////////////
// Provided Interfaces 
////////////////////////////////////////////////////////////

void METH(fsmInit) (fsmInit_t initFsm) {
    int i;
	fsmInit_t init_input;

    FSM_traceInit(&mCapturerController, initFsm.traceInfoAddr,initFsm.id1 );

    CapturerController_init();
	
    isPortEnabled  = ! (initFsm.portsDisabled & 0x1);
     // Omx port 0 is connected to volctrl. Enable or disable both ports of volctrl accordingly
    init_input.portsDisabled = initFsm.portsDisabled & 1 ? 3 : 0 ;
    init_input.portsTunneled = ALL_PORTS_TUNNELED;
    init_input.traceInfoAddr = initFsm.traceInfoAddr;
    init_input.id1           = initFsm.id1;

    inputblk_init.fsmInit(init_input);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    CtrlEvent ev;

    if (cmd == OMX_CommandStateSet) {
        ev.fsmEvent.signal = OMX_SETSTATE_SIG;
        ev.args.setState.state = (OMX_STATETYPE) param;
    } 
    else if (cmd == OMX_CommandPortDisable) {
        ev.fsmEvent.signal = OMX_DISABLE_PORT_SIG;
        ASSERT(param<MAX_PORTS);
        ev.args.portCmd.portIdx = param;
    }
    else if (cmd == OMX_CommandPortEnable) {

        ev.fsmEvent.signal = OMX_ENABLE_PORT_SIG;
        ASSERT(param<MAX_PORTS);
        ev.args.portCmd.portIdx = param;
    } 
    else { ASSERT(0); }

    FSM_dispatch(&mCapturerController, &ev);
}


void METH(inputblk_eventHandler)(
        OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    CtrlEvent ev;

    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        ev.fsmEvent.signal = CTRL_SETSTATECOMPLETE_SIG;
        ev.args.cmdComplete.state = (OMX_STATETYPE) data2;
        ev.args.cmdComplete.portIdx = 0; // not used, zero-inited
        inputInTargetState = 1;
        FSM_dispatch(&mCapturerController, &ev);
    } 
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {   
        proxy.eventHandler(OMX_EventCmdReceived, data1, data2);
    }
    else if (event == OMX_EventBufferFlag ) {
        proxy.eventHandler(event, OMX_PORT_IDX, data2 /* should be only EOS so far */);
    } 
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortDisable) {
        // Conversion from port idx to OMX idx
        ASSERT((data2&OMX_ALL)==OMX_ALL);
        ev.fsmEvent.signal = CTRL_DISABLEPORTCOMPLETE_SIG;
        ev.args.cmdComplete.portIdx = OMX_PORT_IDX;
        FSM_dispatch(&mCapturerController, &ev);
    }
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortEnable) {
        // Conversion from port idx to OMX idx
        ASSERT((data2&OMX_ALL)==OMX_ALL);
        ev.fsmEvent.signal = CTRL_ENABLEPORTCOMPLETE_SIG;
        ev.args.cmdComplete.portIdx = OMX_PORT_IDX;
        FSM_dispatch(&mCapturerController, &ev);
    }
    else if (event == OMX_EventCmdReceived && (data1 == OMX_CommandPortDisable || data1 == OMX_CommandPortEnable)) {
        // Conversion from port idx to OMX idx 
        proxy.eventHandler(event, data1, OMX_PORT_IDX);
    }
    else if (event == OMX_EventError) {
        proxy.eventHandler(OMX_EventError, data1, data2);
    } 
    else {
        ASSERT(0);
    }
}

void METH(portIsRunning)(void){
    CtrlEvent ev;
#ifdef DEBUG_TRACE     
    OstTraceInt0(TRACE_ERROR, "renderercontroller : portIsRunning" );
#endif     
    mIsDmaRunning  = true;  
    
    ev.fsmEvent.signal        = CTRL_SETSTATECOMPLETE_SIG;
    ev.args.cmdComplete.state = mOmxState;
    ev.args.cmdComplete.portIdx = 0; // not used, zero-inited
    
    FSM_dispatch(&mCapturerController, &ev);
}

void METH(portIsStopped)(void){
    CtrlEvent ev;
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_ERROR, "renderercontroller : portIsStopped" );
#endif     
    mIsDmaRunning  = false;  
   
    ev.fsmEvent.signal        = CTRL_SETSTATECOMPLETE_SIG;
    ev.args.cmdComplete.state = mOmxState;
    ev.args.cmdComplete.portIdx = 0; // not used, zero-inited
    
    FSM_dispatch(&mCapturerController, &ev);
}


