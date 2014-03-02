#define DEBUG 1
/*
 * battery_params.c
 * Copyright (c) 2011, ST-Ericsson SA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: 2011 Johan Palsson <johan.palsson@stericsson.com>
 * Author: 2011 Kalle Komierowski <karl.komierowski@stericsson.com>
 *
 * DESCRIPTION:
 * Stores and reads the battery maximum capacity value into cspsa
 */

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <poll.h>

#define LOG_TAG "battery_params"
#include "cutils/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "cspsa.h"

#define D_CSPSA_DEFAULT_NAME "CSPSA0"
#define CSPSA_KEY_CHARGE_FULL 0x00010300
#define CSPSA_KEY_CHARGE_NOW 0x00010305
#define BUF_SIZE 8

static const char CHARGE_FULL[] = "/sys/battery/charge_full";
static const char CHARGE_NOW[] = "/sys/battery/charge_now";
#define C_FULL 0
#define C_NOW 1
#define CAP_CHANGE_PERCENT	3

#define GOTO_ON_ERROR(x, gto)					\
	do {							\
		if (x < 0) {					\
			ALOGE("Error on line: %d\n", __LINE__);	\
			result = -EINVAL;			\
			goto gto;				\
		}						\
	} while (0)

typedef struct {
	CSPSA_Handle_t	handle;
	CSPSA_Key_t	key;
	CSPSA_Size_t	size;
	int		data;
	int		last_saved;
} CSPSA_params;

int write_to_file(int fd, int data)
{
	int res;
	char buf_out[BUF_SIZE];

	res = snprintf(buf_out, BUF_SIZE, "%d\n", data);
	if (res < 0) {
		ALOGD("Can't write to buffer data: %d, buf: %02x", data, buf_out[0]);
		return res;
	}

	res = write(fd, buf_out, sizeof(buf_out));
	if (res < 0) {
		ALOGD("Can't write; error: %d", res);
		return res;
	}
	return 0;
}

static int Write_CSPSA(CSPSA_params *CSPSA)
{
	unsigned int i;
	int tries, ret = 0;
	CSPSA_Data_t *cspsa_data;

	cspsa_data = (CSPSA_Data_t *) malloc(CSPSA->size);
	ALOGD("[%x] handle: %x key: %ux, size: %ud, data %d\n", (unsigned int)CSPSA, (unsigned int) CSPSA->handle, CSPSA->key, CSPSA->size, CSPSA->data);
	for (i = 0; i < CSPSA->size; i++)
			cspsa_data[i] = (CSPSA_Data_t)(0xFF &
			((CSPSA->data & (0xFF << (i * 8)))) >> (i * 8));

	tries = 10;
	while (CSPSA_WriteValue(CSPSA->handle, CSPSA->key, CSPSA->size, cspsa_data) !=
		T_CSPSA_RESULT_OK && --tries);

	if (!tries) {
		ALOGE("Error writing CSPSA");
		ret = -1;
		goto err;
	}

	while (CSPSA_Flush(CSPSA->handle) != T_CSPSA_RESULT_OK
		&& --tries);

	if (!tries) {
		ALOGE("Error flushing CSPSA");
		ret = -1;
		goto err;
	}

err:
	if (cspsa_data)
		free(cspsa_data);

	return ret;
}

static int Read_CSPSA(CSPSA_params *CSPSA)
{
	CSPSA_Result_t result;
	CSPSA_Data_t *cspsa_data = NULL;
	unsigned int i;
	int ret = 0;

	result = CSPSA_Open("CSPSA0", &CSPSA->handle);
	if (result != T_CSPSA_RESULT_OK) {
		ALOGE("Can't open CSPSA area (result 0x%X) ", result);
		ret = -1;
		goto cspsa_finished1;
	}

	result = CSPSA_GetSizeOfValue(CSPSA->handle, CSPSA->key, &CSPSA->size);
	if (result != T_CSPSA_RESULT_OK) {
		ALOGE("Can't get size of key (h %p key 0x%x result 0x%X).",
				CSPSA->handle, CSPSA->key, result);
		ret = -1;
		goto cspsa_finished1;
	}

	cspsa_data = (CSPSA_Data_t *) malloc(CSPSA->size);
	if (!cspsa_data) {
		ALOGE("Can't malloc %d bytes.", CSPSA->size);
		ret = -1;
		goto cspsa_finished1;
	}

	result = CSPSA_ReadValue(CSPSA->handle, CSPSA->key, CSPSA->size, cspsa_data);
	if (result != T_CSPSA_RESULT_OK) {
		ALOGE("Can't read from CSPSA (h %p  key 0x%x size %d res 0x%X).",
				CSPSA->handle, CSPSA->key, CSPSA->size, result);
		ret = -1;
		goto cspsa_finished1;
	}

	ALOGD("CSPSA (h %p  key 0x%x size %d res 0x%X).",
		CSPSA->handle, CSPSA->key, CSPSA->size, result);

	CSPSA->data = 0;

	for (i = 0; i < CSPSA->size; i++)
		CSPSA->data += (cspsa_data[i] << (i * 8));

cspsa_finished1:
	if (cspsa_data)
		free(cspsa_data);

	return ret;
}

