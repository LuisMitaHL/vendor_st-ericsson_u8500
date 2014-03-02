/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   renderercontroller.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <audiocodec/mpc/renderercontroller.nmf>
#include <stdbool.h>
#include <dbc.h>
#include "fsm/generic/include/FSM.inl"
#include "fsm/component/include/OmxEvent.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_renderercontroller_src_renderercontrollerTraces.h"
#endif

//#define DEBUG_TRACE

#define INPUT_TRACE_ID         (1)
#define FEEDBACK_TRACE_ID      (2)

#define OMX_IN_IDX             (0)
#define OMX_OUT_IDX            (1)

#define CALL_INTERFACE_IF_EXISTS(a,b) if (!IS_NULL_INTERFACE(a,b)) { a.b(); } else { ASSERT(0); }

typedef enum {
    PORT_STATUS_DISABLE_REQUESTED,
    PORT_STATUS_DISABLED,
    PORT_STATUS_ENABLE_REQUESTED,
    PORT_STATUS_ENABLED
} ePortStatus;



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

static inline CtrlEvent_signal(const CtrlEvent *evt) { return evt->fsmEvent.signal; }

#define MAX_PORTS  2
#define ALL_PORTS_TUNNELED 0xFFFF

static bool isPortEnabled[MAX_PORTS] = {true, true};

static void omxState(FSM *this, const CtrlEvent *);
static void setStateTransientState(FSM *this, const CtrlEvent *);
static void setStateTransientDisableState(FSM *this, const CtrlEvent *evt);
static void setStateTransientEnableState(FSM *this, const CtrlEvent *evt);

////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////

typedef enum {
    CTRL_RENDERER_SETSTATECOMPLETE_SIG = OMX_LAST_SIG,
    CTRL_SETSTATECOMPLETE_SIG,
    CTRL_DISABLEPORTCOMPLETE_SIG,
    CTRL_ENABLEPORTCOMPLETE_SIG
} ControllerSignal;

static FSM              mRendererController;

static OMX_STATETYPE    mOmxState;
static t_uint32         inputInTargetState;
static t_uint32         feedbackInTargetState;
static t_uint32         inputIsDisabled;
static t_uint32         inputDisableRequested;
static t_uint32         feedbackIsDisabled;
static t_uint32         feedbackDisableRequested;
static bool             shouldTracePortStatus=0;
static bool             mIsDmaRunning  = false;  
static bool             mInputStateCmdReceived;
static bool             mFeedbackStateCmdReceived;

////////////////////////////////////////////////////////////

static void 
RendererController_init() {
    // at initialization, the controller cannot switch into idle state 
    // before knowing the state of other components 
    // => start in transient state with OMX_StateIdle as target state
    mOmxState = OMX_StateIdle;
    inputInTargetState = 0;
    feedbackInTargetState = 0;
    inputIsDisabled=0;
    inputDisableRequested=0;
    feedbackIsDisabled=0;
    feedbackDisableRequested=0;
    mIsDmaRunning  = false;  
    mInputStateCmdReceived = false;
    mFeedbackStateCmdReceived = false;

    FSM_init(&mRendererController, (FSM_State) setStateTransientState);
}




