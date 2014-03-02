/**
 * \file mal_gss.h
 * \brief This file includes headers for GSS MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL GSS header file for GSS Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.4   Added support for timing advance
 * \n
 * \n v0.3   Added support for HSXPA capability control
 * \n
 * \n v0.2   Added mal_gss_set_quick_release_mode to set user
 * \n        activity mode for Power saving enable and disable
 * \n
 * \n v0.1   Initial
 */

/**
 * \defgroup  MAL_GSS GSS API
 * \par
 * \n This part describes the interface to GSS MAL Client
 */

#ifndef MAL_GSS_H
#define MAL_GSS_H

#include <stdint.h>

#define GSS_LIB_VERSION "GSS Lib V 0.4.r2"

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/**
 * \def MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP
 * \brief Set Preferred Network Type Response
 *
 * \n This response is received after sending the request to modem to set the
 * \n Preferred Network Type.
 */
#define MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP 0x01

/**
 * \def MAL_GSS_GET_PREFERRED_NETWORK_TYPE_RESP
 * \brief Get Preferred Network Type Response
 *
 * \n This response is received after sending the request to modem to get the
 * \n Preferred Network Type.
 */
#define MAL_GSS_GET_PREFERRED_NETWORK_TYPE_RESP 0x02

/**
 * \def MAL_GSS_QUERY_AVAILABLE_BAND_MODE_RESP
 * \brief Get Available Band Mode Response
 *
 * \n This response is received after sending the request to modem to get the
 * \n Available Band Mode.
 */
#define MAL_GSS_QUERY_AVAILABLE_BAND_MODE_RESP 0x03

/**
 * \def MAL_GSS_SET_USER_ACTIVITY_INFO_RESP
 * \brief Set User Activity Mode Type Response
 *
 * \n This response is received after sending the request to modem to set the
 * \n User Activity Mode.
 */
#define MAL_GSS_SET_USER_ACTIVITY_INFO_RESP  0x04

/**
 * \def MAL_GSS_SET_USER_ACTIVITY_INFO_IND
 * \brief Set User Activity Mode Type Indication
 *
 * \n This indication is received after sending the request to modem to set the
 * \n User Activity Mode.
 */
#define MAL_GSS_SET_USER_ACTIVITY_INFO_IND  0x05

/**
 * \def MAL_GSS_HSXPA_USER_SETTING_WRITE_RESP
 * \brief Set GSS HSxPA Mode Type Indication
 *
 * \n This indication is received after sending the request to modem to set the
 * \n HSxPA Mode.
 */
#define MAL_GSS_HSXPA_USER_SETTING_WRITE_RESP 0x10

/**
 * \def MAL_GSS_HSXPA_USER_SETTING_READ_RESP
 * \brief Get GSS HSxPA Mode Type Indication set by user
 *
 * \n This indication is received after sending the request to modem to set the
 * \n HSxPA Mode.
 */
#define MAL_GSS_HSXPA_USER_SETTING_READ_RESP 0x13

/**
 * \def MAL_GSS_TIMING_ADVANCE_RESP
 * \brief Get timing advance information
 *
 * \n This indication is received after sending the request to modem to get the
 * \n timing advance information.
 */
#define MAL_GSS_TIMING_ADVANCE_RESP 0x14

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_gss_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the GSS MAL Library. These error types
 * \n are used as return values for the API routines.
 */
typedef enum {
    MAL_GSS_SUCCESS             = 0, /**< Success  */
    MAL_GSS_FAIL                = 1, /**< Failure */
    MAL_GSS_GENERAL_ERROR       = 2, /**< Request cannot be completed because of
                                      * a general error
                                      */
    MAL_GSS_MODE_NOT_SUPPORTED  = 3, /**< Requested mode not supported */
    MAL_GSS_SERVICE_NOT_ALLOWED = 4, /**< Requested service not allowed */
    MAL_GSS_INVALID_BLOCK_TYPE  = 5, /**< Block type not correct */
    MAL_GSS_INSUFFICIENT_MEMORY = 6, /**< Memory allocation failure */
    MAL_GSS_INVALID_DATA        = 7, /**< Invalid data passed to the routine */
    MAL_GSS_SOCK_SEND_ERROR     = 8  /**< Send operation on socket failed */
} mal_gss_error_type;

/**
 * \enum mal_gss_network_type
 * \brief Network types
 *
 * \n This enum defines network types for the GSS MAL Library.
 */
typedef enum {
    MAL_GSS_GSM_WCDMA_PREFERRED_WCDMA     = 0, /**< GSM/WCDMA (WCDMA preferred) */
    MAL_GSS_GSM_ONLY                      = 1, /**< GSM only */
    MAL_GSS_WCDMA_ONLY                    = 2, /**< WCDMA only */
    MAL_GSS_GSM_WCDMA_AUTO_MODE           = 3, /**< GSM/WCDMA (auto mode, according to PRL) */
    MAL_GSS_CDMA_EVDO_AUTO_MODE           = 4, /**< CDMA and EvDo (auto mode, according to PRL) */
    MAL_GSS_CDMA_ONLY                     = 5, /**< CDMA only */
    MAL_GSS_EVDO_ONLY                     = 6, /**< EvDo only */
    MAL_GSS_GSM_WCDMA_CDMA_EVDO_AUTO_MODE = 7  /**< GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL) */
} mal_gss_network_type;

