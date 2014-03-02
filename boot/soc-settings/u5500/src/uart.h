/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __UART_H__
#define __UART_H__

#include <stdarg.h> /* Compiler built in */

/*
 * We won't use writel and readl here since definition of
 * writel(value, addr) is backwards
 */

#define write32(addr, value)	(*(volatile u32 *)(addr) = (u32)(value))
#define read32(addr)		(*(volatile u32 *)(addr))

#define clrbits32(addr, clear) \
	write32((addr), read32(addr) & ~(u32)(clear))
#define setbits32(addr, set) \
	write32((addr), read32(addr) | (u32) (set))


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


#define U5500_PER1_BASE 0xA0020000
#define U5500_PER5_BASE 0x80100000

#define U5500_UART0_BASE (U5500_PER1_BASE + 0x3000)
#define U5500_UART1_BASE (U5500_PER5_BASE + 0x4000)
#define U5500_UART2_BASE (U5500_PER5_BASE + 0x5000)
#define U5500_UART3_BASE (U5500_PER5_BASE + 0x6000)

#define CONFIG_UART_BASE U5500_UART0_BASE
#define CONFIG_UART_BAUD	0x350014

#define UART_DR			0x00 /* data register */
#define UART_RSR_ECR		0x04 /* receive status or error clear */
#define UART_DMAWM		0x08 /* DMA watermark configure */
#define UART_TIMEOUT		0x0C /* Timeout period */
				/* reserved space */
#define UART_FR			0x18 /* flag register */
#define UART_LCRH_RX		0x1C /*receive line control */
#define UART_ILPR		0x20 /* IrDA low-poer */
#define UART_IBRD		0x24 /* integer baud register */
#define UART_FBRD		0x28 /* fractional baud register */
#define UART_LCRH_TX		0x2C /* transmit line control */
#define UART_CR			0x30 /* control register */
#define UART_IFLS		0x34 /* interrupt FIFO level select */
#define UART_IMSC		0x38 /* interrupt mask set/clear */
#define UART_RIS		0x3C /* raw interrupt register */
#define UART_MIS		0x40 /* masked interrupt register */
#define UART_ICR		0x44 /* interrupt clear register */
#define UART_DMACR		0x48 /* DMA control register */

/* flag register bits */
#define UART_FR_RTXDIS	(1 << 13)
#define UART_FR_TERI	(1 << 12)
#define UART_FR_DDCD	(1 << 11)
#define UART_FR_DDSR	(1 << 10)
#define UART_FR_DCTS	(1 << 9)
#define UART_FR_RI	(1 << 8)
#define UART_FR_TXFE	(1 << 7)
#define UART_FR_RXFF	(1 << 6)
#define UART_FR_TXFF	(1 << 5)
#define UART_FR_RXFE	(1 << 4)
#define UART_FR_BUSY	(1 << 3)
#define UART_FR_DCD	(1 << 2)
#define UART_FR_DSR	(1 << 1)
#define UART_FR_CTS	(1 << 0)

/* transmit/recieve line register bits */
#define UART_LCRH_SPS		(1 << 7)
#define UART_LCRH_WLEN_8	(3 << 5)
#define UART_LCRH_WLEN_7	(2 << 5)
#define UART_LCRH_WLEN_6	(1 << 5)
#define UART_LCRH_WLEN_5	(0 << 5)
#define UART_LCRH_FEN		(1 << 4)
#define UART_LCRH_STP2		(1 << 3)
#define UART_LCRH_EPS		(1 << 2)
#define UART_LCRH_PEN		(1 << 1)
#define UART_LCRH_BRK		(1 << 0)

/* control register bits */
#define UART_CR_CTSEN		(1 << 15)
#define UART_CR_RTSEN		(1 << 14)
#define UART_CR_OUT2		(1 << 13)
#define UART_CR_OUT1		(1 << 12)
#define UART_CR_RTS		(1 << 11)
#define UART_CR_DTR		(1 << 10)
#define UART_CR_RXE		(1 << 9)
#define UART_CR_TXE		(1 << 8)
#define UART_CR_LPE		(1 << 7)
#define UART_CR_OVSFACT	(1 << 3)
#define UART_CR_UARTEN		(1 << 0)

void uart_print(const char *fmt, ...);

#ifdef DEBUG
#define logmsg(format, args...) uart_print(format, ##args)
#else
#define logmsg(format, args...) do {} while (0)
#endif

#define logerror(format, args...) uart_print(format, ##args)
void * memcpy ( void * destination, const void * source, u32 num );
#endif /* __UART_H__ */

