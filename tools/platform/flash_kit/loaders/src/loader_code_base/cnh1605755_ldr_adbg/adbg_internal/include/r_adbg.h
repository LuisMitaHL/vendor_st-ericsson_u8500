/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef __R_ADBG_H__
#define __R_ADBG_H__

/**
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_adbg.h"
#include <stdio.h>

/*******************************************************************************
 * Macros
 ******************************************************************************/

/*******************************************************************************
 * Case suite definitions
 ******************************************************************************/
/**
 * Defines a test case suite defined in a C-file.
 */
#define ADBG_SUITE_DEFINE_BEGIN(Name, CaseId, ModuleId) \
  const ADbg_CaseDefinition_t ADbg_##Name = \
        { #Name, \
          CaseId, \
          ModuleId, \
          NULL, \
          { \
            NULL, \
            NULL, \
            NULL  \
          } \
        };\
  const ADbg_CaseDefinition_t* Name[] = \
        { \
          &ADbg_##Name,

/**
 * Defines a suite entry, this is the name of a case.
 */
#define ADBG_SUITE_ENTRY(Name) \
          &Name,

/**
 * End of the suite.
 */
#define ADBG_SUITE_DEFINE_END() \
          NULL \
        }

/*******************************************************************************
 * Case definitions
 ******************************************************************************/
/**
 * Defines a normal case.
 */
#define  ADBG_CASE_DEFINE(Name, CaseId, ModuleId, Run, Parameters_p) \
  const ADbg_CaseDefinition_t Name = \
        {#Name, \
          CaseId, \
          ModuleId, \
          Parameters_p, \
         {(ADbg_Create_t)Do_ADbg_Case_Create, \
          (ADbg_CaseRun_t)Run, \
          (ADbg_Destroy_t)Do_ADbg_Case_Destroy}}
/**
 * Defines a case with a fixture.
 */
#define  ADBG_CASE_DEFINE_W_FIXTURE(Name, CaseId, ModuleId, Create, Run, Destroy, Parameters_p) \
  const ADbg_CaseDefinition_t Name = \
        {#Name, \
          CaseId, \
          ModuleId, \
          Parameters_p, \
         {(ADbg_Create_t)Create, \
          (ADbg_CaseRun_t)Run, \
          (ADbg_Destroy_t)Destroy}}

/*******************************************************************************
 * Module suite definitions
 ******************************************************************************/
/**
 * Defines a module suite defined in a C-file.
 */
#define ADBG_MODULE_SUITE_DEFINE_BEGIN(Name, ModuleId) \
  const ADbg_ModuleDefinition_t ADbg_##Name = \
        { #Name, \
          ModuleId, \
          { NULL, \
            NULL, \
            NULL} \
        };\
  const ADbg_ModuleDefinition_t* Name[] = \
        { \
          &ADbg_##Name,

/**
 * Defines a suite entry, this is the name of a module.
 */
#define ADBG_MODULE_SUITE_ENTRY(Name) \
          &Name,

/**
 * End of the module suite.
 */
#define ADBG_MODULE_SUITE_DEFINE_END() \
          NULL  \
        }

/*******************************************************************************
 * Module definitions
 ******************************************************************************/
/**
 * Defines a module.
 */
#define  ADBG_MODULE_DEFINE(Name, ModuleId, Run) \
  const ADbg_ModuleDefinition_t Name = \
        {#Name, \
          ModuleId, \
          {NULL, (ADbg_ModuleRun_t)Run, NULL}}

/*******************************************************************************
 * Interface group suite definitions
 ******************************************************************************/
/**
 * Defines an interface group suite defined in a C-file.
 */
#define ADBG_INTERFACE_GROUP_SUITE_DEFINE_BEGIN(Name, GroupId, ModuleId) \
  const ADbg_IntGroupDefinition_t ADbg_##Name = \
        { #Name, \
          GroupId, \
          ModuleId, \
         {NULL, \
          NULL, \
          NULL} \
        };\
  const ADbg_IntGroupDefinition_t* Name[] = \
        { \
          &ADbg_##Name,

/**
 * Defines a suite entry, this is the name of an interface group.
 */
#define ADBG_INTERFACE_GROUP_SUITE_ENTRY(Name) \
          &Name,

/**
 * End of the interface group suite.
 */
#define ADBG_INTERFACE_GROUP_SUITE_DEFINE_END() \
          NULL  \
        }

/*******************************************************************************
 * Interface group definitions
 ******************************************************************************/
/**
 * Defines an interface group.
 */
#define  ADBG_INTERFACE_GROUP_DEFINE(Name, GroupId, ModuleId, Run) \
  const ADbg_IntGroupDefinition_t Name = \
        {#Name, \
          GroupId, \
          ModuleId, \
         {(ADbg_Create_t)Do_ADbg_IntGroup_Create, \
          (ADbg_IntGroupRun_t)Run, \
          (ADbg_Destroy_t)Do_ADbg_IntGroup_Destroy} \
        }

/*******************************************************************************
 * Interface function suite definitions
 ******************************************************************************/
/**
 * Defines an interface function suite defined in a C-file.
 */
#define ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(Name, FunctionId, GroupId, ModuleId) \
  ADbg_IntFunctionDefinition_t ADbg_##Name = \
        { #Name, \
          FunctionId, \
          GroupId, \
          ModuleId, \
          NULL, \
         {FALSE, \
          NULL}, \
         {NULL, \
          NULL, \
          NULL} \
        };\
  ADbg_IntFunctionDefinition_t* Name[] = \
        { \
          &ADbg_##Name,

/**
 * Defines a suite entry, this is the name of an interface function.
 */
#define ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Name) \
          &Name,

/**
 * End of the interface function suite.
 */
#define ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END() \
          NULL  \
        }
/*******************************************************************************
 *
 * INTERFACE FUNCTION GROUP DEFINITIONS
 *
 ******************************************************************************/
/**
 * Defines an interface function.
 */
#define  ADBG_INTERFACE_FUNCTION_DEFINE(Name, FunctionId, GroupId, ModuleId, Run, Parameters_p, Backup_p) \
  ADbg_IntFunctionDefinition_t Name = \
         {#Name, \
           FunctionId, \
           GroupId, \
           ModuleId, \
           Parameters_p, \
          {FALSE, \
           Backup_p}, \
          {(ADbg_Create_t)Do_ADbg_IntFunction_Create, \
           (ADbg_IntFunctionRun_t)Run, \
           (ADbg_Destroy_t)Do_ADbg_IntFunction_Destroy}}

/** @} */
#endif /*__R_ADBG_H__*/
