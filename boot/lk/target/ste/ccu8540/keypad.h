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

#ifndef __KEYPAD_H
#define __KEYPAD_H

void close_keypad(void);
int keys_pressed(uint8_t *keyVal);
void keypad_init(void);

/* gpio expander chip addr */
#define I2C_STMPE1801_KEYPAD_ADDR               0x40

/* rc_key_val id */
#define STMPE1801_ID                            0

/* external keys values */
#define UNKNOWN_KEY                             0
#define FB_MAGIC_KEY_BACK                       1
#define FB_MAGIC_KEY_VOLUMEDOWN                 2
#define RC_MAGIC_KEY_HOME                       3
#define RC_MAGIC_KEY_VOLUMEUP                   4

/* keu values 0xrow_col*/
#define STMPE1801_KEY_VOLUMEUP                  0x00
#define STMPE1801_KEY_HOME                      0x01
#define STMPE1801_KEY_VOLUMEDOWN                0x10
#define STMPE1801_KEY_BACK                      0x11

/* misc */
#define KEYPAD_RETRY_ON_ERROR                   10
#define KEYPAD_RESET_DELAY                      100
#define STMPE1801_WAIT_KEY_DETECTED             150

#define KEYPAD_ERROR_ON_RESET                   0xFE
#define KEYPAD_NOT_SUPPORTED                    0xFF

#define NBR_OF_SUPPORTED_HW                     1
#define NBR_DEFINED_KEYS                        4

#define STMPE1801_NO_KEY                        0xF8

#define STMPE1801_ROW_BITS                      0x07
#define STMPE1801_ROW_SHIFT                     4
#define STMPE1801_COL_BITS                      0x78
#define STMPE1801_COL_SHIFT                     3

#define STMPE1801_MAX_KPC_DATA_BYTE_REGS        5

/* stmpe1801 register map */
#define STMPE1801_CHIP_ID_REG                   0x00
#define STMPE1801_VERSION_ID_REG                0x01
#define STMPE1801_SYS_CTRL_REG                  0x02
#define STMPE1801_KPC_ROW_REG                   0x30
#define STMPE1801_KPC_COL_LOW_REG               0x31
#define STMPE1801_KPC_CTRL_LOW_REG              0x33
#define STMPE1801_KPC_CTRL_MID_REG              0x34
#define STMPE1801_KPC_CMD_REG                   0x36
#define STMPE1801_KPC_DATA_BYTE0_REG            0x3A
#define STMPE1801_KPC_DATA_BYTE1_REG            0x3B
#define STMPE1801_KPC_DATA_BYTE2_REG            0x3C
#define STMPE1801_KPC_DATA_BYTE3_REG            0x3D
#define STMPE1801_KPC_DATA_BYTE4_REG            0x3E

/* stmpe1801 register default value */
#define STMPE1801_CHIP_ID_VAL                   0xC1
#define STMPE1801_VERSION_ID_VAL                0x10
#define STMPE1801_SYS_CTRL_VAL                  0X06

/* stmpe1801 register config value */
#define STMPE1801_RESET_VAL                     0x80
#define STMPE1801_KPC_COL_LOW_VAL               0x07
#define STMPE1801_KPC_ROW_VAL                   0x07
#define STMPE1801_KPC_CTRL_LOW_VAL              0x80
#define STMPE1801_KPC_CTRL_MID_VAL              0x80
#define STMPE1801_KPC_CMD_VAL                   0x01

#endif /* __KEYPAD_H */
