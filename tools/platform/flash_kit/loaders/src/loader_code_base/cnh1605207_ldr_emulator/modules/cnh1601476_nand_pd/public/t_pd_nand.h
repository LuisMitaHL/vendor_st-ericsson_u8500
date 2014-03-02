/*******************************************************************************
* $Copyright Ericsson AB 2009 $
********************************************************************************
*
* DESCRIPTION:
*
* Public type definitions for NAND PD.
*
*******************************************************************************/

// =============================================================================
#ifndef INCLUSION_GUARD_T_PD_NAND
#define INCLUSION_GUARD_T_PD_NAND
// =============================================================================

// =============================================================================
//  Configurations
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include "c_system_v2.h"
#include "t_basicdefinitions.h"

// =============================================================================
//  Defines
// =============================================================================

/**
*  Defines the number of NAND chip selects in the system.
*/
#define D_PD_NAND_NBR_OF_CHIP_SELECTS             2

/**
*  Defines max number of address cycles.
*/
#define D_PD_NAND_MAX_NBR_OF_ADDRESS_CYCLES       6

/**
*  Defines number of column address cycles.
*/
#define D_PD_NAND_NBR_OF_COLUMN_ADDRESS_CYCLES    2

/**
*  Defines the maximum size of a page.
*/
#define D_PD_NAND_MAX_PAGE_SIZE                  2048

/**
*  Defines the maximum size of a page's extra area.
*/
#define D_PD_NAND_MAX_PAGE_EXTRA_SIZE            64

/**
*  Defines the maximum amount of data to build an ECC value.
*/
#define D_PD_NAND_MIN_ECC_CALC_LENGTH            256

/**
*  Max number of bytes for ECC per chunk of data. For 256 B ECC
*  data length the least significant 22 bits are used (the remaining bits are set to 0),
*   and for 512 B ECC data length all 24 bits are used.
*/
#define D_PD_NAND_MAX_ECC_LENGTH                  3

// =============================================================================
//  Types
// =============================================================================

/**
 * PD_NAND_Result_t defines the possible result of a call to a NAND PD function.
 *
 * @param T_PD_NAND_RESULT_SUCCESS                  Function returned successfully.
 * @param T_PD_NAND_RESULT_BIT_ERROR_CORRECTED      A bit error was detected and corrected.
 * @param T_PD_NAND_RESULT_UNCORRECTABLE_BIT_ERROR  An uncorrectable bit error was detected.
 * @param T_PD_NAND_RESULT_BAD_PARAMETER            The function could not perform the requested operation due to a bad parameter.
 * @param T_PD_NAND_RESULT_HW_ERROR                 A hardware error occurred.
 * @param T_PD_NAND_RESULT_INTERNAL_ERROR           A module internal error has occurred. The module has reach an unexpected state or request.
 * @param T_PD_NAND_RESULT_BUSY                     If T_PD_NAND_CONFIG_FLAG_BLOCKING_MUTEX is not set, a call to NAND PD may return with
 *                                                  this busy flag.
 * @param T_PD_NAND_RESULT_PAGE_ERASED              The read page is blank and contains only 0xFF in both data area and extra data area.
 * @param T_PD_NAND_RESULT_NUMBER_OF_ITEMS          Number of valid states of this type.
 * @param T_PD_NAND_RESULT_UNDEFINED                Represents an undefined value of this type.
 */
TYPEDEF_ENUM {
    T_PD_NAND_RESULT_SUCCESS = 0,
    T_PD_NAND_RESULT_BIT_ERROR_CORRECTED,
    T_PD_NAND_RESULT_UNCORRECTABLE_BIT_ERROR,
    T_PD_NAND_RESULT_BAD_PARAMETER,
    T_PD_NAND_RESULT_HW_ERROR,
    T_PD_NAND_RESULT_INTERNAL_ERROR,
    T_PD_NAND_RESULT_BUSY,
    T_PD_NAND_RESULT_PAGE_ERASED,
    // - - -
    T_PD_NAND_RESULT_NUMBER_OF_ITEMS,
    T_PD_NAND_RESULT_UNDEFINED
} ENUM32(PD_NAND_Result_t);

// -----------------------------------------------------------------------------

/**
 * EccPageSize_t defines the length of data over which an ECC value is calculated. The shorter the value the better error correction within a page, but the more overhead.
 *
 * @param T_PD_NAND_ECC_PAGE_SIZE_256             ECC shall be calculated over 256 bytes. This options cannot be used if T_PD_NAND_CONFIG_FLAG_USE_AUTO_ECC is enabled.
 * @param T_PD_NAND_ECC_PAGE_SIZE_512             ECC shall be calculated over 512 bytes.
 * @param T_PD_NAND_ECC_PAGE_SIZE_NUMBER_OF_ITEMS Number of valid states of this type.
 * @param T_PD_NAND_ECC_PAGE_SIZE_UNDEFINED       Represents an undefined value of this type.
 */
TYPEDEF_ENUM {
    T_PD_NAND_ECC_PAGE_SIZE_256 = 0,
    T_PD_NAND_ECC_PAGE_SIZE_512,
    // - - -
    T_PD_NAND_ECC_PAGE_SIZE_NUMBER_OF_ITEMS,
    T_PD_NAND_ECC_PAGE_SIZE_UNDEFINED
} ENUM8(EccPageSize_t);

// -----------------------------------------------------------------------------

