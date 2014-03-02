/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Michael Brandt <michael.brandt@stericsson.com>
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
#include "target_config.h"

#define U8500_BACKUPRAM0_BASE	(U8500_PERIPH4_BASE + 0x00000)
#define CPU1_WAKEMAGIC_ADDR	(U8500_BACKUPRAM0_BASE + 0x1ff0)

.text
.globl __cpu_early_init

__cpu_early_init:
	/*
	 * Put core2 into loop waiting for wake up magic.
	 * Only needed for simulation environment, but doesn't harm on real HW
	 * since core2 is already doing the same as below in the bootrom.
	 */
	mrc	p15, 0, r0, c0, c0, 5
	ands	r0, r0, #0xf
	bne	core2

	/* back to calling code */
	mov	pc, lr

core2:
	ldr	r0, =CPU1_WAKEMAGIC_ADDR
	ldr	r2, =0xA1FEED01

1:	ldr	r1, [r0]
	cmp	r1, r2
	bne	1b

	/* Jump Addr is at WakeMagic + 4 */
	ldr	r1, [r0, #4]
	bx	r1

