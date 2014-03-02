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

#ifndef _I2C_H_
#define _I2C_H_

void i2c_init(int bus);
int i2c_read_reg(int bus, uint8_t address, uint8_t reg, uint8_t *val);
int i2c_write_reg(int bus, uint8_t address, uint8_t reg, uint8_t val);
int i2c_set_bus_speed(int bus, int speed);
int i2c_get_bus_speed(int bus);
int i2c_probe(int bus, uint8_t address);
int i2c_read(int bus, uint8_t address, bool StopCondition, uint8_t *buffer, int len);
int i2c_write(int bus, uint8_t address, bool StopCondition, uint8_t *buffer, int len);

#if defined(TEST_CMD_SPLIT)
void i2c_write_build(int bus, uint8_t address, uint8_t reg, uint8_t val);
void i2c_write_go(int bus);
int i2c_write_test(int bus);
#endif

#define I2C_IN_FREQ			24000000
#define FLUSH_TIMEOUT			10000

/* Slave data setup time, 250ns for 24MHz i2c_clk */
#define SLAVE_SETUP_TIME		6

#ifdef CONFIG_I2C_FAST_SPEED
/* Slave data setup time, 100ns for 24MHz i2c_clk */
#define SLAVE_FAST_SETUP_TIME		3
/* data hold time for 24MHz i2c_clk */
#define DATA_HOLD_TIME			9

/* Max clock (Hz) for Standard Mode */
#define I2C_MAX_STANDARD_SCL		100000
/* Max clock (Hz) for Fast Mode */
#define I2C_MAX_FAST_SCL		400000
/* Max clock (Hz) for Fast Plus Mode */
#define I2C_MAX_FAST_PLUS_SCL		1600000
#endif

#define I2C_CR				(0x00)
#define I2C_SCR				(0x04)
#define I2C_HSMCR			(0x08)
#define I2C_MCR				(0x0C)
#define I2C_TFR				(0x10)
#define I2C_SR				(0x14)
#define I2C_RFR				(0x18)
#define I2C_TFTR			(0x1C)
#define I2C_RFTR			(0x20)
#define I2C_DMAR			(0x24)
#define I2C_BRCR			(0x28)
#define I2C_IMSCR			(0x2C)
#define I2C_RISR			(0x30)
#define I2C_ICR				(0x38)
#define I2C_THDDAT			(0x4C)

/* CR BIT PATTERN */
#define CR_PE				(0x1)
#define CR_OM_0				(0x2)
#define CR_OM_1				(0x4)
#define CR_SM_0				(0x10)
#define CR_SM_1				(0x20)
#define CR_FTX				(0x80)
#define CR_FRX				(0x100)

/* RISR BIT PATTERN */
#define RISR_TXFF			(0x4)
#define RISR_RXFE			(0x10)
#define RISR_MTD			(0x80000)
#define RISR_MTDWS			(0x10000000)

/* ICR BIT PATTERN */
#define ICR_MTDIC			(0x80000)
#define ICR_MTDWSIC			(0x10000000)

#endif	/* _I2C_H_ */

/* I2C BUS */
#define I2C_0				(0)
#define I2C_1				(1)

/* I2C REGISTER MAP */
#define I2C_MANUFACTURE_CODE_ADDR	(0x80)

