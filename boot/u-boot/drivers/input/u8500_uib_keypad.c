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

#include <common.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <linux/bitops.h>
#include <i2c.h>
#include <u8500_uib_keypad.h>

static u8 i2c_keypad_init_done = 0;
static u8 i2c_expander_is_TC35893 = 0;
static u8 prev_key[MAX_MULT_KEYS];

/* Define Row Col values */
u8 rc_key_val[NBR_OF_SUPPORTED_HW][NBR_DEFINED_KEYS] ={
	{STMPE_KEY_BACK, STMPE_KEY_VOLUMEDOWN, STMPE_KEY_HOME, STMPE_KEY_VOLUMEUP},
	{TC_KEY_BACK, TC_KEY_VOLUMEDOWN, TC_KEY_HOME, TC_KEY_VOLUMEUP}
};

static int get_ext_key_val(u8 keyVal)
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
	printf("Internal keypad driver not yet implemented");
	return KEYPAD_NOT_SUPPORTED;
#else
	u8 i2c_data;

	if (!i2c_keypad_init_done) {
		(void) i2c_set_bus_num(0);
		/* Probe for TC35893 used in NUIB, UIB R2, and UIB R3*/
		if (!i2c_read(I2C_TC35893_KEYPAD_ADDR, I2C_MANUFACTURE_CODE_ADDR, 1, &i2c_data, 1)) {
			/* I2C slave found, init hardware */
			i2c_data = 1; /* CLKEN.KBDEN */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_CLKEN_ADDR, 1, &i2c_data, 1);
			i2c_data = 0xff; /* KBDSETTLE */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_KBDSETTLE_ADDR, 1, &i2c_data, 1);
			i2c_data = 0xff; /* KBDBOUNCE */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_KBDBOUNCE_ADDR, 1, &i2c_data, 1);
			i2c_data = 0x88; /* KBDSIZE */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_KBDSIZE_ADDR, 1, &i2c_data, 1);
			i2c_data = 0xff; /* KBDDEDCFG */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_KBDDEDCFG_LSB_ADDR, 1, &i2c_data, 1);
			i2c_data = 0xff; /* KBDDEDCFG */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_KBDDEDCFG_MSB_ADDR, 1, &i2c_data, 1);
			i2c_data = 0xAA; /* IOPC */
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_IOPC0_MSB_ADDR, 1, &i2c_data, 1);
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_IOPC0_LSB_ADDR, 1, &i2c_data, 1);
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_IOPC1_MSB_ADDR, 1, &i2c_data, 1);
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_IOPC1_LSB_ADDR, 1, &i2c_data, 1);
			(void) i2c_write(I2C_TC35893_KEYPAD_ADDR, TC_IOPC2_MSB_ADDR, 1, &i2c_data, 1);

			/* Dummy read, first read of key code is for some reason not correct */
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE0_ADDR, 1, &i2c_data, 1);

			i2c_expander_is_TC35893 = 1;
			i2c_keypad_init_done = 1;
			return 0;
		}
		else if (!i2c_read(I2C_STMPE1601_KEYPAD_ADDR, I2C_MANUFACTURE_CODE_ADDR, 1, &i2c_data, 1)) {
			/* enable KP */
			i2c_data = 0x0A;
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_SYS_CTRL, 1, &i2c_data, 1);

			/* ALT FUNC 1 */
			i2c_data = 0x55;
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_U_MSB, 1, &i2c_data, 1);
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_U_LSB, 1, &i2c_data, 1);
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_L_MSB, 1, &i2c_data, 1);
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_GPIO_AF_L_LSB, 1, &i2c_data, 1);

			/* SCAN all cols */
			i2c_data = 0xFF;
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_COL, 1, &i2c_data, 1);

			/* SCAN all rows */
			i2c_data = 0xFF;
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_ROW_LSB, 1, &i2c_data, 1);

			/* scan count 8*/
			i2c_data = 0x80;
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_CTRL_MSB, 1, &i2c_data, 1);

			/* de-bounce 64*/
			/* start SCAN */
			i2c_data = 0x81;
			(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_CTRL_LSB, 1, &i2c_data, 1);

			prev_key[0] = NO_KEY_STMPE;
			prev_key[1] = NO_KEY_STMPE;
			prev_key[2] = NO_KEY_STMPE;

			/* Dummy read needed to catch first key */
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE0_ADDR, 1, &i2c_data, 1);
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE1_ADDR, 1, &i2c_data, 1);
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE2_ADDR, 1, &i2c_data, 1);
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE3_ADDR, 1, &i2c_data, 1);

			i2c_keypad_init_done = 1;
			i2c_expander_is_TC35893 = 0;
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
	i2c_keypad_init_done = 0;
	/* Only needed for STMPE1601 */
	if (!i2c_expander_is_TC35893) {
		u8 i2c_data;
		/* reset device */
		i2c_data = 0x80;
		(void) i2c_write(I2C_STMPE1601_KEYPAD_ADDR, STMPE_SYS_CTRL, 0, &i2c_data, 1);
	}
}

