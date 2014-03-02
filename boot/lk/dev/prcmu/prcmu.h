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

#ifndef __PRCMU_H__
#define __PRCMU_H__

#include <stdlib.h>

/* Init PRCMU relatated things in db8500 SoC platform */
void db8500_prcmu_init(void);

/**
 * prcmu_i2c_read - PRCMU - 4500 communication using PRCMU I2C
 * @reg: - db8500 register bank to be accessed
 * @slave:  - db8500 register to be accessed
 * Returns: ACK_MB5  value containing the status
 */
int prcmu_i2c_read(uint8_t reg, uint16_t slave);

/**
 * prcmu_i2c_write - PRCMU-db8500 communication using PRCMU I2C
 * @reg: - db8500 register bank to be accessed
 * @slave:  - db800 register to be written to
 * @reg_data: - the data to write
 * Returns: ACK_MB5 value containing the status
 */
int prcmu_i2c_write(uint8_t reg, uint16_t slave, uint8_t reg_data);

/**
 * prcmu_sysclk_request - PRCMU-db8500 sysclock request
 * @enable: - enable or disable sysclock
 * Returns: ACK_MB0 value containing the status
 */
int prcmu_sysclk_request(bool enable);

void prcmu_init(void);

void prcmu_set_psw(bool status);

#define ab8500_read			prcmu_i2c_read
#define ab8500_write		prcmu_i2c_write
#define ab8500_sysclk_req	prcmu_sysclk_request
#endif /* __PRCMU_H__ */
