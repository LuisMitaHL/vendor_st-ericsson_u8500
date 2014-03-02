/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Component.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "fsm/component.nmf"
#include "fsm/generic/include/FSM.inl"
#include <omxcommand.idt>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "ens_interface_nmf_mpc_fsm_component_src_ComponentTraces.h"
#endif

static void idleState(Component * this, const OmxEvent *);
static void executingState(Component * this, const OmxEvent *);
static void pauseState(Component * this, const OmxEvent *);
static void transientToIdleState(Component * this, const OmxEvent *);
static void waitForPortFlushedOrDisabled(Component * this, const OmxEvent *);

static FSM_State getStateFromPorts(Component * this);

static void StateChangeIndication(OMX_STATETYPE oldState, OMX_STATETYPE newState) {
}

static void Reset(struct Component *this) {
}

OMX_STATETYPE Component_getState(const Component *this) {
    if (FSM_getState(this) == (FSM_State)idleState) {
        return OMX_StateIdle;
    } else if (FSM_getState(this) == (FSM_State)executingState) {
        return OMX_StateExecuting;
    } else if (FSM_getState(this) == (FSM_State)pauseState) {
        return OMX_StatePause;
    } else if (FSM_getState(this) == (FSM_State)transientToIdleState) {
        return OMX_StateTransientToIdle;
    } else if (FSM_getState(this) == (FSM_State)waitForPortFlushedOrDisabled) {
        return this->transientState;
    } else {
        OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component_getState assert !!");
        ASSERT(0);
        return 0;
    }
}

bool Component_allPortsDisabled(const Component *this) {
    int i;
    for (i = 0; i < this->portCount; i++) {
        if (Port_isEnabled(&this->ports[i]) == true) {
            return false;
        }
    }
    return true;
}

bool Component_allPortsEnabled(const Component *this) {
    int i;
    for (i = 0; i < this->portCount; i++) {
        if (Port_isEnabled(&this->ports[i]) == false) {
            return false;
        }
    }
    return true;
}

bool Component_isOnePortFlushing(const Component *this) {
    int i;
    for (i = 0; i < this->portCount; i++) {
        if (Port_isFlushing(&this->ports[i]) == true) {
            return true;
        }
    }
    return false;
}

static bool postProcessCheck(Component *this) {
    int i;
    for (i = 0; i < this->portCount; i++) {
        if (!Port_postProcessCheck(&this->ports[i])) {
            return false;
        }
    }
    return true;
}

#pragma noprefix
#pragma nopid
extern void *THIS;

void Component_eventHandler(
    Component *this, OMX_EVENTTYPE event, unsigned long data1, unsigned long data2) {
    void *savedTHIS = THIS;

    THIS = this->eventHandler->THIS;
    (*this->eventHandler->eventHandler)(event, data1, data2);

    THIS = savedTHIS;
}

void Component_init(
    Component *this,
    int portCount,
    Port *ports,
    const void *nmfitf) {

    OstTraceFiltInst0(TRACE_API, "ENS_MPC: Enter Component_init function");

    PRECONDITION(this->process != 0 && this->disablePortIndication != 0 &&
        this->enablePortIndication != 0 && this->flushPortIndication != 0)

    //NOTE: should be removed when API break delivered : "this->stateChangeIndication" should be != 0.
    if (this->stateChangeIndication == 0) {
        this->stateChangeIndication = StateChangeIndication;
    }
    if (this->reset == 0) {
        this->reset = Reset;
    }

    this->portCount      = portCount;
    this->ports          = ports;
    this->pendingEvent   = false;
    this->eventHandler   = nmfitf;
    this->transientState = OMX_StateLoaded;

    FSM_init(this, (FSM_State)idleState);
}

static void process(Component *this) {
    this->process(this);
    POSTCONDITION(postProcessCheck(this));
}

