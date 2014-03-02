/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: simpbd include, exposing simpbd api.
 *
 * Author:  Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 *          Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *
 */

#ifndef __simpbd_h__
#define __simpbd_h__ (1)

#include "sim.h"
#include "simpbd_common.h"


/**
 * @brief Get number of required socket servers
 */
int simpbd_get_num_servers(void);


/**
 * @brief Initialize server
 */
int simpbd_init(void);


/**
 * @brief Send event data to client
 */
int simpbd_handle_event(ste_simpb_cause_t cause, void *data_p, size_t data_size);


/**
 * @brief Send event data to client(s)
 */
int simpbd_send_event(const simpb_message_t *msg_p, const size_t size);


/**
 * @brief Send response data to client
 */
int simpbd_handle_response(simpbd_client_t *client_p, void *client_tag_p, simpbd_request_id_t request_id, ste_simpb_result_t result, void *data_p, size_t data_size);


/**
 * @brief Send response data to client
 */
int simpbd_send_response(const simpbd_client_t *client_p, const simpb_message_t *msg_p, const size_t size);


/**
 * @brief Close connection to client
 */
int simpbd_close_client(const int client_id);


/**
 * @brief Shut down server
 */
int simpbd_shutdown(void);

#endif /* __simpbd_h__ */
