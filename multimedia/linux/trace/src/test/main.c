/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* This is a simple dummy test that prints some recognizable data on the XTI channel.
   It is used to test the driver */

#include <stdio.h>
#include <stm-trace.h>
#include <unistd.h>

//arm-linux-gcc -march=armv7-a -mtune=cortex-a8 -pthread -fpic -Wall -g -Werror -fvisibility=hidden -D__STN_8500=20 -D__CUT_20 -DLINUX -D__ARM_LINUX -D__STN_4500=20 -I ${MMROOT}/linux/trace/api main.c -o ${MMROOT}/linux/ramdisk/${PLATFORM}_rootfs_dir/main -L${MMROOT}/linux/trace/lib/${PLATFORM} -Wl,-Bstatic -ltrace -Wl,-Bdynamic

int main(void)
{
	int i, mark=0;
	if ((i=stm_trace_open())) {
		printf("Can't open stm device: %d\n", i);
		return -1;
	}

	for (i=0; i<16; i++) {
		stm_trace_32(1, mark);
		stm_tracet_32(2, mark);
		mark += 0x11111111;
		printf("mark %d printed\n", i);
	}
	stm_trace_close();

	mark=0xDEEDBEAF;
	for (i=0; i<16; i++) {
		stm_trace_buffer( sizeof(mark), (char *)&mark);
		printf("mark %d printed\n", i);
	}
		
	return 0;
}
