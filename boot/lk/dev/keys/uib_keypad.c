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

#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <platform.h>
#include <dev/uib_keypad.h>
#include "i2c.h"
#include "target_config.h"
#include "config.h"
#include "platform_p.h"

static uint8_t	i2c_keypad_init_done = false;
static uint8_t	i2c_expander_is_TC35893 = false;
static uint8_t prev_key[MAX_MULT_KEYS];

/* Defiene Row Col values */
uint8_t rc_key_val[NBR_OF_SUPPORTED_HW][NBR_DEFINED_KEYS] ={
	{STMPE_KEY_BACK, STMPE_KEY_VOLUMEDOWN, STMPE_KEY_HOME, STMPE_KEY_VOLUMEUP},
	{TC_KEY_BACK, TC_KEY_VOLUMEDOWN, TC_KEY_HOME, TC_KEY_VOLUMEUP}
	};

static int get_ext_key_val(uint8_t keyVal)
{
	int i;
	for (i=0;i<NBR_DEFINED_KEYS;i++) {
		if (rc_key_val[i2c_expander_is_TC35893][i] == keyVal) {
			return i+1;
		}
	}
	return UNKNOWN_KEY;
}

static int init_keypad(void)
{
#ifdef USE_INTERNAL_KEYPAD
	dprintf(CRITICAL,"Internal keypad driver not yet implemented");
	return KEYPAD_NOT_SUPPORTED;
#else
	uint8_t i2c_data;

	if (!i2c_keypad_init_done) {
		/* Probe for TC35893 used in NUIB, UIB R2, and UIB R3*/
		if (!i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, I2C_MANUFACTURE_CODE_ADDR, &i2c_data)) {
			/* I2C slave found, init hardware */
			i2c_data = 1; /* CLKEN.KBDEN */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_CLKEN_ADDR, i2c_data);
			i2c_data = 0xff; /* KBDSETTLE */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDSETTLE_ADDR, i2c_data);
			i2c_data = 0xff; /* KBDBOUNCE */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDBOUNCE_ADDR, i2c_data);
			i2c_data = 0x88; /* KBDSIZE */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDSIZE_ADDR, i2c_data);
			i2c_data = 0xff; /* KBDDEDCFG */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDDEDCFG_LSB_ADDR, i2c_data);
			i2c_data = 0xff; /* KBDDEDCFG */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDDEDCFG_MSB_ADDR, i2c_data);
			i2c_data = 0xAA; /* IOPC */
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_IOPC0_MSB_ADDR, i2c_data);
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_IOPC0_LSB_ADDR, i2c_data);
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_IOPC1_MSB_ADDR, i2c_data);
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_IOPC1_LSB_ADDR, i2c_data);
			i2c_write_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_IOPC2_MSB_ADDR, i2c_data);

			/* Dummy read, first read of key code is for some reason not correct */
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE0_ADDR, &i2c_data);

			i2c_expander_is_TC35893 = true;
			i2c_keypad_init_done = true;
			return 0;
		}
		else if (!i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, I2C_MANUFACTURE_CODE_ADDR, &i2c_data)) {
			/* enable KP */
			i2c_data = 0x0A;
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_SYS_CTRL, i2c_data);

			/* ALT FUNC 1 */
			i2c_data = 0x55;
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_U_MSB, i2c_data);
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_U_LSB, i2c_data);
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_L_MSB, i2c_data);
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_L_LSB, i2c_data);

			/* SCAN all cols */
			i2c_data = 0xFF;
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_COL, i2c_data);

			/* SCAN all rows */
			i2c_data = 0xFF;
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_ROW_LSB, i2c_data);

			/* scan count 8*/
			i2c_data = 0x80;
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_CTRL_MSB, i2c_data);

			/* de-bounce 64*/
			/* start SCAN */
			i2c_data = 0x81;
			i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_CTRL_LSB, i2c_data);

			prev_key[0] = NO_KEY_STMPE;
			prev_key[1] = NO_KEY_STMPE;
			prev_key[2] = NO_KEY_STMPE;

			/* Dummy read needed to catch first key */
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE0_ADDR, &i2c_data);
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE1_ADDR, &i2c_data);
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE2_ADDR, &i2c_data);
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE3_ADDR, &i2c_data);

			i2c_keypad_init_done = true;
			i2c_expander_is_TC35893 = false;
			return 0;
		}
		else {
			return KEYPAD_NOT_SUPPORTED;
		}
	}
	else {
		return 0;
	}
