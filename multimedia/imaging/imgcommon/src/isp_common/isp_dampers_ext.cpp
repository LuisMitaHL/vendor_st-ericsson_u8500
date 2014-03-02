/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "ISP_DAMPERS"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "isp_dampers_ext.h"
#include "osi_trace.h"
#include "VhcElementDefs.h"

/*
 * Constructor
 */
CIspDampersExt::CIspDampersExt(TraceObject *traceobj)
  : CIspDampers(traceobj)
{
  IN0(" ");
  OUT0(" ");
}

/*
 * Actual contructor
 */
t_isp_dampers_error_code CIspDampersExt::Construct()
{
  IN0(" ");
  OUTR("%d", ISP_DAMPERS_OK);
  return ISP_DAMPERS_OK;
}

/*
 * Destructor
 */
t_isp_dampers_error_code CIspDampersExt::Destroy()
{
  IN0(" ");
  OUTR("%d", ISP_DAMPERS_OK);
  return ISP_DAMPERS_OK;
}

/*
 * This functions invokes all the dampers to compute Page Elements values
 */
t_isp_dampers_error_code CIspDampersExt::ComputeAndQueuePEs( CIspctlCom*             pIspctlCom,
                                                             const IFM_DAMPERS_DATA* pDampersData)
{
  IN0(" ");

  // Sanity check
  if(pIspctlCom==NULL)
  {
    MSG2("Invalid argument: pIspctlCom=%p, pDampersData=%p\n", pIspctlCom, pDampersData);
    OUTR("%d", ISP_DAMPERS_INVALID_ARGUMENT);
    return ISP_DAMPERS_INVALID_ARGUMENT;
  }

  // no need to handle damper data but should send at least one dummy communication.
  pIspctlCom->queuePE(DeviceParameters_u32_DeviceId_Byte0, 0);

  OUTR("%d", ISP_DAMPERS_OK);
  return ISP_DAMPERS_OK;
}
