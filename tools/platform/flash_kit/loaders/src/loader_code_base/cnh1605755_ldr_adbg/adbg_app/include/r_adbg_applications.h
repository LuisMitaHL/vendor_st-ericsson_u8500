/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef R_ADBG_GROUP_
#define R_ADBG_GROUP_

/**
 * @addtogroup ADbg
 * @{
 * This module implements functionalities for ADbg (Automatic Debugger).
 * It comprises functionalities:
 *
 * - List Case,
 * - List Interface,
 * - Set Precondition,
 * - Recovery Condition,
 * - Run.
 *
 * ADbg module presents a framework that enables building unit tests against
 * different sub-modules which build-up the final loader product. The system is
 * designed to support development of positive and negative test case types.
 * Results returned after executing particular test case are sent through the
 * general response.
 */

/*******************************************************************************
* Includes
******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * @brief Function that is called from each Done.. function from command_marshal.c
 *
 * @param [in]     Result is error code returned from the function which is tested.
 * @return         ErrorCode_e.
 *
 * @remark         It doesn't check the input parameters.
 */
ErrorCode_e ReleaseADbg_AppFunc(ErrorCode_e Result);

/**
 * @brief Function that is called from the test cases for functions which are not commands
 *
 * @param [in]     AssertStatus is the result from the tested assert. 0 if expected result is equal with returned result, 1 if not
 * @return         None.
 *
 * @remark         It doesn't check the input parameters.
 */
void ReleaseADbg(uint32 AssertStatus);

/** @} */
#endif /*R_ADBG_GROUP_*/
