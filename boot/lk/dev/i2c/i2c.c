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

#include <debug.h>
#include <err.h>
#include <reg.h>
#include <string.h>
#include <platform.h>
#include "target_config.h"
#include "i2c.h"

#define I2C_TIMEOUT 20

#define MCR_SHIFT_OP           0
#define MCR_SHIFT_A7           1
#define MCR_SHIFT_AM          12
#define MCR_SHIFT_P           14
#define MCR_SHIFT_LENGTH      15

#if defined(TEST_CMD_SPLIT)
static uint32_t i2c_mcr_built;
#endif

#if PLATFORM_DB8540
static const addr_t i2c_reg_base[] = {
    I2C0_BASE,
    I2C1_BASE,
    I2C2_BASE,
    I2C3_BASE,
    I2C4_BASE,
    I2C5_BASE
};

int bus_speeds[] = {
    I2C_SPEED,
    I2C_SPEED,
    I2C_SPEED,
    I2C_SPEED,
    I2C_SPEED,
    I2C_SPEED
};
#else /* PLATFORM_DB8540 */
static const addr_t i2c_reg_base[] = {
    I2C0_BASE,
    I2C1_BASE,
    I2C2_BASE,
    I2C3_BASE
};

int bus_speeds[] = {
    I2C_SPEED,
    I2C_SPEED,
    I2C_SPEED,
    I2C_SPEED
};
#endif /* PLATFORM_DB8540 */

#define I2C_REG_ADDR(bus, reg) (i2c_reg_base[bus] + (reg))

static inline void set_bit(uint32_t reg_addr, uint32_t bit)
{
    writel(readl(reg_addr) | bit, reg_addr);
}

static inline void clear_bit(uint32_t reg_addr, uint32_t bit)
{
    writel(readl(reg_addr) & ~bit, reg_addr);
}

static int until_bit_set(uint32_t reg, uint32_t bit, unsigned long timeout)
{
    time_t starttime = current_time();

    do {
        if ((readl(reg) & bit) != 0x0UL)
            return 0;
    } while ((current_time() - starttime) < timeout);

    dprintf(INFO, "Timeout in until_bit_set\n");

    return -1;
}

static int until_bit_clear(uint32_t reg, uint32_t bit, unsigned long timeout)
{
    time_t starttime = current_time();

    do {
        if ((readl(reg) & bit) == 0x0UL)
            return 0;
    } while ((current_time() - starttime) < timeout);

    dprintf(SPEW, "Timeout in until_bit_clear\n");

    return -1;
}

static void i2c_abort(int bus)
{
    int nbr_of_data = FLUSH_TIMEOUT;
    /* flush TX & RX FIFO */
    set_bit(I2C_REG_ADDR(bus, I2C_CR),CR_FTX);
    until_bit_clear(I2C_REG_ADDR(bus,I2C_CR), CR_FTX, I2C_TIMEOUT);

    set_bit(I2C_REG_ADDR(bus, I2C_CR),CR_FRX);
    until_bit_clear(I2C_REG_ADDR(bus,I2C_CR), CR_FRX, I2C_TIMEOUT);

    /* Empty data */
    while (nbr_of_data--) {
        if ((!(readl(I2C_REG_ADDR(bus,I2C_CR)))) & (CR_FTX | CR_FRX)) {
            break;
        }
    }

    /* ACK MTD */
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDIC);
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDWSIC);

    /* reinit device */
    i2c_init(bus);
}

void i2c_init(int bus)
{
    uint32_t brcnt2_val;

    if (I2C_LAST_BUS < bus) {
        printf("I2C bus %d not available\n", bus);
        return;
    }

    /* Disable I2C */
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),CR_PE);

    /* Clear registers */
    writel(0, I2C_REG_ADDR(bus,I2C_CR));
    writel(0, I2C_REG_ADDR(bus,I2C_SCR));
    writel(0, I2C_REG_ADDR(bus,I2C_HSMCR));
    writel(0, I2C_REG_ADDR(bus,I2C_TFTR));
    writel(0, I2C_REG_ADDR(bus,I2C_RFTR));
    writel(0, I2C_REG_ADDR(bus,I2C_DMAR));

    /* Set bus speed */
    /* Set standard mode 100K/s */
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),CR_SM_0);
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),CR_SM_1);

    /* Set BRCNT2, set BRCNT1 = 0*/
    /* Baud rate (standard) = fi2cclk / ( (BRCNT2 x 2) + Foncycle) */
    /* Foncycle = 0 */
    brcnt2_val = (uint32_t) (I2C_IN_FREQ / (I2C_SPEED * 2) );
    writel(brcnt2_val, I2C_REG_ADDR(bus, I2C_BRCR));

    /* Set Slave data setup time */
    writel((readl(I2C_REG_ADDR(bus, I2C_SCR)) & 0x0000FFFF) | (SLAVE_SETUP_TIME<<16),
             I2C_REG_ADDR(bus, I2C_SCR));

    /* Disable DMA */
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),0x800); //BIT11

    /* Disable interrupts */
    writel(0,I2C_REG_ADDR(bus,I2C_IMSCR));

    /* Set Operating mode (MASTER ONLY)*/
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),CR_OM_1);
    set_bit(I2C_REG_ADDR(bus, I2C_CR),CR_OM_0);

    /* Set FIFO threashold */
    writel(4,I2C_REG_ADDR(bus,I2C_TFTR));
    writel(4,I2C_REG_ADDR(bus,I2C_RFTR));

    /* Enable I2C */
    set_bit(I2C_REG_ADDR(bus, I2C_CR),CR_PE);
}

