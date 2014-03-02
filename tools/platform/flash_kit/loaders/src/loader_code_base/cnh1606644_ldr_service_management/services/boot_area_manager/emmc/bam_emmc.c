/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file  bam_emmc.c
 * @brief Function definitions for EMMC boot area management
 *
 * @addtogroup ldr_service_bam Boot Area Manager
 * @{
 *   @addtogroup ldr_service_bam_emmc EMMC Boot Area Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "boot_area_management.h"
#include "flash_manager.h"
#include "bam_emmc.h"
#include "toc_handler.h"

#include "hcl_defs.h"
#include "memory_mapping.h"
#include "mmc.h"
#include "fm_emmc.h"


/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * Starts up the EMMC Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Start(void)
{
    uint8 i;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 available_flash_devices = Do_FPD_GetNrOfAvailableFlashDevices();

    // Since we dont know which device to start here return success if at least one is started
    for (i = 0; i < available_flash_devices; i++) {
        if (Do_FPD_IsStarted(i)) {
            ReturnValue = E_SUCCESS;
        } else {
            A_(printf("BAM:% could not be started\n", i);)
        }
    }

    return ReturnValue;
}

/**
 * Stops the EMMC Boot Area Management.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Stop(void)
{
    return E_SUCCESS;
}

/**
 * Reads data from EMMC Boot Area Management.
 * DataSize must be a multiple of PageSize (currently 512 bytes for eMMC)
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 * @param[in]  Offset   Bytes from start of flash where data will be read.
 * @param[out] Data_p   Output buffer.
 * @param[in]  DataSize Bytes to read.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Read(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e     ReturnValue = E_GENERAL_FATAL_ERROR;
    t_mmc_error     MMC_Error;
    uint32          NbrOfPages = 0;
    void           *TempData_p = Data_p;


    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    IS_ALIGNED((uint32)Data_p);

    if ((DataSize % FPD_Config_p->PageSize) != 0) {
        A_(printf("Do_BAM_EMMC_Read %d Invalid DataSize\n", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    NbrOfPages = DataSize / FPD_Config_p->PageSize;

    // Jedec 84-A44 : Size < 2GB => byte addressing
    if (FPD_Config_p->HC_Card) {
        Offset /= MMC_ADDRESS_MODE_FACTOR;
    }

    B_(printf("Offset %08x\nPageSize %08x\nDataSize %08x\nnPages %08x", Offset, FPD_Config_p->PageSize, DataSize, NbrOfPages);)

    // Typecast on Data_p OK, alignment checked above
    MMC_Error = MMC_ReadBlocks(1, Offset, (uint32 *)TempData_p, FPD_Config_p->PageSize, NbrOfPages, FPD_Config_p->DeviceId);
    VERIFY(MMC_OK == MMC_Error, E_GENERAL_FATAL_ERROR);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Writes data in EMMC. It is assumed that Data_p contains one complete boot image
 * DataSize must be a multiple of PageSize (currently 512 bytes for eMMC)
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 * @param[in]  Offset   Bytes from start of flash where data will be written.
 * @param[in]  Data_p   Input buffer.
 * @param[in]  DataSize Bytes to write.
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_Write(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e     ReturnValue = E_GENERAL_FATAL_ERROR;
    TOC_List_t      *NewTOCList = NULL;
    uint32          *WriteData_p = NULL;
    uint8           i = 0;
    uint32          Addr = 0;
    uint32          Length = 0;
    t_mmc_error     MMC_Error;
    uint32          *FirstPageData_p = NULL;
    TOC_Type_e      FirstWriteBlock = TOC_BAMCOPY_1;
    TOC_List_t      *FlashTOCList = NULL;

    // MMC interface requires uint32* for data, Data_p must be 32-bit aligned
    IS_ALIGNED((uint32)Data_p);

    // Allow write to Offset = 0 only. Sanity check
    if ((Offset > 0) || (NULL == Data_p) || (DataSize > BAM_BOOT_IMAGE_COPY_SIZE)) {
        A_(printf("Do_BAM_EMMC_Write %d Invalid parameters\n", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    //  if ((DataSize % FPD_Config.PageSize) != 0)
    //  {
    //    A_(printf ("Do_BAM_EMMC_Write %d Invalid parameters\n",__LINE__);)
    //    A_(printf("PageSize %d, DataSize %d\n", FPD_Config.PageSize, DataSize);)
    //    // TODO Add proper error code for eMMC failure
    //    ReturnValue = E_GENERAL_FATAL_ERROR;
    //    goto ErrorExit;
    //  }
    FirstPageData_p = (uint32 *)malloc(FPD_Config_p->PageSize);
    ASSERT(NULL != FirstPageData_p);

    MMC_Error = MMC_ReadBlocks(1, 0, (uint32 *)FirstPageData_p, FPD_Config_p->PageSize, 1, FPD_Config_p->DeviceId);

    if (MMC_OK == MMC_Error) {
        // Query new TOC (incoming image)
        ReturnValue = Do_TOC_Create_RootList(&FlashTOCList, (uint8 *) FirstPageData_p, DataSize, TOC_BAMCOPY_0);

        if ((E_SUCCESS == ReturnValue) || (E_TOC_HANDLER_INPUT_DATA_EMPTY == ReturnValue)) {
            FirstWriteBlock = TOC_BAMCOPY_0;
        }
    }

    // Query new TOC (incoming image)
    ReturnValue = Do_TOC_Create_RootList(&NewTOCList, Data_p, DataSize, TOC_BAMCOPY_0);
    VERIFY((E_SUCCESS == ReturnValue) || (E_TOC_HANDLER_INPUT_DATA_EMPTY == ReturnValue), ReturnValue);

    Length = (DataSize + FPD_Config_p->PageSize - 1) / FPD_Config_p->PageSize;

    // Write N copies of boot block
    WriteData_p = (uint32 *)malloc(Length * FPD_Config_p->PageSize);
    ASSERT(NULL != WriteData_p);

    memset(WriteData_p, FPD_Config_p->ErasedMemContent, Length * FPD_Config_p->PageSize);
    memcpy(WriteData_p, Data_p, DataSize);

    for (i = FirstWriteBlock; i < BAM_N_BOOT_IMAGE_COPIES; i++) {
        // Create updated TOC
        UpdateTOCInWriteData((uint8 *)WriteData_p, NewTOCList, i, 0);
        C_(printf("bam_emmc.c(%d): Do_BAM_EMMC_Write Block= %d\n", __LINE__, i);)
        // Write to eMMC
        Addr = i * BAM_BOOT_IMAGE_COPY_SIZE;

        // Jedec 84-A44 : Size < 2GB => byte addressing
        if (FPD_Config_p->HC_Card) {
            Addr /= MMC_ADDRESS_MODE_FACTOR;
        }

        B_(printf("Write Addr 0x%08x, Length 0x%08x\n", Addr, Length);)

        MMC_Error = MMC_WriteBlocks(1, Addr, WriteData_p, FPD_Config_p->PageSize, Length, FPD_Config_p->DeviceId, FALSE);
        VERIFY(MMC_OK == MMC_Error, E_GENERAL_FATAL_ERROR);
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    BUFFER_FREE(WriteData_p);
    BUFFER_FREE(FirstPageData_p);

    (void)Do_TOC_DestroyTOCList(NewTOCList);
    (void)Do_TOC_DestroyTOCList(FlashTOCList);

    return ReturnValue;
}

/**
 * Erase all data from EMMC.
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 *
 * @return ErrorCode_e Forwards the response from EMMC module.
 */
ErrorCode_e Do_BAM_EMMC_CompleteErase(FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e     ReturnValue = E_GENERAL_FATAL_ERROR;
    t_mmc_error     MMC_Error;
    uint32          EndAddress = 0;

    EndAddress = (BAM_BOOT_IMAGE_COPY_SIZE * BAM_N_BOOT_IMAGE_COPIES) - 1;

    // Jedec 84-A44 : Size < 2GB => byte addressing
    if (FPD_Config_p->HC_Card) {
        EndAddress /= MMC_ADDRESS_MODE_FACTOR;
    }

    MMC_Error = MMC_Erase(1, 0, EndAddress, FPD_Config_p->DeviceId);
    VERIFY(MMC_OK == MMC_Error, E_GENERAL_FATAL_ERROR);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
