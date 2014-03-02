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
#define FILE_NUMBER     6

void lsim6_01ClockCallInd( s_gnsFA_ClockCalInd* pp_ClockCal )
{
    t_lsimccm_ClockCalIndM* pl_ClockCallInd=NULL;
    
    pl_ClockCallInd  = (t_lsimccm_ClockCalIndM*)MC_RTK_GET_MEMORY (sizeof(*pl_ClockCallInd));

    pl_ClockCallInd->v_Status            = pp_ClockCal->v_Status;
    pl_ClockCallInd->v_FreqLockStatus    = pp_ClockCal->v_FreqLockStatus;
    pl_ClockCallInd->v_ErrorEstimateRMS    = pp_ClockCal->v_ErrorEstimateRMS;
    pl_ClockCallInd->v_Frequency        = pp_ClockCal->v_Frequency;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCCM, 0,
                              PROCESS_CGPS, 0,
                              LSIMCCM_CLOCK_CAL_IND,
                              (t_MsgHeader *)pl_ClockCallInd);

}

void lsim6_02ClockCalReq( t_lsimccm_ClockCalReqM*  pp_ClockCal  )
{
    u_gnsFA_MsgDataOut    v_MsgData;
    s_gnsFA_ClockCalReq *pl_gnsFA_ClockCalReq=NULL;
    
    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsFAClockCalReq.v_Command     = pp_ClockCal->v_Command;
    v_MsgData.v_GnsFAClockCalReq.v_Frequency   = pp_ClockCal->v_Frequency;
    v_MsgData.v_GnsFAClockCalReq.v_Threshold   = pp_ClockCal->v_Threshold;

    GNS_ExecuteFaCallback( E_gnsFA_CLOCK_CAL_REQ , sizeof( *pl_gnsFA_ClockCalReq ) , &v_MsgData  );
}


#endif /* GPS_FREQ_AID_FTR */