static FSM_State getStateFromPorts(Component *this) {
    FSM_State state = 0;
    int       i;
    int       executingPorts    = 0,
        idlePorts               = 0,
        pausedPorts             = 0,
        transientToIdlePorts    = 0,
        transientToDisablePorts = 0,
        flushingPorts           = 0;

    int portCount = this->portCount;

    for (i = 0; i < portCount; i++) {
        switch (Port_getState(&this->ports[i])) {
            case OMX_StateIdle:
                idlePorts++;
                break;

            case OMX_StatePause:
                pausedPorts++;
                break;

            case OMX_StateExecuting:
                executingPorts++;
                break;

            case OMX_StateTransientToIdle:
                transientToIdlePorts++;
                break;

            case OMX_StateTransientToDisable:
                transientToDisablePorts++;
                break;

            case OMX_StateFlushing:
                flushingPorts++;
                break;

            default:
                ASSERT(0);
                break;
        }
    }

    if (executingPorts == portCount) {
        state = (FSM_State)executingState;
    } else if (pausedPorts == portCount) {
        state = (FSM_State)pauseState;
    } else if (idlePorts == portCount) {
        state = (FSM_State)idleState;
    } else if (idlePorts + transientToIdlePorts == portCount) {
        state = (FSM_State)transientToIdleState;
    } else if (executingPorts + transientToDisablePorts == portCount ||
        pausedPorts + transientToDisablePorts == portCount ||
        executingPorts + flushingPorts == portCount ||
        pausedPorts + flushingPorts == portCount) {
        state = (FSM_State)waitForPortFlushedOrDisabled;
    } else {
        OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component_getStateFromPorts assert !!");
        ASSERT(0);
    }

    return state;
}

static void updateState(Component *this, FSM_State currentState) {
    FSM_State newState = 0;
    newState = getStateFromPorts(this);
    if (newState != currentState) {
        FSM_TRANSITION(newState);
    }
}

void setState(Component *this, const OmxEvent *evt, FSM_State currentState) {
    int i;

    for (i = 0; i < this->portCount; i++) {
        FSM_dispatch(&this->ports[i], evt);
    }

    updateState(this, currentState);
}

// component in idle state
// => all ports are in idle state
static void idleState(Component *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component IdleState FSM_ENTRY_SIG");
            this->stateChangeIndication(this->transientState, OMX_StateIdle);
            this->transientState = OMX_StateIdle;
            this->reset(this);
            Component_eventHandler(
            this, OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateIdle);
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component IdleState FSM_EXIT_SIG");
            break;

        case OMX_SETSTATE_SIG:
            setState(this, evt, (FSM_State)idleState);
            break;

        case OMX_PROCESS_SIG:
            break;

        case OMX_FLUSH_SIG:
        case OMX_DISABLE_PORT_SIG:
        case OMX_ENABLE_PORT_SIG:
        {
            int i;
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                for (i = 0; i < this->portCount; i++) {
                    Port *port = &this->ports[i];
                    FSM_dispatch(port, evt);
                }
            } else {
                Port *port = &this->ports[evt->args.portCmd.portIdx];
                FSM_dispatch(port, evt);
            }
        }
        break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component IdleState assert !!");
            ASSERT(0);
            break;
    }
}

// component in executing state
// => all ports are in executing state
static void executingState(Component *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component ExecutingState FSM_ENTRY_SIG");

            if (this->transientState  != OMX_StateExecuting) {
                this->stateChangeIndication(this->transientState, OMX_StateExecuting);
                this->transientState = OMX_StateExecuting;
                Component_eventHandler(
                this, OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StateExecuting);
            }

            //
            // we may already have some buffers to process for e.g
            // when transitionning from pause state
            process(this);
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component ExecutingState FSM_EXIT_SIG");
            break;

        case OMX_PROCESS_SIG:
            process(this);
            break;

        case OMX_RETURNBUFFER_SIG:
        {
            Port *port = &this->ports[evt->args.returnBuffer.portIdx];
            FSM_dispatch(port, evt);
        }
        break;

        case OMX_SETSTATE_SIG:
            setState(this, evt, (FSM_State)executingState);
            break;

        case OMX_FLUSH_SIG:
        case OMX_DISABLE_PORT_SIG:
        {
            int i;
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                for (i = 0; i < this->portCount; i++) {
                    Port *port = &this->ports[i];
                    FSM_dispatch(port, evt);
                }
            } else {
                Port *port = &this->ports[evt->args.portCmd.portIdx];
                FSM_dispatch(port, evt);
            }
            updateState(this, (FSM_State)executingState);
        }
        break;

        case OMX_ENABLE_PORT_SIG:
        {
            int i;
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                for (i = 0; i < this->portCount; i++) {
                    Port *port = &this->ports[i];
                    FSM_dispatch(port, evt);
                }
            } else {
                Port *port = &this->ports[evt->args.portCmd.portIdx];
                FSM_dispatch(port, evt);
            }
            process(this);     //Avoid race conditions when enabling ports in executing state!
        }
        break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component ExecutingState assert !!");
            ASSERT(0);
    }
}

