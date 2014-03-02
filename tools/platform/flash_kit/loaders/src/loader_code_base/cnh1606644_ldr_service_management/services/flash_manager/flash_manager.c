/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_fpd Flash Physical Driver
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "flash_manager.h"
#include "r_service_management.h"
#include "r_debug_macro.h"

#ifdef CNH1606644_FM_USES_PDNAND_NOR
#include "fm_pd_nand_nor.h"
#endif //CNH1606644_FM_USES_PDNAND_NOR

#ifdef CNH1606644_FM_USES_EMMC
#include "fm_emmc.h"
#endif //CNH1606644_FM_USES_EMMC

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
static FPD_Functions_t FPD_Functions = {0};
static FPD_Config_t FPD_Configs[MAX_NR_OF_FLASH_DEVICES];
static uint8 NrOfAvailableFlashDevices = 0;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * This function initialize FPD_Config_t structure with flash memory properties
 * from FPD.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
void Do_FPD_Init(void)
{
    uint8 i;

    NrOfAvailableFlashDevices = 0;

#ifdef CNH1606644_FM_USES_PDNAND_NOR
    FPD_Functions.FPD_Start = Do_PDNAND_NOR_Start;
    FPD_Functions.FPD_Stop = Do_PDNAND_NOR_Stop;
    FPD_Functions.FPD_GetInfo = Do_PDNAND_NOR_GetInfo;
    FPD_Functions.FPD_SetInfo = Do_PDNAND_NOR_SetInfo;
    FPD_Functions.FPD_ReadPage = Do_PDNAND_NOR_ReadPage;
    FPD_Functions.FPD_ReadConsecutivePages = Do_PDNAND_NOR_ReadConsecutivePages;
    FPD_Functions.FPD_WritePage = Do_PDNAND_NOR_WritePage;
    FPD_Functions.FPD_WriteConsecutivePages = Do_PDNAND_NOR_WriteConsecutivePages;
    FPD_Functions.FPD_Erase = Do_PDNAND_NOR_Erase;
    FPD_Functions.FPD_GetPartitionsConfiguration = Do_PDNAND_NOR_GetPartitionsConfiguration;

    Do_PDNAND_NOR_Init();
#endif //CNH1606644_FM_USES_PDNAND_NOR

#ifdef CNH1606644_FM_USES_EMMC
    FPD_Functions.FPD_Start = Do_EMMC_Start;
    FPD_Functions.FPD_Stop = Do_EMMC_Stop;
    FPD_Functions.FPD_GetInfo = Do_EMMC_GetInfo;
    FPD_Functions.FPD_SetInfo = Do_EMMC_SetInfo;
    FPD_Functions.FPD_ReadPage = Do_EMMC_ReadPage;
    FPD_Functions.FPD_ReadConsecutivePages = Do_EMMC_ReadConsecutivePages;
    FPD_Functions.FPD_WritePage = Do_EMMC_WritePage;
    FPD_Functions.FPD_WriteConsecutivePages = Do_EMMC_WriteConsecutivePages;
    FPD_Functions.FPD_Erase = Do_EMMC_Erase;
    FPD_Functions.FPD_IsStarted = Do_EMMC_IsStarted;
    FPD_Functions.FPD_GetPartitionsConfiguration = Do_EMMC_GetPartitionsConfiguration;
    FPD_Functions.FPD_SetEnhancedArea = Do_EMMC_SetEnhancedArea;
    FPD_Functions.FPD_EnPowerReset = Do_EMMC_MMC_EnPowerReset;

    for (i = 0; i < MAX_NR_OF_FLASH_DEVICES; i++) {
        Do_EMMC_Init(i, &FPD_Configs[NrOfAvailableFlashDevices]);

        if (E_SUCCESS == FPD_Functions.FPD_Start(&FPD_Configs[NrOfAvailableFlashDevices])) {
            NrOfAvailableFlashDevices++;
        }
    }

#endif //CNH1606644_FM_USES_EMMC
}



