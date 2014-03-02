/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_security_library
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "e_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "loader_sec_lib_internals.h"
#include "r_debug.h"
#include "r_debug_macro.h"


/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Verify the static data.
 *
 * @param[in] StaticData_p Buffer with static data.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   Successful verification.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC If input parameter is
 *                                                  not valid.
 */
LoaderSecLib_Output_t LoaderSecLib_Verify_StaticData(uint8 const *const StaticData_p)
{
    LoaderSecLib_Output_t   ReturnValue = LOADER_SEC_LIB_FAILURE;
    VERIFY((NULL != StaticData_p), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    //  ReturnValue = LoaderSecLib_Calculate_MAC((uint8*)(StaticData_p+, Length, &ControlKeysData[0]);
    //  VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/** @} */
