/*==========================================================
 *
 *Linux Driver for CW1200 series
 *
 *
 *Copyright (c) ST-Ericsson SA, 2010
 *
 *This program is free software; you can redistribute it and/or modify it
 *under the terms of the GNU General Public License version 2 as published
 *by the Free Software Foundation.
 *
 *=========================================================*/
/**
 *addtogroup Linux Driver SBUS Layer
 *brief
 *
 */
/**
 *file sdio_wrapper.c
 *- <b>PROJECT</b>			 : CW1200_LINUX_DRIVER
 *- <b>FILE</b>					: sdio_wrapper.c
 *brief
 *This module interfaces with the Linux Kernel MMC/SDIO stack.
 *ingroup SBUS
 *date 25/02/2010

 **LAST MODIFIED BY:	Harald Unander
 **CHANGE LOG :
 */


#include <linux/version.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/regulator/consumer.h>
#include <mach/cw1200_plat.h>

#ifndef RESOUTN_WLAN_ENABLE
#include <linux/gpio.h>
#else
#include <mach/db5500-regs.h>
#endif

#include "cw1200_common.h"
#include "sbus_wrapper.h"
#include "eil.h"
#include "sbus.h"
#include "cil.h"

#ifdef RESOUTN_WLAN_ENABLE
/* PRCMU RESOUTN registers */
#define PRCM_RESOUTN_SET_OFFSET	0x214
#define PRCM_RESOUTN_CLR_OFFSET	0x218
#define PRCM_RESOUTN_VAL_OFFSET	0x21C
#define RESOUT2_N_PIN			(1<<2)
#endif

/* Regulator support related function declarations */
static int cw1200_enable_regulator(struct sdio_func *);
static void cw1200_disable_regulator(void);
static int cw1200_detect_card(void);


struct sdio_func *g_func;
const struct cw1200_platform_data *wpd;
CW1200_STATUS_E eil_init_status = SUCCESS;
struct sdio_priv sdio_cw_priv;

/* Regulator support */
struct regulator *regulator;
int32_t regulator_enabled;

static void cw1200_device_release(struct device *dev)
{
	DEBUG(DBG_SBUS,"CW1200 device release\n");
}

struct device cw1200_device = {
	.release = cw1200_device_release,
};

static const struct sdio_device_id if_sdio_ids[] = {
	{ SDIO_DEVICE(SDIO_ANY_ID, SDIO_ANY_ID) },
	{ /*end: all zeroes */				},
};

int sbus_memcpy_fromio(CW1200_bus_device_t *func,
		void *dst, unsigned int addr, int count)
{
	return sdio_memcpy_fromio(func, dst, addr, count);
}

int sbus_memcpy_toio(CW1200_bus_device_t *func,
		unsigned int addr, void *src, int count)
{
	return sdio_memcpy_toio(func, addr, src, count);
}

static int cw1200_detect_card(void)
{
	/* HACK!!!
	 * Rely on mmc->class_dev.class set in mmc_alloc_host
	 * Tricky part: a new mmc hook is being (temporary) created
	 * to discover mmc_host class.
	 * Do you know more elegant way how to enumerate mmc_hosts?
	 */

	struct mmc_host *mmc = NULL;
	struct class_dev_iter iter;
	struct device *dev;

	mmc = mmc_alloc_host(0, NULL);
	if (!mmc)
		return -ENOMEM;

	BUG_ON(!mmc->class_dev.class);
	class_dev_iter_init(&iter, mmc->class_dev.class, NULL, NULL);
	for (;;) {
		dev = class_dev_iter_next(&iter);
		if (!dev) {
			printk(KERN_ERR "cw1200: %s is not found.\n",
				wpd->mmc_id);
			break;
		} else {
			struct mmc_host *host = container_of(dev,
				struct mmc_host, class_dev);

			if (dev_name(&host->class_dev) &&
				strcmp(dev_name(&host->class_dev),
					wpd->mmc_id))
				continue;

			mmc_detect_change(host, 10);
			break;
		}
	}
	mmc_free_host(mmc);
	return 0;
}