/**
 * This function initialize Physical driver, sets memory properties.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_Start(void)
{
    uint8 i;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    // Since we dont know which device to start here return success if at least one is started
    for (i = 0; i < NrOfAvailableFlashDevices; i++) {
        if (E_SUCCESS == FPD_Functions.FPD_Start(&FPD_Configs[i])) {
            ReturnValue = E_SUCCESS;
        } else {
            A_(printf("FPD:% could not be started\n", i);)
        }
    }

    return ReturnValue;
}

/**
 * Physical driver can't be stop. This function need to exist for service
 * manager to work properly.
 *
 * @return       @ref E_SUCCESS  � Success ending.
 */
ErrorCode_e Do_FPD_Stop(void)
{
    uint8 i;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    for (i = 0; i < NrOfAvailableFlashDevices; i++) {
        if (E_SUCCESS == FPD_Functions.FPD_Stop(&FPD_Configs[i])) {
            ReturnValue = E_SUCCESS;
        } else {
            A_(printf("FPD:% could not be stopped\n", i);)
        }
    }

    return ReturnValue;
}

/**
 * Retrieves memory properties.
 *
 * @param [in]   FlashDevice     - Flash device number
 * @param [out]  Configuration_p - Pointer to configuration structure.
 * @return       @ref E_SUCCESS  � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_GetInfo(const FLASH_DEVICE_t FlashDevice, FPD_Config_t *Configuration_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_GetInfo(&FPD_Configs[FlashDevice], Configuration_p);

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Retrieves memory properties.
 *
 * @param [in] FlashDevice    - Flash device number
 * @param [in] NewConfig_p    - Contains necessary information for
 *                              configuring the FPD.
 * @return     @ref E_SUCCESS � Success ending.
 *
 * @return     @ref E_INVALID_INPUT_PARAMETER - If NULL is passed.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_SetInfo(const FLASH_DEVICE_t FlashDevice, FPD_Config_t *NewConfig_p)
{
    return FPD_Functions.FPD_SetInfo(&FPD_Configs[FlashDevice], NewConfig_p);
}

/**
 * Reads one page and/or extra data from FPD. The length of the data buffer
 * Data_p must be "page size" * NbrOfPages. The length of the ExtraData_p must
 * be the ExtraDataLength * NbrOfPages.
 *
 * @param [in]   PageOffset     - An offset expressed in number of pages.
 * @param [in]   FlashDevice    - Flash device number
 * @param [out]  Data_p         - Data buffer. May be NULL if extra data is to
 *                                be read.
 * @param [out]  ExtraData_p    - Data buffer. May be NULL if page data is to
 *                                be read.
 * @return       @ref E_SUCCESS � Success ending.
 *
 * @return       Function can also return some internal FPD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_ReadPage(const uint32 PageOffset, const FLASH_DEVICE_t FlashDevice, uint8 *Data_p, uint8 *ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_ReadPage(PageOffset, &FPD_Configs[FlashDevice], Data_p, ExtraData_p);

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }

    }

ErrorExit:
    return ReturnValue;
}

/**
 * Reads one or several consecutive pages and/or extra data from FPD. The size
 * of the data buffer Data_p must be "page size" * NbrOfPages. The size of the
 * data buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to read.
 * @param [in]  FlashDevice    - Flash device number
 * @param [out] Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be read.
 * @param [out] ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be read.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_ReadConsecutivePages(const uint32 PageOffset, const uint32 NbrOfPages, const FLASH_DEVICE_t FlashDevice, uint8 *Data_p, uint8 *ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_ReadConsecutivePages(PageOffset, NbrOfPages, &FPD_Configs[FlashDevice], Data_p, ExtraData_p);

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }

        ;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Writes one page and/or extra data to FPD. The size of the data buffer Data_p
 * must be "page size" * NbrOfPages. The size of the data buffer ExtraData_p
 * must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  FlashDevice    - Flash device number
 * @param [in]  Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be written.
 * @param [in]  ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be written.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_WritePage(const uint32 PageOffset, const FLASH_DEVICE_t FlashDevice, const uint8 *const Data_p, const uint8 *const ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_WritePage(PageOffset, Data_p, ExtraData_p, &FPD_Configs[FlashDevice]);

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }

    }

ErrorExit:
    return ReturnValue;
}

/**
 * Writes one or several consecutive pages and/or extra data to FPD. The size of
 * the data buffer Data_p must be "page size" * NbrOfPages. The size of the data
 * buffer ExtraData_p must be ExtraDataLength * NbrOfPages.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfPages     - Number of pages to write.
 * @param [in]  FlashDevice    - Flash device number
 * @param [in]  Data_p         - Data buffer. Pointer may be NULL if only extra
 *                               data is to be written.
 * @param [in]  ExtraData_p    - Data buffer. Pointer may be NULL if only page
 *                               data is to be written.
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_WriteConsecutivePages(const uint32 PageOffset,   const uint32 NbrOfPages, const FLASH_DEVICE_t FlashDevice,
        const uint8 *const Data_p, const uint8 *const ExtraData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_WriteConsecutivePages(PageOffset, NbrOfPages, Data_p, ExtraData_p, &FPD_Configs[FlashDevice]);

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }

    }

ErrorExit:
    return ReturnValue;
}

/**
 * Erases one or several blocks in FPD.
 *
 * @param [in]  PageOffset     - An offset expressed in number of pages.
 * @param [in]  NbrOfBlocks    - Number of blocks to erase.
 * @param [in]  FlashDevice    - Flash device number
 * @return      @ref E_SUCCESS � Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_Erase(uint32 PageOffset, uint32 NbrOfBlocks, const FLASH_DEVICE_t FlashDevice)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_Erase(PageOffset, NbrOfBlocks, &FPD_Configs[FlashDevice]);

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }

    }

ErrorExit:
    return ReturnValue;
}

boolean Do_FPD_IsStarted(const FLASH_DEVICE_t FlashDevice)
{
    return FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice]);
}

ErrorCode_e Do_FPD_GetPartitionsConfiguration(void)
{
    return FPD_Functions.FPD_GetPartitionsConfiguration();
}

/**
 * Defines Enhanced Area in flash memory.
 *
 * @param [in]  StartAddress   - Enhanced area start address in bytes.
 * @param [in]  AreaSize       - Enhanced area size in bytes.
 * @param [in]  FlashDevice    - Flash device number
 * @return      @ref E_SUCCESS - Success ending.
 *
 * @return      Function can also return some internal FPD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_FPD_SetEnhancedArea(uint64 StartAddress, uint64 AreaSize, const FLASH_DEVICE_t FlashDevice)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_SetEnhancedArea(StartAddress, AreaSize, &FPD_Configs[FlashDevice]);

        if (E_SUCCESS == ReturnValue) {
            Do_FPD_Init();
        }

        /* Unregister of service is done differently from registering of service
           because it needs to allow FPD error returned from the FPD service
           function to pass thru in case when unregistering is successful. */
        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }
    }

