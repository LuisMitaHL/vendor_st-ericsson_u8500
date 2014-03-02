/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps5supl.c
* \date 16/05/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains system entry function for RTK process and FSM table
* All message receive by the other module are receive in this file\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 16.05.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS5SUPL_C__

#ifdef AGPS_UP_FTR

#include "cgpssupl.h"

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 5



/*+LMSqc38060 */
uint32_t vg_CGPS_Supl_Timeout = 0xFFFFFFFF;
/*-LMSqc38060 */




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
/******************************************************************************/
/* CGPS5_00SuplPushInd : generates verification field using GSECURITY and deliver PDU to SUPL library */
/******************************************************************************/
bool CGPS5_00SuplPushInd(uint32_t vp_SuplHandle, t_cgps_SuplPush* pp_SuplPush)
/*bool CGPS5_00SMSPushInd(uint32_t vp_SuplHandle, t_lsimup_SmsPushInd/M* pp_lsimup_SmsPushInd)*/
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */
{
    e_GN_Status vl_status;

/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
    if( pp_SuplPush->v_HashLength == 0 )
/*  if( pp_lsimup_SmsPushInd->v_HashLength == 0 )*/
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */
    {
#ifdef __RTK_E__
    t_gsec_HMAC_Result pl_Ver;
    t_gsec_HMAC_Result *pl_gsec_HMAC_Result=NULL;

/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
    MC_CGPS_TRACE(("CGPS5_00SuplPushInd : CGPS SUPL handle=0x%x", vp_SuplHandle));
/*  MC_CGPS_TRACE(("CGPS5_00SMSPushInd : CGPS SUPL handle=0x%x", vp_SuplHandle)); */
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */

    if (vg_CGPS_Supl_Slp_Config.v_Addr)
    {
        /*TODO: need to be implemented*/
        /* generate the default adresse from the MCC MNC */
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
        MC_CGPS_TRACE(("CGPS5_00SuplPushInd : Hash generated using address: %s lenght: %i", vg_CGPS_Supl_Slp_Config.v_Addr,vg_CGPS_Supl_Slp_Config.v_AddrLen));
/*      MC_CGPS_TRACE(("CGPS5_00SMSPushInd : Hash generated using address: %s lenght: %i", vg_CGPS_Supl_Slp_Config.v_Addr,vg_CGPS_Supl_Slp_Config.v_AddrLen));*/
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */

        if(  GSEC_OK != MC_GSEC_HMAC( GSEC_HMAC_SHA1_MODE,
                                      (uint8_t*)vg_CGPS_Supl_Slp_Config.v_Addr,
                                      vg_CGPS_Supl_Slp_Config.v_AddrLen,
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
                                      pp_SuplPush->v_pointerToPayload,
                                      pp_SuplPush->v_PayloadLength,
/*                                    pp_lsimup_SmsPushInd->v_pointerToPayload,
                                      pp_lsimup_SmsPushInd->v_PayloadLength, */
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */
                                      pl_Ver
                                    )
           )
        {
            MC_CGPS_TRACE(("MC_GSEC_HMAC failed!!!"));
            return FALSE;
        }
    }

    if( TRUE != GN_SUPL_Push_Delivery_In( &((t_CgpsSuplAppli*)vp_SuplHandle)->v_GPSHandle,
                                          &vl_status,
/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
/*                                        (uint16_t)pp_lsimup_SmsPushInd->v_PayloadLength,
                                          (uint8_t*)pp_lsimup_SmsPushInd->v_pointerToPayload,*/
                                          (uint16_t)pp_SuplPush->v_PayloadLength,
                                          (uint8_t*)pp_SuplPush->v_pointerToPayload,
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */
                                          (uint16_t)sizeof( *pl_gsec_HMAC_Result ),
                                          (uint8_t*)pl_Ver
                                        )
      )
    {
        MC_CGPS_TRACE(("GN_SUPL_Push_Delivery_In failed!!!"));
        return FALSE;
    }
#else
        MC_CGPS_TRACE(("WRN: GCGPS5_00SuplPushInd : No hash Supplied!!!"));

        if( TRUE != GN_SUPL_Push_Delivery_In( &((t_CgpsSuplAppli*)vp_SuplHandle)->v_GPSHandle,
                                             &vl_status,
                                             (uint16_t)pp_SuplPush->v_PayloadLength,
                                             (uint8_t*)pp_SuplPush->v_pointerToPayload,
                                             0,
                                             NULL)
         )
        {
           MC_CGPS_TRACE(("GN_SUPL_Push_Delivery_In failed!!!"));
        return FALSE;
        }
#endif /* __RTK_E__ */

    }
    else
    {
        /* We use the hash supplied by LSIMUP */
        if( TRUE != GN_SUPL_Push_Delivery_In( &((t_CgpsSuplAppli*)vp_SuplHandle)->v_GPSHandle,
                                              &vl_status,
                                              (uint16_t)pp_SuplPush->v_PayloadLength,
                                              (uint8_t*)pp_SuplPush->v_pointerToPayload,
                                              (uint16_t)pp_SuplPush->v_HashLength,
                                              (uint8_t*)pp_SuplPush->v_pointerToHash)
          )
        {
            MC_CGPS_TRACE(("GN_SUPL_Push_Delivery_In failed!!!"));
            return FALSE;
        }
    }

    GN_SUPL_Handler();

    MC_CGPS_TRACE(("SMS pushed successfully"));
    return TRUE;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/******************************************************************************/
/* CGPS5_01EstablishBearerSession : establish a bearer session                */
/******************************************************************************/
bool CGPS5_01EstablishBearerSession(void)
{
    t_lsimup_EstablishBearerReqM * pl_lsimup_EstablishBearerReq =NULL;
    pl_lsimup_EstablishBearerReq = (t_lsimup_EstablishBearerReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_EstablishBearerReq));

    MC_CGPS_TRACE(("CGPS5_01EstablishBearerSession : Start"));

    pl_lsimup_EstablishBearerReq->v_SessionHandle     = vg_CGPS_LSIMUP_Session_Handle;
    pl_lsimup_EstablishBearerReq->v_BearerType        = vg_CGPS_SUPL_Bearer.v_BearerType;
    pl_lsimup_EstablishBearerReq->v_SelectedModem     = vg_CGPS_SUPL_Bearer.v_SelectedModem;
    pl_lsimup_EstablishBearerReq->s_PhoneCalledNumber = NULL;

    if (vg_CGPS_SUPL_Bearer.s_APN)
    {
        pl_lsimup_EstablishBearerReq->s_APN = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((const char*)vg_CGPS_SUPL_Bearer.s_APN)+1));
        strncpy((char*)pl_lsimup_EstablishBearerReq->s_APN, (const char*)vg_CGPS_SUPL_Bearer.s_APN, strlen((const char*)vg_CGPS_SUPL_Bearer.s_APN)+1);
        MC_CGPS_TRACE((vg_CGPS_SUPL_Bearer.s_APN));
    }
    else
    {
        MC_CGPS_TRACE(("CGPS5_01EstablishBearerSession : APN empty!!!"));
        pl_lsimup_EstablishBearerReq->s_APN = NULL;
    }

    if (vg_CGPS_SUPL_Bearer.s_Login)
    {
        pl_lsimup_EstablishBearerReq->s_Login = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((const char*)vg_CGPS_SUPL_Bearer.s_Login)+1));
        strncpy((char*)pl_lsimup_EstablishBearerReq->s_Login, (const char*)vg_CGPS_SUPL_Bearer.s_Login, strlen((const char*)vg_CGPS_SUPL_Bearer.s_Login)+1);
        MC_CGPS_TRACE((vg_CGPS_SUPL_Bearer.s_Login));
    }
    else
    {
        MC_CGPS_TRACE(("CGPS5_01EstablishBearerSession : Login empty!!!"));
        pl_lsimup_EstablishBearerReq->s_Login = NULL;
    }

    if (vg_CGPS_SUPL_Bearer.s_Password)
    {
        pl_lsimup_EstablishBearerReq->s_Password = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((const char*)vg_CGPS_SUPL_Bearer.s_Password)+1));
        strncpy((char*)pl_lsimup_EstablishBearerReq->s_Password, (const char*)vg_CGPS_SUPL_Bearer.s_Password, strlen((const char*)vg_CGPS_SUPL_Bearer.s_Password)+1);
        MC_CGPS_TRACE((vg_CGPS_SUPL_Bearer.s_Password));
    }
    else
    {
        MC_CGPS_TRACE(("CGPS5_01EstablishBearerSession : Password empty!!!"));
        pl_lsimup_EstablishBearerReq->s_Password = NULL;
    }

    CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                               PROCESS_LSIMUP,
                               CGPS_LSIMUP_ESTABLISH_BEARER_REQ,
                               (t_MsgHeader *)pl_lsimup_EstablishBearerReq);

    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_ESTABLISHING;
/*+LMSqc38060 */
        vg_CGPS_Supl_Timeout = GN_GPS_Get_OS_Time_ms() + CGPS_SUPL_MAX_BEARER_SETUP_TIME;
/*-LMSqc38060 */

    MC_CGPS_TRACE(("CGPS5_01EstablishBearerSession : End"));

    return TRUE;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/******************************************************************************/
