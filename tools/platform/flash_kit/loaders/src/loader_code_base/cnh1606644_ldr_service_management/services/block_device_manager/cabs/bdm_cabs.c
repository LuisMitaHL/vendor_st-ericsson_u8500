/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup ldr_service_bdm Block Device Manager
 * @{
 *   @addtogroup ldr_service_bdm_cabs CABS Block Device Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "block_device_management.h"
#include "r_cabs.h"
#include "r_cabs_cfs.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "bdm_cabs.h"
#include "flash_manager.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Offset of BDM error codes in loader local error code table. */
#define BDM_CABS_ERROR_OFFSET              1450

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * @brief Starts CABS block device manager
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Start(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;
    CABS_UnitConfig_t CABS_UnitConfig = {0};
    BDM_Config_t BDM_Config = {0};
    CABS_Config_t *CABS_Config_p = {NULL};
    uint32 Counter = 0;

    ReturnValue = Do_FPD_GetPartitionsConfiguration();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_BDM_GetConfig(&BDM_Config);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    VERIFY(TRUE == BDM_Config.Configuration_Set, E_BDM_NOT_CONFIGURED);

    CABS_Config_p = (CABS_Config_t *)malloc(sizeof(CABS_Config_t));
    ASSERT(NULL != CABS_Config_p);

    CABS_Config_p->Version = BDM_Config.Version;
    CABS_Config_p->Units = BDM_Config.Number_Of_Units;

    for (Counter = 0; Counter < BDM_Config.Number_Of_Units; Counter++) {
        CABS_Config_p->Unit[Counter].MediaType                = (CABS_MediaType_t)BDM_Config.Unit_Configuration[Counter].MediaType;
        CABS_Config_p->Unit[Counter].ChipSelect               = BDM_Config.Unit_Configuration[Counter].ChipSelect;
        CABS_Config_p->Unit[Counter].EraseBlockSizeInBytes    = BDM_Config.Unit_Configuration[Counter].EraseBlockSizeInBytes;
        CABS_Config_p->Unit[Counter].PageSizeInBytes          = BDM_Config.Unit_Configuration[Counter].PageSizeInBytes;
        CABS_Config_p->Unit[Counter].RedundantAreaSizeInBytes = BDM_Config.Unit_Configuration[Counter].RedundantAreaInBytes;
        CABS_Config_p->Unit[Counter].StartBlock               = BDM_Config.Unit_Configuration[Counter].StartBlock;
        CABS_Config_p->Unit[Counter].EndBlock                 = BDM_Config.Unit_Configuration[Counter].EndBlock;
        CABS_Config_p->Unit[Counter].LogicalSectors           = BDM_Config.Unit_Configuration[Counter].LogicalSectors;

        CABS_Config_p->Unit[Counter].BadBlockPercentage       = BDM_Config.Unit_Configuration[Counter].BadBlockOverhead;
        CABS_Config_p->Unit[Counter].Bootable                 = BDM_Config.Unit_Configuration[Counter].Attributes & 0x00000001; /* 1st bit is Boot bit */
        CABS_Config_p->Unit[Counter].ReservedBootBlocks       = BDM_Config.Unit_Configuration[Counter].ResevedBootBlocks;

        CABS_Status = Do_CABS_SetConfig(CABS_Config_p);
        VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));

        CABS_UnitConfig.Version                   = BDM_Config.Version;
        CABS_UnitConfig.GarbageThresholdInPercent = 50;
        CABS_UnitConfig.CacheSizeInBytes          = 2 * BDM_Config.Unit_Configuration[Counter].EraseBlockSizeInBytes;
        CABS_UnitConfig.EraseThreshold            = 512;
        CABS_UnitConfig.TrustErasedBlocks         = FALSE;
        CABS_UnitConfig.Format                    = FALSE;

        CABS_Status = Do_CABS_Startup(Counter, &CABS_UnitConfig);

        if (CABS_SUCCESS != CABS_Status) {
            B_(printf("\nbdm_cabs.c(%d): CABS StartUp failed! Format CABS will be performed!\n", __LINE__);)

            CABS_UnitConfig.Version                 = 7; //Version 7 enables �Format�
            CABS_UnitConfig.CacheSizeInBytes        = 256 * 1024;
            CABS_UnitConfig.ValidateWrites          = FALSE;
            CABS_UnitConfig.SynchronizedStartup     = TRUE;
            CABS_UnitConfig.WriteCacheSize          = 128 * 1024;
            CABS_UnitConfig.TrustErasedBlocks       = TRUE; //Same as the �TrustErasedBlocks� parameter in Do_CABS_Format
            CABS_UnitConfig.Format                  = TRUE;

            CABS_Status = Do_CABS_Startup(Counter, &CABS_UnitConfig);
            VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));
        }
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    BUFFER_FREE(CABS_Config_p);

    return ReturnValue;
}

