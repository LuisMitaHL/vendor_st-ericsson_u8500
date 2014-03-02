/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 *
 * Origin: Most part are moved here from old board code for u8500.
 *
 * License terms: GNU General Public License (GPL), version 2.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/cpu.h>
#include <asm/arch/prcmu.h>

#ifdef CONFIG_ARCH_CPU_INIT

/* Old table to init ED version of db8500. Should be removed */
unsigned int addr_vall_arr[] = {
0x8011F000, 0x0000FFFF, /* Clocks for HSI  TODO Enable reqd only */
0x8011F008, 0x00001CFF, /* Clocks for HSI  TODO Enable reqd only */
0x8000F000, 0x00007FFF, /* Clocks for I2C  TODO Enable reqd only */
0x8000F008, 0x00007FFF, /* Clocks for I2C  TODO Enable reqd only */
0x80157020, 0x00000150, /* I2C 48MHz clock */
0x8012F000, 0x00007FFF, /* Clocks for SD  TODO Enable reqd only */
0x8012F008, 0x00007FFF, /* Clocks for SD  TODO Enable reqd only */
0xA03DF000, 0x0000000D, /* Clock for MTU Timers */
0x8011E00C, 0x00000000, /* GPIO ALT FUNC for EMMC */
0x8011E004, 0x0000FFE0, /* GPIO ALT FUNC for EMMC */
0x8011E020, 0x0000FFE0, /* GPIO ALT FUNC for EMMC */
0x8011E024, 0x00000000, /* GPIO ALT FUNC for EMMC */
0x8012E000, 0x20000000, /* GPIO ALT FUNC for UART */
0x8012E00C, 0x00000000, /* GPIO ALT FUNC for SD */
0x8012E004, 0x0FFC0000, /* GPIO ALT FUNC for SD */
0x8012E020, 0x60000000, /* GPIO ALT FUNC for SD */
0x8012E024, 0x60000000, /* GPIO ALT FUNC for SD */
0x801571E4, 0x0000000C, /* PRCMU settings for B2R2, PRCM_APE_RESETN_SET_REG */
0x80157024, 0x00000130, /* PRCMU settings for EMMC/SD */
0xA03FF000, 0x00000003, /* USB */
0xA03FF008, 0x00000001, /* USB */
0xA03FE00C, 0x00000000, /* USB */
0xA03FE020, 0x00000FFF, /* USB */
0xA03FE024, 0x00000000	/* USB */
};

static void init_regs(void)
{
	/* FIXME Remove magic register array settings for ED also */
	if (u8500_is_earlydrop()) {
		unsigned i;

		for (i = 0; i < ARRAY_SIZE(addr_vall_arr)/2; i++)
			*((volatile unsigned int *)(addr_vall_arr[2 * i]))
				= addr_vall_arr[(2 * i) + 1];
	} else {
		db8500_prcmu_init();
		db8500_clocks_init();
	}
}

/*
 * SOC specific cpu init
 */
int arch_cpu_init(void)
{
	if (u8500_is_earlydrop())
		/* MTU timer clock always enabled (not clocked) */
		writel(0x20000, PRCM_TCR);

	icache_enable();
	init_regs();

	return 0;
}
#endif /* CONFIG_ARCH_CPU_INIT */

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	char *cpu, *version, *clock, *pll;
	char clockbuf[24];
	uint32_t arm_khz;

	if (cpu_is_u9540v10()) {
		cpu = "a9540";
		version = "v1.0";
	} else if (cpu_is_a9500_generic()) {
		cpu = "a9500";
		if (cpu_is_a9500v1())
			version = "v1.0";
		else
			version = "Unknown";
	} else {
		cpu = "db8500";
		/* CPU version: Match most likely CPU first */
		if (cpu_is_u8500v2()) {
			if (cpu_is_u8500v22())
				version = "v2.2";
			else if (cpu_is_u8500v21())
				version = "v2.1";
			else if (cpu_is_u8500v20())
				version = "v2.0";
			else if (cpu_is_u8520v22())
				version = "u8520v2.2";
			else
				version = "v2.x";
		} else if (cpu_is_u8500v11())
			version = "v1.1";
		else if (cpu_is_u8500v1())
			version = "v1.0";
		else if (u8500_is_earlydrop())
			version = "ED";
		else
			version = "Unknown";
	}

	switch (readl(PRCM_ARM_CHGCLKREQ_REG) & 0x3)
	{
		case 0:
			pll = "ARM Pll";
			break;
		case 2:
			pll = "VFCG Pll";
			break;
		case 1:
		case 3:
			pll = "External clock";
			break;
	}

	/* CPU clock speed */
	if (cpu_is_u9540v10())
		arm_khz = db9540_clock_cpu_khz();
	else
		arm_khz = db8500_clock_cpu_khz();

	if (arm_khz == 0)
		clock = "External clock";
	else {
		sprintf(clockbuf, "%u.%u MHz",
			arm_khz / 1000, arm_khz % 1000);
		clock = clockbuf;
	}

	printf("CPU:\tST-Ericsson %s %s (Dual ARM Cortex A9) "
	       "pll: %s currently at %s\n", cpu, version, pll, clock);

	return 0;
}
#endif /* CONFIG_DISPLAY_CPUINFO */

