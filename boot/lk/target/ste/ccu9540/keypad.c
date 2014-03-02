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

#include <dev/keys.h>
#include <dev/gpio_keypad.h>

static unsigned int rows[] = {
	67, 66, 79
};

static unsigned int cols[] = {
	65, 64, 78
};

#define KEY(row, col, val) \
	[(col) * ARRAY_SIZE((rows)) + (row)] = (val)

static uint16_t keymap[] = {
	KEY(0, 0, KEY_VOLUMEUP),
	KEY(0, 1, KEY_HOME),
	KEY(0, 2, KEY_F2),

	KEY(1, 0, KEY_VOLUMEDOWN),
	KEY(1, 1, KEY_BACK),
	KEY(1, 2, KEY_F4),

	/* KEY(6, 0, xxx) not used */
	KEY(2, 1, 0), /* CAMERA_FOCUS; currently not defined in LK */
	KEY(2, 2, 0), /* CAMERA; currently not defined in LK */
};

static struct gpio_keypad_info keypad = {
	.keymap = keymap,
	.output_gpios = cols,
	.input_gpios = rows,
	.noutputs = ARRAY_SIZE(cols),
	.ninputs = ARRAY_SIZE(rows),
	.settle_time = 5, /* ms */
	.poll_time = 20,  /* ms */
	.flags = GPIOKPF_DRIVE_INACTIVE | GPIOKPF_INPUT_PULL_INACTIVE
};

void keypad_init(void)
{
	gpio_keypad_init(&keypad);
}
