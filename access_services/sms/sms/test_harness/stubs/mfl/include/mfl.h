/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * @def _MFL_H
 *  Inclusion guard
 */
#ifndef _MFL_H
#define _MFL_H 1

#include <stdio.h>

#include "t_os.h"
#include "t_sys.h"

/** @mainpage Modem Function Library
 *
 * @section intro_sec Introduction
 * The library provides a framework for communicating with a ST-Ericsson modem.@n
 * It provides clients with functionality to begin and end a session towards the modem.@n
 * The framework is intended to be used with the different mfl-proxy category libraries.@n
 * @subsection API
 * @ref mfl.h
 *
 * @section support_sec Macro support
 * The following macros are supported:
 * @li SEND()
 * @li SIGNAL_ALLOC()
 * @li SIGNAL_FREE()
 * @li SENDER()
 *
 * @section support_func_sec Function support
 * The following function calls are supported as macros:
 * @li Do_ClientTag_Set()
 * @li Do_SR_Resolve_Pid()
 *
 */


/**
 * @file mfl.h
 *
 *
 */

/**
 * The mfl_session_begin() function.
 *
 * Begin a session towards the modem.
 *
 * The method will try to do a connect() indefinitely in 10 seconds intervals.
 *
 * @brief Begin a modem session
 *
 * @return A modem session
 * @retval NULL Failed to begin the session. check errno for error codes
 *
 * @see mfl_session_end(), mfl_session_get_file_descriptor(), mfl_session_get_signal(), mfl_session_get_signal_with_timeout()
 *
 */
modem_session_t *mfl_session_begin(
    void);

/**
 * The mfl_session_end() function.
 *
 * End the session towards the modem.
 *
 * @brief End a modem session
 *
 * @param [in] modem_session_ptr The session that is to be ended
 *
 * @see mfl_session_begin(), mfl_session_get_file_descriptor(), mfl_session_get_signal(), mfl_session_get_signal_with_timeout()
 *
 */
void mfl_session_end(
    modem_session_t * modem_session_ptr);

/**
 * The mfl_session_begin_named_socket() function.
 *
 * Begin a session towards the modem using Unix named socket. This may be called instead of
 * mfl_session_begin() when using mfl to communicating for test purposes.
 *
 * The method will try to do a connect() indefinitely in 10 seconds intervals.
 *
 * The Unix named socket must exist in the system.
 *
 * @brief Begin a modem session over Unix named socket
 *
 * @param [in] name_ptr The Unix named socket address.
 *
 * @return A modem session
 * @retval NULL Failed to begin the session. Check errno for error codes
 *
 * @see mfl_session_end(), mfl_session_get_file_descriptor(), mfl_session_get_signal(), mfl_session_get_signal_with_timeout()
 *
 */
modem_session_t *mfl_session_begin_named_socket(
    const char *const name_ptr);

/**
 * The mfl_session_get_file_descriptor() function.
 *
 * Get's the file descriptor that may be used by the client to have a
 * select() of its own. The client must then do a mfl_get_signal_receive() to
 * reterive the actual signal.
 *
 * @brief Get's the file descriptor
 *
 * @param [in] modem_session_ptr The session to the modem
 * @param [out] fd_ptr File descriptor
 *
 * @see mfl_session_begin(), mfl_session_end(), mfl_session_get_signal(), mfl_session_get_signal_with_timeout()
 *
 */
void mfl_session_get_file_descriptor(
    modem_session_t * modem_session_ptr,
    int *fd_ptr);

/**
 * The mfl_session_get_signal() function.
 *
 * Gets one OSE signal received from the modem. It returns a signal that may be given to a @n
 * Response_xxx_xxxx() SwBP function. @n
 * The signal that is returned shall not be freed by other means than by a SIGNAL_FREE() call. @n
 * This function will only return when a signal is received.
 *
 * @brief Get's a signal from the modem
 *
 * @param [in] modem_session_ptr The session to the modem
 *
 * @return A signal
 * @retval NULL No signal returned. Check errno for error codes
 *
 * @see mfl_session_begin(), mfl_session_end(), mfl_session_get_file_descriptor()
 *
 */
