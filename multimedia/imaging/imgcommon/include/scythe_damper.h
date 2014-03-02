/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SCYTHE_DAMPER_H_
#define _SCYTHE_DAMPER_H_

#include "damper_base.h"
#include "damper_shared.h"
#include "ispctl_communication_manager.h"

typedef enum
{
   SCYTHE_CONTROL_LO,
   SCYTHE_CONTROL_HI,
   SCYTHE_DAMPERS_NUM
} t_scythe_damper_id;


class CScytheDamper: public CSharedDamper
{
   public:
     CScytheDamper(TraceObject *);
     t_damper_error_code Construct();
     t_damper_error_code SetDamper( const t_scythe_damper_id, const float* pValues, const int aNumRows, const int aNumCols);
     t_damper_error_code ComputeAndQueuePEs(CIspctlCom*, const t_damper_base_values*);
};

#endif // _SCYTHE_DAMPER_H_