/**
 * \enum mal_gss_user_activity_mode
 * \brief activity status
 *
 * \n This enum defines the user activity status
 */
typedef enum {
    MAL_GSS_USER_STATUS_UNKNOWN = 0,            /**< Received only in response;
                                                 * will ignore mal_gss_user_activity_mode in
                                                 * mal_gss_set_environment_information.
                                                 */
    MAL_GSS_USER_ACTIVE = 1,                    /**< User is active */
    MAL_GSS_USER_INACTIVE = 2                   /**< User in inactive */
} mal_gss_user_activity_mode;

/**
 * \enum mal_gss_battery_info
 * \brief battery charging status.
 *
 * \n This enum defines the battery charging status.
 */
typedef enum {
    MAL_GSS_BATTERY_INFO_UNKNOWN = 0,           /**< Received only in response;
                                                 * will ignore mal_gss_battery_info in
                                                 * mal_gss_set_environment_information
                                                 */
    MAL_GSS_BATTERY_INFO_IN_CHARGER = 1,        /**< Battery charging */
    MAL_GSS_BATTERY_INFO_NORMAL = 2,            /**< Battery normal */
    MAL_GSS_BATTERY_INFO_LOW = 3                /**< Battery low */
} mal_gss_battery_info;

/**
 * \struct mal_gss_env_info_t
 * \brief Environment information.
 *
 * \n This struct defines environment status
 * \n such as battery status and display activity.
 */
typedef struct {
  mal_gss_user_activity_mode           user_activity_mode;
  mal_gss_battery_info                 battery_info;
} mal_gss_env_info_t;

/**
 * \enum mal_gss_hsxpa_mode
 * \brief activity status
 *
 * \n This enum defines the user activity status
 */
typedef enum {
    MAL_GSS_HSXPA_DISABLE = 1,   /**< Disable both HSUPA and HSDPA */
    MAL_GSS_HSXPA_ENABLE = 2     /**< Enable both HSUPA and HSDPA */
} mal_gss_hsxpa_mode;

/**
 * \enum mal_gss_timing_advance_validity
 * \brief Timing advance information validity
 *
 * \n This enum defines timing advance information validity types.
 */
typedef enum {
    MAL_GSS_TIMING_ADVANCE_NOT_VALID = 0x00, /**< Timing advance not valid */
    MAL_GSS_TIMING_ADVANCE_VALID     = 0x01  /**< Timing advance valid */
} mal_gss_timing_advance_validity;

/**
 * \struct mal_gss_timing_advance_info
 * \brief Timing advance information
 *
 * \n This structure defines timing advance information.
 */
typedef struct {
    mal_gss_timing_advance_validity validity; /**< Validity of information */
    uint8_t                         value;    /**< Timing advance value:
                                                * Valid range: 0 to 63;
                                                * 0xFF - Not available
                                                */
} mal_gss_timing_advance_info;

/**
 * \typedef void (*mal_gss_event_cb_t)(int32_t event_id, void *data,
 *                            mal_gss_error_type error_code, void *client_tag)
 * \brief Event notification to Application
 *
 * \n Callback function for event notification to the Application. This callback is
 * \n used for both solicited responses and unsolicited indications.
 *
 * \param [in] event_id      ID of the event being notified
 * \param [in] data          Pointer to data associated with the event being notified
 * \param [in] error_code    Error code associated with the event
 * \param [in] client_tag    Client tag associated with the response or indication
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP
 * \n NULL
 * \par
 * \arg MAL_GSS_GET_PREFERRED_NETWORK_TYPE_RESP
 * \n Pointer to \ref mal_gss_network_type
 * \par
 * \arg MAL_GSS_QUERY_AVAILABLE_BAND_MODE_RESP
 * \par
 * \arg MAL_GSS_SET_USER_ACTIVITY_INFO_RESP
 * \par
 * \arg MAL_GSS_HSXPA_USER_SETTING_WRITE_RESP
 * \par
 * \arg MAL_GSS_HSXPA_USER_SETTING_READ_RESP
 * \n Pointer to \ref mal_gss_hsxpa_mode type
 * \par
 * \arg MAL_GSS_SET_USER_ACTIVITY_INFO_IND
 * \par
 * \n In the current implementation this event will not be received.
 * \par
 * \arg MAL_GSS_TIMING_ADVANCE_RESP
 * \n Pointer to \ref mal_gss_timing_advance_info
 */
