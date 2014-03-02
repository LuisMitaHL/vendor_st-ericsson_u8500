/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file  block_device_manager.h
 * @brief Function definitions for block device manager
 *
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_bdm Block Device Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "block_device_management.h"
#include "r_service_management.h"
#include "r_debug_macro.h"

#ifdef CNH1606644_BDM_USES_CABS
#include "bdm_cabs.h"
#endif //CNH1606644_BDM_USES_CABS
#ifdef CNH1606644_BDM_USES_EMMC
#include "bdm_emmc.h"
#endif //CNH1606644_BDM_USES_EMMC
#ifdef ENABLE_DEFAULT_BDM_CONFIGURATION
#include "flash_manager.h"
#endif //ENABLE_DEFAULT_BDM_CONFIGURATION

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
static BDM_Config_t BDM_Config = {0};
static BDM_Functions_t BDM_Functions = {0};

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * Sets valid pointer functions.
 */
void Do_BDM_Init(void)
{
#ifdef CNH1606644_BDM_USES_CABS
    BDM_Functions.BDM_Start   = Do_BDM_CABS_Start;
    BDM_Functions.BDM_Stop    = Do_BDM_CABS_Stop;
    BDM_Functions.BDM_Read    = Do_BDM_CABS_Read;
    BDM_Functions.BDM_Write   = Do_BDM_CABS_Write;
    BDM_Functions.BDM_GetInfo = Do_BDM_CABS_GetInfo;
    BDM_Functions.BDM_Junk    = Do_BDM_CABS_Junk;
    BDM_Functions.BDM_GetFSInterface = Do_BDM_CABS_GetFSInterface;
#endif //CNH1606644_BDM_USES_CABS

#ifdef CNH1606644_BDM_USES_EMMC
    BDM_Functions.BDM_Start   = Do_BDM_EMMC_Start;
    BDM_Functions.BDM_Stop    = Do_BDM_EMMC_Stop;
    BDM_Functions.BDM_Read    = Do_BDM_EMMC_Read;
    BDM_Functions.BDM_Write   = Do_BDM_EMMC_Write;
    BDM_Functions.BDM_GetInfo = Do_BDM_EMMC_GetInfo;
    BDM_Functions.BDM_Junk    = Do_BDM_EMMC_Junk;
#endif //CNH1606644_BDM_USES_EMMC
}

