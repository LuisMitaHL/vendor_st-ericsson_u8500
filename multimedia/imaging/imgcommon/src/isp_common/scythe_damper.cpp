/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "SCYTHE_DAMPER"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "scythe_damper.h"
#include "VhcElementDefs.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_scythe_damperTraces.h"
#endif

#define HASH_ENTRY(i) i,#i

typedef struct
{
   t_scythe_damper_id id;
   const char*        name;
   t_uint32           addr;
   float              min;
   float              max;
} t_scythe_damper_info;

static const t_scythe_damper_info KScytheDamperInfo[SCYTHE_DAMPERS_NUM] =
{
   { HASH_ENTRY(SCYTHE_CONTROL_LO), DusterControl_u8_ScytheControl_lo_Byte0, 0.0, 64.0 },
   { HASH_ENTRY(SCYTHE_CONTROL_HI), DusterControl_u8_ScytheControl_hi_Byte0, 0.0, 64.0 }
};

/*
 * Constructor
 */
CScytheDamper::CScytheDamper(TraceObject *traceobj):CSharedDamper("SCYTHE", traceobj)
{
}

/*
 * Actual Constructor
 */
t_damper_error_code CScytheDamper::Construct()
{
   return DoConstruct(SCYTHE_DAMPERS_NUM);
}

/*
 * This function sets the Scythe dampers
 */
t_damper_error_code CScytheDamper::SetDamper( const t_scythe_damper_id aDamperId,
                                              const float*             pValues,
                                              const int                aNumRows,
                                              const int                aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CScytheDamper::SetDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   if(aDamperId<0 || aDamperId>=SCYTHE_DAMPERS_NUM)
   {
      DBGT_ERROR("Invalid damper Id: %d\n", aDamperId);
      OstTraceFiltStatic1(TRACE_ERROR, "Invalid damper Id: %d", (mTraceObject), aDamperId);
      OUTR(" ",DAMPER_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CScytheDamper::SetDamper (%d)", (mTraceObject), DAMPER_INVALID_ARGUMENT);
      return DAMPER_INVALID_ARGUMENT;
   }
   MSG3("[%s], NumRows=%d, NumCols=%d\n", KScytheDamperInfo[aDamperId].name, aNumRows, aNumCols);
   OstTraceFiltStatic3(TRACE_DEBUG, "KScytheDamperInfo[%d].name, NumRows=%d, NumCols=%d", (mTraceObject), aDamperId, aNumRows, aNumCols);
   err = DoSetDamper( aDamperId, pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("[%s] failed to set damper: err=%d\n", KScytheDamperInfo[aDamperId].name, err);
      OstTraceFiltStatic2(TRACE_ERROR, "KScytheDamperInfo[%d].name failed to set damper: err=%d", (mTraceObject), aDamperId, err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CScytheDamper::SetDamper (%d)", (mTraceObject), err);
      return err;
   }
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CScytheDamper::SetDamper (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}

/*
 * This function computes the dampers and queues the Page Elements
 */
t_damper_error_code CScytheDamper::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                       const t_damper_base_values* pBaseValues)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CScytheDamper::ComputeAndQueuePEs", (mTraceObject));
   float pDampedValues[SCYTHE_DAMPERS_NUM] = { 0.0, 0.0 };
   t_damper_error_code err = DAMPER_OK;
   float fValue = 0.0;

   // Evaluate Scythe dampers
   err = DoEvaluate( pBaseValues, pDampedValues);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("Evaluation failed: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Evaluation failed: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CScytheDamper::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Queue the Page Elements
   for(int i=0; i<SCYTHE_DAMPERS_NUM; i++)
   {
      fValue = pDampedValues[i];
      if(fValue < KScytheDamperInfo[i].min)
         fValue = KScytheDamperInfo[i].min;
      else if (fValue > KScytheDamperInfo[i].max)
         fValue = KScytheDamperInfo[i].max;
      MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(KScytheDamperInfo[i].addr), fValue);
      //OstTraceFiltStatic2(TRACE_DEBUG, "%s = %f", (mTraceObject), CXp70::GetPeName(KScytheDamperInfo[i].addr), fValue);
      pIspctlCom->queuePE( KScytheDamperInfo[i].addr, (t_uint32)fValue);
   }

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CScytheDamper::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
