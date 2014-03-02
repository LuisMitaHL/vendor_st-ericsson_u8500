/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Functionality for connecting and communicating with
 * the Call and Network (CN) service.
 */

#ifndef __cn_client_h__
#define __cn_client_h__ (1)

/***** Force structural alignment setting *****
 * When using different compilers and/or compiler settings the alignment may differ.
 * This is not acceptable for the IPC communication. A four byte alignment is chosen
 * since that is the word size of the ARM processor.
 */
#pragma pack(push,4)

#include <cn_general.h>
#include <cn_data_types.h>
#include <cn_message_types.h>

typedef struct cn_context_s cn_context_t;


/*************************************************************************
 *       FUNCTIONS FOR HANDLING THE CONNECTION TO THE C&N SERVICE         *
 *************************************************************************/

/**
 * \fn cn_error_code_t cn_client_init(cn_context_t** context_pp)
 * \brief Initialize connection to the Call&Networking service.
 *
 * \n Initialize connection to the Call&Networking service.
 * \n This function is synchronous and will return when socket connections
 * \n have been achieved with the CN server. Two sockets are opened,
 * \n one for requests/responses, and one for events.
 *
 * \param [out] context_pp   Outgoing pointer to client context structure that is used
 *                           as a handle in all other CN functions.
 * \return  \ref             cn_error_code_t
 *
 */
cn_error_code_t cn_client_init(cn_context_t **context_pp);


/**
 * \fn cn_error_code_t cn_client_get_request_fd(cn_context_t* context_p, int* fd_p)
 * \brief Get request/response socket file descriptor.
 *
 * \n Get request/response socket file descriptor. It is provided to enable e.g. select() handling.
 * \n This function is synchronous.
 *
 * \param [in] context_p    Pointer to client context structure.
 * \param [out] fd_p        Pointer to file descriptor handle.
 *        break;
 *
 * \return  \ref            cn_error_code_t
 *
 */
cn_error_code_t cn_client_get_request_fd(cn_context_t *context_p, int *fd_p);


/**
 * \fn cn_error_code_t cn_client_get_event_fd(cn_context_t* context_p, int* fd_p)
 * \brief Get event socket file descriptor.
 *
 * \n Get event socket file descriptor. It is provided to enable e.g. select() handling.
 * \n This function is synchronous.
 *
 * \param [in] context_p    Pointer to client context structure.
 * \param [out] fd_p        Pointer to file descriptor handle.
 *
 * \return  \ref            cn_error_code_t
 *
 */
cn_error_code_t cn_client_get_event_fd(cn_context_t *context_p, int *fd_p);


/**
 * \fn cn_error_code_t cn_message_receive(int fd, cn_uint32_t *size_p, cn_message_t **msg_pp)
 * \brief Receives a single complete message from socket.
 *
 * \n On return, a pointer to the first complete message, if any, have been written to
 * \n the pointer pointed to by msg_pp. Any remaining unread bytes in the input buffer
 * \n is written to size_p.
 *
 *
 * \param [in] fd           File descriptor.
 * \param [in, out] size_p  Pointer to number of bytes in input buffer.
 * \param [out] msg_pp      Pointer to pointer of message.
 *
 * \return  \ref            cn_error_code_t
 *
 * \n If a message is retrieved this function returns CN_SUCCESS.
 * \n If there is an incomplete message in the buffer, the function
 * \n returns CN_REQUEST_BUSY. No message is returned.
 * \n If there is nothing in the input buffer, or if the Call&Network server
 * \n has closed its end of the socket connection, the function returns
 * \n CN_FAILURE. If the socket is closed, then cn_client_shutdown and
 * \n cn_client_init must be called to re-establish communication with the
 * \n Call&Network server.
 *
 */
cn_error_code_t cn_message_receive(int fd, cn_uint32_t *size_p, cn_message_t **msg_pp);


/**
 * \fn cn_error_code_t cn_get_message_queue_size(int fd, cn_sint32_t* size_p)
 * \brief Get message queue size for a given file descriptor.
 *
 * The purpose of this function is to determine if there is data to be read on any
 * of the CN file descriptors. This is convenient for clients not using select().
 *
 * \param [in] fd           File descriptor.
 * \param [out] size_p      Pointer to number of bytes in message queue.
 *
 * \return  \ref            cn_error_code_t
 *
 */
cn_error_code_t cn_get_message_queue_size(int fd, cn_uint32_t *size_p);


/**
 * \fn cn_error_code_t cn_clear_message_queue(int fd)
 * \brief Clear message queue
 *
 * This function clear the message queue for the given file descriptor. Note
 * that this is intended for events that doesn't have client tags. Clearing a
 * request/response message queue is not recommended since that means the
 * responses are lost.
 *
 * \param [in] fd           File descriptor.
 *
 * \return  \ref            cn_error_code_t
 *
 */
cn_error_code_t cn_clear_message_queue(int fd);


/**
 * \fn cn_error_code_t cn_client_shutdown(cn_context_t* context_p)
 * \brief Shutdown connection to the Call&Networking service.
 *
 * \n Shutdown connection to the Call&Networking service.
 * \n This function is synchronous and will return when resources have been
 * \n closed and freed.
 *
 * \param [in] context_p    Pointer to client context structure.
 *
 * \return  \ref            cn_error_code_t
 *
 */
cn_error_code_t cn_client_shutdown(cn_context_t *context_p);


/*************************************************************************
 *                           REQUEST FUNCTIONS                           *
 *************************************************************************/

/**
 * \fn cn_error_code_t cn_request_rf_on(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request to turn RF on
 *
 * \n This routine is used to turn         break;
 * the RF on and to perform a network attach.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_RF_ON.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return  \ref             cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_modem_status_t (payload in message structure)
 */
