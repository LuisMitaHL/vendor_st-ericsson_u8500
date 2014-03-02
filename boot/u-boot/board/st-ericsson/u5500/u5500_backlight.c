/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Sesahgiri.Holi <seshagiri.holi@stericsson.com> for ST-Ericsson
 * License terms: GNU General Public License (GPL), version 2.
 */

#include <common.h>
#include <i2c.h>
#include <asm/arch/prcmu-db5500.h>
#include "u5500_backlight.h"

#define BACKLIGHT_SLAVE_ADDR   0x36

#define LM3530_GEN_CONFIG		0x10
#define LM3530_ALS_CONFIG		0x20
#define LM3530_BRT_RAMP_RATE		0x30
#define LM3530_ALS_ZONE_REG		0x40
#define LM3530_ALS_IMP_SELECT		0x41
#define LM3530_BRT_CTRL_REG		0xA0
#define LM3530_ALS_ZB0_REG		0x60
#define LM3530_ALS_ZB1_REG		0x61
#define LM3530_ALS_ZB2_REG		0x62
#define LM3530_ALS_ZB3_REG		0x63
#define LM3530_ALS_Z0T_REG		0x70
#define LM3530_ALS_Z1T_REG		0x71
#define LM3530_ALS_Z2T_REG		0x72
#define LM3530_ALS_Z3T_REG		0x73
#define LM3530_ALS_Z4T_REG		0x74
#define LM3530_REG_MAX			15

#define ADDR_LEN			0x01
#define DATA_LEN			0x01

/* General Control Register */
#define LM3530_EN_I2C_SHIFT		(0)
#define LM3530_RAMP_LAW_SHIFT		(1)
#define LM3530_MAX_CURR_SHIFT		(2)
#define LM3530_EN_PWM_SHIFT		(5)
#define LM3530_PWM_POL_SHIFT		(6)
#define LM3530_EN_PWM_SIMPLE_SHIFT	(7)

#define LM3530_ENABLE_I2C		(1 << LM3530_EN_I2C_SHIFT)
#define LM3530_ENABLE_PWM		(1 << LM3530_EN_PWM_SHIFT)
#define LM3530_POL_LOW			(1 << LM3530_PWM_POL_SHIFT)
#define LM3530_ENABLE_PWM_SIMPLE	(1 << LM3530_EN_PWM_SIMPLE_SHIFT)

/* ALS Config Register Options */
#define LM3530_ALS_AVG_TIME_SHIFT	(0)
#define LM3530_EN_ALS_SHIFT		(3)
#define LM3530_ALS_SEL_SHIFT		(5)

#define LM3530_ENABLE_ALS		(3 << LM3530_EN_ALS_SHIFT)

/* Brightness Ramp Rate Register */
#define LM3530_BRT_RAMP_FALL_SHIFT	(0)
#define LM3530_BRT_RAMP_RISE_SHIFT	(3)

/* ALS Resistor Select */
#define LM3530_ALS1_IMP_SHIFT		(0)
#define LM3530_ALS2_IMP_SHIFT		(4)

/* Zone Boundary Register defaults */
#define LM3530_ALS_ZB_MAX		(4)
#define LM3530_ALS_WINDOW_mV		(1000)
#define LM3530_ALS_OFFSET_mV		(4)

/* Zone Target Register defaults */
#define LM3530_DEF_ZT_0			(0x7F)
#define LM3530_DEF_ZT_1			(0x66)
#define LM3530_DEF_ZT_2			(0x4C)
#define LM3530_DEF_ZT_3			(0x33)
#define LM3530_DEF_ZT_4			(0x19)

#define LM3530_MAX_BRIGHTNESS		(0x7F)

static const u8 lm3530_reg[LM3530_REG_MAX] = {
	LM3530_GEN_CONFIG,
	LM3530_ALS_CONFIG,
	LM3530_BRT_RAMP_RATE,
	LM3530_ALS_ZONE_REG,
	LM3530_ALS_IMP_SELECT,
	LM3530_BRT_CTRL_REG,
	LM3530_ALS_ZB0_REG,
	LM3530_ALS_ZB1_REG,
	LM3530_ALS_ZB2_REG,
	LM3530_ALS_ZB3_REG,
	LM3530_ALS_Z0T_REG,
	LM3530_ALS_Z1T_REG,
	LM3530_ALS_Z2T_REG,
	LM3530_ALS_Z3T_REG,
	LM3530_ALS_Z4T_REG,
};

void backlight_init(void)
{
	u8 gen_config;
	u8 brt_ramp;
	u8 byte;
	u8 i;
	int ret = 0;
	u8 reg_val[LM3530_REG_MAX];

	/* Enable regulator for back light*/
	byte = 0x36;
	(void) prcmu_abb_write(0x3, 0x7B, &byte, (u8)sizeof(byte));

	i2c_set_bus_num(2);
	i2c_set_bus_speed(CONFIG_SYS_I2C_SPEED);

	gen_config = 0x1 << LM3530_RAMP_LAW_SHIFT |
		LM3530_FS_CURR_26mA << LM3530_MAX_CURR_SHIFT |
		LM3530_ENABLE_I2C;
	brt_ramp = (LM3530_RAMP_TIME_260ms << LM3530_BRT_RAMP_FALL_SHIFT) |
		(LM3530_RAMP_TIME_260ms << LM3530_BRT_RAMP_RISE_SHIFT);

	reg_val[0] = gen_config;	/* LM3530_GEN_CONFIG */
	reg_val[1] = 0x00;	/* LM3530_ALS_CONFIG */
	reg_val[2] = brt_ramp;		/* LM3530_BRT_RAMP_RATE */
	reg_val[3] = 0x00;		/* LM3530_ALS_ZONE_REG */
	reg_val[4] = 0x00;	/* LM3530_ALS_IMP_SELECT */
	reg_val[5] = LM3530_MAX_BRIGHTNESS;	/* LM3530_BRT_CTRL_REG */
	reg_val[6] = 0x00;		/* LM3530_ALS_ZB0_REG */
	reg_val[7] = 0x00;		/* LM3530_ALS_ZB1_REG */
	reg_val[8] = 0x00;		/* LM3530_ALS_ZB2_REG */
	reg_val[9] = 0x00;		/* LM3530_ALS_ZB3_REG */
	reg_val[10] = LM3530_DEF_ZT_0;	/* LM3530_ALS_Z0T_REG */
	reg_val[11] = LM3530_DEF_ZT_1;	/* LM3530_ALS_Z1T_REG */
	reg_val[12] = LM3530_DEF_ZT_2;	/* LM3530_ALS_Z2T_REG */
	reg_val[13] = LM3530_DEF_ZT_3;	/* LM3530_ALS_Z3T_REG */
	reg_val[14] = LM3530_DEF_ZT_4;	/* LM3530_ALS_Z4T_REG */

	for (i = 0; i < LM3530_REG_MAX; i++) {
		ret = i2c_write(BACKLIGHT_SLAVE_ADDR,
				lm3530_reg[i], ADDR_LEN, &reg_val[i],
				DATA_LEN);
		if (ret)
			break;
	}
	return;
}
