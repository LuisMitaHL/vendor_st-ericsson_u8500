/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "ISP_DAMPERS"
#define OMX_TRACE_UID 0x10
#define DBGT_LAYER 1
/*
 * Includes
 */
#include "osi_trace.h"
#include "VhcElementDefs.h"
#include "isp_dampers.h"
#include "tuning_params.h"
#include "tuning.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "imgcommon_src_isp_common_isp_dampersTraces.h"
#endif

#define HASH_ENTRY(id) {id,#id}

typedef struct
{
   t_isp_dampers_error_code id;
   const char* name;
} t_isp_dampers_error_entry;

static const t_isp_dampers_error_entry KIspDampersErrorCodesList[] =
{
   HASH_ENTRY(ISP_DAMPERS_OK),
   HASH_ENTRY(ISP_DAMPERS_CONSTRUCT_FAILED),
   HASH_ENTRY(ISP_DAMPERS_DESTROY_FAILED),
   HASH_ENTRY(ISP_DAMPERS_INVALID_ARGUMENT),
   HASH_ENTRY(ISP_DAMPERS_TUNING_ERROR),
   HASH_ENTRY(ISP_DAMPERS_CONFIGURE_FAILED),
   HASH_ENTRY(ISP_DAMPERS_PE_COMPUTATION_FAILED),
   HASH_ENTRY(ISP_DAMPERS_INTERNAL_ERROR)
};

#define KIspDampersErrorCodesListSize (sizeof(KIspDampersErrorCodesList)/sizeof(KIspDampersErrorCodesList[0]))

/*
 * Constructor
 */
CIspDampers::CIspDampers(TraceObject *traceobj):
		iRsoDamper(traceobj),
        iDefCorDamper(traceobj),
        iNoiseFilterDamper(traceobj),
        iSharpeningDamperLR(traceobj),
        iSharpeningDamperHR(traceobj),
        iNoiseModelDamper(traceobj),
        iScorpioDamper(traceobj),
        iScytheDamper(traceobj),
		mTraceObject(traceobj)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::CIspDampers", (mTraceObject));
   bRsoEnabled = true;
   bSharpeningLrEnabled = true;
   bSharpeningHrEnabled = true;
   bScorpioEnabled = true;
   bDefCorEnabled = true;
   bNoiseFilterEnabled = true;
   bNoiseModelEnabled = true;
   bScytheEnabled = true;
   OUT0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Exit CIspDampers::CIspDampers", (mTraceObject));
}

/*
 * Destructor
 */
CIspDampers::~CIspDampers()
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::~CIspDampers", (mTraceObject));
   OUT0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Exit CIspDampers::~CIspDampers", (mTraceObject));
}

/*
 * Actual contructor
 */
t_isp_dampers_error_code CIspDampers::Construct()
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::Construct", (mTraceObject));
   t_damper_error_code damperErr = DAMPER_OK;

   // Contruct every dampers
   damperErr = iRsoDamper.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct RSO Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct RSO Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iDefCorDamper.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Defect Correction Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Defect Correction Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iNoiseFilterDamper.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Noise Filter Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Noise Filter Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iSharpeningDamperLR.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Sharpening Damper LR: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Sharpening Damper LR: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iSharpeningDamperHR.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Sharpening Damper HR: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Sharpening Damper HR: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iNoiseModelDamper.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Noise Model Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Noise Model Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iScorpioDamper.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Scorpio Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Scorpio Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   damperErr = iScytheDamper.Construct();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to construct Scythe Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to construct Scythe Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_CONSTRUCT_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_CONSTRUCT_FAILED);
      return ISP_DAMPERS_CONSTRUCT_FAILED;
   }

   // Done
   OUTR(" ",ISP_DAMPERS_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Construct (%d)", (mTraceObject), ISP_DAMPERS_OK);
   return ISP_DAMPERS_OK;
}

/*
 * Destructor
 */
