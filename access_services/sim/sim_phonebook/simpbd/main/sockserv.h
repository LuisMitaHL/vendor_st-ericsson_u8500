/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *  UNIX domain socket server external interface.
 *
 *  Author: Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */


#ifndef __sockserv_h__
#define __sockserv_h__ (1)

#include <sys/types.h>
#include <string.h>
#include <stdint.h>

typedef enum {
    SOCKSERV_REASON_CONNECT_REQUEST,
    SOCKSERV_REASON_CONNECTED,
    SOCKSERV_REASON_RECEIVE,
    SOCKSERV_REASON_DISCONNECTED,
} sockserv_reason_t;

typedef unsigned long sockserv_context_t;


/**
 * @brief Callback function used by the socket server service
 *
 * @param instance  Socket server instance
 * @param client    Client reference
 * @param context_p Pointer to context
 * @param reason    Reason for callback
 * @param buf       Pointer to message buffer
 * @param size      Number of bytes in message buffer
 *
 * @returns         Number of bytes processed
 *
 */
typedef int sockserv_callback_t(const int instance, const int client,
                                const void *context_p, const sockserv_reason_t reason,
                                const uint8_t *buf_p, const size_t size);


/**
 * The sockserv_init() function initialises the socket server service
 *
 * @brief Initialise the socket server service.
 *
 * @param max_servers Number of simultaneous servers allowed.
 *
 * @return Returns an int.
 * @retval >=0 if successful
 * @retval <0 if not.
 */
int sockserv_init(const int max_servers);


/**
 * The sockserv_create() function creates and initialises an instance of the socket server
 *
 * @brief Create and initialise a socket server.
 *
 * @param name      Name of socket.
 * @param max_connections Number of simultaneous connections allowed.
 * @param cb_func   Pointer to callback function
 *
 * @return Returns an int, socket server instance number.
 * @retval >=0 if successful
 * @retval <0 if not.
 */
int sockserv_create(const char *name, const int max_clients,
                    sockserv_callback_t *cb_func);


/**
 * The sockserv_set_context() function to sets context for a client connected socket
 *
 * @brief Send through connected socket
 *
 * @param instance  Socket server instance
 * @param client    Client reference
 * @param context_p Pointer to context
 *
 * @return Returns an int.
 * @retval >=0 if successful
 * @retval <0 if not.
 */
int sockserv_set_context(const int instance, const int client, void *context_p);


/**
 * The sockserv_send() function to send on socket
 *
 * @brief Send through connected socket
 *
 * @param instance  Socket server instance
 * @param client    Client reference
 * @param buf       Pointer to buffer
 * @param len       Buffer length
 *
 * @return Returns an int.
 * @retval >=0 if successful
 * @retval <0 if not.
 */
int sockserv_send(const int instance, const int client, const void *buf_p,
                  const size_t len);


/**
 * The sockserv_close() function closes a connected socket
 *
 * @brief Close connected socket
 *
 * @param instance  Socket server instance
 * @param client    Client reference
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int sockserv_close(const int instance, const int client);


/**
 * The sockserv_destroy() function shuts down a socket server
 *
 * @brief Shut down a socket server.
 *
 * @param instance  Socket server instance
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int sockserv_destroy(const int instance);


/**
 * The sockserv_shutdown() function shuts down the socket server service
 *
 * @brief Shuts down the socket server service.
 *
 * @return Returns an int.
 * @retval 0 if successful
 * @retval -1 if not.
 */
int sockserv_shutdown(void);

#endif /* __sockserv_h__ */
