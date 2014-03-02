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
    SOCSETTINGS (rw) : ORIGIN = 0x00000000, LENGTH = 0x10000
}

SECTIONS
{
    .socsettings :
    {
	/* header */
	KEEP(*(.meminit_version))
        KEEP(*(.type))
        KEEP(*(.length))
        .linker_payload_start = .;
	KEEP(*(.entry_function))

	/* code and data */
	KEEP(*(.text*))
        KEEP(*(.data*))
        KEEP(*(.rodata*))
	*(.AVS*)
        PROVIDE(linker_payload_length = . - .linker_payload_start);
        . = ALIGN(4);
    } > SOCSETTINGS
}
