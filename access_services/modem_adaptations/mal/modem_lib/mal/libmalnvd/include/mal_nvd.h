/**
 * \file mal_nvd.h
 * \brief This file includes headers for NVD MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL NVD header file for NVD Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.1    Initial
 */

/**
 * \defgroup  MAL_NVD NVD API
 * \par
 * \n This part describes the interface to NVD MAL Client
 */

#ifndef MAL_NVD_H
#define MAL_NVD_H

#include <stdint.h>

#define NVD_LIB_VERSION "NVD Lib V 0.1.r1"

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/**
 * \def MAL_NVD_SET_DEFAULT_RESP
 * \brief Set non-volatile data to default
 *
 * \n This response is received after sending the request to modem to set the
 * \n non-volatile data to default values.
 */
#define MAL_NVD_SET_DEFAULT_RESP 0x02

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_nvd_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the NVD MAL Library. These error types
 * \n are used as return values for the API routines.
 */
typedef enum {
    MAL_NVD_SUCCESS             = 0, /**< Success  */
    MAL_NVD_FAIL                = 1, /**< Failure */
    MAL_NVD_NOT_SUPPORTED       = 2, /**< Not supported */
    MAL_NVD_GENERAL_ERROR       = 3, /**< Request cannot be completed because of
                                       * a general error
                                       */
    MAL_NVD_INVALID_BLOCK_TYPE  = 4, /**< Block type not correct */
    MAL_NVD_INSUFFICIENT_MEMORY = 5, /**< Memory allocation failure */
    MAL_NVD_INVALID_DATA        = 6, /**< Invalid data passed to the routine */
    MAL_NVD_SOCK_SEND_ERROR     = 7  /**< Send operation on socket failed */
} mal_nvd_error_type;

/**
 * \typedef void (*mal_nvd_event_cb_t)(int32_t event_id, void *data,
 *                            mal_nvd_error_type error_code, void *client_tag)
 * \brief Event notification to Application
 *
 * \n Callback function for event notification to the Application. This callback
 * \n is used for both solicited responses and unsolicited indications.
 *
 * \param [in] event_id    ID of the event being notified
 * \param [in] data        Pointer to data associated with the event being notified
 * \param [in] error_code  Error code associated with the event
 * \param [in] client_tag  Client tag associated with the response or indication
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_NVD_SET_DEFAULT_RESP
 * \n NULL
 * \n
 */
typedef void (*mal_nvd_event_cb_t)(int32_t event_id, void *data,
                                   mal_nvd_error_type error_code,
                                   void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_nvd_init(int32_t *fd_nvd)
 * \brief Initialize NVD Library
 *
 * \n This routine is used to initialize the NVD Library and open the socket
 * \n using Phonet Library. This routine should be called only once at the time
 * \n of initialization.
 * \param [out] fd_nvd    Pointer to file descriptor of NVD socket
 * \return    \ref        mal_nvd_error_type
 */
int32_t mal_nvd_init(int32_t *fd_nvd);

/**
 * \fn int32_t mal_nvd_deinit(void)
 * \brief De-initialize NVD Library
 *
 * \n This routine is used to de-initialize the NVD Library.
 * \param            void
 * \return    \ref   mal_nvd_error_type
 */
int32_t mal_nvd_deinit(void);

/**
 * \fn int32_t mal_nvd_register_callback(mal_nvd_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * \n This routine registers the callback function for passing events
 * \n to the Application.
 * \param [in] event_cb       Event callback handler
 * \return    \ref            mal_nvd_error_type
 */
int32_t mal_nvd_register_callback(mal_nvd_event_cb_t event_cb);

/**
 * \fn void mal_nvd_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \param   void
 * \return  void
 */
void mal_nvd_response_handler(void);

/**
 * \fn int32_t mal_nvd_set_default(void *client_tag)
 * \brief Request for setting non-volatile data to default
 *
 * \n This routine is called by Application to set non-volatile data to default.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_NVD_SET_DEFAULT_RESP.
 *
 * \param [in] client_tag  Void pointer to client specific data. The client
 *                              specific data is transparent to MAL and is
 *                              returned as-is in the response.
 * \return    \ref         mal_nvd_error_type
 */
int32_t mal_nvd_set_default(void *client_tag);

/**
 *  \fn int32_t mal_nvd_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for nvd module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_nvd_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_nvd_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for nvd module. This is a synchronous call.
 *   \param[out] debug level for nvd  module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 *   \return mal error type. Data type: int32_t (SUCCESS/FAIL/NOT SUPPORTED)
 */
int32_t mal_nvd_request_get_debug_level(uint8_t *level);

#endif /* MAL_NVD_H */
