/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "hsm.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_opmode_transitionTraces.h"
#endif


/*
* OpModeTransition
 */
SCF_STATE COM_SM::OpModeTransition(s_scf_event const *e)
{
   switch (e->sig)
   {
	   case SCF_STATE_ENTRY_SIG:
	   {
		   /*MSG0("OpModeTransition-ENTRY\n");*/
		   /*OstTraceFiltStatic0(TRACE_DEBUG, "OpModeTransition-ENTRY", (&mENSComponent));*/
		   return 0;
	   }
	   case SCF_STATE_EXIT_SIG:
	   {
		   /*MSG0("OpModeTransition-EXIT\n"); */
		   /*OstTraceFiltStatic0(TRACE_DEBUG, "OpModeTransition-EXIT", (&mENSComponent)); */
		   return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&COM_SM::OMX_Executing);
}

/*
* EnteringOpMode
 */
SCF_STATE COM_SM::EnteringOpMode(s_scf_event const *e)
{
   switch (e->sig)
   {
	   case SCF_STATE_ENTRY_SIG:
	   {
		   /*MSG0("EnteringOpMode-ENTRY\n");*/
		   /*OstTraceFiltStatic0(TRACE_DEBUG, "EnteringOpMode-ENTRY", (&mENSComponent));*/
		   return 0;
	   }
	   case SCF_STATE_EXIT_SIG:
	   {
		   /* MSG0("EnteringOpMode-EXIT\n"); */
		   /* OstTraceFiltStatic0(TRACE_DEBUG, "EnteringOpMode-EXIT", (&mENSComponent)); */
		   return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&COM_SM::OpModeTransition);
}

/*
* ExitingOpMode
 */
SCF_STATE COM_SM::ExitingOpMode(s_scf_event const *e)
{
   switch (e->sig)
   {
	   case SCF_STATE_ENTRY_SIG:
	   {
		   /* MSG0("ExitingOpMode-ENTRY\n"); */
		   /* OstTraceFiltStatic0(TRACE_DEBUG, "ExitingOpMode-ENTRY", (&mENSComponent)); */
		   return 0;
	   }
	   case SCF_STATE_EXIT_SIG:
	   {
		   /* MSG0("ExitingOpMode-EXIT\n");*/
		   /* OstTraceFiltStatic0(TRACE_DEBUG, "ExitingOpMode-EXIT", (&mENSComponent));*/
		   return 0;
	   }
       default: break;
   }
   return SCF_STATE_PTR(&COM_SM::OpModeTransition);
}






