/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 /** @file common_sm.cpp
    @brief This file implements all state transitions
    that are common to ISP and Camera state machines
*/

#include "hsm.h" // rename_me "common_sm.h"
#include "error.h"
#include "omxilosalservices.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_hsmTraces.h"
#endif

s_scf_event const pkgStdEvt[] = {
   { SCF_PARENT_SIG, {0}, {0}},
   { SCF_STATE_INIT_SIG,  {0}, {0}},
   { SCF_STATE_ENTRY_SIG, {0}, {0}},
   { SCF_STATE_EXIT_SIG,  {0}, {0}}
};


COM_SM::COM_SM(ENS_Component &enscomp)
    :CScfStateMachine()
    ,mENSComponent(enscomp)
{

    whatNext = NULL;
    SCF_REGISTER_TOPSTATE_INIT(&COM_SM::initial);

    //performance traces start
    memset(&(mTime),0,sizeof(mTime));
    //performance traces end

    ConfiguringISPPipe_ConfigType = ConfiguringISPPipe_All;
    ControlingISP_ControlType = ControlingISP_SendBoot_WaitBootComplete;
    bTrace_latency = OMX_FALSE;
    enabledPort = 0;
    mTime.days = 0;
    mTime.h = 0;
    mTime.min = 0;
    mTime.s = 0;
    mTime.us = 0;
    mTime_e_OMXCAM_StartStreaming_t0.days = 0;
    mTime_e_OMXCAM_StartStreaming_t0.h = 0;
    mTime_e_OMXCAM_StartStreaming_t0.min = 0;
    mTime_e_OMXCAM_StartStreaming_t0.s = 0;
    mTime_e_OMXCAM_StartStreaming_t0.us = 0;
    mTime_e_OMXCAM_StartStreaming_t1.days = 0;
    mTime_e_OMXCAM_StartStreaming_t1.h = 0;
    mTime_e_OMXCAM_StartStreaming_t1.min = 0;
    mTime_e_OMXCAM_StartStreaming_t1.s = 0;
    mTime_e_OMXCAM_StartStreaming_t1.us = 0;
    mTime_e_OMXCAM_StopISP_t0.days = 0;
    mTime_e_OMXCAM_StopISP_t0.h = 0;
    mTime_e_OMXCAM_StopISP_t0.min = 0;
    mTime_e_OMXCAM_StopISP_t0.s = 0;
    mTime_e_OMXCAM_StopISP_t0.us = 0;
    mTime_e_OMXCAM_StopISP_t1.days = 0;
    mTime_e_OMXCAM_StopISP_t1.h = 0;
    mTime_e_OMXCAM_StopISP_t1.min = 0;
    mTime_e_OMXCAM_StopISP_t1.s = 0;
    mTime_e_OMXCAM_StopISP_t1.us = 0;
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
 * COM_SM main state.
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
		case TIMEOUT_SIG:
		{
			MSG0("TIMEOUT_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "TIMEOUT_SIG", (&mENSComponent));
			timeoutAction();
			return 0;
		}
		case Q_LOCK_OK_SIG:
		{
			/* In that case, the event has not been yet put in the queue of differed event */
            s_scf_event event;
            event.type.other=0;
			event.sig = Q_LOCK_OK_SIG;

			pDeferredEventMgr->queuePriorEvent(&event);
			SCF_TRANSIT_TO(&COM_SM::ProcessPendingEvents);

			return 0;
		}	   
		default:
			MSG2("Discarded Event 0x%x (%s)\n",e->sig, CError::stringSMSig(e->sig));
			OstTraceFiltStatic1(TRACE_DEBUG, "Discarded Event 0x%x", (&mENSComponent),e->sig);
			return 0;
	}
	return SCF_STATE_PTR(&COM_SM::top);
}


