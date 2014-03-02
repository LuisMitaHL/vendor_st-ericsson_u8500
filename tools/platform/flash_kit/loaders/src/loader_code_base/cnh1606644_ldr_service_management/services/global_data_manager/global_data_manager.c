/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup ldr_service_management
 * @{
 *   @addtogroup ldr_service_gdm Global Data Manager
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "global_data_manager.h"
#include "r_service_management.h"
#include "r_debug_macro.h"
#include "gdm_cspsa.h"
#include "toc_handler.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
#define MAX_DEVICE_CSPSA0_NAME 7

static GDM_Functions_t GDM_Functions;
char *ActiveDevice_p = NULL;
extern char *InsertedDevice_p;

const char *CSPSA_DefaultDeviceString     = "CSPSA0";

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static ErrorCode_e Check_GD_Device(void);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * Sets valid pointer functions.
 */
void Do_GD_Init(void)
{
    GDM_Functions.GDM_Start            = Do_GDM_CSPSA_Start;
    GDM_Functions.GDM_Stop             = Do_GDM_CSPSA_Stop;
    GDM_Functions.GDM_Format           = Do_GDM_CSPSA_Format;
    GDM_Functions.GDM_Flush            = Do_GDM_CSPSA_Flush;
    GDM_Functions.GDM_GetUnitSize      = Do_GDM_CSPSA_GetUnitSize;
    GDM_Functions.GDM_ReadUnit         = Do_GDM_CSPSA_ReadUnit;
    GDM_Functions.GDM_WriteUnit        = Do_GDM_CSPSA_WriteUnit;
    GDM_Functions.GDM_GetUnitSizeFirst = Do_GDM_CSPSA_GetUnitSizeFirst;
    GDM_Functions.GDM_GetUnitSizeNext  = Do_GDM_CSPSA_GetUnitSizeNext;

    //
    // Register a plug - in functions in CSPSA module.
    //
    CSPSA_LL_EMMC_Init();
}

/**
 * Starts up the GDM (Global Data Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_Start(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    Do_GD_Init();

    if (NULL != InsertedDevice_p) {
        ActiveDevice_p = malloc(strlen(InsertedDevice_p) + sizeof(char));
        ASSERT(NULL != ActiveDevice_p);

        strncpy(ActiveDevice_p, InsertedDevice_p, TOC_ID_LENGTH);
    } else {
        A_(printf("global_data_manager.c(%d): Use default CSPSA device\n", __LINE__);)

        ActiveDevice_p = malloc(TOC_ID_LENGTH * sizeof(char));
        ASSERT(NULL != ActiveDevice_p);

        strncpy(ActiveDevice_p, CSPSA_DefaultDeviceString, TOC_ID_LENGTH);
    }

    ReturnValue = GDM_Functions.GDM_Start();

    return ReturnValue;
}

/**
* Stops the GDM (Global Data Management).
*
* @return see \c ErrorCode_e for more details.
*/
ErrorCode_e Do_GD_Stop(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = GDM_Functions.GDM_Stop();

    return ReturnValue;
}

/**
 * Format the GDM (Global Data Management).
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_Format(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_Format();

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * The current cached parameter storage area data is stored to memory media.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_Flush(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_Flush();

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Retrieve the size of the given Unit.
 *
 * @param [in]  Unit          Unit whose size will be retrieved.
 * @param [out] Size_p        Pointer to variable to get the size of the Unit.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_GetUnitSize(uint32 Unit, uint32 *Size)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_GetUnitSize(Unit, Size);

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Reads the contents of the given unit.
 *
 * @param [in]  Unit          Unit to be read.
 * @param [out] Data_p        Pointer to data to be read.
 * @param [in]  Size          Size of the data to be read.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_ReadUnit(uint32 Unit, uint8 *Data_p, uint32 Size)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_ReadUnit(Unit, Data_p, Size);

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Write a Unit.
 *
 * @param [in] Unit           Unit to be written.
 * @param [in] Data_p         Pointer to data to be written.
 * @param [in] Size           Size of the data to be written.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_WriteUnit(uint32 Unit, uint8 *Data_p, uint32 Size)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_WriteUnit(Unit, Data_p, Size);

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Gets the size and unit number of the first parameter value.
 *
 * @param [out] Unit_p  The unit number of the first parameter value.
 * @param [out] Size_p  Size of the first unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_GetUnitSizeFirst(uint32 *Unit_p, uint32 *Size_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_GetUnitSizeFirst(Unit_p, Size_p);

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/**
 * Get the size of the next unit in the list.
 *
 * @param [out] Unit_p  The unit number of the next parameter value.
 * @param [out] Size_p  Size of the next unit in the list.
 *
 * @return see \c ErrorCode_e for more details.
 */
ErrorCode_e Do_GD_GetUnitSizeNext(uint32 *Unit_p, uint32 *Size_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    ReturnValue = Check_GD_Device();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_Register_Service(GD_SERVICE);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GDM_Functions.GDM_GetUnitSizeNext(Unit_p, Size_p);

    /* Unregister of service is done differently from registring of service
       because it needs to allow GD error returned from the GD service
       function to pass thru in case when unregistering is successfull. */
    if (E_SUCCESS != Do_UnRegister_Service(GD_SERVICE)) {
        ReturnValue = E_UNREGISTER_GD_SERVICE_FAILED;
    }

ErrorExit:
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e Check_GD_Device(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    if (TRUE == IsStarted(GD_SERVICE)) {
        if (0 == strncmp(ActiveDevice_p, InsertedDevice_p, TOC_ID_LENGTH + 1)) {
            ReturnValue = E_SUCCESS;
        } else {
            ReturnValue = Do_Stop_Service(GD_SERVICE);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/** @} */
/** @} */
