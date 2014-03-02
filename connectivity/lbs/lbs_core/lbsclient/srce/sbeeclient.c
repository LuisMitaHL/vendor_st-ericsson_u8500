/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Anshuman Pandey
 *  Email : anshuman.pandey@stericsson.com
 *****************************************************************************/
#include <time.h>
#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include "agps_client.h"
#include "clientutils.h"
#include "gpsclient.h"
#include "TlsHandler.h"
#include "sbeesocketinterface.h"
#include "sbeeclient.h"
#include "BearerHandler.h"
#include "HttpHandler.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "sbeeClient"

/*
* This variable is used to store the connection parameters, since it is coming with the connection request.
*/
t_sbee_si_DataConnectionRequestParams sbeeDataConnectionParams;

/*
* This variable is used to store the handle of the request.
*/
uint32_t sbeeHandle;


void gpsclient_SendCommandToServer( t_agps_Command *command );


/*
* This is called when message handler gets a data connection request for sbee from daemon. This function will store the connection parameters.
*/
void sbeeClient_ProcessRequestSbeeDataConnection(t_sbee_si_DataConnectionRequestParams *sbeeConnectionReq)
{
    DEBUG_LOG_PRINT_LEV2(("Calling notificationCallbacks->sbeeConnectionReq_cb : (sbeeClient_ProcessRequestSbeeDataConnection)\n"));
    memcpy(&sbeeDataConnectionParams, sbeeConnectionReq, sizeof(t_sbee_si_DataConnectionRequestParams));
    sbeeHandle = sbeeConnectionReq->v_handle;
    ProcessOpenBearerConnectionRequest( AGPS_BEARER_REQUESTOR_SBEE );
}

/*
* This is called when message handler gets a close data connection request for sbee from gps daemon
*/
void sbeeClient_ProcessSbeeCloseDataConnection(void)
{
    DEBUG_LOG_PRINT_LEV2(("sbeeClient_ProcessSbeeCloseDataConnection() : Calling ProcessCloseBearerConnectionRequest()"));
    ProcessCloseBearerConnectionRequest( AGPS_BEARER_REQUESTOR_SBEE );
}


/*
* Notifies sbee connection to user
*/
void sbeeClient_ProcessNotifySbeeConnection(t_sbee_si_UserNotificationParams *notif)
{
    /* dummy statement to remove unused variable compiler warning*/
    notif = notif;
    DEBUG_LOG_PRINT_LEV2(("gpsClient_ProcessNotifySbeeConnection() : \n"));
    //Notification has to be sent to Android framework
}

/*
* This is called when message handler gets sbee data request from gps daemon.
*/
void sbeeClient_ProcessSbeeData( t_sbee_si_SbeeDataParams *sbeeData)
{
    int8_t resString[SBEE_REQUEST_STRING_LENGTH];
    int8_t vl_serverAddress[SBEE_SERVER_ADDRESS_LENGTH];

    snprintf( (char *)resString, sbeeData->a_SbeeData.v_Length + 1,"%s", (char *)sbeeData->a_SbeeData.p_Pointer );
    snprintf( (char *)vl_serverAddress, sbeeDataConnectionParams.a_ServerAddress.v_Length + 1, "%s", (char *)sbeeDataConnectionParams.a_ServerAddress.p_Pointer );

    //Connect to server with the request string, server address and port number
    ProcessHttpRequest( (char *)resString, (char *)vl_serverAddress, sbeeDataConnectionParams.v_PortNumber );
}

/*
* This function sends received data from RxNetwork to gps daemon
*/
void sbeeClient_SendData(unsigned char *data, unsigned short length)
{
    t_agps_Command command;
    t_sbee_si_SbeeDataParams sbeeDataParams;
    t_agps_Packet packet;

    sbeeDataParams.a_SbeeData.v_Length = length;
    sbeeDataParams.a_SbeeData.p_Pointer = data;
    sbeeDataParams.v_handle = sbeeHandle;

    command.type = SOCKET_AGPS_SBEE_RECV_DATA;
    packet.length = sizeof(t_sbee_si_SbeeDataParams);
    packet.pData = (void *)&sbeeDataParams;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("sbeeClient_SendData: Sending SOCKET_AGPS_SBEE_DATA to Socket server\n"));
    gpsclient_SendCommandToServer( &command );
}


/*
* This function sends data connection status to gps daemon
*/
void sbeeClient_DataconnectionStatus(uint32_t status)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_sbee_si_status_message_with_handle statusMessage;

    statusMessage.handle = sbeeHandle;
    statusMessage.status = status;

    command.type = SOCKET_AGPS_SBEE_SET_DATA_CONNECTION_STATUS;
    packet.length = sizeof(t_sbee_si_status_message_with_handle);
    packet.pData = (void *)&statusMessage;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("sbeeClient_DataconnectionStatus: Sending SOCKET_AGPS_SBEE_SET_DATA_CONNECTION_STATUS to Socket server\n"));
    gpsclient_SendCommandToServer( &command );
}


/*
* This function sends user response against notification to gps daemon
*/
void sbeeClient_UserResponse(uint32_t handle, uint32_t status)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_sbee_si_status_message_with_handle statusMessage;

    statusMessage.handle = handle;
    statusMessage.status = status;

    command.type = SOCKET_AGPS_SBEE_SET_USER_RESPONSE;
    packet.length = sizeof(t_sbee_si_status_message_with_handle);
    packet.pData = (void *)&statusMessage;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("sbeeClient_UserResponse: Sending SOCKET_AGPS_SBEE_SET_USER_RESPONSE to Socket server\n"));
    gpsclient_SendCommandToServer( &command );
}


