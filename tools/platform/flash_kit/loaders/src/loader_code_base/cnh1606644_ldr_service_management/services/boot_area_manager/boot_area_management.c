/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file  boot_area_management.c
 * @brief Function definitions for boot area management
 *
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_bam Boot Area Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "string.h"
#include "r_service_management.h"
#include "boot_area_management.h"
#include "r_debug.h"
#include "r_debug_macro.h"

#ifdef CNH1606644_BAM_USES_MBBS
#include "bam_mbbs.h"
#endif //CNH1606644_BAM_USES_MBBS
#ifdef CNH1606644_BAM_USES_EMMC
#include "bam_emmc.h"
#endif //CNH1606644_BAM_USES_EMMC

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
static BAM_Config_t BAM_Config = {0};
static BAM_Functions_t BAM_Functions = {0};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * Sets valid pointer functions
 */
void Do_BAM_Init(void)
{
#ifdef CNH1606644_BAM_USES_MBBS
    BAM_Functions.BAM_Start = Do_BAM_MBBS_Start;
    BAM_Functions.BAM_Stop  = Do_BAM_MBBS_Stop;
    BAM_Functions.BAM_Read  = Do_BAM_MBBS_Read;
    BAM_Functions.BAM_UnregisteredReadBootArea = Do_BAM_MBBS_UnregisteredReadBootRecords;
    BAM_Functions.BAM_Write = Do_BAM_MBBS_Write;
    BAM_Functions.BAM_WriteBootArea = Do_BAM_MBBS_WriteBootRecords;
    BAM_Functions.BAM_CompleteErase = Do_BAM_MBBS_CompleteErase;
    BAM_Functions.BAM_GetBlockSize = Do_BAM_MBBS_GetBlockSize;
#endif //CNH1606644_BAM_USES_MBBS

#ifdef CNH1606644_BAM_USES_EMMC
    BAM_Functions.BAM_Start = Do_BAM_EMMC_Start;
    BAM_Functions.BAM_Stop  = Do_BAM_EMMC_Stop;
    BAM_Functions.BAM_Read  = Do_BAM_EMMC_Read;
    BAM_Functions.BAM_Write = Do_BAM_EMMC_Write;
    BAM_Functions.BAM_CompleteErase = Do_BAM_EMMC_CompleteErase;

    BAM_Config.Configuration_Set = FALSE;
#endif //CNH1606644_BAM_USES_EMMC
}

/**
 * Starts up the BAM (Boot Area Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_Start()
{
    return BAM_Functions.BAM_Start();
}


/**
* Stops the BAM (Boot Area Management)
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BAM_Stop()
{
    return BAM_Functions.BAM_Stop();
}

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
ErrorCode_e Do_BAM_Read(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;;

    ReturnValue = Do_Register_Service(BAM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BAM_Functions.BAM_Read(FPD_Config_p, Offset, Data_p, DataSize);

    /* Unregister of service is done differently from registering of service
       because it needs to allow BAM error returned from the BAM service
       function to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(BAM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BAM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Used for reading data from a block without registering service.
 * Used ONLY by BootRecords Handler!!!
 *
 * @param [out] Data_p     Output buffer.
 * @param [out] DataSize_p Size of the data that will be read, in bytes for
 *                         the specified block number.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_UnregisteredReadBootArea(uint8 **Data_pp, uint32 *DataSize_p)
{
    return BAM_Functions.BAM_UnregisteredReadBootArea(Data_pp, DataSize_p);
}

/**
 * Used for writing data to a block.
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
ErrorCode_e Do_BAM_Write(FPD_Config_t *const FPD_Config_p, uint32 Offset, uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BAM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BAM_Functions.BAM_Write(FPD_Config_p, Offset, Data_p, DataSize);

    /* Unregister of service is done differently from registering of service
       because it needs to allow BAM error returned from the BAM service
       function to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(BAM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BAM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Write boot records data.
 *
 * @param[in]  FPDConfig_p Flash device configuration of BAM
 * @param[in]  Data_p   Input buffer.
 * @param[in]  DataSize Bytes to write.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_WriteBootArea(FPD_Config_t *const FPD_Config_p, uint8 *Data_p, uint32 DataSize)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BAM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BAM_Functions.BAM_WriteBootArea(Data_p, DataSize);

    /* Unregister of service is done differently from registering of service
       because it needs to allow BAM error returned from the BAM service
       function to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(BAM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BAM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
* Erases the whole area for the specified Unit.
*
* @param [in]  FPDConfig_p Flash device configuration of BAM
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BAM_CompleteErase(FPD_Config_t *const FPD_Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BAM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BAM_Functions.BAM_CompleteErase(FPD_Config_p);

    /* Unregister of service is done differently from registering of service
       because it needs to allow BAM error returned from the BAM service
       function to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(BAM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BAM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
* Retreives information about the BAM that is currently used.
*
* @param [in]  FPDConfig_p Flash device configuration of BAM
* @param [out] Config_p  Pointer to memory where the retrieved information will
*                        be placed.
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BAM_GetConfig(FPD_Config_t *const FPDConfig_p, BAM_Config_t *Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != Config_p, E_INVALID_INPUT_PARAMETER);

    Config_p->Version = 3;
    Config_p->Number_Of_Units = 1;

    Config_p->Unit_Configuration[0].ChipSelect          = 0;    /* @TODO: Add ChipSelect in FPD configuration!? */
    Config_p->Unit_Configuration[0].MediaType           = 0;     /* MBBS_LL_NAND = 0 */
    Config_p->Unit_Configuration[0].SubMediaType        = 0;
    Config_p->Unit_Configuration[0].EraseBlockSize      = FPDConfig_p->EraseBlockSize;
    Config_p->Unit_Configuration[0].PageSize            = FPDConfig_p->PageSize;
    Config_p->Unit_Configuration[0].RedundantAreaSize   = FPDConfig_p->PageExtraSize;
    Config_p->Unit_Configuration[0].StartBlock          = 0;
    Config_p->Unit_Configuration[0].PagesPerBlock       = FPDConfig_p->EraseBlockSize / FPDConfig_p->PageSize;
    Config_p->Unit_Configuration[0].SectorsPerBlock     = Config_p->Unit_Configuration[0].PagesPerBlock;
    Config_p->Unit_Configuration[0].EndBlockByteAddress = 0x20000;
    Config_p->Unit_Configuration[0].UseBootableCABS     = TRUE;
    Config_p->Unit_Configuration[0].BBMType             = BBM_STANDARD;

    Config_p->Configuration_Set = TRUE;

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
* Configures the BAM that should be started.
*
* @param [in] NewConfig_p   Contains necessary information for configuring the
*                           BAM.
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BAM_SetConfig(BAM_Config_t *const NewConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != NewConfig_p, E_INVALID_INPUT_PARAMETER);
    memcpy(&BAM_Config, NewConfig_p, sizeof(BAM_Config_t));

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/**
 * Query BAM unit for the size of a specific Reserved Block.
 *
 * @param [in] Unit         The unit to retrieve block size from.
 * @param [in] Reserved     Reserved block to get size for.
 * @param [out] BlockSize_p Size of the Reserved block.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BAM_GetBlockSize(FPD_Config_t *const FPD_Config_p, uint32 Reserved, uint32 *BlockSize_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BAM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BAM_Functions.BAM_GetBlockSize(FPD_Config_p, Reserved, BlockSize_p);

    /* Unregister of service is done differently from registering of service
       because it needs to allow BAM error returned from the BAM service
       function to pass thru in case when unregistering is successful. */
    if (E_SUCCESS != Do_UnRegister_Service(BAM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BAM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