static bool
allComponentsInTargetState(void) {
    int i;
    bool answer = true;

    if (!IS_NULL_INTERFACE(feedbackblk_sendcommand,sendCommand )) {
        if (inputInTargetState && feedbackInTargetState) {
            answer= true;
        } else {
            answer= false;
        }
    }
    else{
        if (inputInTargetState) {
            answer = true;
        } else {
            answer= false;
        }
    }
    if (answer) {
#ifdef DEBUG_TRACE         
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : all Components in target state ? YES ");
#endif     
    }
    else{
#ifdef DEBUG_TRACE 
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : all Components NOT in target state");
#endif     
    }
    return answer;

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
// controller in one of the OMX states
static void 
omxState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
#ifdef DEBUG_TRACE  
             OstTraceInt0(TRACE_DEBUG, "renderercontroller : omxState FSM_ENTRY_SIG (Nothing to do)");
#endif 
            
             break;

        case FSM_EXIT_SIG:
#ifdef DEBUG_TRACE               
             OstTraceInt0(TRACE_DEBUG, "renderercontroller : omxState FSM_EXIT_SIG (Nothing to do)");
#endif 
            
             break;

        case OMX_SETSTATE_SIG:
#ifdef DEBUG_TRACE               
             OstTraceInt1(TRACE_DEBUG, "renderercontroller : omxState OMX_SETSTATE_SIG state=%d. dispatching command on input and feedback ports",evt->args.setState.state);
#endif 
            
             inputInTargetState = 0;
            feedbackInTargetState = 0;
            mOmxState = evt->args.setState.state;
            if (!IS_NULL_INTERFACE(feedbackblk_sendcommand,sendCommand )) {
                feedbackblk_sendcommand.sendCommand(OMX_CommandStateSet, mOmxState);
               
                if((mOmxState==OMX_StateExecuting) && isPortEnabled[OMX_OUT_IDX]){  
                    CALL_INTERFACE_IF_EXISTS(feedback_dma_state,enablePort);
                }
                else if (mOmxState==OMX_StateIdle){
                    CALL_INTERFACE_IF_EXISTS(feedback_dma_state,disablePort);
                }
            }
            inputblk_sendcommand.sendCommand(OMX_CommandStateSet, mOmxState);
            
            if((mOmxState==OMX_StateExecuting) && isPortEnabled[OMX_IN_IDX] ){
                dma_state.enablePort();                                    
            }
            else if ((mOmxState==OMX_StateIdle)&& isPortEnabled[OMX_IN_IDX] ){
                dma_state.disablePort();                
            }

            FSM_TRANSITION(setStateTransientState);
            break;

        case OMX_DISABLE_PORT_SIG:
            {
                unsigned int portIdx = evt->args.portCmd.portIdx;
                PRECONDITION(portIdx == OMX_IN_IDX || portIdx == OMX_OUT_IDX );
                isPortEnabled[portIdx] = false;

                if (portIdx == OMX_OUT_IDX) {
#ifdef DEBUG_TRACE 
                    OstTraceInt0(TRACE_DEBUG, "renderercontroller : omxState OMX_DISABLE_PORT_SIG dispatching command on feedback port");
#endif 
                    
                    if (!IS_NULL_INTERFACE(feedbackblk_sendcommand,sendCommand )) {
                        feedbackblk_sendcommand.sendCommand(OMX_CommandPortDisable, OMX_ALL);
                        CALL_INTERFACE_IF_EXISTS(feedback_dma_state,disablePort);
                    }
                    else{
                        ASSERT(0);
                    }
                }
                else if (portIdx == OMX_IN_IDX) {
#ifdef DEBUG_TRACE                     
                    OstTraceInt1(TRACE_DEBUG, "renderercontroller : omxState OMX_DISABLE_PORT_SIG dispatching command on input port",portIdx);
#endif 
                    
                    inputblk_sendcommand.sendCommand(OMX_CommandPortDisable, OMX_ALL);
#ifdef DEBUG_TRACE                     
                    OstTraceInt0(TRACE_DEBUG, "renderercontroller : omxState OMX_DISABLE_PORT_SIG dispatching command on input port");
#endif 
                    
                    if(mOmxState==OMX_StateExecuting){
                        dma_state.disablePort();
                    }
                }
#ifdef DEBUG_TRACE 
                OstTraceInt0(TRACE_DEBUG, "renderercontroller : transitionning to disabled");
#endif 
                
                FSM_TRANSITION(setStateTransientDisableState);
            }
            break;

        case OMX_ENABLE_PORT_SIG:
            {
                unsigned int portIdx = evt->args.portCmd.portIdx;
                PRECONDITION(portIdx == OMX_IN_IDX || portIdx == OMX_OUT_IDX );
                isPortEnabled[portIdx] = true;
                if (portIdx == OMX_OUT_IDX) {
#ifdef DEBUG_TRACE 
                    OstTraceInt1(TRACE_DEBUG, "renderercontroller : omxState OMX_ENABLE_PORT dispatching command on feedback port",portIdx);
#endif 
                    
                    if (!IS_NULL_INTERFACE(feedbackblk_sendcommand,sendCommand )) {
                        feedbackblk_sendcommand.sendCommand(OMX_CommandPortEnable, OMX_ALL);
                        if(mOmxState==OMX_StateExecuting){
                            CALL_INTERFACE_IF_EXISTS(feedback_dma_state,enablePort);
                        }
                    }
                    else{
                        ASSERT(0);
                    }
                }
                else if (portIdx == OMX_IN_IDX) {
#ifdef DEBUG_TRACE 
                    OstTraceInt1(TRACE_DEBUG, "renderercontroller : omxState OMX_ENABLE_PORT_SIG dispatching command on input port",portIdx);
#endif                     
                    inputblk_sendcommand.sendCommand(OMX_CommandPortEnable, OMX_ALL);
                    if(mOmxState==OMX_StateExecuting){
                        dma_state.enablePort();
                    }
                }
                FSM_TRANSITION(setStateTransientEnableState);
            }
            break;
        case CTRL_SETSTATECOMPLETE_SIG:
            //NOTHING TO DO 
            // Called after portIsRunning
            break;

        default:
            ASSERT(0);
    }
}

