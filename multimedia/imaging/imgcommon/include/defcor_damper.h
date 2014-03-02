/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _DEFCOR_DAMPER_H_
#define _DEFCOR_DAMPER_H_

#include "damper_base.h"
#include "damper_shared.h"
#include "ispctl_communication_manager.h"

typedef enum
{
   DEFCOR_DUSTER_RC_THRESHOLD,
   DEFCOR_DUSTER_CC_SIGMA_FACTOR,
   DEFCOR_DAMPERS_NUM
} t_defcor_damper_id;

class CDefCorDamper: public CSharedDamper
{
   public:
      CDefCorDamper(TraceObject *);
      t_damper_error_code Construct();
      t_damper_error_code SetDamper( const t_defcor_damper_id, const float*, const int, const int);
      t_damper_error_code ComputeAndQueuePEs( CIspctlCom*, const t_damper_base_values*);
};

#endif // _DEFCOR_DAMPER_H_
