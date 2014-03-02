/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
#ifndef ADBG_MAIN_MODULE_H_
#define ADBG_MAIN_MODULE_H_

/**
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_adbg.h"
#include "c_system.h"

/*******************************************************************************
 * Module runners
 ******************************************************************************/
#ifdef CFG_ENABLE_ADBG_CNH1605194
void Run_CNH1605194_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605195
void Run_CNH1605195_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605196
void Run_CNH1605196_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605197
void Run_CNH1605197_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605198
void Run_CNH1605198_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605200
void Run_CNH1605200_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605203
void Run_CNH1605203_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605204
void Run_CNH1605204_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605205
void Run_CNH1605205_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605208
void Run_CNH1605208_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605551
void Run_CNH1605551_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605720
void Run_CNH1605720_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605721
void Run_CNH1605721_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606344
void Run_CNH1606344_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606558
void Run_CNH1606558_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606559
void Run_CNH1606559_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606560
void Run_CNH1606560_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606644
void Run_CNH1606644_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606908
void Run_CNH1606908_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1607471
void Run_CNH1607471_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
#endif


//Add new module runner
/*******************************************************************************
 * Module definitions
 ******************************************************************************/
#ifdef CFG_ENABLE_ADBG_CNH1605194
ADBG_MODULE_DEFINE(ADbg_CNH1605194, 1, Run_CNH1605194_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605195
ADBG_MODULE_DEFINE(ADbg_CNH1605195, 2, Run_CNH1605195_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605196
ADBG_MODULE_DEFINE(ADbg_CNH1605196, 3, Run_CNH1605196_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605197
ADBG_MODULE_DEFINE(ADbg_CNH1605197, 4, Run_CNH1605197_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605198
ADBG_MODULE_DEFINE(ADbg_CNH1605198, 5, Run_CNH1605198_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605200
ADBG_MODULE_DEFINE(ADbg_CNH1605200, 6, Run_CNH1605200_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605203
ADBG_MODULE_DEFINE(ADbg_CNH1605203, 7, Run_CNH1605203_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605204
ADBG_MODULE_DEFINE(ADbg_CNH1605204, 8, Run_CNH1605204_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605205
ADBG_MODULE_DEFINE(ADbg_CNH1605205, 9, Run_CNH1605205_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605208
ADBG_MODULE_DEFINE(ADbg_CNH1605208, 10, Run_CNH1605208_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605551
ADBG_MODULE_DEFINE(ADbg_CNH1605551, 12, Run_CNH1605551_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605720
ADBG_MODULE_DEFINE(ADbg_CNH1605720, 13, Run_CNH1605720_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605721
ADBG_MODULE_DEFINE(ADbg_CNH1605721, 14, Run_CNH1605721_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606344
ADBG_MODULE_DEFINE(ADbg_CNH1606344, 15, Run_CNH1606344_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606558
ADBG_MODULE_DEFINE(ADbg_CNH1606558, 16, Run_CNH1606558_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606559
ADBG_MODULE_DEFINE(ADbg_CNH1606559, 17, Run_CNH1606559_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606560
ADBG_MODULE_DEFINE(ADbg_CNH1606560, 18, Run_CNH1606560_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606644
ADBG_MODULE_DEFINE(ADbg_CNH1606644, 19, Run_CNH1606644_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606908
ADBG_MODULE_DEFINE(ADbg_CNH1606908, 20, Run_CNH1606908_Module_Test_Run);
#endif
#ifdef CFG_ENABLE_ADBG_CNH1607471
ADBG_MODULE_DEFINE(ADbg_CNH1607471, 21, Run_CNH1607471_Module_Test_Run);
#endif

//Add new module definition

/*******************************************************************************
* Module suite definition
 ******************************************************************************/
ADBG_MODULE_SUITE_DEFINE_BEGIN(MainModule_pp, 0)
#ifdef CFG_ENABLE_ADBG_CNH1605194
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605194)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605195
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605195)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605196
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605196)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605197
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605197)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605198
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605198)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605200
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605200)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605203
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605203)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605204
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605204)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605205
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605205)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605208
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605208)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605551
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605551)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605720
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605720)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1605721
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1605721)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606344
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1606344)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606558
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1606558)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606559
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1606559)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606560
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1606560)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606644
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1606644)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1606908
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1606908)
#endif
#ifdef CFG_ENABLE_ADBG_CNH1607471
ADBG_MODULE_SUITE_ENTRY(ADbg_CNH1607471)
#endif
//Add new module

ADBG_MODULE_SUITE_DEFINE_END();

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * @brief Function that initializes information about all modules.
 *
 * @param [in] TestModule_pp is pointer to array of all test cases definitions.
 * @param [in] Command_p is pointer to information about received command.
 * @param [in] Result_p is pointer to structure used to prepare general response
 * @param [in] MainModule_p is pointer to information about all modules.
 * @return     None.
 *
 * @remark     It dosen�t check the input parameters.
 */
void Do_ADbg_MainModule_Init(const ADbg_ModuleDefinition_t **const TestModule_pp,
                             ADbg_Command_t                 *Command_p,
                             ADbg_Result_t                  *Result_p,
                             ADbg_MainModule_t              *MainModule_p);

/**
 * @brief Function that finds module specified in command.
 *
 * @param [in] MainModule_p is pointer to information about all modules.
 * @return     Pointer to definition of module specified in command.
 *
 * @remark     None.
 */
const ADbg_ModuleDefinition_t *Do_ADbg_FindModule(ADbg_MainModule_t *MainModule_p);

/**
 * @brief Function that does what is needed in one module.
 *
 * @param [in]     TestModule_p is pointer to module definition.
 * @param [in/out] MainModule_p is pointer to information about all modules.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_InOneModule(const ADbg_ModuleDefinition_t *TestModule_p,
                         ADbg_MainModule_t       *MainModule_p);

/**
 * @brief Function that does what is needed in all modules.
 *
 * @param [in/out] MainModule_p is pointer to information about all modules.
 * @return         None.
 *
 * @remark         None.
 */
void Do_ADbg_InAllModules(ADbg_MainModule_t *MainModule_p);

/** @} */
#endif /*ADBG_MAIN_MODULE_H_*/