// component in pause state
// => all ports are in pause state
static void pauseState(Component *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component PauseState FSM_ENTRY_SIG");
            if (this->transientState  != OMX_StatePause) {
                this->stateChangeIndication(this->transientState, OMX_StatePause);
                this->transientState = OMX_StatePause;
                Component_eventHandler(
                this, OMX_EventCmdComplete, OMX_CommandStateSet, OMX_StatePause);
            }
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component PauseState FSM_EXIT_SIG");
            break;

        case OMX_PROCESS_SIG:
            break;

        case OMX_RETURNBUFFER_SIG:
        {
            // no FIFO on return buffer path
            // so we must send them even in Pause state
            Port *port = &this->ports[evt->args.returnBuffer.portIdx];
            FSM_dispatch(port, evt);
        }
        break;

        case OMX_SETSTATE_SIG:
            setState(this, evt, (FSM_State)pauseState);
            break;

        case OMX_FLUSH_SIG:
        case OMX_DISABLE_PORT_SIG:
        {
            int i;
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                for (i = 0; i < this->portCount; i++) {
                    Port *port = &this->ports[i];
                    FSM_dispatch(port, evt);
                }
            } else {
                Port *port = &this->ports[evt->args.portCmd.portIdx];
                FSM_dispatch(port, evt);
            }
            updateState(this, (FSM_State)pauseState);
        }
        break;

        case OMX_ENABLE_PORT_SIG:
        {
            int i;
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                for (i = 0; i < this->portCount; i++) {
                    Port *port = &this->ports[i];
                    FSM_dispatch(port, evt);
                }
            } else {
                Port *port = &this->ports[evt->args.portCmd.portIdx];
                FSM_dispatch(port, evt);
            }
        }
        break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component PauseState assert !!");
            ASSERT(0);
    }
}

// component in transientToIdle state
// => all ports are either in transientToIdle or idle state
static void transientToIdleState(Component *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component TransientToIdleState FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component TransientToIdleState FSM_EXIT_SIG");
            break;

        case OMX_RETURNBUFFER_SIG:
        {

            Port *        port      = &this->ports[evt->args.returnBuffer.portIdx];
            OMX_STATETYPE currState = Port_getState(port);

            FSM_dispatch(port, evt);

            if (Port_getState(port) != currState) {
                updateState(this, (FSM_State)transientToIdleState);
            }
        }
        break;

        case OMX_PROCESS_SIG:
        {
            int  i;
            bool needStateUpdate = false;

            for (i = 0; i < this->portCount; i++) {
                Port *        port      = &this->ports[i];
                OMX_STATETYPE currState = Port_getState(port);
                if (Port_isEnabled(port)) {
                    FSM_dispatch(port, evt);
                    if (Port_getState(port) != currState) {
                        needStateUpdate = true;
                    }
                }
            }

            if (needStateUpdate) {
                updateState(this, (FSM_State)transientToIdleState);
            }
        }
        break;

        case OMX_FLUSH_SIG:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component TransientToIdleState FLUSH_SIG assert !!");
            ASSERT(0);
            break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component TransientToIdleState assert !!");
            ASSERT(0);
    }
}

// component in waitForPortFlushedOrDisabled state
// => one port at least is in transientToDisabledState or in transientToFlushState
static void waitForPortFlushedOrDisabled(Component *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component waitForPortFlushedOrDisabled FSM_ENTRY_SIG");
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst0(TRACE_FLOW, "ENS_MPC: Component waitForPortFlushedOrDisabled FSM_EXIT_SIG");
            break;

        case OMX_PROCESS_SIG:
        {
            int  i;
            bool portStateChanged = false;

            for (i = 0; i < this->portCount; i++) {
                Port *        port      = &this->ports[i];
                OMX_STATETYPE currState = Port_getState(port);
                if (currState == OMX_StateTransientToDisable ||
                    currState == OMX_StateFlushing) {
                    FSM_dispatch(port, evt);
                    if ((portStateChanged == false) && (Port_getState(port) != currState)) {
                        portStateChanged = true;
                    }
                }
            }
            if (portStateChanged == true) {
                updateState(this, (FSM_State)waitForPortFlushedOrDisabled);
            }

            if (this->transientState == OMX_StateExecuting) {
                process(this);
                updateState(this, (FSM_State)waitForPortFlushedOrDisabled);
            }
        }
        break;

        case OMX_RETURNBUFFER_SIG:
        {
            Port *        port      = &this->ports[evt->args.returnBuffer.portIdx];
            OMX_STATETYPE currState = Port_getState(port);
            FSM_dispatch(port, evt);
            updateState(this, (FSM_State)waitForPortFlushedOrDisabled);
        }
        break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Component waitForPortFlushedOrDisabled assert !!");
            ASSERT(0);
    }
}
