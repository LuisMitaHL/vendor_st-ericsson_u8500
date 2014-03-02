/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "issw_printf.h"
#include "xloader_pl011.h"

static void issw_putc(char c)
{
    UART_PUTC(c);
}

void issw_puts(const char *s)
{
    while (*s) {
        if (*s == '\n')
            issw_putc('\r');
        issw_putc(*s++);
    }
}

void issw_put_hex32(t_uint32 v, int width)
{
    char s[8];
    int  i;
    static const char hex_digits[] = "0123456789ABCDEF";
#define MAX_WIDTH 8
#define BASE 16

    if (width > MAX_WIDTH)
        width = MAX_WIDTH;

    for (i = 0; i < width && v != 0; i++) {
        s[i] = hex_digits[v % BASE];
        v /= BASE;
    }

    if (i == 0)
        issw_putc(hex_digits[0]);
    else {
        for (i--; i >= 0 ; i--)
            issw_putc(s[i]);
    }
}

static void issw_vprintf(const char *fmt, va_list ap)
{
    int      ch;

    for ( ;; ) {
        while ((ch = *fmt++) != '%') {
            if (ch == '\0')
                return;
            if (ch == '\n')
                issw_putc('\r');
            issw_putc(ch);
        }

        ch = *fmt++;
        switch (ch) {
        case 's':
            issw_puts(va_arg(ap, char *));
            break;
        case 'x':
            issw_put_hex32(va_arg(ap, t_uint32), 8);
            break;
        case 'd':
        case 'p':
            issw_putc('0');
            issw_putc('x');
            issw_put_hex32(va_arg(ap, t_uint32), 8);
        break;
        default:
            issw_putc(ch);
        }
    }
}

void issw_printf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    issw_vprintf(fmt, ap);
    va_end(ap);
}

void issw_hex_print_buf(const t_uint8 *buf, t_uint32 bsize)
{
    t_uint32 i;

    for (i = 0; i < bsize; i++) {
        issw_put_hex32(buf[i], 2);
        issw_puts(" ");
    }
    issw_puts("\n");
}

int issw_printf_init(void)
{
    return UART_INIT();
}