/* CGPS5_02EstablishBearerCnf : Bearer establishment confirmation             */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_02EstablishBearerCnf(t_RtkObject* p_FsmObject)
{
    int8_t vl_index = 0;
    const t_OperationDescriptor* pl_NextState = SAME;

    p_FsmObject = p_FsmObject;

    MC_CGPS_TRACE(("CGPS5_02EstablishBearerCnf start"));
/*+LMSqc38060*/
vg_CGPS_Supl_Timeout = 0xFFFFFFFF;
/*-LMSqc38060*/

    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_ESTABLISHED;

/* ++ LMSqb95626    ANP 03/07/2009 */
    /* ++ LMSqb93693 */
    /* notify the user if it's requested */
    if(vg_DataCntNotificationCallback != NULL)
    {
        vg_DataCntNotificationCallback(TRUE);
    }
    /* -- LMSqb93693 */
/* -- LMSqb95626    ANP 03/07/2009 */

    /* establish the first pending TCPIP connection request */
    for (vl_index = 0;
            (vl_index < K_CGPS_MAX_NUMBER_OF_SUPL)
            && (-1 == vg_CGPS_Supl_Pending_Connection);
                    vl_index++
        )
    {
        if(     ( s_CgpsSupl[vl_index].v_IsRegistered == TRUE )
             && ( s_CgpsSupl[vl_index].v_Suspended    == CGPS_SESSION_NOT_SUSPENDED )
             && ( NULL != s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address )
          )
        {
            MC_CGPS_TRACE(("CGPS5_02EstablishBearerCnf : pending connection found : CGPS Handle=0x%x, GPSHandle=0x%x", &s_CgpsSupl[vl_index], s_CgpsSupl[vl_index].v_GPSHandle));
#ifdef CMCC_LOGGING_ENABLE
            CGPS9_23LogPosition (vl_index, CGPS_SUPL_CMCC_NETWORKCONNECTION_CODE, NULL);
#endif
            CGPS5_25EstablishTcpIpSession( vl_index );

            CGPS5_26LsimupMobileInfoReq();
        }
    }
    MC_CGPS_TRACE(("CGPS5_02EstablishBearerCnf : End"));
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/******************************************************************************/
/* CGPS5_03EstablishBearerErr : Bearer establishment error                    */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_03EstablishBearerErr(t_RtkObject* p_FsmObject)
{
    int8_t vl_index = 0;
    /* t_lsimup_EstablishBearerErrM * pl_lsimup_EstablishBearerErr = (t_lsimup_EstablishBearerErrM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject); */
    const t_OperationDescriptor* pl_NextState = SAME;

    p_FsmObject = p_FsmObject;

    MC_CGPS_TRACE(("CGPS5_03EstablishBearerErr"));
/*+LMSqc38060*/
    vg_CGPS_Supl_Timeout = 0xFFFFFFFF;
/*-LMSqc38060*/

    /* reject all pending TCPIP connection requests */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL; vl_index++)
    {
        if ((s_CgpsSupl[vl_index].v_IsRegistered == TRUE)
             && (NULL != s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address)
          )
        {
            e_GN_Status vl_Status = GN_ERR_CONN_REJECTED;
#ifdef CMCC_LOGGING_ENABLE
            if (vg_CGPS_Supl_Network_Registered == FALSE)
                 CGPS9_23LogPosition (vl_index, CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_CODE, NULL);
            else
                CGPS9_23LogPosition (vl_index, CGPS_SUPL_CMCC_NETWORKCONNECTION_FAILURE_CODE, NULL);
#endif
            GN_SUPL_Connect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status, NULL);
/*+SYScs46505*/
            CGPS4_35UpdateNumSuplFailure(&vg_AssistTrack);
/*-SYScs46505*/

            GN_SUPL_Handler();

            MC_CGPS_TRACE(("CGPS5_03EstablishBearerErr : pending connection removed : CGPS Handle=0x%x, GPSHandle=0x%x", s_CgpsSupl[vl_index], s_CgpsSupl[vl_index].v_GPSHandle));

            CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
        }
    }

    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;

    CGPS0_52ConfigureCgpsNextState();
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/******************************************************************************/
/* CGPS5_04TcpIpConnectCnf : TCPIP connection establishment confirmation      */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_04TcpIpConnectCnf(t_RtkObject* p_FsmObject)
{
    int8_t vl_index = vg_CGPS_Supl_Pending_Connection;
    e_GN_Status vl_Status = GN_SUCCESS;
    uint16_t vl_ret =0;
    const t_OperationDescriptor* pl_NextState = SAME;
    t_lsimup_TcpIpConnectCnfM * pl_lsimup_TcpIpConnectCnf = ( t_lsimup_TcpIpConnectCnfM * )MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    MC_CGPS_TRACE(("CGPS5_04TcpIpConnectCnf start : connectionHandle=0x%x", pl_lsimup_TcpIpConnectCnf->v_ConnectionHandle));
/*+LMSqc38060*/
    vg_CGPS_Supl_Timeout = 0xFFFFFFFF;
/*-LMSqc38060*/

    /* Update the pending Request */
    if (-1 != vl_index)
    {
        if(    s_CgpsSupl[vl_index].v_IsRegistered     == TRUE
            && s_CgpsSupl[vl_index].v_ConnectionHandle == 0 )
        {
            s_CgpsSupl[vl_index].v_ConnectionHandle = pl_lsimup_TcpIpConnectCnf->v_ConnectionHandle;
#ifdef CMCC_LOGGING_ENABLE
            CGPS9_23LogPosition (vl_index, CGPS_SUPL_CMCC_SERVERCONNECTION_CODE, NULL);
#endif
            vl_ret = GN_SUPL_Connect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status, &vg_CGPS_Supl_Location_Id);

            if (vl_ret != FALSE)
            {
                MC_CGPS_TRACE(("CGPS5_04TcpIpConnectCnf : GN_SUPL_Connect_Ind_In return an  Status : 0x%x", vl_Status));
            }

            GN_SUPL_Handler();

            MC_CGPS_TRACE(("CGPS5_04TcpIpConnectCnf : GN_SUPL_Handler called with GPSHandle=0x%x", s_CgpsSupl[vl_index].v_GPSHandle));

            vg_CGPS_Supl_Pending_Connection = -1;
/*+SYScs46505*/
            CGPS4_34UpdateLastSUPLSessionTime(&vg_AssistTrack);
/*-SYScs46505*/
        }
/*+LMSqc38060*/
        else
        {
            /*No taker for this connection, lets do a cleanup in LSIM end*/
            CGPS5_31SendTcpDisconnectReq(vg_CGPS_LSIMUP_Session_Handle, pl_lsimup_TcpIpConnectCnf->v_ConnectionHandle);
            vg_CGPS_Supl_Pending_Connection = -1;

        }
/*-LMSqc38060*/
    }
    else
    {
        CGPS5_31SendTcpDisconnectReq(vg_CGPS_LSIMUP_Session_Handle, pl_lsimup_TcpIpConnectCnf->v_ConnectionHandle);
        vg_CGPS_Supl_Pending_Connection = -1;
    }

    /* search for another pending connection request */
    for( vl_index = 0;
               vl_index < K_CGPS_MAX_NUMBER_OF_SUPL
           && (-1 == vg_CGPS_Supl_Pending_Connection);
                        vl_index++
       )
    {
        if(    ( s_CgpsSupl[vl_index].v_IsRegistered     == TRUE )
            && ( s_CgpsSupl[vl_index].v_ConnectionHandle == 0 )
            && ( NULL != s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address)
            && ( s_CgpsSupl[vl_index].v_Suspended        == CGPS_SESSION_NOT_SUSPENDED)
           )
        {
            MC_CGPS_TRACE(("CGPS5_04TcpIpConnectCnf : pending connection found, CGPS handle=0x%x", s_CgpsSupl[vl_index]));

            /* establishes TCPIP connection */
            CGPS5_25EstablishTcpIpSession ( vl_index );

            CGPS5_26LsimupMobileInfoReq();

            /* Only one TCPIP_CONNECT_REQ at a time */
            break;
        }
    }

    if (pl_lsimup_TcpIpConnectCnf->s_IPAddress != NULL)
        MC_RTK_FREE_MEMORY(pl_lsimup_TcpIpConnectCnf->s_IPAddress);

    MC_CGPS_TRACE(("CGPS5_04TcpIpConnectCnf end"));
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/******************************************************************************/
/* CGPS5_05AdpMobileInfo : Mobile information receipt                         */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_05AdpMobileInfo(t_RtkObject* p_FsmObject)
{
    t_OperationType vl_OpType;
    t_lsimup_MobileInfo*          pl_lsimup_MobileInfo    = NULL;
    t_lsimup_MobileInfoCnfM*      pl_lsimup_MobileInfoCnf = NULL;
    t_lsimup_MobileInfoIndM*      pl_lsimup_MobileInfoInd = NULL;
    t_lsimup_WirelessNetworkInfo* pl_lsimup_WirelessNetworkInfo = NULL;
    const t_OperationDescriptor*  pl_NextState = SAME;
    uint8_t vl_newISDN = FALSE;
    uint8_t vl_newIMSI = FALSE;
    uint8_t vl_ret =0;

    MC_CGPS_TRACE(("Start : CGPS5_05AdpMobileInfo"));

    vl_OpType = MC_RTK_GET_MSG_OPTYPE(p_FsmObject->u_ReceivedObj.p_MessageAddress);

    if (vl_OpType == CGPS_LSIMUP_MOBILE_INFO_CNF)
    {
        pl_lsimup_MobileInfoCnf = (t_lsimup_MobileInfoCnfM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
        pl_lsimup_MobileInfo    = &(pl_lsimup_MobileInfoCnf->s_MobileInfo);
    }
    else
    {
        pl_lsimup_MobileInfoInd = (t_lsimup_MobileInfoIndM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
        pl_lsimup_MobileInfo    = &(pl_lsimup_MobileInfoInd->s_MobileInfo);
    }
    /* +++ LMSqb85982*/
    switch (vg_CGPS_Supl_Location_Id.Type)
    {
        case CIT_gsmCell:
        {
            if(   (vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.NMRCount  >  0)
               && (vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.p_NMRList != NULL))
            {
                MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.p_NMRList);
                vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.NMRCount = 0;
                vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.p_NMRList = NULL;
            }
            break;
        }
        case CIT_wcdmaCell:
        case CIT_cdmaCell:
        {
            if(   (vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.MeasuredResultCount    > 0)
               && (vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.p_MeasuredResultsList != NULL))
            {
                /* +LMSqc22768 : 03-FEB-2010 */
                /* Assuming that we have only wcdma currently */
                s_MeasuredResult *p_Temp;
                uint8_t               vl_Index;
    
                p_Temp = vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.p_MeasuredResultsList;
    
                for( vl_Index = 0 ;
                     vl_Index < vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.MeasuredResultCount ;
                     vl_Index ++ , p_Temp ++ )
                {
                    if( (p_Temp->CellMeasuredResultCount > 0 ) && ( p_Temp->p_CellMeasuredResultsList ) )
                    {
                        /* Free every element of this array */
                        MC_RTK_FREE_MEMORY( p_Temp->p_CellMeasuredResultsList );
                    }
                }
                /* -LMSqc22768 : 03-FEB-2010 */
                MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.p_MeasuredResultsList);
                vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.p_MeasuredResultsList = NULL;
                vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.MeasuredResultCount = 0;
            }
    
            break;
        }
        default:
        {
            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo:Nothing to clear " ));
            /*do nothing*/
            break;
        }
    }

    /* --- LMSqb85982*/
    if(pl_lsimup_MobileInfoInd->s_IsInformationPresent == K_LSIMUP_WIRELESS_NETWORK)
    {

        pl_lsimup_WirelessNetworkInfo = &(pl_lsimup_MobileInfoInd->s_WirelessNetworkInfo);
        if(pl_lsimup_WirelessNetworkInfo->v_AccessPointType == K_LSIMUP_WLAN)
        {
            /*The wireless Nework that we have attached to WLAN so populate WLAN Access Point Info to the vg_CGPS_Supl_Location_Id structure*/
            memset( &vg_CGPS_Supl_Location_Id, 0, sizeof(vg_CGPS_Supl_Location_Id) );
            vg_CGPS_Supl_Location_Id.Type   = CIT_AccessPoint_WLAN;
            vg_CGPS_Supl_Location_Id.Status = CIS_current;

            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr           = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_LS_Addr;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr           = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_MS_Addr;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower     = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_TransmitPower;

            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain       = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_AP_AntennaGain;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR               = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_SNR;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType        = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_DeviceType;


            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength    = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_SignalStrength;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel           = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_Channel;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue          = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_RTDValue;

            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits          = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_RTDUnits;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy          = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_Accuracy;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower  = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_SETTransmitPower;


            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain    = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_SETAntennaGain;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain    = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_SETSNR;
            vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength = pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_SETSignalStrength;

            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: Source v_LS_Addr %d", pl_lsimup_WirelessNetworkInfo->v_AccessPointInfo.v_WlanApInfo.v_LS_Addr));
            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: WlanAccessPointInfo.v_LS_Addr %d", vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr));

            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: WlanAccessPointInfo.v_MS_Addr %d", vg_CGPS_Supl_Location_Id.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr));

        }
        else if(pl_lsimup_WirelessNetworkInfo->v_AccessPointType == K_LSIMUP_WIMAX)
        {
            /*The wireless Network that we have attached to WIMAX so populate WIMAX Access Point Info to the vg_CGPS_Supl_Location_Id structure*/
        }
    }
    else if(pl_lsimup_MobileInfoInd->s_IsInformationPresent == K_LSIMUP_CELLULAR_NETWORK)
    {
        if(    (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MCC_FIELD)
            && (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MNC_FIELD))
        {
            /* MCC or MNC have maybe changed, upadte the operator index */
            CGPS4_17UpdateOperatorID(pl_lsimup_MobileInfo->v_Mcc,pl_lsimup_MobileInfo->v_Mnc);
        }

        if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MSISDN_FIELD)
        {
            if (vg_CGPS_Supl_MSISDN != NULL)
            {
                if (strcmp((const char*)vg_CGPS_Supl_MSISDN,(const char*)pl_lsimup_MobileInfo->s_MSISDN.a_PhoneNumber))
                {
                    MC_RTK_FREE_MEMORY(vg_CGPS_Supl_MSISDN);
                    vl_newISDN = TRUE;
                }
            }
            else
            {
                vl_newISDN = TRUE;
            }
            if (vl_newISDN)
            {
                vg_CGPS_Supl_MSISDN = (int8_t*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((char*)pl_lsimup_MobileInfo->s_MSISDN.a_PhoneNumber)+1));
                strncpy((char*)vg_CGPS_Supl_MSISDN, (char*)( pl_lsimup_MobileInfo->s_MSISDN.a_PhoneNumber), strlen((char*)pl_lsimup_MobileInfo->s_MSISDN.a_PhoneNumber)+1);
            }
        }

        if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_IMSI_FIELD)
        {
            if( vg_CGPS_Supl_IMSI != NULL )
            {
                if (strcmp((const char*)vg_CGPS_Supl_IMSI,(char*)(pl_lsimup_MobileInfo->s_IMSI.a_IMSI)))
                {
                    MC_RTK_FREE_MEMORY(vg_CGPS_Supl_IMSI);
                    vl_newIMSI = TRUE;
                }
            }
            else
            {
                vl_newIMSI = TRUE;
            }
            if (vl_newIMSI)
            {
                vg_CGPS_Supl_IMSI = (int8_t*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((char*)pl_lsimup_MobileInfo->s_IMSI.a_IMSI)+1));
                strncpy((char*)vg_CGPS_Supl_IMSI,(char*) pl_lsimup_MobileInfo->s_IMSI.a_IMSI, strlen((char*)pl_lsimup_MobileInfo->s_IMSI.a_IMSI)+1);
            }
        }

        switch (pl_lsimup_MobileInfo->v_Celltype)
        {
            case K_LSIMUP_GSM_CELL:
            {
                memset( &vg_CGPS_Supl_Location_Id, 0, sizeof(vg_CGPS_Supl_Location_Id) );
                vg_CGPS_Supl_Location_Id.Status = CIS_current;

                vg_CGPS_Supl_Location_Id.Type = CIT_gsmCell;
                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MCC_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refMCC = pl_lsimup_MobileInfo->v_Mcc;
                }
                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MNC_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refMNC = pl_lsimup_MobileInfo->v_Mnc;
                }
                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_LAC_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refLAC = pl_lsimup_MobileInfo->v_Lac;
                }
                if( pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_CID_FIELD )
                {
                    vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.refCI = pl_lsimup_MobileInfo->v_CellIdentity;
                }
                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_TA_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.tA = pl_lsimup_MobileInfo->s_TA;
                }
                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_NMR_FIELD)
                {
                    /* get the new NMR list */
                    vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.NMRCount =
                    CGPS4_08GetNMRList( &( vg_CGPS_Supl_Location_Id.of_type.gsmCellInfo.p_NMRList ),
                    pl_lsimup_MobileInfo->p_NMR,
                    pl_lsimup_MobileInfo->v_NeighbouringCells);

                    /* free chain list memory */
                    if (pl_lsimup_MobileInfo->v_NeighbouringCells > 0)
                    {
                        t_lsimup_NMR * pl_NMR;
                        t_lsimup_NMR * pl_NextNMR;
                        uint32_t vl_i;

                        for ( vl_i = 0, pl_NMR = pl_lsimup_MobileInfo->p_NMR;
                                        vl_i < pl_lsimup_MobileInfo->v_NeighbouringCells;
                              vl_i++)
                        {
                            pl_NextNMR = pl_NMR->p_NextNeighbour;
                            MC_RTK_FREE_MEMORY(pl_NMR);
                            pl_NMR = pl_NextNMR;
                        }
                    }
                }
            }
            break;

            case K_LSIMUP_WCDMA_CELL:
            case K_LSIMUP_TDSCDMA_CELL:
            {
                memset( &vg_CGPS_Supl_Location_Id, 0, sizeof(vg_CGPS_Supl_Location_Id) );
                vg_CGPS_Supl_Location_Id.Status = CIS_current;

                vg_CGPS_Supl_Location_Id.Type = CIT_wcdmaCell;

                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MCC_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refMCC = pl_lsimup_MobileInfo->v_Mcc;
                }
                if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_MNC_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refMNC = pl_lsimup_MobileInfo->v_Mnc;
                }

                if ( pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_CID_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refUC = 0;
                    if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_RNC_FIELD)
                    {
                        MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: RNCId : 0x%x",pl_lsimup_MobileInfo->v_RNCId));
                        vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refUC = ((uint32_t) pl_lsimup_MobileInfo->v_RNCId) << 16 ;
                    }
                    MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo : cellID : 0x%x",pl_lsimup_MobileInfo->v_CellIdentity));
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.refUC |= pl_lsimup_MobileInfo->v_CellIdentity;
                }

                vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.p_FrequencyInfo = NULL;

                if( pl_lsimup_MobileInfo->s_Primary_Scrambling_Code > 511 )
                {
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.primaryScramblingCode = -1;
                }
                else
                {
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.primaryScramblingCode = (int16_t)pl_lsimup_MobileInfo->s_Primary_Scrambling_Code;
                }

                if( pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_NMR_FIELD)
                {
                    vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.MeasuredResultCount =
                    CGPS4_09GetMeasuredResultsList( &( vg_CGPS_Supl_Location_Id.of_type.wcdmaCellInfo.p_MeasuredResultsList ),
                    pl_lsimup_MobileInfo->p_MeasuredResultsList,
                    pl_lsimup_MobileInfo->v_NeighbouringCells,
                    pl_lsimup_MobileInfo->v_Celltype);

                    if (pl_lsimup_MobileInfo->v_NeighbouringCells > 0)
                    {
                        t_lsimup_MeasuredResultsList * pl_MRL;
                        t_lsimup_MeasuredResultsList * pl_NextMRL;
                        uint32_t vl_i;

                        for ( vl_i = 0, pl_MRL = pl_lsimup_MobileInfo->p_MeasuredResultsList;
                              vl_i < pl_lsimup_MobileInfo->v_NeighbouringCells;
                              vl_i++)
                        {
                            t_lsimup_cellMeasuredResultsList * pl_CellList;
                            t_lsimup_cellMeasuredResultsList * pl_NextCellList;
                            uint32_t vl_j;

                            for ( vl_j = 0, pl_CellList = pl_MRL->p_cellMeasuredResultsList;
                                  vl_j < pl_MRL->v_NumberOfCellsInUARFCN;
                                  vl_j++)
                            {
                                pl_NextCellList = pl_CellList->p_NextCellMeasuredResultsList;
                                MC_RTK_FREE_MEMORY(pl_CellList);
                                pl_CellList = pl_NextCellList;
                            }

                            pl_NextMRL = pl_MRL->p_NextMeasuredResultsList;
                            MC_RTK_FREE_MEMORY(pl_MRL);
                            pl_MRL = pl_NextMRL;
                        }
                    }
                }
            }
            break;
        }


        if (pl_lsimup_MobileInfo->v_ValidityField & K_LSIMUP_IMSI_FIELD)
        {
            int8_t vl_Temp[128];
            int8_t vl_Eslp[128];
            /* +LMSqc20169 */
            uint16_t  vl_Mcc;

            memset(vl_Temp, 0, 128);
            memset(vl_Eslp, 0, 128);

            /* Copy the MCC field into temporary area */
            strncpy((char*)vl_Temp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI, 3);

            /* Extract the MCC as a number */
            vl_Mcc = atoi( (const char*) vl_Temp );
            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: MCC : %u" , vl_Mcc));

            if( 2 == CGPS5_29GetLengthOfMnc(vl_Mcc) )
            {
                strncpy((char*)vl_Temp, "h-slp.mnc0", strlen("h-slp.mnc0"));
                strncat((char*)vl_Temp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI+3, 2);
                strncpy((char*)vl_Eslp, "e-slp.mnc0", strlen("e-slp.mnc0"));
                strncat((char*)vl_Eslp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI+3, 2);

            }
            else
            {
                strncpy((char*)vl_Temp, "h-slp.mnc", strlen("h-slp.mnc"));
                strncat((char*)vl_Temp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI+3, 3);
                strncpy((char*)vl_Eslp, "e-slp.mnc", strlen("e-slp.mnc"));
                strncat((char*)vl_Eslp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI+3, 3);

            }
            /* -LMSqc20169 */
            strncat((char*)vl_Temp, ".mcc", 4);
            strncat((char*)vl_Temp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI, 3);
            strncat((char*)vl_Temp, ".pub.3gppnetwork.org", strlen(".pub.3gppnetwork.org")+1);

            strncat((char*)vl_Eslp, ".mcc", 4);
            strncat((char*)vl_Eslp, (char *)pl_lsimup_MobileInfo->s_IMSI.a_IMSI, 3);
            strncat((char*)vl_Eslp, ".pub.3gppnetwork.org", strlen(".pub.3gppnetwork.org")+1);

            if(vg_CGPS_Supl_AutoSlp_Config.v_Addr == NULL)
            {
                vg_CGPS_Supl_AutoSlp_Config.v_Addr = (int8_t*)MC_RTK_GET_MEMORY(sizeof(char) * strlen((const char*)vl_Temp) + 1 );
            }

            memcpy( vg_CGPS_Supl_AutoSlp_Config.v_Addr, vl_Temp, strlen((const char*)vl_Temp));
            vg_CGPS_Supl_AutoSlp_Config.v_Addr[strlen((const char*)vl_Temp)] = '\0';

            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: SUPL address = %s, Length = %d, Old length = %d ", vg_CGPS_Supl_AutoSlp_Config.v_Addr, strlen((const char*)vl_Temp), vg_CGPS_Supl_AutoSlp_Config.v_AddrLen));
            vg_CGPS_Supl_AutoSlp_Config.v_AddrLen = strlen( (const char*)vl_Temp );

            if(vg_CGPS_Supl_AutoEslp_Config.v_Addr  == NULL)
            {
                vg_CGPS_Supl_AutoEslp_Config.v_Addr  = (int8_t *)MC_RTK_GET_MEMORY(sizeof(char) * strlen((const char*)vl_Eslp) + 1 );
            }

            memcpy( vg_CGPS_Supl_AutoEslp_Config.v_Addr , vl_Eslp, strlen((const char*)vl_Eslp));
            vg_CGPS_Supl_AutoEslp_Config.v_Addr [strlen((const char*)vl_Eslp)] = '\0';

            vg_CGPS_Supl_AutoEslp_Config.v_AddrLen = strlen( (const char*)vl_Eslp );

            MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo: Eslp address = %s, Length = %d, Old length = %d ", vg_CGPS_Supl_AutoEslp_Config.v_Addr , strlen((const char*)vl_Eslp), vg_CGPS_Supl_AutoEslp_Config.v_AddrLen));

            /*UPdate SLP address from Auto configured one only if SLP address was not retrieved from elsewhere OR
            SLP address is not deleted*/
            
            if((vg_CGPS_Supl_Eslp_Config.v_AddrLen == 0 || vg_CGPS_Supl_Eslp_Config.v_Addr == NULL))
            {
                vg_CGPS_Supl_Eslp_Config.v_Addr = (int8_t*)MC_RTK_GET_MEMORY( vg_CGPS_Supl_AutoEslp_Config.v_AddrLen + 1 );
                memcpy( vg_CGPS_Supl_Eslp_Config.v_Addr, vg_CGPS_Supl_AutoEslp_Config.v_Addr,  vg_CGPS_Supl_AutoEslp_Config.v_AddrLen + 1);
            
                vg_CGPS_Supl_Eslp_Config.v_AddrType = K_CGPS_SLPADDR_FQDN;
                vg_CGPS_Supl_Eslp_Config.v_AddrLen  = vg_CGPS_Supl_AutoEslp_Config.v_AddrLen ;
            }
            if((vg_CGPS_Supl_Slp_Config.v_AddrLen == 0 || vg_CGPS_Supl_Slp_Config.v_Addr == NULL))
            {
                vg_CGPS_Supl_Slp_Config.v_Addr = (int8_t*)MC_RTK_GET_MEMORY( vg_CGPS_Supl_AutoSlp_Config.v_AddrLen + 1 );
                memcpy( vg_CGPS_Supl_Slp_Config.v_Addr, vg_CGPS_Supl_AutoSlp_Config.v_Addr,  vg_CGPS_Supl_AutoSlp_Config.v_AddrLen + 1);

                vg_CGPS_Supl_Slp_Config.v_AddrType = K_CGPS_SLPADDR_FQDN;
                vg_CGPS_Supl_Slp_Config.v_AddrLen  = vg_CGPS_Supl_AutoSlp_Config.v_AddrLen ;

                CGPS5_21SuplConfig();
            }
        }

        if (vl_newIMSI || vl_newISDN)
        {
            /* we receive new information about mobile, update the SUPL lib*/
            /* +LMSqc05144 */
            /* CGPS5_10InitSuplLibrary(); */
            CGPS5_21SuplConfig();
            /* -LMSqc05144 */
        }
    }

    vl_ret = GN_SUPL_Mobile_Info_Ind_In( &vg_CGPS_Supl_Location_Id);
    
    if (vl_ret != FALSE)
    {        
        GN_SUPL_Handler();
        MC_CGPS_TRACE(("CGPS5_05AdpMobileInfo : GN_SUPL_Mobile_Info_Ind_In "));
    }

    if (vl_OpType == CGPS_LSIMUP_MOBILE_INFO_IND)
    {
        t_lsimup_MobileInfoRspM * pl_lsimup_MobileInfoRsp=NULL;

        pl_lsimup_MobileInfoRsp = (t_lsimup_MobileInfoRspM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_MobileInfoRsp));
        pl_lsimup_MobileInfoRsp->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMUP, 0,
                                   CGPS_LSIMUP_MOBILE_INFO_RSP,
                                   (t_MsgHeader *) pl_lsimup_MobileInfoRsp );
    }