/**
 * Starts up the BDM (Block Device Management). All Units.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Start()
{
    return BDM_Functions.BDM_Start();
}

/**
 * Stops the BDM (Block Device Management). All Units.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_Stop(void)
{
    return BDM_Functions.BDM_Stop();
}

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
ErrorCode_e Do_BDM_Read(FPD_Config_t *const FPDConfig_p, uint32 Pos,
                        uint32 Length, uint32 *FirstBad_p,
                        uint8 *Data_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BDM_Functions.BDM_Read(FPDConfig_p, Pos, Length, FirstBad_p, Data_p);

    /* Unregister of service is done differently from registring of service
       because it needs to allow BDM error returned from the BDM service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(BDM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BDM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

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
ErrorCode_e Do_BDM_Write(FPD_Config_t *const FPDConfig_p, uint32 Pos,
                         uint32 Length, uint32 *FirstBad_p,
                         uint8 *Data_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BDM_Functions.BDM_Write(FPDConfig_p, Pos, Length, FirstBad_p, Data_p);

    /* Unregister of service is done differently from registring of service
       because it needs to allow BDM error returned from the BDM service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(BDM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BDM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Returns BDM's Unit information
 *
 * @param [in]  FPDConfig_p      Flash device configuration of BDM
 * @param [out] Configuration_p  Pointer to returned configuration.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_GetInfo(FPD_Config_t *const FPDConfig_p, BDM_Unit_Config_t *Configuration_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BDM_Functions.BDM_GetInfo(FPDConfig_p, Configuration_p);

    /* Unregister of service is done differently from registring of service
       because it needs to allow BDM error returned from the BDM service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(BDM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BDM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

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
ErrorCode_e Do_BDM_Junk(FPD_Config_t *const FPDConfig_p, uint32 Position,
                        uint32 NrOfSectors, uint32 *FirstBad_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BDM_Functions.BDM_Junk(FPDConfig_p, Position, NrOfSectors, FirstBad_p);

    /* Unregister of service is done differently from registring of service
       because it needs to allow BDM error returned from the BDM service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(BDM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BDM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
* Retreives information about the BDM that is currently used.
*
* @param[in]   FPDConfig_p Flash device configuration of BDM
* @param [out] Config_p Pointer to memory where the retrieved information will
*                       be placed.
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BDM_GetConfig(FPD_Config_t *const FPDConfig_p, BDM_Config_t *Config_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    VERIFY(NULL != Config_p, E_INVALID_INPUT_PARAMETER);
    //memcpy( Config_p, &BDM_Config, sizeof(BDM_Config_t));

    Config_p->Version = 3;
    Config_p->Number_Of_Units = 1;

    Config_p->Unit_Configuration[0].EraseBlockSizeInBytes = FPDConfig_p->EraseBlockSize;
    Config_p->Unit_Configuration[0].PageSizeInBytes       = FPDConfig_p->PageSize;
    Config_p->Unit_Configuration[0].RedundantAreaInBytes  = FPDConfig_p->PageExtraSize;
    Config_p->Unit_Configuration[0].StartBlock = 0x20000;   /* XVSZOAN: @TODO: Read these settings from BootRecords */
    Config_p->Unit_Configuration[0].EndBlock = 0x7E00000;

    Config_p->Unit_Configuration[0].LogicalSectors    = 0xDFCA;
    Config_p->Unit_Configuration[0].BadBlockOverhead  = 1;
    Config_p->Unit_Configuration[0].ResevedBootBlocks = 2;
    Config_p->Unit_Configuration[0].ChipSelect        = 0;
    Config_p->Unit_Configuration[0].Attributes        = 0x7;

    Config_p->Configuration_Set = TRUE;

    ReturnValue = Do_UnRegister_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, E_UNREGISTER_BDM_SERVICE_FAILED);

ErrorExit:
    return ReturnValue;
}

/**
* Configures the BDM that should be started.
*
* @param [in] NewConfig_p   Contains necessary information for configuring the
*                           BDM.
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_BDM_SetConfig(BDM_Config_t *const NewConfig_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    VERIFY(NULL != NewConfig_p, E_INVALID_INPUT_PARAMETER);
    memcpy(&BDM_Config, NewConfig_p, sizeof(BDM_Config_t));

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

#ifdef CNH1606644_FS_USES_JEFF_VFAT
/**
 * Retreives function pointers for FS.
 *
 * @param[in]  Unit          BDM unit that should be used.
 * @param[out] Device_p      DDB Handle.
 * @param[out] Functions_pp  Returned function pointers.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_BDM_GetFSInterface(uint32 Unit, DdbHandle *Device_p, DdbFunctions **Functions_pp)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BDM_Functions.BDM_GetFSInterface(Unit, Device_p, Functions_pp);

    /* Unregister of service is done differently from registring of service
       because it needs to allow BDM error returned from the BDM service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(BDM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BDM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}
#endif //DCNH1606644_FS_USES_JEFF_VFAT

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
ErrorCode_e Do_BDM_GetFSInterface(uint32 Unit, DdbHandle *Device_p, DdbFunctions **Functions_pp)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Do_Register_Service(BDM_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = BDM_Functions.BDM_GetFSInterface(Unit, Device_p, Functions_pp);

    /* Unregister of service is done differently from registring of service
       because it needs to allow BDM error returned from the BDM service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(BDM_SERVICE)) {
        ReturnValue = E_UNREGISTER_BDM_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}
#endif //CFG_ENABLE_APPLICATION_FILE_SYS

/** @} */
/** @} */
