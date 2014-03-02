/**
 * \file mal_mis.h
 * \brief This file includes headers for MIS MAL.
 *
  * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL MIS header file for MIS Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.1    Initial
 */

/**
 * \defgroup  MAL_MIS MIS API
 * \par
 * \n This part describes the interface to MIS MAL Client
 */

#ifndef MAL_MIS_H
#define MAL_MIS_H

#include <stdint.h>

#define MIS_LIB_VERSION "MIS Lib V 0.1.r2"

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/**
 * \def MAL_MIS_VERSION_READ_RESP
 * \brief Version Read Response
 *
 * \n This response is received after sending the request to modem to get the
 * \n Software Version.
 */
#define MAL_MIS_VERSION_READ_RESP 0x01

#define MAL_MIS_PP_FLAG_SET_RESP 0x02

#define MAL_MIS_PP_FLAG_READ_RESP 0x03

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_mis_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the MIS MAL Library. These error types
 * \n are used as return values for the API routines.
 */
typedef enum {
    MAL_MIS_SUCCESS             = 0, /**< Success  */
    MAL_MIS_FAIL                = 1, /**< Failure */
    MAL_MIS_NOT_SUPPORTED       = 2, /**< Not supported */
    MAL_MIS_GENERAL_ERROR       = 3, /**< Request cannot be completed because of a
                                       * general error
                                       */
    MAL_MIS_INVALID_BLOCK_TYPE  = 4, /**< Block type not correct */
    MAL_MIS_INSUFFICIENT_MEMORY = 5, /**< Memory allocation failure */
    MAL_MIS_INVALID_DATA        = 6, /**< Invalid data passed to the routine */
    MAL_MIS_SOCK_SEND_ERROR     = 7  /**< Send operation on socket failed */
} mal_mis_error_type;

/**
 * \typedef void (*mal_mis_event_cb_t)(int32_t event_id, void *data,
 *                                mal_mis_error_type error_code, void *client_tag)
 * \brief Event notification to Application
 *
 * \n Callback function for event notification to the Application. This callback
 * \n is used for both solicited responses and unsolicited indications.
 *
 * \param [in] event_id      ID of the event being notified
 * \param [in] data          Pointer to data associated with the event being notified
 * \param [in] error_code    Error code associated with the event
 * \param [in] client_tag    Client tag associated with the response or indication
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_MIS_VERSION_READ_RESP
 * \n Pointer to NULL-terminated const char string presenting the baseband version
 * \par
 * \arg
 * \n
 */
typedef void (*mal_mis_event_cb_t)(int32_t event_id, void *data,
                                   mal_mis_error_type error_code, void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_mis_init(int32_t *fd_mis)
 * \brief Initialize MIS Library
 *
 * \n This routine is used to initialize the MIS Library and open the socket
 * \n using Phonet Library. This routine should be called only once at the time
 * \n of initialization.
 * \param [out] fd_mis    Pointer to file descriptor of MIS socket
 * \return    \ref        mal_mis_error_type
 */
int32_t mal_mis_init(int32_t *fd_mis);

/**
 * \fn int32_t mal_mis_deinit(void)
 * \brief De-initialize MIS Library
 *
 * \n This routine is used to de-initialize the MIS Library.
 * \param            void
 * \return    \ref   mal_mis_error_type
 */
int32_t mal_mis_deinit(void);

/**
 * \fn int32_t mal_mis_register_callback(mal_mis_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * \n This routine registers the callback function for passing events
 * \n to the Application.
 * \param [in] event_cb       Event callback handler
 * \return    \ref            mal_mis_error_type
 */
int32_t mal_mis_register_callback(mal_mis_event_cb_t event_cb);

/**
 * \fn void mal_mis_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \navailable from modem for processing.
 * \param    void
 * \return   void
 */
void mal_mis_response_handler(void);

/**
 * \fn int32_t mal_mis_req_baseband_version(void *client_tag)
 * \brief Request for Baseband version
 *
 * \n This routine is called by Application to query baseband version. Expected
 * \n event type in case of successful invocation of this routine is
 * \n MAL_MIS_VERSION_READ_RESP.
 *
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_mis_error_type
 */
int32_t mal_mis_req_baseband_version(void *client_tag);

/**
 * \fn int32_t mal_mis_req_pp_flags(uint8_t all, uint16_t flag_id, void *client_tag)
 * \brief Request for all PP (Product Profile) flags
 *
 * \n This routine is called by Application to query all or one specifed PP
 * \n (Product Profile) flags. Expected event type in case of successful
 * \n invocation of this routine is MAL_MIS_PP_FLAG_READ_RESP.
 *
 * \param [in] all           Read all pp flags in one message?
 * \param [in] flag_id       Id of a specific PP flag
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref           mal_mis_error_type
 */
int32_t mal_mis_req_pp_flags(uint8_t all, uint16_t flag_id, void *client_tag);

/**
 * \fn int32_t mal_mis_set_pp_flag(uint16_t flag_id uint16_t value, void *client_tag)
 * \brief Request for setting a specific PP (Product Profile) flag
 *
 * \n This routine is called by Application to set a specific PP
 * \n (Product Profile) flag. No call-back defined for this API routine,
 * \n TBD when required.
 *
 * \param [in] flag_id      ID of the desired PP flag
 * \param [in] value        Value to be set for the desired PP flag
 * \param [in] client_tag   Void pointer to client specific data. The client
 *                                specific data is transparent to MAL and is
 *                                returned as-is in the response.
 * \return    \ref          mal_mis_error_type
 */
int32_t mal_mis_set_pp_flag(uint16_t flag_id, uint16_t value, void *client_tag);

/**
 *  \fn int32_t mal_mis_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for mis module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mis_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_mis_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for mis module. This is a synchronous call.
 *   \param[out] debug level for mis module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_mis_request_get_debug_level(uint8_t *level);

#endif /* MAL_MIS_H */
