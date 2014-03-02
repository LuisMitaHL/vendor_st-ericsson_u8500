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

#include <crc32.h>
#include <stdio.h>
#include <string.h>

unsigned long crc32_lookup[256];

void init_crc32(void)
{
	uint32_t c;
	int i, j;

	for (i = 0; i < 256; i++)
	{
		c = i;
		for (j = 8; j; j--)
			c = (c&1) ? ((c >> 1) ^ 0xedb88320) : (c >> 1);
		crc32_lookup[i] = c;
	}
}

uint32_t calc_crc32(uint8_t *data, uint32_t length)
{
	uint32_t crc = 0xffffffff;

	while (length-- !=0)
		crc = crc32_lookup[((uint8_t) crc ^ *(data++))] ^ (crc >> 8);
	return (~crc);
}
