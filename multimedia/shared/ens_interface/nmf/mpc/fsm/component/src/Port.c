/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Port.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "fsm/component.nmf"
#include "fsm/generic/include/FSM.inl"
#include "fsm/component/include/Component.h"
#include <omxcommand.idt>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "ens_interface_nmf_mpc_fsm_component_src_PortTraces.h"
#endif

static void idleState(Port * this, const OmxEvent * evt);
static void executingState(Port * this, const OmxEvent * evt);
static void pauseState(Port * this, const OmxEvent * evt);
static void pauseFromIdleOrDisabledState(Port * this, const OmxEvent * evt);
static void transientToIdleOrDisabledState(Port * this, const OmxEvent * evt);
static void transientToFlushState(Port * this, const OmxEvent * evt);
static void disabledState(Port * this, const OmxEvent * evt);
static void transferQueuedBuffersToSharingPort(Port * this);
static void Port_returnBufferInternalNmf(Port * this, Buffer_p buf);

//---------------------------------------------------------------------
//              Public methods
//---------------------------------------------------------------------
void Port_init(
    Port *this,
    Direction dir,
    bool bufferSupplier,
    bool isHWport,
    Port *sharingPort,
    void *fifoBuf,
    int bufferCount,
    const void *nmfitf,
    int portIdx,
    bool isDisabled,
    bool isTunneled,
    Component *compTHIS) {

    FSM_State initialState;

    // trace init
    this->fsm.traceObject.mTraceInfoPtr = compTHIS->fsm.traceObject.mTraceInfoPtr;
    this->fsm.traceObject.mId1          = compTHIS->fsm.traceObject.mId1;

//	OstTraceFiltInst1(TRACE_API, "ENS_MPC: Enter Port_init funtion (component Id:%d)", ((TRACE_t *)compTHIS)->mId1);

    ASSERT(compTHIS != 0);

    this->direction               = dir;
    this->isBufferSupplier        = bufferSupplier;
    this->isHWport                = isHWport;
    this->eosReceived             = false;
    this->sharingPort             = sharingPort;
    this->portIdx                 = portIdx;
    this->componentTHIS           = compTHIS;
    this->flushState              = OMX_StateInvalid;
    this->notifyStateTransition   = true;
    this->isPortTunneled          = isTunneled;
    this->eventAppliedOnEachPorts = false;
    this->isBufferResetReq        = true;

    if (this->sharingPort && this->isBufferSupplier) {
        this->dequeuedBufferCount = bufferCount;
        Queue_init(&this->bufferQueue, fifoBuf, bufferCount, false, !isDisabled);
    } else if (this->isBufferSupplier) {
        this->dequeuedBufferCount = 0;
        Queue_init(&this->bufferQueue, fifoBuf, bufferCount, true, !isDisabled);
    } else {
        this->dequeuedBufferCount = 0;
        Queue_init(&this->bufferQueue, fifoBuf, bufferCount, false, !isDisabled);
    }

    this->nmfItf = nmfitf;

    if (isDisabled) {
        this->disabledState         = OMX_StateIdle;
        this->notifyStateTransition = false;
        FSM_init(this, (FSM_State)disabledState);
    } else {
        this->disabledState = OMX_StateInvalid;
        FSM_init(this, (FSM_State)idleState);
    }
}

OMX_STATETYPE Port_getState(const Port *this) {
    if (FSM_getState(this) == (FSM_State)idleState) {
        return OMX_StateIdle;
    } else if (FSM_getState(this) == (FSM_State)executingState) {
        return OMX_StateExecuting;
    } else if (FSM_getState(this) == (FSM_State)pauseState
        || FSM_getState(this) == (FSM_State)pauseFromIdleOrDisabledState) {
        return OMX_StatePause;
    } else if (FSM_getState(this) == (FSM_State)transientToIdleOrDisabledState) {
        if (this->disabledState == OMX_StateInvalid) {
            return OMX_StateTransientToIdle;
        } else {
            return OMX_StateTransientToDisable;
        }
    }   else if (FSM_getState(this) == (FSM_State)transientToFlushState) {
        return OMX_StateFlushing;
    } else if (FSM_getState(this) == (FSM_State)disabledState) {
        return this->disabledState;
    } else {
        OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Port_getState assert !!");
        ASSERT(0);
        return 0;
    }
}

