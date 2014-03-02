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
#include "ispproc_src_port_statemachineTraces.h"
#endif


SCF_STATE ISPPROC_SM::EnablePortInIdleExecutingState(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
   {
	   MSG0("EnablePortInIdleExecutingState-ENTRY\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnablePortInIdleExecutingState-ENTRY", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("EnablePortInIdleExecutingState-SCF_STATE_EXIT_SIG\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnablePortInIdleExecutingState-SCF_STATE_EXIT_SIG", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_INIT_SIG:
   {
	   MSG0("EnablePortInIdleExecutingState-SCF_STATE_INIT_SIG\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnablePortInIdleExecutingState-SCF_STATE_INIT_SIG", (&mENSComponent));
	   return 0;
   }
   case Q_DEFERRED_EVENT_SIG:
   {
	   MSG0("EnablePortInIdleExecutingState-Q_DEFERRED_EVENT_SIG\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnablePortInIdleExecutingState-Q_DEFERRED_EVENT_SIG", (&mENSComponent));
	   return 0;
   }
   default: break;

   }
   return SCF_STATE_PTR(&ISPPROC_SM::OMX_Executing);//Up_And_Running
}