int i2c_write(int bus, uint8_t address, bool StopCondition, uint8_t *buffer, int len)
{
    uint32_t mcr_val;

    /* MCR register for write of len bytes */
    /* Set Adress type = 7bit | Stop condition | Slave address | Length */
    mcr_val = (1 << MCR_SHIFT_AM) | ((uint32_t)(address) << MCR_SHIFT_A7);
    mcr_val |= ((uint32_t)(len) << MCR_SHIFT_LENGTH);
    if (StopCondition) {
        mcr_val |= (1 << MCR_SHIFT_P);
    }

    /* program I2C_MCR */
    writel(mcr_val, I2C_REG_ADDR(bus,I2C_MCR));

    for (int index = 0; index < len; index++)
    {
        /* Wait for TX FIFO not full*/
        if (until_bit_clear(I2C_REG_ADDR(bus,I2C_RISR), RISR_TXFF, I2C_TIMEOUT)) {
            i2c_abort(bus);
            return -1;
        }

        /* write data */
        writeb(buffer[index],I2C_REG_ADDR(bus,I2C_TFR));
    }

    /* Wait for MTD */
    if (until_bit_set(I2C_REG_ADDR(bus,I2C_RISR), RISR_MTDWS, I2C_TIMEOUT)) {
        i2c_abort(bus);
        return -1;
    }

    /* ACK MTD */
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDIC);
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDWSIC);

    return 0;
}

int i2c_read(int bus, uint8_t address, bool StopCondition, uint8_t *buffer, int len)
{
    uint32_t mcr_val;

    /* MCR register for read of len bytes */
    /* Set Adress type = 7bit | Stop condition | Slave address | Length | Read OP */
    mcr_val = (1 << MCR_SHIFT_AM) | ((uint32_t)(address) << MCR_SHIFT_A7);
    mcr_val |= ((uint32_t)(len) << MCR_SHIFT_LENGTH) | (1 << MCR_SHIFT_OP);
    if (StopCondition) {
        mcr_val |= (1 << MCR_SHIFT_P);
    }

    /* program I2C_MCR */
    writel(mcr_val, I2C_REG_ADDR(bus,I2C_MCR));

    for (int index = 0; index < len; index++)
    {
        /* Wait for something in RX FIFO */
        if (until_bit_clear(I2C_REG_ADDR(bus,I2C_RISR), RISR_RXFE, I2C_TIMEOUT)) {
            i2c_abort(bus);
            return -1;
        }

        /* Read data */
        buffer[index] = readb(I2C_REG_ADDR(bus,I2C_RFR));
    }

    /* Wait for MTD */
    if (until_bit_set(I2C_REG_ADDR(bus,I2C_RISR), RISR_MTD, I2C_TIMEOUT)) {
        i2c_abort(bus);
        return -1;
    }

    /* ACK MTD */
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDIC);
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDWSIC);

    return 0;
}

int i2c_write_reg(int bus, uint8_t address, uint8_t reg, uint8_t val)
{
    uint8_t buffer[2];

    buffer[0] = reg;
    buffer[1] = val;

    /* write 2 bytes, transaction terminated by a stop condition */
    if (i2c_write(bus, address, true, buffer, 2)) {
        return -1;
    }

    return 0;
}

int i2c_read_reg(int bus, uint8_t address, uint8_t reg, uint8_t *val)
{
    uint8_t buffer;

    buffer = reg;

    /* write one byte, transaction not terminated by a stop condition */
    if (i2c_write(bus, address, false, &buffer, 1)) {
        return -1;
    }

    /* read one byte, transaction terminated by a stop condition */
    if (i2c_read(bus, address, true, &buffer, 1)) {
        return -1;
    }

    *val = buffer;

    return 0;
}

int i2c_probe(int bus, uint8_t address)
{
    uint32_t mcr_val;

    /* MCR register for read one byte */
    /* Set Adress type = 7bit | Set Stop Cond | Slave addr | Length | Read OP */
    mcr_val = 0x1000 | 0x4000 | ((uint32_t)(address) << 1)| 0x8000 | 0x01;

    /* program I2C_MCR */
    writel(mcr_val, I2C_REG_ADDR(bus,I2C_MCR));

    /* Wait for something in RX FIFO */
    if (until_bit_clear(I2C_REG_ADDR(bus,I2C_RISR), RISR_RXFE, I2C_TIMEOUT)) {
        i2c_abort(bus);
        return -1;
    }

    /* Flush FIFO, ACK MTD and reinit device */
    i2c_abort(bus);

    return 0;
}