#ifdef AGPS_HISTORICAL_DATA_FTR
    CGPS10_07CheckCurrentCellStatus();
#endif /*AGPS_HISTORICAL_DATA_FTR*/

    MC_CGPS_TRACE(("End : CGPS5_05AdpMobileInfo"));
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/******************************************************************************/
/* CGPS5_06CloseBearerCnf : Bearer closure confirmation                       */
/******************************************************************************/
const t_OperationDescriptor*  CGPS5_06CloseBearerCnf(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;

    p_FsmObject = p_FsmObject;

    MC_CGPS_TRACE(("CGPS5_06CloseBearerCnf"));
/*+LMSqc38060*/
    vg_CGPS_Supl_Timeout = 0xFFFFFFFF;
/*-LMSqc38060*/

/* ++ LMSqb95626    ANP 03/07/2009 */
    /* ++ LMSqb93693 */
    /* notify the user if it's requested */
    if(vg_DataCntNotificationCallback != NULL)
        vg_DataCntNotificationCallback(FALSE);
    /* -- LMSqb93693 */
/* -- LMSqb95626    ANP 03/07/2009 */
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/******************************************************************************/
/* CGPS5_07CloseBearerInd : Bearer closure indication                         */
/******************************************************************************/
const t_OperationDescriptor*  CGPS5_07CloseBearerInd(t_RtkObject* p_FsmObject)
{
    t_lsimup_CloseBearerRspM * pl_lsimup_CloseBearerRsp=NULL;
    const t_OperationDescriptor* pl_NextState = SAME;

    p_FsmObject = p_FsmObject;

    pl_lsimup_CloseBearerRsp = (t_lsimup_CloseBearerRspM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_CloseBearerRsp));
    pl_lsimup_CloseBearerRsp->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

    MC_CGPS_TRACE(("CGPS5_07CloseBearerInd"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMUP, 0,
                               CGPS_LSIMUP_CLOSE_BEARER_RSP,
                               (t_MsgHeader *)pl_lsimup_CloseBearerRsp);

    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;

/* ++ LMSqb95626    ANP 03/07/2009 */
    /* ++ LMSqb93693 */
    /* notify the user if it's requested */
    if(vg_DataCntNotificationCallback != NULL)
    {
        vg_DataCntNotificationCallback(FALSE);
    }
    /* -- LMSqb93693 */
/* -- LMSqb95626    ANP 03/07/2009 */
    return pl_NextState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/******************************************************************************/
/* CGPS5_08ReceiveDataInd : Receive data indication                           */
/******************************************************************************/
const t_OperationDescriptor*  CGPS5_08ReceiveDataInd(t_RtkObject* p_FsmObject)
{
    int8_t vl_index = 0;
    uint8_t vl_receiveFailed= FALSE;
    e_GN_Status vl_Status = GN_SUCCESS;
    const t_OperationDescriptor* pl_NextState = SAME;

    t_lsimup_ReceiveDataIndM * pl_lsimup_ReceiveDataInd = (t_lsimup_ReceiveDataIndM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    /* search for the corresponding context */
    for (vl_index = 0;
              vl_index < K_CGPS_MAX_NUMBER_OF_SUPL
           && s_CgpsSupl[vl_index].v_ConnectionHandle != pl_lsimup_ReceiveDataInd->v_ConnectionHandle;
                    vl_index++
        );

    MC_CGPS_TRACE(("CGPS5_08ReceiveDataInd : start"));
    /* if the connection hasn't been found */
    if (vl_index == K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        MC_CGPS_TRACE(("CGPS5_08ReceiveDataInd : handle not found"));
        vl_receiveFailed = TRUE;
    }
    else
    {
        if (!GN_SUPL_PDU_Delivery_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status, (uint16_t)pl_lsimup_ReceiveDataInd->v_DataLength, (uint8_t*)pl_lsimup_ReceiveDataInd->v_DataPointer))
        {
            MC_CGPS_TRACE(("CGPS5_08ReceiveDataInd : packet rejected by library"));
            vl_receiveFailed = TRUE;
        }
        else
        {
            t_lsimup_ReceiveDataRspM * pl_lsimup_ReceiveDataRsp=NULL;
            pl_lsimup_ReceiveDataRsp = (t_lsimup_ReceiveDataRspM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_ReceiveDataRsp));
            pl_lsimup_ReceiveDataRsp->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
            pl_lsimup_ReceiveDataRsp->v_ConnectionHandle = pl_lsimup_ReceiveDataInd->v_ConnectionHandle;

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                       PROCESS_LSIMUP, 0,
                                       CGPS_LSIMUP_RECEIVE_DATA_RSP,
                                       (t_MsgHeader *)pl_lsimup_ReceiveDataRsp);
            MC_CGPS_TRACE(("CGPS5_08ReceiveDataInd : packet accepted"));
            GN_SUPL_Handler();
        }
    }

    if (vl_receiveFailed)
    {
        t_lsimup_ReceiveDataRjtM * pl_lsimup_ReceiveDataRjt=NULL;
        pl_lsimup_ReceiveDataRjt = (t_lsimup_ReceiveDataRjtM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_ReceiveDataRjt));
        pl_lsimup_ReceiveDataRjt->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
        pl_lsimup_ReceiveDataRjt->v_ConnectionHandle = pl_lsimup_ReceiveDataInd->v_ConnectionHandle;
        pl_lsimup_ReceiveDataRjt->v_RejectionReason = K_LSIMUP_REJECTION_REASON_RECEIVE_DATA_FAIL;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMUP, 0,
                                   CGPS_LSIMUP_RECEIVE_DATA_RJT,
                                   (t_MsgHeader *)pl_lsimup_ReceiveDataRjt);
    }

    if (pl_lsimup_ReceiveDataInd->v_DataPointer != NULL)
    {
        MC_RTK_FREE_MEMORY(pl_lsimup_ReceiveDataInd->v_DataPointer);
        pl_lsimup_ReceiveDataInd->v_DataPointer = NULL ;
    }

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/******************************************************************************/
/* CGPS5_09TcpIpConnectErr : TCPIP connection establishment error             */
/******************************************************************************/
const t_OperationDescriptor*  CGPS5_09TcpIpConnectErr(t_RtkObject* p_FsmObject)
{
    int8_t vl_index = vg_CGPS_Supl_Pending_Connection;
    t_lsimup_TcpIpConnectErrM * pl_lsimup_TcpIpConnectErr = (t_lsimup_TcpIpConnectErrM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    const t_OperationDescriptor* pl_NextState = SAME;

    e_GN_Status vl_Status = pl_lsimup_TcpIpConnectErr->v_ErrorType==K_LSIMUP_ERROR_TYPE_TIME_OUT?
                            GN_ERR_CONN_TIMEOUT:GN_ERR_CONN_REJECTED;
/*+LMSqc38060*/
   vg_CGPS_Supl_Timeout = 0xFFFFFFFF;
/*-LMSqc38060*/

    MC_CGPS_TRACE(("CGPS5_09TcpIpConnectErr error 0x%X",pl_lsimup_TcpIpConnectErr->v_ErrorType));
    /* Update the pending Request */
    if (-1 != vl_index)
    {
        if(    s_CgpsSupl[vl_index].v_IsRegistered     == TRUE
            && s_CgpsSupl[vl_index].v_ConnectionHandle == 0 )
        {
#ifdef CMCC_LOGGING_ENABLE
            CGPS9_23LogPosition (vl_index, CGPS_SUPL_CMCC_SERVERCONNECTION_FAILURE_CODE, NULL);
#endif

                GN_SUPL_Connect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status, NULL);

                GN_SUPL_Handler();

                CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
    /*+SYScs46505*/
                CGPS4_35UpdateNumSuplFailure(&vg_AssistTrack);
    /*-SYScs46505*/

    /* +LMSqc18414  */
                vg_CGPS_Supl_Pending_Connection = -1;

                /* If connection has failed due to handshake failure, handle it */
                if( pl_lsimup_TcpIpConnectErr->v_ErrorType == K_LSIMUP_ERROR_TLS_HANDSHAKE_FAILED)
                {
                    CGPS5_28HandleAuthenticationFailure();
                }
    /* -LMSqc18414  */
        }
    }

    vg_CGPS_Supl_Pending_Connection = -1;

    /* search for another pending connection request */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && (-1 == vg_CGPS_Supl_Pending_Connection); vl_index++)
    {
        if (   (s_CgpsSupl[vl_index].v_IsRegistered     == TRUE)
            && (s_CgpsSupl[vl_index].v_ConnectionHandle == 0 )
            && (NULL != s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address)
            && (s_CgpsSupl[vl_index].v_Suspended        == CGPS_SESSION_NOT_SUSPENDED)
          )
        {
            CGPS5_25EstablishTcpIpSession( vl_index );

            CGPS5_26LsimupMobileInfoReq();
            break;
        }
    }

    if ( -1 == vg_CGPS_Supl_Pending_Connection )
    {
        if(    (vg_CGPS_Supl_Count == 0)
            && (vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHED))
        {
            t_lsimup_CloseBearerReqM * pl_lsimup_CloseBearerReq=NULL;
            pl_lsimup_CloseBearerReq = (t_lsimup_CloseBearerReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_CloseBearerReq));
            pl_lsimup_CloseBearerReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

            CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                       PROCESS_LSIMUP,
                                       CGPS_LSIMUP_CLOSE_BEARER_REQ,
                                       (t_MsgHeader *)pl_lsimup_CloseBearerReq);

            vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;

            MC_CGPS_TRACE(("CGPS5_14TcpIpConnectErr : No more connection, CGPS_LSIMUP_CLOSE_BEARER_REQ sent"));
        }
    }

    if (pl_lsimup_TcpIpConnectErr->s_IPAddress != NULL)
    {
        MC_RTK_FREE_MEMORY(pl_lsimup_TcpIpConnectErr->s_IPAddress);
    }

    CGPS0_52ConfigureCgpsNextState();
    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/******************************************************************************/
