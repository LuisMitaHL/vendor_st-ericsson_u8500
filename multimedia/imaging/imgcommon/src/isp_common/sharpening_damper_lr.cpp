/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "SHARPENING_DAMPER_LR"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "sharpening_damper_lr.h"
#include "VhcElementDefs.h"

#include <stdio.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_sharpening_damper_lrTraces.h"
#endif

#define I2S(a) #a
#define SHARPENING_DAMPER_LR_NAME(i) (i==SHARPENING_ADSOC_GAIN_LR?I2S(SHARPENING_ADSOC_GAIN_LR):(i==SHARPENING_ADSOC_CORING_LR?I2S(SHARPENING_ADSOC_CORING_LR):"unknown"))

/*
 * Constructor
 */
CSharpeningDamperLR::CSharpeningDamperLR(TraceObject *traceobj):CSharedDamper("SHARPENING-LR", traceobj)
{
    iScaleFactorGain   = 1.0;
    iScaleFactorOffset = 0.0;
    iOmxSharpness      = 0;
}

/*
 * Actual Constructor
 */
t_damper_error_code CSharpeningDamperLR::Construct()
{
   return DoConstruct(SHARPENING_DAMPERS_LR_NUM);
}

/*
 * This function sets the OMX setting value
 */
void CSharpeningDamperLR::SetOmxMapping(float aGain, float aOffset)
{
   iScaleFactorGain   = aGain;
   iScaleFactorOffset = aOffset;
   MSG2("OMX Mapping: gain=%f, offset=%f\n", iScaleFactorGain, iScaleFactorOffset);
   OstTraceFiltStatic2(TRACE_DEBUG, "OMX Mapping: gain=%f, offset=%f", (mTraceObject), iScaleFactorGain, iScaleFactorOffset);
}

/*
 * This function sets the OMX setting value
 */
void CSharpeningDamperLR::SetOmxSetting(t_sint32 aSharpness)
{
   iOmxSharpness = aSharpness;
   MSG1("OMX Sharpness=%ld\n", iOmxSharpness);
   OstTraceFiltStatic1(TRACE_DEBUG, "OMX Sharpness=%ld", (mTraceObject), iOmxSharpness);
}

/*
 * This function sets the sharpening damper
 */
t_damper_error_code CSharpeningDamperLR::SetDamper( const t_sharpening_damper_lr_id aDamperId,
                                                    const float*                    pValues,
                                                    const int                       aNumRows,
                                                    const int                       aNumCols)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CSharpeningDamperLR::SetDamper", (mTraceObject));
   t_damper_error_code err = DAMPER_OK;
   if(aDamperId<0 || aDamperId>=SHARPENING_DAMPERS_LR_NUM)
   {
      DBGT_ERROR("Invalid damper Id: %d\n", aDamperId);
      OstTraceFiltStatic1(TRACE_ERROR, "Invalid damper Id: %d", (mTraceObject), aDamperId);
      OUTR(" ",DAMPER_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CSharpeningDamperLR::SetDamper (%d)", (mTraceObject), DAMPER_INVALID_ARGUMENT);
      return DAMPER_INVALID_ARGUMENT;
   }
   MSG3("[%s] NumRows=%d, NumCols=%d\n", SHARPENING_DAMPER_LR_NAME(aDamperId), aNumRows, aNumCols);
   OstTraceFiltStatic3(TRACE_DEBUG, "SHARPENING_DAMPER_LR_NAME(%d) NumRows=%d, NumCols=%d", (mTraceObject), aDamperId, aNumRows, aNumCols);
   err = DoSetDamper( aDamperId, pValues, aNumRows, aNumCols);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("[%s] failed to set damper: err=%d\n", SHARPENING_DAMPER_LR_NAME(aDamperId), err);
      OstTraceFiltStatic2(TRACE_ERROR, "SHARPENING_DAMPER_LR_NAME(%d) failed to set damper: err=%d", (mTraceObject), aDamperId, err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CSharpeningDamperLR::SetDamper (%d)", (mTraceObject), err);
      return err;
   }
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CSharpeningDamperLR::SetDamper (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}

/*
 * This function computes the dampers and queues the Page Elements
 */
t_damper_error_code CSharpeningDamperLR::ComputeAndQueuePEs( CIspctlCom*                 pIspctlCom,
                                                             const t_damper_base_values* pBaseValues)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CSharpeningDamperLR::ComputeAndQueuePEs", (mTraceObject));
   float pDamperValues[SHARPENING_DAMPERS_LR_NUM] = { 0.0, 0.0};
   t_damper_error_code err = DAMPER_OK;
   float fAdsocGain = 0.0;
   float fAdsocCoring = 0.0;
   float fScaleFactor = 0.0;

   // Evaluate Sharpening dampers
   err = DoEvaluate( pBaseValues, pDamperValues);
   if(err!=DAMPER_OK)
   {
      DBGT_ERROR("Evaluation failed: err=%d\n", err);
      OstTraceFiltStatic1(TRACE_ERROR, "Evaluation failed: err=%d", (mTraceObject), err);
      OUTR(" ",err);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CSharpeningDamperLR::ComputeAndQueuePEs (%d)", (mTraceObject), err);
      return err;
   }

   // Compute and queue Adsoc Gain PE (scale against omx config and clip value)
   fAdsocGain = pDamperValues[SHARPENING_ADSOC_GAIN_LR];
   fScaleFactor = (float)iOmxSharpness * iScaleFactorGain + iScaleFactorOffset;
   MSG1("Scale Factor = %f\n", fScaleFactor);
   OstTraceFiltStatic1(TRACE_DEBUG, "Scale Factor = %f", (mTraceObject), fScaleFactor);
   fAdsocGain *= fScaleFactor;
   if( fAdsocGain < 0.0) {
      fAdsocGain = 0.0;
   }
   else if ( fAdsocGain > 255.0 ) {
      fAdsocGain = 255.0;
   }
   MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain_Byte0), fAdsocGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain = %f", (mTraceObject), fAdsocGain);
   pIspctlCom->queuePE( Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain_Byte0, (t_uint32)fAdsocGain);

   // Compute and queue Adsoc Coring PE
   fAdsocCoring = pDamperValues[SHARPENING_ADSOC_CORING_LR];
   if( fAdsocCoring < 0.0) {
      fAdsocCoring = 0.0;
   }
   else if ( fAdsocCoring > 31.0 ) {
      fAdsocCoring = 31.0;
   }
   MSG2("%s = %f\n", CIspctlCom::pIspctlSensor->GetPeName(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Coring_Level_Byte0), fAdsocCoring);
   OstTraceFiltStatic1(TRACE_DEBUG, "Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Coring_Level = %f", (mTraceObject), fAdsocCoring);
   pIspctlCom->queuePE( Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Coring_Level_Byte0, (t_uint32)fAdsocCoring);

   // Done
   OUTR(" ",DAMPER_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CSharpeningDamperLR::ComputeAndQueuePEs (%d)", (mTraceObject), DAMPER_OK);
   return DAMPER_OK;
}
