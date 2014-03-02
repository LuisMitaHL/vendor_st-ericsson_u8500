/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "DEFCOR_DAMPER"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "defcor_damper.h"
#include "VhcElementDefs.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_defcor_damperTraces.h"
#endif

#define HASH_ENTRY(i) i,#i

typedef struct
{
   t_defcor_damper_id id;
   const char*        name;
   t_uint32           addr;
} t_defcor_damper_info;

static const t_defcor_damper_info KDefCorDamperInfo[DEFCOR_DAMPERS_NUM] =
{
   { HASH_ENTRY(DEFCOR_DUSTER_RC_THRESHOLD),    DusterControl_u8_RingCorrectionNormThr_Byte0       },
   { HASH_ENTRY(DEFCOR_DUSTER_CC_SIGMA_FACTOR), DusterControl_u8_CenterCorrectionSigmaFactor_Byte0 }
};

/*
 * Constructor
 */
CDefCorDamper::CDefCorDamper(TraceObject *traceobj):CSharedDamper("DEFCOR", traceobj)
{
}

/*
 * Actual Constructor
 */
t_damper_error_code CDefCorDamper::Construct()
{
   return DoConstruct(DEFCOR_DAMPERS_NUM);
}

/*
 * This function sets the DefCor dampers
 */
t_damper_error_code CDefCorDamper::SetDamper( const t_defcor_damper_id aDamperId,
                                              const float*             pValues,
                                              const int                aNumRows,
                                              const int                aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CDefCorDamper::SetDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   if(aDamperId<0 || aDamperId>=DEFCOR_DAMPERS_NUM)
   {
      DBGT_ERROR("Invalid damper Id: %d\n", aDamperId);
      OstTraceFiltStatic1(TRACE_ERROR, "Invalid damper Id: %d", (mTraceObject), aDamperId);
      OUTR(" ",DAMPER_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CDefCorDamper::SetDamper (%d)", (mTraceObject), DAMPER_INVALID_ARGUMENT);
      return DAMPER_INVALID_ARGUMENT;
   }
   MSG3("[%s] NumRows=%d, NumCols=%d\n", KDefCorDamperInfo[aDamperId].name, aNumRows, aNumCols);
   OstTraceFiltStatic3(TRACE_DEBUG, "[KDefCorDamperInfo[%d].name] NumRows=%d, NumCols=%d", (mTraceObject), aDamperId, aNumRows, aNumCols);
   err = DoSetDamper( aDamperId, pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("[%s] failed to set damper: err=%d\n", KDefCorDamperInfo[aDamperId].name, err);
      OstTraceFiltStatic2(TRACE_ERROR, "[KDefCorDamperInfo[%d].name] failed to set damper: err=%d", (mTraceObject), aDamperId, err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CDefCorDamper::SetDamper (%d)", (mTraceObject), err);
      return err;
   }
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CDefCorDamper::SetDamper (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}

/*
 * This function computes the dampers and queues the Page Elements
 */
t_damper_error_code CDefCorDamper::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                       const t_damper_base_values* pBaseValues)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CDefCorDamper::ComputeAndQueuePEs", (mTraceObject));
   float pDamperValues[DEFCOR_DAMPERS_NUM] = {0.0, 0.0};
   t_damper_error_code err = DAMPER_OK;

   // Evaluate DefCor dampers
   err = DoEvaluate( pBaseValues, pDamperValues);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("Evaluation failed: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Evaluation failed: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CDefCorDamper::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Queue the Page Elements
   for(int i=0; i<DEFCOR_DAMPERS_NUM; i++)
   {
      MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(KDefCorDamperInfo[i].addr), pDamperValues[i]);
      //OstTraceFiltStatic2(TRACE_DEBUG, "%s = %f", (mTraceObject), CXp70::GetPeName(KDefCorDamperInfo[i].addr), pDamperValues[i]);
      pIspctlCom->queuePE( KDefCorDamperInfo[i].addr, (t_uint32)pDamperValues[i]);
   }

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CDefCorDamper::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
