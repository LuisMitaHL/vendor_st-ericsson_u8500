/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef DGRAMSK_H
#define DGRAMSK_H

enum {
	DGRAM_SK_SERVER,
	DGRAM_SK_CLIENT
};

void *dgram_sk_init(int type, char *name, int max_size);
void dgram_sk_destroy(void *dgram_p);
int dgram_sk_recv(void *dgram_p, char *msg);
int dgram_sk_send(void *dgram_p, char *msg, int len);
int dgram_sk_recv_timeout(void *dgram_p, char *msg, int timeout);

#endif /* ifndef DGRAMSK_H */
