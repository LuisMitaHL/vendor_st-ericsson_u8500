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
#include <soc_settings.h>
#include <memtest.h>
#include <log.h>
#include <bass_app.h>

extern bass_return_code set_ddr_interlv_config(u32 interlv_config);

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
	logdebug("memtest_checkfill() successful!");
	return NULL;
}

/**
 * Function: memtest_check()
 *
 * Helper function to check for pattern.
 *
 * @param address Start address of memory
 * @param nbytes Size of memory
 * @param checkpattern Pattern to check.
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
	logdebug("memtest_check() successful!");
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

	logdebug("memtest_flipbits() successful!");
	return NULL;
}

int intlvtest(u32 intlv_config) {
	int ret = 0, i;
	datum_t *res;
	unsigned int nbytes = 1 << (12 + ((intlv_config >>  1) & 0x1F));
	datum_t *offset;
	static const u32 patterns[] = {
		0xAAAAAAAA,
		0x55555555
	};

	logdebug("Starting interleaver test... intlv.en = %u", intlv_config & 0x01);

	logdebug3("intlv.en = %u", intlv_config & 0x01);
	logdebug3("intlv.page_size = %u", (intlv_config >>  1) & 0x1F);
	logdebug3("intlv.start_addr = %x", (intlv_config >>  6) & 0x3F);
	logdebug3("intlv.end_addr = %x", (intlv_config >> 12) & 0x3F);
	/* fill 256kB = 4 * 64kB of DDR0 with pattern */
	memtest_fill((datum_t *)AP9540_DDR0_OFFSET, 4*nbytes, patterns[0]);

	/* fill 256kB = 4 * 64kB of DDR1 with pattern */
	memtest_fill((datum_t *)AP9540_DDR1_OFFSET, 4*nbytes, patterns[1]);

	mmu_enable();
	/* TODO: disable interleaving without changing the config */
	set_ddr_interlv_config(intlv_config & 0xFFFE);
	mmu_disable();

	/* verify pattern is ok */
	for (i = 0; i < 4; i++) {
		logdebug2("verifying pattern: %x", patterns[i % 2]);
		offset = (i % 2) ? (datum_t *) AP9540_DDR1_OFFSET :
			(datum_t *) AP9540_DDR0_OFFSET;
		offset += i * (nbytes >> 2); /* offset is (u32*) */
		res = memtest_check(offset, nbytes, patterns[i % 2]);
		if (res) {
			logerror("intlvtest() failed, memtest_check() failed at 0x%x",
					res);
			break;
		}
	}

	mmu_enable();
	/* TODO: enable interleaving without changing the config */
	set_ddr_interlv_config(intlv_config);
	mmu_disable();

	logdebug("intlvtest() successful!");

	return ret;
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
	logdebug("memtest_fast() start, base = %x", base);

	if (memtest_databus(base))
		return 1; /* databus failed */

	if (memtest_addressbus(base, size) != NULL)
		return 2; /* addressbus failed */

	logdebug("memtest_fast() successful!");
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

	logdebug("memtest_full() successful!");
	return 0; /* success */
}

/* Test DDR data access */
static int memtest_ddr_data_access(unsigned int offset)
{
	u32 address;
	u32 i;
	u32 data;
	u32 data_written[33];
	u8 byte1;
	u8 byte2;

	logdebug("memtest_ddr_data_access() start, offset = %x", offset);

	/*
	 * write in RAM to test address wires [0:12] and data wires [0:27]
	 * address (0x1) to (0x10000000) (=> some address wires will be tested
	 * twice, doesn't matter)
	 * data 0x1 to 0x4
	 */
	address = offset + 1;
	data = 1;
	for (i = 0; i < 2; i++) {
		data_written[i] = data;
		logdebug3("  writing %x at address = %x", data, address);
		*((u8 *)(address)) = (u8)(data);

		address = offset + (1 << (i+1));
		data = data << 1;
	}

	/*
	 * write in RAM DDR 32bit data
	 * address 0x4 to 0x10000000
	 * data   0x4 to 0x10000000
	 */
	for (i = 2; i < 28; i++) {
		data_written[i] = data;
		logdebug3("  writing %x at address = %x", data, address);
		*((u32 *)(address)) = (u32)(data);

		address = offset + (1 << (i+1));
		data = data<<1;
	}

	/*
	 * write in RAM to test data wires [28:31]
	 * address 0x800 + 4*n*(1 word)
	 * data: 0x10000000 to 0x80000000 =>
	 *   all 32 data wires will be then tested!
	 */
	address = offset + 0x800;
	for (i = 1; i <= 4; i++) {
		/*
		 * write in address 0x804 data 0x10000000
		 * then write in address 0x808 data 0x20000000
		 * then write in address 0x80C data 0x40000000
		 * then write in address 0x810 data 0x80000000
		 */
		data_written[i+27] = 1<<(i+27);
		logdebug3("  writing %x at address = %x", data, address);
		*((u32 *)(address+(4*i))) = 1<<(i+27);
	}

	/*
	 * write in RAM DDR - to check bit 0 and 1 of data bus
	 * address 0x814
	 * data 0x3
	 */
	address = offset + 0x814;
	data = 0x3;
	data_written[32] = data;
	logdebug3("  writing %x at address = %x", data, address);
	*((u32 *)(address)) = data;

	/*
	 * now check the written data by means of a readback
	 * same loops as above.
	 */

	/* test bit 0 and bit 1 of address bus */
	address = offset + 0;

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
	address = offset + 0x4;

	for (i = 2; i < 28; i++) {
		data = *((u32 *)(address));
		if (data != (u32)(data_written[i])) {
			logmemerr("memtest_ddr_data_access() spot3 failed.",
				address, data, data_written[i]);
			return 3;
		}
		address = offset + (1 << (i+1));
	}

	address = offset + 0x800;
	for (i = 1; i <= 4; i++) {
		data = *((u32 *)(address+(4*i)));
		if (data != data_written[i+27]) {
			logmemerr("memtest_ddr_data_access() spot4 failed.",
				address, data, data_written[i+27]);
			return 4;
		}
	}
	/* test bit 0 and bit 1 of data bus */
	address = offset + 0x814;
	data = *((u32 *)address);
	if (data != data_written[32]) {
		logmemerr("memtest_ddr_data_access() spot5 failed.",
			address, data, data_written[i+27]);
		return 5;
	}

	logdebug("memtest_ddr_data_access() successful!");
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
	int ret;

	logdebug("memtest_main() starting, base = %x", base);

	switch (CONFIG_MEMTEST_METHOD) {
	case OPTION_MEMTEST_METHOD_OLD:
		ret = memtest_ddr_data_access((unsigned int)base);
		ret |= memtest_fast(base + 0x10000/sizeof(datum_t), memsize - 0x10000);
		break;
	case OPTION_MEMTEST_METHOD_FAST:
		ret = memtest_fast(base, memsize);
		ret |= memtest_fast(base + 0x10000/sizeof(datum_t), memsize - 0x10000);
		break;
	case OPTION_MEMTEST_METHOD_FULL:
		logwarn("Running FULL memtest. This will take a long time "
			"If you are flashing, "
			"flashing will timeout with an error.");
		ret = memtest_full(base, memsize);
		break;
	}

	return ret; /* Any error */
}
