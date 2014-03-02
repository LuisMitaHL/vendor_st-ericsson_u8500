/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : simd.h
 * Description     : simd interface
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

#ifndef __simd_h__
#define __simd_h__ (1)

#include "event_stream.h"
#include "catd_modem.h"
#include "apdu.h"

/**
 * @brief Control over the amount of debug from logging functions.
 */
typedef enum {
    SIM_LOGGING_E = 0,  /**< ERRORS - When things are going wrong */
    SIM_LOGGING_I,      /**< INFO - Function entry/exit (API level) */
    SIM_LOGGING_D,      /**< DEBUG - Function entry/exit (internal calls) */
    SIM_LOGGING_V,      /**< VERBOSE - Printing lots of internal data */
} sim_logging_t;


/**
 * @brief Add an event stream object to the reader thread
 * @param es    Pointer to event stream object to add
 */
// FIXME: Rename to simd_add_es
int                     catd_add_es(ste_es_t * es);

/**
 * @brief Remove an event stream based on file descritpr
 * @param fd    FIle descritor
 */
// FIXME: Rename to simd_rem_es
int                     catd_rem_es(int fd);




/**
 * @brief Context of the connection.
 */
typedef struct {
    int                     fd; /**< Socket of client */
} ste_sim_client_context_t;


/**
 * @brief Handle input command from the socket.
 * @param cmd   Command to handle
 * @param client_tag Tag supplied by the client
 * @param buf   Payload of command
 * @param len   Sizeof payload
 * @param cc    Client context
 */
void
ste_simd_handle_command(uint16_t cmd, uintptr_t client_tag,
                        const char *buf, uint16_t len,
                        ste_sim_client_context_t * cc);

// TODO: Log functions should be moved to common libs
/**
 * @brief Print a log message.
 * @param level Log level.
 * @param str   String to print.  Should not contain a \n
 * @param client_tag    tag
 */
void                    catd_log_s(sim_logging_t level, const char *str,
                                   uintptr_t client_tag);


/**
 * @brief Print a log message.
 * @param level Log level.
 * @param fmt   Format string.
 * @param ...   Parameters to print, see printf.
 */
void                    catd_log_f(sim_logging_t level, const char *fmt, ...);

/**
 * @brief Print a log message.
 * @param level Log level.
 * @param str   Prefix string
 * @param buf   Prints the contents of the buffer prefixed by str to log as a hex string.
 * @param n     The number of bytes to print from buf.
 */
void                   catd_log_b(sim_logging_t level, const char *str, const void *buf, size_t n);

/**
 * @brief Print a log message.
 * @param level Log level.
 * @param str   Prefix string
 * @param apdu  Apdu to print as hexstring
 */
void                    catd_log_a(sim_logging_t level, const char *str,
                                   const ste_apdu_t * apdu);


/**
 * @brief This function sends a ping message to the main service.
 *
 * The ping service is a simple way to verify that the round-trip communication
 * path has been successfully established.
 * @param fd    The input socket file descriptor for the ping requester.
 * @param client_tag Tag supplied by the client
 */
void                    simd_sig_ping(int fd, uintptr_t client_tag);


/**
 * @brief Fetch the modem object
 * @return      Modem object or 0.
 */
ste_modem_t            *catd_get_modem();


/**
 * @brief Initiate a startup for the entire system.
 * @param fd    The input socket file descriptor
 * @param client_tag Tag supplied by the client
 */
void                    simd_sig_startup(int fd, uintptr_t client_tag);

/**
 * @brief Signal to simd that a daemon has completed its startup. Only used
 * internallly during startup initialization
 * @param daemon daemon id
 * @param status
 */

#define SIMD_DAEMON_NONE  0
#define SIMD_DAEMON_SIMD  1
#define SIMD_DAEMON_UICCD 2
#define SIMD_DAEMON_CATD  3
void simd_sig_startup_completed(int fd, int daemon, int status, uintptr_t client_tag);

/**
 * @brief Initiate a shutdown for the entire system.
 * @param fd    The input socket file descriptor
 * @param client_tag Tag supplied by the client
 */
void                    simd_sig_shutdown(int fd, uintptr_t client_tag);



/**
 * @brief This function
 *
 * @param fd    The input socket file descriptor
 * @param client_tag Tag supplied by the client
 */
void                    simd_sig_connect(int fd, uintptr_t client_tag);


/**
 * @brief This function
 *
 * @param fd    The input socket file descriptor
 * @param client_tag Tag supplied by the client
 */
void                    simd_sig_disconnect(int fd, uintptr_t client_tag);


#endif