void *mfl_session_get_signal(
    modem_session_t * const modem_session_ptr);

/**
 * The mfl_session_get_signal_with_timeout() function.
 *
 * Gets one OSE signal received from the modem. It returns a signal that may be given to a @n
 * Response_xxx_xxxx() SwBP function. @n
 * The signal that is returned shall not be freed by other means than by a SIGNAL_FREE() call. @n
 * This function will return when a signal is received or the given timeout has occurred.
 *
 * @brief Get's a signal from the modem
 *
 * @param [in] modem_session_ptr The session to the modem
 * @param [in] timeout_in_milliseconds The time in milliseconds that the function will wait for a signal before returning
 *
 * @return A signal
 * @retval NULL No signal returned. Check errno for error codes
 *
 * @see mfl_session_begin(), mfl_session_end(), mfl_session_get_file_descriptor()
 *
 */
void *mfl_session_get_signal_with_timeout(
    modem_session_t * const modem_session_ptr,
    int timeout_in_milliseconds);

/**
 * The mfl_signal_free() function.
 *
 * Free's the given signal. @n
 * This method must always be used to free data allocated by mfl_signal_alloc(). @n
 * Internal data is added that will result in a failure if the signal is freed using free().
 *
 * @brief Free a signal
 *
 * @param [in] signal_ptr_ptr The signal.
 *
 *
 */
void mfl_signal_free(
    void **signal_ptr_ptr);

/**
 * The mfl_request_control_block_alloc_with_client_tag() function.
 *
 * Creates a request control block that is used to control the behavior of the SwBp request. @n
 * Only NO_WAIT_MODE is supported. it is internally set by the function. @n
 *
 * Adds the given client_tag data to the request control block. The "Client Tag" functionality corresponds @n
 * to the behavior existing in the ST-Ericsson OSE environment. exception to this is that ClientTag @n
 * is a uint32 and is not suited to carry a  pointer in some environments.
 *
 * @brief Creates a request block and adds a client tag
 *
 * @param [in] modem_session_ptr The session to the modem
 * @param [in] client_tag The client data that shall be returned.
 *
 * @return A request control block
 * @retval NULL Failed to create the request control block. Check errno for error codes
 *
 * @see mfl_request_control_block_free(), mfl_set_client_tag(), mfl_get_client_tag()
 *
 */
RequestControlBlock_t *mfl_request_control_block_alloc(
    modem_session_t * const modem_session_ptr,
    const ClientTag_t client_tag);

/**
 * The mfl_request_control_block_free() function.
 *
 * Free's a given request control block
 *
 * @brief Free's a request control block
 *
 * @param [in] request_control_block_ptr The request control block to release
 *
 * @see mfl_request_control_block_alloc(), mfl_set_client_tag(), mfl_get_client_tag()
 *
 */
void mfl_request_control_block_free(
    RequestControlBlock_t * request_control_block_ptr);

/**
 * The mfl_set_client_tag() function.
 *
 * Set the ClientTag in the given request control block to the given value. @n
 *
 * When the requests that include the request control block is completed the client may get the ClientTag @n
 * by using mfl_get_client_tag(). @n
 *
 * The mfl_set_client_tag() may be used multiple times on the same request control block.
 *
 * @brief Set ClientTag
 *
 * @param [in] request_control_block_ptr The request_controle_block that will be changed
 * @param [in] client_tag The ClientTag
 *
 * @see mfl_request_control_block_alloc(), mfl_request_control_block_free(), mfl_get_client_tag()
 *
 */
void mfl_set_client_tag(
    RequestControlBlock_t * request_control_block_ptr,
    const ClientTag_t client_tag);

/**
 * The mfl_get_client_tag() function.
 *
 * Get the ClientTag value that is contained in the signal. @n
 *
 * The ClientTag will match the value given in the request control block used when calling @n
 * a Request_XXXXX_XXXX.
 *
 * @brief Get ClientTag
 *
 * @param [in] signal_ptr The signal
 *
 * @retval The ClientTag
 *
 * @see mfl_request_control_block_alloc(), mfl_request_control_block_free(), mfl_set_client_tag()
 *
 */
ClientTag_t mfl_get_client_tag(
    void *signal_ptr);

#endif
