/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_INT_FUNCTION_H__
#define __R_ADBG_INT_FUNCTION_H__

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
 * @brief Function that allocates memory space for requested interface function.
 *
 * @return       Pointer to information about one interface function.
 *
 * @remark       None.
 */
ADbg_IntFunction_t *Do_ADbg_IntFunction_Create(void);

/**
 * @brief Function that frees the reserved memory space.
 *
 * @param [in]   IntFunction_p is pointer to information about one interface
 *               function.
 * @return       None.
 *
 * @remark       None.
 */
void Do_ADbg_IntFunction_Destroy(ADbg_IntFunction_t *IntFunction_p);

/**
 * @brief Function that initializes information about interface function.
 *
 * @param [in]     Result_p is pointer to structure used to prepare general
 *                 response.
 * @param [in]     Command_p is pointer to information about received command.
 * @param [in]     Recovery_p is pointer to information for recovery condition.
 * @param [in/out] IntFunction_p is information about one interface function.
 * @return         none.
 *
 * @remark         It dosen’t check the input parameters.
 */
void Do_ADbg_IntFunction_Init(ADbg_Result_t      *Result_p,
                              ADbg_Command_t     *Command_p,
                              ADbg_Recovery_t    *Recovery_p,
                              ADbg_IntFunction_t *IntFunction_p);

/**
 * @brieef Function that sets new value of variable or recovers its original
 *         value, depend on received command.
 *
 * @param [in] Var_p is pointer to variable which value should be changed.
 * @param [in] VarBackup_p is pointer to variable for backup.
 * @param [in] Size is size of variable which value should be changed, in bytes.
 * @param [in] IntFunction_p is information about one interface function.
 * @return     None.
 *
 * @remark     It dosen’t check the input parameters.
 */
void Do_ADbg_SetAndRecoveryCondition(void               *Var_p,
                                     void               *VarBackup_p,
                                     uint8               Size,
                                     ADbg_IntFunction_t *IntFunction_p);

/** @} */
#endif /*__R_ADBG_INT_FUNCTION_H__*/
