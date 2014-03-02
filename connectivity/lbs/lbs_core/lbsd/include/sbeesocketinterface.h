/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *
 *****************************************************************************/
/**
* \file sbeesocketinterface.h
* \date 25/10/2009
* \version 1.0
*
* <B>Description:</B> This file contain all type definition provided by sbee to external world \n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.10.09</TD><TD> ANIL </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#ifndef __SBEE_SOCKET_INTERFACE_H__
#define __SBEE_SOCKET_INTERFACE_H__


//#ifndef _AGPS_CLIENT_c
//#include "sbee.h"
//#endif

#include "agpsptypes.h"
/**
* \enum  e_sbee_si_UserNotificationType
* List of constants used to specify notification .
*/
typedef enum
{
      SBEE_USER_NOTIFICATION_ONLY,   /**< User is notified. */
      SBEE_USER_ACCEPTANCE_REQD,     /**< User is notified and asked for acceptance. Request will be denied if user does not answer */
} e_sbee_si_UserNotificationType;


typedef enum
{
    SBEE_NETWORK_NONE     = 0,
    SBEE_NETWORK_HOME     = 1,
    SBEE_NETWORK_ROAMING  = 2,
} e_sbee_si_NetworkStatus;


/*----------------------------------------------------------------------*/

typedef enum
{
    SBEE_SEED_NEVER                      = 0,
    SBEE_SEED_MANUAL                     = 1,
    SBEE_SEED_AUTO_ALWAYS                = 2,
    SBEE_SEED_AUTO_ONLY_ON_HOME_NETWORK  = 3,
    SBEE_SEED_AUTO_ONLY_ON_ROAMING       = 4,
    SBEE_SEED_AUTO_ALWAYS_ASK            = 5,
} e_sbee_si_ServerConnectionOption;

/*----------------------------------------------------------------------*/

typedef enum
{
    SBEE_PREF_BEARER_ANY                = 0,
    SBEE_PREF_BEARER_CELLULAR           = 1,
    SBEE_PREF_BEARER_WIFI               = 2,
} e_sbee_si_PreferredBearerOption;

/*----------------------------------------------------------------------*/

typedef enum
{
    SBEE_BEARER_NONE               = 0,
    SBEE_BEARER_CELLULAR           = 1,
    SBEE_BEARER_WIFI               = 2,
} e_sbee_si_BearerStatus;

/*----------------------------------------------------------------------*/

typedef enum
{
    SBEE_USER_RESPONSE_ALLOWED           = 0,
    SBEE_USER_RESPONSE_REJECTED          = 1,
} e_sbee_si_UserResponseOption;



typedef enum
{
    SBEE_DATA_CONNECTION_REQUEST_SUCCESS    = 0,
    SBEE_DATA_CONNECTION_REQUEST_FAIL       = 1,
    SBEE_DATA_DISCONNECTION_REQUEST_SUCCESS = 2,
    SBEE_DATA_DISCONNECTION_REQUEST_FAIL    = 3,
} e_sbee_si_DataConnectionStatus;


/*----------------------------------------------------------------------*/

typedef enum
{
    SBEE_PREDICTION_PREF_CHARGING_ONLY   = 0,
    SBEE_PREDICTION_PREF_NONE            = 1,
} e_sbee_si_ChargingModeOption;

/*----------------------------------------------------------------------*/

typedef enum
{
    SBEE_ALTERNATE_SERVER_RETRY_ALLOWED       = 0,
    SBEE_ALTERNATE_SERVER_RETRY_NOT_ALLOWED   = 1
} e_sbee_si_ServerRetryOption;


/*----------------------------------------------------------------------*/

// Maximum Length of Device_ID string.
#define MAX_SBEE_DEVICE_ID_LEN              50
#define MAX_SBEE_INTEGRATOR_MODEL_NAME      50
#define MAX_SBEE_SERVERS                    3


/**
*\struct t_sbeesdm_server_configuration
*\brief parameters for server configuration
*
*/
typedef struct
{
    int8_t                        v_DeviceId[MAX_SBEE_DEVICE_ID_LEN];
    int8_t                        v_IntegratorModelId[MAX_SBEE_INTEGRATOR_MODEL_NAME];
    e_sbee_si_ServerRetryOption v_RetryOption;
    uint8_t                     v_NumSbeeServers;
    uint8_t                     v_SbeeServersLength[MAX_SBEE_SERVERS];
    int8_t*                       v_SbeeServers[MAX_SBEE_SERVERS];
} t_sbee_si_ServerConfiguration;


/**
*\struct t_sbeesdm_user_configuration
*\brief parameters for user configuration
*
*/
typedef struct
{
    e_sbee_si_ServerConnectionOption v_ServerConnectionOption;
    e_sbee_si_PreferredBearerOption  v_PrefBearerOption;
    e_sbee_si_ChargingModeOption     v_ChargingModeOption;
    uint8_t                          v_EEOptionMask;
    uint32_t                          v_EESeedDownloadFreq;
    uint32_t                          v_EEMaxAllowedSeedDownload;
    uint16_t                         v_PrefTimeOfDay;
} t_sbee_si_UserConfiguration;


/**
*\struct t_sbee_UserNotificationParams
*\brief parameters for user notification
*
* SBEE uses these parameters in callback to notify user or cancel notification of external connection
*
*/
typedef struct {
    e_sbee_si_UserNotificationType v_NotificationType;
    unsigned short                 v_TimeoutSec;
    uint32_t                       v_Handle;
} t_sbee_si_UserNotificationParams;


/**
*\struct t_sbee_DataConnectionRequestParms
*\brief parameters for requeting a data connection
*
* SBEE uses these parameters in callback to initiate a data connection
*
*/
typedef struct {
    uint32_t  v_handle;
        t_agps_U8Array a_ServerAddress;
    uint16_t  v_PortNumber;
} t_sbee_si_DataConnectionRequestParams;


/**
*\struct t_sbee_dataConnectionCancelParms
*\brief parameters for cancelling a data connection
*
* SBEE uses these parameters in callback to initiate a data connection
*
*/
typedef struct {
    uint32_t  v_handle;
} t_sbee_si_DataConnectionCancelParams;


/**
*\struct t_sbee_SendDataParams
*\brief parameters for cancelling a data connection
*
* SBEE uses these parameters in callback to initiate a data connection
*
*/
typedef struct {
    uint32_t  v_handle;
    t_agps_U8Array a_SbeeData;
} t_sbee_si_SbeeDataParams;


/**
*\struct t_sbee_si_SbeeStart
*\brief parameters for starting extended ephemeris
*
* uses these parameters in for starting up
*
*/
typedef struct {
    uint16_t  startTime ;
} t_sbee_si_SbeeStart;


/**
*\struct t_sbee_si_status_message
*\brief holds status messages for varius events
*
* uses this structure for storing status messages for various events
*
*/
typedef struct {
    uint32_t  status;
}t_sbee_si_status_message;

/**
*\struct t_sbee_si_status_message_with_handle
*\brief holds status messages for varius events
*
* uses this structure for storing status messages for various events
*
*/
typedef struct {
    uint32_t  handle;
    uint32_t  status;
}t_sbee_si_status_message_with_handle;



#endif /* __SBEE_SOCKET_INTERFACE_H__ */