/* CGPS5_10InitSuplLibrary : Initialise the libary                            */
/******************************************************************************/
bool CGPS5_10InitSuplLibrary(void)
{
/* +LMSqc03064 */
#ifdef CGPS_ENABLE_CONFIG_FROM_FILE
    t_gfl_FILE * pl_File;
    char* pl_Port;
    char* pl_Dummy;
    char* pl_FQDN;

    /*open the bearer configuration file */
    pl_File=(t_gfl_FILE *)MC_GFL_FOPEN(CGPS_SUPL_SERVER_FILE_NAME,(const uint8_t*)"r");
    if (pl_File)
    {
        /* do some dummy read to be at the right position for IP and port reading !*/
        CGPS4_06ReadString(&pl_Dummy, pl_File);
        if (pl_Dummy)MC_RTK_FREE_MEMORY(pl_Dummy);

        CGPS4_06ReadString(&pl_Dummy, pl_File);
        if (pl_Dummy)MC_RTK_FREE_MEMORY(pl_Dummy);

        CGPS4_06ReadString(&pl_Dummy, pl_File);
        if (pl_Dummy)MC_RTK_FREE_MEMORY(pl_Dummy);

        /* read TCP/IP address */
        if (vg_CGPS_Supl_Slp_Config.v_Addr)
        {
            MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Slp_Config.v_Addr);
            vg_CGPS_Supl_Slp_Config.v_Addr = NULL;
        }

        CGPS4_06ReadString(&vg_CGPS_Supl_Slp_Config.v_Addr, pl_File);
        vg_CGPS_Supl_Slp_Config.v_AddrType = K_CGPS_SLPADDR_IPV4;
        vg_CGPS_Supl_Slp_Config.v_AddrLen = strlen( vg_CGPS_Supl_Slp_Config.v_Addr );

        /* read port number from file*/
        CGPS4_06ReadString(&pl_Port, pl_File);
        if (pl_Port)
        {
            vg_CGPS_Supl_Slp_Config.v_PortNum = atol(pl_Port);
            MC_RTK_FREE_MEMORY(pl_Port);
        }

        CGPS4_06ReadString(&pl_FQDN,pl_File);
        if (pl_FQDN)
        {
            vg_CGPS_Supl_Slp_Config.v_AddrType = K_CGPS_SLPADDR_FQDN;
            if (vg_CGPS_Supl_Slp_Config.v_Addr)MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Slp_Config.v_Addr );
            vg_CGPS_Supl_Slp_Config.v_Addr = pl_FQDN;
            vg_CGPS_Supl_Slp_Config.v_AddrLen = strlen( vg_CGPS_Supl_Slp_Config.v_Addr );
        }
        if( vg_CGPS_Supl_Slp_Config.v_Addr != NULL )
        {
            if( vg_CGPS_Supl_Eslp_Config.v_Addr )
            {
                MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Eslp_Config.v_Addr);
            }
            vg_CGPS_Supl_Eslp_Config.v_Addr = (char *)MC_RTK_GET_MEMORY( vg_CGPS_Supl_Slp_Config.v_AddrLen + 1 );
            memcpy( vg_CGPS_Supl_Eslp_Config.v_Addr, vg_CGPS_Supl_Slp_Config.v_Addr,  vg_CGPS_Supl_Slp_Config.v_AddrLen + 1);
        }
        vg_CGPS_Supl_Eslp_Config.v_AddrType = vg_CGPS_Supl_Slp_Config.v_AddrType;
        vg_CGPS_Supl_Eslp_Config.v_AddrLen = vg_CGPS_Supl_Slp_Config.v_AddrLen ;
        /* Close file*/
        MC_GFL_FCLOSE(pl_File);
    }
#endif /* #ifdef CGPS_ENABLE_CONFIG_FROM_FILE */
/* -LMSqc03064 */

    CGPS5_21SuplConfig();

    return TRUE;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/******************************************************************************/
/* CGPS5_11TcpIpDisconnectCnf : reset SUPL appli context                      */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_11TcpIpDisconnectCnf(t_RtkObject* p_FsmObject)
{
    int8_t vl_index;
    t_lsimup_TcpIpDisconnectCnfM * pl_lsimup_TcpIpDisconnectCnf = (t_lsimup_TcpIpDisconnectCnfM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    e_GN_Status vl_Status = GN_SUCCESS;
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS5_11TcpIpDisconnectCnf"));
    /* search for the context corresponding with the connection handle */
    for (vl_index = 0;
            vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_ConnectionHandle != pl_lsimup_TcpIpDisconnectCnf->v_ConnectionHandle;
            vl_index++
        );

    if (vl_index < K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        /* notify the SUPL library */
        GN_SUPL_Disconnect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status);

        CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
    }

    /* if there's no more SUPL applications, CGPS close the bearer */
    if ((vg_CGPS_Supl_Count == 0) && (vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHED))
    {
        t_lsimup_CloseBearerReqM * pl_lsimup_CloseBearerReq=NULL;
        pl_lsimup_CloseBearerReq = (t_lsimup_CloseBearerReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_CloseBearerReq));
        pl_lsimup_CloseBearerReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

        CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                   PROCESS_LSIMUP,
                                   CGPS_LSIMUP_CLOSE_BEARER_REQ,
                                   (t_MsgHeader *)pl_lsimup_CloseBearerReq);

        vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;
    }

    return pl_NextState;

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/******************************************************************************/
/* CGPS5_12TcpIpDisconnectInd : reset SUPL appli context                      */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_12TcpIpDisconnectInd(t_RtkObject* p_FsmObject)
{
    int8_t vl_index;
    t_lsimup_TcpIpDisconnectIndM * pl_lsimup_TcpIpDisconnectInd = (t_lsimup_TcpIpDisconnectIndM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    e_GN_Status vl_Status = GN_ERR_CONN_SHUTDOWN;
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS5_12TcpIpDisconnectInd"));

    /* search for the context corresponding with the connection handle */
    for (vl_index = 0;
            vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_ConnectionHandle != pl_lsimup_TcpIpDisconnectInd->v_ConnectionHandle;
            vl_index++
        );

    /* if the context has been found */
    if (vl_index < K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        t_lsimup_TcpIpDisconnectRspM * pl_lsimup_TcpIpDisconnectRsp=NULL;

        /* notify the SUPL library */
        GN_SUPL_Disconnect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status);

        /* ++ LMSQC11892 */
        GN_SUPL_Handler();
        /* +- LMSQC11892 */

        CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);

        pl_lsimup_TcpIpDisconnectRsp = (t_lsimup_TcpIpDisconnectRspM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_TcpIpDisconnectRsp));
        pl_lsimup_TcpIpDisconnectRsp->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
        pl_lsimup_TcpIpDisconnectRsp->v_ConnectionHandle = pl_lsimup_TcpIpDisconnectInd->v_ConnectionHandle;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMUP, 0,
                                   CGPS_LSIMUP_TCPIP_DISCONNECT_RSP,
                                   (t_MsgHeader *)pl_lsimup_TcpIpDisconnectRsp);

    }
    else /* send negative response to LSIMUP */
    {
        t_lsimup_TcpIpDisconnectRjtM * pl_lsimup_TcpIpDisconnectRjt=NULL;

        pl_lsimup_TcpIpDisconnectRjt = (t_lsimup_TcpIpDisconnectRjtM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_TcpIpDisconnectRjt));
        pl_lsimup_TcpIpDisconnectRjt->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
        pl_lsimup_TcpIpDisconnectRjt->v_ConnectionHandle = pl_lsimup_TcpIpDisconnectInd->v_ConnectionHandle;
        pl_lsimup_TcpIpDisconnectRjt->v_RejectionReason = K_LSIMUP_REJECTION_REASON_TCPIP_DISCONNECT_FAIL;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                   PROCESS_LSIMUP, 0,
                                   CGPS_LSIMUP_TCPIP_DISCONNECT_RJT,
                                   (t_MsgHeader *)pl_lsimup_TcpIpDisconnectRjt);
    }

    /* if there's no more SUPL applications, CGPS close the bearer */
    if ((vg_CGPS_Supl_Count == 0) && (vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHED))
    {
        t_lsimup_CloseBearerReqM * pl_lsimup_CloseBearerReq=NULL;
        pl_lsimup_CloseBearerReq = (t_lsimup_CloseBearerReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_CloseBearerReq));
        pl_lsimup_CloseBearerReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

        CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                   PROCESS_LSIMUP,
                                   CGPS_LSIMUP_CLOSE_BEARER_REQ,
                                   (t_MsgHeader *)pl_lsimup_CloseBearerReq);

        vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;
    }


    CGPS0_52ConfigureCgpsNextState();

    return pl_NextState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/******************************************************************************/
/* CGPS5_13StartSUPL : reset SUPL appli context                               */
/******************************************************************************/
bool CGPS5_13StartSUPL( t_CgpsNaf* pp_Handle, t_cgps_ThirdPartyInfo *third_party_info )
{
    e_GN_Status vl_Status;
    void * pl_NewHandle;
    int8_t vl_index = 0;
    s_GN_SUPL_QoP v_QoP , *p_QoP = NULL ;

    uint8_t result = FALSE;


    MC_CGPS_TRACE(("CGPS5_13StartSUPL : Start"));

    if (  (   MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_SESSION_MO_ALLOWED)
           && MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE ) )
       )
    {
       /* search for the first available handler */
        for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_IsRegistered == TRUE; vl_index++);

        if (vl_index != K_CGPS_MAX_NUMBER_OF_SUPL)
        {
            if ( (NULL != pp_Handle) && (pp_Handle->v_Mode != K_CGPS_PERIODIC))
            {
                p_QoP = &v_QoP;

                CGPS5_20ConvertQoP( pp_Handle , p_QoP );
            }

            /* + LMSqc32881 */
            if( NULL == p_QoP )
            {
                /* There is no exisitng QoP */
                v_QoP.delay     = CGPS_SUPL_DEFAULT_DELAY;
                v_QoP.veracc    = CGPS_SUPL_DEFAULT_VER_ACC;
                v_QoP.horacc    = CGPS_SUPL_DEFAULT_HOR_ACC;
                v_QoP.maxLocAge = CGPS_SUPL_DEFAULT_MAX_LOC_AGE;

                p_QoP = &v_QoP;
            }
            /* - LMSqc32881 */
            result = GN_SUPL_Position_Req_In(&pl_NewHandle, &vl_Status, p_QoP);
            if (result != FALSE)
            {
                /* set a new SUPL application context */
                s_CgpsSupl[vl_index].v_IsRegistered = TRUE;
                s_CgpsSupl[vl_index].v_GPSHandle = pl_NewHandle;
                s_CgpsSupl[vl_index].v_bearer = &vg_CGPS_SUPL_Bearer;
                if (CGPS4_18CheckNewSessionRight(CGPS_SUPL_APPLICATION_ID))
                {
                    CGPS4_19UpdateOtherApplicationState(CGPS_SUPL_APPLICATION_ID);
                    s_CgpsSupl[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
                }
                else
                {
                    s_CgpsSupl[vl_index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
                }
                vg_CGPS_Supl_Count++;

                GN_SUPL_Handler();

                if (MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPLV2_SUPPORTED))
                {
                    if(third_party_info != NULL)
                    {
                        s_GN_SUPL_V2_ThirdPartyInfo  supl_third_party_info;
                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : GN_SUPL_Start_ThirdParty_Location_Transfer called \n"));

                        supl_third_party_info.thirdPartyId     =  third_party_info->v_ThirdPartyId;
                        supl_third_party_info.thirdPartyIdName =  (char*)third_party_info->a_ThirdPartyName;

                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : Third Part ID Info ID \t %d \n",   supl_third_party_info.thirdPartyId));
                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : Third Part ID Info Name \t %s \n", supl_third_party_info.thirdPartyIdName));
                        result = GN_SUPL_Start_ThirdParty_Location_Transfer(pl_NewHandle, &supl_third_party_info,&vl_Status);
                    }
                    if(pp_Handle != NULL)
                    {
                        if((pp_Handle->v_ApplicationIDInfo.v_ApplicationIDInfo_present))
                        {
                            /*Application Id info is present so try t populate on to the supl imnstance*/
                            s_GN_SUPL_V2_ApplicationInfo v_supl_applicationIdInfo;
                            v_supl_applicationIdInfo.applicationIDInfoPresence = pp_Handle->v_ApplicationIDInfo.v_ApplicationIDInfo_present;
                            MC_CGPS_TRACE(("CGPS5_13StartSUPL : Application ID Info resent \t %d \n",v_supl_applicationIdInfo.applicationIDInfoPresence));
                            if(pp_Handle->v_ApplicationIDInfo.a_ApplicationProvider != NULL)
                            {
                                v_supl_applicationIdInfo.applicationProvider = (char*)pp_Handle->v_ApplicationIDInfo.a_ApplicationProvider;
                                MC_CGPS_TRACE(("CGPS5_13StartSUPL : Application ID Application Provider \t %s \n",v_supl_applicationIdInfo.applicationProvider));
                            }

                            if(pp_Handle->v_ApplicationIDInfo.a_ApplicationName != NULL)
                            {
                                v_supl_applicationIdInfo.appName = (char*)pp_Handle->v_ApplicationIDInfo.a_ApplicationName;
                                MC_CGPS_TRACE(("CGPS5_13StartSUPL : Application ID Application Name \t %s \n",v_supl_applicationIdInfo.appName));
                            }
                            if(pp_Handle->v_ApplicationIDInfo.a_ApplicationVersion != NULL)
                            {
                                v_supl_applicationIdInfo.appVersion = (char*)pp_Handle->v_ApplicationIDInfo.a_ApplicationVersion;
                                MC_CGPS_TRACE(("CGPS5_13StartSUPL : Application ID Application Version \t %s \n",v_supl_applicationIdInfo.appVersion));
                            }
                            result = GN_SUPL_ApplicationID_Info_In(pl_NewHandle,&v_supl_applicationIdInfo,&vl_Status);
                        }

                        if(    (pp_Handle->v_Mode          == K_CGPS_PERIODIC)
                            || (pp_Handle->v_AreaEventType != K_CGPS_AREA_EVENT_NOT_SET))
                        {
                            s_GN_SUPL_V2_Triggers vl_TriggerInfo;
                            e_GN_Status           vl_Status     = GN_SUCCESS;

                        /* Periodic case */
                            if( pp_Handle->v_AreaEventType == K_CGPS_AREA_EVENT_NOT_SET )
                            {
                                s_CgpsSupl[vl_index].v_TriggeredEventType                    = K_CGPS_TRIGGER_EVENTTYPE_PERIODIC;
                                s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_NumFixes    = 100; //Current solution doesnot provide number of fix.
                                s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_IntervalFix = pp_Handle->v_FixRate/1000;
                                s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_StartTime   = GN_GPS_Get_OS_Time_ms();

                                vl_TriggerInfo.triggeredMode                          = GN_TriggeredModePeriodic;
                                vl_TriggerInfo.periodicTrigger.NumberOfFixes          = s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_NumFixes;
                                vl_TriggerInfo.periodicTrigger.IntervalBetweenFixes   = s_CgpsSupl[vl_index].v_PeriodicTriggeredParams.v_IntervalFix;
                                /* CGPS representation of Start Time is in terms of OS time, while SUPL is in terms of relative seconds, As its an SI session and session start is now, Start Time in relative context is set to 0 */
                                vl_TriggerInfo.periodicTrigger.StartTime              = 0;
                            }
                            /* Area Event triggered */
                            else
                            {
                                /* TODO - v_MaximumReports*/
                                t_cgps_GeographicTargetArea* pl_GeographicTargetArea;

                                s_CgpsSupl[vl_index].v_TriggeredEventType                        = K_CGPS_TRIGGER_EVENTTYPE_AREA;
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaEventType    = pp_Handle->v_AreaEventType; //Current solution doesnot provide number of fix.
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_MaximumReports   = 100; //Current solution doesnot provide number of fix.
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_MinIntervalTime  = pp_Handle->v_FixRate/1000;
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_StartTime        = pp_Handle->v_StartTime;
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_StopTime         = pp_Handle->v_StopTime;
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaDefinition.v_NumGeoArea     = 1;
                                s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists = 0;

                                pl_GeographicTargetArea = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaDefinition.a_GeographicTargetArea;

                                memcpy( pl_GeographicTargetArea, pp_Handle->p_TargetArea, sizeof( *pl_GeographicTargetArea ));

                                vl_TriggerInfo.triggeredMode                                = GN_TriggeredModeAreaEvent;
                                vl_TriggerInfo.areaEventTrigger.areaEventType               = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaEventType;
                                vl_TriggerInfo.areaEventTrigger.locationEstimateRequested   = FALSE;

                                vl_TriggerInfo.areaEventTrigger.repeatedReportingParams.maximumNumberOfReports  = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_MaximumReports;
                                vl_TriggerInfo.areaEventTrigger.repeatedReportingParams.minimumIntervalTime     = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_MinIntervalTime ;

                                /* CGPS representation of Start Time is in terms of OS time, while SUPL is in terms of relative seconds, As its an SI session and session start is now, Start Time in relative context is set to 0 */
                                vl_TriggerInfo.areaEventTrigger.startTime                   = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_StartTime;
                                vl_TriggerInfo.areaEventTrigger.stopTime                    = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_StopTime;
                                vl_TriggerInfo.areaEventTrigger.geographicTargetAreaCount   = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaDefinition.v_NumGeoArea;
                                vl_TriggerInfo.areaEventTrigger.AreaIdListsCount            = s_CgpsSupl[vl_index].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists;
                                vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams  = (s_GN_SUPL_V2_GeographicTargetArea*)MC_RTK_GET_MEMORY((sizeof(*vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams)
                                                                                                           *(vl_TriggerInfo.areaEventTrigger.geographicTargetAreaCount)));
                                switch( pl_GeographicTargetArea->v_ShapeType )
                                {
                                    case K_CGPS_CIRCULAR_AREA:
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->shapeType = GN_GeographicalTargetArea_Circlar;

                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.center.latitude     = pl_GeographicTargetArea->v_Latitude[0];
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.center.longitude    = pl_GeographicTargetArea->v_Longitude[0];
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.center.latitudeSign = pl_GeographicTargetArea->v_LatSign[0];
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.radius              = pl_GeographicTargetArea->v_CircularRadius;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.radiusMin           = pl_GeographicTargetArea->v_CircularRadiusMin;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.radiusMax           = pl_GeographicTargetArea->v_CircularRadiusMax;

                                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : pl_GeographicTargetArea  radius \t %d \n",vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.radius ));
                                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : pl_GeographicTargetArea  radiusmin \t %d \n",vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.radiusMin ));
                                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : pl_GeographicTargetArea  radiusmax \t %d \n",vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.radiusMax ));
                                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : pl_GeographicTargetArea center.latitude  \t %d \n",vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.center.latitude  ));
                                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : pl_GeographicTargetArea  center.longitude \t %d \n",vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.center.longitude));
                                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : pl_GeographicTargetArea  center.latitudeSign \t %i \n",vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.circularArea.center.latitudeSign ));
                                    break;

                                    case K_CGPS_ELLIPTICAL_AREA:
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->shapeType = GN_GeographicalTargetArea_Ellipse;

                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.center.latitude     = pl_GeographicTargetArea->v_Latitude[0];
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.center.longitude    = pl_GeographicTargetArea->v_Longitude[0];
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.center.latitudeSign = pl_GeographicTargetArea->v_LatSign[0];
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMajor           = pl_GeographicTargetArea->v_EllipSemiMajor;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMinor           = pl_GeographicTargetArea->v_EllipSemiMinor;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMinorAngle      = pl_GeographicTargetArea->v_EllipSemiMinorAngle;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMajorMin        = pl_GeographicTargetArea->v_EllipSemiMajorMin;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMajorMax        = pl_GeographicTargetArea->v_EllipSemiMajorMax;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMinorMax        = pl_GeographicTargetArea->v_EllipSemiMinorMax;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.ellipticalArea.semiMinorMin        = pl_GeographicTargetArea->v_EllipSemiMinorMin;
                                    break;

                                    case K_CGPS_POLYGON_AREA:
                                    {
                                        U1 pointIndex = 0;

                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->shapeType = GN_GeographicalTargetArea_Polygon;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.polygonHysteresis = pl_GeographicTargetArea->v_PolygonHysteresis;

                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.numOfPoints       = pl_GeographicTargetArea->v_NumberOfPoints;
                                        vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.coordinates       = (s_GN_SUPL_V2_Coordinate*)MC_RTK_GET_MEMORY((sizeof(*vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.coordinates)
                                                                                                                                      *(vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.numOfPoints)));
                                        for( pointIndex = 0; pointIndex < vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.numOfPoints ; pointIndex++ )
                                        {
                                            vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.coordinates[pointIndex].latitude     = pl_GeographicTargetArea->v_Latitude[pointIndex];
                                            vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.coordinates[pointIndex].latitudeSign = pl_GeographicTargetArea->v_LatSign[pointIndex];
                                            vl_TriggerInfo.areaEventTrigger.geographicTargetAreaParams->choice.polygonArea.coordinates[pointIndex].longitude    = pl_GeographicTargetArea->v_Longitude[pointIndex];
                                        }
                                    }
                                    break;

                                    default:
                                       MC_CGPS_TRACE(("CGPS5_13StartSUPL : Area event type not set"));
                                    break;
                                }
                            }

                        MC_CGPS_TRACE(("CGPS5_13StartSUPL : Trigger mode = %d", vl_TriggerInfo.triggeredMode));
                        GN_SUPL_TriggerPeriodic_Req_In(pl_NewHandle, &vl_TriggerInfo, &vl_Status);
                        }
                    }
                }

                MC_CGPS_TRACE(("CGPS5_13StartSUPL : GN_SUPL_Position_Req_In successful, GPSHandle=0x%x", pl_NewHandle));
            }
            else
            {
                MC_CGPS_TRACE(("CGPS5_13StartSUPL : GN_SUPL_Position_Req_In failed!!!"));
                return FALSE;
            }
        }
        else
        {
            MC_CGPS_TRACE(("CGPS5_13StartSUPL : K_CGPS_MAX_NUMBER_OF_SUPL reached!!!"));
            return FALSE;
        }
    }
    else
    {
#ifdef CMCC_LOGGING_ENABLE
        vl_index = 0xFF;
        CGPS9_23LogPosition (vl_Index, CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_CODE, NULL);
#endif
        MC_CGPS_TRACE(("CGPS5_13StartSUPL : Didnt start a SUPL session, CGPS config =%x", vg_CGPS_Configuration));
    }

    MC_CGPS_TRACE(("CGPS5_13: SUPL Num=%d I=%d R=%d H=%x S=%d",
                        vg_CGPS_Supl_Count,
                        vl_index,
                        s_CgpsSupl[vl_index].v_IsRegistered,
                        s_CgpsSupl[vl_index].v_GPSHandle,
                        s_CgpsSupl[vl_index].v_Suspended));

    MC_CGPS_TRACE(("CGPS5_13StartSUPL : End"));
    return TRUE;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/******************************************************************************/