int main(int argc, char *argv[])
{
	CSPSA_params now, full;
	CSPSA_Result_t result;
	int fd_full = 0, fd_now = 0, nread, param, loop = 1;
	int res;
	char buf[BUF_SIZE];
	struct pollfd pfd[2];

	now.handle = 0;
	full.handle = 0;
	/* CHARGE_FULL */
	full.key = CSPSA_KEY_CHARGE_FULL;

	res = Read_CSPSA(&full);
	GOTO_ON_ERROR(res, close_file);

	full.last_saved = full.data;
	ALOGD("Got key %d, data: %d size: %d\n", full.key, full.data, full.size);

	fd_full = open(CHARGE_FULL, O_RDONLY);
	GOTO_ON_ERROR(fd_full, close_file);

	if (full.data > 0) {
		res = write_to_file(fd_full, full.data);
		GOTO_ON_ERROR(res, close_file);
	}

	/* CHARGE_NOW */
	now.key = CSPSA_KEY_CHARGE_NOW;

	res = Read_CSPSA(&now);
	GOTO_ON_ERROR(res, close_file);

	now.last_saved = now.data;
	ALOGD("Got key %d, data: %d size: %d\n", now.key, now.data, now.size);

	fd_now = open(CHARGE_NOW, O_RDONLY);
	GOTO_ON_ERROR(fd_now, close_file);

	if (now.data > 0) {
		res = write_to_file(fd_now, now.data);
		GOTO_ON_ERROR(res, close_file);
	} else {
		ALOGD("Saved capacity = %d, not updating...\n", now.data);
	}

	GOTO_ON_ERROR(lseek(fd_now, 0, SEEK_SET), close_file);
	GOTO_ON_ERROR(lseek(fd_full, 0, SEEK_SET), close_file);

	pfd[C_FULL].fd = fd_full;
	pfd[C_FULL].events = POLLERR | POLLPRI;

	pfd[C_NOW].fd = fd_now;
	pfd[C_NOW].events = POLLERR | POLLPRI;

	while (loop) {
		res = poll(pfd, 2, -1);

		switch (res) {
		case -1:
		case 0: /* Timeout*/
			break;
		default:
			/* Capacity now */
			if (pfd[C_NOW].revents) {
				int cap_change = 0;
				bool force_save = false;
				int cap_percent = 0;

				ALOGD("Capacity_NOW changed\n");

				GOTO_ON_ERROR(lseek(fd_now, 0, SEEK_SET), close_file);

				nread = read(fd_now, buf, sizeof(buf));
				if (nread < 0) {
					ALOGE("Error reading \"%s\": %s",
						CHARGE_NOW, strerror(nread));
					result = -EINVAL;
					goto close_file;
				}
				pfd[C_NOW].fd = fd_now;
				now.data = atoi(buf);

				/*
				 * Require a capacity change of more than
				 * 3% of full charge to save it...
				 */
				if (full.data != 0) {
					cap_change = (abs(now.data - now.last_saved) * 100) / full.data;
					cap_percent = ((now.data * 100) / full.data);
				/* Following if needed when closing in to 100 % of charging
				 * make sure saving every time since we might miss one out
				 */
					if (cap_percent >= 100-CAP_CHANGE_PERCENT)
						force_save = true;
				} else {
					force_save = true;
				}

				ALOGD("Cap change %d%% now: %d last: %d, bat cap: %d%% force save: %d\n",
					cap_change, now.data, now.last_saved, cap_percent, force_save);

				 if (cap_change >= CAP_CHANGE_PERCENT || force_save) {
					ALOGD("Read from sys (double) %d", now.data);
					if (now.data) {
						if (Write_CSPSA(&now))
							goto cspsa_finished;
						now.last_saved = now.data;
					}
				} else
					ALOGD("Too small change in capacity,"
						" not saving... %d", now.data);
			}

			/* Capacity full */
			if (pfd[C_FULL].revents) {
				ALOGD("Capacity_FULL changed\n");

				GOTO_ON_ERROR(lseek(fd_now, 0, SEEK_SET), close_file);

				nread = read(fd_full, buf, sizeof(buf));
				if (nread < 0) {
					ALOGE("Error reading \"%s\": %s",
						CHARGE_FULL, strerror(nread));
					result = -EINVAL;
					goto close_file;
				}
				pfd[C_FULL].fd = fd_full;
				param = atoi(buf);
				ALOGD("Read from sys (double) %d", param);

				/* Here we write the updated value to CSPSA */
				if (param != full.last_saved) {
					full.data = param;
					if (Write_CSPSA(&full))
						goto cspsa_finished;
					full.last_saved = param;

				}
			}
		}
	}

	result = 0;

cspsa_finished:
close_file:
	if (fd_full)
		close(fd_full);
	if (fd_now)
		close(fd_now);

	if (full.handle) {
		result = CSPSA_Close(&full.handle);
		if (result != T_CSPSA_RESULT_OK)
			ALOGE("Can't close CSPSA area (result %x) ", result);
	}
	if (now.handle) {
		result = CSPSA_Close(&now.handle);
		if (result != T_CSPSA_RESULT_OK)
			ALOGE("Can't close CSPSA area (result %x) ", result);
	}
	return result;
}