void Port_setTunnelStatus(Port *this, bool isTunneled) {
    this->isPortTunneled = isTunneled;
}
/*FIXME: temporary patch to disable Buffer_reset call
 * To be removed after Wakeup camp */
void Port_setBufferResetReqStatus(Port *this, bool flag) {
    this->isBufferResetReq = flag;
}

void Port_queueBuffer(Port *this, Buffer_p buf) {
    if (buf->flags & BUFFERFLAG_EOS) {
        this->eosReceived = true;
    }
    Queue_push_back(&this->bufferQueue, buf);
}

Buffer_p Port_getBuffer(const Port *this, int n) {
    return Queue_getItem(&this->bufferQueue, n);
}

bool Port_isEnabled(const Port *this) {
    return Queue_getEnabled(&this->bufferQueue);
}

bool Port_isFlushing(const Port *this) {
    if (this->flushState != OMX_StateInvalid) {
        return true;
    }
    return false;
}

int Port_bufferCount(const Port *this) {
    return Queue_size(&this->bufferQueue);
}

int Port_queuedBufferCount(const Port *this) {
    return Queue_itemCount(&this->bufferQueue);
}

int Port_dequeuedBufferCount(const Port *this) {
    return this->dequeuedBufferCount;
}

void Port_returnBuffer(Port *this, Buffer_p buf) {
    OmxEvent evt;
    evt.fsmEvent.signal          = OMX_RETURNBUFFER_SIG;
    evt.args.returnBuffer.buffer = buf;
    FSM_dispatch(this, &evt);
}

void Port_returnUnprocessedBuffer(Port *this, Buffer_p buf) {
    Port_returnBufferInternalNmf(this, buf);
}

Buffer_p Port_dequeueBuffer(Port *this) {
    this->dequeuedBufferCount++;
    return Queue_pop_front(&this->bufferQueue);
}

void Port_requeueBuffer(Port *this, Buffer_p buf) {
    this->dequeuedBufferCount--;
    Queue_push_back(&this->bufferQueue, buf);
}

void Port_dequeueAndReturnBuffer(Port *this) {
    Buffer_p buf = Port_dequeueBuffer(this);
    Port_returnBuffer(this, buf);
}

bool Port_postProcessCheck(const Port *this) {
    if (this->isHWport) {
        // buffers are processed by HW - no check
        return true;
    }
    if (this->sharingPort) {
        if (Port_dequeuedBufferCount(this) >
            Port_bufferCount(this->sharingPort)
            - Port_dequeuedBufferCount(this->sharingPort)) {
            // all dequeued buffers not held by sharing port
            return false;
        }
    } else if (Port_dequeuedBufferCount(this)) {
        // only sharing ports are allowed to have dequeued buffers
        return false;
    }

    return true;
}

//---------------------------------------------------------------------
//              Private methods
//---------------------------------------------------------------------

void        Buffer_reset(Buffer_p this) {
    this->filledLen      = 0;
    this->byteInLastWord = 0;
    this->flags          = 0;
}

#pragma noprefix
#pragma nopid
extern void *THIS;

static void Port_returnBufferInternalNmf(Port *this, Buffer_p buf) {
    void *savedTHIS = THIS;
    THIS = this->nmfItf->THIS;
    (*this->nmfItf->deliverBuffer)(buf);

    THIS = savedTHIS;
}

static void Port_returnBufferInternal(Port *this, Buffer_p buf) {
    this->dequeuedBufferCount--;

    if (this->direction == InputPort) {
        if (this->isBufferResetReq) {
            if (this->isPortTunneled || (this->flushState == OMX_StateInvalid)) {
                Buffer_reset(buf);
            }
        }
    }

    Port_returnBufferInternalNmf(this, buf);
}

static void Port_reset(Port *this) {
    int i;

    if (this->isBufferSupplier && this->sharingPort) {
        PRECONDITION(Port_dequeuedBufferCount(this) == Port_bufferCount(this));
    } else if (this->isBufferSupplier) {
        PRECONDITION(Port_queuedBufferCount(this) == Port_bufferCount(this));
    } else {
        PRECONDITION(Port_queuedBufferCount(this) == 0
            && Port_dequeuedBufferCount(this) == 0);
    }

    for (i = 0; i < Port_queuedBufferCount(this); i++) {
        Buffer_reset(Port_getBuffer(this, i));
    }

}

