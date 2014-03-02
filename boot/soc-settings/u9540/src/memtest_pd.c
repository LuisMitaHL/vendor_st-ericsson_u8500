/*
 * Copyright (c) 1998 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 * Origin:
 * http://www.netrino.com/Embedded-Systems/How-To/Memory-Test-Suite-C
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 *
 * Update to answer ST-Ericsson's soc-settings' needs and coding style.
 */

#include <memtest_pd.h>
#include <log.h>

/**
 * Function: memtest_databus()
 *
 * Description: Test the data bus wiring in a memory region by
 *  performing a walking 1's test at a fixed address
 *  within that region.  The address (and hence the
 *  memory region) is selected by the caller.
 *
 * Notes: This function should be called once for each memory chip.
 *
 * Returns: 0 if the test succeeds.
 *  A non-zero result is the first pattern that failed.
 */
datum_t memtest_databus(
	volatile datum_t *address)
{
	datum_t pattern;
	datum_t val;

	logdebug2("memtest_databus() start, address=%x", address);

	/* Perform a walking 1's test at the given address. */
	for (pattern = 1; pattern != 0; pattern <<= 1) {
		/* Write the test pattern. */
		*address = pattern;

		/* Read it back (immediately is okay for this test). */
		val = *address;
		if (val != pattern) {
			logmemerr("Databus failed.", address, val, pattern);
			return pattern;
		}
	}

	logdebug2("memtest_databus() successful");
	return 0;
}

/**
 *
 * Function: memtest_addressbus()
 *
 * Description: Test the address bus wiring in a memory region by
 *  performing a walking 1's test on the relevant bits
 *  of the address and checking for aliasing. This test
 *  will find single-bit address failures such as stuck
 *  -high, stuck-low, and shorted pins.  The address address
 *  and size of the region are selected by the caller.
 *
 * Notes: For best results, the selected address address should
 *  have enough LSB 0's to guarantee single address bit
 *  changes.  For example, to test a 64-Kbyte region,
 *  select a address address on a 64-Kbyte boundary.  Also,
 *  select the region size as a power-of-two--if at all
 *  possible.
 *
 * Returns: NULL if the test succeeds.
 *  A non-zero result is the first address at which an
 *  aliasing problem was uncovered.  By examining the
 *  contents of memory, it may be possible to gather
 *  additional information about the problem.
 */
datum_t *memtest_addressbus(
	volatile datum_t *address,
	unsigned long nbytes)
{
	unsigned long mask = (nbytes/sizeof(datum_t) - 1);
	unsigned long offset;
	unsigned long testOffset;

	datum_t pattern = (datum_t) 0xAAAAAAAA;
	datum_t antipattern = (datum_t) 0x55555555;
	datum_t val;

	logdebug2("memtest_addressbus() start. address=%x nbytes=%x",
		address, nbytes);

	/* Write the default pattern at each of the power-of-two offsets. */
	for (offset = 1; (offset & mask) != 0; offset <<= 1)
		address[offset] = pattern;

	/* Check for address bits stuck high. */
	testOffset = 0;
	address[testOffset] = antipattern;
	for (offset = 1; (offset & mask) != 0; offset <<= 1) {
		val = address[offset];
		if (val != pattern) {
			logmemerr("Address bus bit stuck high.",
				&address[offset], val, pattern);
			return (datum_t *) &address[offset];
		}
	}
	address[testOffset] = pattern;

	/* Check for address bits stuck low or shorted. */
	for (testOffset = 1; (testOffset & mask) != 0; testOffset <<= 1) {
		address[testOffset] = antipattern;
		val = address[0];
		if (val != pattern) {
			logmemerr("Address bus bit stuck low or shorted.",
				&address[testOffset], val, pattern);
			return (datum_t *) &address[testOffset];
		}

		for (offset = 1; (offset & mask) != 0; offset <<= 1) {
			val = address[offset];
			if ((val != pattern) &&
			  (offset != testOffset)) {
				logmemerr("Address bus bit stuck low "
					"or shorted.",
					&address[testOffset], val, pattern);
				return (datum_t *) &address[testOffset];
			}
		}
		address[testOffset] = pattern;
	}

	logdebug2("memtest_addressbus() successful");
	return NULL;
}

/**
 * Function: memtest_device()
 *
 * Description: Test the integrity of a physical memory device by
 *  performing an increment/decrement test over the
 *  entire region.  In the process every storage bit
 *  the device is tested as a zero and a one.  The
 *  address address and the size of the region are
 *  selected by the caller.
 *
 * Notes:
 *
 * Returns: NULL if the test succeeds.
 *  A non-zero result is the first address at which an
 *  incorrect value was read back.  By examining the
 *  contents of memory, it may be possible to gather
 *  additional information about the problem.
 */
datum_t *memtest_device(
	volatile datum_t *address,
	unsigned long nbytes)
{
	unsigned long offset;
	unsigned long nwords = nbytes / sizeof(datum_t);

	datum_t pattern;
	datum_t antipattern;
	datum_t val;

	logdebug("memtest_device() start. address=%x nbytes=%x",
		address, nbytes);

	/* Fill memory with a known pattern.*/
	logdebug("Fill memory with a known pattern.");
	for (pattern = 1, offset = 0; offset < nwords; pattern++, offset++)
		address[offset] = pattern;

	/* Check each location and invert it for the second pass.*/
	logdebug("Check each location and invert it for the second pass.");
	for (pattern = 1, offset = 0; offset < nwords; pattern++, offset++) {
		val = address[offset];
		if (val != pattern) {
			logmemerr("Memory error, first pass.",
				&address[offset], val, pattern);
			return (datum_t *) &address[offset];
		}

		antipattern = ~pattern;
		address[offset] = antipattern;
	}

	/* Check each location for the inverted pattern. */
	logdebug("Check each location for the inverted pattern. ");
	for (pattern = 1, offset = 0; offset < nwords; pattern++, offset++) {
		antipattern = ~pattern;
		val = address[offset];
		if (val != antipattern) {
			logmemerr("Memory error, second pass.",
				&address[offset], val, pattern);
			return (datum_t *) &address[offset];
		}
	}

	logdebug("memtest_device() success!");
	return NULL;
}
