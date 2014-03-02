/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#define LSIM2CP_C

#ifdef AGPS_FTR

#include "lsimcp.h"

void lsim2_01HandleGpsMsrResultInd(t_RtkObject* p_FsmObject)
{
    t_lsimcp_MsrResultIndM* pl_RecvdGpsMsrResultInd = ( t_lsimcp_MsrResultIndM* ) MC_LSIMCP_GET_MSG_POINTER(p_FsmObject);

    lsim4_02MeasResultInd(pl_RecvdGpsMsrResultInd->v_SessionID, pl_RecvdGpsMsrResultInd->v_FinalReport, &(pl_RecvdGpsMsrResultInd->v_MsrRes));
}

void lsim2_02HandleGpsPosInd(t_RtkObject* p_FsmObject)
{
    t_lsimcp_PosIndM* pl_RcvdGpsPosInd   = ( t_lsimcp_PosIndM* ) MC_LSIMCP_GET_MSG_POINTER(p_FsmObject);

    lsim4_03PosnResultInd(pl_RcvdGpsPosInd->v_SessionID, pl_RcvdGpsPosInd->v_FinalReport, &(pl_RcvdGpsPosInd->v_GpsPosition) );
}

void lsim2_03HandleGpsAidRequInd(t_RtkObject* p_FsmObject)
{
    t_lsimcp_AidRequIndM* pl_RcvdGpsAidInd = ( t_lsimcp_AidRequIndM* ) MC_LSIMCP_GET_MSG_POINTER(p_FsmObject);

    lsim4_01AidRequest(&(pl_RcvdGpsAidInd->v_GpsAidRequest));
}

void lsim2_04HandleGpsAbortCnf(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;
}

void lsim2_05HandleGpsMsAssistedCnf(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;
}

void lsim2_06HandleGpsMsBasedCnf(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;
}

void lsim2_07HandleGpsLocNotifyCnf( t_RtkObject* p_FsmObject )
{
    t_lsimcp_LocNotifyCnfM * pl_NotifRsp = (t_lsimcp_LocNotifyCnfM*)MC_LSIMCP_GET_MSG_POINTER( p_FsmObject );
    lsim4_17SendLocNotifyRsp(pl_NotifRsp->v_SsSessionID , pl_NotifRsp->v_LocNotifyRes.v_UserRsp);
}
/*+LMSqcMOLR*/

