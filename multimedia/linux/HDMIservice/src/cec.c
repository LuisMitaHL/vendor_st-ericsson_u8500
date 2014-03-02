/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Per Persson per.xb.persson@stericsson.com for
 * ST-Ericsson.
 *
 * License terms:
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <linux/types.h>
#include <errno.h>      /* Errors */
#include <stdarg.h>
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#ifdef ANDROID
#include <utils/Log.h>
#endif
#include "../include/hdmi_service_api.h"
#include "../include/hdmi_service_local.h"

const __u8 cecrxeven_val[] = {0x01}; /* Enable CEC RX events */
int cectx_cmd_id;

static int cectxcmdid_set(int cmd_id)
{
	cectx_cmd_id = cmd_id;
	return 0;
}

static int cectxcmdid_get(void)
{
	return cectx_cmd_id;
}

/* Subscribe for incoming CEC messages */
int cecrx_subscribe(void)
{
	int cecrxfd;
	int ret = 0;

	cecrxfd = open(CECRXEVEN_FILE, O_WRONLY);
	if (cecrxfd < 0) {
		LOGHDMILIBE(" failed to open %s", CECRXEVEN_FILE);
		return -1;
	}
	if (write(cecrxfd, cecrxeven_val, sizeof(cecrxeven_val)) !=
			sizeof(cecrxeven_val))
		ret = -2;
	close(cecrxfd);
	return ret;
}

int cecsenderr(void)
{
	int val;
	__u8 buf[32];
	int cmd_id;

	cmd_id = cectxcmdid_get();

	val = HDMI_CECSENDERR;
	memcpy(&buf[CMD_OFFSET], &val, 4);
	memcpy(&buf[CMDID_OFFSET], &cmd_id, 4);
	val = 0;
	memcpy(&buf[CMDLEN_OFFSET], &val, 4);

	/* Send on socket */
	return clientsocket_send(buf, CMDBUF_OFFSET + val);
}

/* Send CEC message */
int cecsend(__u32 cmd_id, __u8 in, __u8 dest, __u8 len, __u8 *data)
{
	int cecsendfd;
	int res;
	char buf[128];

	LOGHDMILIB("%s begin", __func__);

	cectxcmdid_set(cmd_id);

	res = 0;
	buf[0] = in;
	buf[1] = dest;
	buf[2] = len;
	memcpy(&buf[3], data, len);

	/* Send CEC cmd */
	cecsendfd = open(CECSEND_FILE, O_WRONLY);
	if (cecsendfd <= 0) {
		LOGHDMILIBE("***** Failed to open %s *****\n", CECSEND_FILE);
		goto cecsend_err;
	}

	res = write(cecsendfd, buf, len + 3);
	if (res != len + 3) {
		LOGHDMILIBE("***** cecsend failed %d *****\n", res);
		close(cecsendfd);
		goto cecsend_err;
	}

	close(cecsendfd);

	LOGHDMILIB("%s end", __func__);
	return 0;

cecsend_err:
	cecsenderr();
	LOGHDMILIB("%s end", __func__);
	return -1;
}

/* Read received CEC message and forward on client socket */
int cecrx(void)
{
	int cecreadfd;
	__u8 buf[32];
	__u8 cecdata[32];
	int cecsize;
	int cnt;
	int val;
	int res = 0;
	__u32 cmd_id;

	LOGHDMILIB("%s begin", __func__);

	cmd_id = get_new_cmd_id_ind();
	cecreadfd = open(CECREAD_FILE, O_RDONLY);
	if (cecreadfd < 0) {
		LOGHDMILIBE("***** Failed to open %s *****", CECREAD_FILE);
		return -1;
	}
	cecsize = read(cecreadfd, buf, sizeof(buf));
	close(cecreadfd);

	if (cecsize < 0)
		return -1;

	for (cnt = 0; cnt < cecsize; cnt++)
		LOGHDMILIB2("cecrx[%d]:%x", cnt, buf[cnt]);

	val = HDMI_CECRECVD;
	memcpy(&buf[CMD_OFFSET], &val, 4);
	memcpy(&buf[CMDID_OFFSET], &cmd_id, 4);
	val = cecsize;
	memcpy(&buf[CMDLEN_OFFSET], &val, 4);
	memcpy(&buf[CMDBUF_OFFSET], cecdata, val);

	/* Send on socket */
	res = clientsocket_send(buf, CMDBUF_OFFSET + val);

	LOGHDMILIB("%s end", __func__);
	return res;
}