t_isp_dampers_error_code CIspDampers::Destroy()
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::Destroy", (mTraceObject));
   t_damper_error_code damperErr = DAMPER_OK;

   // Destroy every dampers
   damperErr = iRsoDamper.Destroy();
   if( damperErr != DAMPER_OK) {
      DBGT_ERROR("Failed to destroy RSO Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy RSO Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iDefCorDamper.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Defect Correction Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Defect Correction Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iNoiseFilterDamper.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Noise Filter Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Noise Filter Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iSharpeningDamperLR.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Sharpening Damper LR: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Sharpening Damper LR: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iSharpeningDamperHR.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Sharpening Damper HR: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Sharpening Damper HR: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iNoiseModelDamper.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Noise Model Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Noise Model Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iScorpioDamper.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Scorpio Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Scorpio Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   damperErr = iScytheDamper.Destroy();
   if( damperErr != DAMPER_OK)
   {
      DBGT_ERROR("Failed to destroy Scythe Damper: Err=%d\n", damperErr);
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to destroy Scythe Damper: Err=%d", (mTraceObject), damperErr);
      OUTR(" ",ISP_DAMPERS_DESTROY_FAILED);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_DESTROY_FAILED);
      return ISP_DAMPERS_DESTROY_FAILED;
   }

   // Done
   OUTR(" ",ISP_DAMPERS_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Destroy (%d)", (mTraceObject), ISP_DAMPERS_OK);
   return ISP_DAMPERS_OK;
}

/*
 * Dampers Enable/Disable controls
 */
void CIspDampers::RsoControl(t_isp_dampers_control aControl)
{
    bRsoEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("RSO : %s\n", bRsoEnabled ? "Enabled" : "Disabled");
    if (bRsoEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "RSO : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "RSO : Disabled", (mTraceObject));
    }
}

void CIspDampers::SharpeningLrControl(t_isp_dampers_control aControl)
{
    bSharpeningLrEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("Sharpening LR : %s\n", bSharpeningLrEnabled ? "Enabled" : "Disabled");
    if (bSharpeningLrEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Sharpening LR : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Sharpening LR : Disabled", (mTraceObject));
    }
}

void CIspDampers::SharpeningHrControl(t_isp_dampers_control aControl)
{
    bSharpeningHrEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
	
    MSG1("Sharpening HR : %s\n", bSharpeningHrEnabled ? "Enabled" : "Disabled");
    if (bSharpeningHrEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Sharpening HR : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Sharpening HR : Disabled", (mTraceObject));
    }
}

void CIspDampers::ScorpioControl(t_isp_dampers_control aControl)
{
    bScorpioEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("Scorpio : %s\n", bScorpioEnabled ? "Enabled" : "Disabled");
    if (bScorpioEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Scorpio : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Scorpio : Disabled", (mTraceObject));
    }
}

void CIspDampers::DefCorControl(t_isp_dampers_control aControl)
{
    bDefCorEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("DefCor : %s\n", bDefCorEnabled ? "Enabled" : "Disabled");
    if (bDefCorEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "DefCor : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "DefCor : Disabled", (mTraceObject));
    }
}

void CIspDampers::NoiseFilterControl(t_isp_dampers_control aControl)
{
    bNoiseFilterEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("Noise Filter : %s\n", bNoiseFilterEnabled ? "Enabled" : "Disabled");
    if (bNoiseFilterEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Noise Filter : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Noise Filter : Disabled", (mTraceObject));
    }
}

void CIspDampers::NoiseModelControl(t_isp_dampers_control aControl)
{
    bNoiseModelEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("Noise Model : %s\n", bNoiseModelEnabled ? "Enabled" : "Disabled");
    if (bNoiseModelEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Noise Model : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Noise Model : Disabled", (mTraceObject));
    }
}

void CIspDampers::ScytheControl(t_isp_dampers_control aControl)
{
    bScytheEnabled = aControl == ISP_DAMPERS_ENABLE ? true : false;
    MSG1("Scythe : %s\n", bScytheEnabled ? "Enabled" : "Disabled");
    if (bScytheEnabled) {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Scythe : Enabled", (mTraceObject));
    }
    else {
    	OstTraceFiltStatic0(TRACE_DEBUG, "Scythe : Disabled", (mTraceObject));
    }
}

/*
 * Function to set the OMX sharpness setting
 */
void CIspDampers::SetSharpnessOmxSetting(t_sint32 aSharpness)
{
    iSharpeningDamperLR.SetOmxSetting(aSharpness);
    iSharpeningDamperHR.SetOmxSetting(aSharpness);
}

/*
 * Function that configures the dampers with Gain/Offsets mapping values
 */
t_isp_dampers_error_code CIspDampers::SetMappings(CTuning* pTuning)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::SetMappings", (mTraceObject));
   t_tuning_error_code err = TUNING_OK;

   // Sanity check
   if( pTuning == NULL)
   {
      DBGT_ERROR("Invalid tuning object pointer: pTuning == NULL\n");
      OstTraceFiltStatic0(TRACE_ERROR, "Invalid tuning object pointer: pTuning == NULL", (mTraceObject));
      OUTR(" ",ISP_DAMPERS_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::SetMappings (%d)", (mTraceObject), ISP_DAMPERS_INVALID_ARGUMENT);
      return ISP_DAMPERS_INVALID_ARGUMENT;
   }

   // NoiseModel adaptation mapping
   float fShadingCorrectionGainValue = 0.0;
   err = pTuning->GetParam( IQSET_NOISE_MODEL_SETUP, IQSET_USER_DRIVER, f_ShadingCorrectionGain, &fShadingCorrectionGainValue);
   if( err != TUNING_OK )
   {
      DBGT_ERROR("Failed to get tuning param f_ShadingCorrectionGain (0x%X): Err=%d (%s)\n", f_ShadingCorrectionGain, err, CTuning::ErrorCode2String(err));
      OstTraceFiltStatic2(TRACE_ERROR, "Failed to get tuning param f_ShadingCorrectionGain (0x%X): Err=%d", (mTraceObject), f_ShadingCorrectionGain, err);
      OUTR(" ",ISP_DAMPERS_TUNING_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::SetMappings (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
      return ISP_DAMPERS_TUNING_ERROR;
   }
   iNoiseModelDamper.SetShadingCorrectionGain(fShadingCorrectionGainValue);

   // Sharpness adaptation mapping
   float fSharpnessGainValue = 0.0;
   float fSharpnessOffsetValue = 0.0;
   err = pTuning->GetParam(IQSET_PICTURE_SETTINGS_MAPPING, IQSET_USER_DRIVER, f_SharpnessGain, &fSharpnessGainValue);
   if(err != TUNING_OK) {
      DBGT_ERROR("Failed to get tuning param f_SharpnessGain (0x%X), err=%d (%s)\n", f_SharpnessGain, err, CTuning::ErrorCode2String(err));
      OstTraceFiltStatic2(TRACE_ERROR, "Failed to get tuning param f_SharpnessGain (0x%X), err=%d", (mTraceObject), f_SharpnessGain, err);
      OUTR(" ", ISP_DAMPERS_TUNING_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::SetMappings (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
      return ISP_DAMPERS_TUNING_ERROR;
   }
   err = pTuning->GetParam(IQSET_PICTURE_SETTINGS_MAPPING, IQSET_USER_DRIVER, f_SharpnessOffset, &fSharpnessOffsetValue);
   if(err != TUNING_OK) {
      DBGT_ERROR("Failed to get tuning param f_SharpnessOffset (0x%X), err=%d (%s)\n", f_SharpnessOffset, err, CTuning::ErrorCode2String(err));
      OstTraceFiltStatic2(TRACE_ERROR, "Failed to get tuning param f_SharpnessOffset (0x%X), err=%d", (mTraceObject), f_SharpnessOffset, err);
      OUTR(" ", ISP_DAMPERS_TUNING_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::SetMappings (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
      return ISP_DAMPERS_TUNING_ERROR;
   }
   iSharpeningDamperLR.SetOmxMapping( fSharpnessGainValue, fSharpnessOffsetValue);
   iSharpeningDamperHR.SetOmxMapping( fSharpnessGainValue, fSharpnessOffsetValue);

   // Done
   OUTR(" ",ISP_DAMPERS_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::SetMappings (%d)", (mTraceObject), ISP_DAMPERS_OK);
   return ISP_DAMPERS_OK;
}

/*
 * Actual configuration function
 */
t_isp_dampers_error_code CIspDampers::Configure( CTuning*          pTuning,
                                                 const e_iqset_id  aIqSetId)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::Configure", (mTraceObject));
   t_tuning_error_code   tunErr = TUNING_OK;
   t_damper_error_code   damperErr = DAMPER_OK;
   t_tuning_param_info   pDamperEntries[64] = {{0,0}};
   const t_sint32        maxDamperEntries = sizeof(pDamperEntries)/sizeof(pDamperEntries[0]);
   t_sint32              numDamperEntries = 0;
   float                 pValues[CDAMPER_CTRLP_MAX_DIM*CDAMPER_CTRLP_MAX_DIM] = {0};
   int                   numRows = CDAMPER_CTRLP_MAX_DIM;
   int                   numCols = CDAMPER_CTRLP_MAX_DIM;
   int                   numPoints = CDAMPER_CTRLP_MAX_DIM;
   int                   iBaseId = 0;
   t_damper_base_id      eBaseId = DAMPER_BASE_NONE;

   // Sanity check
   if( pTuning == NULL)
   {
      DBGT_ERROR("Invalid tuning object pointer: pTuning == NULL\n");
      OstTraceFiltStatic0(TRACE_ERROR, "Invalid tuning object pointer: pTuning == NULL", (mTraceObject));
      OUTR(" ",ISP_DAMPERS_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INVALID_ARGUMENT);
      return ISP_DAMPERS_INVALID_ARGUMENT;
   }
   MSG1("Fetching dampers from [%s] tuning\n", pTuning->GetInstanceName());
   //OstTraceFiltStatic1(TRACE_DEBUG, "Fetching dampers from [%s] tuning", (mTraceObject), pTuning->GetInstanceName());
   // Get the number of damper entries
   numDamperEntries = pTuning->GetParamCount( aIqSetId, IQSET_USER_ISP_DAMPERS);
   if(numDamperEntries==0)
   {
      // No damper entries in this IQ Set => Nothing to do
      MSG0("No dampers entries => nothing to do\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "No dampers entries => nothing to do", (mTraceObject));
      OUTR(" ",ISP_DAMPERS_OK);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_OK);
      return ISP_DAMPERS_OK;
   }
   if(numDamperEntries>maxDamperEntries)
   {
      // Number of entries too big
      DBGT_ERROR("Number of dampers entries too big: got %ld, %ld max can be handled\n",numDamperEntries, maxDamperEntries);
      OstTraceFiltStatic2(TRACE_ERROR, "Number of dampers entries too big: got %ld, %ld max can be handled", (mTraceObject),numDamperEntries, maxDamperEntries);
      OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
      return ISP_DAMPERS_INTERNAL_ERROR;
   }
   MSG1("%ld dampers entries\n", numDamperEntries);
   OstTraceFiltStatic1(TRACE_DEBUG, "%ld dampers entries", (mTraceObject), numDamperEntries);
   tunErr = pTuning->GetParamInfoList( aIqSetId, IQSET_USER_ISP_DAMPERS, pDamperEntries, &numDamperEntries);
   if( tunErr != TUNING_OK )
   {
      DBGT_ERROR("Failed to get param addresses: Err=%d (%s)\n", tunErr, CTuning::ErrorCode2String(tunErr));
      OstTraceFiltStatic1(TRACE_ERROR, "Failed to get param addresses: Err=%d", (mTraceObject), tunErr);
      OUTR(" ",ISP_DAMPERS_TUNING_ERROR);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
      return ISP_DAMPERS_TUNING_ERROR;
   }

   // Get damper parameter and dispatch to appropriate damper instance
   for(t_sint32 index=0; index<numDamperEntries; index++)
   {
      // ***************************************
      // Step 1: Fetch parameters from data base
      // ***************************************
      numRows = CDAMPER_CTRLP_MAX_DIM;
      numCols = CDAMPER_CTRLP_MAX_DIM;
      numPoints = CDAMPER_CTRLP_MAX_DIM;
      switch( pDamperEntries[index].addr )
      {
         // --------------------------------
         //        Dampers bases
         // --------------------------------
         case RSO_DAMPER_BASE_A:
         case RSO_DAMPER_BASE_B:
         case DEFCOR_DAMPER_BASE_A:
         case DEFCOR_DAMPER_BASE_B:
         case NOISE_FILTER_DAMPER_BASE_A:
         case NOISE_FILTER_DAMPER_BASE_B:
         case SHARPENING_DAMPER_BASE_A:
         case SHARPENING_DAMPER_BASE_B:
         case NOISE_MODEL_DAMPER_BASE_A:
         case NOISE_MODEL_DAMPER_BASE_B:
         case SCORPIO_DAMPER_BASE_A:
         case SCORPIO_DAMPER_BASE_B:
         case SCYTHE_DAMPER_BASE_A:
         case SCYTHE_DAMPER_BASE_B:
         {
            MSG1("Fetching Damper Base, Addr=0x%lX\n", pDamperEntries[index].addr);
            OstTraceFiltStatic1(TRACE_DEBUG, "Fetching Damper Base, Addr=0x%lX", (mTraceObject), pDamperEntries[index].addr);
            tunErr = pTuning->GetParam( aIqSetId, IQSET_USER_ISP_DAMPERS, pDamperEntries[index].addr, &iBaseId);
            if( tunErr != TUNING_OK )
            {
               DBGT_ERROR("Failed to get Damper Base, Addr=0x%lX: Err=%d (%s)\n", pDamperEntries[index].addr, tunErr, CTuning::ErrorCode2String(tunErr));
               OstTraceFiltStatic2(TRACE_ERROR, "Failed to get Damper Base, Addr=0x%lX: Err=%d", (mTraceObject), pDamperEntries[index].addr, tunErr);
               OUTR(" ",ISP_DAMPERS_TUNING_ERROR);
               OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
               return ISP_DAMPERS_TUNING_ERROR;
            }
            eBaseId = (t_damper_base_id)iBaseId;
         }
         break;

         // --------------------------------
         //     Dampers Control Points
         // --------------------------------
         case RSO_DAMPER_BASE_A_CONTROL_POINTS:
         case RSO_DAMPER_BASE_B_CONTROL_POINTS:
         case DEFCOR_DAMPER_BASE_A_CONTROL_POINTS:
         case DEFCOR_DAMPER_BASE_B_CONTROL_POINTS:
         case NOISE_FILTER_DAMPER_BASE_A_CONTROL_POINTS:
         case NOISE_FILTER_DAMPER_BASE_B_CONTROL_POINTS:
         case SHARPENING_DAMPER_BASE_A_CONTROL_POINTS:
         case SHARPENING_DAMPER_BASE_B_CONTROL_POINTS:
         case NOISE_MODEL_DAMPER_BASE_A_CONTROL_POINTS:
         case NOISE_MODEL_DAMPER_BASE_B_CONTROL_POINTS:
         case SCORPIO_DAMPER_BASE_A_CONTROL_POINTS:
         case SCORPIO_DAMPER_BASE_B_CONTROL_POINTS:
         case SCYTHE_DAMPER_BASE_A_CONTROL_POINTS:
         case SCYTHE_DAMPER_BASE_B_CONTROL_POINTS:
         {
            MSG1("Fetching Damper Control Points, Addr=0x%lX\n", pDamperEntries[index].addr);
            OstTraceFiltStatic1(TRACE_DEBUG, "Fetching Damper Control Points, Addr=0x%lX", (mTraceObject), pDamperEntries[index].addr);
            tunErr = pTuning->GetVectorAsFloat( aIqSetId, IQSET_USER_ISP_DAMPERS, pDamperEntries[index].addr, pValues, &numPoints);
            if( tunErr != TUNING_OK )
            {
               DBGT_ERROR("Failed to get Damper Control Points 0x%lX: Err=%d (%s)\n", pDamperEntries[index].addr, tunErr, CTuning::ErrorCode2String(tunErr));
               OstTraceFiltStatic2(TRACE_ERROR, "Failed to get Damper Control Points 0x%lX: Err=%d", (mTraceObject), pDamperEntries[index].addr, tunErr);
               OUTR(" ",ISP_DAMPERS_TUNING_ERROR);
               OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
               return ISP_DAMPERS_TUNING_ERROR;
            }
         }
         break;

         // ---------------------------------
         //       Dampers matrices
         // ---------------------------------
         case RSO_DAMPER_DCTERM_OFFSET_RR:
         case RSO_DAMPER_DCTERM_OFFSET_GR:
         case RSO_DAMPER_DCTERM_OFFSET_GB:
         case RSO_DAMPER_DCTERM_OFFSET_BB:
         case RSO_DAMPER_SLANT_X_RR:
         case RSO_DAMPER_SLANT_X_GR:
         case RSO_DAMPER_SLANT_X_GB:
         case RSO_DAMPER_SLANT_X_BB:
         case RSO_DAMPER_SLANT_Y_RR:
         case RSO_DAMPER_SLANT_Y_GR:
         case RSO_DAMPER_SLANT_Y_GB:
         case RSO_DAMPER_SLANT_Y_BB:
         case DEFCOR_DAMPER_DUSTER_RC_THRESHOLD:
         case DEFCOR_DAMPER_DUSTER_CC_SIGMA_FACTOR:
         case NOISE_FILTER_DAMPER_DUSTER_GAUSSIAN_WEIGHT:
         case SHARPENING_DAMPER_ADSOC_GAIN_LR:
         case SHARPENING_DAMPER_ADSOC_CORING_LR:
         case SHARPENING_DAMPER_ADSOC_GAIN_HR:
         case SHARPENING_DAMPER_ADSOC_CORING_HR:
         case NOISE_MODEL_DAMPER_PIXEL_SIGMA:
         case SCORPIO_DAMPER_CORING_LEVEL:
         case SCYTHE_DAMPER_DUSTER_SCYTHE_CONTROL_LO:
         case SCYTHE_DAMPER_DUSTER_SCYTHE_CONTROL_HI:
         {
             MSG1("Fetching Damper Matrix, Addr=0x%lX\n", pDamperEntries[index].addr);
             OstTraceFiltStatic1(TRACE_DEBUG, "Fetching Damper Matrix, Addr=0x%lX", (mTraceObject), pDamperEntries[index].addr);
             tunErr = pTuning->GetMatrixAsFloat( aIqSetId, IQSET_USER_ISP_DAMPERS, pDamperEntries[index].addr, pValues, &numRows, &numCols);
             if( tunErr != TUNING_OK )
             {
                DBGT_ERROR("Failed to get Damper Matrix, Addr=%lX, Err=%d (%s)\n", pDamperEntries[index].addr, tunErr, CTuning::ErrorCode2String(tunErr));
                OstTraceFiltStatic2(TRACE_ERROR, "Failed to get Damper Matrix, Addr=%lX, Err=%d", (mTraceObject), pDamperEntries[index].addr, tunErr);
                OUTR(" ",ISP_DAMPERS_TUNING_ERROR);
                OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
                return ISP_DAMPERS_TUNING_ERROR;
             }
          }
          break;

         // --------------------
         // Default: unsupported
         // --------------------
         default:
             DBGT_ERROR("Damper parameter 0x%lX not supported\n", pDamperEntries[index].addr);
             OstTraceFiltStatic1(TRACE_ERROR, "Damper parameter 0x%lX not supported", (mTraceObject), pDamperEntries[index].addr);
             OUTR(" ",ISP_DAMPERS_TUNING_ERROR);
             OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_TUNING_ERROR);
             return ISP_DAMPERS_TUNING_ERROR;
      }

      // ************************************************
      // Step 2: dispatch parameter to appropriate damper
      // ************************************************
      switch( pDamperEntries[index].addr )
      {
         // -------------------------
         //          RSO
         // -------------------------
         case RSO_DAMPER_BASE_A:
             MSG0("Found RSO Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO Damper Base A", (mTraceObject));
             damperErr = iRsoDamper.SetBaseA(eBaseId);
             break;
         case RSO_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found RSO Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO Damper Base A Control Points", (mTraceObject));
             damperErr = iRsoDamper.SetBaseAControlPoints(pValues,numPoints);
             break;
         case RSO_DAMPER_BASE_B:
             MSG0("Found RSO Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO Damper Base B", (mTraceObject));
             damperErr = iRsoDamper.SetBaseB(eBaseId);
             break;
         case RSO_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found RSO Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO Damper Base B Control Points", (mTraceObject));
             damperErr = iRsoDamper.SetBaseBControlPoints(pValues,numPoints);
             break;
         case RSO_DAMPER_DCTERM_OFFSET_RR:
             MSG0("Found RSO DcTerm RR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO DcTerm RR Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_DCTERM_RR, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_DCTERM_OFFSET_GR:
             MSG0("Found RSO DcTerm GR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO DcTerm GR Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_DCTERM_GR, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_DCTERM_OFFSET_GB:
             MSG0("Found RSO DcTerm GB Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO DcTerm GB Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_DCTERM_GB, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_DCTERM_OFFSET_BB:
             MSG0("Found RSO DcTerm BB Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO DcTerm BB Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_DCTERM_BB, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_X_RR:
             MSG0("Found RSO SlantX RR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantX RR Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTX_RR, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_X_GR:
             MSG0("Found RSO SlantX GR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantX GR Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTX_GR, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_X_GB:
             MSG0("Found RSO SlantX GB Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantX GB Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTX_GB, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_X_BB:
             MSG0("Found RSO SlantX BB Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantX BB Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTX_BB, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_Y_RR:
             MSG0("Found RSO SlantY RR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantY RR Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTY_RR, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_Y_GR:
             MSG0("Found RSO SlantY GR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantY GR Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTY_GR, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_Y_GB:
             MSG0("Found RSO SlantY GB Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantY GB Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTY_GB, pValues, numRows, numCols);
             break;
         case RSO_DAMPER_SLANT_Y_BB:
             MSG0("Found RSO SlantY BB Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found RSO SlantY BB Damper", (mTraceObject));
             damperErr = iRsoDamper.SetDamper(RSO_SLANTY_BB, pValues, numRows, numCols);
             break;

         // -------------------------
         //     Defect Correction
         // -------------------------
         case DEFCOR_DAMPER_BASE_A:
             MSG0("Found DefCor Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found DefCor Damper Base A", (mTraceObject));
             damperErr = iDefCorDamper.SetBaseA(eBaseId);
             break;
         case DEFCOR_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found DefCor Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found DefCor Damper Base A Control Points", (mTraceObject));
             damperErr = iDefCorDamper.SetBaseAControlPoints(pValues,numPoints);
             break;
         case DEFCOR_DAMPER_BASE_B:
             MSG0("Found DefCor Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found DefCor Damper Base B", (mTraceObject));
             damperErr = iDefCorDamper.SetBaseB(eBaseId);
             break;
         case DEFCOR_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found DefCor Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found DefCor Damper Base B Control Points", (mTraceObject));
             damperErr = iDefCorDamper.SetBaseBControlPoints(pValues,numPoints);
             break;
         case DEFCOR_DAMPER_DUSTER_RC_THRESHOLD:
             MSG0("Found DefCor Duster RC Threshold Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found DefCor Duster RC Threshold Damper", (mTraceObject));
             damperErr = iDefCorDamper.SetDamper(DEFCOR_DUSTER_RC_THRESHOLD, pValues, numRows, numCols);
             break;
         case DEFCOR_DAMPER_DUSTER_CC_SIGMA_FACTOR:
             MSG0("Found DefCor Duster Sigma Factor Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found DefCor Duster Sigma Factor Damper", (mTraceObject));
             damperErr = iDefCorDamper.SetDamper(DEFCOR_DUSTER_CC_SIGMA_FACTOR, pValues, numRows, numCols);
             break;

         // -------------------------
         //      Noise Filter
         // -------------------------
         case NOISE_FILTER_DAMPER_BASE_A:
             MSG0("Found Noise Filter Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Filter Damper Base A", (mTraceObject));
             damperErr = iNoiseFilterDamper.SetBaseA(eBaseId);
             break;
         case NOISE_FILTER_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found Noise Filter Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Filter Damper Base A Control Points", (mTraceObject));
             damperErr = iNoiseFilterDamper.SetBaseAControlPoints(pValues,numPoints);
             break;
         case NOISE_FILTER_DAMPER_BASE_B:
             MSG0("Found Noise Filter Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Filter Damper Base B", (mTraceObject));
             damperErr = iNoiseFilterDamper.SetBaseB(eBaseId);
             break;
         case NOISE_FILTER_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found Noise Filter Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Filter Damper Base B Control Points", (mTraceObject));
             damperErr = iNoiseFilterDamper.SetBaseBControlPoints(pValues,numPoints);
             break;
         case NOISE_FILTER_DAMPER_DUSTER_GAUSSIAN_WEIGHT:
             MSG0("Found Noise Filter Duster Gaussian Weight Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Filter Duster Gaussian Weight Damper", (mTraceObject));
             damperErr = iNoiseFilterDamper.SetDusterGaussianWeightDamper(pValues, numRows, numCols);
             break;

         // -------------------------
         //      Sharpening
         // -------------------------
         case SHARPENING_DAMPER_BASE_A:
             MSG0("Found Sharpening Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Damper Base A", (mTraceObject));
             damperErr = iSharpeningDamperLR.SetBaseA(eBaseId);
             damperErr = iSharpeningDamperHR.SetBaseA(eBaseId);
             break;
         case SHARPENING_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found Sharpening Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Damper Base A Control Points", (mTraceObject));
             damperErr = iSharpeningDamperLR.SetBaseAControlPoints(pValues,numPoints);
             damperErr = iSharpeningDamperHR.SetBaseAControlPoints(pValues,numPoints);
             break;
         case SHARPENING_DAMPER_BASE_B:
             MSG0("Found Sharpening Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Damper Base B", (mTraceObject));
             damperErr = iSharpeningDamperLR.SetBaseB(eBaseId);
             damperErr = iSharpeningDamperHR.SetBaseB(eBaseId);
             break;
         case SHARPENING_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found Sharpening Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Damper Base B Control Points", (mTraceObject));
             damperErr = iSharpeningDamperLR.SetBaseBControlPoints(pValues,numPoints);
             damperErr = iSharpeningDamperHR.SetBaseBControlPoints(pValues,numPoints);
             break;
         case SHARPENING_DAMPER_ADSOC_GAIN_LR:
             MSG0("Found Sharpening Adsoc Gain LR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Adsoc Gain LR Damper", (mTraceObject));
             damperErr = iSharpeningDamperLR.SetDamper(SHARPENING_ADSOC_GAIN_LR, pValues, numRows, numCols);
             break;
         case SHARPENING_DAMPER_ADSOC_CORING_LR:
             MSG0("Found Sharpening Adsoc Coring LR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Adsoc Coring LR Damper", (mTraceObject));
             damperErr = iSharpeningDamperLR.SetDamper(SHARPENING_ADSOC_CORING_LR, pValues, numRows, numCols);
             break;

         case SHARPENING_DAMPER_ADSOC_GAIN_HR:
             MSG0("Found Sharpening Adsoc Gain HR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Adsoc Gain HR Damper", (mTraceObject));
             damperErr = iSharpeningDamperHR.SetDamper(SHARPENING_ADSOC_GAIN_HR, pValues, numRows, numCols);
             break;
         case SHARPENING_DAMPER_ADSOC_CORING_HR:
             MSG0("Found Sharpening Adsoc Coring HR Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Sharpening Adsoc Coring HR Damper", (mTraceObject));
             damperErr = iSharpeningDamperHR.SetDamper(SHARPENING_ADSOC_CORING_HR, pValues, numRows, numCols);
             break;

         // -------------------------
         //     Noise Model
         // -------------------------
         case NOISE_MODEL_DAMPER_BASE_A:
             MSG0("Found Noise Model Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Model Damper Base A", (mTraceObject));
             damperErr = iNoiseModelDamper.SetBaseA(eBaseId);
             break;
         case NOISE_MODEL_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found Noise Model Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Model Damper Base A Control Points", (mTraceObject));
             damperErr = iNoiseModelDamper.SetBaseAControlPoints(pValues,numPoints);
             break;
         case NOISE_MODEL_DAMPER_BASE_B:
             MSG0("Found Noise Model Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Model Damper Base B", (mTraceObject));
             damperErr = iNoiseModelDamper.SetBaseB(eBaseId);
             break;
         case NOISE_MODEL_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found Noise Model Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Model Damper Base B Control Points", (mTraceObject));
             damperErr = iNoiseModelDamper.SetBaseBControlPoints(pValues,numPoints);
             break;
         case NOISE_MODEL_DAMPER_PIXEL_SIGMA:
             MSG0("Found Noise Model Pixel Sigma Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Noise Model Pixel Sigma Damper", (mTraceObject));
             damperErr = iNoiseModelDamper.SetFrameSigmaDamper(pValues, numRows, numCols);
             break;

         // -------------------------
         //         Scorpio
         // -------------------------
         case SCORPIO_DAMPER_BASE_A:
             MSG0("Found Scorpio Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scorpio Damper Base A", (mTraceObject));
             damperErr = iScorpioDamper.SetBaseA(eBaseId);
             break;
         case SCORPIO_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found Scorpio Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scorpio Damper Base A Control Points", (mTraceObject));
             damperErr = iScorpioDamper.SetBaseAControlPoints(pValues,numPoints);
             break;
         case SCORPIO_DAMPER_BASE_B:
             MSG0("Found Scorpio Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scorpio Damper Base B", (mTraceObject));
             damperErr = iScorpioDamper.SetBaseB(eBaseId);
             break;
         case SCORPIO_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found Scorpio Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scorpio Damper Base B Control Points", (mTraceObject));
             damperErr = iScorpioDamper.SetBaseBControlPoints(pValues,numPoints);
             break;
         case SCORPIO_DAMPER_CORING_LEVEL:
             MSG0("Found Scorpio Coring Level Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scorpio Coring Level Damper", (mTraceObject));
             damperErr = iScorpioDamper.SetCoringLevelDamper(pValues, numRows, numCols);
             break;

         // -------------------------
         //         Scythe
         // -------------------------
         case SCYTHE_DAMPER_BASE_A:
             MSG0("Found Scythe Damper Base A\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scythe Damper Base A", (mTraceObject));
             damperErr = iScytheDamper.SetBaseA(eBaseId);
             break;
         case SCYTHE_DAMPER_BASE_A_CONTROL_POINTS:
             MSG0("Found Scythe Damper Base A Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scythe Damper Base A Control Points", (mTraceObject));
             damperErr = iScytheDamper.SetBaseAControlPoints(pValues,numPoints);
             break;
         case SCYTHE_DAMPER_BASE_B:
             MSG0("Found Scythe Damper Base B\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scythe Damper Base B", (mTraceObject));
             damperErr = iScytheDamper.SetBaseB(eBaseId);
             break;
         case SCYTHE_DAMPER_BASE_B_CONTROL_POINTS:
             MSG0("Found Scythe Damper Base B Control Points\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scythe Damper Base B Control Points", (mTraceObject));
             damperErr = iScytheDamper.SetBaseBControlPoints(pValues,numPoints);
             break;
         case SCYTHE_DAMPER_DUSTER_SCYTHE_CONTROL_LO:
             MSG0("Found Scythe Control Lo Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scythe Control Lo Damper", (mTraceObject));
             damperErr = iScytheDamper.SetDamper( SCYTHE_CONTROL_LO, pValues, numRows, numCols);
             break;
         case SCYTHE_DAMPER_DUSTER_SCYTHE_CONTROL_HI:
             MSG0("Found Scythe Control Hi Damper\n");
             OstTraceFiltStatic0(TRACE_DEBUG, "Found Scythe Control Hi Damper", (mTraceObject));
             damperErr = iScytheDamper.SetDamper( SCYTHE_CONTROL_HI, pValues, numRows, numCols);
             break;

         // --------------------
         // Default: unsupported
         // --------------------
         default:
            DBGT_ERROR("Internal error, damper 0x%lX not handled in switch\n", pDamperEntries[index].addr);
            OstTraceFiltStatic1(TRACE_ERROR, "Internal error, damper 0x%lX not handled in switch", (mTraceObject), pDamperEntries[index].addr);
            OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
            OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
            return ISP_DAMPERS_INTERNAL_ERROR;
      }

      if(damperErr != DAMPER_OK)
      {
         DBGT_ERROR("Failed to initialise damper 0x%lX, Err=%d\n", pDamperEntries[index].addr, damperErr);
         OstTraceFiltStatic2(TRACE_ERROR, "Failed to initialise damper 0x%lX, Err=%d", (mTraceObject), pDamperEntries[index].addr, damperErr);
         OUTR(" ",ISP_DAMPERS_CONFIGURE_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_CONFIGURE_FAILED;
      }
   }

   // *******************************************
   // Step 3: Reconfigure the appropriate dampers
   // *******************************************
   if(iRsoDamper.ConfigChanged()==true)
   {
      MSG0("Configuring RSO Damper\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring RSO Damper", (mTraceObject));
      damperErr = iRsoDamper.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure RSO Damper, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure RSO Damper, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iDefCorDamper.ConfigChanged()==true)
   {
      MSG0("Configuring DefCor Damper\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring DefCor Damper", (mTraceObject));
      damperErr = iDefCorDamper.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure DefCor Damper, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure DefCor Damper, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iNoiseFilterDamper.ConfigChanged()==true)
   {
      MSG0("Configuring Noise Filter Damper\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring Noise Filter Damper", (mTraceObject));
      damperErr = iNoiseFilterDamper.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure Noise Filter Damper, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure Noise Filter Damper, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iSharpeningDamperLR.ConfigChanged()==true)
   {
      MSG0("Configuring Sharpening Damper LR\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring Sharpening Damper LR", (mTraceObject));
      damperErr = iSharpeningDamperLR.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure Sharpening Damper LR, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure Sharpening Damper LR, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iSharpeningDamperHR.ConfigChanged()==true)
   {
      MSG0("Configuring Sharpening Damper HR\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring Sharpening Damper HR", (mTraceObject));
      damperErr = iSharpeningDamperHR.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure Sharpening Damper HR, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure Sharpening Damper HR, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iNoiseModelDamper.ConfigChanged()==true)
   {
      MSG0("Configuring Noise Model Damper\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring Noise Model Damper", (mTraceObject));
      damperErr = iNoiseModelDamper.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure Noise Model Damper, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure Noise Model Damper, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iScorpioDamper.ConfigChanged()==true)
   {
      MSG0("Configuring Scorpio Damper\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring Scorpio Damper", (mTraceObject));
      damperErr = iScorpioDamper.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure Scorpio Damper, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure Scorpio Damper, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }
   if(iScytheDamper.ConfigChanged()==true)
   {
      MSG0("Configuring Scythe Damper\n");
      OstTraceFiltStatic0(TRACE_DEBUG, "Configuring Scythe Damper", (mTraceObject));
      damperErr = iScytheDamper.Configure();
      if(damperErr != DAMPER_OK)
      {
         MSG1("Failed to configure Scythe Damper, Err=%d\n", damperErr);
         OstTraceFiltStatic1(TRACE_DEBUG, "Failed to configure Scythe Damper, Err=%d", (mTraceObject), damperErr);
         OUTR(" ",ISP_DAMPERS_INTERNAL_ERROR);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_INTERNAL_ERROR);
         return ISP_DAMPERS_INTERNAL_ERROR;
      }
   }

   // Done
   OUTR(" ",ISP_DAMPERS_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::Configure (%d)", (mTraceObject), ISP_DAMPERS_OK);
   return ISP_DAMPERS_OK;
}

/*
 * This functions invokes all dampers to compute Page Elements values
 */
t_isp_dampers_error_code CIspDampers::ComputeAndQueuePEs( CIspctlCom*             pIspctlCom,
                                                          const IFM_DAMPERS_DATA* pDampersData)
{
   IN0("\n");
   OstTraceFiltStatic0(TRACE_FLOW, "Entry CIspDampers::ComputeAndQueuePEs", (mTraceObject));
   t_damper_error_code  err = DAMPER_OK;
   t_damper_base_values BasesValues;
   float                AverageDigitalGain = 0.0;

   // Sanity check
   if(pIspctlCom==NULL || pDampersData==NULL)
   {
      DBGT_ERROR("Invalid argument: pIspctlCom=%p, pDampersData=%p\n", pIspctlCom, pDampersData);
      OstTraceFiltStatic2(TRACE_ERROR, "Invalid argument: pIspctlCom=0x%x, pDampersData=0x%x", (mTraceObject), (t_uint32)pIspctlCom, (t_uint32)pDampersData);
      OUTR(" ",ISP_DAMPERS_INVALID_ARGUMENT);
      OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_INVALID_ARGUMENT);
      return ISP_DAMPERS_INVALID_ARGUMENT;
   }

   // Format bases values
   BasesValues.IntegrationTime          = pDampersData->BasesValues.IntegrationTime;
   BasesValues.AnalogGain               = pDampersData->BasesValues.AnalogGain;
   BasesValues.PostDgMinimumGain        = pDampersData->BasesValues.PostDgMinimumGain;
   BasesValues.PostDgAverageGain        = pDampersData->BasesValues.PostDgAverageGain;
   BasesValues.PostErMinimumGain        = pDampersData->BasesValues.PostErMinimumGain;
   BasesValues.PostErAverageGain        = pDampersData->BasesValues.PostErAverageGain;
   BasesValues.PostCsMinimumGain        = pDampersData->BasesValues.PostCsMinimumGain;
   BasesValues.PostCsAverageGain        = pDampersData->BasesValues.PostCsAverageGain;
   BasesValues.NomalizedPixelLevel      = pDampersData->BasesValues.NomalizedPixelLevel;
   BasesValues.NomalizedPixelLevelRed   = pDampersData->BasesValues.NomalizedPixelLevelRed;
   BasesValues.NomalizedPixelLevelGreen = pDampersData->BasesValues.NomalizedPixelLevelGreen;
   BasesValues.NomalizedPixelLevelBlue  = pDampersData->BasesValues.NomalizedPixelLevelBlue;
   BasesValues.NomalizedSnr             = pDampersData->BasesValues.NomalizedSnr;
   BasesValues.NomalizedSnrRed          = pDampersData->BasesValues.NomalizedSnrRed;
   BasesValues.NomalizedSnrGreen        = pDampersData->BasesValues.NomalizedSnrGreen;
   BasesValues.NomalizedSnrBlue         = pDampersData->BasesValues.NomalizedSnrBlue;
   AverageDigitalGain                   = pDampersData->Other.AverageDigitalGain;

   // For debug, print out dampers bases values
   MSG1("Damper Base: IntegrationTime          = %f\n", BasesValues.IntegrationTime);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: IntegrationTime          = %f", (mTraceObject), BasesValues.IntegrationTime);
   MSG1("Damper Base: AnalogGain               = %f\n", BasesValues.AnalogGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: AnalogGain               = %f", (mTraceObject), BasesValues.AnalogGain);
   MSG1("Damper Base: PostDgMinimumGain        = %f\n", BasesValues.PostDgMinimumGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: PostDgMinimumGain        = %f", (mTraceObject), BasesValues.PostDgMinimumGain);
   MSG1("Damper Base: PostDgAverageGain        = %f\n", BasesValues.PostDgAverageGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: PostDgAverageGain        = %f", (mTraceObject), BasesValues.PostDgAverageGain);
   MSG1("Damper Base: PostErMinimumGain        = %f\n", BasesValues.PostErMinimumGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: PostErMinimumGain        = %f", (mTraceObject), BasesValues.PostErMinimumGain);
   MSG1("Damper Base: PostErAverageGain        = %f\n", BasesValues.PostErAverageGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: PostErAverageGain        = %f", (mTraceObject), BasesValues.PostErAverageGain);
   MSG1("Damper Base: PostCsMinimumGain        = %f\n", BasesValues.PostCsMinimumGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: PostCsMinimumGain        = %f", (mTraceObject), BasesValues.PostCsMinimumGain);
   MSG1("Damper Base: PostCsAverageGain        = %f\n", BasesValues.PostCsAverageGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: PostCsAverageGain        = %f", (mTraceObject), BasesValues.PostCsAverageGain);
   MSG1("Damper Base: NomalizedPixelLevel      = %f\n", BasesValues.NomalizedPixelLevel);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedPixelLevel      = %f", (mTraceObject), BasesValues.NomalizedPixelLevel);
   MSG1("Damper Base: NomalizedPixelLevelRed   = %f\n", BasesValues.NomalizedPixelLevelRed);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedPixelLevelRed   = %f", (mTraceObject), BasesValues.NomalizedPixelLevelRed);
   MSG1("Damper Base: NomalizedPixelLevelGreen = %f\n", BasesValues.NomalizedPixelLevelGreen);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedPixelLevelGreen = %f", (mTraceObject), BasesValues.NomalizedPixelLevelGreen);
   MSG1("Damper Base: NomalizedPixelLevelBlue  = %f\n", BasesValues.NomalizedPixelLevelBlue);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedPixelLevelBlue  = %f", (mTraceObject), BasesValues.NomalizedPixelLevelBlue);
   MSG1("Damper Base: NomalizedSnr             = %f\n", BasesValues.NomalizedSnr);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedSnr             = %f", (mTraceObject), BasesValues.NomalizedSnr);
   MSG1("Damper Base: NomalizedSnrRed          = %f\n", BasesValues.NomalizedSnrRed);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedSnrRed          = %f", (mTraceObject), BasesValues.NomalizedSnrRed);
   MSG1("Damper Base: NomalizedSnrGreen        = %f\n", BasesValues.NomalizedSnrGreen);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedSnrGreen        = %f", (mTraceObject), BasesValues.NomalizedSnrGreen);
   MSG1("Damper Base: NomalizedSnrBlue         = %f\n", BasesValues.NomalizedSnrBlue);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Base: NomalizedSnrBlue         = %f", (mTraceObject), BasesValues.NomalizedSnrBlue);
   MSG1("Damper Other: AverageDigitalGain      = %f\n", AverageDigitalGain);
   OstTraceFiltStatic1(TRACE_DEBUG, "Damper Other: AverageDigitalGain      = %f", (mTraceObject), AverageDigitalGain);

   // Compute RSO dampers
   if(bRsoEnabled)
   {
      err = iRsoDamper.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute RSO PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute RSO PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute Sharpening LR dampers
   if(bSharpeningLrEnabled)
   {
      err = iSharpeningDamperLR.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute Sharpening LR PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute Sharpening LR PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute Sharpening HR dampers
   if(bSharpeningHrEnabled)
   {
      err = iSharpeningDamperHR.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute Sharpening HR PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute Sharpening HR PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute Scorpio dampers
   if(bScorpioEnabled)
   {
      err = iScorpioDamper.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute Scorpio PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute Scorpio PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute DefCor dampers
   if(bDefCorEnabled)
   {
      err = iDefCorDamper.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute DefCor PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute DefCor PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute Noise Filter dampers
   if(bNoiseFilterEnabled)
   {
      err = iNoiseFilterDamper.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute NoiseFilter PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute NoiseFilter PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute Noise Model dampers
   if(bNoiseModelEnabled)
   {
      err = iNoiseModelDamper.ComputeAndQueuePEs( pIspctlCom, &BasesValues, AverageDigitalGain);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute Noise Model PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute Noise Model PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   // Compute Scythe dampers
   if(bScytheEnabled)
   {
      err = iScytheDamper.ComputeAndQueuePEs( pIspctlCom, &BasesValues);
      if(err!=DAMPER_OK)
      {
         DBGT_ERROR("Failed to Compute Scythe PEs\n");
         OstTraceFiltStatic0(TRACE_ERROR, "Failed to Compute Scythe PEs", (mTraceObject));
         OUTR(" ",ISP_DAMPERS_PE_COMPUTATION_FAILED);
         OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_PE_COMPUTATION_FAILED);
         return ISP_DAMPERS_PE_COMPUTATION_FAILED;
      }
   }

   OUTR(" ",ISP_DAMPERS_OK);
   OstTraceFiltStatic1(TRACE_FLOW, "Exit CIspDampers::ComputeAndQueuePEs (%d)", (mTraceObject), ISP_DAMPERS_OK);
   return ISP_DAMPERS_OK;
}

/*
 * This function converts an error code into a human readable string
 */
const char* CIspDampers::ErrorCode2String(t_isp_dampers_error_code aErr)
{
   for(unsigned int i=0; i<KIspDampersErrorCodesListSize; i++) {
      if(KIspDampersErrorCodesList[i].id == aErr)
         return KIspDampersErrorCodesList[i].name;
   }
   return "*** ERROR CODE NOT FOUND ***";
}
