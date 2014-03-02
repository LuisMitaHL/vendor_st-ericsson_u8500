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
#include <kernel/thread.h>
#include <platform.h>
#include <platform/interrupts.h>
#include <platform/timer.h>
#include "target_config.h"
#include "platform_p.h"
#include <platform/irqs.h>
#include "boottime.h"

/*
 * The MTU device hosts four different counters, with 4 sets of
 * registers.
 */

struct mtu_regs {
	uint32_t imsc;	/* interrupt mask set/clear */
	uint32_t ris;	/* raw interrupt status */
	uint32_t mis;	/* masked interrupt status */
	uint32_t icr;	/* interrupt clear register */
	struct {
		uint32_t lr;	/* load register */
		uint32_t val;	/* current value */
		uint32_t cr;	/* control register */
		uint32_t bglr;	/* background load register */
	} mtu[4];
};

/* bits for the control register */
#define MTU_CR_ENA		0x80
#define MTU_CR_PERIODIC		0x40	/* if 0 = free-running */
#define MTU_CR_PRESCALE_MASK	0x0c
#define MTU_CR_PRESCALE_256	0x08
#define MTU_CR_PRESCALE_16	0x04
#define MTU_CR_PRESCALE_1	0x00
#define MTU_CR_32BITS		0x02
#define MTU_CR_ONESHOT		0x01	/* if 0 = wraps reloading from BGLR*/

/*
 * The MTU is clocked at 133 MHz by default. (V1 and later)
 */
#define TIMER_TICK_RATE		(133 * 1000 * 1000 / 16)
#define USEC_TO_COUNT(x)	((x) * 133 / 16)
#define COUNT_TO_USEC(x)	((x) * 16 / 133)

/*
 * MTU timer to use (from 0 to 3).
 * Ux500 Linux uses timer0 on MTU0 and timer0 on MTU1
 */
#define DELAY_TIMER 1	/* udelay */
#define TICK_TIMER 2	/* periodic tick timer */

static platform_timer_callback timer_callback;
static void *timer_arg;
static time_t timer_interval;
static volatile time_t ticks;

static struct mtu_regs *regs = (struct mtu_regs *) U8500_MTU0_BASE;


static enum handler_return timer_irq(void *arg)
{
	ticks += timer_interval;
	/* ack MTU0 irq */
	writel(1 << TICK_TIMER, &regs->icr);

	return timer_callback(timer_arg, ticks);
}


status_t platform_set_periodic_timer(platform_timer_callback callback,
		void *arg, time_t interval)
{
	uint32_t ticks_per_interval;
	uint32_t reg;

	enter_critical_section();

	writel(0, &regs->mtu[TICK_TIMER].cr);	/* off */
	writel(1 << TICK_TIMER, &regs->imsc);

	timer_callback = callback;
	timer_arg = arg;
	timer_interval = interval;
	/* divide by 1000 because interval is in ms */
	ticks_per_interval = (uint64_t)interval * TIMER_TICK_RATE / 1000;

	writel(ticks_per_interval, &regs->mtu[TICK_TIMER].lr);
	writel(ticks_per_interval, &regs->mtu[TICK_TIMER].bglr);
	reg = MTU_CR_PERIODIC | MTU_CR_PRESCALE_16 | MTU_CR_32BITS;
	writel(reg, &regs->mtu[TICK_TIMER].cr);
	writel(reg | MTU_CR_ENA, &regs->mtu[TICK_TIMER].cr);

	register_int_handler(IRQ_MTU0, timer_irq, 0);
	unmask_interrupt(IRQ_MTU0);

	exit_critical_section();

	return NO_ERROR;
}

time_t current_time(void)
{
	return ticks;
}

bigtime_t current_time_hires(void)
{
	return (bigtime_t)current_time();
}

void platform_init_timer(void)
{
	uint32_t reg;

	/* MTU */
	/* initialise delay timer */
	reg = MTU_CR_ENA | MTU_CR_PRESCALE_16 | MTU_CR_32BITS;
	writel(reg, &regs->mtu[DELAY_TIMER].cr);
	writel(reg | MTU_CR_ENA, &regs->mtu[DELAY_TIMER].cr);

	/* restart decrementing from 0 */
	writel(0, &regs->mtu[DELAY_TIMER].lr);
}

void platform_halt_timers(void)
{
}

/* macro to read the 32 bit timer: since it decrements, we invert read value */
#define READ_TIMER() (~readl(&regs->mtu[DELAY_TIMER].val))

void udelay(unsigned usecs)
{
	unsigned long ini, end;

	ini = READ_TIMER();
	end = ini + USEC_TO_COUNT(usecs);
	while ((signed long)(end - READ_TIMER()) > 0)
		;

	boottime_idle_add(usecs);
}

#define RTC_CLK 			(uint64_t)(32768)
#define NANOSEC_PER_TICK 	(uint64_t)(1000000000/RTC_CLK)


/* Use PRCMU RTC, 1/32768 resolution. Good enough for boottime */
uint32_t get_timer_us(void)
{
	uint32_t c1;
	uint32_t c2;
	uint64_t time;

	do {
		c1 = readl(PRCM_TIMER4_DOWNCOUNT);
		c2 = readl(PRCM_TIMER4_DOWNCOUNT);
	} while (c2 != c1);

	/* Negate because the timer is a decrementing counter */
	c1 = ~c1;

	/* Calc nS, for best resolution */
	time = c1 * NANOSEC_PER_TICK;

	/* Convert to uS for return */
	time /= 1000;

	return (uint32_t)time;
}
