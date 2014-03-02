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
#include <dev/keys.h>
#include <kernel/thread.h>
#include "timer.h"
#include "i2c.h"
#include "keypad.h"

static uint8_t i2c_keypad_init_done = false;
static uint8_t i2c_expander_id = NBR_OF_SUPPORTED_HW;

/* Define Row Col values */
uint8_t rc_key_val[NBR_OF_SUPPORTED_HW][NBR_DEFINED_KEYS] = {
    {STMPE1801_KEY_BACK, STMPE1801_KEY_VOLUMEDOWN,
     STMPE1801_KEY_HOME, STMPE1801_KEY_VOLUMEUP},
};

static int get_ext_key_val(uint8_t keyVal)
{
    int i;
    for (i = 0; i < NBR_DEFINED_KEYS; i++) {
        if (rc_key_val[i2c_expander_id][i] == keyVal)
            return i+1;
    }
    return UNKNOWN_KEY;
}

static int init_keypad(void)
{
#ifdef USE_INTERNAL_KEYPAD
    dprintf(CRITICAL, "Internal keypad driver not yet implemented");
    return KEYPAD_NOT_SUPPORTED;
#else
    uint8_t i2c_chip_id;
    uint8_t i2c_version_id;
    uint8_t i2c_data;
    uint32_t loop;

    if (!i2c_keypad_init_done) {
        if (!i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_CHIP_ID_REG, &i2c_chip_id)
            && !i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                    STMPE1801_VERSION_ID_REG, &i2c_version_id)
            && (i2c_chip_id == STMPE1801_CHIP_ID_VAL)
            && (i2c_version_id == STMPE1801_VERSION_ID_VAL)) {
            /* reset device */
            i2c_data = STMPE1801_RESET_VAL;
            i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_SYS_CTRL_REG, i2c_data);
            loop = KEYPAD_RETRY_ON_ERROR;
            while (loop != 0) {
                if (!i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                         STMPE1801_SYS_CTRL_REG, &i2c_data)
                    && (i2c_data == STMPE1801_SYS_CTRL_VAL))
                    break;

                udelay(KEYPAD_RESET_DELAY);
                loop--;
            }
            if (loop == 0)
                return KEYPAD_ERROR_ON_RESET;

            /* scan 3 cols (column 0-2) */
            i2c_data = STMPE1801_KPC_COL_LOW_VAL;
            i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_KPC_COL_LOW_REG, i2c_data);

            /* scan 3 rows (row 0-2) */
            i2c_data = STMPE1801_KPC_ROW_VAL;
            i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_KPC_ROW_REG, i2c_data);

            /* scan count 8 */
            i2c_data = STMPE1801_KPC_CTRL_LOW_VAL;
            i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_KPC_CTRL_LOW_REG, i2c_data);

            /* de-bounce 64*/
            i2c_data = STMPE1801_KPC_CTRL_MID_VAL;
            i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_KPC_CTRL_MID_REG, i2c_data);

            /* start scan */
            i2c_data = STMPE1801_KPC_CMD_VAL;
            i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                 STMPE1801_KPC_CMD_REG, i2c_data);

            thread_sleep(STMPE1801_WAIT_KEY_DETECTED);

            i2c_keypad_init_done = true;
            i2c_expander_id = STMPE1801_ID;
            return 0;
        } else
            return KEYPAD_NOT_SUPPORTED;
    } else
        return 0;
#endif
}

void close_keypad(void)
{
    if (i2c_expander_id == STMPE1801_ID) {
        uint8_t i2c_data;

        /* reset device */
        i2c_data = STMPE1801_RESET_VAL;
        i2c_write_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                             STMPE1801_SYS_CTRL_REG, i2c_data);
    }

    i2c_keypad_init_done = false;
    i2c_expander_id = NBR_OF_SUPPORTED_HW;
}

int keys_pressed(uint8_t *keyVal)
{
#ifdef USE_INTERNAL_KEYPAD
    dprintf(CRITICAL, "Internal keypad driver not yet implemented");
    return KEYPAD_NOT_SUPPORTED;
#else
    uint8_t i2c_data[STMPE1801_MAX_KPC_DATA_BYTE_REGS];
    int i;
    int nbr_of_keys = 0;

    if (!init_keypad()) {
        if (i2c_expander_id == STMPE1801_ID) {
            /* Read current key codes */
            i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                STMPE1801_KPC_DATA_BYTE0_REG, &i2c_data[0]);
            i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                STMPE1801_KPC_DATA_BYTE1_REG, &i2c_data[1]);
            i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                STMPE1801_KPC_DATA_BYTE2_REG, &i2c_data[2]);
            i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                STMPE1801_KPC_DATA_BYTE3_REG, &i2c_data[3]);
            i2c_read_reg(I2C_1, I2C_STMPE1801_KEYPAD_ADDR,
                                STMPE1801_KPC_DATA_BYTE4_REG, &i2c_data[4]);
            for (i = 0; i < 3; i++) {
                if (i2c_data[i] != STMPE1801_NO_KEY) {
                    uint8_t tmpKeyVal = ((i2c_data[i] & STMPE1801_ROW_BITS)
                                                    << STMPE1801_ROW_SHIFT)
                                        | ((i2c_data[i] & STMPE1801_COL_BITS)
                                                    >> STMPE1801_COL_SHIFT);
                    keyVal[nbr_of_keys++] = get_ext_key_val(tmpKeyVal);
                }
            }
            return nbr_of_keys;
        } else
            return KEYPAD_NOT_SUPPORTED;
    } else
        return KEYPAD_NOT_SUPPORTED;
#endif
}

void keypad_init(void)
{
    uint8_t keys[NBR_DEFINED_KEYS];
    uint8_t key_return;

    key_return = keys_pressed(keys);
    close_keypad();
    if (key_return != KEYPAD_NOT_SUPPORTED) {
        int i;
        for(i = 0 ; i < key_return; i++) {
            if (keys[i] == FB_MAGIC_KEY_BACK)
                keys_post_event(KEY_BACK, 1);
            if (keys[i] == FB_MAGIC_KEY_VOLUMEDOWN)
                keys_post_event(KEY_VOLUMEDOWN, 1);
            if (keys[i] == RC_MAGIC_KEY_HOME)
                keys_post_event(KEY_HOME, 1);
            if (keys[i] == RC_MAGIC_KEY_VOLUMEUP)
                keys_post_event(KEY_VOLUMEUP, 1);
        }
    } else
        dprintf(CRITICAL, "No keypad found\n");
}
