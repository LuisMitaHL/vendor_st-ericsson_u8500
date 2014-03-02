#ifndef _COMPONENT_INL_
#define _COMPONENT_INL_

#include "fsm/component/include/Component.h"
#include "inc/archi-wrapper.h"

static inline
void Component_scheduleProcessEvent(Component *component) {
    int need_process = 0;

    ENTER_CRITICAL_SECTION;
    if (!(component->pendingEvent)) {
        component->pendingEvent = true;
        need_process            = 1;
    }
    EXIT_CRITICAL_SECTION;

    if (need_process) {
        me.processEvent();
    }
}

static inline
void Component_processEvent(Component *component) {
    OmxEvent ev;

    component->pendingEvent = false;

    ev.fsmEvent.signal = OMX_PROCESS_SIG;

    FSM_dispatch(component, &ev);

}

static inline
void  Component_deliverBuffer(Component *component, int portIndex, Buffer_p buf) {
    Port_queueBuffer(&component->ports[portIndex], buf);
    Component_scheduleProcessEvent(component);
}

static inline
void  Component_deliverBufferCheck(Component *component, int portIndex, Buffer_p buf) {
    if (Port_isEnabled(&component->ports[portIndex])) {
        Component_deliverBuffer(component, portIndex, buf);
    } else {
        Port_returnUnprocessedBuffer(&component->ports[portIndex], buf);
    }
}

static inline
void Component_sendCommand(Component *component, OMX_COMMANDTYPE cmd, t_uword param) {
    OmxEvent ev;

    if ((cmd == OMX_CommandStateSet) ||
        (cmd == OMX_CommandPortDisable) ||
        (cmd == OMX_CommandPortEnable)) {
        /* Important to cast param to int in case param is OMX_ALL. In this case, as MMDSP
           is a 24bit machines, 0xFFFFFF is sent as 0x00FFFFFF to the Host if there is no
           cast. The cast is ok as the param is a state (that is less than 0x7FFFFF), or
           port index (possibly OMX_ALL) for the considered commands */
        Component_eventHandler(
            component, OMX_EventCmdReceived, cmd, (int)param);
    }

    if (cmd == OMX_CommandStateSet) {
        ev.fsmEvent.signal     = OMX_SETSTATE_SIG;
        ev.args.setState.state = (OMX_STATETYPE)param;
    } else if (cmd == OMX_CommandFlush) {
        ev.fsmEvent.signal = OMX_FLUSH_SIG;
        ASSERT((param == OMX_ALL) || (param < component->portCount));
        ev.args.portCmd.portIdx = param;
    } else if (cmd == OMX_CommandPortDisable) {
        ev.fsmEvent.signal = OMX_DISABLE_PORT_SIG;
        ASSERT((param == OMX_ALL) || (param < component->portCount));
        ev.args.portCmd.portIdx = param;
    } else if (cmd == OMX_CommandPortEnable) {
        ev.fsmEvent.signal = OMX_ENABLE_PORT_SIG;
        ASSERT((param == OMX_ALL) || (param < component->portCount));
        ev.args.portCmd.portIdx = param;
    } else { ASSERT(0); }

    FSM_dispatch(component, &ev);
}

#endif // _COMPONENT_INL_
