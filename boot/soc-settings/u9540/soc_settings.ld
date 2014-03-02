/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: WenHai Fang <wenhai.h.fang AT stericsson.com>
 *   for ST-Ericsson
 *
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH("arm")

MEMORY
{
    SOCSETTINGS (rw) : ORIGIN = 0x00000000, LENGTH = 0x7000
}

SECTIONS
{
    .socsettings :
    {
	KEEP(*(.meminit_version))
        KEEP(*(.type))
        KEEP(*(.length))
        .linker_payload_start = .;
	KEEP(*(.entry_function))
	/* opp settings */
	KEEP(*(.payload_opp))
	/* ab9540 settings */
	KEEP(*(.payload_pmu))
	/* c2c settings */
	KEEP(*(.payload_c2c))
	/* interlv settings */
	KEEP(*(.payload_interlv))
	/* pasr settings */
	KEEP(*(.payload_pasr))
	/* lvl0 firewall settings */
	KEEP(*(.payload_lvl0_firewall))
	/* imad settings */
	KEEP(*(.payload_imad))
	/* ddr0 settings */
	KEEP(*(.payload0))
	KEEP(*(.payload1))
	/* ddr1 settings */
	KEEP(*(.payload2))
	KEEP(*(.payload3))
	KEEP(*(.text*))
        KEEP(*(.data*))
        KEEP(*(.rodata*))
        PROVIDE(linker_payload_length = . - .linker_payload_start);
        . = ALIGN(4);
    } > SOCSETTINGS
}