/**
 * @brief Stops CABS block device manager
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Stop(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;
    BDM_Config_t BDM_Config = {0};
    uint32 Counter = 0;

    ReturnValue = Do_BDM_GetConfig(&BDM_Config);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    for (Counter = 0; Counter < BDM_Config.Number_Of_Units; Counter++) {
        CABS_Status = Do_CABS_Shutdown(Counter);
        VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));
    }

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/**
 * @brief Reads data from CABS block device manager
 *
 * @param[in]     Unit        CABS Unit that data should be read from.
 * @param[in]     Pos         CABS sector where reading should start.
 * @param[in]     Length      Number of sectors to read.
 * @param[in,out] FirstBad_p  See r_cabs.h for more info.
 * @param[out]    Data_p      Output buffer.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Read(uint32 Unit, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;

    CABS_Status = Do_CABS_Read(Unit, Pos, Length, FirstBad_p, Data_p);
    VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/**
 * @brief Writes data in CABS block device manager
 *
 * @param[in]     Unit        CABS Unit that data should be written to.
 * @param[in]     Pos         CABS sector where writting should start.
 * @param[in]     Length      Number of sectors to write.
 * @param[in,out] FirstBad_p  See r_cabs.h for more info.
 * @param[in]     Data_p      Input data buffer.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_Write(uint32 Unit, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;

    CABS_Status = Do_CABS_Write(Unit, Pos, Length, FirstBad_p, Data_p);
    VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/**
 * @brief Returns CABS configuration
 *
 * @param[in]  Unit             CABS Unit that configuration is read from.
 * @param[out] Configuration_p  Pointer to returned configuration.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_GetInfo(uint32 Unit, BDM_Unit_Config_t *Configuration_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;
    CABS_Info_t CABS_Info;

    VERIFY(NULL != Configuration_p, E_INVALID_INPUT_PARAMETER);

    CABS_Status = Do_CABS_GetInfo(Unit, &CABS_Info);
    VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));

    Configuration_p->MediaType = CABS_Info.MediaType;
    Configuration_p->EraseBlockSizeInBytes = CABS_Info.EraseBlockSizeInBytes;
    Configuration_p->PageSizeInBytes = CABS_Info.PageSizeInBytes;
    Configuration_p->RedundantAreaInBytes = CABS_Info.RedundantAreaSizeInBytes;
    Configuration_p->StartBlock = CABS_Info.StartBlock;
    Configuration_p->EndBlock = CABS_Info.EndBlock;
    Configuration_p->LogicalSectors = CABS_Info.LogicalSectors;
    //  Configuration_p->GarbageThresholdInPercent = CABS_Info.GarbageThresholdInPercent;
    //  Configuration_p->CacheSizeInBytes = CABS_Info.CacheSizeInBytes;

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/**
 * @brief Returns function pointers needed for file system operation
 *
 * @param[in]  Unit          CABS Unit.
 * @param[out] Device_p      DDB Handle.
 * @param[out] Functions_pp  Returned function pointers.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_CABS_GetFSInterface(uint32 Unit, DdbHandle *Device_p, DdbFunctions **Functions_pp)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;

    CABS_Status = Do_CABS_CFS_GetDdbInterface(Unit, Device_p, Functions_pp);
    VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/**
 * @brief Junks consecutive logical sectors.
 *
 * @param[in]      Unit          CABS Unit.
 * @param[in]      Position      Logical sector number.
 * @param[in]      NrOfSectors   Number of sectors to junk.
 * @param[in, out] FirstBad_p    Pointer to storage of the index of the first
 *                               sector that was not junked when error occurred.
 * @return         E_SUCCESS     Logical sectors was successfully junked.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 */
ErrorCode_e Do_BDM_CABS_Junk(uint32 Unit, uint32 Position, uint32 NrOfSectors, uint32 *FirstBad_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CABS_Status_t CABS_Status = CABS_SUCCESS;

    CABS_Status = Do_CABS_Junk(Unit, Position, NrOfSectors, FirstBad_p);
    VERIFY(CABS_SUCCESS == CABS_Status, (ErrorCode_e)(CABS_Status + BDM_CABS_ERROR_OFFSET));

    ReturnValue = E_SUCCESS;
ErrorExit:

    return ReturnValue;
}

/** @} */
/** @} */
