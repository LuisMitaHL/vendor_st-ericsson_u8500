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


#ifndef UART_PL011_H
#define UART_PL011_H

#include <inttypes.h>

/*
 * ARM PrimeCell UART PL011 with ST-Ericsson extensions.
 */

struct pl011_regs {
	uint32_t dr;		/* 0x00 Data register */
	uint32_t ecr;		/* 0x04 Error clear register (Write) */
	uint32_t dmawm;		/* 0x08 DMA watermark config regiser */
	uint32_t timeout;	/* 0x0C Timeout Period register */
	uint32_t rsvd_0x10;	/* 0x10 reserved */
	uint32_t rsvd_0x14;	/* 0x14 reserved */
	uint32_t fr;		/* 0x18 Flag register (Read only) */
	uint32_t lcrh_rx;	/* 0x1C Receive Line Control register */
	uint32_t ilpr;		/* 0x20 IrDA low-power counter register */
	uint32_t ibrd;		/* 0x24 Integer baud rate register */
	uint32_t fbrd;		/* 0x28 Fractional baud rate register */
	uint32_t lcrh;		/* 0x2C Line control register */
	uint32_t cr;		/* 0x30 Control register */
};

/*
 *  PL011 register definitions
 */

#define LCRH_SPS             (1 << 7)
#define LCRH_WLEN_8          (3 << 5)
#define LCRH_WLEN_7          (2 << 5)
#define LCRH_WLEN_6          (1 << 5)
#define LCRH_WLEN_5          (0 << 5)
#define LCRH_FEN             (1 << 4)
#define LCRH_STP2            (1 << 3)
#define LCRH_EPS             (1 << 2)
#define LCRH_PEN             (1 << 1)
#define LCRH_BRK             (1 << 0)

#define CR_CTSEN             (1 << 15)
#define CR_RTSEN             (1 << 14)
#define CR_OUT2              (1 << 13)
#define CR_OUT1              (1 << 12)
#define CR_RTS               (1 << 11)
#define CR_DTR               (1 << 10)
#define CR_RXE               (1 << 9)
#define CR_TXE               (1 << 8)
#define CR_LPE               (1 << 7)
#define CR_IIRLP             (1 << 2)
#define CR_SIREN             (1 << 1)
#define CR_UARTEN            (1 << 0)

#define FR_TXFE              0x80
#define FR_RXFF              0x40
#define FR_TXFF              0x20
#define FR_RXFE              0x10
#define FR_BUSY              0x08

#endif /* UART_PL011_H */
