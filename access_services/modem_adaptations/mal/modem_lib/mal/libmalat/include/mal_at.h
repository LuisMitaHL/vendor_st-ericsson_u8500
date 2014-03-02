/**
 * \file mal_at.h
 * \brief This file includes headers for AT MAL.
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \n MAL AT header file for AT Library.
 * \n \author ST-Ericsson
 * \n
 * \n Revision History:
 * \n
 * \n v0.1    Initial
 */

/**
 * \defgroup  MAL_AT AT API
 * \par
 * \n This part describes the interface to AT MAL Client
 */

#ifndef MAL_AT_H
#define MAL_AT_H

#include <stdint.h>

#define AT_LIB_VERSION "AT Lib V 0.1.r1"

/*
 * Maximum number of connections that can exist at any time cannot be greater
 * than MAL_AT_MAX_NO_OF_DTE_SUPPORTED.
 */
#define MAL_AT_MAX_NO_OF_DTE_SUPPORTED 5

/* No connected DTE for this entry in the array*/
#define MAL_AT_FREE_DTE_SLOT  0xFF

/* Maximum command length */
#define MAL_AT_MAX_COMMAND_LEN 1024

/* Minimum command length */
#define MAL_AT_MIN_COMMAND_LEN 2

/*---------------------------------------------------------------------------*/
/* Event IDs                                                                 */
/*---------------------------------------------------------------------------*/

/**
 * \def MAL_AT_MODEM_CONNECT_RESP
 * \brief Event ID to to indicate connect response for session
 *
 * \n This response is received after sending the request to modem to establish
 * \n a connectoin.
 */
#define MAL_AT_MODEM_CONNECT_RESP 0x01

/**
 * \def MAL_AT_MODEM_DISCONNECT_RESP
 * \brief Event ID to to indicate disconnect response for session
 *
 * \n This response is received after sending the request to modem to
 * \n disconnect a connectoin.
*/
#define MAL_AT_MODEM_DISCONNECT_RESP 0x03

/**
 * \def MAL_AT_MODEM_CMD_RESP
 * \brief Event ID to to indicate command response for session
 *
 * \n This response is received after sending an AT command to modem.
*/
#define MAL_AT_MODEM_CMD_RESP 0x05

/**
 * \def MAL_AT_MODEM_UNSOLICITED_DATA_IND
 *
 * \brief Indication to indicate unsolicited data is received
 *
 * \n This indication is received when any unsolicited data is received.
 */
#define MAL_AT_MODEM_UNSOLICITED_DATA_IND 0x07

/**
 * \def MAL_AT_MODEM_INTERMEDIATE_DATA_IND
 *
 * \brief Indication to indicate intermediate data is received for AT Command
 * \brief sent to the modem
 *
 * \n This indication is received when any intermediate data for an AT Command
 * \n sent to the modem is received.
 */
#define MAL_AT_MODEM_INTERMEDIATE_DATA_IND 0x08

/*----------------------------------------------------------------------------*/
/* Structures, Enums and Typedefs                                             */
/*----------------------------------------------------------------------------*/

/**
 * \enum mal_at_error_type
 * \brief Error types
 *
 * \n This enum defines error types for the AT MAL Library. These error types
 * \n are used as return values for the API routines and with events.
 */
typedef enum {
    MAL_AT_FAIL                = 0, /**< Failure */
    MAL_AT_SUCCESS             = 1, /**< Success  */
    MAL_AT_SUCCESS_DIFF_DTE_ID = 2, /**< Request succussful but DTE ID
                                      * is changed.
                                      */
    MAL_AT_GENERAL_ERROR       = 3, /**< Request cannot be completed because of
                                       * a general error
                                       */
    MAL_AT_NOT_SUPPORTED       = 4, /**< Not supported */
    MAL_AT_INVALID_BLOCK_TYPE  = 5, /**< Block type not correct */
    MAL_AT_INSUFFICIENT_MEMORY = 6, /**< Memory allocation failure */
    MAL_AT_INVALID_DATA        = 7, /**< Invalid data passed to the routine */
    MAL_AT_SOCK_SEND_ERROR     = 8, /**< Send operation on socket failed */
    MAL_AT_DTE_ARRAY_FULL      = 9  /**< DTE Array exhausted*/
} mal_at_error_type;