// controller has sent a SetState command to all components
// => waiting for CmdComplete events from all components
//
static void 
setStateTransientState(FSM *this, const CtrlEvent *evt) {
    switch (CtrlEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
#ifdef DEBUG_TRACE 
             OstTraceInt0(TRACE_DEBUG, "renderercontroller : setStateTransientState FSM_ENTRY_SIG (Nothing to do)");
#endif             
             break;

        case FSM_EXIT_SIG:
#ifdef DEBUG_TRACE              
             OstTraceInt0(TRACE_DEBUG, "renderercontroller : setStateTransientState FSM_EXIT_SIG (Nothing to do)");
#endif             
             break;

        case CTRL_SETSTATECOMPLETE_SIG: 
            { 
#ifdef DEBUG_TRACE                 
                OstTraceInt0(TRACE_DEBUG, "renderercontroller : setStateTransientState CTRL_SETSTATECOMPLETE_SIG");
#endif                 
                if (allComponentsInTargetState() && isDmaReadyForStateChange()) {
#ifdef DEBUG_TRACE                 
                    OstTraceInt1(TRACE_DEBUG, "renderercontroller : all components are in target (%d) omxstate, sending eventhandler to proxy",mOmxState);
#endif                     
                    FSM_TRANSITION(omxState);
                    proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, mOmxState);
                }
                else {
#ifdef DEBUG_TRACE 
                    OstTraceInt1(TRACE_DEBUG, "renderercontroller : all components are NOT in target (%d) omxstate",mOmxState);
#endif                 
                }
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
            {
#ifdef DEBUG_TRACE 
            OstTraceInt0(TRACE_DEBUG, "renderercontroller : setStateTransientDisableState FSM_ENTRY_SIG (nothing to do)");
#endif             
            break;
            }
        case FSM_EXIT_SIG:
            {
#ifdef DEBUG_TRACE             
                OstTraceInt0(TRACE_DEBUG, "renderercontroller : setStateTransientDisableState FSM_EXIT_SIG (nothing to do)");
#endif             
                break;
            }

        case CTRL_DISABLEPORTCOMPLETE_SIG:
            ASSERT(evt->args.cmdComplete.portIdx == OMX_IN_IDX || evt->args.cmdComplete.portIdx == OMX_OUT_IDX );
#ifdef DEBUG_TRACE             
            OstTraceInt1(TRACE_DEBUG, "renderercontroller : disable port complete for port (%d) omxstate",evt->args.cmdComplete.portIdx );
#endif            
            // conversion of port indexes was already done in event handler
            proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortDisable, evt->args.cmdComplete.portIdx);
            FSM_TRANSITION(omxState);
            break;
        case CTRL_SETSTATECOMPLETE_SIG:
            //NOTHING TO DO 
            // Called after portIsRunning
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
            ASSERT(evt->args.cmdComplete.portIdx == OMX_IN_IDX ||evt->args.cmdComplete.portIdx == OMX_OUT_IDX );
            // conversion of port indexes was already done in event handler
            proxy.eventHandler(OMX_EventCmdComplete, OMX_CommandPortEnable,  evt->args.cmdComplete.portIdx);
            FSM_TRANSITION(omxState);
            break;
        case CTRL_SETSTATECOMPLETE_SIG:
            //NOTHING TO DO 
            // Called after portIsRunning
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
	fsmInit_t init_feedback;

    FSM_traceInit(&mRendererController, initFsm.traceInfoAddr,initFsm.id1 );

    RendererController_init();
	

    isPortEnabled[OMX_IN_IDX]  = ! (initFsm.portsDisabled & 0x1);
    isPortEnabled[OMX_OUT_IDX] = ! (initFsm.portsDisabled & 0x2);

    // Omx port 0 is connected to volctrl that is connected to dmaout input. Enable or disable both ports of volctrl accordingly
    init_input.portsDisabled = initFsm.portsDisabled & 0x1 ? 0x3 : 0x0;
    init_input.portsTunneled = ALL_PORTS_TUNNELED;
    init_input.traceInfoAddr = initFsm.traceInfoAddr;
    init_input.id1           = initFsm.id1+INPUT_TRACE_ID;

    inputblk_init.fsmInit(init_input);

    // Omx port 1 is connected to volctrl that is connected to dmaout output. Enable or disable both ports of volctrl accordingly
    init_feedback.portsDisabled = initFsm.portsDisabled & 0x2 ? 0x3 : 0x0;
    	
    init_feedback.portsTunneled = ALL_PORTS_TUNNELED;
    init_feedback.traceInfoAddr = initFsm.traceInfoAddr;
    init_feedback.id1           = initFsm.id1+FEEDBACK_TRACE_ID;
    if (!IS_NULL_INTERFACE(feedbackblk_init,fsmInit)) {
        feedbackblk_init.fsmInit(init_feedback);
    }

}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    CtrlEvent ev;
#ifdef DEBUG_TRACE  
    OstTraceInt2(TRACE_DEBUG, "renderercontroller : sendCommand cmd=%d param=%d", cmd,param);
