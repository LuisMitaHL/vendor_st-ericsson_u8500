/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_MODULE_H__
#define __R_ADBG_MODULE_H__

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
 * @brief Function that allocates memory space for information about requested
 *        module.
 *
 * @param [in] TestCase_pp is pointer to array of all test cases definitions.
 * @param [in] TestIntGroup_pp is pointer to array of all interface groups
 *             definitions.
 * @param [in] Command_p is pointer to information about received command.
 * @param [in] Result_p is pointer to structure used to prepare general response
 * @return     Pointer to information about one module.
 *
 * @remark     It dosen’t check the input parameters.
 */
ADbg_Module_t *Do_ADbg_Module_Create(const ADbg_CaseDefinition_t     **const TestCase_pp,
                                     const ADbg_IntGroupDefinition_t **const TestIntGroup_pp,
                                     ADbg_Command_t                   *Command_p,
                                     ADbg_Result_t                    *Result_p);

/**
 * @brief Function that frees the reserved memory space.
 *
 * @param [in] Module_p is pointer to information about one module.
 * @return     None.
 *
 * @remark     None.
 */
void Do_ADbg_Module_Destroy(ADbg_Module_t *Module_p);

/**
 * @brief Function that finds interface group specified in a command.
 *
 * @param [in] Module_p is pointer to information about one module.
 * @return     Pointer to definition of interface group specified in command.
 *
 * @remark     It dosen’t check the input parameters.
 */
const ADbg_IntGroupDefinition_t *Do_ADbg_FindIntGroup(ADbg_Module_t *Module_p);

/**
 * Function that writes the names and IDs of all test cases in one module in
 * buffer, that holds information to be sent through general response.
 *
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_InModule_ListCase(ADbg_Module_t *Module_p);

/**
 * @brief Function that creates, runs and destroys information about test case.
 *
 * @param [in]     TestCase_p is pointer to test case definition.
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_InModule_Run(const ADbg_CaseDefinition_t *TestCase_p,
                          ADbg_Module_t         *Module_p);

/**
 * @brief Function that does what is needed in one interface group.
 *
 * @param [in]     TestIntGroup_p is pointer to interface group definition.
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_InOneIntGroup(const ADbg_IntGroupDefinition_t *TestIntGroup_p,
                           ADbg_Module_t             *Module_p);

/**
 * @brief Function that does what is needed in all interface groups.
 *
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_InAllIntGroups(ADbg_Module_t *Module_p);

/**
 * @brief Function that finds test case specified in a command.
 *
 * @param [in] Module_p is pointer to information about one module.
 * @return     pointer to definition of test case specified in command.
 *
 * @remark     None.
 */
const ADbg_CaseDefinition_t *Do_ADbg_FindCase(ADbg_Module_t *Module_p);

/** @} */
#endif /*__R_ADBG_MODULE_H__*/
