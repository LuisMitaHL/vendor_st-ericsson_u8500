/*
* Copyright (C) ST-Ericsson 2009
*
* gpsclient.h
* Date: 07-09-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef _AGPS_SERVER_INTERFACE_H
#define _AGPS_SERVER_INTERFACE_H

#ifndef AGPS_GENERIC_SOLUTION_FTR

#define AGPS_SERVER_VERSION "AGPSLR-2.9"


/*! \addtogroup MacroAmil */
/*@{*/
/**
* \def MC_AGPS_SERVICE_START_REQUEST
* starts the GPS Service
* \see AMIL_ServiceStartRequest
*/
#define MC_AGPS_SERVICE_START_REQUEST AMIL_ServiceStartRequest

/**
* \def MC_AGPS_SERVICE_STOP_REQUEST
* stops the GPS Service
* \see AMIL_ServiceStopRequest
*/
#define MC_AGPS_SERVICE_STOP_REQUEST AMIL_ServiceStopRequest

/**
* \def MC_AGPS_PERIODIC_FIX_REQUEST
* registers a periodic fix
* \see AMIL_PeriodicFixRequest
*/
#define MC_AGPS_PERIODIC_FIX_REQUEST AMIL_PeriodicFixRequest


/**
* \def MC_AGPS_SINGLESHOT_FIX_REQUEST
* registers a single shot
* \see AMIL_SingleshotFixRequest
*/
#define MC_AGPS_SINGLESHOT_FIX_REQUEST AMIL_SingleshotFixRequest

/**
* \def MC_AGPS_SET_POSITION_MODE
* sets the position mode
* \see AMIL_SetPositionMode
*/
#define MC_AGPS_SET_POSITION_MODE AMIL_SetPositionMode


/**
* \def MC_AGPS_SET_COLD_START
* sets cold start for comming session(s)
* \see AMIL_SetColdStart
*/
#define MC_AGPS_SET_COLD_START AMIL_SetColdStart


/**
* \def MC_AGPS_SET_SLP_ADDRESS
* sets slp address of the server
* \see AMIL_SetSlpAddress
*/
#define MC_AGPS_SET_SLP_ADDRESS AMIL_SetSlpAddress


/**
* \def MC_AGPS_SEND_USER_RESPONSE
* Sends user response of SUPL notification to the modem
* \see AMIL_SendUserResponse
*/
#define MC_AGPS_SEND_USER_RESPONSE AMIL_SendUserResponse


/**
* \def MC_AGPS_REGISTER_NOTIFICATION
* registers notification callback
* \see AMIL_RegisterNotificationCallback
*/
#define MC_AGPS_REGISTER_NOTIFICATION AMIL_RegisterNotificationCallback


/**
* \def MC_AGPS_SET_GPS_CONFIGURATION
* Sets gps configuration
* \see AMIL_SetGpsConfiguration
*/
#define MC_AGPS_SET_GPS_CONFIGURATION AMIL_SetGpsConfiguration

/**
* \def MC_AGPS_SWITCH_OFF_GPS
*
* \see AMIL_SwitchOffGps()
*/
#define MC_AGPS_SWITCH_OFF_GPS AMIL_SwitchOffGps


/* +LMSqc19265 -Anil */

/**
* \def MC_AGPS_SUPL_SMS_PUSH_IND
*
* \see AMIL_SendSuplPush()
*/
#define MC_AGPS_SUPL_SMS_PUSH_IND AMIL_SendSuplPush

/* -LMSqc19265 -Anil */

#define MC_AGPS_DELETE_NV_STORE_FIELD  AMIL_SendDeleteNvStoreFields

/*@} group MacroAmil */
#else

#define AGPS_SERVER_VERSION "AGPSL-2.8"

/**
* \def MC_ACGPS_SERVICE_START_REQUEST
* starts the GPS Service
* \see MC_CGPS_SERVICE_START
*/
#define MC_AGPS_SERVICE_START_REQUEST MC_CGPS_SERVICE_START


/**
* \def MC_AGPS_SERVICE_START_REQUEST_2
* starts the GPS Service
* \see MC_CGPS_SERVICE_START_2
*/
#define MC_AGPS_SERVICE_START_REQUEST_2         MC_CGPS_SERVICE_START_2



/**
* \def MC_AGPS_SERVICE_STOP_REQUEST
* stops the GPS Service
* \see MC_CGPS_SERVICE_STOP
*/
#define MC_AGPS_SERVICE_STOP_REQUEST MC_CGPS_SERVICE_STOP


