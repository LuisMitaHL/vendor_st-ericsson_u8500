/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#include <io.h>
#include <log.h>
#include <uart.h>
#include <stdarg.h> /* Compiler built in */

static void uart_putc(const char c)
{
	/*
	 * Wait until there is space in the FIFO
	 * We can't use wait_for_status() here as it will create a log loop.
	*/
	while (read32(CONFIG_UART_BASE + UART_FR) & UART_FR_TXFF)
		;

	/* Send the character */
	write32(CONFIG_UART_BASE + UART_DR, c);
}

static void uart_puts(const char *s)
{
	while (*s)
		uart_putc(*s++);
}

static void uart_puthex(const u32 hex)
{
	int i;

	uart_puts("0x");
	for (i = 7; i >= 0; i--) {
		u32 val = (hex >> (4 * i)) & 0xF;
		if (val <= 9)
			uart_putc('0' + val);
		else
			uart_putc('a' + (val - 10));
	}
}

static void uart_putu32(u32 val)
{
	#define BUF_LEN 12 /* 32 bit int value max length */
	char buf[BUF_LEN];
	char *str;

	/* Start from end, and fill backwards */
	str = buf + BUF_LEN - 1;
	*str = '\0';

	if (val == 0)
		*--str = '0';
	else {
		while (val) {
			*--str = (val % 10) + '0';
			val /= 10;
		}
	}

	uart_puts(str);
}

/* Do not use this function directly. Only use the macros in log.h
 *
 * This is a printf() like function. However it only takes:
 * %u for u32
 * %x (for hex dump (always prepends 0x and prints 8 digits)
 * %s for strings
 * %c for chars
 * %% for the %-sign
 *
 * %d (signed intergers) is not used supported
 * %f is not supported. Do not use floats in soc-settings at all.
 */
void _uart_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	for (; *fmt; ++fmt) {
		if (*fmt == '%') {
			++fmt;
			if (*fmt == '\0')
				break;
			else if (*fmt == '%')
				uart_putc('%');
			else if (*fmt == 's') {
				const char *s = va_arg(ap, const char *);
				uart_puts(s ? s : "(null)");
			} else if (*fmt == 'c') {
				const char c = (const char)
					va_arg(ap, const int);
				uart_putc(c);
			} else if (*fmt == 'x') {
				const u32 val = va_arg(ap, const u32);
				uart_puthex(val);
			} else if (*fmt == 'u') {
				const u32 val = va_arg(ap, const u32);
				uart_putu32(val);
			} else
				/* Unknown char after %. Print both. */
				uart_putc(*--fmt);
		} else
			uart_putc(*fmt);
	}

	va_end(ap);
}

void uart_init(void)
{
	/* Clear status/errors */
	write32(CONFIG_UART_BASE + UART_RSR_ECR, 0x0);

	/* Configure baud rate */
	write32(CONFIG_UART_BASE + UART_IBRD, CONFIG_UART_BAUD & 0xFFFF);
	write32(CONFIG_UART_BASE + UART_FBRD, CONFIG_UART_BAUD >> 16);

	/* Configure TX to 8 bits, 1 stop bit, no parity, fifo enabled. */
	write32(CONFIG_UART_BASE + UART_LCRH_TX,
		UART_LCRH_WLEN_8 | UART_LCRH_FEN);

	/* Enable UART and TX */
	write32(CONFIG_UART_BASE + UART_CR, UART_CR_UARTEN | UART_CR_TXE);

	/* Start a new line on the console */
	uart_puts("\r\n");
}
