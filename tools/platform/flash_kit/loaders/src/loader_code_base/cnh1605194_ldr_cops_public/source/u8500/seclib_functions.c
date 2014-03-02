/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup u8500_seclib_functions
 *
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "c_system.h"
#include "t_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "e_loader_sec_lib.h"
#include "r_debug.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * This routine initializes the data used by the Loader Security Library.
 *
 * Create the list of the internal data structures. So that the other external
 * functions can be used. This function must be called before any other security
 * library functions.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Succesful execute of function.
 * @retval  LOADER_SEC_LIB_INIT_CALLED_TWICE          Sec library is already initialized.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Init(void)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    static boolean IsInitCalledOneTime = FALSE;

    C_(printf("Do_LoaderSecLib_Init\n");)

    if (!IsInitCalledOneTime) {
        // This will disable the possibility to call init 2 times..
        IsInitCalledOneTime = TRUE;
        ReturnValue = LOADER_SEC_LIB_SUCCESS;
    } else {
        A_(printf("data_protoection.c (%d): ** ERR: Called Init 2 times! **\n", __LINE__);)
        ReturnValue = LOADER_SEC_LIB_INIT_CALLED_TWICE;
        goto ErrorExit;
    }

    C_(if (ReturnValue == LOADER_SEC_LIB_SUCCESS) printf("data_protoection.c (%d): INFO: Security library initialized successful.! \n", __LINE__);)

ErrorExit:
        if (LOADER_SEC_LIB_SUCCESS != ReturnValue) {
            A_(printf("data_protoection.c (%d): ** ERR: Loader SecLib initialization failed! **\n", __LINE__);)
            Do_LoaderSecLib_ShutDown();
        }

    return ReturnValue;
}

/**
 * This routine free all memories allocated from loader security
 * library and stop the loader.
 */
void Do_LoaderSecLib_ShutDown(void)
{
    A_(printf("Do_LoaderSecLib_ShutDown\n");)
    return;
}

/* Authentication */

/**
 * Authenticate loader.
 *
 * @param[in] Type    Type of authentication method.
 * @param[in] Data_p  Buffer with authentication data.
 * @param[in] Length  Length of the buffer Data_p.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success authentication.
 * @retval .
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Authenticate(uint32 Type, void *Data_p, uint32 Length)
{
    A_(printf("Do_LoaderSecLib_Authenticate\n");)

    return LOADER_SEC_LIB_SUCCESS;
}

/* OTP */

/**
 * Reads OTP data
 *
 * @param[out]  OTP_Data_p Buffer that conatins OTP data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                            Successful reading.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
LoaderSecLib_Output_t Do_LoaderSecLib_Read_OTP(LOADER_SEC_LIB_OTP_t *OTP_Data_p)
{
  A_(printf("Do_LoaderSecLib_Read_OTP\n");)

  return LOADER_SEC_LIB_SUCCESS;
}
 */


/**
 * Writes OTP data and lock bits into fusebox
 *
 * @param[in] OTP_Data_p Buffer that conatins OTP data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                           Successful writing.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 * @retval LOADER_SEC_LIB_OTP_ALREADY_LOCKED         OTP area is already locked.
 * @retval LOADER_SEC_LIB_INVALID_CID_VALUE                CID value is invalid.
LoaderSecLib_Output_t Do_LoaderSecLib_Write_and_Lock_OTP(LOADER_SEC_LIB_OTP_t *OTP_Data_p)
{
  A_(printf("Do_LoaderSecLib_Write_and_Lock_OTP\n");)

  return LOADER_SEC_LIB_SUCCESS;
}
 */

/**
 * Audit permision level.
 *
 * @param[in]  CmdPermission_p  Pointer to structure that hold command
 *                              permission levels.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   Successful unpacking.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Audit_Permission_Levels(CommandPermissionList_t *CmdPermission_p)
{
    B_(printf("Do_LoaderSecLib_Audit_Permission_Levels\n");)

    return LOADER_SEC_LIB_SUCCESS;
}
/** @} */
