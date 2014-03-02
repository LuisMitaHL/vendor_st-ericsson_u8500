/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  MAL RF header file for AT Library.
 *
 *  Author: Mathias Edman <mathias.edman@stericsson.com>
 *
 *  Revision History:
 *
 * v0.2    Add mal_rf_send_tx_backoff_event
 * v0.1    Initial
 */


#ifndef MAL_RF_H
#define MAL_RF_H

#include <stdint.h>
#include <stdbool.h>

#define RF_LIB_VERSION "RF Lib V 0.2.r1"

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/** \def MAL_RF_SEND_TX_BACK_OFF_EVENT_RESP
 *  \brief TX back off response
 *
 *  \n This response is received after sending TX back off request to modem.
 */
#define MAL_RF_SEND_TX_BACK_OFF_EVENT_RESP 0x28

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * enum mal_rf_error_type
 * brief Error types
 *
 * This enum defines error types for the RF MAL Library. These error types
 * are used as return values for the API routines and with events.
 */
typedef enum {

    MAL_RF_SUCCESS             = 0, /**< Success  */
    MAL_RF_GENERAL_ERROR       = 1, /**< Request cannot be completed because of
                                       * a general error */
    MAL_RF_FAIL                = 2, /**< Failure */
    MAL_RF_INVALID_DATA        = 3, /**< Invalid data passed to the routine */
    MAL_RF_SOCK_SEND_ERROR     = 4  /**< Send operation on socket failed */
} mal_rf_error_type;


/**
 * enum mal_rf_resource_type
 *
 * This enum specifies the different resources that can be used in this lib
 */
typedef enum {
    MAL_RF_PN_COMMON_RF_TEST,
    MAL_RF_PN_WCDMA_RF_TEST,
    MAL_RF_PN_GSM_RF_TEST,
    MAX_RESOURCE
} mal_rf_resource_type;


/**
 * \enum mal_rf_tx_back_off_event
 * \brief Event intended for the modem so that TX power reduction
 * can be activated/deactivated (e.g. FCC regulated requirement).
 */
typedef enum {
    MAL_RF_TX_BACK_OFF_EVENT_NO_EVENT                        = 0,
    MAL_RF_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED     = 1,
    MAL_RF_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN       = 2,
    MAL_RF_TX_BACK_OFF_EVENT_DVBH_INACTIVE                   = 3,
    MAL_RF_TX_BACK_OFF_EVENT_DVBH_ACTIVE                     = 4,
    MAL_RF_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_FAR            = 5,
    MAL_RF_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR           = 6,
    MAL_RF_TX_BACK_OFF_EVENT_GPS_INACTIVE                    = 7,
    MAL_RF_TX_BACK_OFF_EVENT_GPS_ACTIVE                      = 8,
    MAL_RF_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE                = 9,
    MAL_RF_TX_BACK_OFF_EVENT_ANTENNA_ACTIVE                  = 10,
    MAL_RF_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_INACTIVE     = 11,
    MAL_RF_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_ACTIVE       = 12,
    MAL_RF_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_INACTIVE           = 13,
    MAL_RF_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE             = 14,
    MAL_RF_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_INACTIVE = 15,
    MAL_RF_TX_BACK_OFF_EVENT_SPURIOUS_BATTERY_STATE_ACTIVE   = 16,
    MAL_RF_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_INACTIVE      = 17,
    MAL_RF_TX_BACK_OFF_EVENT_LOW_BATTERY_STATE_ACTIVE        = 18
} mal_rf_tx_back_off_event;


/*---------------------------------------------------------------------------*/
/* RF MAL API definitions                                                    */
/*---------------------------------------------------------------------------*/

/**
 * typedef void (*mal_rf_event_cb_t)(int32_t event_id, void *data, mal_rf_error_type error_code, void *client_tag)
 *
 * Callback function for data.
 *
 * param [in] event_id    Event id
 * param [in] data        Pointer to the data received
 * param [in] error_code  Error code
 * param [in] client_tag  Pointer to client tag
 *
*/
typedef void (*mal_rf_event_cb_t)(int32_t event_id, void *data, mal_rf_error_type error_code, void *client_tag);


/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/


/**
 * int32_t mal_rf_init(int32_t *fd_rf_p)
 *
 * \n This routine is used to initialize the RF Library and open the socket
 * \n using Phonet Library. This routine should be called only once at the time
 * \n of initialization.
 *
 * param [out] fd_rf_p          descriptor for socket
 * return                       mal_rf_error_type
 */
int32_t mal_rf_init(int32_t *fd_rf_p);


/**
 * int32_t mal_rf_deinit(void)
 *
 * \brief De-initialize RF Library
 *
 * \n This routine is used to de-initialize the RF Library.
 *
 * return                       mal_rf_error_type
 */
int32_t mal_rf_deinit(void);


/**
 * int32_t mal_rf_register_callback(mal_rf_event_cb_t event_cb)
 *
 * Register callbacks from application
 *
 * This routine registers the callback function for data received
 *
 * param [in] event_cb      Data callback handler
 * return                   mal_rf_error_type
 */
int32_t mal_rf_register_callback(mal_rf_event_cb_t event_cb);


/**
 * \fn void mal_rf_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \param    void
 * \return   void
 */
void mal_rf_response_handler(void);


/**
 * \fn cn_error_code_t mal_rf_send_tx_back_off_event(mal_rf_tx_back_off_event event, void *client_tag);
 * \brief Send TX back off event to the modem.
 *
 * \n This routine is used to send a TX back off event to the modem. The modem will decide if
 * \n it is relevant to activate or deactivate TX power reduction based on the overall state
 * \n of all the back off categories. This functionality is needed due to FCC regulations.
 * \n
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_RF_SEND_TX_BACK_OFF_EVENT_RESP
 *
 * \param [in] event           TX back off event.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * return                      mal_rf_error_type
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
int32_t mal_rf_send_tx_back_off_event(mal_rf_tx_back_off_event event, void *client_tag);

#endif /* MAL_RF_H */


