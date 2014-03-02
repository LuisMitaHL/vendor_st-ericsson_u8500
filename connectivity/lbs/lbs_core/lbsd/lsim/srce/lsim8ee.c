/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define LSIM8EE_C


#include "lsimee.h"


#undef  FILE_NUMBER
#define FILE_NUMBER     1

void lsim8_1EEGetEphemerisRsp(s_gnsEE_NavDataList *p_NavDataList)
{
    t_lsimee_GetEphemerisRspM* p_GetEphemerisRspM=NULL;
    uint8_t                            vl_Index = 0;

    DEBUG_LOG_PRINT_LEV1(("lsim8_1EEGetEphemerisRsp"));

    p_GetEphemerisRspM = (t_lsimee_GetEphemerisRspM*) MC_RTK_GET_MEMORY(sizeof(*p_GetEphemerisRspM ));

    p_GetEphemerisRspM->v_GetEphemeris.v_NumEntriesGps = p_NavDataList->v_NumEntriesGps;
    p_GetEphemerisRspM->v_GetEphemeris.v_NumEntriesGlonass = p_NavDataList->v_NumEntriesGlonass;

    DEBUG_LOG_PRINT_LEV1(("NumEntriesGPS =%d\n",p_GetEphemerisRspM->v_GetEphemeris.v_NumEntriesGps));
    DEBUG_LOG_PRINT_LEV1(("NumEntriesGlonass =%d\n",p_GetEphemerisRspM->v_GetEphemeris.v_NumEntriesGlonass));    

    for(vl_Index = 0 ; vl_Index < p_NavDataList->v_NumEntriesGps ; vl_Index++)
    {
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Prn          = p_NavDataList->a_GpsList[vl_Index].v_Prn;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_CAOrPOnL2    = p_NavDataList->a_GpsList[vl_Index].v_CAOrPOnL2;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Ura          = p_NavDataList->a_GpsList[vl_Index].v_Ura;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Health       = p_NavDataList->a_GpsList[vl_Index].v_Health;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_L2PData      = p_NavDataList->a_GpsList[vl_Index].v_L2PData;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Ure          = p_NavDataList->a_GpsList[vl_Index].v_Ure;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_AODO         = p_NavDataList->a_GpsList[vl_Index].v_AODO;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_TGD          = p_NavDataList->a_GpsList[vl_Index].v_TGD;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Af2          = p_NavDataList->a_GpsList[vl_Index].v_Af2;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_EphemFit    = p_NavDataList->a_GpsList[vl_Index].v_EphemFit;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_GpsWeek     = p_NavDataList->a_GpsList[vl_Index].v_GpsWeek;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_IoDc         = p_NavDataList->a_GpsList[vl_Index].v_IoDc;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Toc          = p_NavDataList->a_GpsList[vl_Index].v_Toc;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Toe          = p_NavDataList->a_GpsList[vl_Index].v_Toe;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Iode         = p_NavDataList->a_GpsList[vl_Index].v_Iode;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Af1          = p_NavDataList->a_GpsList[vl_Index].v_Af1;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Crs          = p_NavDataList->a_GpsList[vl_Index].v_Crs;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_DeltaN      = p_NavDataList->a_GpsList[vl_Index].v_DeltaN;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Cuc          = p_NavDataList->a_GpsList[vl_Index].v_Cuc;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Cus          = p_NavDataList->a_GpsList[vl_Index].v_Cus;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Cic          = p_NavDataList->a_GpsList[vl_Index].v_Cic;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Cis          = p_NavDataList->a_GpsList[vl_Index].v_Cis;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Crc          = p_NavDataList->a_GpsList[vl_Index].v_Crc;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_IDot        = p_NavDataList->a_GpsList[vl_Index].v_IDot;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_E            = p_NavDataList->a_GpsList[vl_Index].v_E ;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_SqrtA       = p_NavDataList->a_GpsList[vl_Index].v_SqrtA;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Af0          = p_NavDataList->a_GpsList[vl_Index].v_Af0;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_M0           = p_NavDataList->a_GpsList[vl_Index].v_M0;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Omega0       = p_NavDataList->a_GpsList[vl_Index].v_Omega0;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_I0           = p_NavDataList->a_GpsList[vl_Index].v_I0;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_W            = p_NavDataList->a_GpsList[vl_Index].v_W;
        p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_OmegaDot    = p_NavDataList->a_GpsList[vl_Index].v_OmegaDot;
        
        DEBUG_LOG_PRINT_LEV1(("GPS-v_Prn =%u\n",p_GetEphemerisRspM->v_GetEphemeris.a_GpsList[vl_Index].v_Prn));

    }    

    for(vl_Index = 0 ; vl_Index < p_NavDataList->v_NumEntriesGlonass ; vl_Index++)
    {
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Slot = p_NavDataList->a_GloList[vl_Index].v_Slot;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_FT = p_NavDataList->a_GloList[vl_Index].v_FT;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_M = p_NavDataList->a_GloList[vl_Index].v_M;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Bn  = p_NavDataList->a_GloList[vl_Index].v_Bn;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_UtcOffset = p_NavDataList->a_GloList[vl_Index].v_UtcOffset;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_FreqChannel = p_NavDataList->a_GloList[vl_Index].v_FreqChannel;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Gamma = p_NavDataList->a_GloList[vl_Index].v_Gamma;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Lsx = p_NavDataList->a_GloList[vl_Index].v_Lsx;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Lsy = p_NavDataList->a_GloList[vl_Index].v_Lsy;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Lsz = p_NavDataList->a_GloList[vl_Index].v_Lsz;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_GloSec = p_NavDataList->a_GloList[vl_Index].v_GloSec;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_TauN     = p_NavDataList->a_GloList[vl_Index].v_TauN;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_X = p_NavDataList->a_GloList[vl_Index].v_X;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Y = p_NavDataList->a_GloList[vl_Index].v_Y;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Z = p_NavDataList->a_GloList[vl_Index].v_Z;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Vx = p_NavDataList->a_GloList[vl_Index].v_Vx;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Vy = p_NavDataList->a_GloList[vl_Index].v_Vy;
        p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Vz = p_NavDataList->a_GloList[vl_Index].v_Vz;
            
        DEBUG_LOG_PRINT_LEV1(("Glonass : v_Slot = %d ,FreqChannel =%d\n",p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_Slot,p_GetEphemerisRspM->v_GetEphemeris.a_GloList[vl_Index].v_FreqChannel));

    }    

    
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMEE,0,PROCESS_CGPS,0,CGPS_LSIMEE_GET_EPHEMERIS_RSP ,(t_MsgHeader*) p_GetEphemerisRspM);

}

