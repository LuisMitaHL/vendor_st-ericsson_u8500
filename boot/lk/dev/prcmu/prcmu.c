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

#include <stdio.h>
#include <stdint.h>
#include <reg.h>
#include "target_config.h"

/* PRCMU clock management regs */
#define PRCM_ARMCLKFIX_MGT_REG   (U8500_PRCMU_BASE + 0x000)
#define PRCM_ACLK_MGT_REG        (U8500_PRCMU_BASE + 0x004)
#define PRCM_SDMMCCLK_H_MGT_REG  (U8500_PRCMU_BASE + 0x008)
#define PRCM_SVAMMDSPCLK_MGT_REG (U8500_PRCMU_BASE + 0x008)
#define PRCM_SIAMMDSPCLK_MGT_REG (U8500_PRCMU_BASE + 0x00C)
#define PRCM_SAAMMDSPCLK_MGT_REG (U8500_PRCMU_BASE + 0x010)
#define PRCM_SGACLK_MGT_REG      (U8500_PRCMU_BASE + 0x014)
#define PRCM_UARTCLK_MGT_REG     (U8500_PRCMU_BASE + 0x018)
#define PRCM_MSPCLK_MGT_REG      (U8500_PRCMU_BASE + 0x01C)
#define PRCM_I2CCLK_MGT_REG      (U8500_PRCMU_BASE + 0x020)
#define PRCM_SDMMCCLK_MGT_REG    (U8500_PRCMU_BASE + 0x024)
#define PRCM_SLIMCLK_MGT_REG     (U8500_PRCMU_BASE + 0x028)
#define PRCM_PER1CLK_MGT_REG     (U8500_PRCMU_BASE + 0x02C)
#define PRCM_PER2CLK_MGT_REG     (U8500_PRCMU_BASE + 0x030)
#define PRCM_PER3CLK_MGT_REG     (U8500_PRCMU_BASE + 0x034)
#define PRCM_PER5CLK_MGT_REG     (U8500_PRCMU_BASE + 0x038)
#define PRCM_PER6CLK_MGT_REG     (U8500_PRCMU_BASE + 0x03C)
#define PRCM_PER7CLK_MGT_REG     (U8500_PRCMU_BASE + 0x040)
#define PRCM_DMACLK_MGT_REG      (U8500_PRCMU_BASE + 0x074)
#define PRCM_B2R2CLK_MGT_REG     (U8500_PRCMU_BASE + 0x078)

#define PRCM_PLLSOC0_FREQ_REG    (U8500_PRCMU_BASE + 0x080)
#define PRCM_PLLSOC1_FREQ_REG    (U8500_PRCMU_BASE + 0x084)
#define PRCM_PLLARM_FREQ_REG     (U8500_PRCMU_BASE + 0x088)
#define PRCM_PLLDDR_FREQ_REG     (U8500_PRCMU_BASE + 0x08C)
#define PRCM_ARM_CHGCLKREQ_REG   (U8500_PRCMU_BASE + 0x114)
#define PRCM_PLLVFCGARM_FREQ_REG (U8500_PRCMU_BASE + 0x2000)
#define PRCM_RESET_STATUS        (U8500_PRCMU_BASE + 0x103C)

#define PRCM_TCR_REG             (U8500_PRCMU_BASE + 0x1C8)

/* Mailbox management regs*/
#define PRCM_MBOX_CPU_VAL_REG   (U8500_PRCMU_BASE + 0x0fc)
#define PRCM_MBOX_CPU_SET_REG   (U8500_PRCMU_BASE + 0x100)

/* Mailbox interrupts ack regs */
#define PRCM_ARM_IT1_CLEAR_REG  (U8500_PRCMU_BASE + 0x48C)
#define PRCM_ARM_IT1_VAL_REG    (U8500_PRCMU_BASE + 0x494)

#define PRCM_REQ_MB3            0xE4C /* 372 bytes  */
#define PRCM_REQ_MB5            (tcdm_base + 0xE44)
#define PRCM_ACK_MB5            (tcdm_base + 0xDF4)

/* PRCMU spare out reg */
#define PRCM_SPARE_OUT_REG      (U8500_PRCMU_BASE + 0x2070)

