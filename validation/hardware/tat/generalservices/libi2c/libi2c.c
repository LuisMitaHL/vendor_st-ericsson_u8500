/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>		/* close */

#include "libi2c.h"

int i2c_RDWR(unsigned char instance, unsigned char slave_address,
	     unsigned char mode, unsigned int reg_address,
		 unsigned char count, char *buf)
{
	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msg;
	char device_name[20];
	int ret;
	int fd;

	/* Device instance */
	switch (instance) {
	case 0:
		snprintf(device_name, 19, "/dev/i2c-0");
		break;

	case 1:
		snprintf(device_name, 19, "/dev/i2c-1");
		break;

	case 2:
		snprintf(device_name, 19, "/dev/i2c-2");
		break;

	case 3:
		snprintf(device_name, 19, "/dev/i2c-3");
		break;

	default:
		printf("Illegal i2c device !!!\n");
		exit(1);
	}

	/* open the device file */
	fd = open(device_name, O_RDWR);
	if (fd < 0) {
		printf("ERRNO: OPEN %s\n", strerror(errno));
		exit(1);
	}

	data.msgs = &msg;
	data.nmsgs = 1;

	/* Update driver U8500 */
	slave_address = (slave_address & 0xff);
	msg.addr = slave_address;

	if (mode == I2C_EVENT_WRITE) {	/* WRITE */
		msg.flags = 0;	/* 0 for write , 1 for read */
		msg.len = count;
		msg.buf = (unsigned char *)buf;

		ret = ioctl(fd, I2C_RDWR, &data);
		if (ret == -1) {
			printf("errno = %s\n", strerror(errno));
			close(fd);
			exit(1);
		}

	} else {		/* READ */

		unsigned char addr_buf[2];
		unsigned char addr_len = 1;

		addr_buf[0] = reg_address & 0xFF;
		if (reg_address > 0xFF) {
			addr_buf[1] = (unsigned char)((reg_address >> 8) & 0xFF);
			addr_len += 1;
		}

		msg.buf = addr_buf;
		msg.flags = 0;	/* 0 for write , 1 for read */
		msg.len = addr_len;

		ret = ioctl(fd, I2C_RDWR, &data);
		if (ret == -1) {
			printf("errno = %s\n", strerror(errno));
			close(fd);
			exit(1);
		}

		msg.flags = 1;	/* 0 for write , 1 for read */
		msg.len = count;
		msg.buf = (unsigned char *)buf;

		ret = ioctl(fd, I2C_RDWR, &data);
		if (ret == -1) {
			printf("errno = %s\n", strerror(errno));
			close(fd);
			exit(1);
		}
	}
	close(fd);

	return 1;
}

int i2c_Open(const char *deviceName, int flags)
{
	int fd;

	fd = open(deviceName, flags);
	if (fd < 0)
		printf("i2c-lib OPEN: %s\n", strerror(errno));

	return fd;
}

int i2c_Close(int fd)
{
	int ret;

	ret = close(fd);

	if (ret < 0)
		printf("i2c-lib CLOSE: %s\n", strerror(errno));

	return ret;
}

int i2c_Read(int fd, int address, void *buf, int count)
{
	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msg;
	int ret;

	memset(&msg, 0, sizeof(msg));
	memset(&data, 0, sizeof(data));
	ret = 0;

	msg.flags = 1;		/* 0 for write , 1 for read */
	msg.len = count;
	msg.buf = (unsigned char *)buf;
	msg.addr = address;
	data.msgs = &msg;
	data.nmsgs = 1;

	ret = ioctl(fd, I2C_RDWR, &data);
	if (ret < 0)
		printf("i2c-lib READ: %s\n", strerror(errno));

	return ret;
}

int i2c_Write(int fd, int address, const void *buf, int count)
{
	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msg;
	int ret;

	memset(&msg, 0, sizeof(msg));
	memset(&data, 0, sizeof(data));
	ret = 0;

	msg.flags = 0;		/* 0 for write , 1 for read */
	msg.len = count;
	msg.buf = (unsigned char *)buf;
	msg.addr = address;
	data.msgs = &msg;
	data.nmsgs = 1;

	ret = ioctl(fd, I2C_RDWR, &data);
	if (ret < 0)
		printf("i2c-lib WRITE: %s\n", strerror(errno));

	return ret;
}