/**
 * PD_NAND_ConfigFlags_t defines run-time flags that can be setup when
 * accessing NAND flash.
 *
 * @param T_PD_NAND_CONFIG_FLAG_ECC_ENABLE          Controls whether ECC correction will be used or not.
 * @param T_PD_NAND_CONFIG_FLAG_USE_DMA             Controls whether NAND PD will use DMA or not.
 * @param T_PD_NAND_CONFIG_FLAG_USE_INTERRUPTS      Controls whether NAND PD will use interrupts or polling. May be useful in error handler or if interrupts does not work properly.
 * @param T_PD_NAND_CONFIG_FLAG_BLOCKING_MUTEX      Controls whether a call to a busy NAND PD will kept pending or not.
 * @param T_PD_NAND_CONFIG_FLAG_CHECK_BEFORE_ERASE  Checks if a block is already erased, before erasing a NAND flash block. May be used by loaders to speed up loading into new NAND flashes.
 * @param T_PD_NAND_CONFIG_FLAG_USE_CACHE           Controls whether to use NAND flash's cache features or not.
 * @param T_PD_NAND_CONFIG_FLAG_USE_AUTO_ECC        Controls whether to use NFC's automatic ECC handling mechanism. This option has no effect
 *                                                  if T_PD_NAND_CONFIG_FLAG_USE_DMA is enabled. If set to '1' then the length of the extra space for each page
 *                                                  can only be set to 4, 8, 16 or 32 bytes, see ExtraDataLength in PD_NAND_Config_t.
 * @param T_PD_NAND_CONFIG_FLAG_UNDEFINED           Represents an undefined value of this type.
 */
TYPEDEF_ENUM {
    T_PD_NAND_CONFIG_FLAG_ECC_ENABLE          = (1 << 0),
    T_PD_NAND_CONFIG_FLAG_USE_DMA             = (1 << 1),
    T_PD_NAND_CONFIG_FLAG_USE_INTERRUPTS      = (1 << 2),
    T_PD_NAND_CONFIG_FLAG_BLOCKING_MUTEX      = (1 << 3),
    T_PD_NAND_CONFIG_FLAG_CHECK_BEFORE_ERASE  = (1 << 4),
    T_PD_NAND_CONFIG_FLAG_USE_CACHE           = (1 << 5),
    T_PD_NAND_CONFIG_FLAG_USE_AUTO_ECC        = (1 << 6),
    T_PD_NAND_CONFIG_FLAG_POLL_AFTER_READ     = (1 << 7),
    T_PD_NAND_CONFIG_FLAG_POLL_AFTER_WRITE    = (1 << 8),
    // - - -
    T_PD_NAND_CONFIG_FLAG_UNDEFINED           = (1 << 9)
} ENUM32(PD_NAND_ConfigFlags_t);

// -----------------------------------------------------------------------------

/**
 * Represents a memory configuration boot record.
 *
 * @param BusWidthInBits        Bus width in bits (8, 16, 32).
 * @param RecordLength          Length of this struct in bytes.
 * @param Magic                 Magic number, 0xEDA96521).
 * @param MemoryType            Memory type.
 * @param Size                  Total size of memory in number of pages.
 * @param BlockSize             Size of an erase block.
 * @param PageSize              Size of a page.
 * @param PageExtraSize         Size of a page's extra/redundant area.
 * @param NbrOfAddressCycles    Number of address cycles used for a NAND flash.
 */
typedef struct {
    uint32 BusWidthInBits;
    uint32 RecordLength;
    uint32 Magic;
    uint32 MemoryType;
    uint32 Size;
    uint32 BlockSize;
    uint32 PageSize;
    uint32 PageExtraSize;
    uint32 NbrOfAddressCycles;
} PD_NAND_MemoryProperties_t;

// -----------------------------------------------------------------------------

/**
 * PD_NAND_Config_t defines the run time configuration to use for an access to NAND flash.
 *
 * @param ChipSelect          Specifies which chip select to use.
 * @param EccPageSize         The amount of data the ECC is calculated from (typically 256 B or 512 B).
 * @param ExtraDataLength     Space reserved for extra data in each page. ECC values will be stored directly after this
 *                            data. If T_PD_NAND_CONFIG_FLAG_USE_AUTO_ECC is enabled then the length of the extra space
 *                            for each page can only be set to 4, 8, 16 or 32 bytes, see ExtraDataLength in PD_NAND_Config_t.
 * @param MaxIterationsForUnrecoverableErrors The maximum number of iterations NAND PD will re-read a page with unrecoverable errors.
 * @param Flags               Run time configuration flags. See PD_NAND_ConfigFlags_t.
 */
typedef struct {
    uint8 ChipSelect;
    EccPageSize_t EccPageSize;
    uint8 ExtraDataLength;
    uint32 MaxIterationsForUnrecoverableErrors;
    PD_NAND_ConfigFlags_t Flags;
} PD_NAND_Config_t;

// -----------------------------------------------------------------------------

/**
 * The ECC, calculated over 512 B, consists of four 24-bit values.
 *
 */
typedef uint8 PD_NAND_Ecc2kPage_t[D_PD_NAND_MAX_PAGE_SIZE/D_PD_NAND_MIN_ECC_CALC_LENGTH][D_PD_NAND_MAX_ECC_LENGTH];

// =============================================================================
#endif // INCLUSION_GUARD_T_PD_NAND
// =============================================================================