/* Mailbox headers */
#define _PRCM_MBOX_HEADER           0xFE8 /* 16 bytes */
#define PRCM_MBOX_HEADER_REQ_MB0    (_PRCM_MBOX_HEADER + 0x0)
#define PRCM_MBOX_HEADER_REQ_MB3    (_PRCM_MBOX_HEADER + 0x3)
#define PRCM_MBOX_HEADER_ACK_MB0    (_PRCM_MBOX_HEADER + 0x8)

/* Mailbox 0 headers */
#define MB0H_WAKEUP_EXE             2
#define MB0H_READ_WAKEUP_ACK        3
#define MB0H_WAKEUP_SLEEP           5

/* Mailbox 0 ACKs */
#define PRCM_ACK_MB0 0xE08 /* 52 bytes  */
#define PRCM_ACK_MB0_READ_POINTER   (PRCM_ACK_MB0 + 0x1)
#define PRCM_ACK_MB0_WAKEUP_0_8500  (PRCM_ACK_MB0 + 0x4)
#define PRCM_ACK_MB0_WAKEUP_1_8500  (PRCM_ACK_MB0 + 0x1C)

/* Mailbox 3 headers */
#define MB3H_SYSCLK                 0xE

/* Mailbox 3 Requests */
#define PRCM_REQ_MB3_SYSCLK_MGT     (PRCM_REQ_MB3 + 0x16C)

/* Mailbox 5 Requests */
#define PRCM_REQ_MB5_I2COPTYPE_REG  (PRCM_REQ_MB5 + 0x0)
#define PRCM_REQ_MB5_BIT_FIELDS     (PRCM_REQ_MB5 + 0x1)
#define PRCM_REQ_MB5_I2CSLAVE       (PRCM_REQ_MB5 + 0x2)
#define PRCM_REQ_MB5_I2CVAL         (PRCM_REQ_MB5 + 0x3)

/* Mailbox 5 ACKs */
#define PRCM_ACK_MB5_STATUS     (PRCM_ACK_MB5 + 0x1)
#define PRCM_ACK_MB5_SLAVE      (PRCM_ACK_MB5 + 0x2)
#define PRCM_ACK_MB5_VAL        (PRCM_ACK_MB5 + 0x3)

/* Mailbox bits */
#define BIT(x_) (1<<(x_))
#define I2C_MBOX_BIT            BIT(5)
#define WAKEUP_BIT_SYSCLK_OK    BIT(9)

/* SDMMC0 PSW bit */
#define PSW_SDMMC               BIT(1)

/* PRCMU power state */
#define PRCM_XP70_CUR_PWR_STATE     (tcdm_base + 0xFFC)

/*
 * Current power states defined in PRCMU firmware
 * PRCM_AP_EXECUTE: Current power state is apExecute
 */
#define PRCM_PSTAT_AP_EXECUTE   0x02

/* I2C Read/Write ops for PRCMU FW */
#define PRCMU_I2CWRITE  0
#define PRCMU_I2CREAD   1

/* R/W ops status */
#define PRCMU_STAT_I2C_WR_OK    0x01
#define PRCMU_STAT_I2C_RD_OK    0x02

/* ON/OFF states */
#define PRCMU_OFF       0
#define PRCMU_ON        1

#ifdef  DEBUG_PRCMU
#define debug(lvl,fmt,args...)      dprintf (lvl,fmt ,##args)
#else
#define debug(lvl,fmt,args...)
#endif

#define PRCMU_I2C_WRITE(slave)  \
    (((slave) << 1) | PRCMU_I2CWRITE | (1 << 6))

#define PRCMU_I2C_READ(slave) \
    (((slave) << 1) | PRCMU_I2CREAD | (1 << 6))

static void *tcdm_base;

static int prcmu_is_ready(void)
{
    int ready;

    if (!tcdm_base) {
        tcdm_base = (void *) U8500_PRCMU_TCDM_BASE;
    }

    ready = readb(PRCM_XP70_CUR_PWR_STATE) == PRCM_PSTAT_AP_EXECUTE;
    if (!ready)
        dprintf(INFO,"PRCMU firmware not ready\n");

    return ready;
}

