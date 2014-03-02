/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Naveen Kumar Gaddipati <naveen.gaddipati@stericsson.com>
 *  for ST-Ericsson
 *
 *
 * License terms: GNU General Public License (GPL), version 2.
 */

#ifndef __DB5500_CPU_H__
#define __DB5500_CPU_H__


#include <asm/io.h>
#include <asm/arch/hardware.h>

#define U5500_BOOT_ROM_BASE 0x90000000
#define U5500_ASIC_ID_ADDRESS	(U5500_BOOT_ROM_BASE + 0x1FFF4)
#define CPUID_DB5500V1		0xA0
#define CPUID_DB5500V2		0xB0
#define CPUID_DB5500V21		0xB1

/*
 * Keep these CPU identity functions inline here because they are short
 * and used by many. Will make for fast optimized compiled code.
 */

static inline unsigned int read_cpuid(void)
{
	unsigned long val;

	val = readl(U5500_ASIC_ID_ADDRESS);

	return (val & 0xFF);
}

static inline int cpu_is_u5500v1(void)
{
	return read_cpuid() == CPUID_DB5500V1;
}

static inline int cpu_is_u5500v2(void)
{
	return (read_cpuid() & 0xf0) == 0xb0;
}

static inline int cpu_is_u5500v20(void)
{
	return read_cpuid() == CPUID_DB5500V2;
}

static inline int cpu_is_u5500v21(void)
{
	return read_cpuid() == CPUID_DB5500V21;
}


#endif /* __DB5500_CPU_H__ */