/*Probe Function to be called by SDIO stack when device is discovered */
static int cw1200_sdio_probe(struct sdio_func *func,
		const struct sdio_device_id *id)
{
	CW1200_STATUS_E eil_init_status = SUCCESS;

	DEBUG(DBG_MESSAGE, "CW1200-Driver Probe called\n");
	printk(KERN_ERR "------------------ cw1200_sdio_probe\n");
	if (cw1200_enable_regulator(func))
		DEBUG(DBG_ERROR, "Could not enable regulator framework\n");
	printk(KERN_ERR " after enable regulator\n");
	/* Store the func in GLOBAL variable.
		If probe is not called this var is NULL */
	g_func = func;

	eil_init_status = EIL_Init(func);
	DEBUG(DBG_MESSAGE, "EIL_Init() return status [%d]\n", eil_init_status);
         printk(KERN_ERR " EIL_Init result is %d\n",eil_init_status);
	if (eil_init_status != SUCCESS) {
		sdio_set_drvdata(func, NULL);
		return -1;
	}

	return 0;
}

/*Disconnect Function to be called by SDIO
 * stack when device is disconnected */
static void cw1200_sdio_disconnect(struct sdio_func *func)
{

	DEBUG(DBG_MESSAGE, "%s calling EIL Shutdown\n", __func__);
	if (sdio_cw_priv.driver_priv) {
		if (SHUTDOWN_PENDING == EIL_Shutdown(sdio_cw_priv.driver_priv)) {
			wait_for_completion(&shutdown_comp);
		}
	} else {
		DEBUG(DBG_MESSAGE, "EIL_Shutdown already completed \n");
	}
	sdio_set_drvdata(func, NULL);
	cw1200_disable_regulator();
}

static int cw1200_suspend(struct device *dev)
{
	/* TODO: What is needed to do here? */
	DEBUG(DBG_MESSAGE, "%s,going to suspend\n", __func__);
	return 0;
}

static int cw1200_resume(struct device *dev)
{
	/* TODO: What is needed to do here? */
	DEBUG(DBG_MESSAGE, "%s,resumed from suspend\n", __func__);
	return 0;
}

static struct dev_pm_ops cw1200_pm_ops = {
		.suspend = cw1200_suspend,
		.resume = cw1200_resume,
};

static struct sdio_driver sdio_cw_driver = {
	.name = "cw1200_sdio",
	.id_table = if_sdio_ids,
	.probe = cw1200_sdio_probe,
	.remove = cw1200_sdio_disconnect,
	.drv = {
		.pm = &cw1200_pm_ops,
	}
};



static int cw1200_enable_regulator(struct sdio_func * func)
{
#ifdef CONFIG_REGULATOR
	int err;


	cw1200_device.parent = &func->dev;
	dev_set_name(&cw1200_device, "cw1200");
	err = device_register(&cw1200_device);
	if (err) {
			DEBUG(DBG_ERROR,"Cant register CW1200 device \n");
			return err;
	}
	/* Get and enable regulator. */
	regulator = regulator_get(&cw1200_device, "wlan_1v8");
	if (IS_ERR(regulator)) {
		DEBUG(DBG_ERROR, "Not able to find regulator.");
		err = PTR_ERR(regulator);
	} else {
		err = regulator_enable(regulator);
		if (err) {
			DEBUG(DBG_SBUS, "Not able to enable regulator.");
		}
		else {
			DEBUG(DBG_SBUS, "Regulator enabled.");
			regulator_enabled = true;
		}
	}
	return err;
#else
	return 0;
#endif
}

/**
 * cw1200_disable_regulator() - Disable regulator.
 *
 */
static void cw1200_disable_regulator()
{
#ifdef CONFIG_REGULATOR

	/* Disable and put regulator. */
	if (regulator && regulator_enabled) {
		regulator_disable(regulator);
		regulator_enabled = false;
	}
	regulator_put(regulator);
	regulator = NULL;
	device_unregister(&cw1200_device);
#endif
}