static int wait_for_i2c_mbx_rdy(void)
{
    int timeout = 10000;

    if (readl(PRCM_ARM_IT1_VAL_REG) & I2C_MBOX_BIT) {
        dprintf(INFO,"prcmu: warning i2c mailbox was not acked\n");
        /* clear mailbox 5 ack irq */
        writel(I2C_MBOX_BIT, PRCM_ARM_IT1_CLEAR_REG);
    }

    /* check any already on-going transaction */
    while ((readl(PRCM_MBOX_CPU_VAL_REG) & I2C_MBOX_BIT) && timeout--)
        ;

    if (timeout == 0)
        return -1;

    return 0;
}

static int wait_for_i2c_req_done(void)
{
    int timeout = 10000;

    /* Set an interrupt to XP70 */
    writel(I2C_MBOX_BIT, PRCM_MBOX_CPU_SET_REG);

    /* wait for mailbox 5 (i2c) ack */
    while (!(readl(PRCM_ARM_IT1_VAL_REG) & I2C_MBOX_BIT) && timeout--)
        ;

    if (timeout == 0) {
        dprintf(CRITICAL,"wait_for_i2c_req_done: timed out\n");
        return -1;
    }

    return 0;
}

/**
 * prcmu_i2c_read - PRCMU - 4500 communication using PRCMU I2C
 * @reg: - db8500 register bank to be accessed
 * @slave:  - db8500 register to be accessed
 * Returns: ACK_MB5  value containing the status
 */
int prcmu_i2c_read(uint8_t reg, uint16_t slave)
{
    uint8_t i2c_status;
    uint8_t i2c_val;
    int ret;

    if (!prcmu_is_ready())
        return -1;

    debug(INFO,"prcmu_4500_i2c_read:bank=%x;reg=%x;\n",
            reg, slave);

    ret = wait_for_i2c_mbx_rdy();
    if (ret) {
        dprintf(CRITICAL,"prcmu_i2c_read: mailbox became not ready\n");
        return ret;
    }

    /* prepare the data for mailbox 5 */
    writeb(PRCMU_I2C_READ(reg), PRCM_REQ_MB5_I2COPTYPE_REG);
    writeb((1 << 3) | 0x0, PRCM_REQ_MB5_BIT_FIELDS);
    writeb(slave, PRCM_REQ_MB5_I2CSLAVE);
    writeb(0, PRCM_REQ_MB5_I2CVAL);

    ret = wait_for_i2c_req_done();
    if (ret) {
        dprintf(CRITICAL,"prcmu_i2c_read: mailbox request timed out\n");
        return ret;
    }

    /* retrieve values */
    debug(INFO,"ack-mb5:transfer status = %x\n",
            readb(PRCM_ACK_MB5_STATUS));
    debug(INFO,"ack-mb5:reg bank = %x\n", readb(PRCM_ACK_MB5) >> 1);
    debug(INFO,"ack-mb5:slave_add = %x\n",
            readb(PRCM_ACK_MB5_SLAVE));
    debug(INFO,"ack-mb5:reg_val = %d\n", readb(PRCM_ACK_MB5_VAL));

    i2c_status = readb(PRCM_ACK_MB5_STATUS);
    i2c_val = readb(PRCM_ACK_MB5_VAL);
    /* clear mailbox 5 ack irq */
    writel(I2C_MBOX_BIT, PRCM_ARM_IT1_CLEAR_REG);

    if (i2c_status == PRCMU_STAT_I2C_RD_OK)
        return i2c_val;

    debug(INFO,"prcmu_i2c_read:read return status= %d\n", i2c_status);
    return -1;
}

/**
 * prcmu_i2c_write - PRCMU-db8500 communication using PRCMU I2C
 * @reg: - db8500 register bank to be accessed
 * @slave:  - db800 register to be written to
 * @reg_data: - the data to write
 * Returns: ACK_MB5 value containing the status
 */
