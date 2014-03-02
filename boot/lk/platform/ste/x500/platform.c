/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <err.h>
#include <reg.h>
#include <debug.h>
#include <arch/arm/mmu.h>
#include <platform.h>
#include "config.h"
#include "platform_p.h"
#include "target_config.h"
#include <platform/clock.h>
#include <platform/timer.h>
#include <dev/uart.h>
#include "reboot.h"
#include "tiny_env.h"

/* for l2cc enable and clean_invalidate */
#define L2X0_CTRL			0x100
#define L2X0_CACHE_SYNC		0x730
#define L2X0_CLEAN_INV_WAY	0x7FC
#define L2X0_WAY_MASK		0xFFFF
#define L2X0_DATA_LOCKDOWN_WAY	0x900
#define L2X0_INST_LOCKDOWN_WAY	0x904


#define MODEM_MEM_BASE      0x06000000
#define MODEM_MEM_SIZE      0x01000000


void platform_init_mmu_mappings(unsigned mem_size)
{
	addr_t addr;

	/*
	 * arm_mmu_init() did a default 1:1 mapping (uncached) for us
	 * re-init with cached memory pages
	 */
	for (addr = U8500_SDRAM_BASE; addr < U8500_SDRAM_BASE + mem_size;
			addr += (1024*1024)) {
		arm_mmu_map_section(addr, addr, MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_NO_ALLOCATE);
	}


	/* splash screen */
#if CONFIG_VIDEO_LOGO
	arm_mmu_map_section(CONFIG_SYS_VIDEO_FB_ADRS, CONFIG_SYS_VIDEO_FB_ADRS,
			MMU_MEMORY_AP_READ_WRITE);
#endif

    /* map modem memory as un-cached memory */
    for (addr = MODEM_MEM_BASE; addr < MODEM_MEM_BASE + MODEM_MEM_SIZE;
         addr += (1024*1024)) {
        arm_mmu_map_section(addr, addr, MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_NO_ALLOCATE);
    }
}

void db8500_l2_clean_invalidate_lock(void)
{

	if ((readl(U8500_L2CC_BASE + L2X0_CTRL) & 1) == 1) {
		/* Invalidate all ways */
		writel(L2X0_WAY_MASK, U8500_L2CC_BASE + L2X0_CLEAN_INV_WAY);
		while (readl(U8500_L2CC_BASE + L2X0_CLEAN_INV_WAY)
				& L2X0_WAY_MASK)
			;
		writel(L2X0_WAY_MASK, U8500_L2CC_BASE + L2X0_DATA_LOCKDOWN_WAY);
		writel(L2X0_WAY_MASK, U8500_L2CC_BASE + L2X0_INST_LOCKDOWN_WAY);
	}
}


void platform_early_init(void)
{
	struct prcmu *prcmu = (struct prcmu *) U8500_PRCMU_BASE;

	/* Enable timers */
	writel(1 << 17, &prcmu->tcr);

	u8500_prcmu_enable(&prcmu->per1clk_mgt);
	u8500_prcmu_enable(&prcmu->per2clk_mgt);
	u8500_prcmu_enable(&prcmu->per3clk_mgt);
	u8500_prcmu_enable(&prcmu->per5clk_mgt);
	u8500_prcmu_enable(&prcmu->per6clk_mgt);
	u8500_prcmu_enable(&prcmu->per7clk_mgt);

	u8500_prcmu_enable(&prcmu->uartclk_mgt);
	u8500_prcmu_enable(&prcmu->i2cclk_mgt);

#if PLATFORM_DB8540
	u8500_prcmu_enable(&prcmu->sdmmc_h_mgt);
#else /* PLATFORM_DB8540 */
	u8500_prcmu_enable(&prcmu->sdmmcclk_mgt);
#endif /* PLATFORM_DB8540 */

	db8500_clocks_init();

	/* initialize the interrupt controller */
	platform_init_interrupts();

	/* initialize the timer block */
	platform_init_timer();

	/* initialize the uart */
	uart_init_early();

	dprintf(INFO, "platform_early_init done\n");
}

void platform_init(void)
{

	dprintf(INFO, "platform_init\n");

	uart_init();

	dprintf(INFO, "platform_init done\n");
}

static void platform_soft_reset(void)
{
	writel(1, PRCM_APE_SOFTRST);
	for(;;);
	/* no return */
}

void platform_reboot(uint16_t sw_reset_reason)
{
	switch(sw_reset_reason) {
		case SW_RESET_NORMAL:
		case SW_RESET_FASTBOOT:
		case SW_RESET_RECOVERY:
			dprintf(INFO, "%s to 0x%04x\n",
				__func__, sw_reset_reason);
			break;
		default:
			dprintf(INFO, "%s %d set default to SW_RESET_NORMAL\n",
				__func__,sw_reset_reason);
			sw_reset_reason = SW_RESET_NORMAL;
			break;
	}
	writew(sw_reset_reason, U8500_PRCMU_TCDM_BASE+PRCM_SW_RST_REASON);
	udelay(500000);
	platform_soft_reset();
}

void set_androidboot_serialno(void)
{
	uint32_t buffer[4];
	char buf[MAX_ANDROID_SERIAL_NUMBER_LEN+1];
	uint32_t *backup_ram = (uint32_t *) (PUBLIC_ID_BACKUPRAM1);

	if (backup_ram) {
		buffer[0] = backup_ram[0];
		buffer[1] = backup_ram[1];
		buffer[2] = backup_ram[2];
		buffer[3] = backup_ram[3];

		/* Form the serial number */
		sprintf(buf, "%.8X%.8X%.8X%.8X",
				buffer[0], buffer[1], buffer[2],
				buffer[3]);
		buf[MAX_ANDROID_SERIAL_NUMBER_LEN] = 0;

	} else
		return;

	create_env("androidboot.serialno", (char*)buf, true);

	return;
}