void lsim2_08HandleMolrStartInd( t_RtkObject* p_FsmObject )
{
    t_lsimcp_MolrStartIndM * pl_MolrStartInd       = (t_lsimcp_MolrStartIndM*)MC_LSIMCP_GET_MSG_POINTER( p_FsmObject );

    t_lsimcp_GpsAidRequest   vl_RecvdAidReq        = pl_MolrStartInd->v_GpsAidRequest;
    t_lsimcp_QoP             vl_RecvdQopReq        = pl_MolrStartInd->v_QopRequest;
    t_lsimcp_3rdPartyClient  vl_RecvdClientDetails = pl_MolrStartInd->v_3rdPartyRequest;

    s_gnsCP_GpsAidRequest    vl_AidReq;
    s_gnsCP_QoP              vl_QopReq;
    s_gnsCP_3rdPartyClient   vl_ThirdPartyClient;

    uint8_t vl_index=0;
/* Aid Request parameter mapping */
    memset(vl_AidReq.a_Iode,  0, sizeof(uint8_t)*(K_gnsCP_MAX_SAT_NR));
    memset(vl_AidReq.a_SatID, 0, sizeof(uint8_t)*(K_gnsCP_MAX_SAT_NR));

    vl_AidReq.v_AidMask             =     vl_RecvdAidReq.v_AidMask;
    vl_AidReq.v_GpsWeek             =     vl_RecvdAidReq.v_GpsWeek;
    vl_AidReq.v_GpsToe              =     vl_RecvdAidReq.v_GpsTow;
    vl_AidReq.v_TTOELimit           =     vl_RecvdAidReq.v_TTOELimit;
    vl_AidReq.v_NrOfSats            =     vl_RecvdAidReq.v_NrOfSats;

    for(vl_index=0; vl_index < LSIMCP_MAX_SAT_NR; vl_index++)
    {
        vl_AidReq.a_Iode[vl_index]  =     vl_RecvdAidReq.a_Iode[vl_index];
        vl_AidReq.a_SatID[vl_index] =     vl_RecvdAidReq.a_SatID[vl_index];
    }

/* Qop parameter mapping */
    vl_QopReq.v_Horacc              =      vl_RecvdQopReq.v_Horacc;
    vl_QopReq.v_Veracc              =      vl_RecvdQopReq.v_Veracc;
    vl_QopReq.v_MaxLocAge           =      vl_RecvdQopReq.v_MaxLocAge;
    vl_QopReq.v_Delay               =      vl_RecvdQopReq.v_Delay;

/* Third Part Client details parameter mapping */


    vl_ThirdPartyClient.v_ClientIdConfig          =     vl_RecvdClientDetails.v_ClientIdConfig;
    vl_ThirdPartyClient.v_ClientIdPhoneNumberLen  =     vl_RecvdClientDetails.v_ClientIdPhoneNumberLen;
    vl_ThirdPartyClient.v_ClientIdTonNpi          =     vl_RecvdClientDetails.v_ClientIdTonNpi;
    vl_ThirdPartyClient.v_MlcNumConfig            =     vl_RecvdClientDetails.v_MlcNumConfig;
    vl_ThirdPartyClient.v_MlcNumPhoneNumberLen    =     vl_RecvdClientDetails.v_MlcNumPhoneNumberLen;
    vl_ThirdPartyClient.v_MlcNumTonNpi            =     vl_RecvdClientDetails.v_MlcNumTonNpi;

    memcpy(vl_ThirdPartyClient.v_ClientIdPhoneNumber, 
           vl_RecvdClientDetails.v_ClientIdPhoneNumber,
           K_gnsCP_MAX_PHONE_NUM_LENGTH );

    memcpy(vl_ThirdPartyClient.v_MlcNumPhoneNumber,
           vl_RecvdClientDetails.v_MlcNumPhoneNumber,
           K_gnsCP_MAX_PHONE_NUM_LENGTH );
    lsim4_21SendMolrStartInd(pl_MolrStartInd->v_SsSessionID,
                             pl_MolrStartInd->v_Option,
                             pl_MolrStartInd->v_ServiceType,
                             pl_MolrStartInd->v_MolrType,
                             (s_gnsCP_GpsAidRequest *)&vl_AidReq,
                             (s_gnsCP_QoP*)&vl_QopReq,
                             (s_gnsCP_3rdPartyClient*)&vl_ThirdPartyClient,
                              pl_MolrStartInd->v_SupportedGADShapesMask );
                             

}



void lsim2_09HandleMolrStopInd( t_RtkObject* p_FsmObject )
{
    t_lsimcp_MolrStopIndM * pl_MolrStopInd = (t_lsimcp_MolrStopIndM*)MC_LSIMCP_GET_MSG_POINTER( p_FsmObject );

    lsim4_22SendMolrStopInd(pl_MolrStopInd->v_SsSessionID ,
                            pl_MolrStopInd->v_MolrType,
                            pl_MolrStopInd->v_Error);
}



void lsim2_10HandlePosCapabilityInd( t_RtkObject* p_FsmObject )
{
    t_lsimcp_PosCapabilityIndM * pl_PosCapInd = (t_lsimcp_PosCapabilityIndM*)MC_LSIMCP_GET_MSG_POINTER( p_FsmObject );

    DEBUG_LOG_PRINT_LEV2((" + lsim2_10HandlePosCapabilityInd() %u", pl_PosCapInd->v_Initial));

    if(pl_PosCapInd->v_Initial)
        MC_RTK_PROCESS_START_TIMER(LSIMCP_MODEM_INIT_TIMEOUT, MC_DIN_MILLISECONDS_TO_TICK(K_LSIMCP_MODEM_INIT_TIMER_DURATION));
    else
          lsim4_23SendPosCapabilityInd();
}

void lsim2_11HandlePosCapabilityDelay( t_RtkObject* p_FsmObject )
{
    p_FsmObject = p_FsmObject;

    DEBUG_LOG_PRINT_LEV2((" + lsim2_11HandlePosCapabilityDelay() Timer expired"));
    lsim4_23SendPosCapabilityInd();
    return;
}

/*-LMSqcMOLR*/

#endif /* #ifdef AGPS_FTR */

#undef LSIM2CP_C