int prcmu_i2c_write(uint8_t reg, uint16_t slave, uint8_t reg_data)
{
    uint8_t i2c_status;
    int ret;

    if (!prcmu_is_ready())
        return -1;

    debug(INFO,"prcmu_4500_i2c_write:bank=%x;reg=%x;\n",
            reg, slave);

    ret = wait_for_i2c_mbx_rdy();
    if (ret) {
        dprintf(CRITICAL,"prcmu_i2c_write: mailbox became not ready\n");
        return ret;
    }

    /* prepare the data for mailbox 5 */
    writeb(PRCMU_I2C_WRITE(reg), PRCM_REQ_MB5_I2COPTYPE_REG);
    writeb((1 << 3) | 0x0, PRCM_REQ_MB5_BIT_FIELDS);
    writeb(slave, PRCM_REQ_MB5_I2CSLAVE);
    writeb(reg_data, PRCM_REQ_MB5_I2CVAL);

    ret = wait_for_i2c_req_done();
    if (ret) {
        dprintf(CRITICAL,"prcmu_i2c_write: mailbox request timed out\n");
        return ret;
    }

    /* retrieve values */
    debug(INFO,"ack-mb5:transfer status = %x\n",
            readb(PRCM_ACK_MB5_STATUS));
    debug(INFO,"ack-mb5:reg bank = %x\n", readb(PRCM_ACK_MB5) >> 1);
    debug(INFO,"ack-mb5:slave_add = %x\n",
            readb(PRCM_ACK_MB5_SLAVE));
    debug(INFO,"ack-mb5:reg_val = %d\n", readb(PRCM_ACK_MB5_VAL));

    i2c_status = readb(PRCM_ACK_MB5_STATUS);
    debug(INFO,"\ni2c_status = %x\n", i2c_status);
    /* clear mailbox 5 ack irq */
    writel(I2C_MBOX_BIT, PRCM_ARM_IT1_CLEAR_REG);

    if (i2c_status == PRCMU_STAT_I2C_WR_OK)
        return 0;

    debug(INFO,"prcmu_i2c_write: i2c_status : 0x%x\n", i2c_status);
    return -1;
}

static int restarted = -1; /* -1 to get it into .data section */

static struct clk_mgt_regs {
    uint32_t addr;
    uint32_t val;
} clk_mgt_regs[] = {
    {PRCM_PER1CLK_MGT_REG, 0},
    {PRCM_PER2CLK_MGT_REG, 0},
    {PRCM_PER3CLK_MGT_REG, 0},
    /* PER4CLK does not exist */
    {PRCM_PER5CLK_MGT_REG, 0},
    {PRCM_PER6CLK_MGT_REG, 0},
    /* Only exists in ED but is always ok to write to */
    {PRCM_PER7CLK_MGT_REG, 0},
    {PRCM_UARTCLK_MGT_REG, 0},
    {PRCM_I2CCLK_MGT_REG, 0},
#if PLATFORM_DB8540
    {PRCM_SDMMCCLK_H_MGT_REG, 0},
#else
    {PRCM_SDMMCCLK_MGT_REG, 0},
#endif
    {0, 0}
};

static void prcmu_enable(uint32_t *reg)
{
    writel(readl(reg) | (1 << 8), reg);
}

void prcmu_init(void)
{
    struct clk_mgt_regs *clks;

    /* Enable timers */
    writel(1 << 17, PRCM_TCR_REG);

    clks = clk_mgt_regs;
    if (++restarted == 0) {
        /* Tuck values away */
        while (clks->addr) {
            clks->val = readl(clks->addr);
            clks++;
        }
    } else {
        /* Restore clock registers */
        while (clks->addr) {
            writel(clks->val, clks->addr);
            clks++;
        }
    }
    clks = clk_mgt_regs;
    while (clks->addr) {
        prcmu_enable((uint32_t *)clks->addr);
        clks++;
    }

    /* Clean up the mailbox interrupts after pre-bootloader code. */
    writel(I2C_MBOX_BIT, PRCM_ARM_IT1_CLEAR_REG);
}

static int wait_for_mbx_rdy(uint32_t nbit)
{
    int timeout = 10000;

    /* check any already on-going transaction */
    while ((readl(PRCM_MBOX_CPU_VAL_REG) & BIT(nbit)) && timeout--)
        ;

    if (timeout == 0)
        return -1;

    return 0;
}