/**
* \def MC_AGPS_PERIODIC_FIX_REQUEST
* registers a periodic fix
* \see MC_CGPS_REGISTER_PERIODIC_FIX
*/
#define MC_AGPS_PERIODIC_FIX_REQUEST MC_CGPS_REGISTER_PERIODIC_FIX


/**
* \def MC_AGPS_SINGLESHOT_FIX_REQUEST
* registers a single shot
* \see MC_CGPS_REGISTER_SINGLE_SHOT_FIX
*/
#define MC_AGPS_SINGLESHOT_FIX_REQUEST MC_CGPS_REGISTER_SINGLE_SHOT_FIX


/**
* \def MC_AGPS_REGISTER_FENCE_GEOGRAPHICAL_AREA
* registers a positioning service request which has event reporting based on geo fencing.
* \see MC_CGPS_REGISTER_FENCE_GEOGRAPHICAL_AREA
*/
#define MC_AGPS_REGISTER_FENCE_GEOGRAPHICAL_AREA SendFenceGeographicalAreaReq 

#if defined AGPS_FTR || defined AGPS_UP_FTR
/**
* \def MC_AGPS_REGISTER_LOCATION_FORWARD_REQUEST
* sends the location forward request to the third party
* \see MC_CGPS_REGISTER_LOCATION_FORWARD
*/
#define MC_AGPS_LOCATION_FORWARD_REQUEST SetGpsLocationForward
/**
* \def MC_AGPS_LOCATION_RETRIEVAL_REQUEST
* sends the request for the retrieval of the location of third party
* \see MC_REGISTER_LOCATION_RETRIEVAL
*/
#define MC_AGPS_LOCATION_RETRIEVAL_REQUEST  SetGpsLocationRetrieve
#endif



#define MC_AGPS_DELETE_NV_STORE_FIELD  MC_CGPS_DELETE_NV_STORE_FIELD

/**
* \def MC_AGPS_SEND_USER_RESPONSE
* Sends user response of SUPL notification to the modem
* \see SendUserResponse
*/
#define MC_AGPS_SEND_USER_RESPONSE SendUserResponse


/**
* \def MC_AGPS_REGISTER_NOTIFICATION
* registers notification callback
* \see RegisterNotificationCallback
*/
#define MC_AGPS_REGISTER_NOTIFICATION RegisterNotificationCallback


/**
* \def MC_AGPS_REGISTER_EXTENDED_EPH
* registers Extended Ephemeris request callback
* \see RegisterNotificationCallback
*/
#define MC_AGPS_REGISTER_EXTENDED_EPH RegisterEEClientCallback
/*XYBRID Integration :194997*/

/**
* \def MC_AGPS_REGISTER_REF_LOCATION
* registers Reference Position request callback
* \see RegisterEEClientRefLocationCallback
*/
#define MC_AGPS_REGISTER_REF_LOCATION RegisterEEClientRefLocationCallback

/**
* \def MC_AGPS_REGISTER_REF_LOCATION
* registers Reference Time request callback
* \see RegisterEEClientRefTimeCallback
*/
#define MC_AGPS_REGISTER_REF_TIME RegisterEEClientRefTimeCallback


/**
* \def MC_AGPS_REGISTER_DELETE_SEED
* registers Reference Time request callback
* \see RegisterEEClientRefTimeCallback
*/
#define MC_AGPS_REGISTER_DELETE_SEED RegisterEEClientDeleteSeedCallback

/**
* \def MC_AGPS_REGISTER_BCE_DATA
* registers BCE DATA  callback
* \see RegisterEEClientBCEDataCallback
*/
#define MC_AGPS_REGISTER_BCE_DATA RegisterEEClientBCEDataCallback

/*XYBRID Integration :194997*/

/**
* \def MC_AGPS_REGISTER_NOTIFICATION
* registers notification callback
* \see RegisterBearerRequestCallback
*/
#define MC_AGPS_REGISTER_BEARER RegisterBearerRequestCallback


/**
* \def MC_AGPS_SET_GPS_CONFIGURATION
* Sets gps configuration
* \see SetGpsConfiguration
*/
#define MC_AGPS_SET_GPS_CONFIGURATION SetGpsConfiguration

/**
* \def MC_AGPS_SET_GPS_LOGGING_CONFIGURATION
* Sets gps configuration
* \see SetGpsLoggingConfiguration
*/
#define MC_AGPS_SET_GPS_LOGGING_CONFIGURATION SetGpsLoggingConfiguration



/**
* \def MC_AGPS_SET_GPS_PLATFORM_CONFIGURATION
* Sets gps platform configuration
* \see SetGpsPlatformConfiguration
*/
#define MC_AGPS_SET_GPS_PLATFORM_CONFIGURATION SetGpsPlatformConfiguration