ErrorExit:
    return ReturnValue;
}

/**
 * This function returns number of available flash devices
 *
 * @return  Number of flash devices.
 */
uint8 Do_FPD_GetNrOfAvailableFlashDevices(void)
{
    return NrOfAvailableFlashDevices;
}

/**
 * Function for disable/enable HW reset
 *
 * @param [in] RegValue     - Value to be written in register (1- set, 0-reset)
 * @param [in] FlashDevice  - Flash device number
 * @return      Returns E_SUCCESS for success ending.
 *
 * @return      Returns E_ENABLE_HW_RESET_FAILED for fail.
 *
 * @return      Function can also return some internal MMC error codes.
 */
ErrorCode_e Do_FPD_EnPowerReset(uint32 RegValue, const FLASH_DEVICE_t FlashDevice)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (FPD_Functions.FPD_IsStarted(&FPD_Configs[FlashDevice])) {
        ReturnValue = Do_Register_Service(FPD_SERVICE);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = FPD_Functions.FPD_EnPowerReset(RegValue, &FPD_Configs[FlashDevice]);

        if (E_SUCCESS != Do_UnRegister_Service(FPD_SERVICE)) {
            ReturnValue = E_UNREGISTER_FPD_SERVICE_FAILED;
        }
    }


ErrorExit:
    return ReturnValue;
}
/** @} */
/** @} */
