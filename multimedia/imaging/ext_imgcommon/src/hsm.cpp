/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ext_hsm.h"
#include "ext_error.h"

#undef   LOG_TAG
#define  LOG_TAG DBGT_TAG
#define DBGT_LAYER  3
#undef  DBGT_PREFIX
#define DBGT_PREFIX "GRBC"
#include "debug_trace.h"

s_scf_event const pkgStdEvt[] = {
    { sig:SCF_PARENT_SIG,      type:{other:0}, {0}},
    { sig:SCF_STATE_INIT_SIG,  type:{other:0}, {0}},
    { sig:SCF_STATE_ENTRY_SIG, type:{other:0}, {0}},
    { sig:SCF_STATE_EXIT_SIG,  type:{other:0}, {0}}
};


/*
 * COM_SM Constructor, it takes the ens Component to which it belongs as parameter.
 */


COM_SM::COM_SM(ENS_Component &enscomp)
:CScfStateMachine()
    ,mENSComponent(enscomp)
{
    SCF_REGISTER_TOPSTATE_INIT(&COM_SM::initial);
}

/* COM_SM destructor */
COM_SM::~COM_SM() {
}

/*
 * hsm framework due.
 * This is called when the user is calling hsm.init() function.
 * It initializes the whole state machine. OMX_Executing is the main state.
 */
/* JMV: I think this is only needed in case of MS */
SCF_STATE  COM_SM::initial(s_scf_event const *) {
    SCF_INIT(&COM_SM::OMX_Executing);
    return 0;
}

/*
 * hsm framework due.
 * Not used today.
 */
SCF_STATE  COM_SM::final(s_scf_event const *e) {
    switch (e->sig)
    {
    case SCF_STATE_ENTRY_SIG :
        return 0;
    default: break;
    }
    return SCF_STATE_PTR(&COM_SM::top);
}

/*
 * HSM main state.
 * All the inherited class will inherits from this state.
 * All will have a first state "Booting", that needs to be overwritten by inherited class as this state is pure virtual.
 */
SCF_STATE  COM_SM::OMX_Executing(s_scf_event const *e)
{
    switch (e->sig)
    {
    case SCF_STATE_ENTRY_SIG: ENTRY; return 0;
    case SCF_STATE_EXIT_SIG:  EXIT;  return 0;
    case SCF_STATE_INIT_SIG:  INIT; SCF_INIT(&COM_SM::Booting); return 0;
    case SCF_PARENT_SIG: break;
    case TIMEOUT_SIG: timeoutAction(); return 0;
    case Q_LOCK_OK_SIG:
        {
            /* In that case, the event has not been yet put in the queue of differed event */
            s_scf_event * event = pDeferredEventMgr->getEmptyEvent();
            event->sig = Q_LOCK_OK_SIG;

            pDeferredEventMgr->queuePriorEvent(event);
            SCF_TRANSIT_TO(&COM_SM::ProcessPendingEvents);

            return 0;
        }
    default:
        DBGT_PTRACE("Discarded Event 0x%x (%s)",e->sig, CError::stringSMSig(e->sig));
        return 0;
    }
    return SCF_STATE_PTR(&COM_SM::top);
}


/*
 * As child of OMX_Executing state, this state is inherited from the inherited class of HSM.
 * It will lead to the ProcessPendingEvents() state machine which is also shared to all inherited HSM class.
 */
SCF_STATE COM_SM::Streaming(s_scf_event const *e)
{
    switch (e->sig)
    {
    case SCF_STATE_ENTRY_SIG:ENTRY; return 0;
    case SCF_STATE_INIT_SIG: SCF_INIT(&COM_SM::ProcessPendingEvents); return 0;
    case SCF_STATE_EXIT_SIG: INIT;EXIT;return 0;
    case Q_DEFERRED_EVENT_SIG: return 0;
    case SCF_PARENT_SIG: break;
    default:
        break;
    }
    return SCF_STATE_PTR(&COM_SM::OMX_Executing);
}



