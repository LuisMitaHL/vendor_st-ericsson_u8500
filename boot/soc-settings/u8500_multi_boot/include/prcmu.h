/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson.
 */

#ifndef __PRCMU_H__
#define __PRCMU_H__

#include <prcmu/numerics_v20.h>
#include <prcmu/xp70_memmap_v20.h>
#include <peripheral.h>

/* GPIO reset */
#define PRCM_GPIO_RESETN_SET			(PRCMU_BASE + 0x208)
#define PRCM_GPIO_RESETN_CLR			(PRCMU_BASE + 0x20C)

/* GPIO muxing control */
#define PRCM_GPIOCR				(PRCMU_BASE + 0x138)

/* Mailbox */
#define PRCM_MBOX_CPU_SET			(PRCMU_BASE + 0x100)

#define PRCM_MBOX_CPU_IT19			(1 << 7)
#define PRCM_MBOX_CPU_IT18			(1 << 6)
#define PRCM_MBOX_CPU_IT17			(1 << 5)
#define PRCM_MBOX_CPU_IT16			(1 << 4)
#define PRCM_MBOX_CPU_IT13			(1 << 3)
#define PRCM_MBOX_CPU_IT12			(1 << 2)
#define PRCM_MBOX_CPU_IT11			(1 << 1)
#define PRCM_MBOX_CPU_IT10			(1 << 0)

/* Mailbox 1 ack */
#define PRCM_ACK_MB1				0xE04

/* Mailbox header offset*/
#define PRCM_MBOX_HEADER			0xFE8 /* 16 bytes */
#define PRCM_SW_RST_REASON			(0xFF8)

/* Mailbox 1 Requests */
#define PRCM_REQ_MB1 				0xFD0 /* 12 bytes  */
#define PRCM_REQ_MB1_ARM_OPP			(PRCM_REQ_MB1 + 0x0)

/* Interrupt */
#define PRCM_ARM_IT1_VAL			(PRCMU_BASE + 0x494)
#define PRCM_ARM_IT1_CLR			(PRCMU_BASE + 0x48C)

#define PRCM_ARM_IT1_CLR_ALL			(0xFF)

/* Clock/PLL/reset */
#define PRCM_PLLSOC0_FREQ			(PRCMU_BASE + 0x080)
#define PRCM_PLLARM_FREQ			(PRCMU_BASE + 0x088)
#define PRCM_PER1CLK_MGT			(PRCMU_BASE + 0x02C)
#define PRCM_PER2CLK_MGT			(PRCMU_BASE + 0x030)
#define PRCM_PER3CLK_MGT			(PRCMU_BASE + 0x034)
#define PRCM_PER5CLK_MGT			(PRCMU_BASE + 0x038)
#define PRCM_PER6CLK_MGT			(PRCMU_BASE + 0x03C)
#define PRCM_PER7CLK_MGT			(PRCMU_BASE + 0x040)

#define PRCM_PERCLK38DIV			(1 << 11)
#define PRCM_PERCLK38SRC			(1 << 10)
#define PRCM_PERCLK38				(1 << 9)
#define PRCM_PERCLKEN				(1 << 8)
#define PRCM_PERCLKPLLSW_4			(4 << 5)
#define PRCM_PERCLKPLLDIV_6			(6 << 0)

/* Modem */
#define PRCM_MOD_RESETN_SET_REG			(PRCMU_BASE + 0x1FC)
#define PRCM_MOD_RESETN_SET_REG_2		(1 << 2)
#define PRCM_MOD_RESETN_SET_REG_1		(1 << 1)
#define PRCM_MOD_RESETN_SET_REG_0		(1 << 0)

#define PRCM_MOD_SWRESET_IRQ_ACK		(PRCMU_BASE + 0x4A4)
#define PRCM_MOD_SWRESET_IRQ_ACK_SET		(1)
#define PRCM_MOD_SWRESET_IRQ_ACK_CLR		(0)

#define PRCM_MOD_AWAKE_STATUS			(PRCMU_BASE + 0x4A0)
#define PRCM_MOD_AWAKE_STATUS_COREPD_AWAKE	(1 << 0)
#define PRCM_MOD_AWAKE_STATUS_AAPD_AWAKE	(1 << 1)
#define PRCM_MOD_AWAKE_STATUS_VMODEM_OFF_ISO	(1 << 2)

#define PRCM_HOSTACCESS_REQ			(PRCMU_BASE + 0x334)
#define PRCM_HOSTACCESS_REQ_WAKE_REQ		(1 << 16)
#define PRCM_HOSTACCESS_REQ_HOSTACCESS_ID_4	(3 << 1)
#define PRCM_HOSTACCESS_REQ_HOSTACCESS_REQ	(1 << 0)

/* External interrupt unit */
#define PRCM_LINE_VALUE				(PRCMU_BASE + 0x170)
#define PRCM_LINE_VALUE_MOD_HOST_PORT_AVAILABLE	(1 << 11)

/* PMU/AB8500 clock request and modem sysclk management */
#define PRCM_SYSCLKOK_DELAY			(PRCMU_BASE + 0x318)
#define PRCM_SYSCLKOK_NO_DELAY			(0x00)

/* DDR */
#define PRCM_DDRSUBSYS_APE_MINBW		(PRCMU_BASE + 0x438)
#define PRCM_DDRSUBSYS_APE_MINBW_FMAX		(0)
#define PRCM_DDRSUBSYS_APE_MINBW_FMAX_2		(1)
#define PRCM_DDRSUBSYS_APE_MINBW_FMAX_4		(2)

/* Reset */
#define PRCM_RESET_STATUS			(PRCMU_BASE + 0x103C)
#define PRCM_A9_CPU_WATCHDOG_RESET		(0x00000003)

#endif /* __PRCMU_H__ */

