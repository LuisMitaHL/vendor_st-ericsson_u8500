/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>


#include "dgram-sock.h"

struct dgram_conn_t {
	int sk_fd;
	dgram_sock_type_t sk_type;
	struct sockaddr_un sk_adrr;
};


void *dgram_sock_init(dgram_sock_type_t type, char *name)
{
	int n;
	struct dgram_conn_t *ret = malloc(sizeof(struct dgram_conn_t));
	if (ret == NULL)
		return NULL;

	ret->sk_fd = socket(PF_LOCAL, SOCK_DGRAM, 0);

    if (ret->sk_fd < 0) {
        EXTRADEBUG("%s: socket() %d: %s", __func__, errno, strerror(errno));
    }

    ret->sk_type = type;
	ret->sk_adrr.sun_family = AF_LOCAL;

	strcpy(ret->sk_adrr.sun_path, name);

	if (type == DGRAM_SOCK_SERVER) {

		unlink(name);

		n = bind(ret->sk_fd, \
				(struct sockaddr *)&(ret->sk_adrr), \
				sizeof(ret->sk_adrr));

		if (n < 0) {
		    EXTRADEBUG("%s: bind() %d: %s", __func__, errno, strerror(errno));
		}
	}
	return ret;
}


int dgram_sock_get_fd(void *p)
{
    struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
    return dgram_p->sk_fd;
}


void dgram_sock_destroy(void *p)
{
    struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
    close(dgram_p->sk_fd);
    if (dgram_p->sk_type == DGRAM_SOCK_SERVER) {
        unlink(dgram_p->sk_adrr.sun_path);
    }
	free(dgram_p);
}


int dgram_sock_send(void *p, char *msg, int msg_len)
{
	int n;
	struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
	n = sendto(dgram_p->sk_fd, msg, msg_len, 0, \
			(struct sockaddr *)&dgram_p->sk_adrr, \
			sizeof(dgram_p->sk_adrr));

    if (n < 0) {
        EXTRADEBUG("%s: sendto() %d: %s", __func__, errno, strerror(errno));
    }

	return n;
}


int dgram_sock_recv(void *p, char *msg, int msg_len)
{
	int n;
	socklen_t fromlen;
	struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
	fromlen = sizeof(dgram_p->sk_adrr);

	n = recvfrom(dgram_p->sk_fd, msg, msg_len, 0, \
			(struct sockaddr *)&(dgram_p->sk_adrr), \
			&fromlen);

    if (n < 0) {
        EXTRADEBUG("%s: recvfrom() %d: %s", __func__, errno, strerror(errno));
    }

	return n;
}
