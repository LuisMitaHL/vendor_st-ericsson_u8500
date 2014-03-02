/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "NOISE_MODEL_DAMPER"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "noise_model_damper.h"
#include "VhcElementDefs.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_noise_model_damperTraces.h"
#endif

/*
 * Constructor
 */
CNoiseModelDamper::CNoiseModelDamper(TraceObject *traceobj):CSharedDamper("NOISE-MODEL", traceobj)
{
   iShadingCorrectionGain = 1.0;
}

/*
 * Actual Constructor
 */
t_damper_error_code CNoiseModelDamper::Construct()
{
   return DoConstruct();
}

/*
 * Correction Gain setter
 */
void CNoiseModelDamper::SetShadingCorrectionGain(float aShadingCorrectionGain)
{
   iShadingCorrectionGain = aShadingCorrectionGain;
}

/*
 * This function sets the Noise Model damper
 */
t_damper_error_code CNoiseModelDamper::SetFrameSigmaDamper( const float* pValues,
                                                            const int    aNumRows,
                                                            const int    aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CNoiseModelDamper::SetFrameSigmaDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   MSG2("[Frame Sigma] NumRows=%d, NumCols=%d\n", aNumRows, aNumCols);
   OstTraceFiltStatic2(TRACE_DEBUG, "[Frame Sigma] NumRows=%d, NumCols=%d", (mTraceObject), aNumRows, aNumCols);
   err = DoSetDamper( pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      MSG1("[Frame Sigma] failed to set damper: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_DEBUG, "[Frame Sigma] failed to set damper: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CNoiseModelDamper::SetFrameSigmaDamper (%d)", (mTraceObject), err);
      return err;
   }
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CNoiseModelDamper::SetFrameSigmaDamper (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}

/*
 * This function computes the dampers and queues the Page Elements
 */
t_damper_error_code CNoiseModelDamper::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                           const t_damper_base_values* pBaseValues,
                                                           const float                 aAverageDigitalGain)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CNoiseModelDamper::ComputeAndQueuePEs", (mTraceObject));
   float pDampedValues[1] = { 0.0 };
   float fUserFrameSigma = 0.0;
   t_damper_error_code err = DAMPER_OK;

   // Evaluate Noise Model dampers
   err = DoEvaluate( pBaseValues, pDampedValues);
   if(err!=DAMPER_OK)
   {
      MSG1("Evaluation failed: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_DEBUG, "Evaluation failed: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CNoiseModelDamper::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Scale the damper output by the reference Shading Correction Gain, Average Digital Gain and pipeline dynamic range
   // NOTE: the damper output is now the noise sigma at pipeline input, normalized to pipeline dynamic range
   MSG2("iShadingCorrectionGain=%f, aAverageDigitalGain=%f\n", iShadingCorrectionGain, aAverageDigitalGain);
   OstTraceFiltStatic2(TRACE_DEBUG, "iShadingCorrectionGain=%f, aAverageDigitalGain=%f", (mTraceObject), iShadingCorrectionGain, aAverageDigitalGain);
   fUserFrameSigma = pDampedValues[0] * iShadingCorrectionGain * aAverageDigitalGain  * 4095.0;

   // Clip against hardware limit
   if(fUserFrameSigma >= 1024) {
      fUserFrameSigma = 1024;
   }

   // Queue the Page Elements
   MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(DusterControl_u16_FrameSigma_Byte0), fUserFrameSigma);
   OstTraceFiltStatic1(TRACE_DEBUG, "DusterControl_u16_FrameSigma = %f", (mTraceObject), fUserFrameSigma);
   pIspctlCom->queuePE( DusterControl_u16_FrameSigma_Byte0, (t_uint32)fUserFrameSigma);

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CNoiseModelDamper::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