#endif
}

void close_keypad(void)
{
	i2c_keypad_init_done = false;
	/* Only needed for STMPE1601 */
	if (!i2c_expander_is_TC35893) {
		uint8_t i2c_data;
		/* reset device */
		i2c_data = 0x80;
		i2c_write_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_SYS_CTRL, i2c_data);
	}
}

int keys_pressed(uint8_t *keyVal)
{
#ifdef USE_INTERNAL_KEYPAD
	dprintf(CRITICAL,"Internal keypad driver not yet implemented");
	return KEYPAD_NOT_SUPPORTED;
#else
	uint8_t i2c_data[4];
	int i;
	int nbr_of_keys=0;

	if (!init_keypad()) {
		if(i2c_expander_is_TC35893) {
			/* Read current key codes */
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE0_ADDR, &i2c_data[0]);
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE1_ADDR, &i2c_data[1]);
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE2_ADDR, &i2c_data[2]);
			i2c_read_reg(I2C_0, I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE3_ADDR, &i2c_data[3]);

			/* filter out multiple key press detection */
			i2c_data[0] &= 0x7F;
			i2c_data[1] &= 0x7F;
			i2c_data[2] &= 0x7F;
			i2c_data[3] &= 0x7F;

			for (i=0;i<4;i++) {
				if (i2c_data[i] != 0x7F) {
					keyVal[nbr_of_keys++] = get_ext_key_val(i2c_data[i]);
				}
			}
			return nbr_of_keys;
		}
		else {
			/* Read all data register until now new events found */
			i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE0, &i2c_data[0]);
			i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE1, &i2c_data[1]);
			i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE2, &i2c_data[2]);
			i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE3, &i2c_data[3]);
			while ( (i2c_data[0] != NO_KEY_STMPE)  || (i2c_data[1] != NO_KEY_STMPE)
				|| (i2c_data[2] != NO_KEY_STMPE)) {
				/* Copy to memory if key down*/
				for (i=0;i<3;i++) {
					if (i2c_data[i] & 0x80) {
						prev_key[i] = NO_KEY_STMPE;
					}
					else {
						prev_key[i] = i2c_data[i];
					}
				}
				i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE0, &i2c_data[0]);
				i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE1, &i2c_data[1]);
				i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE2, &i2c_data[2]);
				i2c_read_reg(I2C_0, I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE3, &i2c_data[3]);
			}
			for (i=0;i<3;i++) {
				if (prev_key[i] != NO_KEY_STMPE) {
					uint8_t tmpKeyVal = ((prev_key[i] & STMPE_ROW_BITS) << STMPE_ROW_SHIFT) |
						(prev_key[i] & STMPE_COL_BITS);
					keyVal[nbr_of_keys++] = get_ext_key_val(tmpKeyVal);
				}
			}
			return nbr_of_keys;
		}
	}
	else {
		return KEYPAD_NOT_SUPPORTED;
	}
#endif
}

int is_key_pressed(uint8_t keyVal)
{
	uint8_t keys[MAX_MULT_KEYS];
	int i;
	int return_val;

	return_val = keys_pressed(keys);
	if (return_val != KEYPAD_NOT_SUPPORTED) {
		for (i=0;i<return_val;i++) {
			if (keys[i] == keyVal) {
				return true;
			}
		}
		return false;
	}
	else {
		return return_val;
	}
}