#endif     

    if (cmd == OMX_CommandStateSet) {
        mInputStateCmdReceived = false;
        mFeedbackStateCmdReceived = false;
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

    FSM_dispatch(&mRendererController, &ev);
}


void METH(inputblk_eventHandler)(
				 OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    CtrlEvent ev;
#ifdef DEBUG_TRACE  
    OstTraceInt3(TRACE_DEBUG, "renderercontroller : inputblk_eventHandler Event=%d data1=%d data2=%d", event,data1,data2);
#endif 
    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        OMX_STATETYPE state = (OMX_STATETYPE) data2;
#ifdef DEBUG_TRACE  
        OstTraceInt1(TRACE_ERROR, "renderercontroller :OMX_CommandStateSet OMX_EventCmdComplete, state=%d ",state );
#endif         
        ev.fsmEvent.signal = CTRL_SETSTATECOMPLETE_SIG;
        ev.args.cmdComplete.state = state;
        ev.args.cmdComplete.portIdx = 0; // not used, zero-inited
        inputInTargetState = 1;
        FSM_dispatch(&mRendererController, &ev);

    } 
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {   
#ifdef DEBUG_TRACE  
        OstTraceInt1(TRACE_ERROR, "renderercontroller :OMX_CommandStateSet OMX_EventCmdReceived, state=%d ",data2 );
#endif  
        mInputStateCmdReceived = true;
        if (IS_NULL_INTERFACE(feedbackblk_sendcommand,sendCommand )) {
            proxy.eventHandler(OMX_EventCmdReceived, data1, data2);            
        } else if (!IS_NULL_INTERFACE(feedbackblk_sendcommand,sendCommand ) &&
                mFeedbackStateCmdReceived == true) {
            proxy.eventHandler(OMX_EventCmdReceived, data1, data2);
        }
    }
    else if (event == OMX_EventBufferFlag ) {
        proxy.eventHandler(event, OMX_IN_IDX, data2 /* should be only EOS so far */);
    } 
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortDisable) {
        // Conversion from port idx to OMX idx
#ifdef DEBUG_TRACE  
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : disable input port complete");
#endif         
        ev.fsmEvent.signal = CTRL_DISABLEPORTCOMPLETE_SIG;
        ev.args.cmdComplete.portIdx = OMX_IN_IDX;
        FSM_dispatch(&mRendererController, &ev);
    }
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortEnable) {
        // Conversion from port idx to OMX idx
#ifdef DEBUG_TRACE          
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : enable input port complete");
#endif         
        ev.fsmEvent.signal = CTRL_ENABLEPORTCOMPLETE_SIG;
        ev.args.cmdComplete.portIdx = OMX_IN_IDX;
        FSM_dispatch(&mRendererController, &ev);
    }
    else if (event == OMX_EventCmdReceived && (data1 == OMX_CommandPortDisable || data1 == OMX_CommandPortEnable)) {
        // Conversion from port idx to OMX idx
#ifdef DEBUG_TRACE          
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : disable/enable input port command received");
#endif 
        proxy.eventHandler(event, data1, OMX_IN_IDX);
    }
    else if (event == OMX_EventError) {
#ifdef DEBUG_TRACE          
        OstTraceInt2(TRACE_ERROR, "renderercontroller : error received from input block : data1=%d, data2=%d",data1,data2 );
#endif         
        proxy.eventHandler(OMX_EventError, data1, data2);
    } 
    else {
        ASSERT(0);
    }
}