cn_error_code_t cn_request_rf_on(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_rf_off(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request to turn RF off
 *
 * \n This routine is used to turn the RF off. Expected event type
 * \n in case of successful invocation of this routine is
 * \n CN_RESPONSE_RF_OFF
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return  \ref             cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_modem_status_t (payload in message structure)
 */
cn_error_code_t cn_request_rf_off(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_rf_status(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query for current RF state
 *
 * \n This routine is used to query the RF status. Expected event type
 * \n in case of successful invocation of this routine is
 * \n CN_RESPONSE_RF_STATUS
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return \ref              cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_rf_status_t (payload in message structure)
 */
cn_error_code_t cn_request_rf_status(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_registration_control(cn_context_t *context_p, cn_network_access_config_data_t *config_data_p, cn_client_tag_t client_tag)
 * \brief Registration control
 *
 * \n This routine is used to prevent or allow modem registrations to all
 * \n networks or roaming networks. Expected event type
 * \n in case of successful invocation of this routine is
 * \n CN_RESPONSE_REGISTRATION_CONTROL
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] config_data_p Modem registration configuration.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return \ref              cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_registration_control(cn_context_t *context_p, cn_network_access_config_data_t *config_data_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_reset_modem(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Reset modem
 *
 * \n This routine is used to reset the modem (use with caution).
 * \n During the reset the radio is turned off and all modem services
 * \n are shut down. After the reset the modem and radio states will be set
 * \n according to the initial modem state configuration. Expected event type
 * \n in case of successful invocation of this routine is
 * \n CN_RESPONSE_RESET_MODEM.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return  \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_reset_modem(cn_context_t *context_p, cn_client_tag_t client_tag);

/**
 * \fn cn_error_code_t cn_request_reset_modem_with_dump(cn_context_t *context_p, cn_cpu_type_t cpu_type, cn_client_tag_t client_tag)
 * \brief Reset modem with dump
 *
 * \n This routine is used to reset the modem with dump (use with caution).
 * \n During the reset the radio is turned off and all modem services
 * \n are shut down. After the reset the modem and radio states will be set
 * \n according to the initial modem state configuration and a modem
 * \n crash dump can be generated
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] cpu_type          \ref cn_cpu_type_t
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return  \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_reset_modem_with_dump(cn_context_t *context_p, cn_cpu_type_t cpu_type, cn_client_tag_t client_tag);

/**
 * \fn cn_error_code_t cn_request_sleep_test_mode(cn_context_t *context_p, cn_sleep_mode_setting_t sleep_mode, cn_client_tag_t client_tag)
 * \brief Selection of Modem CPU's to the given sleep mode to disable modem interrupts
 *
 * \n This routine sets the modem CPU's to the given sleep mode.
 * \n It can only be used for testing purposes for current consumption measurements.
 * \n It will disable all other modem interrupts except modem messaging interrupts.
 * \n In this way modem can only be woken up from the sleep with message
 * \n from the host and after the wakeup modem sleep state is restored
 *
 * \param [in] context_p                       Call & Networking connection context.
 *\param [in] sleep_mode                    \ref cn_sleep_mode_setting_t
 * \param [in] client_tag                      Client tag handle specific to the client. It is
 *                                                        transparent to CN and is returned as-is
 *                                                        in the response.
 * \return  \ref                               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_sleep_test_mode(cn_context_t *context_p, cn_sleep_mode_setting_t sleep_mode, cn_client_tag_t client_tag);

/**
 * \fn cn_error_code_t cn_request_set_preferred_network_type(cn_context_t* context_p, cn_network_type_t type, cn_client_tag_t client_tag)
 * \brief Request to set preferred network type
 *
 * \n This routine is called by Application to set the preferred network type.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] type          \ref cn_network_type_t
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_preferred_network_type(cn_context_t *context_p, cn_network_type_t type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_preferred_network_type(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request to get preferred network type
 *
 * \n This routine is called by Application to get the preferred network type.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 *
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_network_type_t (payload in message structure)
 */
cn_error_code_t cn_request_get_preferred_network_type(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_registration_state_normal(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request for registration data
 *
 * \n This routine is used for querying GSM registration state. Expected event
 * \n type in case of successful invocation of this routine is
 * \n CN_RESPONSE_REGISTRATION_STATE_NORMAL
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_registration_info_t (payload in message structure)
 */
cn_error_code_t cn_request_registration_state_normal(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_registration_state_gprs(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request for GPRS registration data
 *
 * \n This routine is used for querying GPRS registration state. Expected event
 * \n type in case of successful invocation of this routine is
 * \n CN_RESPONSE_REGISTRATION_STATE_GPRS
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_registration_info_t (payload in message structure)
 */
cn_error_code_t cn_request_registration_state_gprs(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_cell_info(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request for cell information
 *
 * \n This routine is used for querying cell information. Expected event
 * \n type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CELL_INFO
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_cell_info_t (payload in message structure)
 */
cn_error_code_t cn_request_cell_info(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_automatic_network_registration(cn_context_t *context_p, cn_client_tag_t client_tag)
 * \brief Register to the specified network
 *
 * \n This routine is used to invoke registration to the network. Expected event
 * \n type in case of successful invocation of this routine is
 * \n CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION.
 * \n
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_automatic_network_registration(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_manual_network_registration(cn_context_t* context_p, cn_network_registration_req_data_t *req_data_p, cn_client_tag_t client_tag)
 * \brief Register manually to the specified network
 *
 * \n This routine is used to invoke registration to the network based on MNC
 * \n and MCC of the network. Expected event type in case of successful
 * \n invocation of this routine is CN_RESPONSE_MANUAL_NETWORK_REGISTRATION
 * \n
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] req_data_p    Registration data.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_network_registration_data_t (payload in message structure)
 */
cn_error_code_t cn_request_manual_network_registration(cn_context_t *context_p, cn_network_registration_req_data_t *req_data_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_manual_network_registration_with_automatic_fallback(cn_context_t* context_p, cn_network_registration_req_data_t *req_data_p, cn_client_tag_t client_tag)
 * \brief Register manually to the specified network and if it fails fall back to automatic selection.
 *
 * \n This routine is used to invoke registration to the network based on MNC
 * \n and MCC of the network. If the operation fails, automatic network selection is invoked as a fallback mechanism.
 * \n
 * \n Expected event type in case of successful invocation of this routine is CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK
 * \n
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] req_data_p    Registration data.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_network_registration_data_t in case of manual registration or no payload in case of automatic selection.
 */
cn_error_code_t cn_request_manual_network_registration_with_automatic_fallback(cn_context_t *context_p, cn_network_registration_req_data_t *req_data_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_network_deregister(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Control the cellular system.
 *
 * \n This routine is used to control the cellular system. Expected event type in case of successful
 * \n invocation of this routine is CN_RESPONSE_NETWORK_DEREGISTER.
 * \n
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_network_deregister(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_net_query_mode(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Request for Operator Code
 *
 * \n This routine is used for requesting operator code. Expected event type
 * \n in case of successful invocation of this routine is
 * \n CN_RESPONSE_NET_QUERY_MODE
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_registration_info_t (payload in message structure)
 */
cn_error_code_t cn_request_net_query_mode(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_manual_network_search(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Manual search of available networks
 * \n see also \ref cn_request_interrupt_network_search
 *
 * \n This routine is used to invoke Manual Search of Networks. Expected event
 * \n type in case of successful invocation of this routine is
 * \n CN_RESPONSE_MANUAL_NETWORK_SEARCH
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_manual_network_search_data_t (payload in message structure)
 * \n If request is interrupted by other requests CN_SUCCESS will be returned
 */
cn_error_code_t cn_request_manual_network_search(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_interrupt_network_search(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Interrupt search of networks
 *
 * \n This routine is used to interrupt Manual Search of Networks. Expected
 * \n event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_INTERRUPT_NETWORK_SEARCH
 * \n see also \ref cn_request_manual_network_search
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_interrupt_network_search(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_current_call_list(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Get current call status
 *
 * \n This routine is used to query the current call status. Expected event
 * \n type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CURRENT_CALL_LIST.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_call_list_t (payload in message structure)
 */
cn_error_code_t cn_request_current_call_list(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_dial(cn_context_t* context_p, cn_dial_t* dial_p, cn_client_tag_t client_tag)
 * \brief Initiate MO voice call
 *
 * \n This routine is used to initiate a circuit switched mobile originated voice call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_DIAL.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] dial_p         \ref cn_dial_t with call settings.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_response_dial_t (payload in message structure)
 */
cn_error_code_t cn_request_dial(cn_context_t *context_p, cn_dial_t *dial_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_hangup(cn_context_t* context_p, cn_call_state_filter_t filter, cn_uint8_t call_id, cn_client_tag_t client_tag)
 * \brief Hang-up call based on call id.
 *
 * \n This routine is used to hang-up a call (MO or MT).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_HANGUP.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] filter        Specifies which call-state(s) to hang up on,
 *                                or if to hang up on the specified call-id.
 * \param [in] call_id       Specifies which call to hang-up on (call index).
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 *
 */
cn_error_code_t cn_request_hangup(cn_context_t *context_p, cn_call_state_filter_t filter, cn_uint8_t call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_swap_calls(cn_context_t* context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
 * \brief Swap the hold and active calls.
 *
 * \n This routine is used to swap the hold and active calls.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SWAP_CALLS.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] active_call_id  Specifies the active call id (call index).
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 */
cn_error_code_t cn_request_swap_calls(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_hold_call(cn_context_t* context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
 * \brief Swap the hold the active call.
 *
 * \n This routine is used to hold the active call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_HOLD_CALL.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] active_call_id  Specifies the active call id (call index).
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
  * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 */
cn_error_code_t cn_request_hold_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_resume_call(cn_context_t* context_p, cn_uint8_t hold_call_id, cn_client_tag_t client_tag)
 * \brief Resume a hold call
 *
 * \n This routine is used to resume a hold call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_RESUME_CALL.
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] hold_call_id   Specifies the hold call id (call index).
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 */
cn_error_code_t cn_request_resume_call(cn_context_t *context_p, cn_uint8_t hold_call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_conference_call(cn_context_t* context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
 * \brief Join the call into a conference call.
 *
 * \n This routine is used to join the call into a conference call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CONFERENCE_CALL
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] active_call_id  Specifies the active call id (call index).
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 */
cn_error_code_t cn_request_conference_call(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_conference_call_split(cn_context_t* context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag)
 * \brief Split a conference call.
 *
 * \n This routine is used to split a conference call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CONFERENCE_CALL_SPLIT.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] active_call_id  Specifies the active call id (call index).
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 */
cn_error_code_t cn_request_conference_call_split(cn_context_t *context_p, cn_uint8_t active_call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_explicit_call_transfer(cn_context_t* context_p, cn_uint8_t call_id, cn_client_tag_t client_tag)
 * \brief Explicit call transfer
 *
 * \n This routine is used to perform an explicit call transfer
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_EXPLICIT_CALL_TRANSFER.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] call_id         Specifies the call id (call index).
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 */
cn_error_code_t cn_request_explicit_call_transfer(cn_context_t *context_p, cn_uint8_t call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_answer_call(cn_context_t* context_p, cn_uint8_t call_id, cn_client_tag_t client_tag)
 * \brief Answer call.
 *
 * \n This routine is used to answer an incoming call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_ANSWER_CALL.
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] call_id        Specifies the call id (call index).
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is not CN_FAILURE, no particular datatype is returned (i.e. no payload).
 *
 */
cn_error_code_t cn_request_answer_call(cn_context_t *context_p, cn_uint8_t call_id, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_ussd(cn_context_t* context_p, cn_ussd_info_t  *cn_ussd_info_p, cn_client_tag_t client_tag)
 * \brief Send USSD string.
 *
 * \n This routine is used to send an USSD string.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_USSD
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] cn_ussd_info_p USSD info struct.
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_response_ussd_t
 *
 */
cn_error_code_t cn_request_ussd(cn_context_t *context_p, cn_ussd_info_t *cn_ussd_info_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_ussd_abort(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Abort ongoing USSD command.
 *
 * \n This routine is used to abort an ongoing USSD operation.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_USSD_ABORT.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 *
 */
cn_error_code_t cn_request_ussd_abort(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_signal_info_config(cn_context_t *context_p, cn_signal_info_config_t *config_p, cn_client_tag_t client_tag)
 * \brief Configure the sending of CN_EVENT_SIGNAL_INFO
 *
 * \n This routine is used to configure the sending of CN_EVENT_SIGNAL_INFO.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_SIGNAL_INFO_CONFIG
 *
 * \n Associated event: CN_EVENT_SIGNAL_INFO
 *
 * \param [in] config_p      RSSI event configuration.
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_signal_info_config(cn_context_t *context_p, cn_signal_info_config_t *config_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_signal_info_config(cn_context_t *context_p, cn_client_tag_t client_tag)
 * \brief Query the configuration of CN_EVENT_SIGNAL_INFO event sending.
 *
 * \n This routine is used to query the configuration of CN_EVENT_SIGNAL_INFO event sending.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_SIGNAL_INFO_CONFIG
 *
 * \n Associated event: CN_EVENT_SIGNAL_INFO
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_signal_info_config_t (payload in message structure)

 */
cn_error_code_t cn_request_get_signal_info_config(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_signal_info_reporting(cn_context_t *context_p, cn_rssi_mode_t mode, cn_client_tag_t client_tag)
 * \brief Enable/disable unsolicited signal info reporting.
 *
 * \n This routine is used to enable or disable unsolicited signal info reporting.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_SIGNAL_INFO_REPORTING
 * \n
 * \n Associated event: CN_EVENT_SIGNAL_INFO
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] mode              Enable/Disable event reporting
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_signal_info_reporting(cn_context_t *context_p, cn_rssi_mode_t mode, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_signal_info_reporting(cn_context_t *context_p, cn_client_tag_t client_tag)
 * \brief Query unsolicited signal info reporting.
 *
 * \n This routine is used to query unsolicited signal info reporting.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_SIGNAL_INFO_REPORTING
 * \n
 * \n Associated event: CN_EVENT_SIGNAL_INFO
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_bool_t (reporting_enabled, stored in payload section of the message structure)
 */
cn_error_code_t cn_request_get_signal_info_reporting(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_rssi_value(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Get current RSSI value.
 *
 * \n This routine is used to query the current RSSI level (Received Signal Strength Indication).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_RSSI_VALUE.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_rssi_t (payload in message structure)
 */
cn_error_code_t cn_request_rssi_value(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_clip_status(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query the status of the CLIP supplementary service.
 *
 * \n This routine is used to query the status of the CLIP supplementary service (Calling
 * \n Line Identification Presentation).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CLIP_STATUS.
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_clip_status_t (payload in message structure)
 */
cn_error_code_t cn_request_clip_status(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_clir_status(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query the status of the CLIR supplementary service.
 *
 * \n This routine is used to query the status of the CLIR supplementary service (Calling
 * \n Line Identification Restriction).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CLIR_STATUS.
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 *
 * \n If response msg.error_code is CN_SUCCESS, response.payload is \ref cn_clir_t
 *
 * \n If response msg.error_code is not CN_FAILURE or CN_SUCCESS, no particular datatype is returned (i.e. no payload).

 *
 */
cn_error_code_t cn_request_clir_status(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_cnap_status(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query the status of the CNAP supplementary service.
 *
 * \n This routine is used to query the status of the CNAP supplementary service (Calling
 * \n Name Identification Presentation).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CNAP_STATUS.
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_cnap_status_t (payload in message structure)
 */
cn_error_code_t cn_request_cnap_status(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_colr_status(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query the status of the COLR supplementary service.
 *
 * \n This routine is used to query the status of the COLR supplementary service (Connected line
 * \n identification restriction status).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_COLR_STATUS.
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_colr_status_t (payload in message structure)
 */
cn_error_code_t cn_request_colr_status(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_clir(cn_context_t* context_p, cn_clir_setting_t clir_setting, cn_client_tag_t client_tag)
 * \brief Set CLIR setting.
 *
 * \n This routine is used to set the status of the CLIR supplementary service (Calling
 * \n Line Identification Restriction).
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CLIR
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] clir_setting    \ref cn_clir_setting_t
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_clir(cn_context_t *context_p, cn_clir_setting_t clir_setting, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_query_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag)
 * \brief Query call forward info.
 *
 * \n This routine is used to query the call forwarding status.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_QUERY_CALL_FORWARD
 *
 * \param [in] context_p           Call & Networking connection context.
 * \param [in] call_forward_info_p Pointer to \ref cn_call_forward_info_t
 * \param [in] client_tag          Client tag handle specific to the client. It is
 *                                     transparent to CN and is returned as-is
 *                                     in the response.
 * \return    \ref                 cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_call_forward_info_list_t (payload in message structure)
 */
cn_error_code_t cn_request_query_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag)
 * \brief Set call forward info.
 *
 * \n This routine is used to set the call forwarding status.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CALL_FORWARD
 *
 * \param [in] context_p           Call & Networking connection context.
 * \param [in] call_forward_info_p Pointer to \ref cn_call_forward_info_t
 * \param [in] client_tag          Client tag handle specific to the client. It is
 *                                     transparent to CN and is returned as-is
 *                                     in the response.
 * \return    \ref                 cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_call_forward(cn_context_t *context_p, cn_call_forward_info_t *call_forward_info_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_modem_property(cn_context_t *context_p, cn_modem_property_t modem_property, cn_client_tag_t client_tag)
 * \brief Set modem property
 *
 * \n This routine is used to set modem properties.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_MODEM_PROPERTY
 *
 * \param [in] context_p        Call & Networking connection context.
 * \param [in] modem_property  \ref cn_modem_property_t (service_status member ignored by server for the SET request)
 *          * TODO: This is now a structure, so we should be passing a pointer and not the structure itself.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n If response msg.error_code is CN_FAILURE, response.payload (if modem returns any) is \ref cn_exit_cause_t
 */
cn_error_code_t cn_request_set_modem_property(cn_context_t *context_p, cn_modem_property_t modem_property, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_modem_property(cn_context_t *context_p, cn_modem_property_type_t type, cn_client_tag_t client_tag)
 * \brief Get modem property
 *
 *
 * \n This routine is used to get a specified modem property.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_MODEM_PROPERTY
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] type            \ref cn_modem_property_type_t
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_modem_property_t (modem property structure, sent as payload in message structure)
 */
cn_error_code_t cn_request_get_modem_property(cn_context_t *context_p, cn_modem_property_type_t type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_cssn(cn_context_t* context_p, cn_cssn_setting_t cssn_setting, cn_client_tag_t client_tag)
 * \brief Set CSSN setting.
 *
 * \n This routine is used to enable or disable the supplementary service notification.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CSSN
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] cssn_setting    \ref cn_cssn_setting_t
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_cssn(cn_context_t *context_p, cn_cssn_setting_t cssn_setting, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_dtmf_send(cn_context_t* context_p, char character, cn_client_tag_t client_tag)
 * \brief Send a DTMF tone.
 *
 * \n This routine is used to send a DTMF tone to the active call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_DTMF_SEND
 *
 * \param [in] context_p          Call & Networking connection context.
 * \param [in] dtmf_string        DTMF string, including length and string type
 * \param [in] length             Length of DTMF string
 * \param [in] string_type        String type (ASCII or BCD)
 * \param [in] dtmf_duration_time DTMF duration time (ms)
 * \param [in] dtmf_pause_time    DTMF pause time (ms)
 * \param [in] client_tag         Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_dtmf_send(cn_context_t *context_p,
                                     char *dtmf_string_p,
                                     cn_uint16_t length,
                                     cn_dtmf_string_type_t string_type,
                                     cn_uint16_t dtmf_duration_time,
                                     cn_uint16_t dtmf_pause_time,
                                     cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_dtmf_start(cn_context_t* context_p, char character, cn_client_tag_t client_tag)
 * \brief Start playing a DTMF tone.
 *
 * \n This routine is used to start playing a DTMF tone in the active call. Continue playing DTMF tone until
 * \n the stop command CN_REQUEST_DTMF_STOP has been sent. If a CN_REQUEST_DTMF_SEND request is sent while
 * \n a tone is currently playing, it should cancel the previous tone and play the new one.
 * \n
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_DTMF_START
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] character     Single character with one of 12 values: 0-9,*,#
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_dtmf_start(cn_context_t *context_p, char character, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_dtmf_stop(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief  Stop currently playing DTMF tone.
 *
 * \n This routine is used to stop currently playing DTMF tone in the active call.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_DTMF_STOP
 *
 * \param [in] context_p     Call & Networking connection context.
 * \param [in] client_tag    Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref           cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_dtmf_stop(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_call_waiting(cn_context_t* context_p, cn_call_waiting_t* call_waiting_p, cn_client_tag_t client_tag)
 * \brief Configure the current call waiting state.
 *
 * \n This routine is used to configure the current call waiting state.
 * \n Note: The service class informtaion to modify shall support either any one of the service as specified in TS 27.007.
 * \n       configuring call waiting state for multiple serivce class,
 * \n       for example service class set to "data + voice = 3"is not supported.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CALL_WAITING.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] call_waiting_p  Pointer to \ref cn_call_waiting_t
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_call_waiting(cn_context_t *context_p, cn_call_waiting_t *call_waiting_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_call_waiting(cn_context_t* context_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
 * \brief Query the call waiting state.
 *
 * \n This routine is used to configure the current call waiting state.
 * \n Note: The service class informtaion to query shall support either any one of the service as specified in TS 27.007.
 * \n       querying call waiting state for multiple serivce class,
 * \n       for example service class set to "data + voice = 3"is not supported.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_CALL_WAITING.
 *
 * \param [in] context_p      Call & Networking connection context.
 * \param [in] service_class  Service class of interest
 * \param [in] client_tag     Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref            cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_call_waiting_t (payload in message structure)
 */
cn_error_code_t cn_request_get_call_waiting(cn_context_t *context_p, cn_uint32_t service_class, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_query_call_barring(cn_context_t *context_p, char *facility_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
 * \brief Query the call barring service state
 *
 * \n This routine is used to query the call barring service state.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_QUERY_CALL_BARRING
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] facility_p      Pointer to facility string code as specified in TS 27.007 sec 7.4 (eg "AB" for All Barring services)
 * \param [in] service_class   Service class of interest
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_call_barring_t (payload in message structure)
 */
cn_error_code_t cn_request_query_call_barring(cn_context_t *context_p, char *facility_p, cn_uint32_t service_class, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_call_barring(cn_context_t *context_p, char *facility_p, cn_call_barring_t *call_barring_p, char *passwd_p, cn_client_tag_t client_tag)
 * \brief Set the call barring service state
 *
 * \n This routine is used to set the call barring service state.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CALL_BARRING
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] facility_p      Pointer to facility string code as specified in TS 27.007 sec 7.4 (eg "AB" for All Barring services)
 * \param [in] call_barring_p  Pointer to service operation /ref cn_call_barring_t
 * \param [in] passwd_p        Pointer to password string
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_call_barring(cn_context_t *context_p, char *facility_p, cn_call_barring_t *call_barring_p, char *passwd_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_change_barring_password(cn_context_t *context_p, char *facility_p, char *old_passwd_p, char *new_passwd_p, cn_client_tag_t client_tag)
 * \brief Change the call barring facility password.
 *
 * \n This routine is used to change the call barring facility password.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_CHANGE_BARRING_PASSWORD.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] facility_p      Pointer to facility string code as specified in TS 27.007 sec 7.4
 *                             (eg "AB" for All Barring services)
 * \param [in] old_passwd_p    Pointer to old Password string
 * \param [in] new_passwd_p    Pointer to new Password string
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_change_barring_password(cn_context_t *context_p, char *facility_p, char *old_passwd_p, char *new_passwd_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_query_call_barring_password(cn_context_t *context_p, char *facility_p, cn_uint32_t service_class, cn_client_tag_t client_tag)
 * \brief Query the call barring status.
 *
 * \n This routine is used to query the call barring status.
 * \n Note: The service class information to modify shall support either any one of the service as specified in TS 27.007.
 * \n       configuring call waiting state for multiple service class,
 * \n       for example service class set to "data + voice = 3"is not supported.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CALL_WAITING.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] facility_p      Pointer to facility string code as specified in TS 27.007 sec 7.4
 *                             (eg "AB" for All Barring services)
 * \param [in] service_class   Service class info
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_query_call_barring_password(cn_context_t *context_p, char *facility_p, cn_uint32_t service_class, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_call_barring_password(cn_context_t *context_p, char *facility_p, cn_call_barring_t *call_barring_p, char *passwd_p, cn_client_tag_t client_tag)
 * \brief Configure the call barring facility in call services.
 *
 * \n This routine is used to configure the current call waiting state.
 * \n Note: The service class informtaion to modify shall support either any one of the service as specified in TS 27.007.
 * \n       configuring call waiting state for multiple serivce class,
 * \n       for example service class set to "data + voice = 3"is not supported.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_CALL_WAITING.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] facility_p      Pointer to facility string code as specified in TS 27.007 sec 7.4
 *                             (eg "AB" for All Barring services)
 * \param [in] call_barring_p  Pointer to service operation /ref cn_call_barring_t
 * \param [in] passwd_p        Pointer to password string
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_call_barring_password(cn_context_t *context_p, char *facility_p, cn_call_barring_t *call_barring_p, char *passwd_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_product_profile_flag(cn_context_t* context_p, cn_uint16_t flag_id, cn_uint16_t flag_value, cn_client_tag_t client_tag)
 * \brief Set product profile flag
 *
 * \n This routine is used to set product profile flags.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] flag_id         Flag identifier
 * \param [in] flag_value      Flag value
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_product_profile_flag(cn_context_t *context_p, cn_uint16_t flag_id, cn_uint16_t flag_value, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_l1_parameter(cn_context_t* context_p, cn_uint8_t command, cn_uint16_t parameter, cn_client_tag_t client_tag)
 * \brief Set L1 parameter
 *
 * \n This routine is used to set l1 parameters.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_L1_PARAMETER.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] command         Command
 * \param [in] parameter       Parameter
 *                             For Command 1 - Set antenna path:
 *                                0: Enable Main Antenna Path Only
 *                                1: Enable Diversity Antenna Path Only
 *                                2: Enable Normal RX Diversity Mode
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_l1_parameter(cn_context_t *context_p, cn_uint8_t command, cn_uint16_t parameter, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_user_activity_status(cn_context_t* context_p, cn_user_status_t user_status, cn_client_tag_t client_tag)
 * \brief Set user activity status for modem power saving.
 *
 * \n This routine is used to set user activity status to indicate power saving possibiltities on the modem side.
 * \n One of these power saving routines is fast dormancy.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_USER_ACTIVITY_STATUS
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] user_status     Indicates whether or not the user is active/inactive.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                transparent to CN and is returned as-is
 *                                in the response.
 * \return    \ref             cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_user_activity_status(cn_context_t *context_p, cn_user_status_t user_status, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_neighbour_cells_reporting(cn_context_t *context_p, cn_bool_t enable_reporting, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
 * \brief Enable/disable unsolicited neighbour cell reporting.
 *
 * \n This routine is used to enable or disable unsolicited neighbour cell information for associated RAT type.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING
 * \n
 * \n Associated event: CN_EVENT_NEIGHBOUR_CELLS_INFO
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] enable_reporting  Enable/Disable event reporting
 * \param [in] rat_type          RAT type
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_neighbour_cells_reporting(cn_context_t *context_p, cn_bool_t enable_reporting, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_neighbour_cells_reporting(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
 * \brief Query unsolicited neighbour cell reporting status
 *
 * \n This routine is used to query unsolicited neighbour cell reporting status.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING
 * \n
 * \n Associated event: CN_EVENT_NEIGHBOUR_CELLS_INFO
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] rat_type          RAT type
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_bool_t (reporting_enabled, stored in payload section of the message structure)
 */
cn_error_code_t cn_request_get_neighbour_cells_reporting(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_neighbour_cells_complete_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
 * \brief Query serving cell and neighbouring cells information
 *
 * \n This routine is used to query serving cell and complete neighbouring cells information.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO
 *
 * \param [in] context_p        Call & Networking connection context.
 * \param [in] rat_type          RAT type
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                        transparent to CN and is returned as-is
 *                                        in the response.
 * \return    \ref                    cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_neighbour_cells_info_t (payload in message structure)
 */
cn_error_code_t cn_request_get_neighbour_cells_complete_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_neighbour_cells_extd_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag)
 * \brief Query serving cell and extended neighbouring cells information
 *
 * \n This routine is used to query neighbouring cells extended information.
 * \n  Note that this API will NOT provide the GSM Neighbor cells information for 2G RAT
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO
 *
 * \param [in] context_p        Call & Networking connection context.
 * \param [in] rat_type          RAT type
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                        transparent to CN and is returned as-is
 *                                        in the response.
 * \return    \ref                    cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_neighbour_cells_info_t (payload in message structure)
 */
cn_error_code_t cn_request_get_neighbour_cells_extd_info(cn_context_t *context_p, cn_neighbour_rat_type_t rat_type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_bool_t enable_reporting, cn_client_tag_t client_tag)
 * \brief Enable/disable event reporting for a particular event.
 *
 * \n This routine is used to enable or disable unsolicited event reporting for the specified event.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_EVENT_REPORTING
 * \n
 * \n Configurable events: CN_EVENT_MODEM_REGISTRATION_STATUS
 * \n                      CN_EVENT_TIME_INFO
 * \n                      CN_EVENT_RAB_STATUS
 * \n                      CN_EVENT_NETWORK_INFO
 * \n
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] type              \ref cn_event_reporting_type_t
 * \param [in] enable_reporting  Enable/disable reporting for specified event.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_bool_t enable_reporting, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_client_tag_t client_tag)
 * \brief Query event reporting for a particular event.
 *
 * \n This routine is used to query the event reporting status for a specified event.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_EVENT_REPORTING
 * \n
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] type              \ref cn_event_reporting_type_t
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_bool_t (reporting_enabled; payload found in message structure)
 */
cn_error_code_t cn_request_get_event_reporting(cn_context_t *context_p, cn_event_reporting_type_t type, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_rab_status(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query RAB status
 *
 * \n This routine is used to query RAB status.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_RAB_STATUS
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_rab_status_t (payload in message structure)
 */
cn_error_code_t cn_request_rab_status(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_baseband_version(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Query baseband version
 *
 * \n This routine is used to query baseband version.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_BASEBAND_VERSION
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_baseband_version_t (payload in message structure)
 */
cn_error_code_t cn_request_baseband_version(cn_context_t *context_p, cn_client_tag_t client_tag);

/**
 * \fn cn_error_code_t cn_request_get_pp_flags(cn_context_t *context_p, cn_bool_t all_flags, cn_uint16_t flag_id, cn_client_tag_t client_tag);
 * \brief Query pp flags
 *
 * \n This routine is used to query the value of all or once specified pp flag
 * \n as it is stored in the modem.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_PP_FLAGS
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] all_flags         Boolean stating of all or a specific flag is to be retrieved
 * \param [in] flag_id           Id of the requested pp flag
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_pp_flag_list_t (payload in message structure)
 */
cn_error_code_t cn_request_get_pp_flags(cn_context_t *context_p, cn_bool_t all_flags, cn_uint16_t flag_id, cn_client_tag_t client_tag);

/**
 * \fn cn_error_code_t cn_request_modify_emergency_number_list(cn_context_t* context_p, cn_emergency_number_operation_t operation, cn_emergency_number_config_t* config_p, cn_client_tag_t client_tag)
 * \brief Modify the emergency number list stored in the modem.
 *
 * \n This routine is used to modify the emergency number list stored in the modem.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST.
 *
 * In case of CN_EMERGENCY_NUMBER_OPERATION_REMOVE, only 'emergency_number' should
 * be specified in the config structure (identifying the entry in the list so
 * that it can be removed).
 *
 * In case of CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST, the config structure should
 * be omitted (using NULL as parameter value).
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] operation         Type of operation
 * \param [in] config_p          Pointer to \ref cn_emergency_number_config_t
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_modify_emergency_number_list(cn_context_t *context_p, cn_emergency_number_operation_t operation, cn_emergency_number_config_t *config_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_emergency_number_list(cn_context_t* context_p, cn_client_tag_t client_tag)
 * \brief Get the emergency number list stored in the modem.
 *
 * \n This routine is used to get the emergency number list stored in the modem.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_emergency_number_list_t (payload in message structure)
 */
cn_error_code_t cn_request_get_emergency_number_list(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_error_code_t cn_request_nmr_info(cn_context_t *context_p, cn_nmr_rat_type_t rat, cn_nmr_utran_type_t utran, cn_client_tag_t client_tag)
 * \brief Request to get the NMR information
 *
 * \n  This routine is used to get NMR information for GERAN and UTRAN.
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] rat               Type of rat.
 * \param [in] utran             Type of nmr.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_nmr_info_t (payload in message structure)
 */
cn_error_code_t cn_request_nmr_info(cn_context_t *context_p, cn_nmr_rat_type_t rat, cn_nmr_utran_type_t utran, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_get_timing_advance_value(cn_context_t *context_p,cn_client_tag_t client_tag)
 * \brief Query timing advance value
 *
 * \n This routine is called by Application to query timing advance value.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_TIMING_ADVANCE
 *
 * \param [in] context_p         Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_timing_advance_info_t (payload in message structure)
 */
cn_error_code_t cn_get_timing_advance_value(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_hsxpa_mode(cn_context_t *context_p, cn_hsxpa_mode_t hsxpa_mode, cn_client_tag_t client_tag);
 * \brief Set HSXPA capability
 *
 * \n This routine is used to enable or disable the HSDPA-HSUPA capability
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_HSXPA_MODE
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] hsxpa_mode    \ref cn_hsxpa_mode_t
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_hsxpa_mode(cn_context_t *context_p, cn_hsxpa_mode_t hsxpa_mode, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_hsxpa_mode(cn_context_t *context_p, cn_client_tag_t client_tag);
 * \brief Get HSXPA current status
 *
 * \n This routine is used to query the current status of the HSDPA-HSUPA capability
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_HSXPA_MODE
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_hsxpa_mode_t (payload in message structure)
 */
cn_error_code_t cn_request_get_hsxpa_mode(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_hsxpa_mode(cn_context_t *context_p, cn_hsxpa_mode_t hsxpa_mode, cn_client_tag_t client_tag);
 * \brief Set HSXPA capability
 *
 * \n This routine is used to enable or disable the HSDPA-HSUPA capability
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_HSXPA_MODE
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] hsxpa_mode    \ref cn_hsxpa_mode_t
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_hsxpa_mode(cn_context_t *context_p, cn_hsxpa_mode_t hsxpa_mode, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_get_hsxpa_mode(cn_context_t *context_p, cn_client_tag_t client_tag);
 * \brief Get HSXPA current status
 *
 * \n This routine is used to query the current status of the HSDPA-HSUPA capability
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_GET_HSXPA_MODE
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] client_tag        Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return    \ref               cn_error_code_t
 *
 * \n The datatype used for the response is \ref cn_hsxpa_mode_t (payload in message structure)
 */
cn_error_code_t cn_request_get_hsxpa_mode(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_request_ss(cn_context_t *context_p, cn_ss_command_t ss_command, cn_client_tag_t client_tag);
 * \brief Send supplementary service command
 *
 * \n This routine is used to send a supplementary service command.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SS.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] ss_command      \ref cn_ss_command_t
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return     \ref            cn_error_code_t
 *
 * \n The datatype used for the response (payload in message structure)
 *    is \ref cn_ss_response_t if ss_command->additional_results is true.
 */
cn_error_code_t cn_request_ss(cn_context_t *context_p, cn_ss_command_t ss_command, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_reg_status_event_config(cn_context_t *context_p, cn_reg_status_trigger_level_t trigger_level, cn_client_tag_t client_tag)
 * \brief Configure when the registration status event should be sent.
 *
 * \n This routine is used to configure when CN_EVENT_MODEM_REGISTRATION_STATUS should be
 * \n sent. It is originated from the modem side when parameters in \ref cn_registration_info_t
 * \n have been updated. Due to the nature of these parameters the event may come frequently.
 * \n
 * \n Since the application CPU is awoken from suspended state every time this event is sent
 * \n by the modem it has significant affect on power consumption.
 * \n
 * \n This command makes it possible to specify which parameter changes in the event that
 * \n should trigger the event on the modem side. Depending on user state on the application
 * \n side, it is not always necessary that all parameter changes should trigger the event.
 * \n
 * \n Since CNS have several clients the modem is configured according to the client that
 * \n have the highest (most demanding) trigger level. Note that if no configuration is made
 * \n by any client, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM is the default (event disabled).
 * \n
 * \n As a side note, \ref cn_request_set_event_reporting can be used to configure the event
 * \n subscription in general. Also, \ref cn_request_registration_state_normal and
 * \n \ref cn_request_registration_state_normal are not affected by this command.
 * \n
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_REG_STATUS_EVENT_CONFIG.
 * \n
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] trigger_level   \ref cn_reg_status_trigger_level_t
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return     \ref            cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_reg_status_event_config(cn_context_t *context_p, cn_reg_status_trigger_level_t trigger_level, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_rat_name(cn_context_t *context_p, cn_client_tag_t client_tag)
 * \brief Query the current RAT name.
 *
 * \n This routine is used to query the current RAT name.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_RAT_NAME.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return     \ref            cn_error_code_t
 *
 * \n The datatype used for the response (payload in message structure)
 *    is \ref cn_rat_name_t
 */
cn_error_code_t cn_request_rat_name(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_modem_power_off(cn_context_t *context_p, cn_client_tag_t client_tag)
 * \brief Request modem power off.
 *
 * \n This routine is used to tell the modem to go to a power off state in a controllable manner.
 * \n In case of a system shutdown CN_REQUEST_RF_OFF cannot be used as it affects only the RF
 * \n sub system of the modem. So this request should be used instead.
 * \n
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_MODEM_POWER_OFF.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return     \ref            cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_modem_power_off(cn_context_t *context_p, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_send_tx_back_off_event(cn_context_t *context_p, cn_tx_back_off_event_t event, cn_client_tag_t client_tag)
 * \brief Send TX back off event to the modem.
 *
 * \n This routine is used to send a TX back off event to the modem. The modem will decide if
 * \n it is relevant to activate or deactivate TX power reduction based on the overall state
 * \n of all the back off categories. This functionality is needed due to FCC regulations.
 * \n
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SEND_TX_BACK_OFF_EVENT.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] event           TX back off event.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return     \ref            cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_send_tx_back_off_event(cn_context_t *context_p, cn_tx_back_off_event_t event, cn_client_tag_t client_tag);


/**
 * \fn cn_error_code_t cn_request_set_default_nvmd(cn_context_t *context_p, cn_client_tag_t client_tag)
 * \brief  Write default non-volatile modem data.
 *
 * \n This routine is used to write default non-volatile modem data.
 * \n
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_DEFAULT_NVMD.
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                                  transparent to CN and is returned as-is
 *                                  in the response.
 * \return     \ref            cn_error_code_t
 *
 * \n No particular datatype is used for the response (i.e. no payload).
 */
cn_error_code_t cn_request_set_default_nvmd(cn_context_t *context_p, cn_client_tag_t client_tag);


#ifdef ENABLE_FTD
/**
 * \fn cn_error_code_t cn_request_set_empage(cn_context_t *context_p, cn_empage_t *empage_p , cn_client_tag_t client_tag);
 * \brief Set Engineer Mode
 *
 * \n This routine is used to set engineering mode.
 * \n Expected event type in case of successful invocation of this routine is
 * \n CN_RESPONSE_SET_EMPAGE
 *
 * \param [in] context_p       Call & Networking connection context.
 * \param [in] empage_p        Engineer mode settings
 * \param [in] client_tag      Client tag handle specific to the client. It is
 *                             transparent to CN and is returned as-is in the response.
 * \return    \ref             cn_error_code_t
 *
 */
cn_error_code_t cn_request_set_empage(cn_context_t *context_p, cn_empage_t *empage_p , cn_client_tag_t client_tag);
#endif

/*************************************************************************
 *                             EVENTS                                    *
 *************************************************************************/

/**
 * \def CN_EVENT_RADIO_STATUS
 * \brief Modem Radio Status
 *
 * \n Unsolicited indication reporting modem radio status
 * \n
 * \n The datatype used for the event is \ref cn_rf_status_t
 */


/**
 * \def CN_EVENT_MODEM_REGISTRATION_STATUS
 * \brief Modem Registration Status Indication
 *
 * \n Unsolicited indication reporting Modem Registration Status.
 * \n
 * \n The datatype used for the event is \ref cn_registration_info_t
 */


/**
 * \def CN_EVENT_NETWORK_INFO
 * \brief Network Name Indication
 *
 * \n Unsolicited indication reporting network name info
 * \n
 * \n The datatype used for the event is \ref cn_network_name_info_t
 */


/**
 * \def CN_EVENT_TIME_INFO
 * \brief Time Indication
 *
 * \n Unsolicited indication reporting current time and zone information.
 * \n
 * \n The datatype used for the event is \ref cn_time_info_t
 */


/**
 * \def CN_EVENT_USSD
 * \brief USSD Indication.
 *
 * \n Unsolicited indication reporting USSD (Unstructured Supplementary
 * \n Service Data).
 * \n
 * \n The datatype used for the event is \ref cn_ussd_info_t
 */


/**
 * \def CN_EVENT_RING
 * \brief Incoming call ring indication.
 *
 * \n Incoming call ring indication to upper layer.
 * \n
 * \n The datatype used for the event is \ref cn_call_context_t
 */


/**
 * \def CN_EVENT_CALL_STATE_CHANGED
 * \brief Call state changed indication.
 *
 * \n Call state change indication to upper layer.
 * \n
* \n The datatype used for the event is \ref cn_call_context_t
 */


/**
 * \def CN_EVENT_CALL_SUPP_SVC_NOTIFICATION
 * \brief Supplementary services notification.
 *
 * \n Supplementary service related notification from the network to upper layer.
 * \n
 * \n The datatype used for the event is \ref cn_supp_svc_notification_t
 */


/**
 * \def CN_EVENT_NET_DETAILED_FAIL_CAUSE
 * \brief Net failure cause reported to upper layer
 *
 * \n Modem reports SS or GSM network failure to upper layer
 *
 * \n Signal info indication (RSSI, BER)
 * \n
 * \n The datatype used for the event is \ref cn_net_detailed_fail_cause_t
 * \n Source of report is indicated by \ref cn_net_detailed_fail_cause_class_t.
 */


/**
 * \def CN_EVENT_NEIGHBOUR_CELLS_INFO
 *
 * \n Information related to neighbour cells
 * \n
 * \n The datatype used for the event is \ref cn_neighbour_cells_info_t
 */


/**
 * \def CN_EVENT_SIGNAL_INFO
 *
 * \n Signal info indication (RSSI, BER)
 * \n
 * \n The datatype used for the event is \ref cn_signal_info_t
 */


/**
 * \def CN_EVENT_RAB_STATUS
 *
 * \n Information related to RAB
 * \n
 * \n The datatype used for the event is \ref cn_rab_status_t
 */


/**
 * \def CN_EVENT_CALL_CNAP
 * \brief Caller name presentation.
 *
 * \n Caller name presentation from the network to upper layer.
 * \n
 * \n The datatype used for the event is \ref cn_call_context_t
 */


/**
 * \def CN_EVENT_CELL_INFO
 * \brief Cell net info
 *
 * \n Information on network cell properties and status
 * \n
 * \n The datatype used for the event is \ref cn_cell_info_t
 */


/**
 * \def CN_EVENT_GENERATE_LOCAL_COMFORT_TONES
 * \brief Local comfort tone generation indication.
 *
 * \n Local comfort tone generation indication. This event will be sent if the underlying
 * \n layers do not generate comfort tones for mobile originated voice calls in the alerting
 * \n state (i.e. no in-band information). This means that the layers above is responsible to
 * \n do this locally. The event is needed for e.g. slim modem audio configurations.
 * \n
 * \n Please see 3GPP TS 24.008 section 5.2.1.5 for more information.
 * \n
 * \n The datatype used for the event is \ref cn_comfort_tone_generation_t
 */


/**
 * \def CN_EVENT_SS_STATUS_INFO
 * \brief Supplementary Services status indication.
 *
 * \n Indication carrying information about a SS request.
 * \n
 * \n The datatype used for the event is \ref cn_ss_status_info_t
 */


/**
 * \def CN_EVENT_RAT_NAME
 * \brief Current RAT Name Indication.
 *
 * \n Indication carrying information about the current RAT Name
 * \n
 * \n The datatype used for the event is \ref cn_rat_name_t
 */

/**
 * \def CN_EVENT_EMPAGE_INFO
 * \brief Current Engineer Mode Measurement Indication.
 *
 * \n Indication carrying information about the last measurement done.
 * \n
 * \n The datatype used for the event is \ref cn_empage_info_t
 */

#pragma pack(pop) /* go back to previous packing setting */

#endif /* __cn_client_h__ */
