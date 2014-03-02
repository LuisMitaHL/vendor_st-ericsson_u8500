/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * Adapted from the Linux version:
 * Author: Mattias Nilsson <mattias.i.nilsson@stericsson.com>
 *
 * U5500 PRCM Unit interface driver
 */
#include <config.h>
#include <asm/io.h>
#include <asm/arch/prcmu.h>
#include <asm/types.h>
#include <asm/io.h>
#include <asm/errno.h>


#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/arch/common.h>
#include <asm/arch/prcmu.h>
#include <asm/arch/prcmu-db5500.h>

/* CPU mailbox registers */
#define PRCM_MBOX_CPU_VAL	(U5500_PRCMU_BASE + 0x0fc)
#define PRCM_MBOX_CPU_SET	(U5500_PRCMU_BASE + 0x100)
#define PRCM_MBOX_CPU_CLR	(U5500_PRCMU_BASE + 0x104)

/* register for Ack mailbox interrupts */
#define PRCM_ARM_IT1_CLEAR	(U5500_PRCMU_BASE + 0x48C)
#define PRCM_ARM_IT1_VAL	(U5500_PRCMU_BASE + 0x494)

#define _PRCM_MB_HEADER (tcdm_base + 0xFE8)
#define PRCM_REQ_MB5_HEADER (_PRCM_MB_HEADER + 0x5)
#define PRCM_ACK_MB5_HEADER (_PRCM_MB_HEADER + 0xd)
#define PRCM_XP70_CUR_PWR_STATE (tcdm_base + 0xFFC)
#define _PRCM_MB_HEADER         (tcdm_base + 0xFE8)

/* Req Mailboxes */
#define PRCM_REQ_MB5 (tcdm_base + 0xF90)
#define PRCM_REQ_MB5                   (tcdm_base + 0xF90)
#define PRCM_SW_RST_REASON             (tcdm_base + 0xFF8)

/* Ack Mailboxes */
#define PRCM_ACK_MB5 (tcdm_base + 0xF14)

enum mb_return_code {
	RC_SUCCESS,
	RC_FAIL,
};

/* Mailbox 5 headers. */
enum mb5_header {
	MB5H_I2C_WRITE = 1,
	MB5H_I2C_READ,
};

/* Request mailbox 5 fields. */
#define PRCM_REQ_MB5_I2C_SLAVE (PRCM_REQ_MB5 + 0)
#define PRCM_REQ_MB5_I2C_REG (PRCM_REQ_MB5 + 1)
#define PRCM_REQ_MB5_I2C_SIZE (PRCM_REQ_MB5 + 2)
#define PRCM_REQ_MB5_I2C_DATA (PRCM_REQ_MB5 + 4)

/* Acknowledge mailbox 5 fields. */
#define PRCM_ACK_MB5_RETURN_CODE (PRCM_ACK_MB5 + 0)
#define PRCM_ACK_MB5_I2C_DATA (PRCM_ACK_MB5 + 4)

#define NUM_MB		8
#define MBOX_BIT(x)	(1 << (x))
#define ALL_MBOX_BITS	(MBOX_BIT(NUM_MB) - 1)

/*
 * mb5_transfer - state needed for mailbox 5 communication.
 * @ack:	Reply ("acknowledge") data.
 */
static struct {
	struct {
		u8 header;
		u8 status;
		u8 value[4];
	} ack;
} mb5_transfer;

static int read_mailbox_5(void);

/* PRCMU TCDM base IO address. */
static void *tcdm_base;

static int wait_for_completion(void)
{
	int retry = 0xFFFF;

	/* poll for mbox5 interrupt */
	while (((readl(PRCM_ARM_IT1_VAL) & MBOX_BIT(5)) != MBOX_BIT(5))
			&& retry > 0)
		--retry;
	if (retry == 0) {
		printf("prcmu-db5500: no mbox5 interrupt received\n");
		return -1;
	}

	return 0;
}

/**
 * prcmu_abb_read() - Read register value(s) from the ABB.
 * @slave:      The I2C slave address.
 * @reg:        The (start) register address.
 * @value:      The read out value(s).
 * @size:       The number of registers to read.
 *
 * Reads register value(s) from the ABB.
 * @size has to be <= 4.
 */
