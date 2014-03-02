/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _BLOCK_DEVICE_MANAGEMENT_H_
#define _BLOCK_DEVICE_MANAGEMENT_H_

/**
 * @file  block_device_management.h
 * @brief Function and type declarations for block device manager
 *
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_bdm Block Device Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "flash_manager.h"
#ifdef CFG_ENABLE_APPLICATION_FILE_SYS
#include "ddb_os_free.h"
#endif //CFG_ENABLE_APPLICATION_FILE_SYS

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Maximum number of block device units. */
#define MAX_NUMBER_OF_BDM_UNITS 16

/**
 * Contains information necessary for proper start up of the BDM Units.
 *
 * @param MediaType              Indicates what memory type is being used.
 * @param EraseBlockSizeInBytes  Size of erase block expressed in bytes.
 * @param PageSizeInBytes        Size of pages expressed in bytes.
 * @param RedundantAreaInBytes   Size of redundant area expressed in bytes for
 *                               NAND, set to 0 for NOR.
 * @param StartBlock             Address of first byte of the first erase block.
 *                               Must be a multiple of erase block size
 *                               expressed in bytes.
 * @param EndBlock               Address of next byte after the last erase block.
 *                               Must be a multiple of erase block size
 *                               expressed in bytes.
 * @param LogicalSectors         Number of sectors available for storage.
 * @param BadBlockOverhead       Blocks that are expected to go bad.
 * @param ResevedBootBlocks      The number of blocks that should be reserved
 *                               for BAM blocks.
 * @param ChipSelect             Indicates what chip is select.
 * @param Attributes             Attributes.
 */
typedef struct {
    uint32  MediaType;
    uint32  EraseBlockSizeInBytes;
    uint32  PageSizeInBytes;
    uint32  RedundantAreaInBytes;
    uint64  StartBlock;
    uint64  EndBlock;
    uint32  LogicalSectors;
    uint32  BadBlockOverhead;
    uint32  ResevedBootBlocks;
    uint32  ChipSelect;
    uint32  Attributes;
} BDM_Unit_Config_t;

/**
 * Contains information necessary for proper start up of the BDM.
 *
 * @param Version                Version of BDM configuration.
 * @param Number_Of_Units        Number of units configured.
 * @param Unit_Configuration[MAX_NUMBER_OF_BDM_UNITS] Configuration for each
 *                                                    unit.
 * @param Configuration_Set      Set to TRUE if BDM is initialized.
 */
typedef struct {
    uint32 Version;
    uint32 Number_Of_Units;
    BDM_Unit_Config_t Unit_Configuration[MAX_NUMBER_OF_BDM_UNITS];
    boolean Configuration_Set;
} BDM_Config_t;

/**
 * Contains BDM function pointers.
 */
typedef struct {
    ErrorCode_e(*BDM_Start)(void);
    ErrorCode_e(*BDM_Stop)(void);
    ErrorCode_e(*BDM_Read)(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);
    ErrorCode_e(*BDM_Write)(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);
    ErrorCode_e(*BDM_GetInfo)(FPD_Config_t *const FPDConfig_p, BDM_Unit_Config_t *Configuration_p);
    ErrorCode_e(*BDM_Junk)(FPD_Config_t *const FPDConfig_p, uint32 Position, uint32 NrOfSectors, uint32 *FirstBad_p);
#ifdef CFG_ENABLE_APPLICATION_FILE_SYS
    ErrorCode_e(*BDM_GetFSInterface)(uint32 Unit, DdbHandle *Device_p, DdbFunctions **Functions_pp);
#endif //CFG_ENABLE_APPLICATION_FILE_SYS
} BDM_Functions_t;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Sets valid pointer functions.
 */
void Do_BDM_Init(void);

/**
 * Starts up the BDM (Block Device Management). All Units.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Start(void);

/**
 * Stops the BDM (Block Device Management). All Units.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Stop(void);

/**
 * Used for reading data from a block.
 *
 * @param [in]      FPDConfig_p Flash device configuration of BDM
 * @param [in]      Pos        BDM sector that shpuld be read.
 * @param [in]      Length     Number of sectors to read.
 * @param [in, out] FirstBad_p Pointer to storage of the index of the first
 *                             sector that was not read when error occurred.
 * @param [out]     Data_p     Pointer to memory block where the data should be placed.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Read(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);

/**
 * Used for writing data to a block.
 *
 * @param [in]      FPDConfig_p Flash device configuration of BDM
 * @param [in]      Pos        BDM sector that shpuld be written.
 * @param [in]      Length     Number of sectors to write.
 * @param [in, out] FirstBad_p Pointer to storage of the index of the first
 *                             sector that was not written when error occurred.
 * @param [in]      Data_p     Pointer to memory block where the data is placed.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Write(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p);

/**
 * Junks consecutive logical sectors.
 *
 * @param[in]      FPDConfig_p Flash device configuration of BDM
 * @param[in]      Position    Logical sector number.
 * @param[in]      NrOfSectors Number of sectors to junk.
 * @param[in, out] FirstBad_p  Pointer to storage of the index of the first
 *                             sector that was not junked when error occurred.
 *                             \c NOTE: The logical sector referred to by
 *                             \c Position is at index 0, the next sector is at
 *                             index 1 and so on.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Junk(FPD_Config_t *const FPDConfig_p, uint32 Position, uint32 NrOfSectors, uint32 *FirstBad_p);

/**
 * Retreives information about the BDM that is currently used.
 *
 * @param [in]  FPDConfig_p Flash device configuration of BDM
 * @param [out] Config_p Pointer to memory where the retrieved information will
 *                       be placed.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_GetConfig(FPD_Config_t *const FPDConfig_p, BDM_Config_t *Config_p);

/**
* Configures the BDM that should be started.
*
* @param [in] NewConfig_p   Contains necessary information for configuring the
*                           BDM.
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BDM_SetConfig(BDM_Config_t *const NewConfig_p);

/**
 * Returns BDM's Unit information
 *
 * @param [in]  FPDConfig_p      Flash device configuration of BDM
 * @param [out] Configuration_p  Pointer to returned configuration.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_GetInfo(FPD_Config_t *const FPDConfig_p, BDM_Unit_Config_t *Configuration_p);

#ifdef CFG_ENABLE_APPLICATION_FILE_SYS
/**
 * Retreives function pointers for FS.
 *
 * @param[in]  Unit          BDM unit that should be used.
 * @param[out] Device_p      DDB Handle.
 * @param[out] Functions_pp  Returned function pointers.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_GetFSInterface(uint32 Unit, DdbHandle *Device_p, DdbFunctions **Functions_pp);
#endif //CFG_ENABLE_APPLICATION_FILE_SYS

/** @} */
/** @} */
#endif /*_BLOCK_DEVICE_MANAGEMENT_H_*/
