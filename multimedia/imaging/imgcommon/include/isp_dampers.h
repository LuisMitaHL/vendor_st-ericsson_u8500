/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISP_DAMPERS_
#define _ISP_DAMPERS_

#include <inc/typedef.h>
#include "rso_damper.h"
#include "defcor_damper.h"
#include "noise_filter_damper.h"
#include "sharpening_damper_lr.h"
#include "sharpening_damper_hr.h"
#include "noise_model_damper.h"
#include "scorpio_damper.h"
#include "scythe_damper.h"
#include "tuning.h"
#include "ispctl_communication_manager.h"
#include "IFM_Types.h"
#include "TraceObject.h"

typedef enum
{
   ISP_DAMPERS_OK = 0,
   ISP_DAMPERS_CONSTRUCT_FAILED,
   ISP_DAMPERS_DESTROY_FAILED,
   ISP_DAMPERS_INVALID_ARGUMENT,
   ISP_DAMPERS_TUNING_ERROR,
   ISP_DAMPERS_CONFIGURE_FAILED,
   ISP_DAMPERS_PE_COMPUTATION_FAILED,
   ISP_DAMPERS_INTERNAL_ERROR
} t_isp_dampers_error_code;

typedef enum
{
   ISP_DAMPERS_DISABLE,
   ISP_DAMPERS_ENABLE,
} t_isp_dampers_control;

class CIspDampers
{
   public:
     CIspDampers(TraceObject *);
#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
     virtual ~CIspDampers();
     virtual t_isp_dampers_error_code Construct();
     virtual t_isp_dampers_error_code Destroy();
     virtual void                     RsoControl(t_isp_dampers_control);
     virtual void                     SharpeningLrControl(t_isp_dampers_control);
     virtual void                     SharpeningHrControl(t_isp_dampers_control);
     virtual void                     ScorpioControl(t_isp_dampers_control);
     virtual void                     DefCorControl(t_isp_dampers_control);
     virtual void                     NoiseFilterControl(t_isp_dampers_control);
     virtual void                     NoiseModelControl(t_isp_dampers_control);
     virtual void                     ScytheControl(t_isp_dampers_control);
     virtual void                     SetSharpnessOmxSetting(t_sint32);
     virtual t_isp_dampers_error_code SetMappings(CTuning*);
     virtual t_isp_dampers_error_code Configure(CTuning*, const e_iqset_id);
     virtual t_isp_dampers_error_code ComputeAndQueuePEs(CIspctlCom*, const IFM_DAMPERS_DATA*);
#else
     ~CIspDampers();
     t_isp_dampers_error_code Construct();
     t_isp_dampers_error_code Destroy();
     void                     RsoControl(t_isp_dampers_control);
     void                     SharpeningLrControl(t_isp_dampers_control);
     void                     SharpeningHrControl(t_isp_dampers_control);
     void                     ScorpioControl(t_isp_dampers_control);
     void                     DefCorControl(t_isp_dampers_control);
     void                     NoiseFilterControl(t_isp_dampers_control);
     void                     NoiseModelControl(t_isp_dampers_control);
     void                     ScytheControl(t_isp_dampers_control);
     void                     SetSharpnessOmxSetting(t_sint32);
     t_isp_dampers_error_code SetMappings(CTuning*);
     t_isp_dampers_error_code Configure(CTuning*, const e_iqset_id);
     t_isp_dampers_error_code ComputeAndQueuePEs(CIspctlCom*, const IFM_DAMPERS_DATA*);
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
     static const char*       ErrorCode2String(const t_isp_dampers_error_code);

#ifdef CAMERA_ENABLE_OMX_3A_EXTENSION
   protected:
#else
   private:
#endif // CAMERA_ENABLE_OMX_3A_EXTENSION
     bool                    bRsoEnabled;
     bool                    bSharpeningLrEnabled;
     bool                    bSharpeningHrEnabled;
     bool                    bScorpioEnabled;
     bool                    bDefCorEnabled;
     bool                    bNoiseFilterEnabled;
     bool                    bNoiseModelEnabled;
     bool                    bScytheEnabled;
     CRsoDamper              iRsoDamper;
     CDefCorDamper           iDefCorDamper;
     CNoiseFilterDamper      iNoiseFilterDamper;
     CSharpeningDamperLR     iSharpeningDamperLR;
     CSharpeningDamperHR     iSharpeningDamperHR;
     CNoiseModelDamper       iNoiseModelDamper;
     CScorpioDamper          iScorpioDamper;
     CScytheDamper           iScytheDamper;
   public:
     TraceObject*            mTraceObject;
};

#endif // _ISP_DAMPERS_
