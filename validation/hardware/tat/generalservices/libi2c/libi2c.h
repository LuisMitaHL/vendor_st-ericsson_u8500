/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

enum {
	I2C_EVENT_WRITE,
	I2C_EVENT_READ,
	I2C_EVENT_NBMAX
};

int i2c_Open(const char *deviceName, int flags);
int i2c_Close(int fd);
int i2c_Read(int fd, int address, void *buf, int count);
int i2c_Write(int fd, int address, const void *buf, int count);
int i2c_RDWR(unsigned char instance, unsigned char slave_address,\
		unsigned char mode, unsigned int reg_address,\
		unsigned char count, char *buf);

