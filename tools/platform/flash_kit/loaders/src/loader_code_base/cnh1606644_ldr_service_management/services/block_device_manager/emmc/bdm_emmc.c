/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/**
 * @file  bdm_emmc.c
 * @brief Function definitions for EMMC block device manager
 *
 * @addtogroup ldr_service_bdm Block Device Manager
 * @{
 *   @addtogroup ldr_service_bdm_emmc EMMC Block Device Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "hcl_defs.h"
#include "memory_mapping.h"
#include "mmc.h"

#include "flash_manager.h"
#include "block_device_management.h"
#include "bdm_emmc.h"
#include "fm_emmc.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/*******************************************************************************
 * External data
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * @brief Starts EMMC block device manager
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Start(void)
{
    uint8 i;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 available_flash_devices = Do_FPD_GetNrOfAvailableFlashDevices();

    // Since we dont know which device to start here return success if at least one is started
    for (i = 0; i < available_flash_devices; i++) {
        if (Do_FPD_IsStarted(i)) {
            ReturnValue = E_SUCCESS;
        } else {
            A_(printf("BDM:% could not be started\n", i);)
        }
    }

    return ReturnValue;
}

/**
 * @brief Stops EMMC block device manager
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Stop(void)
{
    return E_SUCCESS;
}

/**
 * @brief Reads data from EMMC block device manager
 *
 * @param[in]     FPDConfig_p Flash device configuration of BDM
 * @param[in]     Pos         EMMC sector where reading should start.
 * @param[in]     Length      Number of sectors to read.
 * @param[in,out] FirstBad_p  Not used for eMMC.
 * @param[out]    Data_p      Output buffer.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_Read(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p)
{
    ErrorCode_e     ReturnValue = E_GENERAL_FATAL_ERROR;
    t_mmc_error     MMC_Error;
    void *TmpData_p = Data_p;

    IDENTIFIER_NOT_USED(FirstBad_p);

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    IS_ALIGNED((uint32)Data_p);

    // Jedec 84-A44 : Size < 2GB => byte addressing
    if (!FPDConfig_p->HC_Card) {
        Pos *= MMC_ADDRESS_MODE_FACTOR;
    }

    // Typecast on Data_p OK, alignment checked above
    MMC_Error = MMC_ReadBlocks(1, Pos, (uint32 *)TmpData_p, FPDConfig_p->PageSize, Length, FPDConfig_p->DeviceId);
    VERIFY(MMC_OK == MMC_Error, E_BDM_READ_FAILED);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * @brief Writes data in EMMC block device manager
 *
 * @param[in]     FPDConfig_p Flash device configuration of BDM
 * @param[in]     Pos         EMMC sector where writting should start.
 * @param[in]     Length      Number of sectors to write.
 * @param[in,out] FirstBad_p  Not used for eMMC.
 * @param[in]     Data_p      Input data buffer.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.*
 */
ErrorCode_e Do_BDM_EMMC_Write(FPD_Config_t *const FPDConfig_p, uint32 Pos, uint32 Length, uint32 *FirstBad_p, uint8 *Data_p)
{
    ErrorCode_e     ReturnValue = E_GENERAL_FATAL_ERROR;
    t_mmc_error     MMC_Error;
    void *TmpData_p = Data_p;

    IDENTIFIER_NOT_USED(FirstBad_p);

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    IS_ALIGNED((uint32)Data_p);

    // Jedec 84-A44 : Size < 2GB => byte addressing
    if (!FPDConfig_p->HC_Card) {
        Pos *= MMC_ADDRESS_MODE_FACTOR;
    }

    // Typecast on Data_p OK, alignment checked above
    MMC_Error = MMC_WriteBlocks(1, Pos, (uint32 *)TmpData_p, FPDConfig_p->PageSize, Length, FPDConfig_p->DeviceId, FALSE);
    VERIFY(MMC_OK == MMC_Error, E_BDM_WRITE_FAILED);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * @brief Returns EMMC configuration
 *
 * @param[in]  FPDConfig_p      Flash device configuration of BDM
 * @param[out] Configuration_p  Pointer to returned configuration.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 *
 * @remark This function should be called only from BDM module.
 */
ErrorCode_e Do_BDM_EMMC_GetInfo(FPD_Config_t *const FPDConfig_p, BDM_Unit_Config_t *Configuration_p)
{
    Configuration_p->MediaType              = 0;                          // TODO TBD
    Configuration_p->EraseBlockSizeInBytes  = FPDConfig_p->EraseBlockSize;  // TODO TBD
    Configuration_p->PageSizeInBytes        = FPDConfig_p->PageSize;        // TODO TBD
    Configuration_p->RedundantAreaInBytes   = FPDConfig_p->PageExtraSize;   // TODO TBD
    Configuration_p->StartBlock             = 0;                          // TODO TBD
    Configuration_p->EndBlock               = FPDConfig_p->DeviceSize;      // TODO TBD
    //  Configuration_p->LogicalSectors         = 0;                          // TODO TBD
    Configuration_p->LogicalSectors         = (uint32)(FPDConfig_p->DeviceSize / FPDConfig_p->PageSize); /* XVSZOAN For Test! */
    Configuration_p->BadBlockOverhead       = 0;                          // TODO TBD
    Configuration_p->ResevedBootBlocks      = 0;                          // TODO TBD
    Configuration_p->ChipSelect             = 0;                          // TODO TBD
    Configuration_p->Attributes             = 0;                          // TODO TBD

    return E_SUCCESS;
}

/**
 * @brief Junks consecutive logical sectors.
 *
 * @param[in]      FPDConfig_p   Flash device configuration of BDM
 * @param[in]      Position      Logical sector number.
 * @param[in]      NrOfSectors   Number of sectors to junk.
 * @param[in, out] FirstBad_p    Not used for eMMC.
 * @return         E_SUCCESS     Logical sectors was successfully junked.
 *
 * @return ErrorCode_e Forwards the response from CABS module.
 */
ErrorCode_e Do_BDM_EMMC_Junk(FPD_Config_t *const FPDConfig_p, uint32 Position, uint32 NrOfSectors, uint32 *FirstBad_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    t_mmc_error MMC_Error;

    IDENTIFIER_NOT_USED(FirstBad_p);

    // Jedec 84-A44 : Size < 2GB => byte addressing
    if (!FPDConfig_p->HC_Card) {
        Position *= MMC_ADDRESS_MODE_FACTOR;
    }

    MMC_Error = MMC_Erase(1, Position, Position + NrOfSectors, FPDConfig_p->DeviceId);
    VERIFY(MMC_OK == MMC_Error, E_BDM_JUNK_FAILED);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
