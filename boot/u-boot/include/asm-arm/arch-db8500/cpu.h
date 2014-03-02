/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 *
 * Origin: Code split from board/st/u8500/u8500.c
 *
 * License terms: GNU General Public License (GPL), version 2.
 */

#ifndef __DB8500_CPU_H__
#define __DB8500_CPU_H__

#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/tee.h>

#define CPUID_DB8500ED		0x410fc090
#define CPUID_DB8500V1		0x411fc091
#define CPUID_DB8500V2		0x412fc091
#define CPUID_DB9540V1		0x413fc090

#define ASICID_DB8500V11	0x008500A1
#define ASICID_DB8500V20	0x008500B0
#define ASICID_DB8500V21	0x008500B1
#define ASICID_DB8500V22	0x008500B2
#define ASICID_DB8520V22	0x008520B2
#define ASICID_DB9540V10	0x009540A0

/*
 * Keep these CPU identity functions inline here because they are short
 * and used by many. Will make for fast optimized compiled code.
 */

static inline unsigned int read_cpuid(void)
{
	unsigned int val;

	/* Main ID register (MIDR) */
	asm("mrc        p15, 0, %0, c0, c0, 0"
	   : "=r" (val)
	   :
	   : "cc");

	return val;
}


static inline int u9540_is_v1(void)
{
	return read_cpuid() == CPUID_DB9540V1;
}

static inline int u8500_is_earlydrop(void)
{
	return read_cpuid() == CPUID_DB8500ED;
}

static inline int cpu_is_u8500v1(void)
{
	return read_cpuid() == CPUID_DB8500V1;
}

static inline int cpu_is_u8500v2(void)
{
	return read_cpuid() == CPUID_DB8500V2;
}

static inline int cpu_is_a9500_generic(void)
{
	unsigned int product_id;
	int ret_val;

	ret_val = get_productid(&product_id);

	/* Only one version exists so far (Nov 2011) */
	return ((ret_val == 0) &&
		((product_id == PRODUCT_ID_9500) || (product_id == PRODUCT_ID_9500C)));
}

static inline int cpu_is_a9500v1(void)
{
	unsigned int product_id;
	int ret_val;

	ret_val = get_productid(&product_id);

	return ((ret_val == 0) &&
		((product_id == PRODUCT_ID_9500) || (product_id == PRODUCT_ID_9500C)));
}

static inline unsigned int read_asicid(void)
{
	unsigned int *address;

	if (u9540_is_v1())
		address = (void *) U9540_ASIC_ID_LOC_V1;
	else if (u8500_is_earlydrop() || cpu_is_u8500v1())
		address = (void *) U8500_ASIC_ID_LOC_ED_V1;
	else
		address = (void *) U8500_ASIC_ID_LOC_V2;

	return readl(address);
}

static inline int cpu_is_u8500v11(void)
{
	return read_asicid() == ASICID_DB8500V11;
}

static inline int cpu_is_u8500v20(void)
{
	return read_asicid() == ASICID_DB8500V20;
}

static inline int cpu_is_u8500v21(void)
{
	return read_asicid() == ASICID_DB8500V21;
}

static inline int cpu_is_u8500v22(void)
{
	return read_asicid() == ASICID_DB8500V22;
}

static inline int cpu_is_u8520v22(void)
{
	return read_asicid() == ASICID_DB8520V22;
}

static inline int cpu_is_u9540v10(void)
{
	return read_asicid() == ASICID_DB9540V10;
}

#endif /* __DB8500_CPU_H__ */