int prcmu_abb_read(u8 slave, u8 reg, u8 *value, u8 size)
{
	int r;
	int retry = 0xFFFF;

	if ((size < 1) || (4 < size))
		return -1;

	/* wait for mb5 pending request to complete */
	while (readl(PRCM_MBOX_CPU_VAL) & MBOX_BIT(5) &&
			(retry > 0))
		--retry;

	if (retry <= 0)
		return -1;

	writeb(slave, PRCM_REQ_MB5_I2C_SLAVE);
	writeb(reg, PRCM_REQ_MB5_I2C_REG);
	writeb(size, PRCM_REQ_MB5_I2C_SIZE);
	writeb(MB5H_I2C_READ, PRCM_REQ_MB5_HEADER);

	writel(MBOX_BIT(5), PRCM_MBOX_CPU_SET);
	r = wait_for_completion();
	if (r < 0)
		return -1;

	/* Read last request ack */
	r = read_mailbox_5();
	if (r < 0)
		return r;

	if ((mb5_transfer.ack.header == MB5H_I2C_READ) &&
		(mb5_transfer.ack.status == RC_SUCCESS)) {
		memcpy(value, mb5_transfer.ack.value, (size_t)size);
		r = 0;
	} else
		r = -1;

	return r;
}

/**
 * prcmu_abb_write() - Write register value(s) to the ABB.
 * @slave:      The I2C slave address.
 * @reg:        The (start) register address.
 * @value:      The value(s) to write.
 * @size:       The number of registers to write.
 *
 * Writes register value(s) to the ABB.
 * @size has to be <= 4.
 */
int prcmu_abb_write(u8 slave, u8 reg, u8 *value, u8 size)
{
	int r;
	int retry = 0xFFFF;

	if ((size < 1) || (4 < size))
		return -1;

	/* wait for mb5 pending request to complete */
	while (readl(PRCM_MBOX_CPU_VAL) & MBOX_BIT(5) &&
			(retry > 0))
		--retry;

	if (retry <= 0)
		return -1;

	writeb(slave, PRCM_REQ_MB5_I2C_SLAVE);
	writeb(reg, PRCM_REQ_MB5_I2C_REG);
	writeb(size, PRCM_REQ_MB5_I2C_SIZE);
	memcpy(PRCM_REQ_MB5_I2C_DATA, value, size);
	writeb(MB5H_I2C_WRITE, PRCM_REQ_MB5_HEADER);

	writel(MBOX_BIT(5), PRCM_MBOX_CPU_SET);

	r = wait_for_completion();
	if (r < 0)
		return -1;

	/* Read last request ack */
	r = read_mailbox_5();
	if (r < 0)
		return r;

	if ((mb5_transfer.ack.header == MB5H_I2C_WRITE) &&
			(mb5_transfer.ack.status == RC_SUCCESS))
		r = 0;
	else
		r = -1;

	return r;
}

static inline void print_unknown_header_warning(u8 n, u8 header)
{
	printf("prcmu: Unknown message header (%d) in mailbox %d.\n",
		header, n);
}

static int read_mailbox_5(void)
{
	u8 header;

	header = readb(PRCM_ACK_MB5_HEADER);
	switch (header) {
	case MB5H_I2C_READ:
		memcpy(mb5_transfer.ack.value, PRCM_ACK_MB5_I2C_DATA, 4);
	case MB5H_I2C_WRITE:
		mb5_transfer.ack.header = header;
		mb5_transfer.ack.status = readb(PRCM_ACK_MB5_RETURN_CODE);
		writel(MBOX_BIT(5), PRCM_ARM_IT1_CLEAR);
		break;
	default:
		writel(MBOX_BIT(5), PRCM_ARM_IT1_CLEAR);
		print_unknown_header_warning(5, header);
		return -1;
		break;
	}

	return 0;
}

/*
 * prcmu_fw_init - arch init call for the Linux PRCMU fw init logic
 */
int prcmu_init(void)
{
	tcdm_base = (void *)U5500_PRCMU_TCDM_BASE;

	/* Clean up the mailbox interrupts after pre-u-boot code. */
	writel(ALL_MBOX_BITS, PRCM_ARM_IT1_CLEAR);

	/* Enable timers */
	writel(1 << 17, PRCM_TCR);

	return 0;
}
int prcmu_crashed()
{
	int ret_val = 0;
	if (tcdm_base != NULL) {
		if((readl(PRCM_SW_RST_REASON) & 0x0000ffff) == 0xDEAD)
			ret_val = 1;
	}
	return ret_val;
}
