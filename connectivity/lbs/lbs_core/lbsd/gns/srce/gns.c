/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/

#include "gns.h"
#include "gnsInternal.h"
#include "gnsFsm.h"

#ifdef AGPS_FTR
#include "gnsCP_Api.h"
#include "lsimcp.h"
#endif /* AGPS_FTR */
#ifdef GNS_CELL_INFO_FTR
#include "gnsCellInfo_Api.h"
#endif

#ifdef AGPS_UP_FTR
#include "gnsSUPL_Api.h"
#define ACCESS_RIGHTS_LSIMUP
#include "lsimup.h"
#undef ACCESS_RIGHTS_LSIMUP
#endif /* AGPS_UP_FTR */

#ifdef GPS_FREQ_AID_FTR
#include "lsimccm.h"
#endif /* AGPS_FREQ_AID_FTR */
#ifdef AGPS_TIME_SYNCH_FTR
#include "lsimfta.h"
#endif /* AGPS_TIME_SYNCH_FTR */


#include "halgps.h"

#ifdef __RTK_E__
/* This is defined in the agpsosa.We do not have one for RTK */
#define OSA_Free( X )    MC_RTK_FREE_MEMORY( X )
#endif

static bool                 vg_DisableLogging;

#ifndef __RTK_E__
static bool                 vg_GnsInit;
bool GNS_Initialize( )
{
    if( !vg_GnsInit )
    {
        LbsOsaTrace_Init();
        AgpsSpawnFsm( g_AgpsFsmTable );
        vg_GnsInit = TRUE;
        vg_DisableLogging = FALSE;
    }

    return vg_GnsInit;
}
#endif /* __RTK_E__ */



#ifdef AGPS_UP_FTR

static t_GnsWirelessCallback vg_gnsWirelessNetworkInfoCallback;

bool GNS_WirelessInitialize(t_GnsWirelessCallback v_Callback )
{
    if(v_Callback != NULL)
    {
        vg_gnsWirelessNetworkInfoCallback = v_Callback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
void    GNS_ExecuteWirelessCallback( e_gnsWIRELESS_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsWIRELESS_MsgData *pp_MsgData )
{
    if(vg_gnsWirelessNetworkInfoCallback != NULL)
    {
        vg_gnsWirelessNetworkInfoCallback(v_MsgType,v_MsgLen,pp_MsgData);
    }
}


static t_GnsSuplCallback     vg_GnsSuplCallback;

bool GNS_SuplInitialize( t_GnsSuplCallback v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsSuplCallback = v_Callback;
        return TRUE;
    }
    else return FALSE;
}

void    GNS_ExecuteSuplCallback( e_gnsSUPL_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsSUPL_MsgData *pp_MsgData )
{
    if( NULL != vg_GnsSuplCallback )
    {
        vg_GnsSuplCallback( v_MsgType , v_MsgLen , pp_MsgData );
    }
}

void GNS_SuplTcpIpConnectCnf( t_GnsConnectionHandle vp_ConnectionHandle )
{
    lsim3_05TcpIpConnectCnf(vp_ConnectionHandle);
}

void GNS_SuplTcpIpConnectErr( t_GnsConnectionHandle vp_ConnectionHandle,int v_ErrorType )
{
    lsim3_06TcpIpConnectErr(vp_ConnectionHandle, v_ErrorType);
}

void GNS_SuplTcpIpDisconnectCnf( t_GnsConnectionHandle vp_ConnectionHandle )
{
    lsim3_07TcpIpDisconnectInd(vp_ConnectionHandle);
}

void GNS_SuplTcpIpDisconnectInd( t_GnsConnectionHandle vp_ConnectionHandle )
{
    lsim3_07TcpIpDisconnectInd(vp_ConnectionHandle);
}


void GNS_SuplSendDataCnf( t_GnsConnectionHandle vp_ConnectionHandle )
{
    lsim3_08SendDataCnf(vp_ConnectionHandle);
}

void GNS_SuplReceiveDataInd( t_GnsConnectionHandle vp_ConnectionHandle , uint8_t* pp_Data , uint32_t vp_DataLen )
{
    lsim3_09ReceiveDataInd(vp_ConnectionHandle,pp_Data,vp_DataLen);
}

void GNS_SuplSmsPushInd( uint8_t* pp_Data , uint32_t vp_DataLen , uint8_t* pp_Hash ,  uint32_t vp_HashLen )
{
    lsim3_10SMSReceiveInd( pp_Data , vp_DataLen ,  pp_Hash , vp_HashLen );
}

void GNS_SuplMobileInfoInd(s_gnsSUPL_MobileInfo* pp_MobileInfo)
{
    lsim3_14MobileInfoUpdate( pp_MobileInfo );
}

void GNS_WirelessNetworkInfoInd(s_gnsWIRELESS_NetworkInfo* pp_WirelessAPInfo)
{
    lsim3_15WirelessNetworkInfoUpdate( pp_WirelessAPInfo );
}

void GNS_SuplEstablishBearerCnf( )
{
    lsim3_16EstablishBearerCnf();
}

void GNS_SuplEstablishBearerErr( )
{
    lsim3_17EstablishBearerErr();
}

void GNS_SuplCloseBearerCnf( )
{
    lsim3_19CloseBearerCnf();
}

void GNS_SuplCloseBearerInd( )
{
    lsim3_19CloseBearerCnf();
}


void GNS_SuplCloseBearerErr( )
{
    lsim3_20CloseBearerErr();
}

#endif /* AGPS_UP_FTR */
#ifdef GNS_CELL_INFO_FTR

static t_GnsCellInfoCallback     vg_GnsCellInfoCallback;

bool GNS_CellInfoInitialize( t_GnsCellInfoCallback v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsCellInfoCallback = v_Callback;
        return TRUE;
    }
    else return FALSE;
}

