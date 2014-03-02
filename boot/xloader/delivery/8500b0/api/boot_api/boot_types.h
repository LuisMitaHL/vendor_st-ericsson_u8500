/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 *
 * \file boot_types.h
 * \brief ROM code's debug traces driver
 * \author STMicroelectronics
 *
 * \addtogroup BOOT
 *
 * This header file provides basic types common to all Boot scenario.
 *
 * @{
 */
/*---------------------------------------------------------------------------*/
#ifndef _BOOT_TYPES_H
#define _BOOT_TYPES_H

#include "boot_secure_types.h"

/* Cut ID and device ID for U8500 Cut B0 (V2) */
#define ROM_CUT_ID_3       0x00    
#define ROM_CUT_ID_2       0x85 
#define ROM_CUT_ID_1       0x00    
#define ROM_CUT_ID_0       0xB0
#define ROM_CUT_ID         0x008500B0

// This type stores the indication below.
#define	ADL_INDICATION			0xF0030003 /**<  Boot indication for ADL indication.*/
#define	PRODUCTION_INDICATION	0xF0030005 /**<  Boot indication for Production indication.*/
#define	NORMAL_INDICATION		0xF0030001 /**<  Boot indication for Normal indication.*/
#define ALT_NORMAL_INDICATION 	0xF0030004 /**<  Boot indication for Alt Normal indication.*/

// FLASHING and PROGRAMMING indication are the same
// This is for compatbility between ROM code and tests
// Now use FLASHING_INDICATION
#define	FLASHING_INDICATION	    0xF0030002 /**<  Boot indication for 2nd boot indication.*/
#define	PROGRAMMING_INDICATION	0xF0030002 /**<  Boot indication for 2nd boot indication.*/

#define MASK_DEBUG_INFO			0xFFFFF8FF	/* masked debug information (bits 8-10) */
#define MASK_DMA_INFO			0xFFFFEFFF	/* masked DMA information (bit  12) */
#define MASK_ALL_INFO			0xFFFFE0FF	/* masked DMA & debug information (bit 12-8) */

#define ASCII_ISSW      "ISSW"
#define ISSW_OFFSET  0x14    /* offset of "X_LOADER" string from beginning of a 512 bytes buffer */
#define ISSW_SIZE    6      /* size of "ISSW" string (including final \0 and +1 byte )*/

#define SEC_FUNC_FLAG_MMU   0x00000001
#define SEC_FUNC_FLAG_IC    0x00000002
#define SEC_FUNC_FLAG_DC    0x00000004
#define SEC_FUNC_FLAG_IMASK 0x00000008
#define SEC_FUNC_FLAG_FMASK 0x00000010

// TOC analysis related values
#define ITEM_NB			5
#define ITEM_NAME_SIZE	12
#define ITEM_MAX_NB		16

/*Asic ID defines*/
#define SEC_ASICID_PUB_ID_SIZE 20
#define SEC_ASICID_ROOT_KEY_HASH_SIZE 20



/** \internal  Structure used to store the boot abort or error cause. */
typedef struct {
	t_uint32 error;      /**<  \internal contains the t_boot_error  or the abort cause.*/
	t_uint32 lrReg;     /**<  \internal contains the LR register*/

}t_boot_error_info;

/** \internal  Structure used to store the boot indication. */
typedef union {
	struct{
		t_bitfield unused1:8;       /**< \internal Bit[7:0]   Unused         */
		t_bitfield TraceEnable :1;  /**< \internal Bit[8]     Trace Enable   */
		t_bitfield unused2:3;       /**< \internal Bit[11:9]  Unused         */    
		t_bitfield UseDma :1;       /**< \internal Bit[12]    Use DMA on 2nd Boot   */    
		t_bitfield unused3:19;      /**< \internal Bit[31:13] Unused         */
	}bit;                   /**< \internal Boot indication in bitfield. */
	t_uint32 scenario;     /**<  \internal contains the boot indication (Normal,ADL, Production...)*/

}t_boot_indication;

/** \internal  Lists the different sources of reset. */	
typedef enum {
    	A9CPU0_WDG_RESET, /**< \internal (0), A9 CPU0 WatchDog reset occurs. */
        A9CPU1_WDG_RESET, /**< \internal (1), A9 CPU1 WatchDog reset occurs. */
        APESW_RESET,      /**< \internal (2), APE software reset occurs. */
        APE_RESET,        /**< \internal (3), APE reset occurs. */
        SEC_WDG_RESET,    /**< \internal (4), Secure WatchDog occurs. */
        POWER_ON_RESET,   /**< \internal (5), Power On reset occurs. */  
        A9_DEEP_SLEEP,    /**< \internal (6), A9 restart from Deep Sleep. */
        APE_DEEP_SLEEP,   /**< \internal (7), APE restart from Deep Sleep. */
        NO_RESET          /**< \internal (8), No Reset source found. */
} t_boot_last_reset;	    
    
/** Type of memory used in normal boot. */
/* Note: this enum should match debug value of bits DbgBootIf in t_secure_rom_boot_info (t_load_if)*/
typedef enum {
    MMC = 2,        /**< (2), MMC card/eMMC */
    ONE_NAND = 3   /**< (3), ONE_NAND/FLEX_ONE_NAND flash */
}t_boot_memType;

/** \internal  This type stores the peripheral used at Boot time 
to send indication, and other data. */	
/* Note: this enum should match debug value of bits DbgBootIf (t_load_if)
in t_secure_rom_boot_info */
typedef enum {
    UART_PERIPHERAL,    /**< \internal (0), UART used.*/
    USB_PERIPHERAL,     /**< \internal (1), USB used.*/
	NO_PERIPHERAL       /**< \internal (2), unused.*/
} t_boot_peripheral ;

/** Boot interface type. */
typedef enum {
    IF_UART = UART_PERIPHERAL,  /**< (0), UART used.*/
    IF_USB = USB_PERIPHERAL,    /**< (1), USB used.*/
    IF_MMC = MMC ,              /**< (2), MMC card/eMMC used. */
    IF_ONE_NAND = ONE_NAND     /**< (3), ONE_NAND/FLEX_ONE_NAND flash used. */
}t_load_if;

/** \internal defines if Memory detection is made on all memories, MMC2/4, or ONE_NAND only */
typedef enum
{
	AUTO_DETECTION = 0,     /**< (0) Detection to be done in this order: ONE_NAND/MMC/NAND/unknownNAND */
    ONENAND_ONLY = 1,       /**< (1) Detection to be done only on ONE_NAND  */
	MMC_ONLY = 2,           /**< (2) Detection to be done only on MMC */
	UNUSED = 3              /**< (3) Not to be used */
}t_mem_detection_type;

/** \internal To define the timeout value for USB after a system reset */
typedef enum
{
	USB_TIME_DEFAULT = 0,     /**< (0) Default: let the value programmed for power on reset */
    USB_TIME_30S = 1,         /**< (1) Set the value to 30s */
    USB_TIME_1MIN = 2,        /**< (2) Set the value to 1min */
    USB_TIME_DISABLE = 3      /**< (3) Disable the timeout*/
}t_usb_timeout;


 /** \internal
 This structure is used to store all boot information read on Antifuse or BootStrap vector at start up time.*/
typedef union {
    struct {
        t_bitfield AntiFuse : 1;        /**< \internal Clear (0) if boot strap vector need to be read.   */           
        t_bitfield MemoryDetection : 2; /**< \internal Define the detection order and type for flash and MMC: (00)AutoDetect (01)OneNand/FlexOneNand (10)MMC (11)Unused. */ 
        t_bitfield UartNumber : 2;      /**< \internal define the UART used during boot sequence (00)UART1, (01)UART2-b, (10)UART2-c (11)No Uart.(see t_uart_number)*/
        t_bitfield UsbEnable : 1;       /**< \internal Set (1) if USB boot scenario is needed.         */
        t_bitfield UsbVendor : 1;       /**< \internal Select the Vendor for USB descriptor: (0)ST.     */  
        t_bitfield PowerMgmt : 1;       /**< \internal Define if boot in normal (0) or in power management mode (1). */
        t_bitfield MMCProg : 2;         /**< \internal Define if MMC is supported and its bus mode: (00)1Bit, (01)4Bit, (10)8Bit, (11)Unused */
        t_bitfield DisableSpeedUp : 1;  /**< \internal If set (1): Speed up execution is disabled. */ 
        t_bitfield PrivRom   : 1;       /**< \internal HW bit, define if priv rom is accessible or not (0)yes  (1)no. */
        t_bitfield MMCPosition : 2;     /**< \internal Define MMC interface position: (0)MMC2_MMC4, (1)MMC2, (2)MMC4.(see t_MmcPosition) */        
        t_bitfield reserved1 : 1;        /**< \internal reserved by security */
        t_bitfield UsbServiceMode1: 1;    /**< \internal  USB Service Mode Enable position 1: 0:Disable  1:Enable */
        t_bitfield UsbServiceMode2: 1;    /**< \internal  USB Service Mode Enable position 2: 0:Disable  1:Enable */
        t_bitfield unused1 : 7;        /*unused */
        t_bitfield OSUsbEnumTimeout : 2;   /**< \internal Set by OS, USB enumeration timeout after system reset: (00)default (01)30s (10)1min (11)disabled*/
        t_bitfield OSUsbFlashTimeout : 2;  /**< \internal Set by OS, USB flashing message timeout after system reset: (00)default (01)30s (10)1min (11)disabled*/
        t_bitfield OSUsbBoot : 1;       /**< \internal Set (1) By OS application if a USB boot is required after a system reset.        */
        t_bitfield OSMmcBoot : 1;       /**< \internal Set (1) By OS application if a MMC boot is required after a system reset.       */
        t_bitfield OSOneNandBoot : 1;   /**< \internal Set (1) By OS application if a OneNand boot is required after a system reset.       */    
        t_bitfield OSEnableDebug : 1;   /**< \internal Set (1) to enable debug trace after software reset. */
        }Bit;                           /**< \internal Antifuse configuration  in bitfield. */
    t_uint32 BootConf;                  /**< \internal Antifuse configuration. */  	    	
} t_secure_rom_boot_info;


 /** \internal
 This structure is used to store all boot information read on Antifuse or BootStrap vector at start up time.*/
typedef union {
    struct {
        t_bitfield DbgUsbVbus : 1;      /**< \internal In case of USB boot, set to (1) if VBUS is detected.   */
        t_bitfield DbgUsbEnumStart : 1; /**< \internal In case of USB boot,set to (1) if enumeration has started (soft connect done).   */
        t_bitfield DbgUsbEnumDone : 1;  /**< \internal In case of USB boot,set to (1) if enumeration is done (set configuration received).   */
        t_bitfield DbgUsbTimeout : 1;   /**< \internal In case of USB boot,set to (1) if timeout for receiving SW image expired.  */
        t_bitfield DbgMmcInit : 1;      /**< \internal Set to (1) if Mmc init succeed.   */
        t_bitfield DbgOneNandInit : 1;  /**< \internal Set to (1) if OneNand init succeed.   */
        t_bitfield DbgBootIf : 3;       /**< \internal Gives on which interface boot has been done: see t_load_if enum value .   */
        t_bitfield DbgImageFound : 1;   /**< \internal Set to (1) if ISSW image is found on 1 of the booting interface.   */
        t_bitfield DbgImageNb : 2;      /**< \internal Set to copy image number (0 to 3) */
        t_bitfield DbgImageLoaded : 1;  /**< \internal Set to (1) if ISSW image is Loaded to internal RAM.   */
        t_bitfield DbgSubImageCopy:15;  /**< \internal Bit_n set to 1 if TOC SubImage_(n+1) is Loaded to internal RAM.   */
        t_bitfield unused: 4;           /**< \internal unused */
        }Bit;                           /**< \internal Boot trace information in bitfield. */
    t_uint32 Reg;                  /**< \internal Boot trace information in word. */  	    	
} t_boot_trace_info;

 /** \internal
 This structure is used to store the ASIC id */
typedef  struct
{
	t_uint8 SubBlockNumber;
	
	/* ID sub block */
	t_uint8 Id_SubBlockId;
	t_uint8 Id_SubBlockSize;
	t_uint8 Id_NomadikSubBlockId;
	t_uint8 Id_NomadikId[3];
	t_uint8 Id_AsicVersion;	
	/* Secure Mode Sub ID */
	t_uint8 Secure_SubBlockId;
	t_uint8 Secure_SubBlockSize;
	t_uint8 Secure_NomadikSubBlockId;
	t_uint8 Secure_ModeInfo;	
	/* Public ID Sub Block */
	t_uint8 PublicId_SubBlockId;
	t_uint8 PublicId_SubBlockSize;
	t_uint8 PublicId_NomadikSubBlockId;
	t_uint8 PublicId_Id[SEC_ASICID_PUB_ID_SIZE];	
	/* Root Key Hash Sub block */
	t_uint8 RootKey_SubBlockId;
	t_uint8 RootKey_SubBlockSize;
	t_uint8 RootKey_NomadikSubBlockId;
	t_uint8 RootKey_Hash[SEC_ASICID_ROOT_KEY_HASH_SIZE];
	/* Checksum Sub Block */
	t_uint8 Checksum_SubBlockId;
	t_uint8 Checksum_SubBlockSize;
	t_uint8 Checksum_NomadikSubBlockId;
	t_uint32 Checksum_PublicRomCrc __attribute__ ((packed));
	t_uint32 Checksum_SecureRomCrc __attribute__ ((packed));
} t_boot_asic_id_desc;


// Image areas list
typedef enum
{
	X_LOADER_ITEM,
	SECOND_ITEM,
	KEYS_ITEM,
	R_AND_D_ITEM,
	PRIMAAPP_ITEM
} t_boot_sub_image_id;

// Description of an image area
typedef struct
{
	t_uint32 StartAddress;
	t_uint32 Length;
	t_uint32 Offset;
} t_boot_data_area;

// Sub image item depiction
typedef struct
{
	t_uint32 Start;
	t_uint32 Size;
	t_uint32 Flags;
	t_uint32 Align;
	t_uint32 Load;
	char	 FileName[12];
} t_boot_toc_item;


typedef t_uint32 t_address; /**< Physical address */

#ifndef HAVE_T_SIZE
#define HAVE_T_SIZE
typedef t_uint32 t_size; /**< Size */
#endif
 
#endif		// _BOOT_TYPES_H
/** @} */
