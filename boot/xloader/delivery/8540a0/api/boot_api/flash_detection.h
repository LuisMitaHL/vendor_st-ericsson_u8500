/*-----------------------------------------------------------------------------
 * ST Ericsson
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by ST Ericsson.
 *---------------------------------------------------------------------------*/
/** \addtogroup FLASH
 * @{
 */ 
/*---------------------------------------------------------------------------*/
/** \file flash_detection.h
 *  \author ST Ericsson
 *  \brief This module provides functions and data structures declaration used 
 *         to implement the Flash detection driver
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

#define ST_MAKER_CODE 		0x20  /**< \internal ST maker code used to detect One Nand */
#define SAMSUNG_MAKER_CODE	0xEC  /**< \internal Samsung maker code used to detect One Nand */

/* ----------- COMMANDS VALUES ----------- */
#define READ_VALUE_FIRST	        0x00
#define READ_VALUE_SECOND	        0x30
#define READ_SPARE_AREA             0x50
#define READ_FOR_COPY_BACK_FIRST	0x00
#define READ_FOR_COPY_BACK_SECOND	0x35
#define READ_ID			            0x90
#define RESET			            0xFF
#define PAGE_PROGRAM_FIRST	        0x80
#define PAGE_PROGRAM_SECOND	        0x10
#define CACHE_PROGRAM_FIRST	        0x80
#define CACHE_PROGRAM_SECOND	    0x15
#define COPY_BACK_FIRST		        0x85
#define COPY_BACK_SECOND	        0x10
#define BLOCK_ERASE_FIRST	        0x60
#define BLOCK_ERASE_SECOND	        0xD0
#define RANDOM_DATA_INPUT	        0x85
#define RANDOM_DATA_OUTPUT_FIRST    0x05
#define RANDOM_DATA_OUTPUT_SECOND   0xE0
#define READ_STATUS		            0x70
#define READ_ONFI_PARAMETER			0xEC

/* ---------- ONFI macros ------------ */

/* ONFI ID */
#define ONFI_ID0	0x4F
#define ONFI_ID1	0x4E
#define ONFI_ID2	0x46
#define ONFI_ID3	0x49

#define NB_MAX_PARAM_PAGE 10
#define CRC_INIT_VALUE 0x4F4E
#define CRC_POLYNOM 0x8005

#define ASCII_X_LOADER "X-LOADER"
#define XLOADER_OFFSET  0x14    /* offset of "X_LOADER" string from beginning of a 512 bytes buffer */
#define XLOADER_SIZE    10      /* size of "X_LOADER" string (including final \0 and +1 byte )*/

/* ----------- COMMANDS ADDRESS ----------- */

/* --------FSMC--PMEM  0------ */
#define FSMC_PMEM0_COMMAND		(NAND_FLASH_CS0_START_ADDR + 0x00010000)
#define FSMC_PMEM0_ADDRESS		(NAND_FLASH_CS0_START_ADDR + 0x00020000)
#define FSMC_PMEM0_READ			NAND_FLASH_CS0_START_ADDR

/* --------FSMC--PATT 0------ */
#define FSMC_PATT0_COMMAND		(NAND_FLASH_CS0_START_ADDR + 0x08010000)
#define FSMC_PATT0_ADDRESS		(NAND_FLASH_CS0_START_ADDR + 0x09000000)
#define FSMC_PATT0_READ			(NAND_FLASH_CS0_START_ADDR + 0x08000000)

/** defines if flash detection is made on 8b or 16b bus width only or both */
typedef enum
{
    WIDTH_8=8,      /**< (8) bus width is 8 or 16bits */
    WIDTH_16=16,    /**< (16) bus width is 8bits only */ 
    WIDTH_32=32     /**< (32) bus width is 32bits only */
}t_data_bus_width;

/** defines a list of 'device codes' used to identify a particular device*/
typedef enum
{     /* LARGE PAGES */
    NAND_512ML		= 0x02, /**< (0xA2,0xB2,0xF2,0xC2) device code for 512M Large page NAND */
    NAND_1GL		= 0x01, /**< (0xA1,0xB1,0xF1,0xC1) device code for 1G Large page NAND */
    NAND_2GL		= 0x0A, /**< (0xAA,0xBA,0xFA,0xCA) device code for 2G Large page NAND */
    NAND_4GL		= 0x0C, /**< (0xAC,0xBC,0xFC,0xCC) device code for 4G Large page NAND */
    NAND_8GL		= 0x03, /**< (0xA3,0xB3,0xF3,0xC3) device code for 8G Large page NAND */
    NAND_16GL		= 0x05, /**< (0xA5,0xB5,0xF5,0xC5) device code for 16G Large page NAND */
    NAND_32GL		= 0x07, /**< (0xA7,0xB7,0xF7,0xC7) device code for 32G Large page NAND */
    NAND_64GL		= 0x0E, /**< (0xAE,0xBE,0xFE,0xCE) device code for 64G Large page NAND */  
    ONE_NAND_SLC = -4, /**< (-4) code used for ONE_NAND_SLC */
    FLEX_ONE_NAND = -5, /**< (-5) code used for FLEX_ONE_NAND */
	NAND_ONFI = -6, /**< (-6) code used for NAND ONFI*/
    NO_VALID_ID	= 0, /**< (0) device code invalid */
    UNKNOWN_ID	= -3 /**< (-3) device code unknown */
}t_flash_command_id;

