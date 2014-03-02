/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#include "alsactrl_debug.h"

#include <sys/ioctl.h>
#include <time.h>               /* used for generating random numbers */
#include <termios.h>
#include <errno.h>
#include <fcntl.h>              /*low-level i/o */
#include <linux/videodev2.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

#include <linux/mfd/cg2900_audio.h>

/* Params and functions for audio driver*/

#define CHAR_DEV_OP_CODE_SET_DAI_CONF			0x01
#define CHAR_DEV_OP_CODE_GET_DAI_CONF			0x02
#define CHAR_DEV_OP_CODE_CONFIGURE_ENDPOINT		0x03
#define CHAR_DEV_OP_CODE_CONNECT_AND_START_STREAM	0x04
#define CHAR_DEV_OP_CODE_STOP_STREAM			0x05

#define CG2900_AUDIO_DEVICE_NAME			    "cg2900_audio"

#define FM_RX 0
#define FM_TX 1

static int chardev_fd = -1;
static unsigned int stream_handle = 0xffffffff;

static int open_chardev(void);
static void close_chardev(void);
static int setup_i2s_dai(void);
static int setup_fm_endpoint(int rxtx);
static int setup_analog_out_endpoint (void);
static int start_stream(enum cg2900_audio_endpoint_id ep1,
			enum cg2900_audio_endpoint_id ep2);
static void stop_stream(void);



static int open_chardev(void)
{
	LOG_I("Enter.");

	if (chardev_fd >= 0) {
		LOG_I("Char-dev already open!");
		return 0;
	}

	chardev_fd = open("/dev/" CG2900_AUDIO_DEVICE_NAME, O_RDWR);
	if (chardev_fd < 0) {
		LOG_E("ERROR: Failed to open device (%s)!", strerror(errno));
		return -1;
	} else {
		return 0;
	}
}



static void close_chardev(void)
{
	LOG_I("Enter.");

	close(chardev_fd);
	chardev_fd = -1;
}



static int setup_i2s_dai(void)
{
	LOG_I("Enter.");

	struct cg2900_dai_config dai_conf;

	unsigned char *buf;
	size_t len;
	ssize_t r;

	memset(&dai_conf, 0, sizeof(dai_conf));
	dai_conf.port                 = PORT_0_I2S;
	dai_conf.conf.i2s.mode        = DAI_MODE_SLAVE;
	dai_conf.conf.i2s.half_period = HALF_PER_DUR_16;
	dai_conf.conf.i2s.channel_sel = CHANNEL_SELECTION_BOTH;
	dai_conf.conf.i2s.sample_rate = SAMPLE_RATE_48;
	dai_conf.conf.i2s.word_width  = WORD_WIDTH_32;

	len = 4 + sizeof(dai_conf);
	buf = (unsigned char *)malloc(len);
	if (!buf) {
		LOG_E("ERROR: Out of memory!");
		return -1;
	}
	memset(buf, 0, len);
	buf[0] = CHAR_DEV_OP_CODE_SET_DAI_CONF;
	memcpy(buf + 4, &dai_conf, sizeof(dai_conf));

	r = write(chardev_fd, buf, len);
	free(buf);
	if (r != (ssize_t)len) {
		LOG_E("ERROR: Set DAI failed (%s)!\n", (r < 0) ? strerror(errno) : "Internal error");
		return -1;
	} else {
		return 0;
	}
}



static int setup_fm_endpoint(int rxtx)
{
	LOG_I("Enter (%s)", rxtx == FM_TX ? "FMTX" : "FMRX");

	struct cg2900_endpoint_config ep_conf;

	unsigned char *buf;
	size_t len;
	ssize_t r;

	memset(&ep_conf, 0, sizeof(ep_conf));
	if (rxtx == FM_TX)
		ep_conf.endpoint_id = ENDPOINT_FM_TX;
	else
		ep_conf.endpoint_id = ENDPOINT_FM_RX;

	ep_conf.config.fm.sample_rate = ENDPOINT_SAMPLE_RATE_48_KHZ;

	len = 4 + sizeof(ep_conf);
	buf = (unsigned char *)malloc(len);
	if (!buf) {
		LOG_E("ERROR: Out of memory!");
		return -1;
	}
	memset(buf, 0, len);
	buf[0] = CHAR_DEV_OP_CODE_CONFIGURE_ENDPOINT;
	memcpy(buf + 4, &ep_conf, sizeof(ep_conf));

	r = write(chardev_fd, buf, len);
	free(buf);
	if (r != (ssize_t)len) {
		LOG_E("ERROR: Configure endpoint failed (%s)!", (r < 0) ? strerror(errno) : "Internal error");
		return -1;
	} else {
		return 0;
	}
}

