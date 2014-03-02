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
#ifndef SBEE_CLIENT_H
#define SBEE_CLIENT_H

#define SBEE_SERVER_ADDRESS_LENGTH 100
#define SBEE_REQUEST_STRING_LENGTH 300

/*
* This is called when message handler gets a data connection request for sbee from daemon. This function will store the connection parameters.
*/
void sbeeClient_ProcessRequestSbeeDataConnection(t_sbee_si_DataConnectionRequestParams *sbeeConnectionReq);

/*
* This is called when message handler gets a close data connection request for sbee from gps daemon
*/
void sbeeClient_ProcessSbeeCloseDataConnection(void);


/*
* Notifies sbee connection to user
*/
void sbeeClient_ProcessNotifySbeeConnection(t_sbee_si_UserNotificationParams *notif);

/*
* This is called when message handler gets sbee data request from gps daemon.
*/
void sbeeClient_ProcessSbeeData( t_sbee_si_SbeeDataParams *sbeeData);


/*
* This function sends received data from RxNetwork to gps daemon
*/
void sbeeClient_SendData(unsigned char *data, unsigned short length);


/*
* This function sends data connection status to gps daemon
*/
void sbeeClient_DataconnectionStatus(uint32_t status);


/*
* This function sends user response against notification to gps daemon
*/
void sbeeClient_UserResponse(uint32_t handle, uint32_t status);


/*
* Sends sbee initialization request to gps daemon
*/
int sbeeClient_Initialize();


/*
* Sends sbee start request to gps daemon
*/
int sbeeClient_Start(unsigned short startTime);


/*
* Sends sbee stop request to gps daemon
*/
int sbeeClient_Stop();

/*
* Sends sbee user configuration to gps daemon
*/
void sbeeClient_SendUserConfiguration(uint8_t  serverConnectionOption,
                                                   uint8_t  prefBearerOption,
                                                   uint8_t  chargingModeOption,
                                                   uint8_t  eeOptionMask,
                                                   uint8_t  eeSeedDownloadFreq,
                                                   uint8_t  eeMaxAllowedSeedDownload,
                                                   uint16_t prefTimeOfDay);

/*
* Sends sbee server configuration to gps daemon
*/
void sbeeClient_SendServerConfiguration( int8_t      *deviceId,
                                                      uint8_t    deviceIdLength,
                                                      int8_t      *integratorModelId,
                                                      uint8_t    integratorModelIdLength,
                                                      uint8_t    retryOption,
                                                      int8_t      *sbeeServer,
                                                      uint8_t    sbeeServerLength);

#endif /* SBEE_CLIENT_H */