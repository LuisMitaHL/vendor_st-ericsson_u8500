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

#include <reg.h>
#include "target_config.h"
#include <sys/types.h>

struct clkrst {
	unsigned int pcken;
	unsigned int pckdis;
	unsigned int kcken;
	unsigned int kckdis;
};

static unsigned int clkrst_base[] = {
	U8500_PER1_PRCC_BASE,
	U8500_PER2_PRCC_BASE,
	U8500_PER3_PRCC_BASE,
	0,
	U8500_PER5_PRCC_BASE,
	U8500_PER6_PRCC_BASE,
	U8500_PER7_PRCC_BASE,	/* ED only */
};

/* Turn on peripheral clock at PRCC level */
static void clock_enable(int periph, int cluster, int kern)
{
	struct clkrst *clkrst = (struct clkrst *) clkrst_base[periph - 1];

	if (kern != -1)
		writel(1 << kern, &clkrst->kcken);

	if (cluster != -1)
		writel(1 << cluster, &clkrst->pcken);
}

/* Main clock init function. Called from platform.c:platform_early_init() */
void db8500_clocks_init(void)
{
	/*
	 * Enable all potentially needed clocks.
	 */

	clock_enable(1, 9, -1);	/* GPIO0 */
	clock_enable(2, 11, -1);	/* GPIO1 */
	clock_enable(3, 8, -1);	/* GPIO2 */
	clock_enable(5, 1, -1);	/* GPIO3 */

	clock_enable(3, 6, 6);	/* UART2 */

#if PLATFORM_DB8540
	clock_enable(1, 2, 2);	/* I2C1 */
#else /* PLATFORM_DB8540 */
	clock_enable(3, 3, 3);	/* I2C0 */
#endif /* PLATFORM_DB8540 */

	clock_enable(1, 5, 5);	/* SDI0, SD */
	clock_enable(2, 4, 2);	/* SDI4, eMMC */

	clock_enable(6, 6, -1);	/* MTU0 */

	clock_enable(3, 4, 4);	/* SDI2, PoPed eMMC */

	clock_enable(1, 1, 1);	/* UART1 */
	clock_enable(1, 0, 0);	/* UART0 */

	clock_enable(3, 2, 2);	/* SSP1 */
	clock_enable(3, 1, 1);	/* SSP0 */

	clock_enable(2, 8, -1);	/* SPI0 */
	clock_enable(2, 5, 3);	/* MSP2 */
	clock_enable(5, 0, 0);	/* USB */
}


