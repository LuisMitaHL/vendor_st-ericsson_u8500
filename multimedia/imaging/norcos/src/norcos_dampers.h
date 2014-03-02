/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NORCOS_DAMPERS_H_
#define _NORCOS_DAMPERS_H_

#include "damper_base.h"
#include "damper_shared.h"


class CNorcosDamperFlatDetectGrade: public CSharedDamper
{
   public:
      CNorcosDamperFlatDetectGrade(TraceObject*);
      t_damper_error_code Construct();
      t_damper_error_code SetDamper( const float* pValues, const int aNumRaws, const int aNumCols);
      t_damper_error_code EvaluateDamper( const t_damper_base_values* pBaseValues, float* pDamperValue);
};

class CNorcosDamperFlatParam: public CSharedDamper
{
   public:
      CNorcosDamperFlatParam(TraceObject*);
      t_damper_error_code Construct();
      t_damper_error_code SetDamper( const float* pValues, const int aNumRaws, const int aNumCols);
      t_damper_error_code EvaluateDamper( const t_damper_base_values* pBaseValues, float* pDamperValue);
};
#endif // _NORCOS_DAMPERS_H_