static int wait_for_sysclk_response(void)
{
    int timeout = 20000;
    bool done = false;
    uint8_t hdr;
    uint32_t val;

    /* wait for mailbox 0 response */
    while (!(readl(PRCM_ARM_IT1_VAL_REG) & BIT(0)) && timeout--)
        ;

    if (timeout <= 0)
        return -1;

    /* check header */
    hdr= readb(tcdm_base + PRCM_MBOX_HEADER_ACK_MB0);

    if((hdr == MB0H_WAKEUP_EXE) || (hdr == MB0H_WAKEUP_SLEEP)) {
        if (readb(tcdm_base + PRCM_ACK_MB0_READ_POINTER) & 1)
            val = readl(tcdm_base + PRCM_ACK_MB0_WAKEUP_1_8500);
        else
            val = readl(tcdm_base + PRCM_ACK_MB0_WAKEUP_0_8500);

        done= val & WAKEUP_BIT_SYSCLK_OK;
    } else {
        dprintf(CRITICAL,"sysclk_response: unknown header\n");
    }

    writel(BIT(0), PRCM_ARM_IT1_CLEAR_REG);

    if(!done) {
        dprintf(CRITICAL,"sysclk_response: no sysclk response\n");
        return -2;
    }

    /*confirm to prcmu*/
    if (wait_for_mbx_rdy(0)) {
        dprintf(CRITICAL,"sysclk_response: mailbox 0 not ready\n");
        return -3;
    }

    writeb(MB0H_READ_WAKEUP_ACK, (tcdm_base + PRCM_MBOX_HEADER_REQ_MB0));
    writel(BIT(0), PRCM_MBOX_CPU_SET_REG);

    return 0;
}

/**
 * prcmu_sysclk_request - PRCMU-db8500 sysclock request
 * @enable: - enable or disable sysclock
 * Returns: ACK_MB0 value containing the status
 */
int prcmu_sysclk_request(bool enable)
{
    int r;

    r= wait_for_mbx_rdy(3u);
    if (r) {
        dprintf(CRITICAL,"prcmu_request_sysclk: mailbox 3 not ready\n");
        return r;
    }

    writeb((enable ? PRCMU_ON : PRCMU_OFF), (tcdm_base + PRCM_REQ_MB3_SYSCLK_MGT));
    writeb(MB3H_SYSCLK, (tcdm_base + PRCM_MBOX_HEADER_REQ_MB3));
    writel(BIT(3), PRCM_MBOX_CPU_SET_REG);

    /*
     * The firmware only sends an ACK if we want to enable the
     * SysClk, and it succeeds.
     */
    if (enable)
        r= wait_for_sysclk_response();

    return r;
}

void prcmu_set_psw(bool status)
{
    uint32_t val;

    val = readl(PRCM_SPARE_OUT_REG);
    if (status)
        val |= PSW_SDMMC;
    else
        val &= ~PSW_SDMMC;
    writel(val, PRCM_SPARE_OUT_REG);
}

#ifdef WITH_LIB_CONSOLE
#include <debug.h>
#include <stdlib.h>
#include <printf.h>
#include <list.h>
#include <string.h>
#include <platform/debug.h>
#include <lib/console.h>

/* PLLs for clock management registers */
enum {
    GATED = 0,
    PLLSOC0,    /* pllsw = 001, ffs() = 1 */
    PLLSOC1,    /* pllsw = 010, ffs() = 2 */
    PLLDDR,     /* pllsw = 100, ffs() = 3 */
    PLLARM,
};

static struct pll_frequency_registers {
    int      index; /* index fror pll_name and pll_khz arrays */
    uint32_t address;
} pll_frequency_registers[] = {
    {PLLSOC0, PRCM_PLLSOC0_FREQ_REG},
    {PLLSOC1, PRCM_PLLSOC1_FREQ_REG},
    {PLLDDR, PRCM_PLLDDR_FREQ_REG},
    {PLLARM, PRCM_PLLARM_FREQ_REG},
    {0, 0},
};

static const char *pll_name[5] = {"GATED", "SOC0", "SOC1", "DDR", "ARM"};
static uint32_t pll_khz[5]; /* use ffs(pllsw(reg)) as index for 0..3 */