/*Init Module function -> Called by insmod */
static int __init sbus_sdio_init(void)
{
	int ret;

	printk(KERN_NOTICE "cw1200: %s -->\n", __func__);
        printk(KERN_NOTICE "nnnnnnnnnnnnn  check new");
        printk(KERN_ERR "nnnnnnnnnnnnn check new again ");
	wpd = cw1200_get_platform_data();
        printk(KERN_ERR "nnnnnn after cw1200_get_platform_data ");
	if (NULL == wpd) {
		printk(KERN_ERR "%s: cw1200_platform_data NULL\n", __func__);
		return -EBUSY;
	}
	printk(KERN_ERR "1111111111");
	if (wpd->clk_ctrl) {
		ret = wpd->clk_ctrl(wpd, true);
		if (ret)
			return ret;
	}
         if (wpd->power_ctrl) {
                ret = wpd->power_ctrl(wpd, true);
                if (ret)
                        return ret;
        }

	printk(KERN_ERR "222222222222222");
	ret = sdio_register_driver(&sdio_cw_driver);
	if (ret) {
		printk(KERN_NOTICE "cw1200: sdio_register_driver FAILED with [%d] -->\n", ret);
		return ret;
	}
	printk(KERN_ERR "33333333333333333");

#ifndef RESOUTN_WLAN_ENABLE
          printk(KERN_NOTICE "bbbbbbbbbbbbbbbb, rest-0x%x",wpd->reset);
	/* Request WLAN Enable GPIO */
	//ret = gpio_request(wpd->reset->start, "sdio_init");
	ret = gpio_request(wpd->reset->start, wpd->reset->name);
	if (ret) {
		printk(KERN_NOTICE "INIT:Unable to request gpio_[%d], ErrorCode = [%d]\n",
				wpd->reset->start, ret);
		sdio_unregister_driver(&sdio_cw_driver);
		return ret;
	}
        printk(KERN_NOTICE "aaaaaaaaaaaaaaaaaaaaa");
	ret = gpio_direction_output(wpd->reset->start, 1);
       msleep(50);
	if (ret) {
		printk(KERN_NOTICE "INIT:Unable to set gpio direction, ErrorCode = [%d]\n",
				ret);
		gpio_free(wpd->reset->start);
		sdio_unregister_driver(&sdio_cw_driver);
		return ret;
	}
       printk(KERN_NOTICE "99999999999999");
	gpio_set_value(wpd->reset->start, 1);
       printk(KERN_NOTICE "88888888888888888888");
	mdelay(10);
#else
      printk(KERN_NOTICE "77777777777777777");
	void __iomem *_PRCMU_BASE = __io_address(U5500_PRCMU_BASE);
        printk(KERN_NOTICE "666666666666666666");
	writel(RESOUT2_N_PIN, _PRCMU_BASE + PRCM_RESOUTN_SET_OFFSET);
         printk(KERN_NOTICE "55555555555555555555");
	mdelay(50);
#endif
        printk(KERN_NOTICE "44444444444444444444444444");
	cw1200_detect_card();
	printk(KERN_NOTICE "cw1200: %s <--\n", __func__);
	return 0;
}


/*Called at Driver Unloading */
static void __exit sbus_sdio_exit(void)
{

	printk(KERN_NOTICE "cw1200: %s -->\n", __func__);
	if (delayed_work_pending(&sdio_cw_priv.unload_work)){
		DEBUG(DBG_SBUS,"%s Cancelling delayed unload_work\n",__func__);
		cancel_delayed_work_sync(&sdio_cw_priv.unload_work);
	}

	sdio_unregister_driver(&sdio_cw_driver);
	mdelay(10);
#ifndef RESOUTN_WLAN_ENABLE
	if (wpd) {
		gpio_set_value(wpd->reset->start, 0);
		mdelay(10);
		gpio_free(wpd->reset->start);
	} else {
		printk(KERN_ERR "%s: cw1200_platform_data NULL\n", __func__);
	}

#else
	void __iomem *_PRCMU_BASE = __io_address(U5500_PRCMU_BASE);
	writel(RESOUT2_N_PIN, _PRCMU_BASE + PRCM_RESOUTN_CLR_OFFSET);
	mdelay(50);
#endif
	/*If func is null then probe was not called */
	if (NULL == g_func) {
		printk(KERN_NOTICE "EXIT: g_func == NULL\n");
		return;
	}
	cw1200_detect_card();
	if (wpd && wpd->clk_ctrl)
		wpd->clk_ctrl(wpd, false);
	printk(KERN_NOTICE "cw1200: %s <--\n", __func__);

}


module_init(sbus_sdio_init);
module_exit(sbus_sdio_exit);
