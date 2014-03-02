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

#ifndef __CLOCK_H
#define __CLOCK_H

struct prcmu {
	unsigned int armclkfix_mgt;
	unsigned int armclk_mgt;
#if PLATFORM_DB8540
	unsigned int sdmmc_h_mgt;
#else /* PLATFORM_DB8540 */
	unsigned int svammdspclk_mgt;
#endif /* PLATFORM_DB8540 */
	unsigned int siammdspclk_mgt;
	unsigned int reserved;
	unsigned int sgaclk_mgt;
	unsigned int uartclk_mgt;
	unsigned int msp02clk_mgt;
	unsigned int i2cclk_mgt;
	unsigned int sdmmcclk_mgt;
#if PLATFORM_DB8540
	unsigned int camclk_mgt;
#else /* PLATFORM_DB8540 */
	unsigned int slimclk_mgt;
#endif /* PLATFORM_DB8540 */
	unsigned int per1clk_mgt;
	unsigned int per2clk_mgt;
	unsigned int per3clk_mgt;
	unsigned int per5clk_mgt;
	unsigned int per6clk_mgt;
	unsigned int per7clk_mgt;
	unsigned int lcdclk_mgt;
#if PLATFORM_DB8540
	unsigned int spare1clk_mgt;
#else /* PLATFORM_DB8540 */
	unsigned int reserved1;
#endif /* PLATFORM_DB8540 */
	unsigned int bmlclk_mgt;
	unsigned int hsitxclk_mgt;
	unsigned int hsirxclk_mgt;
	unsigned int hdmiclk_mgt;
	unsigned int apeatclk_mgt;
	unsigned int apetraceclk_mgt;
	unsigned int mcdeclk_mgt;
	unsigned int ipi2cclk_mgt;
	unsigned int dsialtclk_mgt;
#if PLATFORM_DB8540
	unsigned int hsicclk_mgt;
#else /* PLATFORM_DB8540 */
	unsigned int spare2clk_mgt;
#endif /* PLATFORM_DB8540 */
	unsigned int dmaclk_mgt;
	unsigned int b2r2clk_mgt;
	unsigned int tvclk_mgt;
	unsigned int unused[82];
	unsigned int tcr;
	unsigned int unused1[23];
	unsigned int ape_softrst;
};

void db8500_clocks_init(void);

static inline void u8500_prcmu_enable(unsigned int *reg)
{
	writel(readl(reg) | (1 << 8), reg);
}

#endif /* __CLOCK_H */
