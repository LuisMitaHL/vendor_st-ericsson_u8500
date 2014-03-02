/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_RESULT_H__
#define __R_ADBG_RESULT_H__

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
 * @brief Function that initializes structure used to prepare general response.
 *
 * @param [in]     StartData_p is pointer to general response payload.
 * @param [in/out] Result_p is pointer to structure used to prepare general
 *                 response.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_Result_Init(uint8 *StartData_p, ADbg_Result_t *Result_p);

/**
 * Function that writes interface function name and ID into buffer to be sent
 * through general response.
 *
 * @param [in] IntGroup_p is pointer to information about one interface group.
 * @return     None.
 *
 * @remark     None.
 */
void Do_CopyIntFunction_List_Result(ADbg_IntGroup_t *IntGroup_p);

/**
 * @brief Function that writes interface group name and ID into buffer to be
 *        sent through general response.
 *
 * @param [in]     TestIntGroup_p is pointer to interface group definitions.
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         None.
 */
void Do_CopyIntGroup_List_Result(const ADbg_IntGroupDefinition_t *TestIntGroup_p,
                                 ADbg_Module_t             *Module_p);

/**
 * @brief Function that writes test case name and ID into buffer to be sent
 *        through general response.
 *
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         None.
 */
void Do_CopyCase_List_Result(ADbg_Module_t *Module_p);

/**
 * @brief Function that writes module name and ID into buffer to be sent through
 *        general response.
 *
 * @param [in]     TestModule_p is pointer to module definitions.
 * @param [in/out] MainModule_p is pointer to information about all modules.
 * @return         None.
 *
 * @remark         None.
 */
void Do_CopyModule_List_Result(const ADbg_ModuleDefinition_t *TestModule_p,
                               ADbg_MainModule_t       *MainModule_p);

/**
 * @brief Function that writes error information into buffer that is sent
 *        through general response.
 *
 * @param [in]     ErrorValue is error code that should be written into
 *                 general response payload.
 * @param [in/out] Result_p is pointer to structure used to prepare general
 *                 response.
 * @return         None.
 *
 * @remark         None.
 */
void Do_Copy_Error_Result(ErrorCode_e    ErrorValue,
                          ADbg_Result_t *Result_p);

/** @} */
#endif /*__R_ADBG_RESULT_H__*/