static FSM_State translateState(OMX_STATETYPE state) {
    switch (state) {
        case OMX_StateIdle:         return (FSM_State)idleState;

        case OMX_StateExecuting:    return (FSM_State)executingState;

        case OMX_StatePause:        return (FSM_State)pauseState;

        default:
            ASSERT(0);
            return 0;
    }
}

void Port_flushBuffers(Port *this) {
    int i, queuedBufferCount;
    OstTraceFiltInst1(TRACE_API, "ENS_MPC: port %d Enter Port_flushBuffers function", this->portIdx);

    if (this->sharingPort && this->isBufferSupplier) {
        transferQueuedBuffersToSharingPort(this);
    } else {
        queuedBufferCount = Port_queuedBufferCount(this);

        for (i = 0; i < queuedBufferCount; i++) {
            Buffer_p buf = Port_dequeueBuffer(this);
            OstTraceFiltInst2(TRACE_DEBUG, "ENS_MPC: Port %d flushes buffer addr:0x%x", this->portIdx, (unsigned int)buf);
            Port_returnBufferInternal(this, buf);
        }
    }
}

static void portFlushComplete(Port *this, bool fromTransientState) {

    this->componentTHIS->flushPortIndication(this->portIdx);
    this->flushState = OMX_StateInvalid;

    if (this->eventAppliedOnEachPorts == false) {
        Component_eventHandler(
            this->componentTHIS, OMX_EventCmdComplete, OMX_CommandFlush, this->portIdx);
    } else if (Component_isOnePortFlushing(this->componentTHIS) == false) {
        if (fromTransientState
            || this->portIdx == (unsigned)(this->componentTHIS->portCount - 1)) {
            Component_eventHandler(
                this->componentTHIS, OMX_EventCmdComplete, OMX_CommandFlush, ((int)(OMX_ALL)));
        }
    }
    this->eventAppliedOnEachPorts = false;
}

static void idleState(Port *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d IdleState FSM_ENTRY_SIG", this->portIdx);
            Port_reset(this);
            break;

        case FSM_EXIT_SIG:
        {
            FSM_State targetState = this->fsm.state;
            if (targetState == (FSM_State)executingState
                && this->isBufferSupplier
                && this->direction == InputPort) {
                // buffer-supplier input ports must send all their
                // buffers when transitionning from idle to executing
                Port_flushBuffers(this);
            }
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d IdleState FSM_EXIT_SIG", this->portIdx);
        }
        break;

        case OMX_SETSTATE_SIG:
        {
            FSM_State newState = translateState(evt->args.setState.state);
            PRECONDITION(newState != (FSM_State)idleState);

            if (newState == (FSM_State)pauseState
                && this->isBufferSupplier
                && this->direction == InputPort) {
                FSM_TRANSITION(pauseFromIdleOrDisabledState);
            } else {
                FSM_TRANSITION(newState);
            }
        }
        break;

        case OMX_PROCESS_SIG:
            break;

        case OMX_FLUSH_SIG:
            if (evt->args.portCmd.portIdx != OMX_ALL) {
                Component_eventHandler(
                this->componentTHIS, OMX_EventCmdComplete, OMX_CommandFlush, this->portIdx);
            } else if (this->portIdx == (this->componentTHIS->portCount - 1)) {
                Component_eventHandler(
                this->componentTHIS, OMX_EventCmdComplete, OMX_CommandFlush, ((int)(OMX_ALL)));
            }
            break;

        case OMX_DISABLE_PORT_SIG:
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            this->disabledState = OMX_StateIdle;
            FSM_TRANSITION(disabledState);
            break;

        default:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_MPC: Port %d idleState assert !!", this->portIdx);
            ASSERT(0);
            break;
    }
}

static void transferQueuedBuffersToSharingPort(Port *this) {
    int      i, nbuf;
    Buffer_p buf;
    nbuf = Port_queuedBufferCount(this);

    for (i = 0; i < nbuf; i++) {
        buf            = Port_dequeueBuffer(this);
        buf->filledLen = 0;
        Port_returnBuffer(this->sharingPort, buf);
    }
}