/*
 * This state is the mother of  ProcessPendingEvents()
 * This state is the "sleep" state : when no events have to be processed by a component, then it is in WaitEvent state.
 * As soon as an event arises then it is directed to the ProcessPendingEvents() state, which will manage it.
 */
SCF_STATE COM_SM::WaitEvent(s_scf_event const *e)
{


    switch (e->sig)
    {
    case SCF_STATE_ENTRY_SIG:
        {
            ENTRY;

            t_uint16 state = pOmxStateMgr->updateState();
            if(state != OMX_StateMax) mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, state);

            return 0;


        }
    case SCF_STATE_EXIT_SIG:  EXIT; return 0;
    case Q_DEFERRED_EVENT_SIG:
        {
            SCF_TRANSIT_TO(&COM_SM::ProcessPendingEvents);
            return 0;
        }
    case Q_FINAL_SIG:
        {
            SCF_TRANSIT_TO(&COM_SM::final);
            return 0;
        }
    case SCF_PARENT_SIG: break;
    case SCF_STATE_INIT_SIG: break;
    default:
        break;
    }
    return SCF_STATE_PTR(&COM_SM::Streaming);
}

/*
 * This state is the state which manages the differed events.
 * This state is virtual.
 * All class which inherits have to implement their own state if they want to manage dedicated signal.
 * Shared signal could be put there.
 */
SCF_STATE COM_SM::ProcessPendingEvents(s_scf_event const *e)
{
    switch (e->sig)
    {
    case SCF_STATE_ENTRY_SIG:
        {
            DBGT_PTRACE("COM_ProcessPendingEvents-Q_DEFERRED_EVENT_SIG");
            /* TODO : This is needed to be commented because this is done in child state (mean state of the HSM Child),
             * but how to manage it if one child only needs the shared one and does not override it ?
             */
            //pIspctlCom->requestPE(DeviceParameters_uwDeviceId_MSByte);
            pExtIspctlCom->requestPE(0x0000);
            return 0;
        }
    case Q_DEFERRED_EVENT_SIG:
        {
            DBGT_PTRACE("ProcessPendingEvents-Q_DEFERRED_EVENT_SIG");
            return 0;
        }
    case EVT_ISPCTL_INFO_SIG:
        {
            DBGT_PTRACE("ProcessPendingEvents-EVT_ISPCTL_INFO_SIG");
            /* Dequeue the event*/
            s_scf_event * event;
            DBGT_PTRACE("NOW Dequeue prior event and process it");
            event = pDeferredEventMgr->dequeuePriorEvent();
            if(event->sig == SHARED_RETURN_SIG) {
                DBGT_PTRACE("No prior events thus Dequeue pending event and process it");
                event = pDeferredEventMgr->dequeuePendingEvent();
            }
            DBGT_PTRACE("Event to be processed =%d (%s)", event->sig,CError::stringSMSig(event->sig));

            /* Process the event */
            switch (event->sig)
            {
            case SHARED_RETURN_SIG:
                {
                    SCF_TRANSIT_TO(&COM_SM::WaitEvent);
                    break;
                }
            default :
                {
                    /* if it's none of known event, at that stage, then it means it has been corrupted, assert it. */
                    DBGT_PTRACE("ProcessPendingEvents-%d;",event->sig);
                    DBC_ASSERT(0);
                }
            }
            return 0;
        }
    case SCF_STATE_EXIT_SIG: EXIT; return 0;
    default: break;
    }
    return SCF_STATE_PTR(&COM_SM::WaitEvent);
}

/* */
/* HSM::timeoutAction
 * actions taken if timeout happens
 * overloaded in HSMCAM class
 */
/**/
void COM_SM::timeoutAction (void)
{

    DBGT_PTRACE("Discarded Timeout Event");

}
