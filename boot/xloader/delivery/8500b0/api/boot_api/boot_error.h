/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file boot_error.h
 * \brief ROM code's debug traces driver
 * \author STMicroelectronics
 *
 * \addtogroup BOOT
 *
 *  This header file provides error list for Boot ROM.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _BOOT_ERROR_H
#define _BOOT_ERROR_H

#include "boot_types.h"

#define BOOT_OK 							0    /**< SUCCESS, no  error.*/

/* Flash management errors */
#define	BOOT_ERROR_FLASH_NOT_FOUND 			0xA0010031 /**< Normal scenario error. No Flash found to boot. */
#define	BOOT_ERROR_TOC_NOT_FOUND 			0xA0010032 /**< Normal scenario error. No TOC found in the found memory.. */
#define	BOOT_ERROR_INVALID_BLOCK_NUMBER 	0xA0010033 /**< Invalid block number. */
#define	BOOT_ERROR_ADDRESS_OUT_OF_RANGE 	0xA0010034 /**< Out of Range read address. */
#define	BOOT_ERROR_SIZE_OUT_OF_RANGE 		0xA0010035 /**< Out of range read size. */
#define	BOOT_ERROR_ECC_ERROR 				0xA0010036 /**< ECC error while reading in flash. */
#define	BOOT_ERROR_CODE_NOT_EXECUTABLE  	0xA0010037 /**< Code not executable. */
#define	BOOT_ERROR_INVALID_BLOCK        	0xA0010038 /**< Flash block invalid. */
#define	BOOT_ERROR_INVALID_BANK         	0xA0010039 /**< Bank not supported. */
#define	BOOT_ERROR_INVALID_MEMORY       	0xA0010030 /**< Input memory not initialized. */
/* Interface errors */
#define	BOOT_UART_ERROR 					0xA0010040 /**< Uart error. */
#define	BOOT_SSI_ERROR 						0xA0010041 /**< HSI error.  */
#define	BOOT_TMR_ERROR						0xA0010042 /**< Timer error.*/
/* VIC errors */
#define	BOOT_VIC_LINE_NOT_AUTHORIZED		0xA0010050 /**< VIC error.*/
#define	BOOT_VIC_LINE_NOT_CONFIGURED		0xA0010051 /**< VIC error.*/
#define	BOOT_VIC_NO_LINE_ACTIVE				0xA0010052 /**< VIC error.*/
/* Boot scenario errors */
#define	BOOT_ITEM_NOT_PRESENT_IN_TOC_ERROR	0xA0010060 /**< Scenario error.Item missing in TOC.*/
#define	BOOT_INVALID_MAGIC_NUM				0xA0010061 /**< Scenario error.Bad magic number.*/
#define	BOOT_IMAGE_SIZE_ERROR				0xA0010062 /**< Scenario error. Image size error.*/
#define	BOOT_BAD_INDICATION_ERROR			0xA0010063 /**< Scenario error. Bad boot indication.*/
#define	BOOT_ERROR_NO_SUBIMAGE_COPY         0xA0010064 /**< Scenario error. SubImage copy not done.*/
/* Secure services errors */
#define BOOT_SECURE_BUILD_ASIC_ID_ERROR     0xA0010081 /**< Secure error. Error in Asic Id build.*/ 
/* MMC management related errors */
#define BOOT_ERROR_MMC_TIMEOUT              0xA0010090   /**< MMC error. Timeout while accessing the MMC. */
#define BOOT_ERROR_MMC_CRC_FAIL             0xA0010091   /**< MMC error. CRC error found while reading/sending command. */
#define BOOT_ERROR_MMC_LOCKED               0xA0010092   /**< MMC error. MMC card is locked for reading. */
#define BOOT_ERROR_ADDR_MISALIGNED          0xA0010093   /**< MMC error. Address not block size aligned. */
#define BOOT_ERROR_ILLEGAL_CMD              0xA0010094   /**< MMC error. Command sent not supported. */
#define BOOT_ERROR_MMC_FIFO                 0xA0010095   /**< MMC error. SDI Fifo error while reading (overrun...).*/
#define BOOT_ERROR_MMC_INTERNAL_ERR         0xA0010096   /**< MMC error. MMC internal error. */
#define BOOT_ERROR_MMC_BLK_LEN_ERR          0xA0010097   /**< MMC error. Block lenght error.*/
#define BOOT_ERROR_MMC_VOLTAGE_ERR          0xA0010098   /**< MMC error. Voltage error in initialization. */

/*2nd boot error or API generic errors*/
#define BOOT_ERROR_CANCEL_2ND_BOOT          0xA00100A0   /**< 2nd boot error. 
                                                            Received size is zero: 2nd boot is cancelled. */
#define BOOT_ERROR_NULL_POINTER             0xA00100A1   /**< API error. one pointer parameter of the function is NULL*/
#define BOOT_ERROR_RPC_INVALID_INDEX        0xA00100A2   /**< RPC error. Invalid function index. */
#define BOOT_ERROR_TIMEOUT                  0xA00100A3   /**< 2nd boot error. Timeout in waiting for data */

/* Reset or abort error*/
#define BOOT_ERROR_RESET_INVALID            0xA00100B0   /**< Reset error. Kind of reset not handled by the boot code. */

/* USB Error */ 

/* DMA Errors */ 
#define BOOT_ERROR_DMA_NO_IT_ON_USB_CHAN    0xA00100D0
#define BOOT_ERROR_DMA_CHAN_IN_ERROR        0xA00100D1
#define BOOT_ERROR_DMA_CHAN_IS_EXTENDED     0xA00100D2
#define BOOT_ERROR_DMA_CHAN_IS_SECURE       0xA00100D3



typedef t_uint32 t_boot_error;

#endif 		// _BOOT_ERROR_H
/** @} */