int keys_pressed(u8 *keyVal)
{
#ifdef USE_INTERNAL_KEYPAD
	printf(CRITICAL,"Internal keypad driver not yet implemented");
	return KEYPAD_NOT_SUPPORTED;
#else
	u8 i2c_data[4];
	int i;
	int nbr_of_keys=0;

	if (!init_keypad()) {
		if(i2c_expander_is_TC35893) {
			/* Read current key codes */
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE0_ADDR, 1, &i2c_data[0], 1);
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE1_ADDR, 1, &i2c_data[1], 1);
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE2_ADDR, 1, &i2c_data[2], 1);
			(void) i2c_read(I2C_TC35893_KEYPAD_ADDR, TC_KBDCODE3_ADDR, 1, &i2c_data[3], 1);

			/* filter out multiple key press detection */
			i2c_data[0] &= 0x7F;
			i2c_data[1] &= 0x7F;
			i2c_data[2] &= 0x7F;
			i2c_data[3] &= 0x7F;

			for (i=0;i<4;i++) {
				if (i2c_data[i] != 0x7F) {
					keyVal[nbr_of_keys++] = i2c_data[i];
				}
			}
			return nbr_of_keys;
		}
		else {
			/* Read all data register until now new events found */
			(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE0, 1, &i2c_data[0], 1);
			(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE1, 1, &i2c_data[1], 1);
			(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE2, 1, &i2c_data[2], 1);
			(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE3, 1, &i2c_data[3], 1);
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
				(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE0, 1, &i2c_data[0], 1);
				(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE1, 1, &i2c_data[1], 1);
				(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE2, 1, &i2c_data[2], 1);
				(void) i2c_read(I2C_STMPE1601_KEYPAD_ADDR, STMPE_KPC_DATA_BYTE3, 1, &i2c_data[3], 1);
			}
			for (i=0;i<3;i++) {
				if (prev_key[i] != NO_KEY_STMPE) {
					u8 tmpKeyVal = ((prev_key[i] & STMPE_ROW_BITS) << STMPE_ROW_SHIFT) |
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

int is_key_pressed(u8 keyVal)
{
	u8 keys[MAX_MULT_KEYS];
	int i;
	int return_val;

	return_val = keys_pressed(keys);
	if (return_val != KEYPAD_NOT_SUPPORTED) {
		for (i=0;i<return_val;i++) {
			if (keys[i] == keyVal) {
				return 0;
			}
		}
		return 1;
	}
	else {
		return return_val;
	}
}

static int do_keypad_detection(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if(!(is_key_pressed(TC_KEY_VOLUMEDOWN) && is_key_pressed(TC_KEY_BACK)))
		printf("Magic Key Detected!\n");
	else
		printf("No magic key detected!\n");

	return 0;
}

U_BOOT_CMD(
	magickey, 1, 0, do_keypad_detection,
	"Magic key detection - press VOLUMEDOWN or BACK while running the application",
);
