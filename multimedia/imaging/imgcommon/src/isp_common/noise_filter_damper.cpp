/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "NOISE_FILTER_DAMPER"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "noise_filter_damper.h"
#include "VhcElementDefs.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_noise_filter_damperTraces.h"
#endif

/*
 * Constructor
 */
CNoiseFilterDamper::CNoiseFilterDamper(TraceObject *traceobj):CSharedDamper("NOISE-FILTER", traceobj)
{
}

/*
 * Actual Constructor
 */
t_damper_error_code CNoiseFilterDamper::Construct()
{
   return DoConstruct(1);
}

/*
 * This function sets the Gaussian Weight damper
 */
t_damper_error_code CNoiseFilterDamper::SetDusterGaussianWeightDamper( const float* pValues,
                                                                       const int    aNumRows,
                                                                       const int    aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CNoiseFilterDamper::SetDusterGaussianWeightDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   MSG2("[Gaussian Weight] NumRows=%d, NumCols=%d\n", aNumRows, aNumCols);
   OstTraceFiltStatic2(TRACE_DEBUG, "[Gaussian Weight] NumRows=%d, NumCols=%d", (mTraceObject), aNumRows, aNumCols);
   err = DoSetDamper( pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("[Gaussian Weight] failed to set damper: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "[Gaussian Weight] failed to set damper: err=%d", (mTraceObject), err);
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
t_damper_error_code CNoiseFilterDamper::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                            const t_damper_base_values* pBaseValues)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CNoiseFilterDamper::ComputeAndQueuePEs", (mTraceObject));
   float pDampedValues[1] = { 0.0 };
   t_damper_error_code err = DAMPER_OK;

   // Evaluate Noise Filter dampers
   err = DoEvaluate( pBaseValues, pDampedValues);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("Failed to evaluate damper: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to evaluate damper: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CNoiseFilterDamper::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Queue the Page Elements
   float fGaussianWeightDamped = pDampedValues[0];
   MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(DusterControl_u8_GaussianWeight_Byte0), fGaussianWeightDamped);
   OstTraceFiltStatic1(TRACE_DEBUG, "DusterControl_u8_GaussianWeight = %f", (mTraceObject), fGaussianWeightDamped);
   pIspctlCom->queuePE( DusterControl_u8_GaussianWeight_Byte0, (t_uint32)fGaussianWeightDamped);

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CNoiseFilterDamper::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