/* CGPS5_14TcpIpDisconnectErr : reset SUPL appli context                      */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_14TcpIpDisconnectErr(t_RtkObject* p_FsmObject)
{
    int8_t vl_index;
    t_lsimup_TcpIpDisconnectErrM * pl_lsimup_TcpIpDisconnectErr = (t_lsimup_TcpIpDisconnectErrM *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    e_GN_Status vl_Status = GN_SUCCESS;
    const t_OperationDescriptor* pl_NextState = SAME;

    MC_CGPS_TRACE(("CGPS5_14TcpIpDisconnectErr : ConnectionHandle=0x%x", pl_lsimup_TcpIpDisconnectErr->v_ConnectionHandle));

    /* search for the context corresponding with the connection handle */
    for (vl_index = 0;
            vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_ConnectionHandle != pl_lsimup_TcpIpDisconnectErr->v_ConnectionHandle;
            vl_index++
        );

    if (vl_index < K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        /* notify the SUPL library */
        GN_SUPL_Disconnect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status);

        CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);

        /* if there's no more SUPL applications, CGPS close the bearer */
        if ((vg_CGPS_Supl_Count == 0) && (vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHED))
        {
            t_lsimup_CloseBearerReqM * pl_lsimup_CloseBearerReq=NULL;
            pl_lsimup_CloseBearerReq = (t_lsimup_CloseBearerReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_CloseBearerReq));
            pl_lsimup_CloseBearerReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

            CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                       PROCESS_LSIMUP,
                                       CGPS_LSIMUP_CLOSE_BEARER_REQ,
                                       (t_MsgHeader *)pl_lsimup_CloseBearerReq);

            vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;
            MC_CGPS_TRACE(("CGPS5_14TcpIpDisconnectErr : No more connection, CGPS_LSIMUP_CLOSE_BEARER_REQ sent"));
        }
    }

    CGPS0_52ConfigureCgpsNextState();

    MC_CGPS_TRACE(("CGPS5_14TcpIpDisconnectErr : End"));
    return pl_NextState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
/******************************************************************************/
/* CGPS5_15CloseBearerErr : Bearer closure error                              */
/******************************************************************************/
const t_OperationDescriptor* CGPS5_15CloseBearerErr(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    p_FsmObject = p_FsmObject;
    MC_CGPS_TRACE(("CGPS5_15CloseBearerErr"));
    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;
    return pl_NextState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 16
/******************************************************************************/
/* CGPS5_16SuplFreeMemory : Free all memory allocated by SUPL                 */
/******************************************************************************/
void CGPS5_16SuplFreeMemory(void)
{
    MC_CGPS_TRACE(("CGPS5_16SuplFreeMemory"));
    if (vg_CGPS_SUPL_Bearer.s_APN)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_APN);
        vg_CGPS_SUPL_Bearer.s_APN = NULL;
    }
    if (vg_CGPS_SUPL_Bearer.s_Login)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_Login);
        vg_CGPS_SUPL_Bearer.s_Login = NULL;
    }
    if (vg_CGPS_SUPL_Bearer.s_Password)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_Password);
        vg_CGPS_SUPL_Bearer.s_Password = NULL;
    }
    if (vg_CGPS_Supl_Slp_Config.v_Addr)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_Supl_Slp_Config.v_Addr);
        vg_CGPS_Supl_Slp_Config.v_Addr = NULL;
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 18
/******************************************************************************/
/* CGPS5_18ConnectSUPLSession : try to connect a SUPL session                 */
/******************************************************************************/
void CGPS5_18ConnectSUPLSession(int8_t vp_Index)
{
    /* if the bearer isn't already established */
    if (vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHING)
    {
        MC_CGPS_TRACE(("CGPS5_18ConnectSUPLSession : Bearer currently establishing"));
    }
    else
    {
        if (vg_CGPS_SUPL_Bearer.v_BearerState != K_CGPS_BEARER_ESTABLISHED)
        {
            if (!CGPS5_01EstablishBearerSession())
            {
                MC_CGPS_TRACE(("CGPS5_18ConnectSUPLSession : Bearer establishment request failed!!!"));
            }
            else
            {
                MC_CGPS_TRACE(("CGPS5_18ConnectSUPLSession : Bearer establishment request successful"));
                /* the TCPIP connection(s) will be established when K_LSIMUP_ESTABLISH_BEARER_CNF will be received */
            }
        }
        else
        {
            /* if there is no connection already available for this handle do nothing */
            if ( s_CgpsSupl[vp_Index].v_ConnectionHandle != 0 )
            {
                MC_CGPS_TRACE(("CGPS5_18ConnectSUPLSession : Connection already exists!"));
            }
            /* if there's a pending connection, just wait for K_LSIMUP_TCPIP_CONNECT_CNF */
            else if (-1 != vg_CGPS_Supl_Pending_Connection)
            {
                MC_CGPS_TRACE(("CGPS5_18ConnectSUPLSession : pending connection(s)"));
            }
            else
            {
                CGPS5_25EstablishTcpIpSession( vp_Index );

                CGPS5_26LsimupMobileInfoReq();
            }
        }
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 19
/******************************************************************************/
/* CGPS5_19InitSuplBearer : Initialise the bearer                             */
/******************************************************************************/
void CGPS5_19InitSuplBearer(void)
{
/* +LMSqc03064 */
#ifdef CGPS_ENABLE_CONFIG_FROM_FILE
    t_gfl_FILE * pl_File;
#endif /* #ifdef CGPS_ENABLE_CONFIG_FROM_FILE */
    /* set bearer configuration */
    vg_CGPS_SUPL_Bearer.v_BearerType = K_LSIMUP_GPRS_BEARER;
    vg_CGPS_SUPL_Bearer.v_SelectedModem = K_LSIMUP_ISP_MODEM;
    vg_CGPS_SUPL_Bearer.s_PhoneCalledNumber = NULL;

    if (vg_CGPS_SUPL_Bearer.s_APN)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_APN);
        vg_CGPS_SUPL_Bearer.s_APN = NULL;
    }
    if (vg_CGPS_SUPL_Bearer.s_Login)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_Login);
        vg_CGPS_SUPL_Bearer.s_Login = NULL;
    }
    if (vg_CGPS_SUPL_Bearer.s_Password)
    {
        MC_RTK_FREE_MEMORY(vg_CGPS_SUPL_Bearer.s_Password);
        vg_CGPS_SUPL_Bearer.s_Password = NULL;
    }

#ifdef CGPS_ENABLE_CONFIG_FROM_FILE
    /*open the bearer configuration file */
    pl_File=(t_gfl_FILE *)MC_GFL_FOPEN(CGPS_SUPL_SERVER_FILE_NAME,(const uint8_t*)"r");
    if (pl_File)
    {
        CGPS4_06ReadString(&vg_CGPS_SUPL_Bearer.s_APN,      pl_File);

        CGPS4_06ReadString(&vg_CGPS_SUPL_Bearer.s_Login,    pl_File);

        CGPS4_06ReadString(&vg_CGPS_SUPL_Bearer.s_Password, pl_File);

        /* Close file*/
        MC_GFL_FCLOSE(pl_File);
    }
    else
    {
#endif /* #ifdef CGPS_ENABLE_CONFIG_FROM_FILE */
        /* used default setting*/
        vg_CGPS_SUPL_Bearer.s_APN = (int8_t*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen(CGPS_SUPL_DEFAULT_APN)+1));
        strncpy((char*)vg_CGPS_SUPL_Bearer.s_APN, CGPS_SUPL_DEFAULT_APN, strlen(CGPS_SUPL_DEFAULT_APN)+1);

        vg_CGPS_SUPL_Bearer.s_Login = (int8_t*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen(CGPS_SUPL_DEFAULT_LOGIN)+1));
        strncpy((char*)vg_CGPS_SUPL_Bearer.s_Login, CGPS_SUPL_DEFAULT_LOGIN, strlen(CGPS_SUPL_DEFAULT_LOGIN)+1);

        vg_CGPS_SUPL_Bearer.s_Password = (int8_t*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen(CGPS_SUPL_DEFAULT_PWD)+1));
        strncpy((char*)vg_CGPS_SUPL_Bearer.s_Password, CGPS_SUPL_DEFAULT_PWD, strlen(CGPS_SUPL_DEFAULT_PWD)+1);
#ifdef CGPS_ENABLE_CONFIG_FROM_FILE
    }
#endif /* #ifdef CGPS_ENABLE_CONFIG_FROM_FILE */
/* -LMSqc03064 */

    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 20

