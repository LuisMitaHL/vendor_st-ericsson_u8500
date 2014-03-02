/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 /** @file ispproc_sm.cpp
    @brief This file implements state transitions for isp proc state machine
*/

#define DBGT_PREFIX "HSMISPPROC"

#include "hsm.h" // rename_me "common_sm.h
#include "hsmispproc.h" // rename_me "ispproc_sm.h"
#include "VhcElementDefs.h"

//extern "C"{
//#include <cm/inc/cm_macros.h>
#include <cm/inc/cm.hpp>
//}

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_hsmispprocTraces.h"
#endif



SCF_STATE ISPPROC_SM::ProcessPendingEvents(s_scf_event const *e)
{
	switch (e->sig) {

		case SCF_STATE_ENTRY_SIG:
		{
			MSG0("ISPPROC_ProcessPendingEvents-ENTRY\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "ISPPROC_ProcessPendingEvents-ENTRY", (&mENSComponent));
			pResourceSharerManager->mRSPing.Ping(0);
			return 0;
		}

		case Q_DEFERRED_EVENT_SIG:
		{
			MSG0("ProcessPendingEvents-Q_DEFERRED_EVENT_SIG\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "ProcessPendingEvents-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
			return 0;
		}
		case Q_PONG_SIG:
		{
			MSG0("ProcessPendingEvents-Q_PONG_SIG\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "ProcessPendingEvents-Q_PONG_SIG", (&mENSComponent));
            s_scf_event event;
			OMX_BOOL prior = OMX_TRUE;
			MSG0("NOW Dequeue prior event and process it\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "NOW Dequeue prior event and process it", (&mENSComponent));
			pDeferredEventMgr->dequeuePriorEvent(&event);

			if(event.sig == SHARED_RETURN_SIG) {
				prior = OMX_FALSE;
				MSG0("No prior events thus Dequeue pending event and process it\n");
				OstTraceFiltStatic0 (TRACE_DEBUG, "No prior events thus Dequeue pending event and process it", (&mENSComponent));
				pDeferredEventMgr->dequeuePendingEvent(&event);
			}

			MSG2("Event to be processed signal =%d (%s)\n", event.sig,CError::stringSMSig(event.sig));
			OstTraceFiltStatic1 (TRACE_DEBUG, "Event to be processed =%d", (&mENSComponent), event.sig);

			switch (event.sig)
			{

				//case Q_FREED_SIG:
				case Q_LOCK_OK_SIG:
					SCF_TRANSIT_TO(&ISPPROC_SM::Freed);
					break;

				case ISPPROC_EXIT_CAPTURE_SIG:
				{
					SCF_TRANSIT_TO(&ISPPROC_SM::ExitingRawCapture);
					break;
				}
			    case SHARED_RETURN_SIG:
			    {
					OMX_STATETYPE state = pOmxStateMgr->updateState();
					if(state != OMX_StateMax) {
						MSG0("state change\n");
						OstTraceFiltStatic0 (TRACE_DEBUG, "state change", (&mENSComponent));
						mENSComponent.eventHandler(OMX_EventCmdComplete, OMX_CommandStateSet, (OMX_U32)state);
					}
					SCF_TRANSIT_TO(&ISPPROC_SM::WaitEvent);
					break;
			    }
				case ISPPROC_STRIPE_SIG:
				{
					SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture);
					break;
				}
				case UPDATE_PORT_FOR_ROTATION_SIG:
				{
					//mENSComponent.setRotation(rotation);
					SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture);
					break;
				}

			    default:
			    {
					if(prior == OMX_FALSE) pDeferredEventMgr->requeueEventAtSameLocation(&event);
					else pDeferredEventMgr->requeuePriorEventAtSameLocation(&event);

					// generate a LIST_INFO_SIG so that the event is not known here and directed to the mother class ;-)

					pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0,0);
					pIspctlCom->readQueue();

                    SM_PUSH_STATE_STATIC(&ISPPROC_SM::ProcessPendingEvents);

					break;
			    }
			}
			return 0;
		}

	   case SCF_STATE_EXIT_SIG:
		MSG0("ProcessPendingEvents-EXIT\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "ProcessPendingEvents-EXIT", (&mENSComponent));
		return 0;
       default: break;
	}

	return COM_SM::ProcessPendingEvents(e);
}


void ISPPROC_SM::SW3A_start(SCF_PSTATE whatNext){
        DBC_ASSERT(0);
}