static struct clk_management_registers {
    uint32_t   address;
    const char *description;
} clk_management_registers[] = {
    {PRCM_ARMCLKFIX_MGT_REG, "ARMCLKFIX"},
    {PRCM_ACLK_MGT_REG, "ACLK"},
#if PLATFORM_DB8540
    {PRCM_SDMMCCLK_H_MGT_REG, "SDMMC_H"},
#else /* PLATFORM_DB8540 */
    {PRCM_SVAMMDSPCLK_MGT_REG, "SVA"},
#endif /* PLATFORM_DB8540 */
    {PRCM_SIAMMDSPCLK_MGT_REG, "SIA"},
#if !PLATFORM_DB8540
    {PRCM_SAAMMDSPCLK_MGT_REG, "SAA"},
#endif /* PLATFORM_DB8540 */
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
    {PRCM_PER7CLK_MGT_REG, "PER7"},
    {PRCM_DMACLK_MGT_REG, "DMA"},
    {PRCM_B2R2CLK_MGT_REG, "B2R2"},
    {0, NULL},
};

static int cmd_clkinfo(int argc, const cmd_args *argv);

STATIC_COMMAND_START
#if DEBUGLEVEL > 0
    { "clkinfo", "clock information", &cmd_clkinfo },
#endif
STATIC_COMMAND_END(cmd_prcmu);

static int cmd_clkinfo(int argc, const cmd_args *argv)
{
    uint32_t inclk_khz;
    uint32_t reg, phi, idf, ldf, odf, seldiv;
    uint32_t clk_khz;
    unsigned int clock_sel;
    struct clk_management_registers *clks = clk_management_registers;
    struct pll_frequency_registers *plls = pll_frequency_registers;

    /* Parameters are not used */
    (void) argc;
    (void) argv;

    /*
     * Go through list of PLLs.
     * Initialise pll out frequency array (pll_khz) and print frequency.
     */
    inclk_khz = 38400;  /* 38.4 MHz */
    while (plls->address) {
        reg = readl(plls->address);

        /*
         * PLLOUTCLK = PHI = (INCLK*LDF)/(2*ODF*IDF) if SELDIV2=0
         * PLLOUTCLK = PHI = (INCLK*LDF)/(4*ODF*IDF) if SELDIV2=1
         * where:
         * IDF=R(2:0) (when R=000, IDF=1d)
         * LDF = 2*D(7:0) (D must be greater than or equal to 6)
         * ODF = N(5:0) (when N=000000, 0DF=1d)
        */

        idf = (reg & 0x70000) >> 16;
        ldf = (reg & 0xff) * 2;
        odf = (reg & 0x3f00) >> 8;
        seldiv = (reg & 0x01000000) >> 24;
        phi = (inclk_khz * ldf) / (2 * odf * idf);
        if (seldiv) {
            phi = phi/2;
        }
        pll_khz[plls->index] = phi;
        printf("%s PLL out frequency: %d.%d Mhz\n",
                pll_name[plls->index], phi/1000, phi % 1000);
        plls++;
    }

    /* check ARM clock source */
    reg = readl(PRCM_ARM_CHGCLKREQ_REG);
    printf("A9 running on ");
    if (reg & 1) {
        printf("external clock");
    } else {
        printf("ARM PLL");
    }
    printf("\n");

    /* go through list of clk_mgt_reg */
    printf("\n%19s %9s %7s %9s  enabled\n",
            "name(addr)", "value", "PLL", "CLK[MHz]");
    while (clks->address) {
        reg = readl(clks->address);
        /* convert bit position into array index */
        clock_sel = ffs((reg >> 5) & 0x7);    /* PLLSW[2:0] */
        printf("%9s(%08x): %08x", clks->description, clks->address, reg);
        printf(", %6s", pll_name[clock_sel]);
        if (reg & 0x200) {
            clk_khz = 38400;    /* CLK38 is set */
        } else if ((reg & 0x1f) == 0) {
            /* ARMCLKFIX_MGT is 0x120, e.g. div = 0 ! */
            clk_khz = 0;
        } else if ((clks->address == PRCM_I2CCLK_MGT_REG) &&
                (clock_sel == PLLDDR)) {
            /*
             * i2cclk, if clk source is pllddr, divide by 2 first
             * because it is on a fixed clock
             */
            clk_khz = (pll_khz[clock_sel] / 2) / (reg & 0x1f);
        } else {
            clk_khz = pll_khz[clock_sel] / (reg & 0x1f);
        }
        printf(", %4d.%03d", clk_khz / 1000, clk_khz % 1000);
        printf(", %s\n", (reg & 0x100) ? "ena" : "dis");
        clks++;
    }

    return 0;
}
#endif // WITH_LIB_CONSOLE
