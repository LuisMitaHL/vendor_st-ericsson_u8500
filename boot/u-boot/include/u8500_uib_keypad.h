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

#ifndef __U8500_UIB_KEYPAD_H
#define __U8500_UIB_KEYPAD_H

int is_key_pressed(u8 keyVal);
int keys_pressed(u8 *keyVal);
void close_keypad(void);

#define NBR_OF_SUPPORTED_HW			2
#define NBR_DEFINED_KEYS			4

/* EXTERNAL KEY VALUES */
#define FB_MAGIC_KEY_BACK			1
#define FB_MAGIC_KEY_VOLUMEDOWN 		2
#define RC_MAGIC_KEY_HOME			3
#define RC_MAGIC_KEY_VOLUMEUP			4
#define UNKNOWN_KEY				0

#define I2C_TC35893_KEYPAD_ADDR 		0x44
#define I2C_STMPE1601_KEYPAD_ADDR 		0x40
#define I2C_MANUFACTURE_CODE_ADDR		0x80

/* KEY VALUES 0xROW_COL*/
#define TC_KEY_BACK				0x05
#define TC_KEY_VOLUMEDOWN 			0x64
#define TC_KEY_HOME 				0x41
#define TC_KEY_VOLUMEUP 			0x62

#define STMPE_KEY_BACK				0x62
#define STMPE_KEY_VOLUMEDOWN			0x35
#define STMPE_KEY_HOME				0x41
#define STMPE_KEY_VOLUMEUP			0x42

/* MISC */
#define MAX_MULT_KEYS				4
#define KEYPAD_NOT_SUPPORTED			0xFF

#define NO_KEY_STMPE				0xF8
#define NO_KEY_TC				0x7F

#define STMPE_ROW_BITS				0x38
#define STMPE_ROW_SHIFT 			1
#define STMPE_COL_BITS				0x07

/* I2C REGISTER MAP TC35893*/
#define TC_KBDSETTLE_ADDR			(0x01)
#define TC_KBDBOUNCE_ADDR			(0x02)
#define TC_KBDSIZE_ADDR 			(0x03)
#define TC_KBDDEDCFG_MSB_ADDR			(0x04)
#define TC_KBDDEDCFG_LSB_ADDR			(0x05)
#define TC_KBDCODE0_ADDR			(0x0B)
#define TC_KBDCODE1_ADDR			(0x0C)
#define TC_KBDCODE2_ADDR			(0x0D)
#define TC_KBDCODE3_ADDR			(0x0E)
#define TC_IOPC0_MSB_ADDR			(0xAA)
#define TC_IOPC0_LSB_ADDR			(0xAB)
#define TC_IOPC1_MSB_ADDR			(0xAC)
#define TC_IOPC1_LSB_ADDR			(0xAD)
#define TC_IOPC2_MSB_ADDR			(0xAE)
#define TC_CLKEN_ADDR				(0x8A)

/* I2C REGISTER MAP STMPE1601*/
#define STMPE_SYS_CTRL				(0x02)
#define STMPE_GPIO_AF_U_MSB			(0x92)
#define STMPE_GPIO_AF_U_LSB			(0x93)
#define STMPE_GPIO_AF_L_MSB			(0x94)
#define STMPE_GPIO_AF_L_LSB			(0x95)
#define STMPE_KPC_COL				(0x60)
#define STMPE_KPC_ROW_LSB			(0x62)
#define STMPE_KPC_CTRL_MSB			(0x63)
#define STMPE_KPC_CTRL_LSB			(0x64)
#define STMPE_KPC_DATA_BYTE0			(0x68)
#define STMPE_KPC_DATA_BYTE1			(0x69)
#define STMPE_KPC_DATA_BYTE2			(0x6A)
#define STMPE_KPC_DATA_BYTE3			(0x6B)

#endif /* __KEYPAD_H */