static void goToFlushBuffers(Port *this) {
    ASSERT(this->flushState != OMX_StateInvalid);

    if (this->isBufferSupplier && this->sharingPort) {

        transferQueuedBuffersToSharingPort(this);

        if (Port_dequeuedBufferCount(this) == Port_bufferCount(this)) {
            // all buffers returned to sharing port so complete the flush
            portFlushComplete(this, false);
        } else {
            // need to wait for missing buffers to be returned by tunneled port
            // so go to transient state
            FSM_TRANSITION(transientToFlushState);
        }
    } else if (this->isBufferSupplier) {
        if (Port_queuedBufferCount(this) == Port_bufferCount(this)) {
            // all buffers at port queue so complete the flush
            portFlushComplete(this, false);
        } else {
            // need to wait for missing buffers to be returned by tunneled port
            // so go to transient state
            FSM_TRANSITION(transientToFlushState);
        }
    } else {
        // non-supplier port
        if (Port_dequeuedBufferCount(this) == 0) {
            // no dequeued buffers so all buffers are
            // either queued or held by tunneled port
            Port_flushBuffers(this);
            portFlushComplete(this, false);
        } else {
            // need to wait for dequeued buffers to be returned before
            // completing the command, so go to transient state
            FSM_TRANSITION(transientToFlushState);
        }
    }
}

static void goToIdleOrDisable(Port *this) {
    FSM_State targetState;
    if (this->disabledState == OMX_StateInvalid) {
        targetState = (FSM_State)idleState;
    } else {
        targetState = (FSM_State)disabledState;
    }

    if (this->isBufferSupplier && this->sharingPort) {

        transferQueuedBuffersToSharingPort(this);

        if (Port_dequeuedBufferCount(this) == Port_bufferCount(this)) {
            // all buffers returned to sharing port
            FSM_TRANSITION(targetState);
        } else {
            FSM_TRANSITION(transientToIdleOrDisabledState);
        }
    } else if (this->isBufferSupplier) {
        if (Port_queuedBufferCount(this) == Port_bufferCount(this)) {
            FSM_TRANSITION(targetState);
        } else {
            FSM_TRANSITION(transientToIdleOrDisabledState);
        }
    } else {
        // non-supplier port
        if (Port_dequeuedBufferCount(this) == 0) {
            // no dequeued buffers so all buffers are
            // either queued or held by tunneled port
            Port_flushBuffers(this);
            FSM_TRANSITION(targetState);
        } else {
            FSM_TRANSITION(transientToIdleOrDisabledState);
        }
    }
}

static void executingState(Port *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d ExecutingState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d ExecutingState FSM_EXIT_SIG", this->portIdx);

            break;

        case OMX_SETSTATE_SIG:
        {
            FSM_State newState = translateState(evt->args.setState.state);
            PRECONDITION(newState != (FSM_State)executingState);

            if (newState == (FSM_State)idleState) {
                goToIdleOrDisable(this);
            } else if (newState == (FSM_State)pauseState) {
                FSM_TRANSITION(pauseState);
            }
        }
        break;

        case OMX_RETURNBUFFER_SIG:
            Port_returnBufferInternal(this, evt->args.returnBuffer.buffer);
            break;

        case OMX_FLUSH_SIG:
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            this->flushState = OMX_StateExecuting;
            goToFlushBuffers(this);
            break;

        case OMX_DISABLE_PORT_SIG:
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            this->disabledState = OMX_StateExecuting;
            goToIdleOrDisable(this);
            break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Port executingState assert !!");
            ASSERT(0);
            break;
    }
}

static void pauseState(Port *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d PauseState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d PauseState FSM_EXIT_SIG", this->portIdx);
            break;

        case OMX_SETSTATE_SIG:
        {
            FSM_State newState = translateState(evt->args.setState.state);

            if (newState == (FSM_State)idleState) {
                goToIdleOrDisable(this);
            } else if (newState == (FSM_State)executingState) {
                FSM_TRANSITION(executingState);
            }
        }
        break;

        case OMX_RETURNBUFFER_SIG:
            Port_returnBufferInternal(this, evt->args.returnBuffer.buffer);
            break;

        case OMX_FLUSH_SIG:
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            this->flushState = OMX_StatePause;
            goToFlushBuffers(this);
            break;

        case OMX_DISABLE_PORT_SIG:
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            this->disabledState = OMX_StatePause;
            goToIdleOrDisable(this);
            break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Port pauseState assert !!");
            ASSERT(0);
            break;
    }
}