/*
* As child of OMX_Executing state, this state is inherited from the inherited class of COM_SM.
* It will lead to the ProcessPendingEvents() state machine which is also shared to all inherited COM_SM class.
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
			MSG0("COM_ProcessPendingEvents-Q_DEFERRED_EVENT_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "COM_ProcessPendingEvents-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
			/* TODO : This is needed to be commented because this is done in child state (mean state of the COM_SM Child),
			 * but how to manage it if one child only needs the shared one and does not override it ?
			 */
			//pIspctlCom->requestPE(DeviceParameters_uwDeviceId_MSByte);
			return 0;
		}
		case Q_DEFERRED_EVENT_SIG:
		{
			MSG0("ProcessPendingEvents-Q_DEFERRED_EVENT_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ProcessPendingEvents-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
			return 0;
		}
		case EVT_ISPCTL_LIST_INFO_SIG:
		{
			MSG0("ProcessPendingEvents-EVT_ISPCTL_INFO_SIG\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "ProcessPendingEvents-EVT_ISPCTL_INFO_SIG", (&mENSComponent));
			/* Dequeue the event*/
			s_scf_event event;
			MSG0("NOW Dequeue prior event and process it\n");
			OstTraceFiltStatic0(TRACE_DEBUG, "NOW Dequeue prior event and process it", (&mENSComponent));
			pDeferredEventMgr->dequeuePriorEvent(&event);
			if(event.sig == SHARED_RETURN_SIG) {
				MSG0("No prior events thus Dequeue pending event and process it\n");
				OstTraceFiltStatic0(TRACE_DEBUG, "No prior events thus Dequeue pending event and process it", (&mENSComponent));
				pDeferredEventMgr->dequeuePendingEvent(&event);
			}
			MSG2("Event to be processed =%d (%s)\n", event.sig,CError::stringSMSig(event.sig));
			OstTraceFiltStatic1(TRACE_DEBUG, "Event to be processed =%d", (&mENSComponent), event.sig);

			/* Process the event */
			switch (event.sig)
			{
				case SHARED_READLISTPE_DEBUG_SIG:
				{
					MSG0("ProcessPendingEvents-READLISTPE_DEBUG_SIG;\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "ProcessPendingEvents-READLISTPE_DEBUG_SIG;", (&mENSComponent));
					pTrace->iPEMode = READ_LIST_OF_PE;
					pTrace->bCallbackNeeded = OMX_TRUE;
					pTrace->iListOfPeToRead = event.type.readStruct;

					SCF_TRANSIT_TO(&COM_SM::SHARED_Debugging);
					break;
				}
				case SHARED_WRITELISTPE_SIG:
				{
					MSG0("ProcessPendingEvents-WRITELISTPE_SIG;\n");
					OstTraceFiltStatic0(TRACE_DEBUG, "ProcessPendingEvents-WRITELISTPE_SIG;", (&mENSComponent));
					pTrace->iPEMode = WRITE_LIST_OF_PE;
					pTrace->bCallbackNeeded = OMX_TRUE;
					pTrace->iListOfPeToWrite = event.type.writeStruct;					

					SCF_TRANSIT_TO(&COM_SM::SHARED_Debugging);
					break;
				}
				case SHARED_RETURN_SIG:
				{
					SCF_TRANSIT_TO(&COM_SM::WaitEvent);
					break;
				}
				default :
				{
					/* if it's none of known event, at that stage, then it means it has been corrupted, assert it. */
					MSG1("ProcessPendingEvents-%d;\n",event.sig);
					OstTraceFiltStatic1(TRACE_DEBUG, "ProcessPendingEvents-%d;", (&mENSComponent),event.sig);
					//DBC_ASSERT(0);
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
/* COM_SM::timeoutAction
 * actions taken if timeout happens
 * overloaded in CAM_SM class
 */
/**/
bool COM_SM::timeoutAction (void)
{
	MSG0("Discarded Timeout Event\n");
	OstTraceFiltStatic0(TRACE_DEBUG, "Discarded Timeout Event", (&mENSComponent));
	return FALSE;
}

//#define HSM_STATES_OVERWRITE_DBG
#ifdef HSM_STATES_OVERWRITE_DBG
#include <stdio.h>
#endif /* HSM_STATES_OVERWRITE_DBG */

/** Push the state return point to be used the next time a dynamic state transition should be taken.
 * Current implementation does not support nested jumps.
 * return: 
 *   - false: push is successful
 *   - true: push is failed: Either because the queue is full or because the new return point is the same as the one previously set.
 */ 
bool  COM_SM::pushNextState(SCF_PSTATE next)
{ 
#ifdef HSM_STATES_OVERWRITE_DBG
    printf("whatNext Log: Pushing %s from %s.\n", get_state_str(next), get_state_str(getCurrentState()));
#endif /* HSM_STATES_OVERWRITE_DBG */

    if (whatNext == NULL) {
        whatNext = next;
        return false;
    } else {
        if (whatNext == next) {
#ifdef HSM_STATES_OVERWRITE_DBG
            printf("whatNext Error: Trying to overwrite the return point with the same value. No visible impact expected, but potential issue under the hood. %s\n", get_state_str(next));
#endif /* HSM_STATES_OVERWRITE_DBG */
            DBGT_ERROR("whatNext Error: Trying to overwrite the return point with the same value. No visible impact expected, but potential issue under the hood. %s\n", get_state_str(next));
            OstTraceFiltStatic0(TRACE_ERROR, "whatNext Error: Trying to overwrite the return point with the same value. No visible impact expected, but potential issue under the hood.", (&mENSComponent));
            return false; // Fixme: 'true' should be returned as soon as the state overwrites are removed from hsmcamera.
        } else {
#ifdef HSM_STATES_OVERWRITE_DBG
            printf("whatNext Error: State flow corruption: overwriting %s by %s.\n", get_state_str(whatNext), get_state_str(next));
#endif /* HSM_STATES_OVERWRITE_DBG */
            DBGT_ERROR("whatNext Error: State flow corruption: overwriting %s by %s.\n", get_state_str(whatNext), get_state_str(next));
            OstTraceFiltStatic0(TRACE_ERROR, "whatNext Error: State flow corruption: overwriting ...", (&mENSComponent));
            if (whatNext == SCF_STATE_PTR(&COM_SM::ProcessPendingEvents)) { // Replacing processPendingEvent shall be forbidden. Allow it temporarily to allow regression testing continuity.
#ifdef HSM_STATES_OVERWRITE_DBG
                printf("whatNext Error: Temporarily allow to continue, for regression testability purpose.\n");
#endif /* HSM_STATES_OVERWRITE_DBG */
                DBGT_ERROR("whatNext Error: Temporarily allow to continue, for regression testability purpose.\n");
                OstTraceFiltStatic0(TRACE_ERROR, "whatNext Error: Temporarily allow to continue, for regression testability purpose.", (&mENSComponent));
                whatNext = next;
                return false;
            } else {
                return true;
            }
        }
    }
}

/** Pop the next state return point.
 * Return NULL if the queue is empty. This is not necessarily an error. It could just denote that a static state transition will take over.
 */ 
SCF_PSTATE  COM_SM::popNextState()
{ 
    SCF_PSTATE ret_state = whatNext;
    whatNext = NULL;
    return ret_state; 
}


/* prints status of the state machine */
void COM_SM::printStatus()
{
#ifdef DEBUG_CHECKALIVE
    printf("Current state = %s\n", getCurrentState()->Name());
    //OstTraceFiltStatic1(TRACE_DEBUG, "What next = %s", (&mENSComponent), get_state_str(whatNext));
#endif
}

void COM_SM::latencyMeasure(IFM_TIME* pTime)
{
	OMX_U64 u64_time =  omxilosalservices::OmxILOsalTimer::GetSystemTime();
	pTime->days = u64_time/24/60/60/1000000;
	pTime->h = (u64_time - pTime->days*24*60*60*1000000)/60/60/1000000;
	pTime->min = (u64_time - (pTime->days*24+pTime->h)*60*60*1000000)/60/1000000;
	pTime->s = (u64_time - ((pTime->days*24+pTime->h)*60+pTime->min)*60*1000000)/1000000;
	pTime->us = u64_time - (((pTime->days*24+pTime->h)*60+pTime->min)*60+pTime->s)*1000000;
}
