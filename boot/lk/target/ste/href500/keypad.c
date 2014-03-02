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
#include <dev/uib_keypad.h>

void keypad_init(void)
{
	uint8_t keys[MAX_MULT_KEYS];
	uint8_t key_return;

	key_return = keys_pressed(keys);
	close_keypad();
	if (key_return != KEYPAD_NOT_SUPPORTED ) {
		int i;
		for(i = 0 ; i < key_return ; i++) {
			if (keys[i] == FB_MAGIC_KEY_BACK)
				keys_post_event(KEY_BACK, 1);
			if (keys[i] == FB_MAGIC_KEY_VOLUMEDOWN)
				keys_post_event(KEY_VOLUMEDOWN, 1);
			if (keys[i] == RC_MAGIC_KEY_HOME)
				keys_post_event(KEY_HOME, 1);
			if (keys[i] == RC_MAGIC_KEY_VOLUMEUP)
				keys_post_event(KEY_VOLUMEUP, 1);
		}
	} else {
		dprintf(INFO, "No keypad found\n");
	}
}
