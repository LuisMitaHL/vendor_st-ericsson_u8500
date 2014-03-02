/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SUBSCRIBE_H_
#define _SUBSCRIBE_H_

struct subscribe_t * subscribe_init();
void subscribe_deinit(struct subscribe_t *context);

int subscribe_add(struct subscribe_t *context, struct sockaddr *addr, socklen_t addr_len);
int subscribe_remove(struct subscribe_t *context, struct sockaddr *addr, socklen_t addr_len);

void subscribe_clear(struct subscribe_t *context);

struct subscribe_client_t * subscribe_get_first(struct subscribe_t *context);
struct subscribe_client_t * subscribe_get_last(struct subscribe_t *context);
struct subscribe_client_t * subscribe_get_next(struct subscribe_client_t *client);
struct subscribe_client_t * subscribe_get_previous(struct subscribe_client_t *client);

struct sockaddr * subscribe_get_addr(struct subscribe_client_t *client);
socklen_t subscribe_get_addr_len(struct subscribe_client_t *client);

#endif

