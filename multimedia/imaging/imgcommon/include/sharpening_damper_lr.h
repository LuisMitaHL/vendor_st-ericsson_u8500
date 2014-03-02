/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SHARPENING_DAMPER_LR_H_
#define _SHARPENING_DAMPER_LR_H_

#include "damper_base.h"
#include "damper_shared.h"
#include "ispctl_communication_manager.h"

typedef enum
{
   SHARPENING_ADSOC_GAIN_LR,
   SHARPENING_ADSOC_CORING_LR,
   SHARPENING_DAMPERS_LR_NUM
} t_sharpening_damper_lr_id;

class CSharpeningDamperLR: public CSharedDamper
{
   public:
      CSharpeningDamperLR(TraceObject *);
      t_damper_error_code Construct();
      void                SetOmxMapping(float, float);
      void                SetOmxSetting(t_sint32);
      t_damper_error_code SetDamper( const t_sharpening_damper_lr_id, const float*, const int, const int);
      t_damper_error_code ComputeAndQueuePEs( CIspctlCom*, const t_damper_base_values*);

   private:
      float    iScaleFactorGain;
      float    iScaleFactorOffset;
      t_sint32 iOmxSharpness;
};

#endif // _SHARPENING_DAMPER_LR_H_
