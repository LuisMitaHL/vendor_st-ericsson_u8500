/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_BOOT_RECORDS_HANDLER_H_
#define _T_BOOT_RECORDS_HANDLER_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup ldr_boot_records_handler
 *    @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_basicdefinitions.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
/** Define maximum number of boot records.*/
#define MAX_NUMBER_OF_BOOTRECORDS  50

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/**
 * @name Boot Records Id values.
 *
 * Values of boot record identifiers (Id's) for every boot record.
 * @{
 */
/** Id of memory configuration boot record. Also defines memory device width.*/
#define MEMCONF_1_ID      0x00000001
/** Id of memory configuration boot record. Also defines memory device width.*/
#define MEMCONF_4_ID      0x00000004
/** Id of memory configuration boot record. Also defines memory device width.*/
#define MEMCONF_8_ID      0x00000008
/** Id of memory configuration boot record. Also defines memory device width.*/
#define MEMCONF_16_ID     0x00000010
/** Id of memory configuration boot record. Also defines memory device width.*/
#define MEMCONF_32_ID     0x00000020
/** Id of SIGNATURE boot record.*/
#define SIGNATURE_ID      0x00010007
/** Id of HEADER object boot record.*/
#define HEADER_ID         0x0001000A
/** Id of PAYLOAD data boot record.*/
#define PAYLOAD_ID        0x0001000B
/** Id of USB ID descriptors boot record.*/
#define USBDESCRIPTOR_ID  0x0001000D
/** Id of domain data object boot record.*/
#define DOMAIN_DATA_ID    0x0001000E
/** Id of root key object boot record.*/
#define ROOTKEY_ID        0x0001000F
/** Id of patch data object boot record.*/
#define PATCH_ID          0x00010010
/** Id of SW revision object boot record.*/
#define BOOTSWREV_ID      0x00010011
/** Id of PARTITIONS boot record.*/
#define PARTITIONS_ID     0x00010040
/** Id of partition configuration boot records.*/
#define PARTITION_ID      0x00010042
/** Id of ENDFIRST boot record.*/
#define ENDFIRST_ID       0x000FFFFF
/** Id of STOP boot record.*/
#define STOP_ID           0xFFFFFFFF

/** @} */

#define MEMCONF_MAGIC     0xEDA96521

/** Flash memory types. */
typedef enum {
    NOR_MEMORY = 0x00000001,
    ONE_NAND   = 0x00000010,
    LPDDRNOR   = 0x00000040,
    MMC_MMC0   = 0x00000080,
    MMC_MMC1   = 0x00000081,
    MMC_MMC2   = 0x00000082,
    MMC_MMC3   = 0x00000083,
    SD_MMC0    = 0x00000100,
    SD_MMC1    = 0x00000101,
    SD_MMC2    = 0x00000102,
    SD_MMC3    = 0x00000103,
    NAND_SMALL = 0x00000200,
    NAND_SMALL_BLOCK0_ONLY = 0x000002FF,
    NAND_LARGE             = 0x00000800,
    NAND_LARGE_BLOCK0_ONLY = 0x000008FF
} FlashTypes_t;

/** Types of different partitions.*/
TYPEDEF_ENUM {
    MBBS = 0x5342424D,
    CABS = 0x53424143,
    TRIM = 0x4D495254
} ENUM32(PartitionTypes_t);

/** Boot record configuration.*/
typedef struct {
    uint32         BR_Id;          /**< Record Id. */
    uint32         PartitionIndex; /**< Partition index. */
    uint32         BR_Size;        /**< Boot record size. */
    uint32         *BRData_p;      /**< Boot record data.*/
} BootRecord_t;

/** MEMCONF boot record.*/
typedef struct {
    /**< Record Id. Also defines memory device width.*/
    uint32  MemconfId;
    /**< Record size in bytes.*/
    uint32  MemconfSize;
    /**< Magic word (0xEDA96521).*/
    uint32  Magic;
    /**< Size of flash page in Bytes.*/
    uint32  PageSize;
    /**< Size of flash erase block in Bytes.*/
    uint32  BlockSize;
    /**< Number of bits in column address.*/
    uint32  ColumnBits;
    /**< Number of address cycles.*/
    uint32  AddrCycles;
    /**< Flash type.*/
    FlashTypes_t  Type;
    /**< Size of entire flash in Bytes.*/
    uint32  TotalSize;
    /**< May be 0 if not applicable. Size is in Bytes.*/
    uint32  RedundantAreaSize;
} Memconf_BootRecord_t;

/** SIGNATURE BOOT RECORD*/
typedef struct {
    uint32         Signature_Id;        /**< Record Id.*/
    uint32         Signature_Size;      /**< Record size in bytes.*/
    uint32         Signature;
} Signature_t;

/** PARTITIONS boot record. */
typedef struct {
    /**< Record Id. */
    uint32 Partitions_Id;
    /**< Size of this boot record. */
    uint32 Partitions_Size;
    /**< Must be 0 as there shall be only one PARTITIONS boot record. */
    uint32 RecordIndex;
    /**< Number of PARTITION boot records that will follow. */
    uint32 NrOfPartitions;
} Partitions_BootRecord_t;

/** PARTITION boot record. */
typedef struct {
    uint32 Record_Id;
    uint32 Size;
    uint32 RecordIndex;
    uint32 PartitionIndex;
    PartitionTypes_t Type;
    uint32 Name;
    uint64 StartInBytes;
    uint64 EndInBytes;
    uint32 Version;
    uint32 Padding;
    uint32 Other[6]; /* MBBS has 3 more params, CABS 5 and TRIM 3 */
} Partition_BootRecord_t;

/* Other field detalied description
 *        MBBS                 CABS                TRIM
 * [0]  BootableCABS     Logical Sectors
 * [1]  BBM Type         BadBlock Overhead
 * [2]  ChipSelect       Reserved BootBlocks
 * [3]  Padding          ChipSelect
 * [4]                   Attributes
 * [5]                   Padding
 */

/** @} */
/** @} */
#endif /*_T_BOOT_RECORDS_HANDLER_H_*/
