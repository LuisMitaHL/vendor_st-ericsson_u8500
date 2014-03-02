/**
 * \file mal_mce.h
 * \brief This file includes headers for MCE MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL MCE header file for MCE Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.5    Added netlink socket fd, used to communicate with shm driver
 * \n             for modem silent reboot.
 * \n
 * \n v0.4    Correction in enum mal_mce_action
 * \n         Removed declaration mce_cbfunc_event
 * \n
 * \n v0.3    Added enum mal_mce_action
 * \n         For MAL_MCE_RF_STATE_IND data, response
 * \n             data value is from mal_mce_action table
 * \n         The same done for MAL_MCE_RESET_RESP,
 * \n             MAL_MCE_WATCHDOG_CONTROL_RESP, MAL_MCE_SLEEP_CONTROL_RESP
 * \n         For added input param for mal_mce_enable_watchdog &
 * \n             mal_mce_enable_sleep_mode
 * \n         Added #include <stdint.h>
 */

/**
 * \defgroup  MAL_MCE MCE API
 * \par
 * \n This part describes the interface to MCE MAL Client
 */

#ifndef MAL_MCE_H
#define MAL_MCE_H

#include <stdint.h>

#define MCE_LIB_VERSION "MCE Lib V 0.4.r2"

/*----------------------------------------------------------------------------*/
/* Event IDs                                                                  */
/*----------------------------------------------------------------------------*/

/**
 * \def MAL_MCE_RF_STATE_RESP
 * \brief RF State Change Response
 *
 * \n This response is received after sending the request to the modem to
 * \n change the state of RF.
 */
#define MAL_MCE_RF_STATE_RESP 0x04

/**
 * \def MAL_MCE_RF_STATE_IND
 * \brief RF State Change Indication
 *
 * \n This is an unsolicited indication reporting the change in RF state.
 */
#define MAL_MCE_RF_STATE_IND 0x05

/**
 * \def MAL_MCE_RF_STATE_QUERY_RESP
 * \brief RF State Query Response
 *
 * \n This response is received after sending the request to the modem to query
 * \n the state of RF.
 */
#define MAL_MCE_RF_STATE_QUERY_RESP 0x07

/**
 * \def MAL_MCE_POWER_OFF_RESP
 * \brief Power off response
 *
 * \n This response is received after sending the request to the modem to
 * \n power off modem.
 */
#define MAL_MCE_POWER_OFF_RESP 0x09

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_mce_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the MCE MAL Library. These error types
 * \n are used as return values for the API routines.
 */
typedef enum {
    MAL_MCE_SUCCESS,                     /**< Success  */
    MAL_MCE_FAIL,                        /**< Failure */
    MAL_MCE_R_GENERAL_ERROR,             /**< Request can not be completed for general error */
    MAL_MCE_R_COMPLETED,                 /**< Request successfully already completed  */
    MAL_MCE_R_NOT_INITIALIZED,           /**< Request can not be completed because modem libraries are not initialized.  */
    MAL_MCE_R_REQUEST_NOT_SUPPORTED,     /**< Request not supported  */
    MAL_MCE_R_INCORRECT_PARAMETERS,      /**< Request rejected because parameters provided are incorrect */
    MAL_MCE_R_MODEM_TRANSACTION_ONGOING, /**< Modem state not established, then not possible to send any ISI message to modem*/
    MAL_MCE_R_MODEM_RESET,               /**< Modem is off or reset, it means that host must reload modem SW and reset modem */
    MAL_MCE_R_MODEM_NOT_READY,           /**< Modem is not ready */
    MAL_MCE_R_BUSY,                      /**< Request can not be treated because another request is ongoing  */
    MAL_MCE_R_ALREADY_ACTIVE,            /**< Requested state is already active */
    MAL_MCE_R_SOCK_SEND_ERROR,           /**< Error on socket send */
    MAL_MCE_R_SOCK_CONN_ERROR            /**< Error on socket connection */
} mal_mce_error_type;

/**
 * \enum mal_mce_rf_status
 * \brief RF status
 *
 * \n This enum defines the RF status.
 */
typedef enum {
    MAL_MCE_RF_OFF        = 0x00, /**< RF is on */
    MAL_MCE_RF_ON         = 0x01, /**< RF is off */
    MAL_MCE_RF_TRANSITION = 0X02  /**< RF is in transition mode */
} mal_mce_rf_status;

/**
 * \enum mal_mce_status
 * \brief MCE status
 *
 * \n This enum defines the MCE status.
 */
typedef enum {
    MAL_MCE_OK                 = 0x00, /**< Request succeded */
    MAL_MCE_REQ_FAIL           = 0x01, /**< Request failed */
    MAL_MCE_NOT_ALLOWED        = 0X02, /**< Requested state not allowed */
    MAL_MCE_ALREADY_ACTIVE     = 0X06, /**< Requested state already active */
    MAL_MCE_TRANSITION_ONGOING = 0x16, /**< Transition in progress */
    MAL_MCE_RESET_REQUIRED     = 0x17  /**< Reset required to fulfil the request */
} mal_mce_status;

