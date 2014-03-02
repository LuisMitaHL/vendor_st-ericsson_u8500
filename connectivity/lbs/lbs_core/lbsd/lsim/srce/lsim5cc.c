/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define LSIM6CCGNS_C

#ifdef GPS_FREQ_AID_FTR

#include "lsimccm.h"

#undef  FILE_NUMBER
#define FILE_NUMBER     5

void lsim5_01HandleClockCalReq(t_RtkObject* p_FsmObject)
{
    t_lsimccm_ClockCalReqM* pl_ClockCalReq = 
                ( t_lsimccm_ClockCalReqM* ) (p_FsmObject->u_ReceivedObj.p_MessageAddress);

    lsim6_02ClockCalReq(pl_ClockCalReq);
}


#endif /* GPS_FREQ_AID_FTR */

