/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_ASSERT_H__
#define __R_ADBG_ASSERT_H__

/**
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
* Includes
 ******************************************************************************/
#include "t_adbg.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * @brief Function that allocates space for some structure and set its value
 *        according to input data.
 *
 * @param [in/out]  Var_pp is pointer to pointer to input parameters.
 * @return          Pointer to structure which is input parameter for tested
 *                  function.
 *
 * @remark          It dosen’t check the input parameters.
 */
void *Do_ADbg_GetDataPointer(uint32  Size,
                             void **Var_pp);

/**
 * @brief Function that sets value of input variable according to input data.
 *
 * @param [in]      Size  is size of input parameter.
 * @param [in]      Var_pp is pointer to pointer to new value of input parameter
 * @param [in/out]  Param_p  is pointer to input parameter.
 * @return          None.
 *
 * @remark          It dosen’t check the input parameters.
 */
void Do_ADbg_GetDataVar(uint32  Size,
                        uint8 **Var_pp,
                        void   *Param_p);

/**
 * @brief Function that tests input expression and saves result of the test.
 *
 * @param [in]      Expression is assert result.
 * @param [in]      Case_p is pointer to information about one test case.
 * @return          None.
 *
 * @remark          It dosen’t check the input parameters.
 */
void Do_ADbg_Assert(boolean      Expression,
                    ADbg_Case_t *Case_p);

/** @} */
#endif /*__R_ADBG_ASSERT_H__*/