static void pauseFromIdleOrDisabledState(Port *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d pauseFromIdleOrDisabledState FSM_ENTRY_SIG", this->portIdx);
            ASSERT(this->isBufferSupplier && this->direction == InputPort);
            break;

        case FSM_EXIT_SIG:
        {
            FSM_State targetState = this->fsm.state;
            if (targetState == (FSM_State)executingState) {
                // buffer-supplier input ports must send all their
                // buffers when transitionning from idle to executing
                Port_flushBuffers(this);
            }
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d pauseFromIdleOrDisabledState FSM_EXIT_SIG", this->portIdx);

        }
        break;

        case OMX_SETSTATE_SIG:
        {
            FSM_State newState = translateState(evt->args.setState.state);

            if (newState == (FSM_State)idleState) {
                goToIdleOrDisable(this);
            } else if (newState == (FSM_State)executingState) {
                FSM_TRANSITION(executingState);
            }
        }
        break;

        case OMX_RETURNBUFFER_SIG:
            Port_returnBufferInternal(this, evt->args.returnBuffer.buffer);
            break;

        case OMX_FLUSH_SIG:
            if (evt->args.portCmd.portIdx != OMX_ALL) {
                Component_eventHandler(
                this->componentTHIS, OMX_EventCmdComplete, OMX_CommandFlush, this->portIdx);
            } else if (this->portIdx == (this->componentTHIS->portCount - 1)) {
                Component_eventHandler(
                this->componentTHIS, OMX_EventCmdComplete, OMX_CommandFlush, ((int)(OMX_ALL)));
            }
            break;

        case OMX_DISABLE_PORT_SIG:
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            this->disabledState = OMX_StatePause;
            FSM_TRANSITION(disabledState);
            break;

        default:
            OstTraceFiltInst0(TRACE_ERROR, "ENS_MPC: Port pauseFromIdleOrDisabledState assert !!");
            ASSERT(0);
            break;
    }
}

static void transientToIdleOrDisabledState(Port *this, const OmxEvent *evt) {

    FSM_State targetState;
    if (this->disabledState == OMX_StateInvalid) {
        targetState = (FSM_State)idleState;
    } else {
        targetState = (FSM_State)disabledState;
    }

    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d transientToIdleOrDisabledState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d transientToIdleOrDisabledState FSM_EXIT_SIG", this->portIdx);

            break;

        case OMX_PROCESS_SIG:
            if (this->isBufferSupplier && this->sharingPort) {

                transferQueuedBuffersToSharingPort(this);

                if (Port_dequeuedBufferCount(this) == Port_bufferCount(this)) {
                    // all buffers returned to sharing port
                    FSM_TRANSITION(targetState);
                }
            } else if (this->isBufferSupplier) {
                if (Port_queuedBufferCount(this) == Port_bufferCount(this)) {
                    // all buffers are queued
                    FSM_TRANSITION(targetState);
                }
            }
            break;

        case OMX_RETURNBUFFER_SIG:
            if (this->isBufferSupplier) {
                // dequeued buffers on buffer supplier sharing ports
                // should never occur since the port reuses buffers from
                // its sharing port
                ASSERT(!this->sharingPort);
                // buffer is queued since we are transitionning to idle
                Port_queueBuffer(this, evt->args.returnBuffer.buffer);
                if (Port_queuedBufferCount(this) == Port_bufferCount(this)) {
                    // all buffers are queued
                    FSM_TRANSITION(targetState);
                }
            } else {
                Port_returnBufferInternal(this, evt->args.returnBuffer.buffer);
                if (Port_dequeuedBufferCount(this) == 0) {
                    // all buffers are either queued or held by
                    // tunneled port
                    Port_flushBuffers(this);
                    FSM_TRANSITION(targetState);
                }
            }
            break;

        default:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_MPC: Port %d transientToIdleOrDisabledState assert !!", this->portIdx);
            ASSERT(0);
            break;

    }
}
static void transientToFlushState(Port *this, const OmxEvent *evt) {

    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d transientToFlushState FSM_ENTRY_SIG", this->portIdx);
            break;

        case FSM_EXIT_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d transientToFlushState FSM_EXIT_SIG", this->portIdx);
            break;

        case OMX_PROCESS_SIG:
            if (this->isBufferSupplier && this->sharingPort) {

                transferQueuedBuffersToSharingPort(this);

                if (Port_dequeuedBufferCount(this) == Port_bufferCount(this)) {
                    // all buffers returned to sharing port so complete the command
                    FSM_State nextState = translateState(this->flushState);
                    portFlushComplete(this, true);
                    FSM_TRANSITION(nextState);
                }
            } else if (this->isBufferSupplier) {
                if (Port_queuedBufferCount(this) == Port_bufferCount(this)) {
                    // all buffers are at port queue so complete the command
                    FSM_State nextState = translateState(this->flushState);
                    portFlushComplete(this, true);
                    FSM_TRANSITION(nextState);
                }
            }
            break;

        case OMX_RETURNBUFFER_SIG:
            Port_returnBufferInternal(this, evt->args.returnBuffer.buffer);
            if (Port_dequeuedBufferCount(this) == 0) {
                // all buffers are either queued or held by
                // tunneled port so complete the command
                FSM_State nextState = translateState(this->flushState);
                Port_flushBuffers(this);
                portFlushComplete(this, true);
                FSM_TRANSITION(nextState);
            }
            break;

        default:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_MPC: Port %d transientToFlushState assert !!", this->portIdx);
            ASSERT(0);
            break;

    }
}

