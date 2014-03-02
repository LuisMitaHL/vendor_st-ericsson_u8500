/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "hsmispproc.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_boot_statemachineTraces.h"
#endif


SCF_STATE ISPPROC_SM::Booting(s_scf_event const *e) {
switch (e->sig) {
case SCF_STATE_ENTRY_SIG:
	MSG0("Booting-ENTRY\n");
	OstTraceFiltStatic0 (TRACE_DEBUG, "Booting-ENTRY", (&mENSComponent));
		return 0;
	case SCF_STATE_EXIT_SIG:
			MSG0("Booting-SCF_STATE_EXIT_SIG\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "Booting-SCF_STATE_EXIT_SIG", (&mENSComponent));
	return 0;
	case SCF_STATE_INIT_SIG:
	{
		MSG0("Booting-SCF_STATE_INIT_SIG\n");
		OstTraceFiltStatic0 (TRACE_DEBUG, "Booting-SCF_STATE_INIT_SIG", (&mENSComponent));
		SCF_INIT(&ISPPROC_SM::BootingStart); return 0;
	}
	case SCF_PARENT_SIG:break;
	default:
	{
		//MSG1("ISPPROC_SM::Booting\n : %d\n", e->sig);
		//OstTraceFiltStatic1 (TRACE_DEBUG, "ISPPROC_SM::Booting\n : %d", (&mENSComponent), e->sig);
		break;
		//return 0;
	}	
}
return SCF_STATE_PTR(&ISPPROC_SM::OMX_Executing);
}


SCF_STATE  ISPPROC_SM::BootingStart(s_scf_event const *e){
switch (e->sig) {
	case SCF_STATE_ENTRY_SIG:
		{
			MSG0("BootingStart-ENTRY\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "BootingStart-ENTRY", (&mENSComponent));
		
			//pIspctlCom->requestPE(DeviceParameters_u16_DeviceId_Byte0);
			pResourceSharerManager->mRSPing.Ping(0);
			
		return 0;

		}
	case SCF_STATE_EXIT_SIG:
		{

			MSG0("BootingStart-SCF_STATE_EXIT_SIG\n");
			OstTraceFiltStatic0 (TRACE_DEBUG, "BootingStart-SCF_STATE_EXIT_SIG", (&mENSComponent));
		return 0;
		}
		/*
		case Q_LOCK_OK_SIG:
		case Q_FREED_SIG:
		{
			s_scf_event event;
			event.type.other=0;
			event.sig = Q_FREED_SIG;
			pDeferredEventMgr->queueNewEvent(&event);
			SCF_TRANSIT_TO(&ISPPROC_SM::ProcessPendingEvents);
			return 0;
		}		*/
		
	case Q_PONG_SIG:	
	//case EVT_ISPCTL_INFO_SIG:
		   {
			   MSG0("EVT_ISPCTL_INFO_SIG-SCF_STATE_EXIT_SIG\n");
			   OstTraceFiltStatic0 (TRACE_DEBUG, "EVT_ISPCTL_INFO_SIG-SCF_STATE_EXIT_SIG", (&mENSComponent));
				//if (e->type.ispctlInfo.info_id != ISP_READ_DONE) break;
				SCF_TRANSIT_TO(&ISPPROC_SM::WaitEvent);

				return 0;
		   }
	
	case SCF_PARENT_SIG:break;
	case SCF_STATE_INIT_SIG:break;
	default :
	{
		//MSG1("ISPPROC_SM::BootingStart\n : %d\n", e->sig);
		//OstTraceFiltStatic1 (TRACE_DEBUG, "ISPPROC_SM::BootingStart\n : %d", (&mENSComponent), e->sig);
		//return 0;
		break;
	}	
	}
	return SCF_STATE_PTR(&ISPPROC_SM::Booting);
}