/*XYBRID Integration :194997*/
void lsim8_2EEGetRefLocationRsp(t_gnsEE_RefPosition *p_RefPosition)
{
    t_lsimee_GetRefLocationRspM* p_GetRefLocationRspM=NULL;


    DEBUG_LOG_PRINT_LEV1(("lsim8_2EEGetRefLocationRsp"));

    p_GetRefLocationRspM = (t_lsimee_GetRefLocationRspM*) MC_RTK_GET_MEMORY(sizeof(*p_GetRefLocationRspM ));

    
    p_GetRefLocationRspM->v_GetrRefLocation.v_Type = p_RefPosition->v_RefPosData.v_Type ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_Latitude = p_RefPosition->v_RefPosData.v_Latitude ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_Longitude = p_RefPosition->v_RefPosData.v_Longitude ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_Altitude = p_RefPosition->v_RefPosData.v_Altitude ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_ECEF_X = p_RefPosition->v_RefPosData.v_ECEF_X ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_ECEF_Y = p_RefPosition->v_RefPosData.v_ECEF_Y ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_ECEF_Z = p_RefPosition->v_RefPosData.v_ECEF_Z ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_UncertSemiMajor = p_RefPosition->v_RefPosData.v_UncertSemiMajor ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_UncertSemiMinor = p_RefPosition->v_RefPosData.v_UncertSemiMinor ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_OrientMajorAxis = p_RefPosition->v_RefPosData.v_OrientMajorAxis ;
    p_GetRefLocationRspM->v_GetrRefLocation.v_Confidence = p_RefPosition->v_RefPosData.v_Confidence ;
    
    DEBUG_LOG_PRINT_LEV1(("p_GetRefLocationRspM->v_GetrRefLocation.v_Latitude =%f\n",p_GetRefLocationRspM->v_GetrRefLocation.v_Latitude));
    DEBUG_LOG_PRINT_LEV1(("p_GetRefLocationRspM->v_GetrRefLocation.v_Longitude =%f\n",p_GetRefLocationRspM->v_GetrRefLocation.v_Longitude));
    DEBUG_LOG_PRINT_LEV1(("p_GetRefLocationRspM->v_GetrRefLocation.v_Altitude =%f\n",p_GetRefLocationRspM->v_GetrRefLocation.v_Altitude));
    
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMEE,0,PROCESS_CGPS,0,CGPS_LSIMEE_GET_REFLOCATION_RSP ,(t_MsgHeader*) p_GetRefLocationRspM);

}


/*XYBRID Integration :194997*/
void lsim8_3GetExtendedEphemerisReq(t_RtkObject* p_FsmObject)
{
    t_gnsEE_ExtendedEphInd *P_GnsEEReq= (t_gnsEE_ExtendedEphInd *)malloc( sizeof( *P_GnsEEReq ) );
    t_lsimee_GetEphemerisReq *p_EEReq = (t_lsimee_GetEphemerisReq*)p_FsmObject->u_ReceivedObj.p_MessageAddress;

    P_GnsEEReq->v_PrnBitMask = p_EEReq->v_PrnBitMask ;
    P_GnsEEReq->v_ConstelType = p_EEReq->v_ConstelType;
    P_GnsEEReq->v_EERefGPSTime = p_EEReq->v_EERefGPSTime;
    GNS_EEGetEphemerisRequest(P_GnsEEReq);

}


void lsim8_4GetRefLocationReq(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;
    GNS_EEGetRefLocationRequest();

}


void lsim8_5GetRefTimeReq(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;
    GNS_EEGetRefTimeRequest();
}


void lsim8_6DeleteSeedReq(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;
    GNS_EEDeleteSeedRequest();
}

void lsim8_7FeedBCEInd(t_RtkObject* p_FsmObject)
{
    t_gnsEE_NavDataBCE *P_GnsBCE = (t_gnsEE_NavDataBCE *)malloc( sizeof( *P_GnsBCE ) );
    t_lsimee_SAGPS_BCEData   *pl_SAGPSBCEData = (t_lsimee_SAGPS_BCEData*)p_FsmObject->u_ReceivedObj.p_MessageAddress;
    memcpy(P_GnsBCE, &(pl_SAGPSBCEData->v_NavDataBCE), sizeof(t_lsimee_NavDataBCE));

    GNS_EEFeedBCEInd(P_GnsBCE);

}

