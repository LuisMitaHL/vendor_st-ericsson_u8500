/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DAMPER_BASE_H_
#define _DAMPER_BASE_H_

#include "damper_func.h"

typedef enum
{
   DAMPER_BASE_OK = 0,
   DAMPER_BASE_NOT_SET,
   DAMPER_BASE_INVALID_ARGUMENT
} t_damper_base_error_code;

typedef enum
{
   // Damper Bases identifiers
   DAMPER_BASE_NONE                         = 0,
   DAMPER_BASE_INTEGRATION_TIME             = 1,
   DAMPER_BASE_ANALOG_GAIN                  = 2,
   DAMPER_BASE_POST_DG_MINIMUM_GAIN         = 3,
   DAMPER_BASE_POST_DG_AVERAGE_GAIN         = 4,
   DAMPER_BASE_POST_ER_MINIMUM_GAIN         = 5,
   DAMPER_BASE_POST_ER_AVERAGE_GAIN         = 6,
   DAMPER_BASE_POST_CS_MINIMUM_GAIN         = 7,
   DAMPER_BASE_POST_CS_AVERAGE_GAIN         = 8,
   DAMPER_BASE_NORMALIZED_PIXEL_LEVEL       = 9,
   DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_RED   = 10,
   DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_GREEN = 11,
   DAMPER_BASE_NORMALIZED_PIXEL_LEVEL_BLUE  = 12,
   DAMPER_BASE_NORMALIZED_SNR               = 13,
   DAMPER_BASE_NORMALIZED_SNR_RED           = 14,
   DAMPER_BASE_NORMALIZED_SNR_GREEN         = 15,
   DAMPER_BASE_NORMALIZED_SNR_BLUE          = 16,
   DAMPER_BASE_COUNT
} t_damper_base_id;

typedef struct
{
   float IntegrationTime;
   float AnalogGain;
   float PostDgMinimumGain;
   float PostDgAverageGain;
   float PostErMinimumGain;
   float PostErAverageGain;
   float PostCsMinimumGain;
   float PostCsAverageGain;
   float NomalizedPixelLevel;
   float NomalizedPixelLevelRed;
   float NomalizedPixelLevelGreen;
   float NomalizedPixelLevelBlue;
   float NomalizedSnr;
   float NomalizedSnrRed;
   float NomalizedSnrGreen;
   float NomalizedSnrBlue;
} t_damper_base_values;

class CDamperBase
{
   public:
     CDamperBase();
     CDamperBase(const char*,const char*);
     t_damper_base_error_code SetBase(t_damper_base_id);
     t_damper_base_error_code SetControlPoints(const float*, const int);
     const float*             GetControlPoints();
     unsigned int             GetNumPoints();
     bool                     IsOk();
     float                    SelectBase(const t_damper_base_values*);
     const char*              GetBaseString();
     static const char*       GetBaseString(t_damper_base_id);

   private:
     void             Initialize();
     const char*      iDamperName;
     const char*      iInstanceName;
     t_damper_base_id iBase;
     unsigned int     iSize;
     float            iControlPoints[CDAMPER_CTRLP_MAX_DIM];

};

#endif // _DAMPER_BASE_H_
