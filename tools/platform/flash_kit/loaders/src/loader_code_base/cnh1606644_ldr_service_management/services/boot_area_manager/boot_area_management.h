/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _BOOT_AREA_MANAGEMENT_H_
#define _BOOT_AREA_MANAGEMENT_H_

/**
 * @file  boot_area_management.h
 * @brief Function and type declarations for boot area management
 *
 * @addtogroup ldr_service_management
 * @{
 *    @addtogroup ldr_service_bam Boot Area Management
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "flash_manager.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Maximum number of boot area units. */
#define MAX_NUMBER_OF_BAM_UNITS  8

#define BAM_BOOT_IMAGE_COPY_SIZE      0x20000
#define BAM_BOOT_BLOCK_SIZE           0x80000
#define BOOT_IMAGE_FIRST_COPY_OFFSET  (0 * BAM_BOOT_IMAGE_COPY_SIZE)
#define BOOT_IMAGE_SECOND_COPY_OFFSET (1 * BAM_BOOT_IMAGE_COPY_SIZE)
#define BOOT_IMAGE_THIRD_COPY_OFFSET  (2 * BAM_BOOT_IMAGE_COPY_SIZE)
#define BOOT_IMAGE_FORTH_COPY_OFFSET  (3 * BAM_BOOT_IMAGE_COPY_SIZE)

#define BAM_BOOT_AREA_V3_MAX_SIZE      ((0x800000) + BAM_BOOT_BLOCK_SIZE + BAM_BOOT_BLOCK_SIZE)

/**
 * Boot area type
 */
typedef enum {
    BOOT_AREA_EMPTY = 0,
    BOOT_AREA_LAYOUT_V2,
    BOOT_AREA_LAYOUT_V3,
    BOOT_AREA_LAYOUT_UNKNOWN
} BootAreaLayout_e;

/**
 * Bad block management type of boot area
 */
typedef enum {
    BBM_NONE = 0,
    BBM_STANDARD,
    BBM_STRICT,
    BBM_UNKNOWN
} BAM_BBM_t;

/**
 * Contains information necessary for proper start up of the BAM.
 *
 * @param ChipSelect           Indicates what chip is select.
 * @param MediaType            Indicates what memory type is being used.
 * @param SubMediaType         Indicates what sub media type is being used.
 * @param EraseBlockSize       Size of erase block expressed in bytes.
 * @param PageSize             Size of page expressed in bytes.
 * @param RedundantAreaSize    Size of redundant area expressed in bytes.
 * @param StartBlock           Address of first byte of the first erase block.
 *                             Must be a multiple of erase block size expressed
 *                             in bytes.
 * @param PagesPerBlock        Pages per block.
 * @param SectorsPerBlock      Sectors per block.
 * @param EndBlockByteAddress  Address of the first byte not accessed by BAM.
 *                             Must be a multiple of erase block size expressed
 *                             in bytes.
 * @param UseBootableCABS      If BAM should use BDM to store some of it's
 *                             blocks.
 * @param BBMType              What kind of bad block management scheme will be
 *                             used for the first few erase blocks of the flash.
 *                             This is only applicable if MediaType is NAND, for
 *                             NOR it should be set to BBM_NONE.
 */
typedef struct {
    uint32    ChipSelect;   /* @TODO: New members, check if they are necessary! */
    uint32    MediaType;
    uint32    SubMediaType;   /* @TODO: New members, check if they are necessary! */
    uint32    EraseBlockSize;
    uint32    PageSize;
    uint32    RedundantAreaSize;
    uint64    StartBlock;
    uint32    PagesPerBlock;
    uint32    SectorsPerBlock;   /* @TODO: Do we need this member? */
    uint64    EndBlockByteAddress;
    boolean   UseBootableCABS;   /* @TODO: New members, check if they are necessary! */
    BAM_BBM_t BBMType;
} BAM_Unit_Config_t;

/**
 * Contains information necessary for proper start up of the BAM.
 *
 * @param Version               Version of BAM configuration.
 * @param Number_Of_Units       Number of units configured.
 * @param Unit_Configuration[MAX_NUMBER_OF_BAM_UNITS] Configuration for each
 *                                                    unit.
 * @param Configuration_Set     Set to TRUE if BAM is initialized.
 */
