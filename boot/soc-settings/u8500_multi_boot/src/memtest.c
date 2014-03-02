/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#include <cpu.h>
#include <memtest.h>
#include <log.h>
#include <config.h>

/**
 * Function: memtest_fill()
 *
 * Helper function to first fill memory with pattern
 *
 * @param address Start address of memory
 * @param sizes Size of memory
 * @param fillpattern Pattern to fill with
 */
static void memtest_fill(
	volatile datum_t *address,
	unsigned long nbytes,
	u32 fillpattern)
{
	unsigned long offset;
	unsigned long nwords = nbytes / sizeof(datum_t);

	logdebug("memtest_fill() start. address=%x nbytes=%x fillpattern=%x",
		address, nbytes, fillpattern);

	/* Fill memory with a known pattern.*/
	for (offset = 0; offset < nwords; offset++)
		address[offset] = fillpattern;

	logdebug("memtest_fill() end.");
}

/**
 * Function: memtest_checkfill()
 *
 * Helper function to check for 'checkpattern' and then refill with
 * 'fillpattern' in the same pass.
 *
 * @param address Start address of memory
 * @param sizes Size of memory
 * @param checkpattern Pattern to check again.
 * @param fillpattern Pattern to fill with after check.
 *
 * @return NULL on success, memory address on failure
 */
static datum_t *memtest_checkfill(
	volatile datum_t *address,
	unsigned long nbytes,
	u32 checkpattern,
	u32 fillpattern)
{
	unsigned long offset;
	unsigned long nwords = nbytes / sizeof(datum_t);

	logdebug("memtest_checkfill() start. "
		"address=%x nbytes=%x checkpattern=%x fillpattern=%x",
		address, nbytes, checkpattern, fillpattern);

	/* Check each location and fill it in the same pass.*/
	for (offset = 0; offset < nwords; offset++) {
		datum_t val;

		val = address[offset];
		if (val != checkpattern) {
			logmemerr("Check/Fill memory error.",
				&address[offset], val, checkpattern);
			return (datum_t *) &address[offset];
		}
		address[offset] = fillpattern;
	}
	logdebug("memtest_checkfill() success!");
	return NULL;
}

/**
 * Function: memtest_check()
 *
 * Helper function to check for pattern.
 *
 * @param address Start address of memory
 * @param sizes Size of memory
 * @param pattern Pattern to check.
 *
 * @return NULL on success, memory address on failure
 */
static datum_t *memtest_check(
	volatile datum_t *address,
	unsigned long nbytes,
	u32 checkpattern)
{
	unsigned long offset;
	unsigned long nwords = nbytes / sizeof(datum_t);

	logdebug("memtest_check() start. "
		"address=%x nbytes=%x checkpattern=%x",
		address, nbytes, checkpattern);

	/* Check each location and fill it in the same pass.*/
	for (offset = 0; offset < nwords; offset++) {
		datum_t val;

		val = address[offset];
		if (val != checkpattern) {
			logmemerr("Check/Fill memory error.",
				&address[offset], val, checkpattern);
			return (datum_t *) &address[offset];
		}
	}
	logdebug("memtest_check() success!");
	return NULL;
}

/**
 * Function: memtest_flipbits()
 *
 * A test that will in two passes try to flip as many bits as possible in an
 * attempt to draw as much power from the power supply as possible.
 * Exposing any power issues.
 *
 * @param address Start address of memory
 * @param sizes Size of memory
 *
 * @return 0 on success, 1 on failure
 */
static datum_t *memtest_flipbits(
	volatile datum_t *address,
	unsigned long nbytes)
{
	static const u32 patterns[] = {
		0x00000000,
		0xFFFFFFFF,
		0x00000000,
		0xFFFFFFFF,
		0xAAAAAAAA,
		0x55555555,
		0xAAAAAAAA,
		0x55555555
	};

	datum_t *res;
	unsigned i;

	logdebug("memtest_flipbits() start. address=%x nbytes=%x",
		address, nbytes);

	/* Start setting memory to first pattern*/
	memtest_fill(address, nbytes, patterns[0]);

	/* Flip bits for all patterns */
	for (i = 0; i < ARRAY_SIZE(patterns) - 1; i++) {
		res = memtest_checkfill(address, nbytes,
			patterns[i], patterns[i + 1]);
		if (res)
			return res;
	}

	/* Check the last pattern */
	res = memtest_check(address, nbytes,
		patterns[ARRAY_SIZE(patterns) - 1]);
	if (res)
		return res;

	logdebug("memtest_flipbits() success!");
	return NULL;
}


/** memtest_fast
 *
 * Will test the memory fast.
 *
 * @param base Start address of memory
 * @param sizes Size of memory
 *
 * @return 0 on success, 1 on databus failure, 2 on addressbus failure
 */
static int memtest_fast(
	volatile datum_t *base,
	unsigned long size)
{
	logdebug("memtest_fast() start.");

	if (memtest_databus(base))
		return 1; /* databus failed */

	if (memtest_addressbus(base, size) != NULL)
		return 2; /* addressbus failed */

	logdebug("memtest_fast() success!");
	return 0; /* success */
}

/** memtest_full
 *
 * Same as memtest_fast, only it will also run the time consuming
 * memtest_device test.
 *
 * @param base Start address of memory
 * @param sizes Size of memory
 *
 * @return 0 on success
 *  1 on databus failure
 *  2 on addressbus failure,
 *  3 on memory device error
 *  4 on flip bits error
 */