static void disabledState(Port *this, const OmxEvent *evt) {
    switch (OmxEvent_signal(evt)) {
        case FSM_ENTRY_SIG:
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d DisabledState FSM_ENTRY_SIG", this->portIdx);
            if (this->notifyStateTransition) {
                Port_reset(this);
                Queue_setEnabled(&this->bufferQueue, false);
                this->componentTHIS->disablePortIndication(this->portIdx);
                if (this->eventAppliedOnEachPorts == false) {
                    Component_eventHandler(
                    this->componentTHIS, OMX_EventCmdComplete, OMX_CommandPortDisable, this->portIdx);
                } else if (Component_allPortsDisabled(this->componentTHIS)) {
                    Component_eventHandler(
                    this->componentTHIS, OMX_EventCmdComplete, OMX_CommandPortDisable, ((int)(OMX_ALL)));
                }
                this->eventAppliedOnEachPorts = false;
            } else {
                Queue_setEnabled(&this->bufferQueue, false);
                this->notifyStateTransition = true;
            }
            break;

        case FSM_EXIT_SIG:
        {
            FSM_State targetState = this->fsm.state;
            if (targetState == (FSM_State)executingState
                && this->isBufferSupplier
                && this->direction == InputPort) {
                // buffer-supplier input ports must send all their
                // buffers when transitionning from idle to executing
                Port_flushBuffers(this);
            }
            this->componentTHIS->enablePortIndication(this->portIdx);
            if (this->eventAppliedOnEachPorts == false) {
                Component_eventHandler(
                    this->componentTHIS, OMX_EventCmdComplete, OMX_CommandPortEnable, this->portIdx);
            } else if (Component_allPortsEnabled(this->componentTHIS)) {
                Component_eventHandler(
                    this->componentTHIS, OMX_EventCmdComplete, OMX_CommandPortEnable, ((int)(OMX_ALL)));
            }
            this->eventAppliedOnEachPorts = false;
            OstTraceFiltInst1(TRACE_FLOW, "ENS_MPC: Port %d DisabledState FSM_EXIT_SIG", this->portIdx);
        }
        break;

        case OMX_SETSTATE_SIG:
        {
            PRECONDITION(evt->args.setState.state != this->disabledState);
            this->disabledState = evt->args.setState.state;
        }
        break;

        case OMX_ENABLE_PORT_SIG:
        {
            FSM_State newState = translateState(this->disabledState);
            this->disabledState = OMX_StateInvalid;
            if (evt->args.portCmd.portIdx == OMX_ALL) {
                this->eventAppliedOnEachPorts = true;
            }
            Queue_setEnabled(&this->bufferQueue, true);
            if (newState == (FSM_State)pauseState
                && this->isBufferSupplier
                && this->direction == InputPort) {
                FSM_TRANSITION(pauseFromIdleOrDisabledState);
            } else {
                if (newState == (FSM_State)executingState
                    && this->isBufferSupplier
                    && this->direction == InputPort) {
                    Port_flushBuffers(this);
                }
                FSM_TRANSITION(newState);
            }
        }
        break;

        default:
            OstTraceFiltInst1(TRACE_ERROR, "ENS_MPC: Port %d disabledState assert !!", this->portIdx);
            ASSERT(0);
            break;
    }
}
