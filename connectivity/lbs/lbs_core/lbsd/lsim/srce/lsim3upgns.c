/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#define LSIM3UPGNS_C

#ifdef AGPS_UP_FTR

#include "lsimup.h"
#include "agps.h"

#undef  FILE_NUMBER
#define FILE_NUMBER     3

/* This file defines functions which execute the GNS callback with appropriate data */
void lsim3_00MobileInfoReq( e_lsimup_ValidityField v_RequestedData )
{
#ifndef GNS_CELL_INFO_FTR
    if((K_LSIMUP_WLAN_FIELD & v_RequestedData)||(K_LSIMUP_WIMAX_FIELD & v_RequestedData))
    {
        u_gnsWIRELESS_MsgData         v_MsgData;
        s_gnsWIRELESS_NetworkInfoReq  *pl_gnsWirelessNwInfoReq = NULL;

        v_MsgData.v_GnsWirelessNetWorkInfoReq.v_RequestedFields       = 0;

        if(K_LSIMUP_WLAN_FIELD & v_RequestedData)
        {
            v_MsgData.v_GnsWirelessNetWorkInfoReq.v_RequestedFields |= E_gnsWIRELESS_WLAN_FIELD ;
        }
        
        if(K_LSIMUP_WIMAX_FIELD & v_RequestedData)
        {
            v_MsgData.v_GnsWirelessNetWorkInfoReq.v_RequestedFields |= E_gnsWIRELESS_WIMAX_FIELD ;
        }

        GNS_ExecuteWirelessCallback(E_gnsWIRELESS_INFO_REQ, sizeof(*pl_gnsWirelessNwInfoReq), &v_MsgData);
    }
    else
    {

        u_gnsSUPL_MsgData   v_MsgData;
        s_gnsSUPL_MobileInfoReq *pl_gnsSUPL_MobileInfoReq=NULL;

        v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields = 0;

        memset( &v_MsgData , 0 , sizeof(v_MsgData) );

        if(  K_LSIMUP_MCC_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_MCC_FIELD ;
        if(  K_LSIMUP_MNC_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_MNC_FIELD;
        if(  K_LSIMUP_LAC_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_LAC_FIELD;
        if(  K_LSIMUP_CID_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_CID_FIELD;
        if(  K_LSIMUP_RNC_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_RNC_FIELD;
        if(  K_LSIMUP_NMR_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_NMR_FIELD;
        if(  K_LSIMUP_TA_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_NMR_FIELD ;
        if(  K_LSIMUP_MSISDN_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_MSISDN_FIELD;
        if(  K_LSIMUP_IMSI_FIELD & v_RequestedData ) v_MsgData.v_GnsSuplMobileInfoReq.v_RequestedFields |= E_gnsSUPL_IMSI_FIELD;

        GNS_ExecuteSuplCallback( E_gnsSUPL_MOBILE_INFO_REQ , sizeof( *pl_gnsSUPL_MobileInfoReq ) , &v_MsgData );

    }
#else //if defined GNS_CELL_INFO_FTR
      u_gnsCellInfo_MsgData   v_MsgData;
      s_gnsCellInfo_MobileInfoReq *pl_gnsCellInfo_MobileInfoReq=NULL;

    v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields =0;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    if(  K_LSIMUP_MCC_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_MCC_FIELD ;
    if(  K_LSIMUP_MNC_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_MNC_FIELD;
    if(  K_LSIMUP_LAC_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_LAC_FIELD;
    if(  K_LSIMUP_CID_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_CID_FIELD;
    if(  K_LSIMUP_RNC_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_RNC_FIELD;
    if(  K_LSIMUP_NMR_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_NMR_FIELD;
    if(  K_LSIMUP_TA_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_NMR_FIELD ;
    if(  K_LSIMUP_MSISDN_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_MSISDN_FIELD;
    if(  K_LSIMUP_IMSI_FIELD & v_RequestedData ) v_MsgData.v_GnsCellInfo_MobileInfoReq.v_RequestedFields |= E_gnsSUPL_IMSI_FIELD;
    GNS_ExecuteCellInfoCallback( E_gnsCellInfo_MOBILE_INFO_REQ , sizeof( *pl_gnsCellInfo_MobileInfoReq ) , &v_MsgData );

#endif
}

void lsim3_01TcpIpConnectReq( t_lsimup_ConnectionHandle v_ConnectionHandle , t_lsimup_TcpIpConnectReqM *p_Req )
{
    u_gnsSUPL_MsgData   v_MsgData;
    s_gnsSUPL_ConnectReq  *pl_gnsSUPL_ConnectReq=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsSuplConnectReq.v_Handle = (t_GnsConnectionHandle)v_ConnectionHandle;
    v_MsgData.v_GnsSuplConnectReq.p_ServerAddr = (uint8_t*)p_Req->s_IPAddress;

    switch( p_Req->s_IPAddressType )
    {
        case K_LSIMUP_IPV4_ADDRESS :
            v_MsgData.v_GnsSuplConnectReq.v_ServerAddrType = E_gnsSUPL_IPV4_ADDRESS;
            break;
        case K_LSIMUP_IPV6_ADDRESS :
            v_MsgData.v_GnsSuplConnectReq.v_ServerAddrType = E_gnsSUPL_IPV6_ADDRESS;
            break;
        case K_LSIMUP_URL_ADDRESS :
            v_MsgData.v_GnsSuplConnectReq.v_ServerAddrType = E_gnsSUPL_FQDN_ADDRESS;
            break;
        default:
                /* K_LSIMUP_ADDRESS_IN_SIM not handled */
            break;
    }

    //v_MsgData.v_GnsSuplConnectReq.p_ServerAddr    = (uint8_t*)p_Req->s_IPAddress;

   switch(p_Req->s_SocketType)
       {
        case K_LSIMUP_SOCKET_TYPE_TLS :
            v_MsgData.v_GnsSuplConnectReq.v_SocketType = E_gnsSUPL_SOCKETTYPE_TLS;
        break;
        case K_LSIMUP_SOCKET_TYPE_TCP :
            v_MsgData.v_GnsSuplConnectReq.v_SocketType = E_gnsSUPL_SOCKETTYPE_TCP;
        break;
        default:
            v_MsgData.v_GnsSuplConnectReq.v_SocketType = E_gnsSUPL_SOCKETTYPE_TLS;
        break;
       }
    v_MsgData.v_GnsSuplConnectReq.v_ServerPortNum = p_Req->s_IPPortNum;

    GNS_ExecuteSuplCallback( E_gnsSUPL_CONNECT_REQ, sizeof( *pl_gnsSUPL_ConnectReq ) , &v_MsgData );
}

void lsim3_02TcpIpDisconnectReq( t_lsimup_ConnectionHandle v_ConnectionHandle )
{
    u_gnsSUPL_MsgData   v_MsgData;
    s_gnsSUPL_DisconnectReq  *pl_gnsSUPL_DisconnectReq=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsSuplDisconnectReq.v_Handle = (t_GnsConnectionHandle)v_ConnectionHandle;

    GNS_ExecuteSuplCallback( E_gnsSUPL_DISCONNECT_REQ, sizeof( *pl_gnsSUPL_DisconnectReq ) , &v_MsgData );
}


void lsim3_03SendDataReq( t_lsimup_ConnectionHandle v_ConnectionHandle , uint8_t* p_Data , uint32_t v_DataLen)
{
    u_gnsSUPL_MsgData   v_MsgData;
    s_gnsSUPL_SendDataReq  *pl_gnsSUPL_SendDataReq=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsSuplSendDataReq.v_Handle = (t_GnsConnectionHandle)v_ConnectionHandle;
    v_MsgData.v_GnsSuplSendDataReq.p_Data = p_Data;
    v_MsgData.v_GnsSuplSendDataReq.v_DataLen = v_DataLen;
    GNS_ExecuteSuplCallback( E_gnsSUPL_SEND_DATA_REQ, sizeof( *pl_gnsSUPL_SendDataReq ) , &v_MsgData );
}

void lsim3_04ReceiveDataCnf( t_lsimup_ConnectionHandle v_ConnectionHandle )
{
    u_gnsSUPL_MsgData   v_MsgData;
    s_gnsSUPL_ReceiveDataCnf  *pl_gnsSUPL_ReceiveDataCnf=NULL;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    v_MsgData.v_GnsSuplReceiveDataCnf.v_Handle = (t_GnsConnectionHandle)v_ConnectionHandle;

    GNS_ExecuteSuplCallback( E_gnsSUPL_RECEIVE_DATA_CNF, sizeof( *pl_gnsSUPL_ReceiveDataCnf ) , &v_MsgData );
}

void lsim3_05TcpIpConnectCnf( t_GnsConnectionHandle v_ConnectionHandle )
{
    t_lsimup_TcpIpConnectCnfM* p_TcpIpConnectCnf=NULL;

    p_TcpIpConnectCnf = (t_lsimup_TcpIpConnectCnfM*) MC_RTK_GET_MEMORY(sizeof(* p_TcpIpConnectCnf));

    p_TcpIpConnectCnf->v_SessionHandle = 0;
    p_TcpIpConnectCnf->v_ConnectionHandle = (t_lsimup_ConnectionHandle ) v_ConnectionHandle;
    p_TcpIpConnectCnf->s_IPAddress = NULL;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_TCPIP_CONNECT_CNF,p_TcpIpConnectCnf);
}

void lsim3_06TcpIpConnectErr( t_GnsConnectionHandle v_ConnectionHandle, int v_ErrorType )
{
    t_lsimup_TcpIpConnectErrM* p_TcpIpConnectErr=NULL;

    p_TcpIpConnectErr = (t_lsimup_TcpIpConnectErrM*) MC_RTK_GET_MEMORY(sizeof(* p_TcpIpConnectErr));

    p_TcpIpConnectErr->v_SessionHandle = 0;
    p_TcpIpConnectErr->v_ConnectionHandle = (t_lsimup_ConnectionHandle ) v_ConnectionHandle;
    p_TcpIpConnectErr->s_IPAddress = NULL;


    switch((e_agps_ErrorType)v_ErrorType)
    {
        case K_AGPS_ERROR_TYPE_UNKNOWN:
        {
            p_TcpIpConnectErr->v_ErrorType = K_LSIMUP_ERROR_TYPE_UNKNOWN;
            break;
        }
        case K_AGPS_ERROR_TLS_HANDSHAKE_FAILED:
        {
            p_TcpIpConnectErr->v_ErrorType = K_LSIMUP_ERROR_TLS_HANDSHAKE_FAILED;
            break;
        }
        case K_AGPS_ERROR_DNS_RESOLVE_FAILED:
        {
            p_TcpIpConnectErr->v_ErrorType = K_LSIMUP_ERROR_NOT_ABLE_TO_GET_HOST_NAME;
            break;
        }
        case K_AGPS_ERROR_CONNECT_FAILED:
        {
            p_TcpIpConnectErr->v_ErrorType = K_LSIMUP_ERROR_CONNECT;
            break;
        }
        default :
        {
            p_TcpIpConnectErr->v_ErrorType = K_LSIMUP_ERROR_TYPE_UNKNOWN;
            break;
        }
    }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_TCPIP_CONNECT_ERR,p_TcpIpConnectErr);
}

void lsim3_07TcpIpDisconnectInd( t_GnsConnectionHandle v_ConnectionHandle )
{
    t_lsimup_TcpIpDisconnectIndM* p_TcpIpDisconnectInd=NULL;

    p_TcpIpDisconnectInd = (t_lsimup_TcpIpDisconnectIndM*) MC_RTK_GET_MEMORY(sizeof(* p_TcpIpDisconnectInd));

    p_TcpIpDisconnectInd->v_SessionHandle = 0;
    p_TcpIpDisconnectInd->v_ConnectionHandle = (t_lsimup_ConnectionHandle ) v_ConnectionHandle;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_TCPIP_DISCONNECT_IND,p_TcpIpDisconnectInd);
}

void lsim3_08SendDataCnf(  t_GnsConnectionHandle v_ConnectionHandle )
{
    t_lsimup_SendDataCnfM *p_SendDataCnf=NULL;

    p_SendDataCnf = (t_lsimup_SendDataCnfM*)MC_RTK_GET_MEMORY( sizeof(*p_SendDataCnf));

    p_SendDataCnf->v_SessionHandle = 0;
    p_SendDataCnf->v_ConnectionHandle = (t_lsimup_ConnectionHandle ) v_ConnectionHandle;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_SEND_DATA_CNF,p_SendDataCnf);
}

void lsim3_09ReceiveDataInd( t_GnsConnectionHandle v_ConnectionHandle , uint8_t *p_Data , uint32_t v_DataLen )
{
    t_lsimup_ReceiveDataIndM *p_ReceiveDataInd=NULL;

    p_ReceiveDataInd = (t_lsimup_ReceiveDataIndM*)MC_RTK_GET_MEMORY(sizeof(*p_ReceiveDataInd));

    p_ReceiveDataInd->v_SessionHandle = 0;
    p_ReceiveDataInd->v_ConnectionHandle = (t_lsimup_ConnectionHandle ) v_ConnectionHandle;
    p_ReceiveDataInd->v_DataPointer = p_Data;
    p_ReceiveDataInd->v_DataLength = v_DataLen;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_RECEIVE_DATA_IND,p_ReceiveDataInd);
}

void lsim3_10SMSReceiveInd( uint8_t *p_Data , uint32_t v_DataLen , uint8_t* p_Hash ,  uint32_t v_HashLen )
{
    t_lsimup_SmsPushIndM *p_SmsPushInd=NULL;

    p_SmsPushInd = (t_lsimup_SmsPushIndM*)MC_RTK_GET_MEMORY(sizeof(*p_SmsPushInd));

    p_SmsPushInd->v_SessionHandle = 0;
    p_SmsPushInd->v_pointerToPayload = p_Data;
    p_SmsPushInd->v_PayloadLength = v_DataLen;
    p_SmsPushInd->v_pointerToHash = p_Hash;
    p_SmsPushInd->v_HashLength = v_HashLen;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_SMS_PUSH_IND,p_SmsPushInd);
}

static void lsim3_11HandleGsmMobileInfo( s_gnsSUPL_GSMCellInfo *p_GsmInfo , t_lsimup_MobileInfo* p_MobileInfo  )
{
    if( (p_GsmInfo->v_MCC < 1000) && ( p_GsmInfo->v_MNC < 1000 ) )
    {
        /* Validity range for MCC and MNC from 0..999 */
        p_MobileInfo->v_Mcc = p_GsmInfo->v_MCC;
        p_MobileInfo->v_Mnc = p_GsmInfo->v_MNC;
        p_MobileInfo->v_Lac = p_GsmInfo->v_LAC;
        p_MobileInfo->v_CellIdentity =  p_GsmInfo->v_CellIdentity;
        p_MobileInfo->v_ValidityField  |= (K_LSIMUP_MCC_FIELD|K_LSIMUP_MNC_FIELD|K_LSIMUP_CID_FIELD);
    }

    if( p_GsmInfo->v_LAC != K_gnsUNKNOWN_U32 )
    {
        p_MobileInfo->v_Lac = p_GsmInfo->v_LAC;
        p_MobileInfo->v_ValidityField  |= K_LSIMUP_LAC_FIELD;
    }

    p_MobileInfo->v_Celltype = K_LSIMUP_GSM_CELL;

    if( p_GsmInfo->v_TimingAdvance < 0x100 )
    {
        p_MobileInfo->v_ValidityField  |= K_LSIMUP_TA_FIELD;
        p_MobileInfo->s_TA = p_GsmInfo->v_TimingAdvance;
    }

    if( p_GsmInfo->v_NumNeighbouringCells > 0 )
    {
        uint8_t vl_i;
        t_lsimup_NMR* pl_lsimup_NMR=NULL;

        p_MobileInfo->v_ValidityField  |= K_LSIMUP_NMR_FIELD;
        p_MobileInfo->v_NeighbouringCells = p_GsmInfo->v_NumNeighbouringCells;
        p_MobileInfo->p_NMR = (t_lsimup_NMR*) MC_RTK_GET_MEMORY(sizeof(* pl_lsimup_NMR));
        pl_lsimup_NMR =  p_MobileInfo->p_NMR ;
        vl_i = p_MobileInfo->v_NeighbouringCells;

        while( vl_i > 0 )
        {
            s_gnsSUPL_GsmNMR* p_Nmr = &p_GsmInfo->a_GsmNMR[p_MobileInfo->v_NeighbouringCells - vl_i];

            pl_lsimup_NMR->s_ARFCN = p_Nmr->v_ARFCN;
            pl_lsimup_NMR->s_BSIC = p_Nmr->v_BSIC;
            pl_lsimup_NMR->s_RXLev = p_Nmr->v_RXLev;
            vl_i--;

            if (vl_i>0)
            {
                pl_lsimup_NMR->p_NextNeighbour = (t_lsimup_NMR*) MC_RTK_GET_MEMORY(sizeof(* pl_lsimup_NMR));
                pl_lsimup_NMR = pl_lsimup_NMR->p_NextNeighbour;
            }
        }
    }
}

static void lsim3_12HandleWcdmaFddMobileInfo( s_gnsSUPL_WcdmaFDDCellInfo *p_WcdmaFddInfo , t_lsimup_MobileInfo* p_MobileInfo  )
{
    if( (p_WcdmaFddInfo->v_MCC < 1000) && ( p_WcdmaFddInfo->v_MNC < 1000 ) )
    {
        /* Validity range for MCC and MNC from 0..999 */
        p_MobileInfo->v_Mcc = p_WcdmaFddInfo->v_MCC;
        p_MobileInfo->v_Mnc = p_WcdmaFddInfo->v_MNC;
        p_MobileInfo->v_CellIdentity =  p_WcdmaFddInfo->v_CellIdentity & 0xFFFF;
        p_MobileInfo->v_RNCId = (p_WcdmaFddInfo->v_CellIdentity & 0xFFFF0000)>>16;

        p_MobileInfo->v_ValidityField  |= (K_LSIMUP_MCC_FIELD|K_LSIMUP_MNC_FIELD| \
            K_LSIMUP_LAC_FIELD|K_LSIMUP_CID_FIELD | K_LSIMUP_RNC_FIELD);


        p_MobileInfo->v_FrequencyInfo.v_DLArfcn = p_WcdmaFddInfo->v_Uarfcn_DL;

        p_MobileInfo->v_FrequencyInfo.v_ULArfcn = p_WcdmaFddInfo->v_Uarfcn_UL;
        p_MobileInfo->v_FrequencyInfo.v_ULArfcnPresent = ( p_WcdmaFddInfo->v_Uarfcn_UL != 0xFFFF );

        p_MobileInfo->s_Primary_Scrambling_Code = ( p_WcdmaFddInfo->v_PrimaryScramblingCode != 0xFFFF ) ? p_WcdmaFddInfo->v_PrimaryScramblingCode : -1;
    }

        INF("lsim3_12HandleWcdmaFddMobileInfo Serving Cell Message:MCC =%u,MNC =%u,CellIdentity =%u,PrimaryScramblingCode =%u,Uarfcn_DL =%u,NumNeighbouringCell =%u",
                     p_WcdmaFddInfo->v_MCC, \
                     p_WcdmaFddInfo->v_MNC, \
                     p_WcdmaFddInfo->v_CellIdentity, \
                     p_WcdmaFddInfo->v_PrimaryScramblingCode, \
                     p_WcdmaFddInfo->v_Uarfcn_DL, \
                     p_WcdmaFddInfo->v_NumNeighbouringCells
                     );

    p_MobileInfo->v_Celltype = K_LSIMUP_WCDMA_CELL;

    if( p_WcdmaFddInfo->v_NumNeighbouringCells > 0 )
    {
        uint8_t vl_i;
        t_lsimup_MeasuredResultsList* pl_MeasuredResultsList=NULL;
        s_gnsSUPL_WcdmaMeasResultsListFDD *p_MsrResultList=NULL;

        p_MobileInfo->v_ValidityField  |= K_LSIMUP_NMR_FIELD;
        p_MobileInfo->v_NeighbouringCells = p_WcdmaFddInfo->v_NumNeighbouringCells;
        p_MobileInfo->p_MeasuredResultsList = (t_lsimup_MeasuredResultsList*) MC_RTK_GET_MEMORY(sizeof(* pl_MeasuredResultsList));
        vl_i = p_MobileInfo->v_NeighbouringCells;

        pl_MeasuredResultsList = p_MobileInfo->p_MeasuredResultsList;
        while( vl_i > 0 )
        {
            t_lsimup_cellMeasuredResultsList    *pl_cellMeasuredResultsList;
            uint8_t vl_j;

            p_MsrResultList = &(p_WcdmaFddInfo->a_WcdmaMeasResultsListFDD[ p_MobileInfo->v_NeighbouringCells - vl_i]);

            /* Frequency info*/

            pl_MeasuredResultsList->v_freqInfo.v_ULArfcn = p_MsrResultList->v_Uarfcn_UL;
            pl_MeasuredResultsList->v_freqInfo.v_DLArfcn = p_MsrResultList->v_Uarfcn_DL;
            pl_MeasuredResultsList->v_freqInfo.v_ULArfcnPresent = ( p_MsrResultList->v_Uarfcn_UL != 0xFFFF );

            pl_MeasuredResultsList->v_UTRACarrierRSS = p_MsrResultList->v_UTRACarrierRSS;

            INF("lsim3_12HandleWcdmaFddMobileInfo Neighbour Cell 1 :Uarfcn_DL =%u,UTRACarrierRSS =%u,NumberOfCellsMeasured =%u",
                         p_MsrResultList->v_Uarfcn_DL, \
                         p_MsrResultList->v_UTRACarrierRSS, \
                         p_MsrResultList->v_NumberOfCellsMeasured
                         );

            /*Manage cellMeasuredResultsList Array*/
            pl_MeasuredResultsList->v_NumberOfCellsInUARFCN = p_MsrResultList->v_NumberOfCellsMeasured;

            if (pl_MeasuredResultsList->v_NumberOfCellsInUARFCN > 0)
            {
                s_gnsSUPL_MeasuredResultFDD *p_MsrResult=NULL;

                pl_MeasuredResultsList->p_cellMeasuredResultsList = (t_lsimup_cellMeasuredResultsList*)\
                        MC_RTK_GET_MEMORY(sizeof(*pl_cellMeasuredResultsList));
                pl_cellMeasuredResultsList =  pl_MeasuredResultsList->p_cellMeasuredResultsList ;
                vl_j = pl_MeasuredResultsList->v_NumberOfCellsInUARFCN;

                while (vl_j > 0)
                {

                    p_MsrResult = &( p_MsrResultList->a_CellMeasuredResultsList[pl_MeasuredResultsList->v_NumberOfCellsInUARFCN - vl_j]);

                    pl_cellMeasuredResultsList->v_CellId = p_MsrResult->v_CellIdentity;

                    pl_cellMeasuredResultsList->v_CellIdPresent = (p_MsrResult->v_CellIdentity != 0xFFFFFFFF );

                    pl_cellMeasuredResultsList->v_ScramblingCode = p_MsrResult->v_PrimaryScramblingCode;

                    pl_cellMeasuredResultsList->v_CPICH_Bitmap = 0;

                    pl_cellMeasuredResultsList->v_CPICH_EcNo = p_MsrResult->v_CPICH_EcNo;

                    pl_cellMeasuredResultsList->v_CPICH_RSCP = p_MsrResult->v_CPICH_RSCP;

                    pl_cellMeasuredResultsList->v_CPICH_PathLoss = p_MsrResult->v_CPICH_PathLoss;

                    if( pl_cellMeasuredResultsList->v_CPICH_EcNo != 0xFF )
                        pl_cellMeasuredResultsList->v_CPICH_Bitmap |= K_LSIMUP_CELL_MEAS_EC_NO_VALID;
                    if( pl_cellMeasuredResultsList->v_CPICH_RSCP != 0xFF )
                        pl_cellMeasuredResultsList->v_CPICH_Bitmap |= K_LSIMUP_CELL_MEAS_RSCP_VALID;
                    if( pl_cellMeasuredResultsList->v_CPICH_PathLoss != 0xFF )
                        pl_cellMeasuredResultsList->v_CPICH_Bitmap |= K_LSIMUP_CELL_MEAS_PATHLOSS_VALID;

                    INF("lsim3_12HandleWcdmaFddMobileInfo Neighbour Cell 2 :CPICH_RSCP =%d,CPICH_PathLoss =%u,CPICH_EcNo =%u, UCID =%u, PSC = %u",
                             p_MsrResult->v_CPICH_RSCP, \
                             p_MsrResult->v_CPICH_PathLoss, \
                             p_MsrResult->v_CPICH_EcNo, \
                             p_MsrResult->v_CellIdentity, \
                             p_MsrResult->v_PrimaryScramblingCode
                             );

                    vl_j--;
                    if (vl_j>0)
                    {
                        pl_cellMeasuredResultsList->p_NextCellMeasuredResultsList = (t_lsimup_cellMeasuredResultsList*)\
                                MC_RTK_GET_MEMORY(sizeof(*pl_cellMeasuredResultsList));
                        pl_cellMeasuredResultsList = pl_cellMeasuredResultsList->p_NextCellMeasuredResultsList;
                    }
                }
            }
            vl_i--;
            if (vl_i>0)
            {
                pl_MeasuredResultsList->p_NextMeasuredResultsList = (t_lsimup_MeasuredResultsList*)\
                        MC_RTK_GET_MEMORY(sizeof(*pl_cellMeasuredResultsList));
                pl_MeasuredResultsList = pl_MeasuredResultsList->p_NextMeasuredResultsList;
            }
        }
    }
}

static void lsim3_13HandleWcdmaTddMobileInfo( s_gnsSUPL_WcdmaTDDCellInfo *p_WcdmaTddInfo , t_lsimup_MobileInfo* p_MobileInfo  )
{
    if( (p_WcdmaTddInfo->v_MCC < 1000) && ( p_WcdmaTddInfo->v_MNC < 1000 ) )
    {
        /* Validity range for MCC and MNC from 0..999 */
        p_MobileInfo->v_Mcc = p_WcdmaTddInfo->v_MCC;
        p_MobileInfo->v_Mnc = p_WcdmaTddInfo->v_MNC;
        p_MobileInfo->v_CellIdentity =  p_WcdmaTddInfo->v_CellIdentity & 0xFFFF;
        p_MobileInfo->v_RNCId = (p_WcdmaTddInfo->v_CellIdentity & 0xFFFF0000)>>16;

        p_MobileInfo->v_ValidityField  |= (K_LSIMUP_MCC_FIELD|K_LSIMUP_MNC_FIELD| \
            K_LSIMUP_LAC_FIELD|K_LSIMUP_CID_FIELD | K_LSIMUP_RNC_FIELD);

        p_MobileInfo->v_FrequencyInfo.v_DLArfcn = p_WcdmaTddInfo->v_Uarfcn_Nt;

        /* For TDSCDMA - Same frequency is used for uplink and downlink */
        p_MobileInfo->v_FrequencyInfo.v_ULArfcn = 0xFFFF;
        p_MobileInfo->v_FrequencyInfo.v_ULArfcnPresent = FALSE;

        p_MobileInfo->s_Primary_Scrambling_Code = ( p_WcdmaTddInfo->v_CellParameterID!= 0xFFFF ) ? p_WcdmaTddInfo->v_CellParameterID : -1;
    }

    p_MobileInfo->v_Celltype = K_LSIMUP_TDSCDMA_CELL;

    if( p_WcdmaTddInfo->v_NumNeighbouringCells > 0 )
    {
        uint8_t vl_i;
        t_lsimup_MeasuredResultsList* pl_MeasuredResultsList=NULL;
        s_gnsSUPL_WcdmaMeasResultsListTDD *p_MsrResultList=NULL;

        p_MobileInfo->v_ValidityField  |= K_LSIMUP_NMR_FIELD;
        p_MobileInfo->v_NeighbouringCells = p_WcdmaTddInfo->v_NumNeighbouringCells;
        p_MobileInfo->p_MeasuredResultsList = (t_lsimup_MeasuredResultsList*) MC_RTK_GET_MEMORY(sizeof(* pl_MeasuredResultsList));
        vl_i = p_MobileInfo->v_NeighbouringCells;
        pl_MeasuredResultsList = p_MobileInfo->p_MeasuredResultsList;

        while( vl_i > 0 )
        {
            t_lsimup_cellMeasuredResultsList    *pl_cellMeasuredResultsList=NULL;
            uint8_t vl_j;

            p_MsrResultList = &(p_WcdmaTddInfo->a_WcdmaMeasResultsListTDD[ p_MobileInfo->v_NeighbouringCells - vl_i]);

            /* Frequency info*/

            pl_MeasuredResultsList->v_freqInfo.v_ULArfcn = 0xFFFF;
            pl_MeasuredResultsList->v_freqInfo.v_DLArfcn = p_MsrResultList->v_Uarfcn_Nt;
            pl_MeasuredResultsList->v_freqInfo.v_ULArfcnPresent = FALSE;

            pl_MeasuredResultsList->v_UTRACarrierRSS = p_MsrResultList->v_UTRACarrierRSS;

            /*Manage cellMeasuredResultsList Array*/
            pl_MeasuredResultsList->v_NumberOfCellsInUARFCN = p_MsrResultList->v_NumberOfCellsMeasured;

            if (pl_MeasuredResultsList->v_NumberOfCellsInUARFCN > 0)
            {
                s_gnsSUPL_MeasuredResultTDD *p_MsrResult=NULL;

                pl_MeasuredResultsList->p_cellMeasuredResultsList = (t_lsimup_cellMeasuredResultsList*)\
                        MC_RTK_GET_MEMORY(sizeof(* pl_cellMeasuredResultsList));
                pl_cellMeasuredResultsList =  pl_MeasuredResultsList->p_cellMeasuredResultsList ;
                vl_j = pl_MeasuredResultsList->v_NumberOfCellsInUARFCN;

                while (vl_j > 0)
                {

                    p_MsrResult = &( p_MsrResultList->a_CellMeasuredResultsList[pl_MeasuredResultsList->v_NumberOfCellsInUARFCN - vl_j]);

                    pl_cellMeasuredResultsList->v_CellId = p_MsrResult->v_CellIdentity;

                    pl_cellMeasuredResultsList->v_CellIdPresent = (p_MsrResult->v_CellIdentity != 0xFFFFFFFF );

                    pl_cellMeasuredResultsList->v_ScramblingCode = p_MsrResult->v_CellParametersID;

                    pl_cellMeasuredResultsList->v_CPICH_Bitmap = 0;

                    pl_cellMeasuredResultsList->v_CPICH_EcNo = p_MsrResult->v_ProposedTGSN;

                    pl_cellMeasuredResultsList->v_CPICH_RSCP = p_MsrResult->v_PrimaryCCPCH_RSCP;

                    pl_cellMeasuredResultsList->v_CPICH_PathLoss = p_MsrResult->v_PathLoss;

                    if( pl_cellMeasuredResultsList->v_CPICH_EcNo != 0xFF )
                        pl_cellMeasuredResultsList->v_CPICH_Bitmap |= K_LSIMUP_CELL_MEAS_EC_NO_VALID;
                    if( pl_cellMeasuredResultsList->v_CPICH_RSCP != 0xFF )
                        pl_cellMeasuredResultsList->v_CPICH_Bitmap |= K_LSIMUP_CELL_MEAS_RSCP_VALID;
                    if( pl_cellMeasuredResultsList->v_CPICH_PathLoss != 0xFF )
                        pl_cellMeasuredResultsList->v_CPICH_Bitmap |= K_LSIMUP_CELL_MEAS_PATHLOSS_VALID;

                    pl_cellMeasuredResultsList->v_TimeslotISCPList.v_NumTimeslotISCP = p_MsrResult->v_TimeslotISCP_Count;

                    if( 0 != p_MsrResult->v_TimeslotISCP_Count )
                    {
                        pl_cellMeasuredResultsList->v_TimeslotISCPList.p_TimeslotISCPList = MC_RTK_GET_MEMORY( p_MsrResult->v_TimeslotISCP_Count * sizeof( uint8_t )  );

                        memcpy( pl_cellMeasuredResultsList->v_TimeslotISCPList.p_TimeslotISCPList ,
                                p_MsrResult->a_TimeslotISCP_List,
                                p_MsrResult->v_TimeslotISCP_Count );
                    }
                    else
                    {
                        pl_cellMeasuredResultsList->v_TimeslotISCPList.p_TimeslotISCPList = NULL;
                    }

                    vl_j--;
                    if (vl_j>0)
                    {
                        pl_cellMeasuredResultsList->p_NextCellMeasuredResultsList = (t_lsimup_cellMeasuredResultsList*)\
                                MC_RTK_GET_MEMORY(sizeof(* pl_cellMeasuredResultsList));
                        pl_cellMeasuredResultsList = pl_cellMeasuredResultsList->p_NextCellMeasuredResultsList;
                    }
                }
            }
            vl_i--;
            if (vl_i>0)
            {
                pl_MeasuredResultsList->p_NextMeasuredResultsList = (t_lsimup_MeasuredResultsList*)\
                        MC_RTK_GET_MEMORY(sizeof(* pl_MeasuredResultsList));
                pl_MeasuredResultsList = pl_MeasuredResultsList->p_NextMeasuredResultsList;
            }
        }
    }
}

void lsim3_14MobileInfoUpdate( s_gnsSUPL_MobileInfo *p_MobileInfo )
{
    t_lsimup_MobileInfoIndM *p_MsgMobileInfoInd = NULL;

    p_MsgMobileInfoInd = (t_lsimup_MobileInfoIndM*) MC_RTK_GET_MEMORY(sizeof(*p_MsgMobileInfoInd ));
    p_MsgMobileInfoInd->s_MobileInfo.v_ValidityField = 0;
    p_MsgMobileInfoInd->s_IsInformationPresent = K_LSIMUP_CELLULAR_NETWORK;

    if( p_MobileInfo->a_IMSI[0] != '\0' )
    {
        memcpy( &p_MsgMobileInfoInd->s_MobileInfo.s_IMSI ,  p_MobileInfo->a_IMSI , sizeof(p_MsgMobileInfoInd->s_MobileInfo.s_IMSI) );
        p_MsgMobileInfoInd->s_MobileInfo.v_ValidityField  |= K_LSIMUP_IMSI_FIELD;
    }
    if( p_MobileInfo->a_MSISDN[0] != '\0' )
    {
        memcpy( &p_MsgMobileInfoInd->s_MobileInfo.s_MSISDN , p_MobileInfo->a_MSISDN , sizeof(p_MsgMobileInfoInd->s_MobileInfo.s_MSISDN) );
        p_MsgMobileInfoInd->s_MobileInfo.v_ValidityField  |= K_LSIMUP_MSISDN_FIELD;
    }

    /* +Common to GSM / WCDMA and TDSCDMA */

    /*- PR LMSqb08971*/
    switch( p_MobileInfo->v_RegistrationState )
    {
        case E_gnsSUPL_REGISTRATION_MOBILE_OFF:   p_MsgMobileInfoInd->s_MobileInfo.v_RegistrationState = K_LSIMUP_NO_REGIST_OFF;break;
        case E_gnsSUPL_REGISTRATION_NO_CARRIER:   p_MsgMobileInfoInd->s_MobileInfo.v_RegistrationState = K_LSIMUP_NO_CARRIER;break;
        case E_gnsSUPL_REGISTRATION_EMERGENCY_IDLE : p_MsgMobileInfoInd->s_MobileInfo.v_RegistrationState = K_LSIMUP_EMERGENCY_IDLE;break;
        case E_gnsSUPL_REGISTRATION_SERVICE_ALLOWED: p_MsgMobileInfoInd->s_MobileInfo.v_RegistrationState = K_LSIMUP_IDLE_UPDATE;break;
    }

    switch( p_MobileInfo->v_CellInfoType )
    {
        case E_gnsSUPL_GSM_CELL: lsim3_11HandleGsmMobileInfo(&(p_MobileInfo->v_CellInfo.v_GsmCellInfo),&p_MsgMobileInfoInd->s_MobileInfo);break;
        case E_gnsSUPL_WCDMA_FDD_CELL:lsim3_12HandleWcdmaFddMobileInfo(&(p_MobileInfo->v_CellInfo.v_WcdmaFDDCellInfo),&p_MsgMobileInfoInd->s_MobileInfo);break;
        case E_gnsSUPL_WCDMA_TDD_CELL:lsim3_13HandleWcdmaTddMobileInfo(&(p_MobileInfo->v_CellInfo.v_WcdmaTDDCellInfo),&p_MsgMobileInfoInd->s_MobileInfo);break;
    }
    /*Send the info */
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_MOBILE_INFO_IND,(t_MsgHeader*) p_MsgMobileInfoInd);

}

void lsim3_15WirelessNetworkInfoUpdate( s_gnsWIRELESS_NetworkInfo *p_WirelessNetworkInfo )
{
    t_lsimup_MobileInfoIndM *p_MsgMobileInfoInd = NULL;

    if(p_WirelessNetworkInfo != NULL)
    {

        p_MsgMobileInfoInd = (t_lsimup_MobileInfoIndM*) MC_RTK_GET_MEMORY(sizeof(*p_MsgMobileInfoInd ));
        p_MsgMobileInfoInd->s_MobileInfo.v_ValidityField = 0;
        //The below flag is used because at the boot up mobile Info is called from the modem and we crash. To avoid this set the flag to TRUE so that when we know
        //Any of the Wireless Network is present only then accept wireless network info
        p_MsgMobileInfoInd->s_IsInformationPresent = K_LSIMUP_WIRELESS_NETWORK;

        memcpy(&(p_MsgMobileInfoInd->s_WirelessNetworkInfo),p_WirelessNetworkInfo, sizeof(p_MsgMobileInfoInd->s_WirelessNetworkInfo));

        /*Send the info */
        if(p_MsgMobileInfoInd!= NULL)
        {
            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_MOBILE_INFO_IND,(t_MsgHeader*) p_MsgMobileInfoInd);
        }

    }
}


void lsim3_15EstablishBearerReq()
{
    u_gnsSUPL_MsgData   v_MsgData;


    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    GNS_ExecuteSuplCallback( E_gnsSUPL_ESTABLISH_BEARER_REQ, 0 , &v_MsgData );

}

void lsim3_16EstablishBearerCnf()
{
    t_lsimup_EstablishBearerCnfM * p_MsgEstablishBearerCnf = NULL;

    /*Send Confirmation to CGPS*/
    p_MsgEstablishBearerCnf =  (t_lsimup_EstablishBearerCnfM *)\
                               MC_RTK_GET_MEMORY(sizeof(* p_MsgEstablishBearerCnf));
    p_MsgEstablishBearerCnf->v_SessionHandle =  0;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_ESTABLISH_BEARER_CNF,(t_MsgHeader*) p_MsgEstablishBearerCnf);

}

void lsim3_17EstablishBearerErr()
{

   t_lsimup_EstablishBearerErrM * p_MsgEstablishBearerErr = NULL;

   /*Send Confirmation to CGPS*/
   p_MsgEstablishBearerErr =  (t_lsimup_EstablishBearerErrM *)\
                              MC_RTK_GET_MEMORY(sizeof(* p_MsgEstablishBearerErr));
   p_MsgEstablishBearerErr->v_SessionHandle =  0;
   p_MsgEstablishBearerErr->v_ErrorType = K_LSIMUP_ERROR_BEARER_IMPOSSIBLE_TO_CONFIGURE;

   MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_ESTABLISH_BEARER_ERR,(t_MsgHeader*) p_MsgEstablishBearerErr);
}

void lsim3_18CloseBearerReq()
{
    u_gnsSUPL_MsgData   v_MsgData;


    memset( &v_MsgData , 0 , sizeof(v_MsgData) );

    GNS_ExecuteSuplCallback( E_gnsSUPL_CLOSE_BEARER_REQ, 0 , &v_MsgData );

}

void lsim3_19CloseBearerCnf()
{
    t_lsimup_CloseBearerCnfM* p_MsgCloseBearerCnf=NULL;

    p_MsgCloseBearerCnf =  (t_lsimup_CloseBearerCnfM *)\
                           MC_RTK_GET_MEMORY(sizeof(* p_MsgCloseBearerCnf));
    p_MsgCloseBearerCnf->v_SessionHandle =  0;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_CLOSE_BEARER_CNF,(t_MsgHeader*) p_MsgCloseBearerCnf);

}

void lsim3_20CloseBearerErr()
{
    t_lsimup_CloseBearerErrM* p_MsgCloseBearerErr=NULL;

    p_MsgCloseBearerErr =  (t_lsimup_CloseBearerErrM *)\
                           MC_RTK_GET_MEMORY(sizeof(* p_MsgCloseBearerErr));
    p_MsgCloseBearerErr->v_SessionHandle =  0;
    p_MsgCloseBearerErr->v_ErrorType = K_LSIMUP_ERROR_BEARER_IMPOSSIBLE_TO_STOP;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_CLOSE_BEARER_ERR ,(t_MsgHeader*) p_MsgCloseBearerErr);

}

void lsim3_21CloseBearerInd()
{
    t_lsimup_CloseBearerIndM* p_MsgCloseBearerInd=NULL;

    p_MsgCloseBearerInd =  (t_lsimup_CloseBearerIndM *)\
                           MC_RTK_GET_MEMORY(sizeof(* p_MsgCloseBearerInd));
    p_MsgCloseBearerInd->v_SessionHandle =  0;

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMUP,0,PROCESS_CGPS,0,CGPS_LSIMUP_CLOSE_BEARER_IND,(t_MsgHeader*) p_MsgCloseBearerInd);

}


#endif /* #define AGPS_UP_FTR */

#undef LSIM3UPGNS_C