/**
 * \struct mal_at_cmd_info
 * \brief
 *
 * \n This structure provides information about the command sent to modem or any
 * \n unsolicited indication received from modem for a particular DTE in
 * \n connection.
 */
typedef struct {
    uint8_t  dte_id;  /**< DTE ID */
    uint16_t len;     /**< Length of command or indication buffer */
    uint8_t  *buffer; /**< Command or Indication buffer */
} mal_at_cmd_info;

/**
 * \struct mal_at_cmd_resp
 * \brief
 *
 * \n This structure provides information about the command response received
 * \n from modem for a particular DTE in connection.
 */
typedef struct {
    uint8_t  *resp_buffer;         /**< Response buffer */
    uint8_t  *intermediate_buffer; /**< Intermediate data buffer */
    uint16_t resp_len;             /**< Length of response buffer */
    uint16_t intermediate_len;     /**< Lenght of intermediate data buffer */
    uint8_t  dte_id;               /**< DTE ID */
} mal_at_cmd_resp;

/*---------------------------------------------------------------------------*/
/* AT MAL API definitions                                                    */
/*---------------------------------------------------------------------------*/

/**
 * \typedef void (*mal_at_event_cb_t)(int32_t event_id, void *data,
 *                            mal_at_error_type error_code, void *client_tag)
 * \brief Event notification to Application
 *
 * \n Callback function for event notification to the Application. This callback is
 * \n used for both solicited responses and unsolicited indications.
 *
 * \param [in] event_id    ID of the event being notified
 * \param [in] data        Pointer to data associated with the event being notified
 * \param [in] error_code  Error code associated with the event
 * \param [in] client_tag  Client tag associated with the response or indication
 *
 * \par
 * Following is the detail on data based on the event type:
 * \par
 * \arg MAL_AT_MODEM_CONNECT_RESP
 * \n Pointer to \ref uint8_t returning dte_id
 * \par
 * \arg MAL_AT_MODEM_DISCONNECT_RESP
 * \n Pointer to \ref uint8_t returning dte_id
 * \par
 * \arg MAL_AT_MODEM_CMD_RESP
 * \n Pointer to \ref mal_at_cmd_resp
 * \par
 * \arg MAL_AT_MODEM_UNSOLICITED_DATA_IND
 * \n Pointer to \ref mal_at_cmd_req
 * \par
 * \arg MAL_AT_MODEM_INTERMEDIATE_DATA_IND
 * \n NULL
*/
typedef void (*mal_at_event_cb_t)(int32_t event_id, void *data,
                                  mal_at_error_type error_code,
                                  void *client_tag);

/*----------------------------------------------------------------------------*/
/* Function declarations                                                      */
/*----------------------------------------------------------------------------*/

/**
 * \fn int32_t mal_at_init(int32_t *fd_at)
 * \brief Initialize AT Library
 *
 * \n This routine is used to initialize the AT Library and open the socket
 * \n using Phonet Library. This routine should be called only once at the time
 * \n of initialization.
 * \param [out] fd_at    Pointer to file descriptor of AT socket
 * \return    \ref       mal_at_error_type
 */
int32_t mal_at_init(int32_t *fd_at);

/**
 * \fn int32_t mal_at_deinit(void)
 * \brief De-initialize AT Library
 *
 * \n This routine is used to de-initialize the AT Library.
 * \param            void
 * \return    \ref   mal_at_error_type
 */
int32_t mal_at_deinit(void);

/**
 * \fn int32_t mal_at_register_callback(mal_at_event_cb_t event_cb)
 * \brief Register callbacks from application
 *
 * \n This routine registers the callback function for passing events
 * \n to the Application.
 * \param [in] event_cb       Event callback handler
 * \return    \ref            mal_at_error_type
 */
int32_t mal_at_register_callback(mal_at_event_cb_t event_cb);

