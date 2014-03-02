/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _RSO_DAMPER_H_
#define _RSO_DAMPER_H_

#include "damper_base.h"
#include "damper_shared.h"
#include "ispctl_communication_manager.h"

typedef enum
{
   RSO_DCTERM_RR,
   RSO_SLANTX_RR,
   RSO_SLANTY_RR,

   RSO_DCTERM_GR,
   RSO_SLANTX_GR,
   RSO_SLANTY_GR,

   RSO_DCTERM_GB,
   RSO_SLANTX_GB,
   RSO_SLANTY_GB,

   RSO_DCTERM_BB,
   RSO_SLANTX_BB,
   RSO_SLANTY_BB,

   RSO_DAMPERS_NUM
} t_rso_damper_id;

class CRsoDamper: public CSharedDamper
{
   public:
      CRsoDamper(TraceObject *);
      t_damper_error_code Construct();
      t_damper_error_code SetDamper( const t_rso_damper_id, const float*, const int, const int);
      t_damper_error_code ComputeAndQueuePEs( CIspctlCom*, const t_damper_base_values*);
};

#endif // _RSO_DAMPER_H_
