/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "hsmispproc.h"
#include "IFM_Trace.h"
#include "ispproc.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "ispproc_src_freed_statemachineTraces.h"
#endif


SCF_STATE ISPPROC_SM::Freed(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
   {
	   MSG0("Freed-ENTRY\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed-ENTRY", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("Freed-SCF_STATE_EXIT_SIG\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed-SCF_STATE_EXIT_SIG", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_INIT_SIG:
   {
	   MSG0("Freed-SCF_STATE_INIT_SIG\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed-SCF_STATE_INIT_SIG", (&mENSComponent));
	   SCF_INIT(&ISPPROC_SM::Freed_StateMachine);
	   return 0;
   }
   case Q_DEFERRED_EVENT_SIG:
   {
	   MSG0("Freed-Q_DEFERRED_EVENT_SIG\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
	   return 0;
   }
   default: break;

   }
   return SCF_STATE_PTR(&ISPPROC_SM::OMX_Executing);//Up_And_Running
}

SCF_STATE ISPPROC_SM::Freed_StateMachine(s_scf_event const *e) {
	switch (e->sig) {

	   case SCF_STATE_ENTRY_SIG: {
		   MSG0("Freed_StateMachine-ENTRY\n");
		   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed_StateMachine-ENTRY", (&mENSComponent));
			pResourceSharerManager->mRSPing.Ping(0);
		   return 0;
	   }
	   case Q_PONG_SIG:
	   {
		   MSG0("Freed_StateMachine-Q_PONG_SIG\n");
		   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed_StateMachine-Q_PONG_SIG", (&mENSComponent));
		   IspProc* pIsp = (IspProc*)&mENSComponent.getProcessingComponent();
		   //performance traces start
		   latencyMeasure(&mTime);
		   pIsp->mlatency_Ispproc.e_OMXISPSNAP_Process.t1 = mTime;
		   pIsp->mlatency_Ispproc.e_OMXISPSTILL_Process.t1 = mTime;
		   OstTraceInt1 (TRACE_LATENCY, "e_OMXISPPROC_Process %d", 1);
		   //performance traces end

			captureRequest[ISPPROC_PORT_IN0]++;
            			
			ispprocport * 	port = (ispprocport *)  mENSComponent.getPort(ISPPROC_PORT_OUT1);
			if(port->getParamPortDefinition().bEnabled == OMX_TRUE) captureRequest[ISPPROC_PORT_OUT1]++;
	
			port = (ispprocport *)  mENSComponent.getPort(ISPPROC_PORT_OUT0);
			if(port->getParamPortDefinition().bEnabled == OMX_TRUE) captureRequest[ISPPROC_PORT_OUT0]++;			   
		   
		   isTransitionDone=OMX_FALSE;
		   SCF_TRANSIT_TO(&ISPPROC_SM::EnteringRawCapture);

		   return 0;
	   }


	   case SCF_STATE_EXIT_SIG:  {
		   MSG0("Freed_StateMachine-SCF_STATE_EXIT_SIG\n");
		   OstTraceFiltStatic0 (TRACE_DEBUG, "Freed_StateMachine-SCF_STATE_EXIT_SIG", (&mENSComponent));
		   return 0;
	   }
       default: break;

	}
	return SCF_STATE_PTR(&ISPPROC_SM::Freed);//Up_And_Running
}