typedef struct {
    uint32 Version;
    uint32 Number_Of_Units;
    BAM_Unit_Config_t Unit_Configuration[MAX_NUMBER_OF_BAM_UNITS];
    boolean Configuration_Set;
} BAM_Config_t;

/**
 * Contains BAM function pointers.
 */
typedef struct {
    ErrorCode_e(*BAM_Start)(void);
    ErrorCode_e(*BAM_UnregisteredStart)(void);
    ErrorCode_e(*BAM_Stop)(void);
    ErrorCode_e(*BAM_Read)(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize);
    ErrorCode_e(*BAM_UnregisteredReadBootArea)(uint8 **Data_pp, uint32 *DataSize_p);
    ErrorCode_e(*BAM_Write)(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize);
    ErrorCode_e(*BAM_WriteBootArea)(uint8 *Data_p, uint32 DataSize);
    ErrorCode_e(*BAM_CompleteErase)(FPD_Config_t *const FPD_Config_p);
    ErrorCode_e(*BAM_GetBlockSize)(FPD_Config_t *const FPD_Config_p, uint32 Reserved, uint32 *BlockSize_p);
} BAM_Functions_t;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Sets valid pointer functions.
 */
void Do_BAM_Init(void);

/**
 * Starts up the BAM (Boot Area Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_Start(void);

/**
 * Starts up the BAM (Boot Area Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_UnregisteredStart(void);

/**
 * Stops the BAM (Boot Area Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_Stop(void);

/**
 * Used for reading data from a block.
 *
 * @param [in]  FPDConfig_p Flash device configuration of BAM
 * @param [in]  Offset   Offset, within the block, which specifies where reading
 *                       of data should start.
 * @param [out] Data_p   Pointer to memory block where the data should be placed.
 * @param [in]  DataSize Size, of the data that should be read, in bytes.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_Read(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize);

/**
 * Used for reading data from a block without registering service.
 * Used ONLY by BootRecords Handler!!!
 *
 * @param [out] Data_p     Pointer to memory block where the data will be placed.
 * @param [in]  DataSize_p Size of the data that should be read, in bytes for
 *                         the specified block number.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_UnregisteredReadBootArea(uint8 **Data_pp, uint32 *DataSize_p);

/**
 * Used for writing data to a specified block.
 *
 * @param [in] FPDConfig_p Flash device configuration of BAM
 * @param [in] Offset   Offset, within the block, which specifies where writing
 *                      of data should start.
 * @param [in] Data_p   Pointer to memory block where data, that should be
 *                      written, is placed.
 * @param [in] DataSize Size of the data that should be written.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_Write(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize);

/**
 * Write boot records data.
 *
 * @param [in]  FPDConfig_p Flash device configuration of BAM
 * @param [in]  Data_p   Input buffer.
 * @param [in]  DataSize Bytes to write.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_WriteBootArea(FPD_Config_t *const FPD_Config_p, uint8 *Data_p, uint32 DataSize);

/**
* Erases the whole area for the cpecified Unit.
*
* @param [in]  FPDConfig_p Flash device configuration of BAM
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BAM_CompleteErase(FPD_Config_t *const FPD_Config_p);

/**
 * Retreives information about the BAM that is currently used.
 *
 * @param [in]  FPDConfig_p Flash device configuration of BAM
 * @param [out] Config_p Pointer to memory where the retrieved information will
 *                       be placed.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_GetConfig(FPD_Config_t *const FPD_Config_p, BAM_Config_t *Config_p);

/**
 * Configures the BAM that should be started.
 *
 * @param [in] NewConfig_p  Contains necessary information for configuring the
 *                          BAM.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_SetConfig(BAM_Config_t *const NewConfig_p);

/**
 * Query BAM unit for the size of a specific Reserved Block.
 *
 * @param [in] FPDConfig_p  Flash device configuration of BAM
 * @param [in] Reserved     Reserved block to get size for.
 * @param [out] BlockSize_p Size of the Reserved block.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_GetBlockSize(FPD_Config_t *const FPD_Config_p, uint32 Reserved, uint32 *BlockSize_p);

/** @} */
/** @} */
#endif /*_BOOT_AREA_MANAGEMENT_H_*/
