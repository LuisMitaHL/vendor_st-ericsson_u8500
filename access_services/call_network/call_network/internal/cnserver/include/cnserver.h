/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Functionality for serving Call and Network service
 * services to connecting clients.
 */

#ifndef __cnserver_h__
#define __cnserver_h__ (1)

#include "cnsocket.h"
#include "cn_data_types.h"
#include "cn_message_types.h"

typedef void (*cnserver_client_disconnected_cb_t)(int client_id);

/**
 * @brief Get number of required socket servers
 */
int cnserver_get_num_servers(void);


/**
 * @brief Initialize server
 */
int cnserver_init(void);


/**
 * @brief Register callback function for client disconnections.
 */
void cnserver_register_client_disconnected_cb(cnserver_client_disconnected_cb_t cb);


/**
 * @brief Send event data to client(s)
 */
int cnserver_send_event(const cn_message_t *msg_p, const size_t size);


/**
 * @brief Send response data to client
 */
int cnserver_send_response(const int client_id, const cn_message_t *msg_p, const size_t size);


/**
 * @brief Close connection to client
 */
int cnserver_close_client(const int client_id);


/**
 * @brief Shut down server
 */
int cnserver_shutdown(void);


#endif /* __cnserver_h__ */
