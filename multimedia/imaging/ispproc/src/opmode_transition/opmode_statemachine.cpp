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
#include "ispproc_src_opmode_transition_opmode_statemachineTraces.h"
#endif



/* */
/* EnteringPreview state machine */
/**/

SCF_STATE ISPPROC_SM::EnteringPreview(s_scf_event const *e) {
   switch (e->sig) {
   case SCF_STATE_ENTRY_SIG:
   {
	   MSG0("EnteringPreview-ENTRY;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview-ENTRY;", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_EXIT_SIG:
   {
	   MSG0("EnteringPreview-EXIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview-EXIT;", (&mENSComponent));
	   return 0;
   }
   case SCF_STATE_INIT_SIG:
   {
	   MSG0("EnteringPreview-INIT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview-INIT;", (&mENSComponent));
	   return 0;
   }
   case EVT_ISPCTL_ERROR_SIG:
	{
		DBGT_ERROR("EnteringPreview-EVT_ISPCTL_INFO_SIG;\n");
		OstTraceFiltStatic0 (TRACE_ERROR, "EnteringPreview-EVT_ISPCTL_INFO_SIG;", (&mENSComponent));
		if (e->type.ispctlError.error_id == ISP_POLLING_TIMEOUT_ERROR)
		{
			DBGT_ERROR("EnteringPreview-ISP_POLLING_TIMEOUT_ERROR;\n");
			OstTraceFiltStatic0 (TRACE_ERROR, "EnteringPreview-ISP_POLLING_TIMEOUT_ERROR;", (&mENSComponent));
		}
        return 0;
	}
   case Q_DEFERRED_EVENT_SIG:
   {
	   MSG0("EnteringPreview-DEFERRED_EVENT;\n");
	   OstTraceFiltStatic0 (TRACE_DEBUG, "EnteringPreview-DEFERRED_EVENT;", (&mENSComponent));
	   return 0;
   }
   default: break;
   }

   return SCF_STATE_PTR(&ISPPROC_SM::EnteringOpMode);//Up_And_Running
}


