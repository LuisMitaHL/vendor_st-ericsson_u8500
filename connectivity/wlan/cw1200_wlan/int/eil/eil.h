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
 ==========================================================================*/
/**
 * \addtogroup Linux Driver - EIL(Ethernet Interface)Layer
 * \brief
 *
 */
/**
 * \file eil.h
 * - <b>PROJECT</b>             : CW1200_LINUX_DRIVER
 * - <b>FILE</b>                : eil.h
 * \brief
 * This module interfaces with the Linux Kernel 802.3/Ethernet Layer.
 * \ingroup EIL
 * \date 25/02/2010
 */
#ifndef __EIL_HEADER__
#define __EIL_HEADER__

/*****************************************************************************
		INCLUDE FILES
*****************************************************************************/
#include "cw1200_common.h"
#include <linux/fs.h>   /* File handling */
#ifndef USE_SPI
#include <linux/mmc/sdio_func.h>  /* SDIO funcs */
#else
#include <linux/spi/spi.h>
#endif

#define EIL_TX_TIMEOUT (4*HZ)
#define ETH_HLEN	14
#define DEV_NAME "wlan%d"

#define AC_BK 		0
#define AC_BE 		1
#define AC_VI 		2
#define AC_VO 		3

#define AC_BK_QUEUE	15
#define AC_BE_QUEUE	20
#define AC_VI_QUEUE	30
#define AC_VO_QUEUE	60

#define UMAC_TX_QUEUE_SIZE  59

#define ALIGN_SKB_DATA		2

#define MORE_DATA_FLAG	0x8000
#define FRAME_COUNT	0x7FFF
#define BRIDGED_FRAME   0x80000000

enum cw1200_shutdown_states {
	SHUTDOWN_SUCCESS,
	SHUTDOWN_NOT_STARTED,
	SHUTDOWN_STARTED,
	SHUTDOWN_PENDING,
};

#define SUCCESS 0

/****************************************************************************
*        						EIL Global Function prototypes
*****************************************************************************/
CW1200_STATUS_E EIL_Init(CW1200_bus_device_t *func);

int EIL_Shutdown(struct CW1200_priv *priv);

void EIL_Init_Complete(struct CW1200_priv *priv);

int EIL_Transmit(struct sk_buff *skb, struct net_device *dev);

int32_t map_mac_addr(struct CW1200_priv *priv, uint8_t *mac_addr);

#endif