typedef void (*mal_gss_event_cb_t)(int32_t event_id, void *data,
                                   mal_gss_error_type error_code, void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_gss_init(int32_t *fd_gss)
 * \brief Initialize GSS Library
 *
 * \n This routine is used to initialize the GSS Library and open the socket
 * \n using Phonet Liibrary. This routine should be called only once at the time
 * \n of initialization.
 * \param [out] fd_gss    Pointer to file descriptor of GSS socket
 * \return    \ref        mal_gss_error_type
 */
int32_t mal_gss_init(int32_t *fd_gss);

/**
 * \fn int32_t mal_gss_deinit(void)
 * \brief De-initialize GSS Library
 *
 * \n This routine is used to de-initialize the GSS Library.
 * \param            void
 * \return    \ref   mal_gss_error_type
 */
int32_t mal_gss_deinit(void);

/**
 * \fn int32_t mal_gss_register_callback(mal_gss_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * \n This routine registers the callback function for passing events
 * \n to the Application.
 * \param [in] event_cb       Event callback handler
 * \return    \ref            mal_gss_error_type
 */
int32_t mal_gss_register_callback(mal_gss_event_cb_t event_cb);

/**
 * \fn void mal_gss_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \param    void
 * \return    void
 */
void mal_gss_response_handler(void);

/**
 * \fn int32_t mal_gss_set_preferred_network_type(mal_gss_network_type type, void *client_tag);
 * \brief Request to set preferred network type
 *
 * \n This routine is called by Application to set the preferred network type.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_GSS_SET_PREFERRED_NETWORK_TYPE_RESP.
 *
 * \param [in] type    \ref  mal_gss_network_type
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_set_preferred_network_type(mal_gss_network_type type,
        void *client_tag);

/**
 * \fn int32_t mal_gss_get_preferred_network_type(void *client_tag)
 * \brief Request to get preferred network type
 *
 * \n This routine is called by Application to get the preferred network type.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_GSS_GET_PREFERRED_NETWORK_TYPE_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                               specific data is transparent to MAL and is
 *                               returned as-is in the response.
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_get_preferred_network_type(void *client_tag);

/**
 * \fn int32_t mal_gss_query_available_band_modes(uint32_t *modes, void *client_tag)
 * \brief Query available band modes
 *
 * \n This routine is called by Application to query available band mode. Since
 * \n modem does not support this feature at the moment, this is a synnchronous
 * \n call as a place-holder. No request is sent out to modem and band mode
 * \n remains "unspecified". This is a synchronous call; there is no event
 * \n expected.
 *
 * \param [out] modes        Available band modes to choose from.
 *                                modes[0] specifes number of modes
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_query_available_band_modes(uint32_t **modes, void *client_tag);

/**
 * \fn int32_t mal_gss_set_band_mode(uint32_t mode, void *client_tag)
 * \brief Request to set a specific band
 *
 * \n This routine is called by Application to set a band mode. Since modem
 * \n does not support this feature at the moment, this is a synnchronous
 * \n call as a place-holder. No request is sent out to modem and band mode
 * \n remains "unspecified". Hence this routine will return error if mode
 * \n requested is non-zero. This is a synchronous call; there is no event
 * \n expected.
 *
 * \param [in] mode          Requested band mode, only 0 (unspecified) supported
 *                                as of now, specifying the "automatic" mode
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_set_band_mode(uint32_t mode, void *client_tag);

/**
* \fn int32_t mal_gss_set_environment_information(mal_gss_env_info_t env_info, void *client_tag)
* \brief Request to set environment information
*
* \n This routine is called by Application to set environment information for
* \n power saving enable and disable. Expected event type in case of successful
* \n invocation of this routine is MAL_GSS_SET_USER_ACTIVITY_INFO_RESP
* \n
*
* \param [in] env_info          Requested environment information
*
* \param [in] client_tag    Void pointer to client specific data. The client
*                                specific data is transparent to MAL and is
*                                returned as-is in the response.
*
* \return    \ref           mal_gss_error_type
*/
int32_t mal_gss_set_environment_information(mal_gss_env_info_t *env_info, void *client_tag);

/**
 * \fn int32_t mal_gss_set_hsxpa_mode(mal_gss_hsxpa_mode mode,void *client_tag)
 * \brief Request to set the HSxPA mode
 *
 * \n This routine is called by Application to set the HSxPA mode
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_GSS_HSXPA_USER_SETTING_WRITE_RESP
 * \n
 *
 * \param [in] mode          Requested user activity mode
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 *
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_set_hsxpa_mode(mal_gss_hsxpa_mode mode, void *client_tag);


/**
 * \fn int32_t mal_gss_get_hsxpa_mode(void *client_tag)
 * \brief Request to get HSxPA setting
 *
 * \n This routine is called by Application to get HSxPA setting
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_GSS_HSXPA_USER_SETTING_READ_RESP
 * \n
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 *
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_get_hsxpa_mode(void *client_tag);

/**
 * \fn int32_t mal_gss_get_timing_advance_value(void *client_tag)
 * \brief Query timing advance value
 *
 * \n This routine is called by Application to query timing advance value.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_GSS_TIMING_ADVANCE_RESP
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_gss_error_type
 */
int32_t mal_gss_get_timing_advance_value(void *client_tag);

/**
 *  \fn int32_t mal_gss_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for gss  module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_gss_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_gss_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for gss module. This is a synchronous call.
 *   \param[out] debug level for gss  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_gss_request_get_debug_level(uint8_t *level);

#endif /* MAL_GSS_H */