void CGPS5_20ConvertQoP( t_CgpsNaf* pp_Handle , s_GN_SUPL_QoP * pp_QoP )
{
    double vl_HorizAccuracy , vl_VertAccuracy;
    uint8_t i = 0;
    /* +LMSqc17264 */
    /* uint32_t delay = pp_Handle->v_Timeout / 1000;*/ /* delay set in ms*/
    uint32_t delay = ( pp_Handle->v_Timeout - GN_GPS_Get_OS_Time_ms())/ 1000; /* v_Timeout is absolute value. Calculate max delay from now and request for it*/
    /* -LMSqc17264 */
    if (delay < CGPS_SUPL_CONNECTION_LATENCY) /* substract the CGPS latency time (time for the connexion....) it's require to do that because this timeout, it's the timeout wanted by the user.*/
    {
        pp_QoP->delay = 0x0;
    }
    else
    {
        delay -= CGPS_SUPL_CONNECTION_LATENCY;
        if ( delay > 127 )
        {
            pp_QoP->delay = 0x7;
        }
        else
        {
            bool done = FALSE;
            pp_QoP->delay = 0;
            for ( i=0 ; ( i < 8 ) && ( !done ) ; i++ )
            {
                delay = delay / 2;
                if ( delay > 0 )
                {
                    pp_QoP->delay++;
                }
                else
                {
                    done = TRUE;
                }
            }
        }
    }

    /* convert horizontal accuracy from m to 3GPP spec. hor_acc_in_m = 10*( 1.1^k -1 )
                therefore k = log( (hor_acc_in_m/10) + 1 ) / log ( 1.1 )
     */
    vl_HorizAccuracy = log( (pp_Handle->v_HorizAccuracy/10.0) + 1.0 ) / log ( 1.1 ) ;

    if ( vl_HorizAccuracy > 127.0 )
    {
        pp_QoP->horacc = 127;
    }
    else
    {
        pp_QoP->horacc = (uint8_t)floor(vl_HorizAccuracy);
    }

    /* vertical accuracy according to 3GPP spec is  ver_acc_in_m = 45*( 1.025^k - 1 )
              therefore k = log( (ver_acc_in_m/45) + 1 ) / log ( 1.025 )
          */
    vl_VertAccuracy = log( (pp_Handle->v_VertAccuracy /45.0) + 1.0 ) / log ( 1.025 ) ;
    if ( vl_VertAccuracy > 127.0 )
    {
        pp_QoP->veracc = 127;
    }
    else
    {
        pp_QoP->veracc = (uint8_t)floor(vl_VertAccuracy);
    }

    /* AgeLimit in NAF is in milliseconds */
    pp_QoP->maxLocAge = pp_Handle->v_AgeLimit / 1000.0 ;

    MC_CGPS_TRACE(("CGPS5_20ConvertQoP: calculated value \n  Horizontal accuracy : %i\n vertical accuracy:%i \nAge limit: %i \nDelay: %i",pp_QoP->horacc,pp_QoP->veracc,pp_QoP->maxLocAge,pp_QoP->delay));

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 21
void CGPS5_21SuplConfig( void )
{

    s_GN_SUPL_Config * pl_Config=NULL;
    char version[256];

    MC_CGPS_TRACE(("Enter : CGPS5_21SuplConfig"));

    pl_Config = (s_GN_SUPL_Config*)MC_RTK_GET_MEMORY(sizeof(*pl_Config));
    memset( pl_Config , 0 , sizeof( *pl_Config ) );

    memset( pl_Config , 0 , sizeof(*pl_Config) );

    if( MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPLV2_SUPPORTED))
    {
        /* According to the SUPL 2.0 specificaiton ie OMA-TS-ULP-V2_0-20091208-C,
           the value of UT1 and UT2 is 11 seconds */
        pl_Config->User_Timer_1_Duration = 11;
        pl_Config->User_Timer_2_Duration = 11;
    }
    else
    {
        /* According to the latest SUPL 1.0 specificaiton available ie OMA-AD-SUPL-V1_0-20070615-A,
           the value of UT1 and UT2 is 10 seconds */
        pl_Config->User_Timer_1_Duration = 10;
        pl_Config->User_Timer_2_Duration = 10;
    }

    pl_Config->User_Timer_3_Duration = 10;

    pl_Config->p_SET_mdn = NULL;
    pl_Config->p_SET_min = NULL;

    if (vg_CGPS_Supl_MSISDN)
    {
        pl_Config->p_SET_msisdn = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((char*)vg_CGPS_Supl_MSISDN)+1));
        strncpy(pl_Config->p_SET_msisdn, (char*)vg_CGPS_Supl_MSISDN, strlen((const char*)vg_CGPS_Supl_MSISDN)+1);
        MC_CGPS_TRACE(("Set MSISDN : %s" , pl_Config->p_SET_msisdn ));
    }
    else
    {
        pl_Config->p_SET_msisdn = NULL;
        MC_CGPS_TRACE(("No MSISDN" ));
    }

    if (vg_CGPS_Supl_IMSI)
    {
        pl_Config->p_SET_imsi = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((const char*)vg_CGPS_Supl_IMSI)+1));
        strncpy(pl_Config->p_SET_imsi, (char*)vg_CGPS_Supl_IMSI, strlen((const char*)vg_CGPS_Supl_IMSI)+1);
        MC_CGPS_TRACE(("Set IMSI : %s" , pl_Config->p_SET_imsi ));
    }
    else
    {
        pl_Config->p_SET_imsi = NULL;
        MC_CGPS_TRACE(("No IMSI "));
    }

    pl_Config->p_SET_nai          = NULL;
    pl_Config->SET_IP_Type        = IP_None;
    pl_Config->SET_IP_Address     = NULL;
    pl_Config->PT_agpsSETAssisted = MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_MSASSISTED ); /* Positioning Technology AGPS SET Assisted */
    pl_Config->PT_agpsSETBased    = MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_MSBASED );    /* Positioning Technology AGPS SET Based */
    pl_Config->PT_autonomousGPS   = MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_AUTONOMOUS ); /* Positioning Technology Autonomous GPS */
    pl_Config->PT_eCID            = MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_MS_POS_TECH_ECID );       /* Positioning Technology Enhanced Cell ID */

    MC_CGPS_TRACE(("SlpAddress pointer : %p " , vg_CGPS_Supl_Slp_Config.v_Addr ));

    if( NULL != vg_CGPS_Supl_Slp_Config.v_Addr )
    {
        pl_Config->SLP_IP_Address = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((const char*)vg_CGPS_Supl_Slp_Config.v_Addr)+1));
        strncpy(pl_Config->SLP_IP_Address,(const char*)vg_CGPS_Supl_Slp_Config.v_Addr, strlen((const char*)vg_CGPS_Supl_Slp_Config.v_Addr)+1);
        vg_CGPS_Supl_Slp_Config.v_AddrLen = strlen( (const char*)vg_CGPS_Supl_Slp_Config.v_Addr );
        MC_CGPS_TRACE(("SlpAddress : %s " , vg_CGPS_Supl_Slp_Config.v_Addr ));

        switch (vg_CGPS_Supl_Slp_Config.v_AddrType)
        {
            case K_CGPS_SLPADDR_FQDN:
            {
                pl_Config->SLP_IP_Type = FQDN_Address;
                break;
            }
            case K_CGPS_SLPADDR_IPV6:
            {
                pl_Config->SLP_IP_Type = IPV6_Address;
                break;
            }
            case K_CGPS_SLPADDR_IPV4:
            default:
            {
                pl_Config->SLP_IP_Type = IPV4_Address;
                break;
            }
        }
    }
    else
    {
        pl_Config->SLP_IP_Address = NULL;
        pl_Config->SLP_IP_Type = IP_None;
        MC_CGPS_TRACE(("No SUPL Server Configured!"));
    }

    pl_Config->SLP_IP_Port = vg_CGPS_Supl_Slp_Config.v_PortNum;

    snprintf( version , sizeof(version) , "ST-Ericsson_LBS-%02d.%03d.%03d-%03d.%03d" ,  \
                            LBS_MAJOR_VERSION,         \
                            LBS_MINOR_VERSION,         \
                            LBS_PATCH_VERSION,         \
                            LBS_CSL_PROJECT_IDENT,     \
                            LBS_CSL_PATCH_VERSION      \
                                );

    // Pass the platfrom version to SUPL library
    pl_Config->p_PlatfromVersion = version;

    GN_SUPL_Set_Config(pl_Config);
    /* +SUPLv2.0 Modifications */


        CGPS5_45SuplConfigExt();

        if (pl_Config->p_SET_imsi)
        {
            MC_RTK_FREE_MEMORY(pl_Config->p_SET_imsi);
        }
        if (pl_Config->p_SET_msisdn)
        {
            MC_RTK_FREE_MEMORY(pl_Config->p_SET_msisdn);
        }
        if (pl_Config->SLP_IP_Address)
        {
            MC_RTK_FREE_MEMORY(pl_Config->SLP_IP_Address);
        }
        MC_RTK_FREE_MEMORY(pl_Config);


    MC_CGPS_TRACE(("Exit : CGPS5_21SuplConfig"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 25
/******************************************************************************/
/* CGPS5_25EstablishTcpIpSession : Request To Establish a TCP/IP session      */
/******************************************************************************/
void CGPS5_25EstablishTcpIpSession( int8_t vp_SessionIndex )
{
    t_lsimup_TcpIpConnectReqM * pl_lsimup_TcpIpConnectReq=NULL;

    uint8_t     vl_AddressLength   = strlen((const char*)s_CgpsSupl[vp_SessionIndex].v_ConnectionParm.p_TcpIp_Address);
    pl_lsimup_TcpIpConnectReq = (t_lsimup_TcpIpConnectReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_lsimup_TcpIpConnectReq));

    /* establish TCPIP connection */
    pl_lsimup_TcpIpConnectReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
    pl_lsimup_TcpIpConnectReq->s_IPAddressType = CGPS4_07ConvertTcpIpAddressType( s_CgpsSupl[vp_SessionIndex].v_ConnectionParm.TcpIp_AddressType );
    if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_NOTLS_SUPPORTED))
    pl_lsimup_TcpIpConnectReq->s_SocketType    = K_LSIMUP_SOCKET_TYPE_TCP;
    else
        pl_lsimup_TcpIpConnectReq->s_SocketType    = K_LSIMUP_SOCKET_TYPE_TLS;
    MC_CGPS_TRACE(("CGPS5_25EstablishTcpIpSession: Socket Type %d",pl_lsimup_TcpIpConnectReq->s_SocketType));
    pl_lsimup_TcpIpConnectReq->s_IPPortNum     = (uint32_t)s_CgpsSupl[vp_SessionIndex].v_ConnectionParm.Port;
    pl_lsimup_TcpIpConnectReq->s_IPAddress     = (char*)MC_RTK_GET_MEMORY((vl_AddressLength + 1)*sizeof(char));
    strncpy(pl_lsimup_TcpIpConnectReq->s_IPAddress, s_CgpsSupl[vp_SessionIndex].v_ConnectionParm.p_TcpIp_Address, (vl_AddressLength + 1));

    CGPS4_28SendMsgToProcess(PROCESS_CGPS ,
                             PROCESS_LSIMUP,
                             CGPS_LSIMUP_TCPIP_CONNECT_REQ,
                             (t_MsgHeader *)pl_lsimup_TcpIpConnectReq);

    vg_CGPS_Supl_Pending_Connection = vp_SessionIndex;
    /* +LMSqc38060 */
    vg_CGPS_Supl_Timeout = GN_GPS_Get_OS_Time_ms() + CGPS_SUPL_MAX_CONNECTION_SETUP_TIME;
    /* -LMSqc38060 */
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 26
/******************************************************************************/
/* CGPS5_26LsimupMobileInfoReq : Request To Mobile Info                       */
/******************************************************************************/
void CGPS5_26LsimupMobileInfoReq()
{
    t_lsimup_MobileInfoReqM* p_MsgMobileInfoReq =NULL;
    p_MsgMobileInfoReq = (t_lsimup_MobileInfoReqM *)MC_RTK_GET_MEMORY(sizeof(*p_MsgMobileInfoReq));

    p_MsgMobileInfoReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;
    p_MsgMobileInfoReq->v_RequiredField = K_LSIMUP_MCC_FIELD      /* Mobile Country Code */
                                          | K_LSIMUP_MNC_FIELD    /* Mobile Network Code */
                                          | K_LSIMUP_LAC_FIELD    /* Location Area Code */
                                          | K_LSIMUP_CID_FIELD    /* Cell Identity */
                                          | K_LSIMUP_RNC_FIELD    /* Additional network information for UMTS */
                                          | K_LSIMUP_NMR_FIELD    /* Network Measurement results */
                                          | K_LSIMUP_TA_FIELD     /* Timing Advance */
                                          | K_LSIMUP_MSISDN_FIELD /* MSISDN */
                                          | K_LSIMUP_IMSI_FIELD;  /* IMSI */

    if(MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPLV2_SUPPORTED))
    {
        p_MsgMobileInfoReq->v_RequiredField = p_MsgMobileInfoReq->v_RequiredField | K_LSIMUP_WLAN_FIELD |K_LSIMUP_WIMAX_FIELD;
        //MC_CGPS_TRACE(("CGPS5_26LsimupMobileInfoReq WLAN Field %x WIMAX Field %x", (p_MsgMobileInfoReq->v_RequiredField&K_LSIMUP_WLAN_FIELD),(p_MsgMobileInfoReq->v_RequiredField&K_LSIMUP_WIMAX_FIELD)));
    }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_LSIMUP, 0,
                               CGPS_LSIMUP_MOBILE_INFO_REQ,
                               (t_MsgHeader *)p_MsgMobileInfoReq);
}

/* + LMSQC16384 : LMSQC16386 SUPL WAP Push */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 27
/******************************************************************************/
/* CGPS5_27SuplPush : Indicator of new SUPL INIT via WAP push                       */
/******************************************************************************/
e_cgps_Error CGPS5_27SuplPush  ( t_cgps_SuplPush  vp_SuplPush )
{
    t_cgps_SuplPush    *pl_cgps_SuplPush=NULL;
    t_CGPS_SuplPushInd* pl_SuplPushInd=NULL;
    pl_SuplPushInd = (t_CGPS_SuplPushInd*)MC_RTK_GET_MEMORY( sizeof(*pl_SuplPushInd ));

    MC_CGPS_TRACE(("CGPS5_27SuplPush called"));

    memcpy(&pl_SuplPushInd->v_SuplPush, &vp_SuplPush, sizeof(*pl_cgps_SuplPush));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,
                               PROCESS_CGPS,0,
                               CGPS_SUPL_PUSH_IND,
                               (t_MsgHeader*)pl_SuplPushInd );

    MC_CGPS_TRACE(("CGPS5_27SuplPush end"));

    return K_CGPS_NO_ERROR;
}
/* - LMSQC16384 : LMSQC16386 SUPL WAP Push */

