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
 * Update to fit ST-Ericsson's soc-settings need and coding style.
 */

#ifndef __MEMTEST_PD_H__
#define __MEMTEST_PD_H__

#include <types.h>

/*
 * Set the data bus width.
 */
typedef u32 datum_t;

/*
 * Function prototypes.
 */

/**
 * Test1: Will test each databus pin usnig the given address.
 */
datum_t memtest_databus(
	volatile datum_t *address);

/**
 * Test2: Will test each address bus pin, in the memory from 'address'
 * to 'address + nbytes'.
 * Can only be trusted if the databus test has been executed first.
 */
datum_t *memtest_addressbus(
	volatile datum_t *address,
	unsigned long nbytes);

/**
 * Test3: Will test the entire memory from 'address' to 'address + nbytes'.
 * Can only be trusted if the datapin and addressbus tests has been
 * executed first.
 */
datum_t *memtest_device(
	volatile datum_t *address,
	unsigned long nbytes);

#endif /* __MEMTEST_PD_H__ */
