/*
 * (C) Copyright 2009 ST-Ericsson
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/arch/clock.h>
#include <asm/arch/prcmu.h>

struct clkrst {
	unsigned int pcken;
	unsigned int pckdis;
	unsigned int kcken;
	unsigned int kckdis;
};

static unsigned int clkrst_base[] = {
	U5500_CLKRST1_BASE,
	U5500_CLKRST2_BASE,
	U5500_CLKRST3_BASE,
	0,
	U5500_CLKRST5_BASE,
	U5500_CLKRST6_BASE,
};

/* PLLs for clock management registers */
enum {
	GATED = 0,
	PLLSOC0,	/* pllsw = 001, ffs() = 1 */
	PLLSOC1,	/* pllsw = 010, ffs() = 2 */
	PLLDDR,		/* pllsw = 100, ffs() = 3 */
	PLLARM,
};

static struct pll_freq_regs {
	int idx;	/* index fror pll_name and pll_khz arrays */
	uint32_t addr;
} pll_freq_regs[] = {
	{PLLSOC0, PRCM_PLLSOC0_FREQ_REG},
	{PLLSOC1, PRCM_PLLSOC1_FREQ_REG},
	{PLLDDR, PRCM_PLLDDR_FREQ_REG},
	{PLLARM, PRCM_PLLARM_FREQ_REG},
	{0, 0},
};

static const char *pll_name[5] = {"GATED", "SOC0", "SOC1", "DDR", "ARM"};
static uint32_t pll_khz[5];	/* use ffs(pllsw(reg)) as index for 0..3 */

static struct clk_mgt_regs {
	uint32_t addr;
	const char *descr;
} clk_mgt_regs[] = {
	/* register content taken from bootrom settings */
	{PRCM_ARMCLKFIX_MGT_REG, "ARMCLKFIX"}, /* Cortex-A9 fix clock */
	{PRCM_ACLK_MGT_REG, "ACLK"},	/* DB5500 Interconnect */
	{PRCM_SVAMMDSPCLK_MGT_REG, "SVA"},
	{PRCM_SIAMMDSPCLK_MGT_REG, "SIA"},
	{PRCM_SGACLK_MGT_REG, "SGA"},
	{PRCM_UARTCLK_MGT_REG, "UART"},
	{PRCM_MSPCLK_MGT_REG, "MSP"},
	{PRCM_I2CCLK_MGT_REG, "I2C"},
	{PRCM_SDMMCCLK_MGT_REG, "SDMMC"},
	{PRCM_PER1CLK_MGT_REG, "PER1"},
	{PRCM_PER2CLK_MGT_REG, "PER2"},
	{PRCM_PER3CLK_MGT_REG, "PER3"},
	{PRCM_PER5CLK_MGT_REG, "PER5"},
	{PRCM_PER6CLK_MGT_REG, "PER6"},
	{PRCM_DMACLK_MGT_REG, "DMA"},
	{PRCM_B2R2CLK_MGT_REG, "B2R2"},
	{0, NULL},
};

/* Turn on peripheral clock at PRCC level */
void u5500_clock_enable(int periph, int cluster, int kern)
{
	struct clkrst *clkrst = (struct clkrst *) clkrst_base[periph - 1];

	if (kern != -1)
		writel(1 << kern, &clkrst->kcken);

	if (cluster != -1)
		writel(1 << cluster, &clkrst->pcken);
}

/*
 * get_pll_freq_khz - return PLL frequency in kHz
 */

static uint32_t get_pll_freq_khz(uint32_t inclk_khz, uint32_t freq_reg)
{
	uint32_t idf, ldf, odf, phi;

	/*
	 * PLLOUTCLK = PHI = (INCLK*LDF)/(2*ODF*IDF) if SELDIV2=0
	 * PLLOUTCLK = PHI = (INCLK*LDF)/(4*ODF*IDF) if SELDIV2=1
	 * where:
	 * IDF=R(2:0) (when R=000, IDF=1d)
	 * LDF = 2*D(7:0) (D must be greater than or equal to 6)
	 * ODF = N(5:0) (when N=000000, 0DF=1d)
	 */

	idf = (freq_reg & 0x70000) >> 16;
	ldf = (freq_reg & 0xff) * 2;
	odf = (freq_reg & 0x3f00) >> 8;
	phi = (inclk_khz * ldf) / (2 * odf * idf);

	if (freq_reg & (1 << 24))	/* SELDIV2 */
		phi = phi/2;

	if (freq_reg & (1 << 25))	/* DIV2EN */
		phi = phi/2;

	return phi;
}

int do_clkinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint32_t inclk_khz;
	uint32_t reg, phi;
	uint32_t clk_khz;
	unsigned int clk_sel;
	struct clk_mgt_regs *clks = clk_mgt_regs;
	struct pll_freq_regs *plls = pll_freq_regs;

	/*
	 * Go through list of PLLs.
	 * Initialise pll out frequency array (pll_khz) and print frequency.
	 */
	/*
	 * XXX: find out the real PLL input freq. 26/26.0014 (ULP PLL)
	 */
	inclk_khz = 26000; /* 26.0 MHz */
	while (plls->addr) {
		reg = readl(plls->addr);
		phi = get_pll_freq_khz(inclk_khz, reg);
		pll_khz[plls->idx] = phi;
		printf("%s PLL out frequency: %d.%d Mhz\n",
				pll_name[plls->idx], phi/1000, phi % 1000);
		plls++;
	}

	/* check ARM clock source */
	reg = readl(PRCM_ARM_CHGCLKREQ_REG);
	printf("A9 running on ");
	if (reg & 1)
		printf("external clock");
	else
		printf("ARM PLL");
	printf("\n");

	/* go through list of clk_mgt_reg */
	printf("\n%19s %9s %7s %9s  enabled\n",
			"name(addr)", "value", "PLL", "CLK[MHz]");
	while (clks->addr) {
		reg = readl(clks->addr);
		/* convert bit position into array index */
		clk_sel = ffs((reg >> 5) & 0x7);	/* PLLSW[2:0] */
		printf("%9s(%08x): %08x", clks->descr, clks->addr, reg);
		printf(", %6s", pll_name[clk_sel]);
		if (reg & 0x200)
			clk_khz = 26000;	/* CLK26 is set */
		else if ((reg & 0x1f) == 0)
			/* ARMCLKFIX_MGT is 0x120, e.g. div = 0 ! */
			clk_khz = 0;
		else if ((clks->addr == PRCM_I2CCLK_MGT_REG) &&
				(clk_sel == PLLDDR))
			/*
			 * i2cclk, if clk source is pllddr, divide by 2 first
			 * because it is on a fixed clock
			 */
			clk_khz = (pll_khz[clk_sel] / 2) / (reg & 0x1f);
		else
			clk_khz = pll_khz[clk_sel] / (reg & 0x1f);
		printf(", %4d.%03d", clk_khz / 1000, clk_khz % 1000);
		printf(", %s\n", (reg & 0x100) ? "ena" : "dis");
		clks++;
	}

	return 0;
}

U_BOOT_CMD(
	clkinfo,	1,	1,	do_clkinfo,
	"print clock info",
	""
);

