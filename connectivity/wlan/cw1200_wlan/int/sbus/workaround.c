/*=============================================================================
 *
 * Linux Driver for CW1200 series
 *
 *
 * Copyright (c) ST-Ericsson SA, 2010
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 *=============================================================================*/
/**
 * \addtogroup Linux Driver SBUS Layer
 * \brief
 *
 */
/**
 * \file workaround.c
 * - <b>PROJECT</b>             : CW1200_LINUX_DRIVER
 * - <b>FILE</b>                    : workaround.c
 * \brief
 * This module interfaces with the Linux Kernel MMC/SDIO stack.
 * \ingroup SBUS
 * \date 25/02/2010
 
 ** LAST MODIFIED BY:	Ajitpal Singh
 ** CHANGE LOG :
 */

#ifndef USE_SPI
#include <linux/mmc/sdio_func.h>
#else
#include <linux/device.h>
#include <linux/spi/spi.h>
#endif
#include <linux/workqueue.h>
#include "cw1200_common.h"
#include "sbus.h"

static struct delayed_work ctrl_work;
static struct workqueue_struct * ctrl_WQ;

#ifndef USE_SPI
#ifndef GPIO_BASED_IRQ
void cw1200_sbus_interrupt(CW1200_bus_device_t *func);
#endif
#else
irqreturn_t cw1200_sbus_interrupt(int irq, void *dev_id);
#endif

static struct CW1200_priv * priv = NULL;

extern CW1200_STATUS_E SBUS_SDIO_RW_Reg(struct CW1200_priv * priv,
	uint32_t addr, uint8_t * rw_buf, uint16_t length, int32_t read_write );

static void CONTROL_bh(struct work_struct *work)
{
	int32_t retval=0;
	uint32_t cont_reg_val = 0;
	uint32_t read_len=0;


#ifdef WORKAROUND
	down(&priv->sem);
#endif

	if((retval = SBUS_SDIO_RW_Reg(priv, ST90TDS_CONTROL_REG_ID,
			(char *)&cont_reg_val, BIT_16_REG,SDIO_READ)))
		return;

	cont_reg_val = (uint16_t) (cont_reg_val & ST90TDS_SRAM_BASE_LSB_MASK );

	/* Extract next message length(in words) from CONTROL register */
	read_len = SPI_CONT_NEXT_LENGTH(cont_reg_val);
	/* Covert length to length in BYTES */
	read_len = read_len * 2;

	if(read_len)
	{
#ifndef USE_SPI
#ifndef GPIO_BASED_IRQ
		cw1200_sbus_interrupt(priv->func);
#endif
#else
		cw1200_sbus_interrupt(priv->func->irq, priv);
#endif
	}

#ifdef WORKAROUND
	up(&priv->sem);
#endif

	queue_delayed_work(ctrl_WQ,&ctrl_work,2);
}

void irq_poll_init(struct CW1200_priv * l_priv)
{
	priv = l_priv;
	ctrl_WQ = create_singlethread_workqueue("ctrl_work");

	if (!ctrl_WQ) {
		DEBUG(DBG_ERROR, "%s cannot create ctrl_WQ", __func__);
		return;
	}
	INIT_DELAYED_WORK(&ctrl_work,CONTROL_bh);
	queue_delayed_work(ctrl_WQ,&ctrl_work,20);
}

void irq_poll_destroy(struct CW1200_priv * l_priv)
{
	if (ctrl_WQ) {
		flush_workqueue(ctrl_WQ);
		destroy_workqueue(ctrl_WQ);
	}
}
