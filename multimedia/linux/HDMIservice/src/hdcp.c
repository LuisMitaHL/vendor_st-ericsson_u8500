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
#include <sys/ioctl.h>
#include <ctype.h>
#ifdef ANDROID
#include <utils/Log.h>
#endif
#include "../include/hdmi_service_api.h"
#include "../include/hdmi_service_local.h"

const __u8 hdcp_auth_start_val[] = {0x01, 0x00}; /* Start authentication */
const __u8 hdcp_encr_start_val[] = {0x03, 0x01}; /* Start encryption */
const __u8 hdcp_encr_stop_val[] = {0x00, 0x00}; /* Stop encryption */
const __u8 hdcp_even_val[] = {0x01}; /* Enable HDCP events */

static char *dbg_otp(int value)
{
	switch (value) {
	case OTP_UNPROGGED:
		return "OTP IS NOT PROGRAMMED";
		break;
	case OTP_PROGGED:
		return "OTP IS PROGRAMMED";
		break;
	default:
		return "OTP status UNKNOWN";
		break;
	}
}

static char *dbg_loadaes(int value)
{
	switch (value) {
	case LOADAES_OK:
		return "LOAD AES OK\n";
		break;
	case LOADAES_NOT_OK:
		return "LOAD AES FAILED\n";
		break;
	case LOADAES_NOT_FUSED:
		return "LOAD AES FAILED NOT FUSED\n";
		break;
	case LOADAES_CRC_MISMATCH:
		return "LOAD AES FAILED CRC MISMATCH\n";
		break;
	default:
		return "LOAD AES result UNKNOWN\n";
		break;
	}
}

static char *dbg_hdcpstate(int value)
{
	switch (value) {
	case HDCP_STATE_NO_RECV:
		return "HDCP STATE NO_RECV";
		break;
	case HDCP_STATE_RECV_CONN:
		return "HDCP STATE RECV_CONN";
		break;
	case HDCP_STATE_NO_HDCP:
		return "HDCP STATE NO_HDCP";
		break;
	case HDCP_STATE_NO_ENCR:
		return "HDCP STATE NO_ENCR";
		break;
	case HDCP_STATE_AUTH_ONGOING:
		return "HDCP STATE AUTH_ONGOING";
		break;
	case HDCP_STATE_AUTH_FAIL:
		return "HDCP STATE AUTH_FAIL";
		break;
	case HDCP_STATE_AUTH_SUCCEDED:
		return "HDCP STATE AUTH_SUCCEDED";
		break;
	case HDCP_STATE_ENCR_ONGOING:
		return "HDCP STATE ENCR_ONGOING";
		break;
	default:
		return "HDCP STATE UNKNOWN";
		break;
	}
}