void METH(feedbackblk_eventHandler)(
				    OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    CtrlEvent ev;
#ifdef DEBUG_TRACE      
    OstTraceInt3(TRACE_DEBUG, "renderercontroller : feedbackblk_eventHandler Event=%d data1=%d data2=%d", event,data1,data2);
#endif 
    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        ev.fsmEvent.signal = CTRL_SETSTATECOMPLETE_SIG;
        ev.args.cmdComplete.state = (OMX_STATETYPE) data2;
        ev.args.cmdComplete.portIdx = 0; // not used, zero-inited
        feedbackInTargetState = 1;
        FSM_dispatch(&mRendererController, &ev);
    } 
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {   
#ifdef DEBUG_TRACE  
        OstTraceInt1(TRACE_ERROR, "renderercontroller :OMX_CommandStateSet OMX_EventCmdReceived, state=%d ",data2 );
#endif  
        mFeedbackStateCmdReceived = true;
        if(mInputStateCmdReceived == true) {
            proxy.eventHandler(OMX_EventCmdReceived, data1, data2);
        }
    }
    else if (event == OMX_EventBufferFlag ) {
        proxy.eventHandler(event, OMX_OUT_IDX, data2 /* should be only EOS so far */);
    } 
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortDisable) {
        // Conversion from port idx to OMX idx
#ifdef DEBUG_TRACE          
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : disable feedback port complete");
#endif 
        ev.fsmEvent.signal = CTRL_DISABLEPORTCOMPLETE_SIG;
        ev.args.cmdComplete.portIdx = OMX_OUT_IDX;
        FSM_dispatch(&mRendererController, &ev);
    }
    else if (event == OMX_EventCmdComplete && data1 == OMX_CommandPortEnable) {
#ifdef DEBUG_TRACE          
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : enable feedback port complete");
#endif         
        // Conversion from port idx to OMX idx
        ev.fsmEvent.signal = CTRL_ENABLEPORTCOMPLETE_SIG;
        ev.args.cmdComplete.portIdx = OMX_OUT_IDX;
        FSM_dispatch(&mRendererController, &ev);
    }
    else if (event == OMX_EventCmdReceived && (data1 == OMX_CommandPortDisable || data1 == OMX_CommandPortEnable)) {
        // Conversion from port idx to OMX idx
#ifdef DEBUG_TRACE          
        OstTraceInt0(TRACE_DEBUG, "renderercontroller : disable/enable feedback port command received");
#endif 
        proxy.eventHandler(event, data1, OMX_OUT_IDX);
    }
    else if (event == OMX_EventError) {
#ifdef DEBUG_TRACE         
        OstTraceInt2(TRACE_ERROR, "renderercontroller : error received from feedback block : data1=%d, data2=%d",data1,data2 );
#endif         
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
        
    FSM_dispatch(&mRendererController, &ev);
  
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
        
    FSM_dispatch(&mRendererController, &ev);
}


