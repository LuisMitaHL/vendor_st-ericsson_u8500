/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 * write_sim.c
 */
#define SIM_RESET_STR   1
#define SIM_CLOSE       2
#define SIM_CREAT       3
#define SIM_IOCTL       4
#define SIM_OPEN        5
#define SIM_READ        6
#define SIM_WRITE       7
#define SIM_ACCESS      8
#define SIM_LSEEK       9
#define SIM_RESET_BUF   10
#define SIM_UNLINK      11
#define SIM_EXIT        12
#define SIM_MEM_CFG     13
 
/*
 * addresses of stdio registers of the debug module
 */
#define FILE_COMMAND_CODE 0xff
#define OPEN_CODE       0x01
#define CLOSE_CODE      0x02
#define READ_CODE       0x04
#define WRITE_CODE      0x08
#define LSEEK_CODE      0x10
#define UNLINK_CODE     0x20
#include <stwdsp.h>

int write_sim( int fildes, char *buf, unsigned int nbyte )
{
	int res=0;
	int i;

	//debug_stdio_int0 = fildes;
  *(volatile MMIO*)0xF510U = fildes;

	//debug_stdio_cmd = SIM_RESET_BUF;
  *(MMIO*)0xF50EU = SIM_RESET_BUF;

	for(i=0; i<nbyte; i++)
	{
		//debug_stdio_buf = *buf++;
		*( volatile MMIO*)0xF519U = *buf++;
	}

	//debug_stdio_int1 = nbyte;
  *(MMIO*)0xF511U = nbyte;

	//debug_stdio_cmd = SIM_WRITE;
   *(MMIO*)0xF50EU = SIM_WRITE;

	//res =debug_stdio_res;

	//errno = debug_stdio_errno;

	return res;
}