/**
 * \typedef void (*mal_mce_event_cb_t)(int event_id, void *data,
 *                                mal_mce_error_type error_code, void *client_tag)
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
 * \arg MAL_MCE_RF_STATE_QUERY_RESP
 * \n Pointer to \ref enum mal_mce_rf_status
 * \par
 * \arg MAL_MCE_RF_STATE_RESP
 * \n Pointer to \ref enum mal_mce_status
 * \par
 * \arg MAL_MCE_RF_STATE_IND
 * \n Pointer to \ref enum mal_mce_rf_status
 */
typedef void (*mal_mce_event_cb_t)(int event_id, void *data,
                                   mal_mce_error_type error_code, void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_mce_init(int32_t *fd_mce, int32_t *fd_netlnk)
 * \brief Initialize MCE Library
 *
 * \n This routine is used to initialize the MCE Library and open the phonet socket
 * \n using Phonet Liibrary and netlink socket using netlink library.
 * \n This routine should be called only once at the time of initialization.
 * \param [out] fd_net    Pointer to file descriptor of MCE socket
 * \param [out] fd_netlnk Pointer to file descriptor of netlink socket
 * \return    \ref        mal_net_error_type
 */
int32_t mal_mce_init(int32_t *fd_mce, int32_t *fd_netlnk);

/**
 * \fn int32_t mal_mce_deinit(void)
 * \brief De-initialize MCE Library
 *
 * \n This routine is used to de-initialize the MCE Library.
 * \param        void
 * \return \ref  mal_mce_error_type
 */
int32_t mal_mce_deinit(void);

/**
 * \fn int32_t mal_mce_config(void)
 * \brief Configure MCE Library
 *
 * \n This routine is used to configure the MCE library.
 * \return mal_mce_error_type
 *
 */
int32_t mal_mce_config(void);

/**
 * \fn int32_t mal_mce_register_callback(mal_mce_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * \n This routine registers the callback function for passing events
 * \n to the Application.
 * \param [in] event_cb   event callback handler.
 * \return    \ref        mal_mce_error_type
 */
int32_t mal_mce_register_callback(mal_mce_event_cb_t event_cb);

/**
 * \fn int32_t mal_mce_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \return    void
 */
void mal_mce_response_handler(void);

/**
 * \fn int32_t mal_mce_rf_status_query(void *client_tag)
 * \brief Query for current RF state
 *
 * \n This routine is used to query the RF status. Expected event type
 * \n in case of successful invocation of this routine is
 * \n MAL_MCE_RF_STATE_QUERY_RESP.
 *
 * \param    [in] client_tag   Client tag
 * \return \ref                mal_mce_error_type
 */
int32_t mal_mce_rf_status_query(void *client_tag);

/**
 * \fn int32_t mal_mce_rf_on(void *client_tag)
 * \brief Request to turn RF on
 *
 * \n This routine is used to turn the RF on. Expected event type
 * \n in case of successful invocation of this routine is MAL_MCE_RF_STATE_RESP.
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_mce_error_type
 */
int32_t mal_mce_rf_on(void *client_tag);

/**
 * \fn int32_t mal_mce_rf_off(void *client_tag)
 * \brief Request to turn RF off
 *
 * \n This routine is used to turn the RF off. Expected event type
 * \n in case of successful invocation of this routine is MAL_MCE_RF_STATE_RESP.
 * \param [client_tag]    Void pointer to client specific data. The client
 *                            specific data is transparent to MAL and is
 *                            returned as-is in the response.
 *   \return \ref         mal_mce_error_type
 */
int32_t mal_mce_rf_off(void *client_tag);

/**
 * \fn int32_t mal_mce_reset(void *client_tag)
 * \brief Request to recet MCE
 *
 * \n This routine is used to reset the MCE. Expected event type
 * \n in case of successful invocation of this routine is <none, as of now>.
 * \param [in]client_tag    Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    \ref          mal_mce_error_type
 * Not USED currently
 */
int32_t mal_mce_reset(void *client_tag);

/**
 * \fn int32_t mal_mce_modem_switch_off(void *client_tag)
 * \brief Request to switch off the modem
 *
 * \n This routine is used to switch the MCE off. Expected event type
 * \n in case of successful invocation of this routine is <none, as of now>.
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    \ref           mal_mce_error_type
 */

int32_t mal_mce_modem_switch_off(void *client_tag);


/**
 *  \fn int32_t mal_mce_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for mce module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mce_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_mce_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for mce module. This is a synchronous call.
 *   \param[out] debug level for mis  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mce_request_get_debug_level(uint8_t *level);

#endif /*MAL_MCE_H */