static int memtest_full(
	volatile datum_t *base,
	unsigned long size)
{
	int res;

	logdebug("memtest_full() start.");

	/* First run the fast test, need in order to trust data and
	 * address buses
	 */
	res = memtest_fast(base, size);
	if (res != 0)
		return res;

	if (memtest_device(base, size) != NULL)
		return 3; /* Memory device error */


	if (memtest_flipbits(base, size) != NULL)
		return 4; /* Flip bits error */

	logdebug("memtest_full() success!");
	return 0; /* success */
}

/* Test DDR data access */
static int memtest_ddr_data_access(void)
{
	u32 address;
	u32 i;
	u32 data;
	u32 data_written[33];
	u8 byte1;
	u8 byte2;

	logdebug("memtest_ddr_data_access() start");

	/*
	 * write in RAM to test address wires [0:12] and data wires [0:27]
	 * adress (0x1) to (0x8000000) (=> some address wires will be tested
	 * twice, doesn't matter)
	 * data 0x1 to 0x8000000
	 */
	address = 1;
	data = 1;
	for (i = 0; i < 2; i++) {
		data_written[i] = data;
		*((u8 *)(address)) = (u8)(data);

		address = 1<<(i+1);
		data = data<<1;
	}

	/*
	 * write in RAM DDR 32bit data
	 *adress 0x4 to 0x8000000
	 *data   0x4 to 0x8000000
	 */
	for (i = 2; i < 28; i++) {
		data_written[i] = data;
		*((u32 *)(address)) = (u32)(data);

		address = 1<<(i+1);
		data = data<<1;
	}

	/*
	 * write in RAM to test data wires [28:31]
	 * address 0x800 + 4*n*(1 word)
	 * data: 0x10000000 to 0x80000000 =>
	 *   all 32 data wires will be then tested!
	 */
	address = 0x800;
	for (i = 1; i <= 4; i++) {
		/*
		 * write in address 0x804 data 0x10000000
		 * then write in address 0x808 data 0x20000000
		 * then write in address 0x80C data 0x40000000
		 * then write in address 0x810 data 0x80000000
		 */
		data_written[i+27] = 1<<(i+27);
		*((u32 *)(address+(4*i))) = 1<<(i+27);
	}

	/*
	 * write in RAM DDR - to check bit 0 and 1 of data bus
	 * address 0x814
	 * data 0x3
	 */
	address = 0x814;
	data = 0x3;
	data_written[32] = data;
	*((u32 *)(address)) = data;

	/*
	 * now check the written data by the way of a readback
	 * same loops as the both above.
	 */

	/* test bit 0 and bit 1 of address bus */
	address = 0;

	byte1 = ((*(u32 *)(address)) & (0x00FF00)) >> 8;
	if (byte1 != (u8)(data_written[0])) {
		logmemerr("memtest_ddr_data_access() spot1 failed.",
			address, byte1, (u8)data_written[0]);
		return 1;
	}

	byte2 = ((*(u32 *)(address)) & (0xFF0000)) >> 16;
	if (byte2 != (u8)(data_written[1])) {
		logmemerr("memtest_ddr_data_access() spot2 failed.",
			address, byte2, (u8)data_written[1]);
		return 2;
	}

	/*set address for next reading back*/
	address = 0x4;

	for (i = 2; i < 28; i++) {
		data = *((u32 *)(address));
		if (data != (u32)(data_written[i])) {
			logmemerr("memtest_ddr_data_access() spot3 failed.",
				address, data, data_written[i]);
			return 3;
		}
		address = 1<<(i+1);
	}

	address = 0x800;
	for (i = 1; i <= 4; i++) {
		data = *((u32 *)(address+(4*i)));
		if (data != data_written[i+27]) {
			logmemerr("memtest_ddr_data_access() spot4 failed.",
				address, data, data_written[i+27]);
			return 4;
		}
	}
	/* test bit 0 and bit 1 of data bus */
	address = 0x814;
	data = *((u32 *)address);
	if (data != data_written[32]) {
		logmemerr("memtest_ddr_data_access() spot5 failed.",
			address, data, data_written[i+27]);
		return 5;
	}

	logdebug("memtest_ddr_data_access() ended successfully.");
	return 0;
}

/** memtest_main
 *
 * Main memory tester function. Will select the configured memtest method
 * and return its result. It will test the full memory from start address 0x0 to
 * 'memsize'.
 *
 * @param memsize Size of full memory.
 *
 * @return 0 on success, otherwise error code of selected test.
 */
int memtest_main(volatile datum_t *base, unsigned long memsize)
{
	struct config *config = get_config();

	switch (config->memtest_method) {
	case MEMTEST_METHOD_OLD:
		return memtest_ddr_data_access();

	case MEMTEST_METHOD_FAST:
		return memtest_fast(base, memsize);

	case MEMTEST_METHOD_FULL:
		logwarn("Running FULL memtest. This will take a long time "
			"If you are flashing, "
			"flashing will timeout with an error.");
		return memtest_full(base, memsize);
	default:
		logwarn("Unknown MEMTEST method!");
		return 1;
	}

	/* Will never be reached, but keeps compiler from complaining */
	return 1; /* Any error */
}
