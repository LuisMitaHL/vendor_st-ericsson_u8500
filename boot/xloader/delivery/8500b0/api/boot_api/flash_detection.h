/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file flash_detection.h
 * \brief Flash detection module for boot
 * \author STMicroelectronics
 *
 * \addtogroup FLASH
 *
 * This module provides some code in charge of  handling the flash memories
 * and initialize the FSMC component. \n
 * Specification release related to this implementation: MMTC_SAMOS_PF_0069 V0.1
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef __BOOT_FLASH_h
#define __BOOT_FLASH_h


/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "boot_types.h"
#include "boot_error.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

#define ST_MAKER_CODE 		0x20  /**< ST maker code used to detect One Nand */
#define SAMSUNG_MAKER_CODE	0xEC  /**< Samsung maker code used to detect One Nand */


/** defines a list of 'device codes' used to identify a particular device*/
typedef enum
{ 
    ONE_NAND_SLC = -4, /**< (-4) code used for ONE_NAND_SLC */
    FLEX_ONE_NAND = -5, /**< (-5) code used for FLEX_ONE_NAND */
    NO_VALID_ID	= 0, /**< (0) device code invalid */
    UNKNOWN_ID	= -3 /**< (-3) device code unknown */
}t_flash_command_id;

/** define on which FSMC BANK the memory is connected*/
typedef enum
{
    NOR_BANK_0 = 0, /**< (0) NOR bank 0 */
    LAST_BANK        /**< (2) invalid bank */
}t_flash_FSMC_bank;

/** define on which NAND flash block the TOC search mechanism must be
  executed*/
typedef enum
{
    NAND_BLOCK_0 = 0, /**< (0) */
    NAND_BLOCK_1 = 1, /**< (1) */
    NAND_BLOCK_2 = 2, /**< (2) */
    NAND_BLOCK_3 = 3, /**< (3) */
    NAND_LAST_BLOCK   /**< (4) invalid block */
}t_flash_nand_block;

/** defines if flash detection is made on 8b or 16b bus width only or both */
typedef enum
{
	BOTH_8_16			= 0, /**< (0) bus width is 8 or 16bits */
	EIGHT_BITS_ONLY		= 1, /**< (1) bus width is 8bits only*/
	SIXTEEN_BITS_ONLY	= 2  /**< (2) bus width is 16bits only*/
}t_flash_detection_width;

/** defines ONENAND flash command set */
typedef struct{
    t_uint32 pageXblock; /**< Number of pages in one block */
    t_uint32 pageSize;   /**< Page size */
    t_uint32 blockSize;  /**< Block size */
    t_uint32 spareSize;  /**< Spare area size */
    t_uint32 memorySize; /**< NAND memory size */
    t_uint32 EccSize;    /**< Ecc size */
    t_uint32 dataWidth;  /**< Bus width */
    t_uint32 pmemCommand;     /**< Contains address for read command*/
    t_uint32 pmemAddress[2];  /**< Contains address for address command ([1]attribute cmd)*/
    t_uint32 pattCommand;     /**< Contains address for attribute read command  (timings longer than normal read cmd)*/
    t_uint32 pmemRead;        /**< Contains address for data command */
    t_uint32 memCommand;      /**< Value for read command */
    t_uint32 attCommand;      /**< Value for attribute read command */
    t_uint32 spareCommand;    /**< Value for spare read command */
    t_flash_FSMC_bank memoryBank;  /**< FSMC memory bank to use (NOR or NAND) */
    t_uint8  padding[3];      /**< Unused*/
}t_flash_command_set;    

/** defines flex OneNAND flash property */
typedef struct{
    t_uint32 pageXblockMlc; /**< Number of pages in one block for MLC part. */
    t_uint32 blockSizeMlc;  /**< Block size for MLC part (in bytes).*/
    t_uint32 boundarySlcMlc;  /**< Address boundary between the SLC and MLC parts*/
    t_uint32 slcSize;       /**< Size in bytes of the SLC Part. */
}t_flash_flex_onenand;

/** Flash memory description */
typedef struct{
t_boot_memType		device;         /**< Specify memory type: NAND, ONE_NAND or NOR flash (or MMC/SD)*/
t_uint8             padding;        /**< unused */
t_flash_command_id  readId;        /**< Specify id if the memory found */
t_flash_command_set	commandSet;    /**< Specify which command set must be used */
t_flash_flex_onenand flexOnenand;  /**< Properties of flexOneNand if found */
}t_FlashType;

typedef enum
{
    WIDTH_8=8,
    WIDTH_16=16,
    WIDTH_32=32
}t_data_bus_width;
/*------------------------------------------------------------------------
 * Function definition
 *----------------------------------------------------------------------*/

/* PUBLIC */
PUBLIC t_boot_error FLASH_DetectAndInit(t_FlashType*,t_flash_FSMC_bank, t_flash_detection_width );
PUBLIC void FLASH_ResetGpio(t_boot_memType, t_data_bus_width);
#endif /* !defined(__BOOT_FLASH_h) */
/** @} */