void    GNS_ExecuteCellInfoCallback( e_gnsCellInfo_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsCellInfo_MsgData *pp_MsgData )
{
    if( NULL != vg_GnsCellInfoCallback )
    {
        vg_GnsCellInfoCallback(v_MsgType , v_MsgLen , pp_MsgData );
    }
}
#endif /* GNS_CELL_INFO_FTR */

#ifdef AGPS_FTR

static t_GnsCpCallback          vg_GnsCpCallback;

bool GNS_CpInitialize( t_GnsCpCallback v_Callback )
{
    if( v_Callback != NULL )
    {
        vg_GnsCpCallback = v_Callback;
        return TRUE;
    }
    else return FALSE;
}

void    GNS_ExecuteCpCallback( e_gnsCP_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsCP_MsgData *pp_MsgData )
{
    if( NULL != vg_GnsCpCallback )
    {
        vg_GnsCpCallback( v_MsgType , v_MsgLen , pp_MsgData );
    }
}

void GNS_CpPosInstructInd( uint8_t vp_SessionId , s_gnsCP_PosInstruct * pp_PosInstruct )
{
    lsim4_04PosInstructInd( vp_SessionId , pp_PosInstruct);
    OSA_Free(pp_PosInstruct);
}

void GNS_CpNavModelElmInd( uint8_t vp_NumSv , s_gnsCP_NavModelElm *pp_NavModelElm )
{
    lsim4_05NavModelElmInd(vp_NumSv,pp_NavModelElm);
    OSA_Free( pp_NavModelElm );
}

void GNS_CpAcquisAssistInd( s_gnsCP_AcquisAssist *pp_AcquisAssist )
{
    lsim4_06AcquisAssistInd( pp_AcquisAssist );
    OSA_Free(pp_AcquisAssist);
}

void GNS_CpAlmanacElm( uint8_t vp_NumSv , s_gnsCP_AlmanacElm *pp_AlmanacElm , s_gnsCP_GlobalHealth *pp_GlobalHealth )
{
    lsim4_07AlmanacElm(vp_NumSv,pp_AlmanacElm,pp_GlobalHealth);
    OSA_Free(pp_AlmanacElm);
    if( NULL != pp_GlobalHealth )
    {
        OSA_Free(pp_GlobalHealth);
    }
}

void GNS_CpRefTimeInd(s_gnsCP_RefTime *pp_RefTime )
{
    lsim4_08RefTimeInd(E_gns_ASSIST_SRC_CP, pp_RefTime);
    OSA_Free(pp_RefTime);
}

void GNS_CpRefTimeWithSrcInd(e_gnsAS_AssistSrc v_Src, s_gnsCP_RefTime *pp_RefTime )
{
    lsim4_08RefTimeInd(v_Src, pp_RefTime);
    OSA_Free(pp_RefTime);
}


void GNS_CpRefPositionInd( s_gnsCP_GpsPosition *pp_RefPosition )
{
    lsim4_09RefPositionInd( pp_RefPosition);
    OSA_Free(pp_RefPosition);
}

