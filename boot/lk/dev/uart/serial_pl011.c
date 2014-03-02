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

#include <reg.h>			/* readl/writel */
#include "debugconfig.h"		/* DEBUG_UART */
#include "serial_pl011.h"

/*
 * XXX: make this more elegant and just include <config.h>, i.e.
 *  no if PLATFORM or TARGET in this file.
 */
#if PLATFORM_STE_X500
#include "target_config.h"

#define CLOCK	38400000
#if OPTIMIZED_TIME
#define BITRATE		921600
#else
#define BITRATE		115200
#endif

static void *const port[] = {
	(void *)U8500_UART0_BASE,
	(void *)U8500_UART1_BASE,
	(void *)U8500_UART2_BASE,
};
#elif PLATFORM_DB9600
#include <platform/db9600.h>

#define CLOCK	38400000
#define BITRATE		115200

static void *const port[] = {
	(void *)U9600_UART0_BASE,
	(void *)U9600_UART1_BASE,
	(void *)U9600_UART2_BASE,
};
#else
#error Unknown target/platform for db_shared/serial_pl011.c
#endif

void uart_init_port(int portnum, unsigned int baud)
{
	unsigned int temp;
	unsigned int divider;
	unsigned int remainder;
	unsigned int fraction;
	struct pl011_regs *regs = (struct pl011_regs *) port[portnum];

	/* Empty RX fifo if necessary */
	if (readl(&regs->cr) & CR_UARTEN) {
		while (!(readl(&regs->fr) & FR_RXFE))
			temp = readl(&regs->dr);
	}

	/* disable controller */
	writel(0, &regs->cr);

	/* set baud rate */
	temp = 16 * baud;
	divider = CLOCK / temp;
	remainder = CLOCK % temp;
	temp = (8 * remainder) / baud;
	fraction = (temp >> 1) + (temp & 1);

	writel(divider, &regs->ibrd);
	writel(fraction, &regs->fbrd);

	/* set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled */
	writel(LCRH_WLEN_8 | LCRH_FEN, &regs->lcrh);

	/* same with RX (ST-Ericsson extension to plain pl011) */
	writel(LCRH_WLEN_8 | LCRH_FEN, &regs->lcrh_rx);

	/* enable the UART */
	writel(CR_UARTEN | CR_TXE | CR_RXE,
		&regs->cr);

}

int uart_putc(int portnum, char c)
{
	struct pl011_regs *regs = (struct pl011_regs *) port[portnum];

	/* Wait until there is space in the FIFO */
	while (readl(&regs->fr) & FR_TXFF)
		;

	/* Send the character */
	writel(c, &regs->dr);

	return 0;
}

int uart_getc(int portnum, bool wait)
{
	unsigned int data;
	struct pl011_regs *regs = (struct pl011_regs *) port[portnum];

	if (wait) {
	/* Wait until there is data in the FIFO */
	while (readl(&regs->fr) & FR_RXFE)
		;
	} else if (readl(&regs->fr) & FR_RXFE) {
		return -1;
	}

	data = readl(&regs->dr);

	/* Check for an error flag */
	if (data & 0xFFFFFF00) {
		/* Clear the error */
		writel(0xFFFFFFFF, &regs->ecr);
		return -1;
	}

	return (int) data;
}

void uart_init_early(void)
{
	uart_init_port(DEBUG_UART, BITRATE);
}

void uart_init(void)
{
}
