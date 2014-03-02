/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * Author: Szymon Bigos <szymon.bigos@tieto.com> for ST-Ericsson
 *         Tomasz Hliwiak <tomasz.hliwiak@tieto.com> for ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <linux/caif/caif_socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include "modem_utils.h"

struct sockaddr_caif caif_sck_addr = {
		.family = AF_CAIF,
		.u.at.type = CAIF_ATTYPE_PLAIN,
};

int modem_connect()
{
	int modem_sck;
	int ret;
	int priority = CAIF_PRIO_NORMAL;
	int latency = CAIF_LINK_LOW_LATENCY;

	// create socket
	modem_sck = socket(AF_CAIF, SOCK_SEQPACKET, CAIFPROTO_AT);
	if (0 > modem_sck) {
		perror("ste_pcsc: cannot open socket");
		return modem_sck;
	} else
		printf("ste_pcsc: socket created succesfully\n");

	// set non-blocking socket
	fcntl(modem_sck, F_SETFL, O_NONBLOCK);

	// Set socket params.
	if (setsockopt(modem_sck, SOL_SOCKET, SO_PRIORITY,
			&priority, sizeof(priority)))
		perror("ste_pcsc: cannot set socket priority");
	if (setsockopt(modem_sck, SOL_CAIF, CAIFSO_LINK_SELECT,
			&latency, sizeof(latency)))
		perror("ste_pcsc: cannot set socket priority");

	// connect socket
	ret = connect(modem_sck, (struct sockaddr *)&caif_sck_addr,
				  sizeof(caif_sck_addr));
	if (0 > ret) {
		perror("ste_pcsc: cannot connect to socket");
		close(modem_sck);
		return ret;
	} else
		printf("ste_pcsc: socket connected succesfully\n");

	return modem_sck;
}

int modem_disconnect(int handler)
{
	close(handler);
	return 0;
}

#ifdef TEST_AT
int write_AT_req(int sck, char *cmd, size_t len)
#else
static int write_AT_req(int sck, char *cmd, size_t len)
#endif
{
	int writen = 0;
	int ret = 0;

	while (len-writen) {
		ret = write(sck, cmd+writen, len-writen);
		if (0 > ret) {
			perror("ste_pcsc: cannoct write to socket");
			return writen;
		}
		writen += ret;
	}
	return writen;
}

#ifdef TEST_AT
int read_AT_resp(int sck, char *buf, size_t len)
#else
static int read_AT_resp(int sck, char *buf, size_t len)
#endif
{
	fd_set rfds;
	struct timeval timeout;
	int ret;
	int nfds = sck+1;

	FD_ZERO(&rfds);
	FD_SET(sck, &rfds);

	timeout.tv_sec = MODEM_RESP_TIMEOUT;
	timeout.tv_usec = 0;

	ret = select(nfds, &rfds, NULL, NULL, &timeout);

	if (0 > ret) {
		perror("ste_pcsc: cannoct read socket");
		return ret;
	}

	if (FD_ISSET(sck, &rfds) == 0)
		return -ETIMEDOUT;

	if (ret && FD_ISSET(sck, &rfds)) {
		ret = read(sck, buf, len);
		return ret;
	}

	return 0;
}

int write_APDU_req(int sck, char *cmd, size_t len)
{
    int ret;

    //TODO:
    //Add AT cmd string
    ret = write_AT_req(sck, cmd, len);

    return ret;
}

int read_APDU_resp(int sck, char *buf, size_t len)
{
    int ret;

   //TODO:
   //Add AT cmd string
   ret = read_AT_resp(sck, buf, len);

return ret;
}
