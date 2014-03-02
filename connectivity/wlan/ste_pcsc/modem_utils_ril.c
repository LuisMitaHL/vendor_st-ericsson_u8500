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
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/un.h>
#include <fcntl.h>
#include "modem_utils.h"
#include "ste_pcsc_dbg.h"

#define AT_SOCKET_NAME "/dev/socket/at_core"
#define SIM_ACCESS_CMD "AT+CSIM="
#define SIM_ACCESS_CMD_LEN 8

#define BUF_SIZE 4096

int modem_connect()
{
	int modem_sck;
	int ret;
	size_t len;
	struct sockaddr_un ux_addr;

	// create socket
	modem_sck = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (0 > modem_sck) {
		PCSC_LOGE("RIL: cannot open socket");
		return modem_sck;
	}

	// set non-blocking socket
	fcntl(modem_sck, F_SETFL, O_NONBLOCK);

	// connecting socket
	bzero((char *) &ux_addr, sizeof(ux_addr));
	ux_addr.sun_family = AF_UNIX;
	strncpy(ux_addr.sun_path, AT_SOCKET_NAME,
			sizeof(ux_addr.sun_path));
	len = strlen(ux_addr.sun_path) + sizeof(ux_addr.sun_family);
	ret = connect(modem_sck, (struct sockaddr *) &ux_addr, len);
	if (0 > ret) {
		PCSC_LOGE("RIL: cannot connect to socket");
		close(modem_sck);
		return ret;
	}

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

	int writen_bytes = 0;
	int ret = 0;
	char end_of_line[] = "\r";

	while (len - writen_bytes) {
		ret = write(sck, cmd + writen_bytes, len - writen_bytes);
		if (0 > ret) {
			PCSC_LOGE("RIL: cannoct write to socket");
			return ret;
		}
		writen_bytes += ret;
	}
	while (len + 1 - writen_bytes) {
		ret = write(sck, end_of_line, 1);
		if (0 > ret) {
			PCSC_LOGE("RIL:   cannoct write to socket");
			return ret;
		}
		writen_bytes += ret;
	}
	return writen_bytes;
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
	int nfds = sck + 1;
	int data_to_read = 1;
	int read_bytes = 0;

	timeout.tv_sec = MODEM_RESP_TIMEOUT;
	timeout.tv_usec = 0;

	//Read all available data from socket
	while (data_to_read) {
		FD_ZERO(&rfds);
		FD_SET(sck, &rfds);
		ret = select(nfds, &rfds, NULL, NULL, &timeout);

		if (0 > ret) {
			PCSC_LOGE("RIL: cannoct read socket");
			return ret;
		}

		if (!read_bytes && FD_ISSET(sck, &rfds) == 0)
			return -ETIMEDOUT;

		if (ret && FD_ISSET(sck, &rfds)) {
			ret = read(sck, buf + read_bytes, len - read_bytes);
			read_bytes += ret;
		} else
			--data_to_read;

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
	}
	return read_bytes;
}

int write_APDU_req(int sck, char *cmd, size_t len)
{
	int ret;
	char command[BUF_SIZE] = SIM_ACCESS_CMD;

	// assembling command
	ret = snprintf(command + SIM_ACCESS_CMD_LEN, BUF_SIZE - SIM_ACCESS_CMD_LEN - 1,
			"%i,\"%s\"", len, cmd);
	if (0 > ret)
		return ret;

	PCSC_LOGD("RIL: AT COMMAND: %s\n", command);

	ret = write_AT_req(sck, command, strlen(command));

    return ret;
}

int read_APDU_resp(int sck, char *buf, size_t len)
{
	int ret = 0;
	char *pos = NULL;
	char *begin_resp = NULL;
	char resp[BUF_SIZE];
	int rest = 0;

	if (len > BUF_SIZE)
		len = BUF_SIZE;

	ret = read_AT_resp(sck, resp, BUF_SIZE);

	PCSC_LOGD("RIL: AT resp: %s\n", resp);

	if (0 >= ret)
		return ret;

	// looking for command
	pos = resp;
	while ((pos < resp + ret) && (*pos == '\r' || *pos == '\n'))
		++pos;
	rest = resp + ret - pos - 1;

	if (*pos == '+') {
		while (0 < rest && *pos != '\"') {
			++pos;
			--rest;
		}
		if (0 < rest) {
			begin_resp = ++pos;
			--rest;
		}
		while (0 <= rest && *pos != '\"') {
			++pos;
			--rest;
		}
		if (0 <= rest)
			*pos='\0';
		else
			return -1;

		if (len < strlen(begin_resp) + 1)
			return -1;
		strncpy(buf, begin_resp, len);
	} else // unexpected response
		return -1;

    return strlen(begin_resp);
}
