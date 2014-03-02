/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ISP_DAMPERS_EXT_
#define _ISP_DAMPERS_EXT_

#include <inc/typedef.h>
#include "isp_dampers.h"

class CIspDampersExt : public CIspDampers
{
public:
  CIspDampersExt(TraceObject *);
  virtual t_isp_dampers_error_code Construct();
  virtual t_isp_dampers_error_code Destroy();
  virtual t_isp_dampers_error_code SetMappings(CTuning*)
  {
    return ISP_DAMPERS_OK;
  };
  virtual t_isp_dampers_error_code Configure(CTuning*, const e_iqset_id)
  {
    return ISP_DAMPERS_OK;
  };
  virtual t_isp_dampers_error_code ComputeAndQueuePEs(CIspctlCom*, const IFM_DAMPERS_DATA*);
};

#endif // _ISP_DAMPERS_EXT_
