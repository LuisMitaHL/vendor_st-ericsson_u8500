/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef ADBG_COMMAND_H_
#define ADBG_COMMAND_H_

/**
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
* Includes
 ******************************************************************************/
#include "t_adbg.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
extern ErrorCode_e ExpectedResult;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * @brief Function that initializes information about received command.
 *
 * @param [in]     Command is command number of received command.
 * @param [in]     ModuleId is module Id.
 * @param [in]     CaseId is case Id.
 * @param [in]     IntGroupId is interface group Id.
 * @param [in]     IntFunctionId is interface function Id.
 * @param [in]     RecoveryFlag if TRUE then condition should be saved.
 * @param [in]     Data_p is pointer to parameters.
 * @param [in/out] Command_p is pointer to information about received command.
 * @return         None.
 *
 * @remark         It dosen’t check the input parameters.
 */
void Do_ADbg_Command_Init(uint8  Command,
                          uint32 ModuleId,
                          uint32 CaseId,
                          uint32 IntGroupId,
                          uint32 IntFunctionId,
                          uint32 RecoveryFlag,
                          uint8 *Data_p,
                          ADbg_Command_t *Command_p);

/**
 * @brief Function that decides what should be done in specified module.
 *
 * @param [in/out] Module_p is pointer to information about one module.
 * @return         None.
 *
 * @remark         It dosen’t check the input parameters.
 */
void Do_ADbg_InModule_Command(ADbg_Module_t *Module_p);

/**
 * @brief Function that decides what should be done in specified interface
 *        group.
 *
 * @param [in/out] IntGroup_p is pointer to information about one interface
 *                 group.
 * @return         None.
 *
 * @remark         It dosen’t check the input parameters.
 */
void Do_ADbg_InIntGroup_Command(ADbg_IntGroup_t *IntGroup_p);

/** @} */
#endif /*ADBG_COMMAND_H_*/
