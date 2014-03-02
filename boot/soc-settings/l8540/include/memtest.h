/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __MEMTEST_H__
#define __MEMTEST_H__

#include <memtest_pd.h>

struct secr_intlv {
	unsigned int en : 1;
	unsigned int page_size : 5;
	unsigned int end_addr : 6;
	unsigned int start_addr : 6;
	/* padding up to 32 bits */
	unsigned int : 14;
};

/** memtest
 *
 * Main memory tester function. Will select the configured memtest method
 * and return its result. It will test the full memory from start
 * address 'base' to 'base + memsize'.
 *
 * @param base Start address of memory.
 * @param memsize Size of full memory, in bytes.
 *
 * @return 0 on success, otherwise error code of selected test.
 */
int memtest_main(
	volatile datum_t *base,
	unsigned long memsize);

int intlvtest(u32 intlv);

void memcpy(u8 *dest, u8* src, unsigned length);

#endif /* __MEMTEST_H__ */
