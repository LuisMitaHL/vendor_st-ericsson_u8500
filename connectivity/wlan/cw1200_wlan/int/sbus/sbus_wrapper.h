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
 * \file sbus_wrapper.h
 * - <b>PROJECT</b>             : CW1200_LINUX_DRIVER
 * - <b>FILE</b>                    : sbus_wrapper.h
 * \brief
 * This module interfaces with the Linux Kernel MMC/SDIO stack.
 * \ingroup SBUS
 * \date 25/02/2010
 ** LAST MODIFIED BY:	Harald Unander
 ** CHANGE LOG :
 */


#ifndef __SBUS_WRAPPER_HEADER__
#define __SBUS_WRAPPER_HEADER__

#ifndef USE_SPI
	#include <linux/mmc/sdio_func.h>
#else
	#include <linux/spi/spi.h>
#endif

#define MAX_WAIT 50 /*maximum limit to loop*/

int sbus_memcpy_fromio(CW1200_bus_device_t *func, void *dst, unsigned int addr, int count);
int sbus_memcpy_toio(CW1200_bus_device_t *func, unsigned int addr, void *src, int count);

#endif
