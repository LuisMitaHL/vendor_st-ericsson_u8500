/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SHARPENING_DAMPER_HR_H_
#define _SHARPENING_DAMPER_HR_H_

#include "damper_base.h"
#include "damper_shared.h"
#include "ispctl_communication_manager.h"

typedef enum
{
   SHARPENING_ADSOC_GAIN_HR,
   SHARPENING_ADSOC_CORING_HR,
   SHARPENING_DAMPERS_HR_NUM
} t_sharpening_damper_hr_id;

class CSharpeningDamperHR: public CSharedDamper
{
   public:
      CSharpeningDamperHR(TraceObject *);
      t_damper_error_code Construct();
      void                SetOmxMapping(float,float);
      void                SetOmxSetting(t_sint32);
      t_damper_error_code SetDamper( const t_sharpening_damper_hr_id, const float*, const int, const int);
      t_damper_error_code ComputeAndQueuePEs( CIspctlCom*, const t_damper_base_values*);

   private:
      float    iScaleFactorGain;
      float    iScaleFactorOffset;
      t_sint32 iOmxSharpness;
};

#endif // _SHARPENING_DAMPER_HR_H_