#if defined(TEST_CMD_SPLIT)
void i2c_write_build(int bus, uint8_t address, uint8_t reg, uint8_t val)
{
    uint32_t mcr_val;

    writeb(reg, I2C_REG_ADDR(bus,I2C_TFR));

    writeb(val, I2C_REG_ADDR(bus,I2C_TFR));

    /* MCR register for read of two byte */
    /* Set Adress type = 7bit | Stop condition | Slave address | Length */
    mcr_val = 0x1000 | 0x4000 | ((uint32_t)(address) << 1) | 0x10000;

    i2c_mcr_built = mcr_val;
}

void i2c_write_go(int bus)
{
    /* program I2C_MCR */
    writel(i2c_mcr_built, I2C_REG_ADDR(bus,I2C_MCR));
}

int i2c_write_test(int bus)
{
    /* Wait for TX FIFO not full*/
    if (until_bit_clear(I2C_REG_ADDR(bus,I2C_RISR), RISR_TXFF, I2C_TIMEOUT)) {
        i2c_abort(bus);
        return -1;
    }

    /* ACK MTD */
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDIC);
    set_bit(I2C_REG_ADDR(bus, I2C_ICR),ICR_MTDWSIC);

    return 0;
}
#endif

int i2c_set_bus_speed(int bus, int speed)
{
    int return_speed;
    uint32_t brcnt2_val;

#ifdef CONFIG_I2C_FAST_SPEED
    uint32_t sm_value;
    uint32_t slave_setup_time;
#endif

#ifndef CONFIG_I2C_FAST_SPEED
    /* Set bus speed */
    /* Set standard mode 100K/s */
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),CR_SM_0);
    clear_bit(I2C_REG_ADDR(bus, I2C_CR),CR_SM_1);

    /* Set BRCNT2, set BRCNT1 = 0*/
    /* Baud rate (standard) = fi2cclk / ( (BRCNT2 x 2) + Foncycle) */
    /* Foncycle = 0 */
    brcnt2_val = (uint32_t) (I2C_IN_FREQ / (speed * 2) );
    writel(brcnt2_val, I2C_REG_ADDR(bus, I2C_BRCR));

    return_speed = (int) (I2C_IN_FREQ / (brcnt2_val * 2));
#else
    if( speed > I2C_MAX_FAST_PLUS_SCL)
    {
        printf("Max i2c bus_speed supported is %d \n",
                I2C_MAX_FAST_PLUS_SCL);
        speed = I2C_MAX_FAST_PLUS_SCL;
    }

    if( speed > I2C_MAX_FAST_SCL)
    {
        /* FAST PLUS */
        sm_value = 0x11;
        brcnt2_val = (uint32_t) (I2C_IN_FREQ / (speed * 3));
        return_speed = (int)(I2C_IN_FREQ / (brcnt2_val * 3));
        slave_setup_time = SLAVE_FAST_SETUP_TIME;
    }
    else if( speed > I2C_MAX_STANDARD_SCL)
    {
        /*FAST */
        sm_value = 0x01;
        brcnt2_val = (uint32_t) (I2C_IN_FREQ / (speed * 3));
        return_speed = (int)(I2C_IN_FREQ / (brcnt2_val * 3));
        slave_setup_time = SLAVE_FAST_SETUP_TIME;
    }
    else
    {
        /* STANDARD */
        sm_value = 0x00;
        brcnt2_val = (uint32_t) (I2C_IN_FREQ / (speed * 2));
        return_speed = (int)(I2C_IN_FREQ / (brcnt2_val * 2));
        slave_setup_time = SLAVE_SETUP_TIME;
    }

    /* Set baud rate */
    writel(brcnt2_val, I2C_REG_ADDR(bus, I2C_BRCR));

    /* Set speed mode */
    writel((readl(I2C_REG_ADDR(bus, I2C_CR)) & ~0x30) | (sm_value<<4), I2C_REG_ADDR(bus, I2C_CR));

    /* Set data hold time */
    writel(DATA_HOLD_TIME, I2C_REG_ADDR(bus, I2C_THDDAT));

    /* Set slave setup time */
    writel((readl(I2C_REG_ADDR(bus, I2C_SCR)) & ~0xFFFF0000) | (slave_setup_time<<16),
            I2C_REG_ADDR(bus, I2C_SCR));
#endif

    /* Save current speed */
    bus_speeds[bus] = return_speed;

    return return_speed;
}

int i2c_get_bus_speed(int bus)
{
    return(bus_speeds[bus]);
}
