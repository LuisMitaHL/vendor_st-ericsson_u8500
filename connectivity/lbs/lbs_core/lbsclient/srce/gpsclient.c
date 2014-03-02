/*
* Copyright (C) ST-Ericsson 2009
*
* gpsclient.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <math.h>
#include <unistd.h>



#include <time.h>
#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
#ifdef GPSCLIENT_FORCE_IMSI
#include <cutils/properties.h>
#endif
#endif
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "agps_client.h"
#include "clientutils.h"
#include "gpsclient.h"
#ifdef AGPS_UP_FTR
#ifdef AGPS_UNIT_TEST_FTR
#include "TlsHandler_stub.h"
#else
#include "TlsHandler.h"
#endif
#endif
#include "sbeesocketinterface.h"
#include "sbeeclient.h"
#include "BearerHandler.h"
#include "HttpHandler.h"




#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "gpsclient"

static t_gpsClient_Callbacks     *gpsCallbacks;
#ifdef AGPS_UP_FTR
static t_gpsClient_agpsCallbacks    *agpsCallbacks;
#endif /*AGPS_UP_FTR*/


#ifdef AGPS_ANDR_ECLAIR_FTR
static t_gpsClient_NiCallbacks  *gpsNiCallbacks;
#endif /*  AGPS_ANDR_ECLAIR_FTR */

#ifdef AGPS_ANDR_GINGER_FTR
static t_gpsClient_RilCallbacks   *agpsRilCallbacks;
#endif /*  AGPS_ANDR_GINGER_FTR */

uint32_t PositionMode = 0;

t_clientUtils_GpsState gpsState;

t_agps_SlpAddress slpAddress;

/* This structure is filled in case GPS thread disconnects  */
t_agps_PeriodicFixCommand*  vg_Periodic_fix_dataparam;

uint32_t clientCap
#ifdef AGPS_ANDR_CUPCAKE_FTR
 = K_AGPS_REGISTER_FIX_CLIENT | K_AGPS_REGISTER_DATA_CONN_CLIENT |

#elif AGPS_ANDR_DONUT_FTR
 = K_AGPS_REGISTER_FIX_CLIENT | K_AGPS_REGISTER_DATA_CONN_CLIENT | K_AGPS_REGISTER_BEARER_CLIENT  |

#elif AGPS_ANDR_ECLAIR_FTR
 = K_AGPS_REGISTER_FIX_CLIENT | K_AGPS_REGISTER_DATA_CONN_CLIENT | K_AGPS_REGISTER_BEARER_CLIENT  \
 | K_AGPS_REGISTER_NOTIFICATION_CLIENT | K_AGPS_REGISTER_SBEE_CLIENT | K_AGPS_REGISTER_NI_CLIENT |

#ifdef AGPS_ANDR_GINGER_FTR

#ifndef MODEM_SUPPORT_CELLINFO_FTR
K_AGPS_REGISTER_MOBILE_INFO_CLIENT | 
#endif  /*! MODEM_SUPPORT_CELLINFO_FTR */

#ifndef MODEM_SUPPORT_WIRELESSINFO_FTR
K_AGPS_REGISTER_WIRELESS_NETWORK_CLIENT |
#endif  /*! MODEM_SUPPORT_WIRELESSINFO_FTR*/

#endif /* AGPS_ANDR_GINGER_FTR*/
#endif /* AGPS_ANDR_CUPCAKE_FTR */

0;

void gpsClient_GpsReconnectHandler( void );