/* Load aes keys and start hdcp encryption */
int hdcp_init(__u8 *aes)
{
	int hdcpchkaesotp;
	int hdcploadaes;
	int hdcpauthencr;
	int hdcpeven;
	int res;
	int value = 0;
	char buf[128];
	int result = HDCP_OK;

	/* Check if OTP is fused */
	hdcpchkaesotp = open(HDCPCHKAESOTP_FILE, O_RDONLY);
	if (hdcpchkaesotp < 0) {
		LOGHDMILIBE("***** Failed to open %s *****",
							HDCPCHKAESOTP_FILE);
		result = SYSFS_FILE_FAILED;
		goto hdcp_end;
	}
	res = read(hdcpchkaesotp, buf, sizeof(buf));
	close(hdcpchkaesotp);
	if (res != 1) {
		LOGHDMILIBE("***** %s read error *****", HDCPCHKAESOTP_FILE);
		result = SYSFS_FILE_FAILED;
		goto hdcp_end;
	}
	value = *buf;
	LOGHDMILIB("%s", dbg_otp(value));

	if (value == OTP_PROGGED) {
		/* Subscribe for hdcp events */
		hdcpeven = open(HDCPEVEN_FILE, O_WRONLY);
		if (hdcpeven < 0) {
			LOGHDMILIBE("***** Failed to open %s *****",
					HDCPEVEN_FILE);
			result = SYSFS_FILE_FAILED;
			goto hdcp_end;
		}
		res = write(hdcpeven, hdcp_even_val, sizeof(hdcp_even_val));
		close(hdcpeven);
		if (res != sizeof(hdcp_even_val)) {
			result = SYSFS_FILE_FAILED;
			goto hdcp_end;
		}

		/* Write aes keys */
		hdcploadaes = open(HDCPLOADAES_FILE, O_WRONLY);
		if (hdcploadaes < 0) {
			LOGHDMILIBE("***** Failed to open %s *****",
					HDCPLOADAES_FILE);
			result = SYSFS_FILE_FAILED;
			goto hdcp_end;
		}
		res = write(hdcploadaes, aes, AES_KEYS_SIZE);
		close(hdcploadaes);
		if (res != AES_KEYS_SIZE) {
			LOGHDMILIBE("***** Failed to write hdcploadaes %d "
					"*****", res);
			result = SYSFS_FILE_FAILED;
			goto hdcp_end;
		}

		usleep(LOADAES_WAITTIME);

		/* Check result */
		hdcploadaes = open(HDCPLOADAES_FILE, O_RDONLY);
		if (hdcploadaes < 0) {
			LOGHDMILIBE("***** Failed to open %s *****",
					HDCPLOADAES_FILE);
			result = SYSFS_FILE_FAILED;
			goto hdcp_end;
		}
		res = read(hdcploadaes, buf, sizeof(buf));
		close(hdcploadaes);
		if (res != 1) {
			LOGHDMILIBE("***** %s read error *****",
						HDCPLOADAES_FILE);
			result = SYSFS_FILE_FAILED;
			goto hdcp_end;
		}
		value = *buf;
		LOGHDMILIB("%s", dbg_loadaes(value));
		if (value == LOADAES_OK) {
			LOGHDMILIB("%s", "--- LOAD AES keys OK ---");
		} else {
			result = AESKEYS_FAIL;
			goto hdcp_end;
		}

		usleep(LOADAES_WAITTIME);

		/* Start HDCP encryption */
		hdcpauthencr = open(HDCPAUTH_FILE, O_WRONLY);
		if (hdcpauthencr < 0) {
			LOGHDMILIBE("***** Failed to open %s *****",
					HDCPAUTH_FILE);
			result = HDCPAUTHENCR_FAIL;
			goto hdcp_end;
		}
		res = write(hdcpauthencr, hdcp_encr_start_val,
				sizeof(hdcp_encr_start_val));
		close(hdcpauthencr);
		if (res != sizeof(hdcp_encr_start_val)) {
			LOGHDMILIBE("***** Failed to write hdcpauthencr %d "
					"*****", res);
			result = HDCPAUTHENCR_FAIL;
			goto hdcp_end;
		}
		usleep(HDCPAUTH_WAITTIME);

	} else {
		printf("***** Missing aes file or HDCP AES OTP is not fused."
				" *****\n");
	}

hdcp_end:
	return result;
}

/* Get current hdcp state */
int hdcp_state(void)
{
	int hdcpstateget;
	int result = HDCP_OK;
	int res;
	__u8 buf[128];
	int val;
	__u32 cmd_id;

	cmd_id = get_new_cmd_id_ind();

	/* Check hdcpstate */
	hdcpstateget = open(HDCPSTATEGET_FILE, O_RDONLY);
	if (hdcpstateget < 0) {
		LOGHDMILIBE("***** Failed to open %s *****",
				HDCPSTATEGET_FILE);
		result = SYSFS_FILE_FAILED;
		goto hdcp_state_end;
	}
	res = read(hdcpstateget, buf, sizeof(buf));
	close(hdcpstateget);
	if (res != 1) {
		LOGHDMILIBE("***** %s read error *****",
				HDCPSTATEGET_FILE);
		result = HDCPSTATE_FAIL;
		goto hdcp_state_end;
	}

	val = HDMI_HDCPSTATE;
	memcpy(&buf[CMD_OFFSET], &val, 4);
	memcpy(&buf[CMDID_OFFSET], &cmd_id, 4);
	val = 1;
	memcpy(&buf[CMDLEN_OFFSET], &val, 4);
	memcpy(&buf[CMDBUF_OFFSET], buf, val);

	/* Send on socket */
	if (clientsocket_send(buf, CMDBUF_OFFSET + val) != 0)
		result = HDCPSTATE_FAIL;

	LOGHDMILIB("%s", dbg_hdcpstate(val));

hdcp_state_end:
	return result;
}
