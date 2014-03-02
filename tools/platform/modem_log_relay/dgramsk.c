/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>


#include "dgramsk.h"
#include "utils.h"

struct dgram_conn_t {
	int sk_fd;
	int sk_recv_or_send;
	struct sockaddr_un sk_adrr;
	int max_size_msg;
};

int dgram_sk_max_size_msg;

struct dgram_conn_t *dgram_conn_p;

/*  previous interface uses the 2 above global variables */
/*  the 2 global variables, build in a single exec, prevent from
 *  creating several instance   */

void *dgram_sk_init(int type, char *name, int max_size)
{
	int n;
	struct dgram_conn_t *ret = malloc(sizeof(struct dgram_conn_t));
	if (ret == NULL)
		return NULL;
	ret->max_size_msg = max_size;

	ret->sk_fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if(ret->sk_fd < 0){
		free(ret);
		return NULL;
	}

	ret->sk_adrr.sun_family = AF_LOCAL;

	strcpy(ret->sk_adrr.sun_path, name);

	if (type == DGRAM_SK_SERVER) {

		unlink(name);

		n = bind(ret->sk_fd, \
				(struct sockaddr *)&(ret->sk_adrr), \
				sizeof(ret->sk_adrr));

		EXTRADEBUG("Bind of mal_sk_init = %d", n);
	}
	return (void *)ret;
}

void dgram_sk_destroy(void *dgram_p)
{
	free(dgram_p);
}


int dgram_sk_send(void *p, char *msg, int len)
{
	int n;
	struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
	n = sendto(dgram_p->sk_fd, msg, len, 0, \
			(struct sockaddr *)&dgram_p->sk_adrr, \
			sizeof(dgram_p->sk_adrr));

	return n;
}

int dgram_recvfrom_timeout(int sk_fd, char *msg, int max_size_msg, \
		int flags, struct sockaddr *sk_addr, \
		socklen_t *fromlen, int timeout)
{
	struct timeval stTimeOut;
	fd_set stReadFDS;
	int t, n;

	FD_ZERO(&stReadFDS);

	/* Timeout of one second */
	stTimeOut.tv_sec = timeout;
	stTimeOut.tv_usec = 0;

	FD_SET(sk_fd, &stReadFDS);

	while (1) {

		t = select(sk_fd + 1, \
				&stReadFDS, NULL, NULL, &stTimeOut);

		if (t == 0) {
			EXTRADEBUG("socket timeout\n");
			return 0;
		} else if (t < 0) {
			EXTRADEBUG("socket error returned\n");
			return -1;
		} else if (FD_ISSET(sk_fd, &stReadFDS)) {

			EXTRADEBUG("There are data pending to be read");

			n = recvfrom(sk_fd, \
				msg, max_size_msg, flags, \
				sk_addr, \
				fromlen);
			return n;
		}
	}
}


int dgram_sk_recv_timeout(void *p, char *msg, int timeout)
{
	int n;
	socklen_t fromlen;
	struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
	fromlen = sizeof(dgram_p->sk_adrr);

	n = dgram_recvfrom_timeout(dgram_p->sk_fd, \
			msg, dgram_p->max_size_msg, 0, \
			(struct sockaddr *)&(dgram_p->sk_adrr),
			&fromlen, timeout);

	return n;
}

int dgram_sk_recv(void *p, char *msg)
{
	int n;
	socklen_t fromlen;
	struct dgram_conn_t *dgram_p = (struct dgram_conn_t *)p;
	fromlen = sizeof(dgram_p->sk_adrr);

	n = recvfrom(dgram_p->sk_fd, msg, dgram_p->max_size_msg, 0, \
			(struct sockaddr *)&(dgram_p->sk_adrr), \
			&fromlen);

	return n;
}