/* +LMSqc18414 */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 28
/******************************************************************************/
/* CGPS5_28HandleAuthenticationFailure :                                      */
/******************************************************************************/
void CGPS5_28HandleAuthenticationFailure()
{
    /* if the connection to old supl address has failed with authentication failure. Replace it with the auto-configured address */
    /* If we were already using the auto-configured address, do nothing */

    if( strncmp((const char*)vg_CGPS_Supl_Slp_Config.v_Addr, (const char*)vg_CGPS_Supl_AutoSlp_Config.v_Addr , strlen((const char*)vg_CGPS_Supl_Slp_Config.v_Addr) ) != 0 )
    {
        t_cgps_SuplPush vl_SuplPush;
        int vl_Index,vl_AvailableIndexFound;

        MC_RTK_FREE_MEMORY( vg_CGPS_Supl_Slp_Config.v_Addr );
        vg_CGPS_Supl_Slp_Config.v_Addr = (int8_t*)MC_RTK_GET_MEMORY( vg_CGPS_Supl_AutoSlp_Config.v_AddrLen + 1 );
        memcpy( vg_CGPS_Supl_Slp_Config.v_Addr, vg_CGPS_Supl_AutoSlp_Config.v_Addr,  vg_CGPS_Supl_AutoSlp_Config.v_AddrLen + 1);

        vg_CGPS_Supl_Slp_Config.v_AddrType = K_CGPS_SLPADDR_FQDN;
        vg_CGPS_Supl_Slp_Config.v_AddrLen = vg_CGPS_Supl_AutoSlp_Config.v_AddrLen ;

        if(vg_CGPS_Supl_Eslp_Config.v_Addr != NULL)
        MC_RTK_FREE_MEMORY( vg_CGPS_Supl_Eslp_Config.v_Addr );

        vg_CGPS_Supl_Eslp_Config.v_Addr = (int8_t*)MC_RTK_GET_MEMORY( vg_CGPS_Supl_AutoEslp_Config.v_AddrLen + 1 );
        memcpy( vg_CGPS_Supl_Eslp_Config.v_Addr, vg_CGPS_Supl_AutoEslp_Config.v_Addr ,  vg_CGPS_Supl_AutoEslp_Config.v_AddrLen + 1);

        vg_CGPS_Supl_Eslp_Config.v_AddrType = K_CGPS_SLPADDR_FQDN;
        vg_CGPS_Supl_Eslp_Config.v_AddrLen = vg_CGPS_Supl_AutoEslp_Config.v_AddrLen ;

        CGPS5_21SuplConfig();

        if(vg_CGPS_Supl_Init_Length)
        {
            /* We have now configured the new SUPL address */
            vl_SuplPush.v_HashLength = 0;
            vl_SuplPush.v_pointerToHash = NULL;

            vl_SuplPush.v_pointerToPayload = pg_CGPS_Supl_Init;
            vl_SuplPush.v_PayloadLength    = vg_CGPS_Supl_Init_Length;

            for( vl_Index = 0 , vl_AvailableIndexFound = FALSE;
                ( vl_Index < K_CGPS_MAX_NUMBER_OF_SUPL ) && ( !vl_AvailableIndexFound );
                 vl_Index ++ )
            {
                if( s_CgpsSupl[vl_Index].v_IsRegistered == FALSE )
                {
                    vl_AvailableIndexFound = TRUE;
                }
            }

            if( vl_AvailableIndexFound
                && ( vl_Index < K_CGPS_MAX_NUMBER_OF_SUPL)
            )
            {
                if(CGPS4_18CheckNewSessionRight(CGPS_SUPL_APPLICATION_ID))
                {
                    CGPS4_19UpdateOtherApplicationState(CGPS_SUPL_APPLICATION_ID);
                    s_CgpsSupl[vl_Index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
                }
                else
                {
                    s_CgpsSupl[vl_Index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
                }

                s_CgpsSupl[vl_Index].v_IsRegistered = TRUE;
                /* increase the number of applications */
                vg_CGPS_Supl_Count++;

                CGPS5_00SuplPushInd((uint32_t)&(s_CgpsSupl[vl_Index]),&vl_SuplPush );
            }

        }

    }
}
/* -LMSqc18414 */

/* +LMSqc20169 */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 29
/******************************************************************************/
/* CGPS5_29GetLengthOfMnc :                                                   */
/******************************************************************************/
uint8_t CGPS5_29GetLengthOfMnc( uint16_t v_MCC )
{
    uint16_t vl_Index;

    for( vl_Index = 0 ;
        (vg_CGPS_MCC_With_3Digit_MNC_List[vl_Index]!=0) && ( vg_CGPS_MCC_With_3Digit_MNC_List[vl_Index] != v_MCC );
        vl_Index++ );

    return (vg_CGPS_MCC_With_3Digit_MNC_List[vl_Index]!=0) ? 3 : 2;
}
/* -LMSqc20169 */


/* +LMSqc38060 */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 30
/******************************************************************************/
/* CGPS5_30VerifySuplActivity : Verify the SUPL actvity                       */
/******************************************************************************/
void CGPS5_30VerifySuplActivity()
{
    /*Following case should be ideally handled:
    1) When NAF session corresponding to SUPL session is stopped, SUPL session must also be aborted.
    2a) When all NAF session are stopped, all MO SUPL session is stopped
    2b) When all NAF session are stopped, all NI session must not be stopped

    To handle  (1) require SUPL and NAF session to be linked together. Currently software doesnot link them together, so (1) is not taken care.
    To handle (2b) NI/MO session information must be stored in SUPL Database. This is not currently available and hence (2b) is not taken care.
    Only case taken care is (2a) and all session including NI is stopped.

    */
    uint8_t vl_index = 0;

    if((vg_CGPS_Naf_Count != 0) || (vg_CGPS_Supl_Count == 0)) return;

    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL; vl_index++)
    {
         if ((s_CgpsSupl[vl_index].v_IsRegistered == FALSE)) continue;

         if( s_CgpsSupl[vl_index].v_Suspended != CGPS_SESSION_SUSPENDED )
         {
              GN_SUPL_Abort(s_CgpsSupl[vl_index].v_GPSHandle);
         }
    }

    GN_SUPL_Handler();

    CGPS0_52ConfigureCgpsNextState();

    return;
}
/* -LMSqcGoogleApp */
/* +LMSqcGoogleApp2 */


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 30
/******************************************************************************/
/* CGPS5_31SendTCPDisconnectReq : Send Disconnect                             */
/******************************************************************************/
void CGPS5_31SendTcpDisconnectReq(uint32_t v_SessionHandle, uint32_t v_ConnectionHandle)
{
    t_lsimup_TcpIpDisconnectReqM * pl_lsimup_TcpIpDisconnectReq;
    pl_lsimup_TcpIpDisconnectReq = (t_lsimup_TcpIpDisconnectReqM*)MC_RTK_GET_MEMORY(sizeof(t_lsimup_TcpIpDisconnectReqM));
    pl_lsimup_TcpIpDisconnectReq->v_SessionHandle = v_SessionHandle;
    pl_lsimup_TcpIpDisconnectReq->v_ConnectionHandle = v_ConnectionHandle;

    CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                             PROCESS_LSIMUP,
                             CGPS_LSIMUP_TCPIP_DISCONNECT_REQ,
                             (t_MsgHeader *)pl_lsimup_TcpIpDisconnectReq);

    MC_CGPS_TRACE(("CGPS5_31SendTcpDisconnectReq : TCPIP disconnection request sent to LSIMUP "));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 31
/******************************************************************************/
/* CGPS5_31HandleBearerEstablishmentFailure : Send Disconnect                 */
/******************************************************************************/
void CGPS5_31HandleBearerEstablishmentFailure()
{
    int8_t vl_index = 0;

    MC_CGPS_TRACE(("CGPS5_31HandleBearerEstablishmentFailure"));

    /* reject all pending TCPIP connection requests */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL; vl_index++)
    {
         if (    ( s_CgpsSupl[vl_index].v_IsRegistered == TRUE )
          && ( NULL != s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address )
         )
         {
              e_GN_Status vl_Status = GN_ERR_CONN_REJECTED;

              GN_SUPL_Connect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status, NULL);

              GN_SUPL_Handler();

              MC_CGPS_TRACE(("CGPS5_31HandleBearerEstablishmentFailure : pending connection removed : CGPS Handle=0x%x, GPSHandle=0x%x", \
                              s_CgpsSupl[vl_index], s_CgpsSupl[vl_index].v_GPSHandle));

              CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
/*+SYScs46505*/
              CGPS4_35UpdateNumSuplFailure(&vg_AssistTrack);
/*-SYScs46505*/
         }
    }

    vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;

    CGPS0_52ConfigureCgpsNextState();

    return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 32
/******************************************************************************/
/* CGPS5_32HandleTcpConnectionFailure : Send Disconnect                       */
/******************************************************************************/
void CGPS5_32HandleTcpConnectionFailure()
{
    int8_t vl_index = vg_CGPS_Supl_Pending_Connection;
    e_GN_Status vl_Status=GN_ERR_CONN_TIMEOUT;

    MC_CGPS_TRACE(("CGPS5_32HandleTcpConnectionFailure"));

    if (-1 != vl_index)
    {
        if(    s_CgpsSupl[vl_index].v_IsRegistered     == TRUE
            && s_CgpsSupl[vl_index].v_ConnectionHandle == 0 )
        {

            GN_SUPL_Connect_Ind_In(s_CgpsSupl[vl_index].v_GPSHandle, &vl_Status, NULL);

            GN_SUPL_Handler();

            CGPS4_13DeleteSuplContext(&s_CgpsSupl[vl_index]);
/*+SYScs46505*/
            CGPS4_35UpdateNumSuplFailure(&vg_AssistTrack);
/*-SYScs46505*/

        }
    }

    vg_CGPS_Supl_Pending_Connection = -1;

    /* search for another pending connection request */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && (-1 == vg_CGPS_Supl_Pending_Connection); vl_index++)
    {
        if ((s_CgpsSupl[vl_index].v_IsRegistered == TRUE)
                && (s_CgpsSupl[vl_index].v_ConnectionHandle == 0)
                && (NULL != s_CgpsSupl[vl_index].v_ConnectionParm.p_TcpIp_Address)
            && (s_CgpsSupl[vl_index].v_Suspended        == CGPS_SESSION_NOT_SUSPENDED)
          )
        {
            CGPS5_25EstablishTcpIpSession( vl_index );

            CGPS5_26LsimupMobileInfoReq();
            break;
        }
    }

    if ( -1 == vg_CGPS_Supl_Pending_Connection )
    {
        if(    (vg_CGPS_Supl_Count == 0)
            && (vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHED))
        {
            t_lsimup_CloseBearerReqM * pl_lsimup_CloseBearerReq;

            pl_lsimup_CloseBearerReq = (t_lsimup_CloseBearerReqM*)MC_RTK_GET_MEMORY(sizeof(t_lsimup_CloseBearerReqM));
            pl_lsimup_CloseBearerReq->v_SessionHandle = vg_CGPS_LSIMUP_Session_Handle;

            CGPS4_28SendMsgToProcess(PROCESS_CGPS,
                                       PROCESS_LSIMUP,
                                       CGPS_LSIMUP_CLOSE_BEARER_REQ,
                                       (t_MsgHeader *)pl_lsimup_CloseBearerReq);

            vg_CGPS_SUPL_Bearer.v_BearerState = K_CGPS_BEARER_INITIALISED;

            MC_CGPS_TRACE(("CGPS5_32HandleTcpConnectionFailure : No more connection, CGPS_LSIMUP_CLOSE_BEARER_REQ sent"));
        }
    }


    CGPS0_52ConfigureCgpsNextState();
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 33
/******************************************************************************/
/* CGPS5_31SendTCPDisconnectReq : Send Disconnect                             */
/******************************************************************************/
void CGPS5_33HandleTimerExpiry()
{
    uint32_t vl_CurrentTime = GN_GPS_Get_OS_Time_ms();

    if(   ( vg_CGPS_Supl_Timeout == 0xFFFFFFFF)
       || ( vl_CurrentTime      <= vg_CGPS_Supl_Timeout ))
    {
         return;
    }

    vg_CGPS_Supl_Timeout = 0xFFFFFFFF;

    if(vg_CGPS_SUPL_Bearer.v_BearerState == K_CGPS_BEARER_ESTABLISHING)
    {
        /*Timeout for Bearer to establish, handle similar to Bearer Err*/
        CGPS5_31HandleBearerEstablishmentFailure();

        MC_CGPS_TRACE(("CGPS5_33HandleTimerExpiry : Waiting for Bearer Establishment"));

        return;
    }

    if(vg_CGPS_Supl_Pending_Connection != -1)
    {
    /*Timeout for Connection Request*/

        CGPS5_32HandleTcpConnectionFailure();

        MC_CGPS_TRACE(("CGPS5_33HandleTimerExpiry : Waiting for TCP Connection"));

        return;
    }
}
/* -LMSqc38060 */




