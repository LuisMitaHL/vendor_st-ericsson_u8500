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

#include <stdlib.h>
#include <platform/timer.h>
#include "config.h"
#include "target_config.h"
#include "mcde_display.h"
#include "mcde_regs.h"
#include "mcde.h"
#include "abx500.h"
#include "prcmu.h"
#include "db_gpio.h"

struct reg_vol {
	int vol;
	int reg;
};

static struct reg_vol volreg[] = {
	{ .vol = 1100000, .reg  = 0x0, },
	{ .vol = 1200000, .reg  = 0x1, },
	{ .vol = 1300000, .reg  = 0x2, },
	{ .vol = 1400000, .reg  = 0x3, },
	{ .vol = 1500000, .reg  = 0x4, },
	{ .vol = 1800000, .reg  = 0x5, },
	{ .vol = 1850000, .reg  = 0x6, },
	{ .vol = 1900000, .reg  = 0x7, },
	{ .vol = 2500000, .reg  = 0x8, },
	{ .vol = 2650000, .reg  = 0x9, },
	{ .vol = 2700000, .reg  = 0xa, },
	{ .vol = 2750000, .reg  = 0xb, },
	{ .vol = 2800000, .reg  = 0xc, },
	{ .vol = 2900000, .reg  = 0xd, },
	{ .vol = 3000000, .reg  = 0xe, },
	{ .vol = 3300000, .reg  = 0xf, },
};

int mcde_start_dsi(void)
{
	int val;
	unsigned int i;

	val = ab8500_reg_read(/*AB8500_REGU_CTRL2, */AB8500_REGU_VAUX12_REGU_REG);
	if (val < 0)
		return -1;
	if (ab8500_reg_write(/*AB8500_REGU_CTRL2,*/
			AB8500_REGU_VAUX12_REGU_REG, val & ~MASK_LDO_VAUX1) < 0)
		return -1;
	udelay(10 * 1000);
	for (i = 0; i < ARRAY_SIZE(volreg) ; i++) {
		if (volreg[i].vol == CONFIG_SYS_DISPLAY_VOLTAGE) {
			if (ab8500_reg_write(/*AB8500_REGU_CTRL2,*/
						AB8500_REGU_VAUX1_SEL_REG,
						volreg[i].reg) < 0) {
				dprintf(CRITICAL, "%s: exit -1\n",
								__func__);
				return -1;
			}
			break;
		}
	}
	val = val & ~MASK_LDO_VAUX1;
	val = val | (1 << MASK_LDO_VAUX1_SHIFT);
	if (ab8500_reg_write(/*AB8500_REGU_CTRL2,*/
					AB8500_REGU_VAUX12_REGU_REG, val) < 0)
		return -1;
	if (ab8500_reg_write(/*AB8500_REGU_CTRL2, */AB8500_REGU_VPLLVANA_REGU_REG,
						VANA_ENABLE_IN_HP_MODE) < 0)
		return -1;
	if (ab8500_reg_write(AB8500_PWM_OUT_CTRL7_REG, ENABLE_PWM1) < 0)
		return -1;
	if (ab8500_reg_write(AB8500_PWM_OUT_CTRL1_REG, PWM_DUTY_LOW_1024_1024) < 0)
		return -1;
	if (ab8500_reg_write(AB8500_PWM_OUT_CTRL2_REG, PWM_DUTY_HI_1024_1024) < 0)
		return -1;

	if ( !(ab8500_reg_read(/*AB8500_REGU_CTRL2, */AB8500_REGU_VAUX12_REGU_REG)
				& MASK_LDO_VAUX1) ||
		(volreg[ab8500_reg_read(/*AB8500_REGU_CTRL2,*/
			AB8500_REGU_VAUX1_SEL_REG) & VAUXSEL_VOLTAGE_MASK].vol)
		!= CONFIG_SYS_DISPLAY_VOLTAGE )
		return -1;

	return 0;
}