/*
* Sends sbee initialization request to gps daemon
*/
int sbeeClient_Initialize()
{
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_SBEE_INITIALIZE;
    packet.length = 0;
    packet.pData = NULL;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("sbeeClient_Initialize: Sending SOCKET_AGPS_SBEE_INITIALIZE to Socket server\n"));
    gpsclient_SendCommandToServer( &command );
    return GPSCLIENT_NO_ERROR;
}

/*
* Sends sbee start request to gps daemon
*/
int sbeeClient_Start(unsigned short startTime)
{
    t_agps_Command command;
    t_sbee_si_SbeeStart sbeeStart;
    t_agps_Packet packet;

    sbeeStart.startTime = startTime;
    command.type = SOCKET_AGPS_SBEE_START_EXTENDED_EPHEMERIS;
    packet.length = sizeof(sbeeStart);
    packet.pData = (void *)&sbeeStart;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("sbeeClient_Start: Sending SOCKET_AGPS_SBEE_START_EXTENDED_EPHEMERIS to Socket server\n"));
    gpsclient_SendCommandToServer( &command );
    return GPSCLIENT_NO_ERROR;
}

/*
* Sends sbee stop request to gps daemon
*/
int sbeeClient_Stop()
{
    t_agps_Command command;
    t_agps_Packet packet;

    command.type = SOCKET_AGPS_SBEE_ABORT_EXTENDED_EPHEMERIS;
    packet.length = 0;
    packet.pData = NULL;

    command.data = (void *) ( &packet );

    DEBUG_LOG_PRINT_LEV2(("sbeeClient_Stop: Sending SOCKET_AGPS_SBEE_ABORT_EXTENDED_EPHEMERIS to Socket server\n"));
    gpsclient_SendCommandToServer( &command );
    return GPSCLIENT_NO_ERROR;
}

void sbeeClient_SendUserConfiguration(uint8_t  serverConnectionOption,
                                                   uint8_t  prefBearerOption,
                                                   uint8_t  chargingModeOption,
                                                   uint8_t  eeOptionMask,
                                                   uint8_t  eeSeedDownloadFreq,
                                                   uint8_t  eeMaxAllowedSeedDownload,
                                                   uint16_t prefTimeOfDay)
{

    t_agps_Command command;
    t_agps_Packet packet;
    t_sbee_si_UserConfiguration userConfiguration;

    userConfiguration.v_ServerConnectionOption = serverConnectionOption;
    userConfiguration.v_PrefBearerOption = prefBearerOption;
    userConfiguration.v_ChargingModeOption = chargingModeOption;
    userConfiguration.v_EESeedDownloadFreq = eeSeedDownloadFreq;
    userConfiguration.v_PrefTimeOfDay = prefTimeOfDay;
    userConfiguration.v_EEOptionMask = eeOptionMask;
    userConfiguration.v_EEMaxAllowedSeedDownload = eeMaxAllowedSeedDownload;

    command.type = SOCKET_AGPS_SBEE_SET_USER_CONFIGURATION;
    packet.length = sizeof(t_sbee_si_UserConfiguration);
    packet.pData = (void *)&userConfiguration;

    command.data = (void *) ( &packet );
    DEBUG_LOG_PRINT_LEV2(("agpsClient_sbee_SendUserConfiguration: Sending SOCKET_AGPS_SBEE_USER_CONFIGURATION to Socket server\n"));
    gpsclient_SendCommandToServer( &command );

}

void sbeeClient_SendServerConfiguration( int8_t      *deviceId,
                                                      uint8_t    deviceIdLength,
                                                      int8_t      *integratorModelId,
                                                      uint8_t    integratorModelIdLength,
                                                      uint8_t    retryOption,
                                                      int8_t      *sbeeServer,
                                                      uint8_t    sbeeServerLength)
{
    t_agps_Command command;
    t_agps_Packet packet;
    t_sbee_si_ServerConfiguration serverConfiguration;
    if(retryOption != SBEE_ALTERNATE_SERVER_RETRY_ALLOWED && retryOption != SBEE_ALTERNATE_SERVER_RETRY_NOT_ALLOWED)
    {
        DEBUG_LOG_PRINT_LEV2(("agpsClient_sbee_SendServerConfiguration: RetryOption is invalid\n"));
        return;
    }

    memset( &serverConfiguration , 0 , sizeof( serverConfiguration ) );

    memcpy(serverConfiguration.v_DeviceId, deviceId, deviceIdLength);
    memcpy(serverConfiguration.v_IntegratorModelId, integratorModelId, integratorModelIdLength);
    serverConfiguration.v_RetryOption = retryOption;
    serverConfiguration.v_SbeeServersLength[0] = sbeeServerLength;
    serverConfiguration.v_NumSbeeServers = 1;
    serverConfiguration.v_SbeeServers[0] = sbeeServer;

    command.type = SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION;
    packet.length = sizeof(t_sbee_si_ServerConfiguration);
    packet.pData = (void *)&serverConfiguration;

    command.data = (void *) ( &packet );
    DEBUG_LOG_PRINT_LEV2(("agpsClient_sbee_SendServerConfiguration: Sending SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION to Socket server\n"));
    gpsclient_SendCommandToServer( &command );

}

