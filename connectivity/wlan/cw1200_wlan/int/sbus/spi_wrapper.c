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
 * \file spi_wrapper.c
 * - <b>PROJECT</b>             : CW1200_LINUX_DRIVER
 * - <b>FILE</b>                    : spi_wrapper.c
 * \brief
 * This module interfaces with the Linux Kernel MMC/SDIO stack.
 * \ingroup SBUS
 * \date 25/02/2010
 
 ** LAST MODIFIED BY:	Harald Unander
 ** CHANGE LOG :
 */


//#ifdef SPI_WRAPPER_DBG
// #define err(fmt,arg...) printk(KERN_ERR"%s:"fmt"\n",__FUNCTION__,##arg)
// #define info(fmt,arg...) printk(KERN_INFO"%s:"fmt"\n",__FUNCTION__,##arg)
// #define warn(fmt,arg...) printk(KERN_WARNING"%s:"fmt"\n",__FUNCTION__,##arg)
//#else
// #define err(fmt,arg...) ;
// #define info(fmt,arg...) ;
// #define warn(fmt,arg...) ;
//#endif

#define SWAP_2(x) ( ( ((x)&0xff)<<8) | ((unsigned short)(x)>>8 ))
#define FIX_SHORT(x) (*(unsigned short*)&(x) = SWAP_2(*(unsigned short*)&(x)))

#include "cw1200_common.h"
#include "sbus_wrapper.h"
#include "eil.h"

#include <linux/spi/spi.h>
#include <linux/device.h>
//MODULE_LICENSE("GPL");

/* MACROS */
#define SDIO_TO_SPI_ADDR(addr) ((addr & 0x1f)>>2)
#define SET_WRITE 0x7FFF //usage: and operation
#define SET_READ 0x8000  //usage: or operation

static int sbus_spi_read(struct spi_device *spi, u32 addr, uint8_t *rxdata,int len);
static int sbus_spi_write(struct spi_device *spi, u32 addr, uint8_t *txdata,int len);

int sbus_memcpy_fromio(CW1200_bus_device_t *func, void *dst, unsigned int addr, int count)
/**
 * sbus_memcpy_fromio - read a chunk of memory from a SDIO function
 * @func: SDIO function to access
 * @dst: buffer to store the data
 * @addr: address to begin reading from
 * @count: number of bytes to read
 *
 * Reads from the address space of a given SDIO function. Return
 * value indicates if the transfer succeeded or not.
 */
{
	uint32_t addrl = SDIO_TO_SPI_ADDR(addr);

	count++;
	count/=2;

	return sbus_spi_read(func, addrl, dst, count);
}

int sbus_memcpy_toio(CW1200_bus_device_t *func, unsigned int addr, void *src, int count)
/**
 * sbus_memcpy_toio - write a chunk of memory to a SDIO function
 * @func: SDIO function to access
 * @addr: address to start writing to
 * @src: buffer that contains the data to write
 * @count: number of bytes to write
 *
 * Writes to the address space of a given SDIO function. Return
 * value indicates if the transfer succeeded or not.
 */
{
	uint32_t addrl = SDIO_TO_SPI_ADDR(addr);
  
	count++;
	count/=2;
  
	return sbus_spi_write(func, addrl, src, count);  
}
iii

static int sbus_spi_write(struct spi_device *spi, u32 addr, uint8_t *txdata,int len)
{

	uint16_t regaddr;
	struct spi_transfer     t_addr = {
                    .tx_buf         = &regaddr,
                    .len            = sizeof(regaddr),
            };
	struct spi_transfer     t_msg = {
                    .tx_buf         = txdata,
                    .len            = len * 2,
            };
	struct spi_message      m;


	regaddr=(addr)<<12;
	regaddr&=SET_WRITE;
	regaddr|=(len);
	

	spi_message_init(&m);
	spi_message_add_tail(&t_addr, &m);
	spi_message_add_tail(&t_msg, &m);
	return spi_sync(spi, &m);
}

static int sbus_spi_read(struct spi_device *spi, u32 addr, uint8_t *rxdata,int len)
{

	uint16_t regaddr;
	struct spi_transfer     t_addr = {
                    .tx_buf         = &regaddr,
                    .len            = sizeof(regaddr),
            };
	struct spi_transfer     t_msg = {
                    .rx_buf         = rxdata,
                    .len            = len * 2,
            };
	struct spi_message      m;

	regaddr=(addr)<<12;
	regaddr|=SET_READ;
	regaddr|=(len);

	spi_message_init(&m);
	spi_message_add_tail(&t_addr, &m);
	spi_message_add_tail(&t_msg, &m);
	return spi_sync(spi, &m);
}

/*  Probe Function to be called by SDIO stack when device is discovered */
static int cw1200_spi_probe(struct spi_device *card)
{
    CW1200_STATUS_E status = SUCCESS;
    DEBUG(DBG_SBUS,"Probe called \n");
    printk(KERN_ERR"############## enter cw1200_spi_probe");
    status = EIL_Init(card);
    DEBUG(DBG_SBUS,"EIL_Init() return status [%d] \n",status);
    return 0;
}

/*  Disconnect Function to be called by SDIO stack when device is disconnected */
static int __devexit cw1200_spi_disconnect(struct spi_device *func)
{
    struct CW1200_priv * priv = NULL;

    priv = spi_get_drvdata(func);
    if (priv)
        EIL_Shutdown(priv);
    return 0;
}

static struct spi_driver stlc_spi_driver = {
	.probe  = cw1200_spi_probe,
	.remove = __devexit_p(cw1200_spi_disconnect),
	.driver = {
		.name   = "stlc9000_spi",
		.bus    = &spi_bus_type,
		.owner  = THIS_MODULE,
	},
};

/* Init Module function -> Called by insmod */
static int __init sbus_spi_init (void)
{
   printk(KERN_ERR"STE WLAN- Driver Init Module Called \n");
   return spi_register_driver (&stlc_spi_driver);
}

 /* Called at Driver Unloading */
static void __exit sbus_spi_exit (void)
{
    spi_unregister_driver (&stlc_spi_driver);
    DEBUG(DBG_SBUS,"Unloaded SBUS Test driver, \n");
}


module_init(sbus_spi_init);
module_exit(sbus_spi_exit);
