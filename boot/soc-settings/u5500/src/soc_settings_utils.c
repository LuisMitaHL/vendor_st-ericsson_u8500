/*
 *  Copyright (C) 2010 ST-Ericsson AB
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 *
 *
 * File Name     : soc_settings_utils.c
 * Author        : Sesahgiri.Holi <seshagiri.holi@stericsson.com>
 *
 * This file provids general and common function used within meminit.
 */
#include <t_basicdefinitions.h>
#include <uart.h>

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

/*
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
void uart_print(const char *fmt, ...)
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
void * memcpy ( void * destination, const void * source, u32 num )
{
	u8 i = 0;
	u8 *des = (u8*) destination;
	u8 *src = (u8*) source;
	for(i = 0; i < num; i++){
		*des = *src;
		des++;
		src++;
	}
	return destination;
}