/* +SUPLv2.0 Modifications */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 35
/******************************************************************************/
/* CGPS5_35TriggeredIntervalTime : Start Timer for Periodic/Event Interval Time period                           */
/******************************************************************************/
void CGPS5_35TriggeredIntervalTime(uint8_t vl_Index)
{
    if( s_CgpsSupl[vl_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_PERIODIC)
    {
        MC_CGPS_TRACE(("CGPS5_35TriggeredIntervalTime : timerId %u, StartTime %u",vl_Index,s_CgpsSupl[vl_Index].v_PeriodicTriggeredParams.v_IntervalFix));
        CGPS5_38StartTimer(vl_Index,(s_CgpsSupl[vl_Index].v_PeriodicTriggeredParams.v_IntervalFix));
    }

    else if ( s_CgpsSupl[vl_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_AREA)
    {
        MC_CGPS_TRACE(("CGPS5_35TriggeredIntervalTime : timerId %u, StartTime %u",vl_Index,s_CgpsSupl[vl_Index].v_AreaEventTriggerParams.v_MinIntervalTime));
        CGPS5_38StartTimer(vl_Index,(s_CgpsSupl[vl_Index].v_AreaEventTriggerParams.v_MinIntervalTime));
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 36
/******************************************************************************/
/* CGPS5_36TriggeredStartTime : Start Timer for Start Time period                              */
/******************************************************************************/
void CGPS5_36TriggeredStartTime(uint8_t vl_Index)
{
    uint32_t vl_CurrentOSTime = GN_GPS_Get_OS_Time_ms();

    if( s_CgpsSupl[vl_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_PERIODIC)
    {
        MC_CGPS_TRACE(("CGPS5_36TriggeredStartTime : timerId %u, StartTime %u",vl_Index,s_CgpsSupl[vl_Index].v_PeriodicTriggeredParams.v_StartTime - vl_CurrentOSTime));
        CGPS5_38StartTimer(vl_Index,(s_CgpsSupl[vl_Index].v_PeriodicTriggeredParams.v_StartTime - vl_CurrentOSTime));
    }
    else if ( s_CgpsSupl[vl_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_AREA)
    {
        MC_CGPS_TRACE(("CGPS5_36TriggeredStartTime : timerId %u, StartTime %u",vl_Index,s_CgpsSupl[vl_Index].v_AreaEventTriggerParams.v_StartTime - vl_CurrentOSTime));
        CGPS5_38StartTimer(vl_Index,(s_CgpsSupl[vl_Index].v_AreaEventTriggerParams.v_StartTime - vl_CurrentOSTime));
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 38
/******************************************************************************/
/* CGPS5_38StartTimer : Start the Timer                         */
/******************************************************************************/
void CGPS5_38StartTimer(uint8_t timerId,uint32_t v_Millisec )
{

    MC_CGPS_TRACE(("CGPS5_38StartTimer : Trigger for timerId %u",timerId));

    switch(timerId)
    {
     case 0:
         MC_RTK_PROCESS_START_TIMER(CGPS_SUPLEVENT_TRIGGER_TIMEOUT0, MC_DIN_MILLISECONDS_TO_TICK(v_Millisec));
         break;
     case 1:
         MC_RTK_PROCESS_START_TIMER(CGPS_SUPLEVENT_TRIGGER_TIMEOUT1, MC_DIN_MILLISECONDS_TO_TICK(v_Millisec));
         break;
     case 2:
         MC_RTK_PROCESS_START_TIMER(CGPS_SUPLEVENT_TRIGGER_TIMEOUT2, MC_DIN_MILLISECONDS_TO_TICK(v_Millisec));
         break;
     case 3:
         MC_RTK_PROCESS_START_TIMER(CGPS_SUPLEVENT_TRIGGER_TIMEOUT3, MC_DIN_MILLISECONDS_TO_TICK(v_Millisec));
         break;
     case 4:
         MC_RTK_PROCESS_START_TIMER(CGPS_SUPLEVENT_TRIGGER_TIMEOUT4, MC_DIN_MILLISECONDS_TO_TICK(v_Millisec));
         break;
     default:
         break;
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 37
/******************************************************************************/
/* CGPS5_37SuplProcessEventTimer : Process the event generated by a particular Timer       */
/******************************************************************************/
void CGPS5_37SuplProcessEventTimer(uint8_t v_Index)
{
    uint32_t    vl_CurrentOsTime = GN_GPS_Get_OS_Time_ms();

    MC_CGPS_TRACE(("CGPS5_37SuplProcessEventTimer"));

    if(!s_CgpsSupl[v_Index].v_GPSHandle)
    {
        MC_CGPS_TRACE(("CGPS5_37SuplProcessEventTimer : Handler not valid"));
        return;
    }

    if(s_CgpsSupl[v_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_PERIODIC)
    {
        e_GN_Status v_Status = GN_SUCCESS;

        MC_CGPS_TRACE(("CGPS5_37SuplProcessEventTimer : Periodic Trigger for index %u",v_Index));

        MC_CGPS_TRACE(("CGPS5_37SuplProcessEventTimer : SUPL Context : CurrentTime %i, StartTime %i, NumFixes %i", \
            vl_CurrentOsTime, \
            s_CgpsSupl[v_Index].v_PeriodicTriggeredParams.v_StartTime, \
            s_CgpsSupl[v_Index].v_PeriodicTriggeredParams.v_NumFixes  \
            ));

        if(s_CgpsSupl[v_Index].v_PeriodicTriggeredParams.v_NumFixes > 0)
        {
            GN_SUPL_Trigger_Ind_In(s_CgpsSupl[v_Index].v_GPSHandle, &v_Status);

            CGPS5_35TriggeredIntervalTime(v_Index);

            s_CgpsSupl[v_Index].v_PeriodicTriggeredParams.v_NumFixes--;
        }
    }
    else if(s_CgpsSupl[v_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_AREA)
    {
        MC_CGPS_TRACE(("CGPS5_37SuplProcessEventTimer : Area Event Trigger for index %u",v_Index));

        MC_CGPS_TRACE(("CGPS5_37SuplProcessEventTimer : SUPL Context : CurrentTime %i, StartTime %i, StopTime %i, MaxReports %i", \
            vl_CurrentOsTime, \
            s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_StartTime, \
            s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_StopTime,  \
            s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_MaximumReports  \
            ));

        if( s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_StopTime <= vl_CurrentOsTime )
        {
            GN_SUPL_Trigger_End_Ind_In(s_CgpsSupl[v_Index].v_GPSHandle);
        }
        else if(s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_MaximumReports == 0)
        {
            GN_SUPL_Trigger_End_Ind_In(s_CgpsSupl[v_Index].v_GPSHandle);
        }
        else if( s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_MaximumReports > 0 )
        {
            e_cgps_RelativePosition vl_CurrentPositionStatus;
            e_GN_Status v_Status = GN_SUCCESS;

            if (s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_AreaDefinition.v_NumAreaIdLists)
            {
                if( CGPS4_56CheckForAreaIdEvent( s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_AreaDefinition.a_AreaIdLists,
                                           s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_AreaEventType,
                                           s_CgpsSupl[v_Index].v_PrevCellIdPositionStatus,
                                           &vl_CurrentPositionStatus,
                                           &vg_CGPS_Supl_Location_Id ) )
                {
                    MC_CGPS_TRACE(("CGPS4_56CheckForAreaIdEvent : AreaId event occurred, %s", a_CGPS_RelativePositionasEnum[vl_CurrentPositionStatus]));

                    GN_SUPL_Trigger_Ind_In(s_CgpsSupl[v_Index].v_GPSHandle, &v_Status);
                }

                s_CgpsSupl[v_Index].v_PrevCellIdPositionStatus = vl_CurrentPositionStatus;
            }
            else
            {
                GN_SUPL_Trigger_Ind_In(s_CgpsSupl[v_Index].v_GPSHandle, &v_Status);
            }

            if( s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_StopTime
                        < vl_CurrentOsTime+s_CgpsSupl[v_Index].v_AreaEventTriggerParams.v_MinIntervalTime )
            {
                /* If Stop time is lesser than the next fix interval, start a timer with stop time & abort SUPL session on expiry*/
                CGPS5_46AreaEventStopTime(v_Index);
            }
            else
            {
                CGPS5_35TriggeredIntervalTime(v_Index);
            }
        }
    }
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 39
/******************************************************************************/
/* CGPS5_39HandleEventSuplTimer0 : Handler for Timer ID 0      */
/******************************************************************************/
void CGPS5_39HandleEventSuplTimer0(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

   CGPS5_37SuplProcessEventTimer(0);
   MC_CGPS_TRACE(("CGPS5_39HandleEventSuplTimer0"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 40
/******************************************************************************/
/* CGPS5_39HandleEventSuplTimer1 : Handler for Timer ID 1     */
/******************************************************************************/
void CGPS5_40HandleEventSuplTimer1(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

    CGPS5_37SuplProcessEventTimer(1);
    MC_CGPS_TRACE(("CGPS5_39HandleEventSuplTimer1"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 41
/******************************************************************************/
/* CGPS5_39HandleEventSuplTimer2 : Handler for Timer ID 2     */
/******************************************************************************/
void CGPS5_41HandleEventSuplTimer2(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

    CGPS5_37SuplProcessEventTimer(2);
    MC_CGPS_TRACE(("CGPS5_39HandleEventSuplTimer2"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 42
/******************************************************************************/
/* CGPS5_39HandleEventSuplTimer3 : Handler for Timer ID 3      */
/******************************************************************************/
void CGPS5_42HandleEventSuplTimer3(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

    CGPS5_37SuplProcessEventTimer(3);
    MC_CGPS_TRACE(("CGPS5_39HandleEventSuplTimer3"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 43
/******************************************************************************/
/* CGPS5_39HandleEventSuplTimer4 : Handler for Timer ID 4      */
/******************************************************************************/
void CGPS5_43HandleEventSuplTimer4(t_RtkObject* p_FsmObject)
{
    p_FsmObject = p_FsmObject;

    CGPS5_37SuplProcessEventTimer(4);
    MC_CGPS_TRACE(("CGPS5_39HandleEventSuplTimer4"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 45
/******************************************************************************/
/* CGPS5_45SuplConfigExt : SUPL Configuration Extended to support SUPL v2.0  */
/******************************************************************************/
void CGPS5_45SuplConfigExt( void )
{
    s_GN_SUPL_Extd_Config * pl_ConfigExt;

    uint8_t vl_Index = 0;
    
    MC_CGPS_TRACE(("Enter : CGPS5_45SuplConfigExt"));

    pl_ConfigExt = (s_GN_SUPL_Extd_Config*)MC_RTK_GET_MEMORY(sizeof(s_GN_SUPL_Extd_Config));

    memset( pl_ConfigExt , 0 , sizeof( s_GN_SUPL_Extd_Config ) );
    if( MC_CGPS_IS_CONFIGURATION_ELEMENT_SET( K_CGPS_CONFIG_SUPLV2_SUPPORTED))
    {
        pl_ConfigExt->Major_Version = 2;//SUPL Version 2 supported
    }
    else
    {
        pl_ConfigExt->Major_Version = 1;
    }

/* General configuration for reporting data to the SUPL server */
    pl_ConfigExt->Reporting_Mode_Real_Time       = TRUE;
    pl_ConfigExt->Reporting_Mode_Quasi_Real_Time = FALSE;
    pl_ConfigExt->Reporting_Mode_Batch           = FALSE;
    pl_ConfigExt->Min_Report_Interval            = 1;
    pl_ConfigExt->Max_Report_Interval            = 300; /* Spirent system expects this value to be 1*/

/* Batch & Quasi Real Time Reporting Configuration */
    pl_ConfigExt->Batch_Report_Position          = FALSE;
    pl_ConfigExt->Batch_Report_Measurements      = FALSE;
    pl_ConfigExt->Max_Positions_In_Batch         = 10;
    pl_ConfigExt->Max_Measurement_In_Batch       = 10;

/* Periodic Trigger related configuration */
    pl_ConfigExt->Periodic_Trigger               = TRUE;

/* Area Event Trigger related configuration */
    pl_ConfigExt->Area_Event_Trigger             = TRUE;
    pl_ConfigExt->Geographic_Area_Ellipse        = TRUE;
    pl_ConfigExt->Geographic_Area_Polygon        = TRUE;
    pl_ConfigExt->Max_Geographic_Areas           = K_CGPS_MAX_NUM_GEO_AREA;
    pl_ConfigExt->Max_Area_ID_Lists              = K_CGPS_MAX_AREA_ID_LIST;
    pl_ConfigExt->Max_Area_ID_Per_List           = K_CGPS_MAX_AREA_ID;

/* User Timer configuration */
    pl_ConfigExt->User_Timer_5_Duration          = 10;
    pl_ConfigExt->User_Timer_6_Duration          = 10;
    pl_ConfigExt->User_Timer_7_Duration          = 10;
    pl_ConfigExt->User_Timer_8_Duration          = 10;
    pl_ConfigExt->User_Timer_9_Duration          = 60;
    pl_ConfigExt->User_Timer_10_Duration         = 60;

/* Number of sessions reported as supported to the SUPL server */
    pl_ConfigExt->Max_Num_Session                = K_CGPS_MAX_NUMBER_OF_SUPL;

    for(vl_Index = 0; vl_Index < CGPS_MAX_ESLP_ADDRESS ;vl_Index++)
    {
       if( vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[vl_Index] )
        {
            uint8_t v_EslpAddrLength = strlen( (const char*)vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[vl_Index]);
            pl_ConfigExt->p_ESlp_WhiteList[vl_Index] = (CH *)MC_RTK_GET_MEMORY(sizeof(char)* (v_EslpAddrLength +1));
            memcpy(pl_ConfigExt->p_ESlp_WhiteList[vl_Index],vg_CGPS_Eslp_whitelist_Address.s_ESlpAddr[vl_Index],v_EslpAddrLength);
            *(pl_ConfigExt->p_ESlp_WhiteList[vl_Index] + v_EslpAddrLength ) = 0x0;
            MC_CGPS_TRACE(("CGPS0_46UserConfigurationReceived : ESLP Address%i Address %s",vl_Index,pl_ConfigExt->p_ESlp_WhiteList[vl_Index]));

        }
    }

    if( NULL != vg_CGPS_Supl_Eslp_Config.v_Addr )
    {
        pl_ConfigExt->ESLP_IP_Address = (char*)MC_RTK_GET_MEMORY(sizeof(char)*(strlen((const char*)vg_CGPS_Supl_Eslp_Config.v_Addr)+1));
        strncpy(pl_ConfigExt->ESLP_IP_Address, (const char*)vg_CGPS_Supl_Eslp_Config.v_Addr, strlen((const char*)vg_CGPS_Supl_Eslp_Config.v_Addr)+1);
    
        pl_ConfigExt->ESLP_IP_Type = FQDN_Address;
        pl_ConfigExt->ESLP_IP_Port = vg_CGPS_Supl_Eslp_Config.v_PortNum ;

        MC_CGPS_TRACE(("ESlpAddress : %s " , vg_CGPS_Supl_Eslp_Config.v_Addr ));
    }
    else
    {
        pl_ConfigExt->ESLP_IP_Address = NULL;
        pl_ConfigExt->ESLP_IP_Type = IP_None;
        pl_ConfigExt->ESLP_IP_Port = 0  ;

        MC_CGPS_TRACE(("No ESLP Server Configured!"));
    }

    GN_SUPL_Extd_Set_Config(pl_ConfigExt);

    for(vl_Index = 0; vl_Index < CGPS_MAX_ESLP_ADDRESS ;vl_Index++)
    {
       if(pl_ConfigExt->p_ESlp_WhiteList[vl_Index])
       {
            MC_RTK_FREE_MEMORY(pl_ConfigExt->p_ESlp_WhiteList[vl_Index]);
       }
    }

    MC_RTK_FREE_MEMORY(pl_ConfigExt);

    MC_CGPS_TRACE(("Exit : CGPS5_45SuplConfigExt"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 46
/******************************************************************************/
/* CGPS5_46AreaEventStopTime : Start Timer for Start Time period                              */
/******************************************************************************/
void CGPS5_46AreaEventStopTime(uint8_t vl_Index)
{
    uint32_t vl_CurrentOSTime = GN_GPS_Get_OS_Time_ms();

    if ( s_CgpsSupl[vl_Index].v_TriggeredEventType == K_CGPS_TRIGGER_EVENTTYPE_AREA)
    {
        MC_CGPS_TRACE(("CGPS5_46AreaEventStopTime : timerId %u, StartTime %u",vl_Index,s_CgpsSupl[vl_Index].v_AreaEventTriggerParams.v_StopTime - vl_CurrentOSTime));
        CGPS5_38StartTimer(vl_Index,(s_CgpsSupl[vl_Index].v_AreaEventTriggerParams.v_StopTime - vl_CurrentOSTime));
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 47
/******************************************************************************/
/* CGPS5_47ThirdPartyStartSUPL : to retrieve the location of the Third Party                                */
/******************************************************************************/
bool CGPS5_47ThirdPartyStartSUPL( t_CgpsNaf* pp_Handle, t_cgps_ThirdPartyInfo *p_Third_party_info )
{
    e_GN_Status vl_Status;
    void * pl_NewHandle;
    int8_t vl_index = 0;
    s_GN_SUPL_QoP vl_Qop;

    MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Start"));


    /* search for the first available handler */
    for (vl_index = 0; vl_index < K_CGPS_MAX_NUMBER_OF_SUPL && s_CgpsSupl[vl_index].v_IsRegistered == TRUE; vl_index++);

    if (vl_index != K_CGPS_MAX_NUMBER_OF_SUPL)
    {
        if ( (NULL != pp_Handle) && (pp_Handle->v_Mode != K_CGPS_PERIODIC))
        {
            CGPS5_20ConvertQoP( pp_Handle , &vl_Qop );
        }

        if(    (pp_Handle != NULL)
            && p_Third_party_info != NULL
           )
        {
            s_GN_SUPL_V2_ThirdPartyInfo  supl_third_party_info;
            bool                         result=FALSE;
            MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : GN_SUPL_Start_ThirdParty_Location_Transfer called \n"));

            supl_third_party_info.thirdPartyId     =  p_Third_party_info->v_ThirdPartyId;
            supl_third_party_info.thirdPartyIdName =  (char*)p_Third_party_info->a_ThirdPartyName;

            MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Third Part ID Info ID \t %d \n",supl_third_party_info.thirdPartyId));
            MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Third Part ID Info Name \t %s \n",supl_third_party_info.thirdPartyIdName));
            result = GN_SUPL_ThirdPartyPosition_Req_In(&pl_NewHandle, &vl_Status, &vl_Qop, &supl_third_party_info);

            if(result != TRUE)
            {
                MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : GN_SUPL_Start_ThirdPartyPosition_Req_In_1 failed!!!"));
                return FALSE;
            }
            else
            {

                /* set a new SUPL application context */
                s_CgpsSupl[vl_index].v_IsRegistered = TRUE;
                s_CgpsSupl[vl_index].v_GPSHandle    = pl_NewHandle;
                s_CgpsSupl[vl_index].v_bearer       = &vg_CGPS_SUPL_Bearer;
                if (CGPS4_18CheckNewSessionRight(CGPS_SUPL_APPLICATION_ID))
                {
                    CGPS4_19UpdateOtherApplicationState(CGPS_SUPL_APPLICATION_ID);
                    s_CgpsSupl[vl_index].v_Suspended = CGPS_SESSION_NOT_SUSPENDED;
                }
                else
                {
                    s_CgpsSupl[vl_index].v_Suspended = CGPS_MIN_PRIORITY_VALUE;
                }

                if((pp_Handle->v_ApplicationIDInfo.v_ApplicationIDInfo_present))
                {
                    /*Application Id info is present so try t populate on to the supl imnstance*/
                    s_GN_SUPL_V2_ApplicationInfo v_supl_applicationIdInfo;
                    v_supl_applicationIdInfo.applicationIDInfoPresence = pp_Handle->v_ApplicationIDInfo.v_ApplicationIDInfo_present;
                    MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Application ID Info resent \t %d \n",v_supl_applicationIdInfo.applicationIDInfoPresence));
                    if(pp_Handle->v_ApplicationIDInfo.a_ApplicationProvider != NULL)
                    {
                        v_supl_applicationIdInfo.applicationProvider = (char*)pp_Handle->v_ApplicationIDInfo.a_ApplicationProvider;
                        MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Application ID Application Provider \t %s \n",v_supl_applicationIdInfo.applicationProvider));
                    }

                    if(pp_Handle->v_ApplicationIDInfo.a_ApplicationName != NULL)
                    {
                        v_supl_applicationIdInfo.appName = (char*)pp_Handle->v_ApplicationIDInfo.a_ApplicationName;
                        MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Application ID Application Name \t %s \n",v_supl_applicationIdInfo.appName));
                    }
                    if(pp_Handle->v_ApplicationIDInfo.a_ApplicationVersion != NULL)
                    {
                        v_supl_applicationIdInfo.appVersion = (char*)pp_Handle->v_ApplicationIDInfo.a_ApplicationVersion;
                        MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : Application ID Application Version \t %s \n",v_supl_applicationIdInfo.appVersion));
                    }
                    result = GN_SUPL_ApplicationID_Info_In(pl_NewHandle,&v_supl_applicationIdInfo,&vl_Status);
                }
                vg_CGPS_Supl_Count++;
                GN_SUPL_Handler();
            }
        }

        MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : GN_SUPL_Start_ThirdPartyPosition_Req_In successful, GPSHandle=0x%x", pl_NewHandle));
    }
    else
    {
        MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : K_CGPS_MAX_NUMBER_OF_SUPL reached!!!"));
        return FALSE;
    }


    MC_CGPS_TRACE(("CGPS5_47ThirdPartyStartSUPL : End"));
    return TRUE;
}
/******************************************************************************/
/* CGPS5_48Delete_LocationID                              */
/******************************************************************************/
void CGPS5_48Delete_LocationID_Store( void  )
{
    MC_CGPS_TRACE(("CGPS5_48Delete_LocationID"));

    if(GN_SUPL_Delete_LocationID_Data_Ind_In())
    {
        MC_CGPS_TRACE(("LocationID_Store Deleted"));
    }
}

/* -SUPLv2.0 Modifications */

#endif /*AGPS_UP_FTR*/
#undef __CGPS5SUPL_C__