static int setup_analog_out_endpoint (void)
{
	LOG_I("Enter");

	struct cg2900_endpoint_config ep_conf;

	unsigned char *buf;
	size_t len;
	ssize_t r;

	memset(&ep_conf, 0, sizeof(ep_conf));
	ep_conf.endpoint_id = ENDPOINT_ANALOG_OUT;

	len = 4 + sizeof(ep_conf);
	buf = (unsigned char *)malloc(len);
	if (!buf) {
		LOG_E("ERROR: Out of memory!");
		return -1;
	}
	memset(buf, 0, len);
	buf[0] = CHAR_DEV_OP_CODE_CONFIGURE_ENDPOINT;
	memcpy(buf + 4, &ep_conf, sizeof(ep_conf));

	r = write(chardev_fd, buf, len);
	free(buf);
	if (r != (ssize_t)len) {
		LOG_E("ERROR: Configure endpoint failed (%s)!", (r < 0) ? strerror(errno) : "Internal error");
		return -1;
	} else {
		return 0;
	}

	return 0;
}

static int start_stream(enum cg2900_audio_endpoint_id ep1,
			enum cg2900_audio_endpoint_id ep2)
{
	LOG_I("Enter.");

	int error = 0;
	unsigned int data[2];
	unsigned char *buf;
	size_t len;
	ssize_t r;

	if (stream_handle != 0xffffffff) {
		LOG_I("Tear down old stream.");
		/* tear down old stream first */
		stop_stream();
	}

	len = 4 + sizeof(ep1) + sizeof(ep2);
	buf = (unsigned char *)malloc(len);
	if (!buf) {
		LOG_E("Error: Out of memory!");
		return -1;
	}
	memset(buf, 0, len);
	buf[0] = CHAR_DEV_OP_CODE_CONNECT_AND_START_STREAM;
	memcpy(buf + 4, &ep1, sizeof(ep1));
	memcpy(buf + 4 + sizeof(ep1), &ep2, sizeof(ep2));

	r = write(chardev_fd, buf, len);
	free(buf);
	if (r != (ssize_t)len) {
		LOG_E("ERROR: Start stream failed (%s)", (r < 0) ? strerror(errno) : "Internal error");
		return -1;
	}

	r = read(chardev_fd, data, sizeof(data));
	if ((unsigned int)r < sizeof(data)) {
		LOG_E("ERROR: Start stream failed (%s)!", (r < 0) ? strerror(errno) : "Internal error");
		error = -1;
	} else {
		stream_handle = data[1];
	}

	return error;
}



static void stop_stream(void)
{
	unsigned char *buf;
	size_t len;
	ssize_t r;

	len = 4 + sizeof(stream_handle);
	buf = (unsigned char *)malloc(len);
	if (!buf) {
		LOG_E("Error: Out of memory!");
		return;
	}
	memset(buf, 0, len);
	buf[0] = CHAR_DEV_OP_CODE_STOP_STREAM;
	memcpy(buf + 4, &stream_handle, sizeof(stream_handle));

	r = write(chardev_fd, buf, len);
	free(buf);
	if (r != (ssize_t)len)
		LOG_E("ERROR: Stop stream failed (%s)!", (r < 0) ? strerror(errno) : "Internal error");
	stream_handle = 0xffffffff;
}



int setup_fm_rx_i2s(void)
{
	LOG_I("Enter.");

	if (open_chardev() != 0)
		return -1;

	if (setup_i2s_dai() != 0)
		return -1;

	if (setup_fm_endpoint(FM_RX) != 0)
		return -1;

	if (start_stream(ENDPOINT_FM_RX,
			 ENDPOINT_PORT_0_I2S) != 0)
		return -1;

	return 0;
}



int setup_i2s_fm_tx(void)
{
	LOG_I("Enter.");

	if (open_chardev() != 0)
		return -1;

	if (setup_i2s_dai() != 0)
		return -1;

	if (setup_fm_endpoint(FM_TX) != 0)
		return -1;

	if (start_stream(ENDPOINT_PORT_0_I2S, ENDPOINT_FM_TX) != 0)
		return -1;

	return 0;
}


int setup_fm_analog_out(void)
{
	LOG_I("Enter.");

	if (open_chardev() != 0)
		return -1;

	if (setup_analog_out_endpoint() != 0)
		return -1;

	return 0;
}

void teardown_fm_i2s(void)
{
	stop_stream();
	close_chardev();
}