void GNS_CpIonoModelInd( s_gnsCP_IonoModel *pp_IonoModel )
{
     lsim4_10IonoModelInd( pp_IonoModel);
    OSA_Free(pp_IonoModel);
}

void GNS_CpUtcModelInd( s_gnsCP_UtcModel *pp_UtcModel )
{
    lsim4_11UtcModelInd( pp_UtcModel);
    OSA_Free(pp_UtcModel);
}

void GNS_CpRtIntegrityInd( s_gnsCP_RTIntegrity *pp_RtIntegrity )
{
    lsim4_12RtIntegrityInd( pp_RtIntegrity);
    OSA_Free(pp_RtIntegrity);
}

void GNS_CpDgpsCorrectionsInd( s_gnsCP_DGpsCorr *pp_DgpsCorr )
{
    lsim4_13DgpsCorrectionsInd( pp_DgpsCorr);
    OSA_Free(pp_DgpsCorr);
}

void GNS_CpAbortInd( s_gnsCP_Abort *pp_Abort )
{
    lsim4_14AbortInd( pp_Abort );
    OSA_Free(pp_Abort);
}

void GNS_CpLocNotificationReq(uint32_t vp_SessionId,s_gnsCP_LocNotification *pp_LocNotif)
{
    lsim4_16SendLocNotifyReq(vp_SessionId, pp_LocNotif );
    OSA_Free( pp_LocNotif );
}
void GNS_CpLocNotificationAbortInd(uint32_t vp_SessionId)
{
    lsim4_24SendLocNotifyAbortInd(vp_SessionId);
}


void GNS_CpResetPositionInd( void )
{
    lsim4_18SendResetGpsDataReq( );
}


/* +LMSqcMOLR */
void GNS_CpMolrAbortInd(uint8_t vp_SessionId,e_gnsCP_MolrErrorReason vp_ErrorReason, e_gnsCP_MolrType vp_MolrType)
{
    lsim4_19SendMolrAbortReq(vp_SessionId,vp_ErrorReason,vp_MolrType);
}

void GNS_CpMolrStartRespInd(uint8_t vp_SessionId,s_gnsCP_GpsPosition *pp_GpsPosition)
{
    lsim4_20SendMolrStartResp(vp_SessionId,pp_GpsPosition );
}
/* -LMSqcMOLR */


#endif /* AGPS_FTR */

#ifdef GPS_FREQ_AID_FTR

static t_gnsFA_Callback          vg_GnsFaCallback;

bool GNS_FaInitialize( t_gnsFA_Callback v_FaCallback )
{
    if( v_FaCallback != NULL )
    {
        vg_GnsFaCallback = v_FaCallback;
        return TRUE;
    }
    else return FALSE;
}

void GNS_FaClockCalInd( s_gnsFA_ClockCalInd *pp_ClockCal )
{
    lsim6_01ClockCallInd( pp_ClockCal);
    OSA_Free(pp_ClockCal);
}


void GNS_ExecuteFaCallback( e_gnsFA_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsFA_MsgDataOut *pp_MsgData )
{
    if( NULL != vg_GnsFaCallback )
    {
        vg_GnsFaCallback( v_MsgType , v_MsgLen , pp_MsgData );
    }
}

#endif /* GPS_FREQ_AID_FTR */



#ifdef AGPS_TIME_SYNCH_FTR

static t_gnsFTA_Callback   vg_GnsFtaCallback = NULL;

bool GNS_FtaInitialize( t_gnsFTA_Callback v_FtaCallback )
{
    if( v_FtaCallback != NULL )
    {
        vg_GnsFtaCallback = v_FtaCallback;
        return TRUE;
    }
    else return FALSE;
}

void GNS_FtaPulseCnf( s_gnsFTA_PulseCnf *p_PulseTime )
{
    lsim7_03SendFtaPulseCnf(p_PulseTime);
    if( NULL != p_PulseTime )
    {
        OSA_Free( p_PulseTime );
    }
}


void GNS_ExecuteFtaCallback( e_gnsFTA_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsFTA_MsgDataOut *pp_MsgData )
{
    if( NULL != vg_GnsFtaCallback )
    {
        vg_GnsFtaCallback( v_MsgType , v_MsgLen , pp_MsgData );
    }
}

#endif /* GPS_FREQ_AID_FTR */
