/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_CASE_H__
#define __R_ADBG_CASE_H__

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
 * @brief Function that allocates memory space for requested test case.
 *
 * @return     pointer to information about one module.
 *
 * @remark     It dosen’t check the input parameters.
 */
ADbg_Case_t *Do_ADbg_Case_Create(void);

/**
 * @brief Function that frees the reserved memory space.
 *
 * @param [in] Case_p is pointer to information about one test case.
 * @return     None.
 *
 * @remark     It dosen’t check the input parameters.
 */
void Do_ADbg_Case_Destroy(ADbg_Case_t *Case_p);

/**
 * @brief Function that initializes pointer to information about one test case.
 *
 * @param [in]     Result_p is pointer to structure used to prepare general
 *                 response.
 * @param [in]     Command_p is pointer to information about received command.
 * @param [in/out] Case_p is pointer to information about one test case.
 * @return         None.
 *
 * @remark         It dosen’t check the input parameters.
 */
void Do_ADbg_Case_Init(ADbg_Result_t  *Result_p,
                       ADbg_Command_t *Command_p,
                       ADbg_Case_t    *Case_p);

/** @} */
#endif /*__R_ADBG_CASE_H__*/