/* ++ LMSqc36741 */
/*****************************************************************************
* gpsClient_Mobile_Info :
*****************************************************************************/
int gpsClient_Mobile_Info( int mcc , int mnc , int cellId , int v_Lac , int celltype , const char* imsi , char* msisdn )
/* -- LMSqc36741 */
{
    t_agps_MobileInfoInd v_MobileInfoInd;
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_SUPL_MOBILE_INFO_IND;

    v_MobileInfoInd.v_MCC = mcc;
    v_MobileInfoInd.v_MNC = mnc;
    v_MobileInfoInd.v_CellIdentity = cellId;
    v_MobileInfoInd.v_CellType = celltype;
    /* ++ LMSqc36741 */
    v_MobileInfoInd.v_Lac = v_Lac;
    /* -- LMSqc36741 */

    if( NULL != imsi )
    {
        memcpy( v_MobileInfoInd.a_IMSI , imsi , strlen(imsi) + 1 );
    }
    else
    {
        v_MobileInfoInd.a_IMSI[0] = '\0';
    }
    if( NULL != msisdn )
    {
        memcpy( v_MobileInfoInd.a_MSISDN, msisdn , strlen(msisdn) + 1 );
    }
    else
    {
        v_MobileInfoInd.a_MSISDN[0] = '\0';
    }

    packet.pData = (void *)&v_MobileInfoInd;
    packet.length = sizeof(v_MobileInfoInd);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("IMSI : %s, MSISDN : %s" ,v_MobileInfoInd.a_IMSI , v_MobileInfoInd.a_MSISDN ));
    DEBUG_LOG_PRINT_LEV2(("gpsClient_Mobile_Info(): Sending SOCKET_AGPS_SUPL_MOBILE_INFO_IND to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_WLAN_Info : Only MAC is the mandatory parameter and all other parameters are optional. 
* If the caller doesnot intend to pass optional parameter then it can call with NULL pointer
*****************************************************************************/
int gpsClient_WLAN_Info(t_gpsClient_WlanInfo *v_WLANInfo)
{
    t_agps_WLANInfoInd v_WLANInfoInd;
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_WIRELESS_NETWORK_INFO_IND;

    if(v_WLANInfo != NULL)
    {

        v_WLANInfoInd.v_WLAN_AP_MS_Addr             = v_WLANInfo->v_WLAN_AP_MS_Addr;
        v_WLANInfoInd.v_WLAN_AP_LS_Addr             = v_WLANInfo->v_WLAN_AP_LS_Addr;
        if(v_WLANInfo->v_WLAN_OptionalInfo != NULL)
        {
            v_WLANInfoInd.v_WLAN_AP_TransmitPower       = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_TransmitPower;
            v_WLANInfoInd.v_WLAN_AP_AntennaGain         = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_AntennaGain;
            v_WLANInfoInd.v_WLAN_AP_SNR                 = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_SNR;
            v_WLANInfoInd.v_WLAN_AP_DeviceType          = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_DeviceType;
            v_WLANInfoInd.v_WLAN_AP_SignalStrength      = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_SignalStrength;
            v_WLANInfoInd.v_WLAN_AP_Channel             = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_Channel;
            v_WLANInfoInd.v_WLAN_AP_RTDValue            = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_RTDValue;
            v_WLANInfoInd.v_WLAN_AP_RTDUnits            = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_RTDUnits;
            v_WLANInfoInd.v_WLAN_AP_Accuracy            = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_Accuracy;
            v_WLANInfoInd.v_WLAN_AP_SETTransmitPower    = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_SETTransmitPower;
            v_WLANInfoInd.v_WLAN_AP_SETAntennaGain      = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_SETAntennaGain;
            v_WLANInfoInd.v_WLAN_AP_SETSNR              = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_SETSNR;
            v_WLANInfoInd.v_WLAN_AP_SETSignalStrength   = v_WLANInfo->v_WLAN_OptionalInfo->v_WLAN_AP_SETSignalStrength;
        }
    }


    packet.pData = (void *)&v_WLANInfoInd;
    packet.length = sizeof(v_WLANInfoInd);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("WLAN AP Addr_MS_Field : %d, Addr_LS_Field : %x" ,v_WLANInfoInd.v_WLAN_AP_MS_Addr , v_WLANInfoInd.v_WLAN_AP_LS_Addr ));
    DEBUG_LOG_PRINT_LEV2(("gpsClient_Mobile_Info(): Sending SOCKET_AGPS_WLAN_INFO_IND to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return GPSCLIENT_NO_ERROR;
}

/*****************************************************************************
* gpsClient_Set_Configuration :  Sets gps configuration settings
*****************************************************************************/
int gpsClient_Set_Configuration(
            int configMaskValid, int configMask,
            int config2Valid, unsigned char sensMod, unsigned char sensTTFF,
            unsigned char powerPref, unsigned char coldStart)
{
    t_agps_Command command;
    t_agps_Configuration gpsConfiguration;
    t_agps_Packet packet;
    DEBUG_LOG_PRINT_LEV2(("Setting configuration\n"));

    command.type = SOCKET_AGPS_SET_USER_CONFIG;

    gpsConfiguration.configMaskValid = configMaskValid;
    gpsConfiguration.configMask = configMask;
    gpsConfiguration.config2Valid = config2Valid;
    gpsConfiguration.sensMod = sensMod;
    gpsConfiguration.sensTTFF = sensTTFF;
    gpsConfiguration.powerPref = powerPref;
    gpsConfiguration.coldStart = coldStart;
    gpsConfiguration.slpAddress.portNumber = 7275;
    gpsConfiguration.slpAddress.type = K_AGPS_SLP_ADDRESS_FQDN;
    memcpy(gpsConfiguration.slpAddress.address, "www.spirent-lcs.com", strlen("www.spirent-lcs.com") + 1 );

    packet.pData = (void *) (&gpsConfiguration);
    packet.length = sizeof( gpsConfiguration );
    command.data = (void *) ( &packet );

    //waiting for the socket connection
    usleep( SLEEP_TIME_MICRO_SECONDS );

    DEBUG_LOG_PRINT_LEV2(("SetConfiguration: Sending SOCKET_AGPS_SET_USER_CONFIG to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_Set_PlatformConfiguration : Sets gps Platformconfiguration settings
*****************************************************************************/
int gpsClient_Set_PlatformConfiguration(
            int Config1MaskValid, int Config1Mask,int Config2Mask,
            int MsaMandatoryMask, int MsbMandatoryMask,
            uint32_t SleepTimerDuration,uint32_t ComaTimerDuration,uint32_t DeepComaTimerDuration)
{
    t_agps_Command command;
    t_agps_PlatformConfiguration gpsPlatformConfiguration;
    t_agps_Packet packet;
    DEBUG_LOG_PRINT_LEV2(("Setting Platform configuration\n"));

    command.type = SOCKET_AGPS_SET_PLATFORM_CONFIG;

    gpsPlatformConfiguration.v_Config1MaskValid = Config1MaskValid;
    gpsPlatformConfiguration.v_Config1Mask = Config1Mask;
    gpsPlatformConfiguration.v_Config2Mask = Config2Mask;
    gpsPlatformConfiguration.v_MsaMandatoryMask = MsaMandatoryMask;
    gpsPlatformConfiguration.v_MsbMandatoryMask = MsbMandatoryMask;
    gpsPlatformConfiguration.v_SleepTimerDuration= SleepTimerDuration;
    gpsPlatformConfiguration.v_ComaTimerDuration= ComaTimerDuration;
    gpsPlatformConfiguration.v_DeepComaDuration= DeepComaTimerDuration;

    packet.pData = (void *) (&gpsPlatformConfiguration);
    packet.length = sizeof( gpsPlatformConfiguration );
    command.data = (void *) ( &packet );

    //waiting for the socket connection
    usleep( SLEEP_TIME_MICRO_SECONDS );

    DEBUG_LOG_PRINT_LEV2(("SetConfiguration: Sending SOCKET_AGPS_SET_PLATFORM_CONFIG to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_Set_LoggingConfiguration : Sets gps logging configuration settings
*****************************************************************************/
int gpsClient_Set_LoggingConfiguration(
            int ModuleType, int LogType,int LogOpt, int LogFileSize)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_LoggingConfiguration gpsLoggingConfiguration;

    DEBUG_LOG_PRINT_LEV2(("Setting configuration\n"));

    command.type = SOCKET_AGPS_SET_LOGGING_CONFIG;

    gpsLoggingConfiguration.v_Moduletype= ModuleType;
    gpsLoggingConfiguration.v_LogType= LogType;
    gpsLoggingConfiguration.v_LogOpt= LogOpt;
    gpsLoggingConfiguration.v_LogFileSize= LogFileSize;

    packet.pData = (void *) (&gpsLoggingConfiguration);
    packet.length = sizeof( gpsLoggingConfiguration );
    command.data = (void *) ( &packet );

    //waiting for the socket connection
    usleep( SLEEP_TIME_MICRO_SECONDS );

    DEBUG_LOG_PRINT_LEV2(("SetConfiguration: Sending SOCKET_AGPS_SET_LOGGING_CONFIG to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_GpsStateEventHandler : Handles all the events coming to gpsClient
*****************************************************************************/
void gpsClient_GpsStateEventHandler(t_agps_Command command)
{
    DEBUG_LOG_PRINT_LEV2(("gpsClient_GpsStateEventHandler : revcd commands %d\n", command.type));
    switch( command.type )
    {
        case SOCKET_AGPS_NI_NOTIFICATION :
        {
            DEBUG_LOG_PRINT_LEV2(("Received is NI Notification callback, sending it for processing\n"));
            if((gpsCallbacks != NULL) && (gpsCallbacks->ni_cb!=NULL))
            {
                uint8_t ErrorType;
                memcpy(&ErrorType, (void*)command.data->pData, sizeof(uint8_t));
                gpsCallbacks->ni_cb(ErrorType);
                DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
            }
        }
        break;
        case SOCKET_AGPS_NAVDATA:
        {
            DEBUG_LOG_PRINT_LEV2(("Received is NavData callback, sending it for processing\n"));
            if((gpsCallbacks != NULL) && (gpsCallbacks->nav_cb!=NULL ))
            {
                t_gpsClient_NavData NavData;
                /*Note here the two structure are same so memcpy is enough*/
                memcpy(&NavData, (t_agps_CStructNavData *)((t_agps_Packet *)(command.data))->pData,
                sizeof(NavData));
                gpsCallbacks->nav_cb(&NavData );
            }
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;

#ifdef METRICO_TEST_TOOL

        case SOCKET_AGPS_DEBUG_LOG_RESP:
        {
            t_gpsClient_NmeaData NmeaData;
            t_agps_Packet *p_Nmea = (t_agps_Packet *)command.data;
            /*Note here the two structure are same so memcpy is enough*/
            NmeaData.length = p_Nmea->length;
            NmeaData.pData =  p_Nmea->pData;
            DEBUG_LOG_PRINT_LEV2(("Received is Nmea Data Debug Log Resp\n"));
            gpsCallbacks->nmea_cb( &NmeaData );
            // gpsClient_ProcessNmeaCallback( command.data );
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;
#endif
        case SOCKET_AGPS_STATUS:
        {
            DEBUG_LOG_PRINT_LEV2(("Received data is an status message, sending it for processing\n"));
            clientUtils_ProcessStatus( &gpsState, (e_agps_Status*)((t_agps_Packet *)(command.data))->pData );
            DEBUG_LOG_PRINT_LEV2(("Status message processed\n"));
        }
        break;
#ifdef AGPS_UP_FTR
        case SOCKET_AGPS_SUPL_TCPIP_CONNECT_REQ:
        {
            t_agps_TcpIpConnectReq *tcpipConnectReq = (t_agps_TcpIpConnectReq *)((t_agps_Packet *)(command.data))->pData;
            t_TlsServer v_Server;
            DEBUG_LOG_PRINT_LEV2(("Received data is Tcp/Ip connect request\n"));
            v_Server.v_Port = tcpipConnectReq->slpAddress.portNumber;
            switch( tcpipConnectReq->slpAddress.type )
            {
                case K_AGPS_SLP_ADDRESS_FQDN:
                    v_Server.v_Type = K_TLS_FQDN_ADDRESS;
                    break;
                case K_AGPS_SLP_ADDRESS_IPV4:
                    v_Server.v_Type = K_TLS_IPV4_ADDRESS;
                    break;
                default:
                    /* Not handled : K_AGPS_SLP_ADDRESS_DEFAULT &  K_AGPS_SLP_ADDRESS_IPV6*/
                    break;
            }

            memcpy( v_Server.a_Addr , tcpipConnectReq->slpAddress.address , 256 );
            if( tcpipConnectReq->v_IsTls )
            {
                TlsConnect( tcpipConnectReq->v_ConnectionHandle , &v_Server );
            }
            else
            {
                TlsConnectUnsecure( tcpipConnectReq->v_ConnectionHandle , &v_Server );
            }

            DEBUG_LOG_PRINT_LEV2(("Tcp/Ip connect request processed\n"));
        }
        break;
        case SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_REQ:
        {
            t_agps_TcpIpDisconnectReq *tcpipDisconnectRequest = (t_agps_TcpIpDisconnectReq *)((t_agps_Packet *)(command.data))->pData;
            DEBUG_LOG_PRINT_LEV2(("Received data is Tcp/Ip disconnect request\n"));
            TlsDisconnect(tcpipDisconnectRequest->v_ConnectionHandle);
            DEBUG_LOG_PRINT_LEV2(("Tcp/Ip disconnect request processed\n"));
        }
        break;

        case SOCKET_AGPS_SUPL_SEND_DATA_REQ:
        {
            t_agps_SuplData *p_SuplData = (t_agps_SuplData *)((t_agps_Packet *)(command.data))->pData;

            DEBUG_LOG_PRINT_LEV2(("Received data is Supl send data request\n"));
            TlsWrite(p_SuplData->v_ConnectionHandle,(char *)p_SuplData->v_Data.p_Pointer,p_SuplData->v_Data.v_Length);
            DEBUG_LOG_PRINT_LEV2(("Supl send data request processed\n"));
        }
        break;
#endif /*AGPS_UP_FTR*/

#if defined( AGPS_ANDR_DONUT_FTR ) || defined( AGPS_ANDR_ECLAIR_FTR )
#ifdef AGPS_UP_FTR
/* For all Android after cupcake, the following events are handled here itself */
        case SOCKET_AGPS_START_BEARER_REQ:
        {
            ProcessOpenBearerConnectionRequest( AGPS_BEARER_REQUESTOR_SUPL );
            DEBUG_LOG_PRINT_LEV2(("Bearer request processed\n"));
        }
        break;
        case SOCKET_AGPS_CLOSE_BEARER_REQ:
        {
            ProcessCloseBearerConnectionRequest( AGPS_BEARER_REQUESTOR_SUPL );

            DEBUG_LOG_PRINT_LEV2(("Close bearer request processed\n"));
        }
        break;
#endif /*AGPS_UP_FTR*/

        case SOCKET_AGPS_NMEA_DATA :
        {
            t_gpsClient_NmeaData NmeaData;
            t_agps_Packet *p_Nmea = (t_agps_Packet *)command.data;
            /*Note here the two structure are same so memcpy is enough*/
           NmeaData.length = p_Nmea->length;
           NmeaData.pData =  p_Nmea->pData;

           gpsCallbacks->nmea_cb( &NmeaData );
        }
        break;

#ifdef AGPS_ANDR_ECLAIR_FTR
        case SOCKET_AGPS_NOTIFICATION:
        {
            DEBUG_LOG_PRINT_LEV2(("Received data is a Notification callback, sending it for processing\n"));

            if((gpsNiCallbacks != NULL) && (gpsNiCallbacks->notify_cb!=NULL ))
            {
                t_gpsClient_NotifyData NotifyData;
                /*Note here the two structure are same so memcpy is enough*/
                memcpy(&NotifyData, (t_agps_NotificationInd *)((t_agps_Packet *)(command.data))->pData,
                sizeof(NotifyData));
                gpsNiCallbacks->notify_cb(&NotifyData );
               }


            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;
#endif   /* AGPS_ANDR_ECLAIR_FTR */
#endif   /* AGPS_ANDR_ECLAIR_FTR ||AGPS_ANDR_DONUT_FTR */
        case SOCKET_AGPS_MOBILE_INFO_REQ:
        {
#ifdef AGPS_ANDR_GINGER_FTR
            DEBUG_LOG_PRINT_LEV2(("Received data is a Mobile Info Request, sending it for processing\n"));

            if( NULL != agpsRilCallbacks)
            {
                /*Mobile Information request*/
                if(agpsRilCallbacks->ril_cb!=NULL )
                {
                    agpsRilCallbacks->ril_cb( GPSCLIENT_NETOWRK_INFO_TYPE_CELLULAR );
                }
                else
                {
                    DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks->rilcb is null pointer.\n"));
                }
                /*IMSI information request*/
                if(agpsRilCallbacks->ril_setid_cb != NULL)
                {
                    agpsRilCallbacks->ril_setid_cb();
                }
                else
                {
                    DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks->ril_setid_cb is null pointer.\n"));
                }
            }
            else
            {
                DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks is a null pointer\n"));
            }
#endif /* AGPS_ANDR_GINGER_FTR */
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;

        case SOCKET_AGPS_WIRELESS_NETWORK_INFO_REQ:
        {
#ifdef AGPS_ANDR_GINGER_FTR
             DEBUG_LOG_PRINT_LEV2(("Received data is a WLAN Info Request, sending it for processing\n"));

             if( NULL != agpsRilCallbacks)
             {
               /*WLAN MAC Address request to the RIL layer*/
                  if(agpsRilCallbacks->ril_cb!=NULL )
                  {
                       DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks->rilcb is Called.\n"));
                       agpsRilCallbacks->ril_cb( GPSCLIENT_NETOWRK_INFO_TYPE_WLAN );
                  }
                  else
                  {
                       DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks->rilcb is null pointer.\n"));
                  }
             }
             else
             {
                  DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks is a null pointer\n"));
             }
#endif /* AGPS_ANDR_GINGER_FTR */
        }
        break;

        case SOCKET_AGPS_PRODUCTION_TEST_DATA:
        {
            t_agps_ProductionTestData *p_Data;

            p_Data = (t_agps_ProductionTestData *)((t_agps_Packet *)(command.data))->pData;

            DEBUG_LOG_PRINT_LEV2(("Received SOCKET_AGPS_PRODUCTION_TEST_DATA : CheckLimit <%u> CNo <%u>\n" , p_Data->v_CheckLimit , p_Data->v_CNoValue));
        }
        break;
#ifdef AGPS_SBEE_FTR
        case SOCKET_AGPS_SBEE_NOTIFY_CONNECTION:
        {
            DEBUG_LOG_PRINT_LEV2(("Received data is a Notification to indicate the user about SBEE connection\n"));
            sbeeClient_ProcessNotifySbeeConnection( (t_sbee_si_UserNotificationParams*)((t_agps_Packet *)(command.data))->pData );
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;

        case SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION:
        {
            DEBUG_LOG_PRINT_LEV2(("Received data is a data connection request from SBEE\n"));
            sbeeClient_ProcessRequestSbeeDataConnection( (t_sbee_si_DataConnectionRequestParams *)((t_agps_Packet *)(command.data))->pData );
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;

        case SOCKET_AGPS_SBEE_REQUEST_DATA_DISCONNECT:
        {
            DEBUG_LOG_PRINT_LEV2(("Received data is a data disconnect request from SBEE\n"));
            sbeeClient_ProcessSbeeCloseDataConnection( );
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;
        case SOCKET_AGPS_SBEE_DATA:
        {
            DEBUG_LOG_PRINT_LEV2(("Received data is an SBEE data blcok\n"));
            sbeeClient_ProcessSbeeData( (t_sbee_si_SbeeDataParams *)((t_agps_Packet *)(command.data))->pData );
            DEBUG_LOG_PRINT_LEV2(("Callback processed\n"));
        }
        break;
#endif /*AGPS_SBEE_FTR*/
        default:
            DEBUG_LOG_PRINT_LEV2(("Command not supported\n"));
        break;

    }

}


#ifdef AGPS_UP_FTR
/*****************************************************************************
* gpsClient_UpdateFdList :
*****************************************************************************/
int gpsClient_UpdateFdList( fd_set *p_ReadFds , fd_set *p_WriteFds ,fd_set *p_ErrorFds )
{
    int maxReadFd,maxWriteFd,maxErrorFd;
    maxReadFd = TlsPopulateReadSelect(p_ReadFds);
    maxWriteFd = TlsPopulateWriteSelect(p_WriteFds);
    maxErrorFd = TlsPopulateErrorSelect(p_ErrorFds);

    return ( maxReadFd > maxWriteFd ) ? maxReadFd : maxWriteFd;
}


/*****************************************************************************
* gpsClient_HandleEventList :
*****************************************************************************/
int gpsClient_HandleEventList( fd_set *p_ReadFds , fd_set *p_WriteFds ,fd_set *p_ErrorFds )
{
    int vl_Count = 0;
    vl_Count += TlsHandleWriteEvent(p_WriteFds);
    vl_Count += TlsHandleReadEvent(p_ReadFds);
    vl_Count += TlsHandleErrorEvent(p_ErrorFds);
    return vl_Count;
}
#endif /*AGPS_UP_FTR*/


/*****************************************************************************
* gpsClient_ServiceStart :
*****************************************************************************/
int gpsClient_ServiceStart()
{
    int vl_status=0;

    t_agps_Command command;

 // Now we check if we can do system.get property here
 #ifdef GPSCLIENT_FORCE_IMSI
    {
        char imsi[64];

        DEBUG_LOG_PRINT_LEV2(("Calling property_get : ril.IMSI"));
        property_get("ril.IMSI" , imsi , "" );
        DEBUG_LOG_PRINT_LEV2(("Return from property_get : ril.IMSI - <%s>" , imsi));

        /* ++ LMSqc36741 */
        /* gpsClient_Mobile_Info(-1, -1, -1, -1, imsi, NULL); */
        gpsClient_Mobile_Info(-1, -1, -1, -1, -1 , imsi, NULL);
        /* -- LMSqc36741 */
    }
#endif

    command.type = SOCKET_AGPS_SERVICE_START;

    DEBUG_LOG_PRINT_LEV2(("Entering gpsClient_ServiceStart()\n"));

    DEBUG_LOG_PRINT_LEV2(("gpsClient_Start(): Sending SOCKET_AGPS_SERVICE_START to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );

    vl_status=clientUtils_WaitForStatusFromServer(&gpsState);
    if(vl_status!=SOCKET_AGPS_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("StarusFrom server%d\n",vl_status));
        return vl_status;
    }


    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_ServiceStart2 :
*****************************************************************************/
int gpsClient_ServiceStart2(t_gpsClient_ApplicationIDInfo *p_AppInfo)
{
    int vl_status=0;

    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_ApplicationIDInfo      application_id_info;

 // Now we check if we can do system.get property here
 #ifdef GPSCLIENT_FORCE_IMSI
    {
        char imsi[64];

        DEBUG_LOG_PRINT_LEV2(("Calling property_get : ril.IMSI"));
        property_get("ril.IMSI" , imsi , "" );
        DEBUG_LOG_PRINT_LEV2(("Return from property_get : ril.IMSI - <%s>" , imsi));

        /* ++ LMSqc36741 */
        gpsClient_Mobile_Info(-1, -1, -1, -1, -1 , imsi, NULL);
        /* -- LMSqc36741 */
    }
#endif

    command.type = SOCKET_AGPS_APPLICATIONID_START;

    memset( &application_id_info, 0, sizeof(t_agps_ApplicationIDInfo));

    if(p_AppInfo != NULL)
    {
        if((strlen((const char *)p_AppInfo->v_ApplicationProvider)<GPSCLIENT_MAX_APP_PROVIDER_LEN)&&
           (strlen((const char *)p_AppInfo->v_ApplicationName)<GPSCLIENT_MAX_APP_NAME_LEN)&&
           (strlen((const char *)p_AppInfo->v_ApplicationVersion)<GPSCLIENT_MAX_APP_VERSION_LEN))
        {

           memcpy(&application_id_info, p_AppInfo, sizeof(t_gpsClient_ApplicationIDInfo));
           DEBUG_LOG_PRINT_LEV2(("GPS CLient Application ID: Application Provider     - <%s>" , application_id_info.v_ApplicationProvider));
           DEBUG_LOG_PRINT_LEV2(("GPS CLient Application ID: Application Name         - <%s>" , application_id_info.v_ApplicationName));
           DEBUG_LOG_PRINT_LEV2(("GPS CLient Application ID: Application version      - <%s>" , application_id_info.v_ApplicationVersion));
           DEBUG_LOG_PRINT_LEV2(("GPS CLient Application ID: Application Info Present - <%d>" , application_id_info.v_ApplicationIDInfoPresent));
        }
       else
       {
           DEBUG_LOG_PRINT_LEV2(("gpsClient_ServiceStart2: The App info passed string not NULL terminated"));
           return GPSCLIENT_INVALID_PARAMETER;
       }
    }
    packet.pData = (void *) (&application_id_info);
    packet.length = sizeof( application_id_info );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("Entering gpsClient_ApplicationIdStart()\n"));

    DEBUG_LOG_PRINT_LEV2(("gpsClient_ApplicationIdStart(): Sending SOCKET_AGPS_SERVICE_START to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );

    vl_status=clientUtils_WaitForStatusFromServer(&gpsState);
    if(vl_status!=SOCKET_AGPS_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("StarusFrom server%d\n",vl_status));
        return vl_status;
    }


    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_RegisterMask :
*****************************************************************************/
void gpsClient_RegisterMask()
{
    t_agps_RegisterClientMask mask;
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_REGISTER_CLIENT;

    mask = clientCap;

    DEBUG_LOG_PRINT_LEV2(("Entering gpsClient_RegisterMask()\n"));
    packet.length = sizeof(mask);
    packet.pData = &mask;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_Start(): Sending SOCKET_AGPS_REGISTER_CLIENT to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );
#ifdef AGPS_INJECT_MOBILE_INFO_TEST
    /* this is specific to the ULTS in WCDMA mode*/
    DEBUG_LOG_PRINT_LEV2(("Test Mode : Sending Mobile Info"));
    gpsClient_Mobile_Info( 1 , 1 , 1 , 1 , 1 , "001010123456789"  , NULL );
#endif
}


/*****************************************************************************
* gpsClient_ProductionTestStart :
*****************************************************************************/
void gpsClient_ProductionTestStart()
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_CnoCriteria v_CnoCriteria;

    command.type = SOCKET_AGPS_PRODUCTION_TEST_START;

    DEBUG_LOG_PRINT_LEV2(("Entering gpsClient_ProductionTestStart()\n"));

    memset( &v_CnoCriteria , 0 , sizeof(v_CnoCriteria) );
    v_CnoCriteria.v_E9L = 1;

    packet.length = sizeof(v_CnoCriteria);
    packet.pData = &v_CnoCriteria;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_ProductionTestStart(): Sending SOCKET_AGPS_PRODUCTION_TEST_START to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );
}


#ifdef METRICO_TEST_TOOL
/*****************************************************************************
* gpsClient_DebugLogingStart :
*****************************************************************************/
void gpsClient_DebugLogingStart()
{
    t_agps_Command command;
    t_agps_Packet packet;
    int ret_val;

    command.type = SOCKET_AGPS_DEBUG_LOG_START;

    DEBUG_LOG_PRINT_LEV2(("Entering gpsClient_ProductionTestStart()\n"));


    packet.length = 0;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_DebugLogingStart(): Sending SOCKET_AGPS_DEBUG_LOG_START to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );
}
#endif /*METRICO_TEST_TOOL */


/*****************************************************************************
* gpsClient_DisableECellID :
*****************************************************************************/
void  gpsClient_DisableECellID()
{
    t_agps_Command command;
    t_agps_Configuration gpsConfiguration;
    t_agps_Packet packet;

    memset( &gpsConfiguration , 0 , sizeof(gpsConfiguration) );


    command.type = SOCKET_AGPS_SET_USER_CONFIG;

    gpsConfiguration.configMaskValid = K_AGPS_CONFIG_MS_POS_TECH_ECID; /* Since configmask is already 0, this will disable ECell ID */

    packet.pData = (void *) (&gpsConfiguration);
    packet.length = sizeof( gpsConfiguration );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("SetConfiguration: Sending SOCKET_AGPS_SET_USER_CONFIG to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

}


#if defined(AGPS_ANDR_DONUT_FTR) || defined( AGPS_ANDR_ECLAIR_FTR )

/*****************************************************************************
* gpsClient_SendSlpAddressActual :
*****************************************************************************/
void gpsClient_SendSlpAddressActual()
{
    if( slpAddress.type != K_AGPS_SLP_ADDRESS_DEFAULT )
    {
        t_agps_Command command;
        t_agps_Configuration gpsConfiguration;
        t_agps_Packet packet;

        memset( &gpsConfiguration , 0 , sizeof(gpsConfiguration) );


        command.type = SOCKET_AGPS_SET_USER_CONFIG;

        gpsConfiguration.config2Valid = K_AGPS_CONFIG2_SUPL_SERVER_CONFIG;
        gpsConfiguration.slpAddress = slpAddress;

        packet.pData = (void *) (&gpsConfiguration);
        packet.length = sizeof( gpsConfiguration );
        command.data = (void *) ( &packet );

        DEBUG_LOG_PRINT_LEV2(("SetConfiguration: Sending SOCKET_AGPS_SET_USER_CONFIG to Socket server\n"));
        clientUtils_SendCommandToServer( &gpsState, &command );

        slpAddress.type = K_AGPS_SLP_ADDRESS_DEFAULT;
    }
}
#endif /* AGPS_ANDR_DONUT_FTR || AGPS_ANDR_ECLAIR_FTR */


/*****************************************************************************
* gpsClient_Register
*****************************************************************************/
void gpsClient_Register( void )
{
    /* Register with LBSD */
    gpsClient_RegisterMask();

    if(gpsState.reconnectReq  == FALSE)
    {
#ifdef AGPS_UP_FTR
        TlsInit(NULL);
#endif /*AGPS_UP_FTR*/
    }

    /* if there is a stored slp address send it off *. This is put in place to get around a behavior from GpsLocationProvider where
            it dumps the slp address without checking if gps is initialized */
    gpsClient_SendSlpAddressActual();

#ifdef METRICO_TEST_TOOL
    gpsClient_DebugLogingStart();
#endif
}


/*****************************************************************************
* gpsClient_Init:
*****************************************************************************/
int gpsClient_Init( t_gpsClient_Callbacks *callbacks )
{
    int retVal;

    if( callbacks == NULL )
         return GPSCLIENT_INVALID_PARAMETER;

    gpsCallbacks               = callbacks;
    gpsState.callback          = gpsClient_GpsStateEventHandler;
    gpsState.callbackReconnect = gpsClient_GpsReconnectHandler;
    gpsState.fixRequested      = FALSE;

#ifdef AGPS_UP_FTR
    gpsState.callbackEventUpdate = gpsClient_HandleEventList;
    gpsState.callbackFdUpdate = gpsClient_UpdateFdList;
#endif /*AGPS_UP_FTR*/

    retVal = clientUtils_InitGpsState( &gpsState );
    DEBUG_LOG_PRINT_LEV2(("gpsClient_InitGpsState() returned %d\n", retVal ));

    if(retVal == FALSE)
        return retVal;
    else
    {
        gpsState.reconnectReq  = FALSE;
        gpsClient_Register();
        return retVal;
    }
}


/*****************************************************************************
* gpsClient_Disable:
*****************************************************************************/
int gpsClient_Disable()
{
    int vl_status=0;
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_SWITCH_OFF_GPS;

    DEBUG_LOG_PRINT_LEV2(("Entering gpsClient_SwitchOffGps()\n"));

    packet.length = 0;
    packet.pData = NULL;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_Start(): Sending SOCKET_AGPS_SWITCH_OFF_GPS to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );

    vl_status=clientUtils_WaitForStatusFromServer(&gpsState);
    if(vl_status!=GPSCLIENT_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("Status From server%d\n",vl_status));
        return vl_status;
    }

    clientUtils_StopGpsState(&gpsState);
    gpsState.fixRequested = FALSE;
    gpsState.reconnectReq = FALSE;

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_StorePeriodicFixParams
*****************************************************************************/
void gpsClient_StorePeriodicFixParams( unsigned int outputType, unsigned int nmeaMask, unsigned int fixRate )
{
    if(vg_Periodic_fix_dataparam == NULL)
    {
        vg_Periodic_fix_dataparam = (t_agps_PeriodicFixCommand *)malloc( sizeof( t_agps_PeriodicFixCommand ) );
    }

    vg_Periodic_fix_dataparam->fixRate    = fixRate;
    vg_Periodic_fix_dataparam->nmeaMask   = nmeaMask;
    vg_Periodic_fix_dataparam->outputType = outputType;
}


/*****************************************************************************
* gpsClient_ClearPeriodicFixParams
*****************************************************************************/
void gpsClient_ClearPeriodicFixParams( void )
{
    if(vg_Periodic_fix_dataparam != NULL)
    {
        free(vg_Periodic_fix_dataparam);
        vg_Periodic_fix_dataparam = NULL;
    }
    DEBUG_LOG_PRINT_LEV2(("gpsClient_ClearPeriodicFixParams: Successfully Cleared"));
}


/*****************************************************************************
* gpsClient_PeriodicFix
*****************************************************************************/
int gpsClient_PeriodicFix(unsigned int outputType, unsigned int nmeaMask, unsigned int fixRate)
{
    int vl_status=0;

    t_agps_Command command;
    t_agps_PeriodicFixCommand periodicFixCommand;
    t_agps_Packet packet;

    if( outputType != K_AGPS_NMEA && outputType != K_AGPS_C_STRUCT
    /* +LMSqc26087 -Anil */
        && outputType != K_AGPS_NMEA_AND_C_STRUCT )
    /* -LMSqc26087 -Anil */
        return GPSCLIENT_INVALID_PARAMETER;
    if( nmeaMask > MAX_S16 )
        return GPSCLIENT_INVALID_PARAMETER;

    command.type = SOCKET_AGPS_PERIODIC_FIX;

    periodicFixCommand.outputType = outputType;
    periodicFixCommand.nmeaMask = nmeaMask;
    periodicFixCommand.fixRate = fixRate;

    packet.pData = (void *) (&periodicFixCommand);
    packet.length = sizeof( periodicFixCommand );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("PeriodicFix(): Sending SOCKET_AGPS_PERIODIC_FIX to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    vl_status=clientUtils_WaitForStatusFromServer(&gpsState);
    if(vl_status!=GPSCLIENT_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("PeriodicFix(): Status From server%d\n",vl_status));
        return vl_status;
    }
    else
    {
        gpsState.fixRequested = TRUE;
        gpsClient_StorePeriodicFixParams( outputType, nmeaMask, fixRate);
        return GPSCLIENT_NO_ERROR;
    }
}


/*****************************************************************************
* gpsClient_SingleShotFix
*****************************************************************************/
int gpsClient_SingleShotFix( unsigned int outputType, unsigned int nmeaMask, \
                      unsigned int timeOut, unsigned int horizAccuracy, unsigned int vertAccuracy, unsigned int ageLimit )
{
    int vl_status=0;

    t_agps_Command command;
    t_agps_SingleShotFixCommand singleShotFixCommand;
    t_agps_Packet packet;

    if( outputType != K_AGPS_NMEA && outputType != K_AGPS_C_STRUCT
    /* ER - 322372 */
        && outputType != K_AGPS_NMEA_AND_C_STRUCT )
        return GPSCLIENT_INVALID_PARAMETER;
    if( nmeaMask > MAX_S16 )
        return GPSCLIENT_INVALID_PARAMETER;

    command.type = SOCKET_AGPS_SINGLE_SHOT_FIX;

    singleShotFixCommand.outputType = outputType;
    singleShotFixCommand.nmeaMask = nmeaMask;
    singleShotFixCommand.timeout = timeOut;
    singleShotFixCommand.horizAccuracy = horizAccuracy;
    singleShotFixCommand.vertAccuracy = vertAccuracy;
    singleShotFixCommand.ageLimit = ageLimit;

    packet.pData = (void *) (&singleShotFixCommand);
    packet.length = sizeof( singleShotFixCommand );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("SingleShotFix(): Sending SOCKET_AGPS_SINGLE_SHOT_FIX to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    vl_status=clientUtils_WaitForStatusFromServer(&gpsState);
    if(vl_status!=GPSCLIENT_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("SingleShotFix(): Status From server%d\n",vl_status));
        return vl_status;
    }
    else
    {
    /* For handling single shot fix, timers has to be included in gpsclient code(new timeout = old timeout-amount of time elapsed)
            So, single shot fix is not handled */
        gpsState.fixRequested = FALSE;
        return GPSCLIENT_NO_ERROR;
    }
}


/*****************************************************************************
* gpsClient_FenceGeographicalArea :
*****************************************************************************/
int gpsClient_FenceGeographicalArea( uint8_t outputType, uint16_t nmeaMask, t_gpsClient_PositioningParams positioningParams,
                                     const t_gpsClient_GeographicTargetArea* targetArea, uint16_t speedThreshold, e_gpsClient_AreaEventType areaEventType
                                   )
{

    /* Only parameter check done here is the number of polygon points as it is required for serializing data.
    Rest of parameters are checked along polygon points is validated if in range inside CGPS */
    if(   (   targetArea->v_NumberOfPoints > GPSCLIENT_MAX_POLYGON_POINTS
           || targetArea->v_NumberOfPoints == 0
          )
        && targetArea->v_ShapeType == GPSCLIENT_POLYGON_AREA )
    {
        DEBUG_LOG_PRINT_LEV2(("Invalid number of polygon points\n"));

        return GPSCLIENT_INVALID_PARAMETER;
    }
    else
    {
        int     vl_status=0;
        uint8_t vl_NumOfElem = 0;

        t_agps_Command command;
        t_agps_FenceGeographicalAreaCommand fenceGeoAreaCommand;
        t_agps_Packet packet;

        command.type = SOCKET_AGPS_FENCE_GEOGRAPHICAL_AREA;

        fenceGeoAreaCommand.v_OutputType = outputType;
        fenceGeoAreaCommand.v_NmeaMask   = nmeaMask;
        memcpy(&fenceGeoAreaCommand.v_PositioningParams, &positioningParams, sizeof(t_gpsClient_PositioningParams));

        fenceGeoAreaCommand.v_TargetArea.v_ShapeType           = targetArea->v_ShapeType;
        fenceGeoAreaCommand.v_TargetArea.v_NumberOfPoints      = targetArea->v_NumberOfPoints;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMinorAngle = targetArea->v_EllipSemiMinorAngle;
        fenceGeoAreaCommand.v_TargetArea.v_CircularRadius      = targetArea->v_CircularRadius;
        fenceGeoAreaCommand.v_TargetArea.v_CircularRadiusMin   = targetArea->v_CircularRadiusMin;
        fenceGeoAreaCommand.v_TargetArea.v_CircularRadiusMax   = targetArea->v_CircularRadiusMax;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMajor      = targetArea->v_EllipSemiMajor;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMajorMin   = targetArea->v_EllipSemiMajorMin;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMajorMax   = targetArea->v_EllipSemiMajorMax;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMinor      = targetArea->v_EllipSemiMinor;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMinorMin   = targetArea->v_EllipSemiMinorMin;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMinorMax   = targetArea->v_EllipSemiMinorMax;
        fenceGeoAreaCommand.v_TargetArea.v_EllipSemiMinorMax   = targetArea->v_EllipSemiMinorMax;
        fenceGeoAreaCommand.v_TargetArea.v_PolygonHysteresis   = targetArea->v_PolygonHysteresis;

        vl_NumOfElem = (targetArea->v_ShapeType==GPSCLIENT_POLYGON_AREA)?targetArea->v_NumberOfPoints:1;

        memcpy(fenceGeoAreaCommand.v_TargetArea.v_LatSign,   targetArea->v_LatSign,   sizeof(uint8_t) *vl_NumOfElem);
        memcpy(fenceGeoAreaCommand.v_TargetArea.v_Latitude,  targetArea->v_Latitude,  sizeof(uint32_t)*vl_NumOfElem);
        memcpy(fenceGeoAreaCommand.v_TargetArea.v_Longitude, targetArea->v_Longitude, sizeof(int32_t) *vl_NumOfElem);


        fenceGeoAreaCommand.v_SpeedThreshold = speedThreshold;
        fenceGeoAreaCommand.v_AreaEventType  = areaEventType;

        packet.pData  = (void *) (&fenceGeoAreaCommand);
        packet.length = sizeof( fenceGeoAreaCommand );
        command.data  = (void *) (&packet);

        DEBUG_LOG_PRINT_LEV2(("gpsClient_FenceGeographicalArea(): Sending SOCKET_AGPS_FENCE_GEOGRAPHICAL_AREA to Socket server\n"));
        clientUtils_SendCommandToServer( &gpsState, &command );
        vl_status = clientUtils_WaitForStatusFromServer(&gpsState);

        if( vl_status != GPSCLIENT_NO_ERROR )
        {
            DEBUG_LOG_PRINT_LEV2(("StarusFrom server%d\n",vl_status));
            return vl_status;
        }

        else
            return GPSCLIENT_NO_ERROR;
    }
}


/*****************************************************************************
* gpsClient_LocationForward :
*****************************************************************************/
int gpsClient_LocationForward(
    const t_gpsClient_Qop *qop,
    const t_gpsClient_ThirdClientInfo *clientInfo,
    e_gpsClient_LcsServiceType serviceType
)
{
    int vl_status=0;

    t_agps_Command command;
    t_agps_LocationForwardCommand locationForwardCommand;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_LOCATION_FORWARD;

    memcpy(&locationForwardCommand.qop,qop,sizeof(t_gpsClient_Qop));
    memcpy(&locationForwardCommand.clientInfo,clientInfo,sizeof(t_gpsClient_ThirdClientInfo));

    locationForwardCommand.serviceType = serviceType;

    packet.pData = (void *) (&locationForwardCommand);
    packet.length = sizeof( locationForwardCommand );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_LocationForward(): Sending SOCKET_AGPS_LOCATION_FORWARD to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    vl_status = clientUtils_WaitForStatusFromServer(&gpsState);

    if(vl_status!=GPSCLIENT_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("StarusFrom server%d\n",vl_status));
        return vl_status;
    }

    else
        return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_LocationRetrieval :
*****************************************************************************/
int gpsClient_LocationRetrieval(
    const t_gpsClient_Qop *qop,
    const t_gpsClient_ThirdClientInfo *clientInfo
)
{
    int vl_status=0;

    t_agps_Command command;
    t_agps_LocationRetrieveCommand locationRetrieveCommand;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_LOCATION_RETRIEVAL;

    memcpy(&locationRetrieveCommand.qop,qop,sizeof(t_gpsClient_Qop));
    memcpy(&locationRetrieveCommand.clientInfo,clientInfo,sizeof(t_gpsClient_ThirdClientInfo));

    packet.pData = (void *) (&locationRetrieveCommand);
    packet.length = sizeof( locationRetrieveCommand );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_LocationRetrieval(): Sending SOCKET_AGPS_LOCATION_RETRIEVAL to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    vl_status = clientUtils_WaitForStatusFromServer(&gpsState);

    if(vl_status!=GPSCLIENT_NO_ERROR)
    {
        DEBUG_LOG_PRINT_LEV2(("StarusFrom server%d\n",vl_status));
        return vl_status;
    }

    else
        return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_DeleteAidingData :
*****************************************************************************/
int gpsClient_DeleteAidingData( uint16_t aidingDataType )
{
    t_agps_Command command;

    t_agps_Packet packet;
    t_agps_Configuration config;
    uint32_t fields = 0;

    memset( &config , 0 , sizeof(config) );

    command.type = SOCKET_AGPS_DELETE_AIDING_DATA;
    DEBUG_LOG_PRINT_LEV2(("gpsClient_DeleteAidingData : Recvd : %x\n" , aidingDataType ));

    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_ALMANAC )
    {
        fields |= K_AGPS_FIELD_ALMANAC;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_POSITION)
    {
        fields |= K_AGPS_FIELD_POSITION;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_EPHEMERIS)
    {
        fields |= K_AGPS_FIELD_EPHEMERIS;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_TIME)
    {
        fields |= K_AGPS_FIELD_TIME;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_IONO)
    {
        fields |= K_AGPS_FIELD_IONO_MODEL;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_HEALTH)
    {
        fields |= K_AGPS_FIELD_SV_HEALTH;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_UTC)
    {
        fields |= K_AGPS_FIELD_UTC;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_SEED_EPHEMERIS)
    {
        fields |= K_AGPS_FIELD_SEED_DATA;
    }
    if( aidingDataType & GPSCLIENT_ASSIST_DELETE_LOCID)
    {
        fields |= K_AGPS_FIELD_LOCID;
    }

    packet.pData = (void *) (&fields);
    packet.length = sizeof( fields );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsClient_DeleteAidingData(): Sending SOCKET_AGPS_DELETE_AIDING_DATA to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_SetPositionMode :
*****************************************************************************/
int gpsClient_SetPositionMode( uint32_t gpsPositionMode )
{
    PositionMode = gpsPositionMode;

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_Stop :
*****************************************************************************/
int gpsClient_Stop()
{
    t_agps_Command command;
    t_agps_PeriodicFixCommand periodicFixCommand;
    t_agps_Packet packet;

    packet.length = 0;
    packet.pData = NULL;

    command.data = (void *) ( &packet );

    command.type = SOCKET_AGPS_SERVICE_STOP;

    DEBUG_LOG_PRINT_LEV2(("StopCgps(): Sending SOCKET_AGPS_SERVICE_STOP to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );
    gpsClient_ClearPeriodicFixParams();
    gpsState.fixRequested = FALSE;

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsclient_SendCommandToServer :
*****************************************************************************/
void gpsclient_SendCommandToServer( t_agps_Command *command )
{
    clientUtils_SendCommandToServer( &gpsState, command );
}


/*****************************************************************************
* gpsClient_RestartPositionFix
*****************************************************************************/
void gpsClient_RestartPositionFix(void)
{
    int retVal = GPSCLIENT_ERROR;

    /* For handling single shot fix, timers has to be included in gpsclient code(new timeout = old timeout-amount of time elapsed)
            So, only periodic fix is restarted and single shot fix is not handled */
    if( vg_Periodic_fix_dataparam != NULL )
    {
        gpsClient_ServiceStart( );
        retVal = gpsClient_PeriodicFix(vg_Periodic_fix_dataparam->outputType, vg_Periodic_fix_dataparam->nmeaMask, vg_Periodic_fix_dataparam->fixRate);
    }

    if( retVal == GPSCLIENT_NO_ERROR )
    {
        DEBUG_LOG_PRINT_LEV2(("%s: SUCCESS : Periodic Fix request re-issue to lbsd ", __FUNCTION__ ));
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("%s: ERROR : Periodic Fix request re-issue to lbsd ", __FUNCTION__ ));
    }
}



/*****************************************************************************
* gpsClient_GpsReconnectHandler
*****************************************************************************/
void gpsClient_GpsReconnectHandler( void )
{
    gpsClient_Register();

    // IF a Fix was requested by Android, we will have to re-establish this connection
    if(gpsState.fixRequested == TRUE)
    {
        pthread_t  vl_thread;
        if ( pthread_create( &vl_thread, NULL, (void *)&gpsClient_RestartPositionFix, NULL ) != 0 )
        {
            DEBUG_LOG_PRINT_LEV2(("%s: ERROR: Thread creation for periodic fix request: %s",__FUNCTION__, strerror(errno)));
            // We will return TRUE because although we were not able to re-register with lbsd for fix, we were still able to re-connect successfully
        }
    }
}


#ifdef AGPS_ANDR_CUPCAKE_FTR
/*****************************************************************************
* gpsClient_SlpAddress :
*****************************************************************************/
int gpsClient_SlpAddress( unsigned int ipAddress, int port )
{
    t_agps_Command command;
    t_agps_Configuration gpsConfiguration;
    BOOL retVal;
    t_agps_Packet packet;

    int index;
    int ipParts[ 4 ];
    int noOfParts = 4;
    int byteSize = 8;
    uint32_t mask = 0xFFFF0000;


    for( index = 0; index < noOfParts; ++index )
    {
        ipParts[ index ] = (mask | ipAddress) >> (noOfParts - index + 1) * byteSize;
    }

    command.type = SOCKET_AGPS_SET_USER_CONFIG;

    gpsConfiguration.configMaskValid = 0;
    gpsConfiguration.configMask = 0;
    gpsConfiguration.config2Valid = K_AGPS_CONFIG2_SUPL_SERVER_CONFIG;
    gpsConfiguration.sensMod = 0;
    gpsConfiguration.sensTTFF = 0;
    gpsConfiguration.powerPref = 0;
    gpsConfiguration.coldStart = 0;
    gpsConfiguration.slpAddress.portNumber = (uint16_t)port;
    gpsConfiguration.slpAddress.type = K_AGPS_SLP_ADDRESS_IPV4;
    sprintf( gpsConfiguration.slpAddress.adress, "%d.%d.%d.%d", ipParts[0], ipParts[1], ipParts[2], ipParts[3] );
    gpsConfiguration.slpAddress.length = strlen(gpsConfiguration.slpAddress.adress) + 1;

    packet.pData = (void *) (&gpsConfiguration);
    packet.length = sizeof( gpsConfiguration );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("SetConfiguration: Sending SOCKET_AGPS_SET_USER_CONFIG to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return GPSCLIENT_NO_ERROR;
}
#endif /* AGPS_ANDR_CUPCAKE_FTR */


#if defined(AGPS_ANDR_DONUT_FTR) || defined( AGPS_ANDR_ECLAIR_FTR )
/*****************************************************************************
* gpsClient_SlpAddress :
*****************************************************************************/
int gpsClient_SlpAddress( char *hostname , int port )
{
    int vl_Index;
    int vl_Length;

    DEBUG_LOG_PRINT_LEV2(("server : %s , port %d" , hostname , port ));

    /* We have to figure out if address is of type IPV4 or FQDN */
    /* @todo Use   int inet_aton(const char *cp, struct in_addr *inp); */

    slpAddress.type = K_AGPS_SLP_ADDRESS_IPV4;
    vl_Length = strlen(hostname);

    for( vl_Index = 0 ; vl_Index < vl_Length ; vl_Index++ )
    {
        if ( (!isdigit( hostname[vl_Index]))&& ( hostname[vl_Index] != '.' ) )
        {
            slpAddress.type = K_AGPS_SLP_ADDRESS_FQDN;
            break;
        }
    }

    DEBUG_LOG_PRINT_LEV2(("server : %s , port %d , type(1.FQDN, 2.IPV4 - ) %d" , hostname , port,  slpAddress.type ));

    memcpy( slpAddress.address , hostname , vl_Length );
    slpAddress.portNumber = port;

    if( gpsState.init )
    {
        gpsClient_SendSlpAddressActual();
    }

    return GPSCLIENT_NO_ERROR;
}


/*****************************************************************************
* gpsClient_AgpsInit :
*****************************************************************************/
int gpsClient_AgpsInit( t_gpsClient_agpsCallbacks *callbacks )
{
    DEBUG_LOG_PRINT_LEV2(("gpsClient_AgpsInit Called!"));

#ifdef AGPS_UP_FTR
    if( NULL != callbacks )
    {
        agpsCallbacks = callbacks;
        BearerHandlerInit( callbacks );
        return GPSCLIENT_NO_ERROR;
    }
    else
        return GPSCLIENT_ERROR;

#else
    DEBUG_LOG_PRINT_LEV2(("AGPS SUPL feature is disabled!"));
    return GPSCLIENT_ERROR;
#endif /*AGPS_UP_FTR*/

}


/*****************************************************************************
* gpsclient_DataConnOpen :
*****************************************************************************/
int gpsclient_DataConnOpen( char *apn )
{

    /* this is a dummy statement to get rid of unused parameter apn warning when the MULTIPLE_PDP_FOR_ATT_TEMP_FTR switch is not enabled */
    apn = apn;
    
#ifdef AGPS_UP_FTR
#ifdef MULTIPLE_PDP_FOR_ATT_TEMP_FTR
    if(strcmp(apn,"wap.cingular") == 0)
        TlsSetNetworkInterface("pdp1");  /* Set this to pdp0 for testing with ULTS. Set this to pdp1 for AT&T testing */
    else
        TlsSetNetworkInterface("gprs0");
#endif

    ProcessDataConnectionResponse( AGPS_BEARER_CONNECTION_CNF );

    return GPSCLIENT_NO_ERROR;
#else
        return GPSCLIENT_ERROR;
#endif /*AGPS_UP_FTR*/
}


/*****************************************************************************
* gpsclient_DataConnClosed :
*****************************************************************************/
int gpsclient_DataConnClosed()
{
#ifdef AGPS_UP_FTR
    ProcessDataConnectionResponse( AGPS_BEARER_CLOSE_CNF );
    return GPSCLIENT_NO_ERROR;
#else
    return GPSCLIENT_ERROR;
#endif /*AGPS_UP_FTR*/
}


/*****************************************************************************
* gpsclient_DataConnFailed :
*****************************************************************************/
int gpsclient_DataConnFailed()
{
#ifdef AGPS_UP_FTR
    ProcessDataConnectionResponse(     AGPS_BEARER_CONNECTION_ERR );
    return GPSCLIENT_NO_ERROR;
#else
    return GPSCLIENT_ERROR;
#endif /*AGPS_UP_FTR*/
}


/*****************************************************************************
* gpsclient_DataConnCloseFailed :
*****************************************************************************/
int gpsclient_DataConnCloseFailed()
{
#ifdef AGPS_UP_FTR

    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_CLOSE_BEARER_ERR;

    packet.pData = NULL;
    packet.length = 0;
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsclient_DataConnClosed(): Sending SOCKET_AGPS_CLOSE_BEARER_CNF to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return GPSCLIENT_NO_ERROR;
#else
    return GPSCLIENT_ERROR;
#endif /*AGPS_UP_FTR*/

}
#endif /* defined(AGPS_ANDR_DONUT_FTR) && defined( AGPS_ANDR_ECLAIR_FTR )  */


#ifdef AGPS_ANDR_ECLAIR_FTR
/*****************************************************************************
* gpsclient_NotificationInit :
*****************************************************************************/
int gpsclient_NotificationInit( t_gpsClient_NiCallbacks *callbacks )
{
    DEBUG_LOG_PRINT_LEV2(("gpsclient_NotificationInit"));
    if( NULL != callbacks )
    {
        gpsNiCallbacks = callbacks;
        return GPSCLIENT_NO_ERROR;
    }
    else return GPSCLIENT_ERROR;
}


/*****************************************************************************
* gpsclient_NotificationResponse :
*****************************************************************************/
int gpsclient_NotificationResponse( int handle , e_gpsClient_UserResponseType response )
{
    t_agps_Command  command;
    t_agps_Packet   packet;
    uint8_t              answer = 0;

    t_agps_NotificationRsp notifResp;

    DEBUG_LOG_PRINT_LEV2(("Notification user answer is %d and handle is %d\n", response, handle));

    switch( response )
    {
        case GPSCLIENT_NI_RESPONSE_ACCEPT:
            answer = K_AGPS_USER_ACCEPT;
            break;
        case GPSCLIENT_NI_RESPONSE_DENY:
            answer = K_AGPS_USER_REJECT;
            break;
        case GPSCLIENT_NI_RESPONSE_NORESP:
            DEBUG_LOG_PRINT_LEV2(("In case of No Response, no need to forward!"));
            return GPSCLIENT_ERROR;
    }

    command.type = SOCKET_AGPS_NOTIFICATION_USER_RESPONSE;

    notifResp.v_Handle = (uint32_t)handle;
    notifResp.v_Answer = (uint8_t)answer;

    packet.pData = (void *) (&notifResp);
    packet.length = sizeof( notifResp );
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("Notification user answer is %d and handle is %d\n", notifResp.v_Answer, notifResp.v_Handle));
    DEBUG_LOG_PRINT_LEV2(("agpsClient_SendUserAnswer(): Sending SOCKET_AGPS_NOTIFICATION_USER_RESPONSE to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return GPSCLIENT_NO_ERROR;
}
#endif /* AGPS_ANDR_ECLAIR_FTR */


#ifdef AGPS_UP_FTR
/*****************************************************************************
* TlsConnectCnf :
*****************************************************************************/
void TlsConnectCnf(unsigned int handle)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_TcpIpConnectCnf cnf;

    command.type = SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF;

    cnf.v_ConnectionHandle = handle;

    packet.pData = (void *)&cnf;
    packet.length = sizeof(cnf);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("TlsConnectCnf(): Sending SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF for handle %d to Socket server\n" , handle ));
    clientUtils_SendCommandToServer( &gpsState, &command );

    return;
}


/*****************************************************************************
* TlsGetErr :
*****************************************************************************/
e_agps_ErrorType TlsGetErr(int error)
{
    e_agps_ErrorType agps_err = K_AGPS_ERROR_TYPE_UNKNOWN;
    switch((e_TlsError)error)
    {
        case K_TLS_NO_ERROR:
        {
            agps_err = K_AGPS_ERROR_TYPE_UNKNOWN;
            break;
        }
        case K_TLS_ERROR_SSL_HANDSHAKE:
        {
            agps_err = K_AGPS_ERROR_TLS_HANDSHAKE_FAILED;
            break;
        }
        case K_TLS_ERROR_DNS_RESOLVE_FAILURE:
        {
            agps_err = K_AGPS_ERROR_DNS_RESOLVE_FAILED;
            break;
        }
        case K_TLS_ERROR_NO_CONNECT:
        {
            agps_err = K_AGPS_ERROR_CONNECT_FAILED;
            break;
        }
        default :
        {
            agps_err = K_AGPS_ERROR_TYPE_UNKNOWN;
            break;
        }
    }
    return agps_err;

}


/*****************************************************************************
* TlsConnectErr :
*****************************************************************************/
void TlsConnectErr(unsigned int handle,int error)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_TcpIpConnectErr err;

    command.type = SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR;

    err.v_ConnectionHandle = handle;

    err.v_ErrorType = TlsGetErr(error);


    packet.pData = (void *)&err;
    packet.length = sizeof(err);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("TlsConnectErr(): Sending SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return;
}


/*****************************************************************************
* TlsReadInd :
*****************************************************************************/
void TlsReadInd(unsigned int handle,char * p_Data,int v_Length)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_SuplData suplData;

    suplData.v_ConnectionHandle = handle;
    suplData.v_Data.p_Pointer = (uint8_t *)p_Data;
    suplData.v_Data.v_Length = v_Length;

    command.type = SOCKET_AGPS_SUPL_RECEIVE_DATA_IND;

    packet.pData = (void *)&suplData;
    packet.length = v_Length + + sizeof(suplData.v_ConnectionHandle);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("TlsReadInd(): Sending SOCKET_AGPS_SUPL_RECEIVE_DATA_IND for handle %d to Socket server\n" , handle ));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return;
}


/*****************************************************************************
* TlsDisconnectCnf :
*****************************************************************************/
void TlsDisconnectCnf(unsigned int handle )
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_agps_TcpIpDisconnectCnf cnf;

    command.type = SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF;

    cnf.v_ConnectionHandle = handle;

    packet.pData = (void *)&cnf;
    packet.length = sizeof(cnf);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("TlsDisconnectCnf(): Sending SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return;
}
#endif /*AGPS_UP_FTR*/




/*****************************************************************************
* gpsclient_SendSuplPush :
*****************************************************************************/
void gpsclient_SendSuplPush( char *p_Push , int v_PushLen , char *p_Hash , int v_HashLen )
{
    t_agps_SmsPushInd v_Push;
    t_agps_Command command;
    t_agps_Packet packet;

    v_Push.v_SmsPushInd.p_Pointer = (uint8_t*)p_Push;
    v_Push.v_SmsPushInd.v_Length  = v_PushLen;
    v_Push.v_Hash.p_Pointer       = (uint8_t*)p_Hash;
    v_Push.v_Hash.v_Length        = v_HashLen;

    command.type = SOCKET_AGPS_SUPL_SMS_PUSH_IND;

    packet.pData = (void *)&v_Push;
    packet.length = sizeof(v_Push);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("gpsclient_SendSuplPush(): Sending SOCKET_AGPS_SUPL_SMS_PUSH_IND\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
}


#ifdef AGPS_UNIT_TEST_FTR
/*****************************************************************************
* agpsClient_SmsPush :
*****************************************************************************/
int agpsClient_SmsPush( char* p_Sms , int v_SmsLen , char* p_Hash , int v_HashLen )
{
    t_agps_SmsPushInd v_SmsPushInd;
    t_agps_Command command;
    t_agps_Packet packet;

    v_SmsPushInd.v_SmsPushInd.p_Pointer = p_Sms;
    v_SmsPushInd.v_SmsPushInd.v_Length = v_SmsLen;
    v_SmsPushInd.v_Hash.p_Pointer = p_Hash;
    v_SmsPushInd.v_Hash.v_Length = v_HashLen;

    command.type = SOCKET_AGPS_SUPL_SMS_PUSH_IND;

    packet.pData = (void *)&v_SmsPushInd;
    packet.length = sizeof(v_SmsPushInd);
    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("agpsClient_SmsPush(): Sending SOCKET_AGPS_SUPL_SMS_PUSH_IND to Socket server\n"));
    clientUtils_SendCommandToServer( &gpsState, &command );
    return GPSCLIENT_NO_ERROR;
}
#endif /* AGPS_UNIT_TEST_FTR */


#ifdef AGPS_ANDR_GINGER_FTR
/*****************************************************************************
* gpsclient_AgpsRilInit :
*****************************************************************************/
void gpsclient_AgpsRilInit( t_gpsClient_RilCallbacks *callbacks )
{
    if( NULL != callbacks )
    {
        agpsRilCallbacks = callbacks;
        DEBUG_LOG_PRINT_LEV2(("gpsclient_AgpsRilInit : Valid AGpsRilCallbacks passed"));
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("agpsRilCallbacks is null pointer."));
    }
}


/*****************************************************************************
* gpsclient_SetMobileInfo :
*****************************************************************************/
void gpsclient_SetMobileInfo(const t_gpsClient_RefLocation* agps_reflocation, size_t sz_struct , const char* imsi)
{
    DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo : Enter, type=%d",agps_reflocation->type));
    if( (agps_reflocation->type == GPSCLIENT_REF_LOCATION_TYPE_GSM_CELLID) ||
        (agps_reflocation->type == GPSCLIENT_REF_LOCATION_TYPE_UMTS_CELLID) )
    {
        if( sz_struct  == sizeof(t_gpsClient_RefLocation) )
        {
            t_gpsClient_RefLocationCellID   cellID;
            int cellType;

            /* ++ LMSqc36667 */
            memcpy( &cellID , &agps_reflocation->u.cellID , sizeof(t_gpsClient_RefLocationCellID) );
            /* -- LMSqc36667 */

            /* If GSM, cell type 0, else cell type 1 */
            cellType =
                ( agps_reflocation->type ==
                GPSCLIENT_REF_LOCATION_TYPE_GSM_CELLID ) ? 0 : 1;

            /* +RRR : Extra logging added to debug Seattle IOT issue */
            DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo : Input MCC %d , MNC %d , CellID %d, imsi=%s"
                ,cellID.mcc , cellID.mnc , cellID.cid, imsi ));
            /* -RRR : Extra logging added to debug Seattle IOT issue */
            /* ++ LMSqc36741 */
            /* gpsClient_Mobile_Info((int)cellID.mcc, (int)cellID.mnc, (int)cellID.cid, cellType, imsi, NULL); */
            gpsClient_Mobile_Info((int)cellID.mcc, (int)cellID.mnc, (int)cellID.cid, (int)cellID.lac , cellType, imsi, NULL);
            /* -- LMSqc36741 */
        }
        else
        {
            DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo ERROR: size Expected  : %d , Actual : %d",sizeof(t_gpsClient_RefLocationCellID) ,sz_struct));
        }
    }
    else if(agps_reflocation->type == GPSCLIENT_REG_LOCATION_TYPE_MAC)
    {
        t_gpsClient_RefLocationMac  Mac;
        t_gpsClient_WlanInfo        WlanInfo;

        unsigned short  ms_addr = 0;
        unsigned long ls_addr= 0;
        memcpy( &Mac.mac, &agps_reflocation->u.mac, sizeof(t_gpsClient_RefLocationMac) );

        ms_addr = Mac.mac[0];
        ms_addr = ms_addr<<8;
        ms_addr = ms_addr|Mac.mac[1];


        ls_addr = Mac.mac[2];
        ls_addr = ls_addr<<24;
        ls_addr = ls_addr|( (Mac.mac[3] & (0x000000ff))<<16);
        ls_addr = ls_addr|(( Mac.mac[4] & (0x000000ff))<<8);
        ls_addr = ls_addr| (( Mac.mac[5] & (0x000000ff)));

        WlanInfo.v_WLAN_AP_LS_Addr = ls_addr;
        WlanInfo.v_WLAN_AP_MS_Addr = ms_addr;
        /*From the framework we get only the MAC ID so all optional parameters are assigned NULL*/
        WlanInfo.v_WLAN_OptionalInfo = NULL;

        DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo : MAC ID LS Addr %x", WlanInfo.v_WLAN_AP_LS_Addr ));
        DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo : MAC ID MS Addr %x", WlanInfo.v_WLAN_AP_MS_Addr ));

        gpsClient_WLAN_Info(&WlanInfo);
    }
    else
    {
        DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo : Unknown type of mobile info. Send IMSI"));
        /* ++ LMSqc36741 */
        /* gpsClient_Mobile_Info(-1, -1, -1, -1, imsi, NULL); */
        gpsClient_Mobile_Info(-1, -1, -1, -1, -1, imsi, NULL);
        /* -- LMSqc36741 */
    }

    DEBUG_LOG_PRINT_LEV2(("gpsclient_SetMobileInfo : Exit"));
}
#endif /* AGPS_ANDR_GINGER_FTR */


int gpsClient_ProductionTestStartReq(t_gpsClient_TestRequest prodStartTREQ)
{
    t_agps_Command     command;
    t_agps_Packet     packet;

    t_agps_TestRequest TREQ_Data;

    memset(&TREQ_Data, 0, sizeof(TREQ_Data));

    DEBUG_LOG_PRINT(("gpsClient_ProductionTestStartReq : Start\n"));

    TREQ_Data.v_TestReqMask = prodStartTREQ.v_TestReqMask;
    memcpy((void *)TREQ_Data.v_InputSigFreq, prodStartTREQ.v_InputSigFreq, MAX_FREQ_LEN);
    memcpy((void *)TREQ_Data.v_RefClockFreq, prodStartTREQ.v_RefClockFreq, MAX_FREQ_LEN);
    memcpy((void *)TREQ_Data.v_PgaValues,    prodStartTREQ.v_PgaValues,    MAX_PGA_VALUE_LEN);
    TREQ_Data.v_InPinValue        = prodStartTREQ.v_InPinValue;
    TREQ_Data.v_InPinMask         = prodStartTREQ.v_InPinMask;
    TREQ_Data.v_OutPinValue       = prodStartTREQ.v_OutPinValue;
    TREQ_Data.v_OutPinMask        = prodStartTREQ.v_OutPinMask;
    TREQ_Data.v_SpectrumTestType  = prodStartTREQ.v_SpectrumTestType;
    TREQ_Data.v_FileLogResult     = prodStartTREQ.v_FileLogResult;

    DEBUG_LOG_PRINT(("gpsClient_ProductionTestStartReq : Start\n ReqMask %d, Freq : %s, SATCmdType : %d\n", TREQ_Data.v_TestReqMask, TREQ_Data.v_InputSigFreq, TREQ_Data.v_SpectrumTestType));

    command.type = SOCKET_AGPS_PRODUCTION_TEST_START_REQ;
    packet.pData = (void *) (&TREQ_Data);
    packet.length = sizeof(t_agps_TestRequest);
    command.data = (void *) (&packet);

    usleep( SLEEP_TIME_MICRO_SECONDS );
    DEBUG_LOG_PRINT(("gpsClient_ProductionTestStartReq(): Sending SOCKET_AGPS_PRODUCTION_TEST_START_REQ to Socket server\n"));

    clientUtils_SendCommandToServer( &gpsState, &command );

    DEBUG_LOG_PRINT(("gpsClient_ProductionTestStartReq : Exit\n"));

    return GPSCLIENT_NO_ERROR; // return 0 mean success

}


int gpsClient_ProductionTestStopReq(t_gpsClient_TestStop testStopReq)
{
    t_agps_Command command;
    t_agps_Packet     packet;

    t_agps_StopRequest TREQ_Stop;
    memset(&TREQ_Stop, 0, sizeof(TREQ_Stop));

    DEBUG_LOG_PRINT_LEV2(("gpsClient_ProductionTestStopReq : Start\n"));

    TREQ_Stop.v_TestReqMask = testStopReq.v_TestMask;
    TREQ_Stop.v_State = testStopReq.v_State;

    DEBUG_LOG_PRINT(("gpsClient_ProductionTestStopReq : STOP \n v_eTestReqMask %d, v_State  %d\n",TREQ_Stop.v_TestReqMask, TREQ_Stop.v_State));
    command.type = SOCKET_AGPS_PRODUCTION_TEST_STOP_IND;
    packet.pData = (void *) (&TREQ_Stop);
    packet.length = sizeof(t_agps_StopRequest);
    command.data = (void *) (&packet);

    usleep( SLEEP_TIME_MICRO_SECONDS );
    clientUtils_SendCommandToServer( &gpsState, &command );
    DEBUG_LOG_PRINT_LEV2(("gpsClient_ProductionTestStopReq(): Sending SOCKET_AGPS_PRODUCTION_TEST_STOP_IND to Socket server\n"));

    DEBUG_LOG_PRINT_LEV2(("gpsClient_ProductionTestStopReq : Exit\n"));

    return GPSCLIENT_NO_ERROR;
}