/**
* \def MC_AGPS_USER_CONFIG_REQ
* Gets gps configuration
* \see GetGpsConfiguration
*/
#define MC_AGPS_USER_CONFIG_REQ GetGpsConfiguration


/**
* \def MC_AGPS_START_BEARER_CNF
*
* \see StartBearerCnf
*/
#define MC_AGPS_START_BEARER_CNF StartBearerCnf


/**
* \def MC_AGPS_START_BEARER_ERR
*
* \see StartBearerErr
*/
#define MC_AGPS_START_BEARER_ERR StartBearerErr


/**
* \def MC_AGPS_CLOSE_BEARER_IND
*
* \see CloseBearerInd
*/
#define MC_AGPS_CLOSE_BEARER_IND CloseBearerInd


/**
* \def MC_AGPS_CLOSE_BEARER_CNF
*
* \see CloseBearerCnf
*/
#define MC_AGPS_CLOSE_BEARER_CNF CloseBearerCnf


/**
* \def MC_AGPS_CLOSE_BEARER_ERR
*
* \see CloseBearerErr
*/
#define MC_AGPS_CLOSE_BEARER_ERR CloseBearerErr



/**
* \def MC_AGPS_SUPL_TCPIP_CONNECT_CNF
*
* \see SuplTcpIpConnectCnf
*/
#define MC_AGPS_SUPL_TCPIP_CONNECT_CNF SuplTcpIpConnectCnf

/**
* \def MC_AGPS_SUPL_TCPIP_CONNECT_ERR
*
* \see SuplTcpIpConnectErr
*/
#define MC_AGPS_SUPL_TCPIP_CONNECT_ERR SuplTcpIpConnectErr

/**
* \def MC_AGPS_SUPL_TCPIP_DISCONNECT_IND
*
* \see SuplTcpIpDisconnectInd
*/
#define MC_AGPS_SUPL_TCPIP_DISCONNECT_IND SuplTcpIpDisconnectInd

/**
* \def MC_AGPS_SUPL_TCPIP_DISCONNECT_CNF
*
* \see SuplTcpIpDisconnectCnf
*/
#define MC_AGPS_SUPL_TCPIP_DISCONNECT_CNF SuplTcpIpDisconnectCnf


/**
* \def MC_AGPS_SUPL_TCPIP_DISCONNECT_ERR
*
* \see SuplTcpIpDisconnectErr
*/
#define MC_AGPS_SUPL_TCPIP_DISCONNECT_ERR SuplTcpIpDisconnectErr

/**
* \def MC_AGPS_SUPL_MOBILE_INFO_IND
*
* \see SuplMobileInfoInd
*/
#define MC_AGPS_SUPL_MOBILE_INFO_IND SuplMobileInfoInd

/**
* \def MC_AGPS_WLAN_INFO_IND
*
* \see SuplSmsPushInd
*/
#define MC_AGPS_WLAN_INFO_IND WLANInfoInd


/**
* \def MC_AGPS_SUPL_SMS_PUSH_IND
*
* \see SuplSmsPushInd
*/
#define MC_AGPS_SUPL_SMS_PUSH_IND SuplSmsPushInd


/**
* \def MC_AGPS_SUPL_SEND_DATA_CNF
*
* \see SuplSendDataCnf
*/
#define MC_AGPS_SUPL_SEND_DATA_CNF SuplSendDataCnf

/**
* \def MC_AGPS_SUPL_RECEIVE_DATA_IND
*
* \see SuplReceiveDataInd
*/
#define MC_AGPS_SUPL_RECEIVE_DATA_IND SuplReceiveDataInd

#define gpsClient_AgpsInit                InitAgps

#define MC_AGPS_SWITCH_OFF_GPS  MC_CGPS_DISABLE_GPS

#endif

/* +LMSqc19820 -Anil */

/**
* \def MC_AGPS_GET_SERVER_VERSION
*
* \see GetServerVersion
*/

#define MC_AGPS_GET_SERVER_VERSION GetServerVersion

/* -LMSqc19820 -Anil */


/**
* \def MC_AGPS_EE_GET_EPHEMERIS_RSP
*
* \see EEGetEphemerisRsp
*/
#define MC_AGPS_EE_GET_EPHEMERIS_RSP EEGetEphemerisRsp

/*XYBRID Integration :194997*/

/**
* \def MC_AGPS_EE_GET_REFLOCATION_RSP
*
* \see EEGetRefLocationRsp
*/

#define MC_AGPS_EE_GET_REFLOCATION_RSP EEGetRefLocationRsp

/*XYBRID Integration :194997*/

#endif /* #ifndef _AGPS_SERVER_INTERFACE_H */

