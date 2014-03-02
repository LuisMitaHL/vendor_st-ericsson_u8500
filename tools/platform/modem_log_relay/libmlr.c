/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "dgramsk.h"

#define LIBMLR_C

#include "libmlr.h"


int mlr_sup_sk_send(char *msg, int len)
{
	int n;

	n = dgram_sk_send(mlr_sup_dgram_sk[MLR_SUP_SK_REQ], msg, len);

	return n;
}

int mlr_sup_sk_recv(char *msg)
{
	int n;
	n = dgram_sk_recv(mlr_sup_dgram_sk[MLR_SUP_SK_RSP], msg);

	return n;
}

int sup_mlr_sk_send(char *msg, int len)
{
	int n;

	n = dgram_sk_send(mlr_sup_dgram_sk[MLR_SUP_SK_RSP], msg, len);

	return n;
}

int sup_mlr_sk_recv(char *msg, int timeout)
{
	int n;

	n = dgram_sk_recv_timeout(mlr_sup_dgram_sk[MLR_SUP_SK_REQ], msg, \
			timeout);

	return n;
}

int mlr_send_coredump_gen(int timeout)
{
	int  n;
	char buf[MLR_SUP_DGRAM_SK_LENGTH_MSG_MAX];

	sup_mlr_sk_send(MSG_GEN_MODEM_COREDUMP, strlen(MSG_GEN_MODEM_COREDUMP));

	n = sup_mlr_sk_recv(buf, timeout);

	if (n > 0) {
		if (strncmp(buf, "OK", 2) == 0)
			return 1;
		else
			return -2;
	}
	return n;
}

void mlr_send_modem_ready(void)
{
	sup_mlr_sk_send(MSG_MODEM_READY, strlen(MSG_MODEM_READY));
}

int mlr_sup_init_sk(int ident)
{
	int i;

	for (i = 0; i < MLR_SUP_SK_MAX; i++) {
		mlr_sup_dgram_sk[i] = dgram_sk_init(\
				mlr_sup_dgram_sk_type[ident][i], \
				mlr_sup_dgram_sk_name[i], \
				MLR_SUP_DGRAM_SK_LENGTH_MSG_MAX);
		if (!mlr_sup_dgram_sk[i])
			return -1;
	}

	return 0;
}



