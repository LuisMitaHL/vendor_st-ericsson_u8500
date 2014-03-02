/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef READPE_IOCTL_H
#define READPE_IOCTL_H
#include <linux/ioctl.h>

#define READHWREG_NAME "readhwreg_camera"

// Status of last command
#define READHWREG_STATUS_OK	0x0
#define READHWREG_STATUS_FAILED	0x1


/* Message structure between client and server
*/
typedef struct
{
	unsigned int u32_address;
	unsigned int u32_data;
	unsigned int u32_dataSize;
	unsigned char u8_access;
	unsigned char u8_status;
	unsigned char u8_type;
}sMsgBuf_t;

#define READHWREG_TRUE	(1)
#define READHWREG_FALSE	(0)
#define READHWREG_INVALID (~0)

/*Xshutdown from host takes two arguments*/
#define READHWREG_MAGIC_NUMBER 'k'
#define READHWREG_CAM_READREG _IOWR(READHWREG_MAGIC_NUMBER, 1, sMsgBuf_t *)
#define READHWREG_CAM_WRITEREG _IOWR(READHWREG_MAGIC_NUMBER, 2, sMsgBuf_t *)
#define READHWREG_CAM_READPE _IOWR(READHWREG_MAGIC_NUMBER, 3, sMsgBuf_t *)
#define READHWREG_CAM_WRITEPE _IOWR(READHWREG_MAGIC_NUMBER, 4, sMsgBuf_t *)

#endif