typedef enum
{  
    /* LARGE PAGES */
    NAND_1V8_8		= 0xA0, /**< \internal (0xA2,0xA1,0xAA,0xAC,0xA3,0xA5,0xA7,0xAE) device code for 1.8V and x8 NAND*/
    NAND_1V8_16		= 0xB0, /**< \internal (0xB2,0xB1,0xBA,0xBC,0xB3,0xB5,0xB7,0xBE) device code for 1.8V and x16 NAND */
    NAND_3V_8		= 0xF0, /**< \internal (0xF2,0xF1) device code for 3V and x8 NAND */
    NAND_3V_8_b 	= 0xD0, /**< \internal (0xDA,0xDC,0xD3,0xD5,0xD7,0xDE) device code for 3V and x8 NAND */
    NAND_3V_16		= 0xC0  /**< \internal (0xC2,0xC1,0xCA,0xCC,0xC3,0xC5,0xC7,0xCE) device code for 3V and x16 NAND */
}t_flash_nand_device_id;

/** define on which FSMC BANK the memory is connected*/
typedef enum
{
    NOR_BANK_0 = 0, /**< (0) NOR bank 0 */
	NAND_BANK_0 = 1, /**< (1) NAND bank 0 */
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


#define NAND_INVALID_BLOCK NAND_LAST_BLOCK

/** defines if flash detection is made on 8b or 16b bus width only or both */
typedef enum
{
	BOTH_8_16			= 0, /**< (0) bus width is 8 or 16bits */
	EIGHT_BITS_ONLY		= 1, /**< (1) bus width is 8bits only*/
	SIXTEEN_BITS_ONLY	= 2  /**< (2) bus width is 16bits only*/
}t_flash_detection_width;

/** Defines ONENAND flash command set */
typedef struct{
    t_uint32 pageXblock; /**< Number of pages in one block */
    t_uint32 pageSize;   /**< Page size */
    t_uint32 blockSize;  /**< Block size */
    t_uint32 spareSize;  /**< Spare area size */
    t_uint32 memorySize32b; /**< NAND memory size */
    t_uint32 EccSize;    /**< Ecc size */
    t_data_bus_width dataWidth;  /**< Bus width */
    t_uint32 pmemCommand;     /**< Contains address for read command*/
    t_uint32 pmemAddress[2];  /**< Contains address for address command ([1]attribute cmd)*/
    t_uint32 pattCommand;     /**< Contains address for attribute read command  (timings longer than normal read cmd)*/
    t_uint32 pmemRead;        /**< Contains address for data command */
    t_uint32 memCommand;      /**< Value for read command */
    t_uint32 attCommand;      /**< Value for attribute read command */
    t_uint32 spareCommand;    /**< Value for spare read command */
    t_uint32 NbOfLogicalPage; /**< Number of logical page */
    t_flash_FSMC_bank memoryBank;  /**< FSMC memory bank to use (NOR or NAND) */
    t_uint8  padding[3];      /**< Unused*/
}t_flash_command_set;    

/** Defines flex OneNAND flash property */
typedef struct{
    t_uint32 pageXblockMlc; /**< \internal Number of pages in one block for MLC part. */
    t_uint32 blockSizeMlc;  /**< \internal Block size for MLC part (in bytes).*/
    t_uint32 boundarySlcMlc;  /**< \internal Address boundary between the SLC and MLC parts*/
    t_uint32 slcSize;       /**< \internal Size in bytes of the SLC Part. */
}t_flash_flex_onenand;

/** Flash memory description */
typedef struct{
t_boot_memType		device;         /**< Specify memory type: NAND, ONE_NAND or NOR flash (or MMC/SD)*/
t_uint8             padding;        /**< unused */
t_flash_command_id  readId;        /**< Specify id if the memory found */
t_flash_command_set	commandSet;    /**< Specify which command set must be used */
t_flash_flex_onenand flexOnenand;  /**< Properties of flexOneNand if found */
}t_FlashType;

/** \internal ONFI flash parameter description */
/* some fields can be developped in bit field */
typedef struct {
 t_bitfield  DataWidth:1;
 t_bitfield  MulLun:1;
 t_bitfield  NonSeqProg:1;
 t_bitfield  MultiPlanEraseOp:1;
 t_bitfield  OddEvenCopy:1;
 t_bitfield  SourceSync:1;
 t_bitfield  MultiPlaneReadOp:1;
 t_bitfield  ExtParamPage:1;
 t_bitfield  ProgPageRegcleanEnh:1;
 t_bitfield  EzNand:1;
 t_bitfield   Res:6;
}t_Features;

typedef struct {
 t_uint8	Code[4];              // ONFI Code   -- Revision information and features block
 t_uint8    RevNum[2];            // Supported ONFI Revision number   BF
 t_uint8	SupportdFeatures[2];     // Features supported  BF
 t_uint8    OptionalCmd[2];       // Optional commands supported   BF
 t_uint8	Res0[2];
 t_uint8    ExtParamPageLen[2];   // Extended parameter page length
 t_uint8    NbParamPage;          // Number of parameter pages
 t_uint8    Res1[17];
 }t_OnfiRevInfo;
 
 typedef struct {
 t_uint8    ManufactId[12];       // Device manufacturer (12 ASCII characters)  -- Manufacturer information block
 t_uint8    DeviceMod[20];        // Device model (20 ASCII characters)
 t_uint8    JedecId;              // JEDEC manufacturer ID
 t_uint8    DataCode[2];          // Date code
 t_uint8    Res2[13];
 }t_OnfiManufacturerInfo;
 
 typedef struct {
 t_uint8    NbBytePage[4];        // number of data bytes per page  (pow of 2)  -- Memory organization block
 t_uint8    NbSpareBytePage[2];   //  number of sapre bytes per page 
 t_uint8    Obsolete0[6];
 t_uint8    NbPageBlock[4];       // Number of pages per block
 t_uint8    NbBlockLUN[4];        // Number of block per LUN
 t_uint8    NbLUN;                // Number of LUNs
 t_uint8    NbAddCycle;           // Number of column and row add cycles   BF
 t_uint8    NbBitCell;            // Number of bits per cell
 t_uint8    MaxBadBlock[2];       // Max bad block per LUN
 t_uint8    BlockEndurance[2];
 t_uint8    ValidBlock;           // Guaranted valid blocks at beginning of target
 t_uint8    BlockEnduranceGuaranted[2];  // Block endurance for guaranteed valid blocks
 t_uint8    NbPgm;                // Number of programs per page
 t_uint8    Obsolete1;
 t_uint8    ECCbitCorrect;        // Number of bits ECC correctability
 t_uint8    NbAddBitPlane;        // Number of plane address bits   BF
 t_uint8    MultiPlaneAtt;        // Multi-plane operation attributes   BF
 t_uint8    SupportEZ;            // EZ NAND support  BF
 t_uint8    Res3[12];             // "128 bytes"
 }t_OnfiMemOrganisation;
 
 typedef struct {
 t_uint8    IOCapa;               // I/O pin capacitance, maximum   -- Electrical parameters block
 t_uint8    AsyncTiming[2];       // Asynchronous timing mode support    BF
 t_uint8    Obsolete2[2];
 t_uint8    Tprog[2];             // tPROG Maximum page program time (μs)
 t_uint8    Tbers[2];             // tBERS Maximum block erase time (μs)
 t_uint8    Tr[2];                // tR Maximum page read time (μs)
 t_uint8    Tccs[2];              // tCCS Minimum change column setup time (ns)
 t_uint8    SourceSyncTiming[2];  // Source synchronous timing mode support    BF
 t_uint8    SourceSyncFeatures;   // Source synchronous features     BF
 t_uint8    CLKInputCap[2];       // CLK input pin capacitance, typical
 t_uint8    InOutPinCap[2];       // I/O pin capacitance, typical
 t_uint8    InPinCap[2];          // Input pin capacitance, typical
 t_uint8    InPinCapMax;          // Input pin capacitance, maximum
 t_uint8    DriverStrength;       // Driver strength support    BF
 t_uint8    TrMax[2];             // tR Maximum multi-plane page read time (μs)
 t_uint8    Tadl[2];              // tADL Program page register clear enhancement tADL value (ns)
 t_uint8    TrPageRead[2];        // tR Typical page read time for EZ NAND (μs)
 t_uint8    Res4[6];
 }t_OnfiElecParam;
 
 typedef struct {
 t_uint8    VendorRevNb[2];       // Vendor specific Revision number -- Vendor block
 t_uint8    VendorSpecific[88];   // Vendor specific
 t_uint8    IntegrityCRC[2];      // Integrity CRC
}t_OnfiVendorInfos;

 typedef struct 
{
 t_OnfiRevInfo  RevisionInfos;
 t_OnfiManufacturerInfo ManufacturerInfos;
 t_OnfiMemOrganisation MemOrganisation;
 t_OnfiElecParam ElectricalParam;
 t_OnfiVendorInfos VendorInfos;
 }t_FlashOnfiType;

/*------------------------------------------------------------------------
 * Function definition
 *----------------------------------------------------------------------*/

/* PUBLIC */
PUBLIC t_boot_error FLASH_DetectAndInit(t_FlashType*,t_flash_FSMC_bank, t_flash_detection_width );
PUBLIC void FLASH_ResetGpio(t_boot_memType, t_data_bus_width);
#endif /* !defined(__BOOT_FLASH_h) */
/** @} */