/**
 * \fn void mal_at_response_handler(void)
 * \brief Handler for received messages
 *
 * \n This routine is called by Application to inform that some message is
 * \n available from modem for processing.
 * \param   void
 * \return  void
 */
void mal_at_response_handler(void);

/**
 * \fn int32_t mal_at_req_connnection(uint8_t dte_conn_id, void *client_tag)
 * \brief Request for establishing connection with specific dte_id
 *
 * \n This routine is called by Application to start a new session for a
   particular DTE.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_AT_MODEM_CONNECT_RESP .
 *
 * \param [in] dte_conn_id    DTE_ID for which connection request is made.
 * \param [in] client_tag     Void pointer to client specific data. The client
 *                            specific data is transparent to MAL and is
 *                            returned as-is in the response.
 * \return    \ref            mal_at_error_type
 */
int32_t mal_at_req_connnection(uint8_t dte_conn_id, void *client_tag);

/**
 * \fn int32_t mal_at_req_disconnnection(uint8_t dte_conn_id, void *client_tag)
 * \brief Request for disconnection with specific dte_id
 *
 * \n This routine is called by Application to disconnect a session for a
        particular DTE.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_AT_MODEM_DISCONNECT_RESP .
 *
 * \param [in] dte_conn_id DTE_ID for which disconnection request is made.
 * \param [in] client_tag    Void pointer to client specific data. The client
 *                           specific data is transparent to MAL and is
 *                           returned as-is in the response.
 * \return    \ref           mal_at_error_type
 */
int32_t mal_at_req_disconnnection(uint8_t dte_conn_id, void *client_tag);

/**
 * \fn int32_t mal_at_req_send_command(mal_at_cmd_info *at_cmd_req,
 *                                     void *client_tag)
 * \brief Request for sending command to specific dte_id
 *
 * \n This routine is called by Application to send an AT Command to the Modem
 * \n AT Server for a particular DTE_ID.
 * \n Expected event type in case of successful invocation of this routine is
 * \n MAL_AT_MODEM_CMD_RESP .
 *
 * \param [in] at_cmd_req     Pointer to structure mal_at_cmd_info.
 *                              Structure elements dte_id, length of command
 *                              buffer, command buffer.
 * \return    \ref            mal_at_error_type
 */
int32_t mal_at_req_send_command(mal_at_cmd_info *at_cmd_req, void *client_tag);

/**
 * \fn int32_t mal_at_config(void)
 * \brief Establish single DTE connection
 *
 * \n This routine is used to establish a single connection.
 *
 * \param           void
 * \return   \ref   mal_at_error_type
 */
int32_t mal_at_config(void);

/**
 * \fn int32_t mal_at_get_dte_array(uint8_t *dte_array)
 * \brief API to read the connected dte_ids
 *
 * \n This routine is called by Application to get the array of length
 * \n MAL_AT_MAX_NO_OF_DTE_SUPPORTED which contains connected dte_ids.
 * \n MAL_AT_FREE_DTE_SLOT does not have corresponding connected DTE.
 *
 * \param [in]  dte_array       Array of dte_ids
 * \return      \ref            mal_at_error_type
 */
int32_t mal_at_get_dte_array(uint8_t *dte_array);

/**
 *  \fn int32_t mal_at_request_set_debug_level(uint8_t level)
 *  \brief  This Api is used to set the debug level for at module. This is a synchronous call.
 *  \param [in] level  0 - No Prints; 1 - Entry & Exit Prints; 2 - Value/Info Prints; 3 - ISI MSG Prints
 *                               4 - All Prints
 * \return      \ref            mal_at_error_type
 */
int32_t mal_at_request_set_debug_level(uint8_t level);

/**
 *  \fn int32_t mal_at_request_get_debug_level(uint8_t *level)
 *  \brief  This Api is used to get the debug level for at module. This is a synchronous call.
 *   \param[out] debug level for net module. data type: pointer to uint8_t.
 *                       Caller needs to take care of memory allocation
 * \return      \ref            mal_at_error_type
 */
int32_t mal_at_request_get_debug_level(uint8_t *level);

#endif /* MAL_AT_H */
