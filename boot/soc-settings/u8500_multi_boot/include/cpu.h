/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __CPU_H__
#define __CPU_H__

#include <types.h>

/*
 * Bits in Co-processor 15, Control register 1
 */
#define CP15_CR1_DCACHE	(1 << 2) /* Dcache */
#define CP15_CR1_ICACHE	(1 << 12) /* Icache enable */

/**
 * Memory barrier. Compiler can't optimize over it
 */
static inline void barrier(void)
{
	__asm__ __volatile__ ("" : : : "memory");
}

/**
 * NOP (No operation) instruction to delay the CPU minimal amount of time.
 */
static inline void nop(void)
{
	__asm__ __volatile__("mov r0,r0	@ nop");
	barrier();
}

/**
 * Used to keep CPU busy in a busy-wait-loop,
 * but not skip it (being optimized away) or over heat
 */
static inline void cpu_relax(void)
{
	nop();
}

/**
 * Read from co-processor 15, control register 1.
 */
static inline u32 cp15_read_cr1(void)
{
	u32 value;
	__asm__ __volatile__("mrc p15, 0, %0, c1, c0, 0"
			"	@ read co-cpu 15, ctrl-reg 1" : "=r"(value)
			: : "memory");
	return value;
}

/**
 * Write to co-processor 15, control register 1.
 */
static inline void cp15_write_cr1(u32 value)
{
	__asm__ __volatile__("mcr p15, 0, %0, c1, c0, 0"
		"	@ write co-cpu 15, ctrl-reg 1" : : "r"(value)
		: "memory");
}

/**
 * Set a bit in co-processor 15, control register 1
 */
static inline void cp15_setbit_cr1(u32 mask)
{
	u32 reg;

	reg = cp15_read_cr1();
	cp15_write_cr1(reg | mask);
}

/**
 * Clear a bit in co-processor 15, control register 1
 */
static inline void cp15_clearbit_cr1(u32 mask)
{
	u32 reg;

	reg = cp15_read_cr1();
	cp15_write_cr1(reg & ~mask);
}

/* Check status of a bit in co-processor 15, control register 1 */
static inline int cp15_checkbit_cr1(u32 mask)
{
	return (cp15_read_cr1() & mask) != 0;
}

/**
 * Enable CPU L1 instruction cache
 */
static inline void l1icache_enable(void)
{
	cp15_setbit_cr1(CP15_CR1_ICACHE);
}

/**
 * Disable CPU L1 instruction cache
 */
static inline void l1cache_disable(void)
{
	cp15_clearbit_cr1(CP15_CR1_ICACHE);
}

/**
 * Get status of CPU L1 instruction cache
 */
static inline int l1icache_status(void)
{
	return cp15_checkbit_cr1(CP15_CR1_ICACHE);
}

/* Enable CPU L1 data cache */
static inline void l1dcache_enable(void)
{
	cp15_setbit_cr1(CP15_CR1_DCACHE);
}

/**
 * Disable CPU L1 data cache
 */
static inline void l1dcache_disable(void)
{
	cp15_clearbit_cr1(CP15_CR1_DCACHE);
}

/**
 * Get status of CPU L1 data cache
 */
static inline int l1dcache_status(void)
{
	return cp15_checkbit_cr1(CP15_CR1_DCACHE);
}

#endif /* __CPU_H__ */
