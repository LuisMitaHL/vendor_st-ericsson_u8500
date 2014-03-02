/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "SCORPIO_DAMPER"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "scorpio_damper.h"
#include "VhcElementDefs.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_scorpio_damperTraces.h"
#endif

/*
 * Constructor
 */
CScorpioDamper::CScorpioDamper(TraceObject *traceobj):CSharedDamper("SCORPIO", traceobj)
{
}

/*
 * Actual Constructor
 */
t_damper_error_code CScorpioDamper::Construct()
{
   return DoConstruct();
}

/*
 * This function sets the Coring Level damper
 */
t_damper_error_code CScorpioDamper::SetCoringLevelDamper( const float* pValues,
                                                          const int    aNumRows,
                                                          const int    aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CScorpioDamper::SetCoringLevelDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   MSG2("[Coring Level] NumRows=%d, NumCols=%d\n", aNumRows, aNumCols);
   OstTraceFiltStatic2(TRACE_DEBUG, "[Coring Level] NumRows=%d, NumCols=%d", (mTraceObject), aNumRows, aNumCols);
   err = DoSetDamper( pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("[Coring Level] failed to set damper: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "[Coring Level] failed to set damper: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CScorpioDamper::SetCoringLevelDamper (%d)", (mTraceObject), err);
      return err;
   }
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CScorpioDamper::SetCoringLevelDamper (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}

/*
 * This function computes the dampers and queues the Page Elements
 */
t_damper_error_code CScorpioDamper::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                        const t_damper_base_values* pBaseValues)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CScorpioDamper::ComputeAndQueuePEs", (mTraceObject));
   float pDampedValues[1] = { 0.0 };
   float fCoringLevel = 0.0;
   t_damper_error_code err = DAMPER_OK;

   // Evaluate Scorpio dampers
   err = DoEvaluate( pBaseValues, pDampedValues);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("Evaluation failed: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Evaluation failed: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CScorpioDamper::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Queue the Page Elements list
   fCoringLevel = pDampedValues[0];
   MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(Scorpio_Ctrl_u8_CoringLevel_Ctrl_Byte0), fCoringLevel);
   OstTraceFiltStatic1(TRACE_DEBUG, "Scorpio_Ctrl_u8_CoringLevel_Ctrl = %f", (mTraceObject), fCoringLevel);
   pIspctlCom->queuePE( Scorpio_Ctrl_u8_CoringLevel_Ctrl_Byte0, (t_uint32)fCoringLevel);

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CScorpioDamper::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
