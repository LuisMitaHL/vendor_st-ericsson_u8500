/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh1606644.c
 *
 * @brief Test cases for CNH1606644 module.
 *
 * This file consist of test cases for automatic testing functions from
 * CNH1606644 module.
 */

/*
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_adbg.h"
#include "r_adbg_assert.h"
#include "r_adbg_case.h"
#include "r_adbg_module.h"
#include "r_adbg_command.h"
#include "r_service_management.h"
#include "block_device_management.h"
#include "boot_area_management.h"
#include "cops_data_manager.h"
#include "os_free_fs.h"
#include "flash_manager.h"
#include "global_data_manager.h"
#include "r_pd_nand.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
//Service management
static void ADbg_Do_Register_Service(ADbg_Case_t *Case_p);
static void ADbg_Do_UnRegister_Service(ADbg_Case_t *Case_p);
static void ADbg_Do_Stop_Service(ADbg_Case_t *Case_p);
static void ADbg_IsStarted(ADbg_Case_t *Case_p);
static void ADbg_GetReferenceCount(ADbg_Case_t *Case_p);

//Block device manager
static void ADbg_Do_BDM_Start(ADbg_Case_t *Case_p);
static void ADbg_Do_BDM_Stop(ADbg_Case_t *Case_p);
static void ADbg_Do_BDM_Read(ADbg_Case_t *Case_p);
static void ADbg_Do_BDM_Write(ADbg_Case_t *Case_p);
static void ADbg_Do_BDM_GetInfo(ADbg_Case_t *Case_p);
static void ADbg_Do_BDM_GetFSInterface(ADbg_Case_t *Case_p);
static void ADbg_Do_BDM_Junk(ADbg_Case_t *Case_p);

//Boot area manager
static void ADbg_Do_BAM_Start(ADbg_Case_t *Case_p);
static void ADbg_Do_BAM_Stop(ADbg_Case_t *Case_p);
static void ADbg_Do_BAM_Read(ADbg_Case_t *Case_p);
static void ADbg_Do_BAM_Write(ADbg_Case_t *Case_p);
static void ADbg_Do_BAM_CompleteErase(ADbg_Case_t *Case_p);

//Cops data manager
static void ADbg_Do_COPS_Start(ADbg_Case_t *Case_p);
static void ADbg_Do_COPS_Stop(ADbg_Case_t *Case_p);

//File system manager
/** TODO */

//Flash manager
static void ADbg_Do_FPD_Start(ADbg_Case_t *Case_p);
static void ADbg_Do_FPD_GetInfo(ADbg_Case_t *Case_p);
static void ADbg_Do_FPD_Stop(ADbg_Case_t *Case_p);
static void ADbg_Do_FPD_ReadPage(ADbg_Case_t *Case_p);
static void ADbg_Do_FPD_ReadConsecutivePages(ADbg_Case_t *Case_p);
static void ADbg_Do_FPD_Erase(ADbg_Case_t *Case_p);

//Global data manager
static void ADbg_Do_GD_Start(ADbg_Case_t *Case_p);
static void ADbg_Do_GD_Stop(ADbg_Case_t *Case_p);
static void ADbg_Do_GD_GetUnitSize(ADbg_Case_t *Case_p);
static void ADbg_Do_GD_ReadUnit(ADbg_Case_t *Case_p);
static void ADbg_Do_GD_WriteUnit(ADbg_Case_t *Case_p);
static void ADbg_Do_GD_Format(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
//Service management
static uint8 Parameters1_2[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Register_Service, 2, 19, ADbg_Do_Register_Service, Parameters1_2);
static uint8 Parameters1_3[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_UnRegister_Service, 3, 19, ADbg_Do_UnRegister_Service, Parameters1_3);
static uint8 Parameters1_4[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Stop_Service, 4, 19, ADbg_Do_Stop_Service, Parameters1_4);
static uint8 Parameters1_5[3] = {2, WORD, BYTE};
ADBG_CASE_DEFINE(Test_IsStarted, 5, 19, ADbg_IsStarted, Parameters1_5);
static uint8 Parameters1_6[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_GetReferenceCount, 6, 19, ADbg_GetReferenceCount, Parameters1_6);

//Block device manager
static uint8 Parameters2_1[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_Start, 7, 19, ADbg_Do_BDM_Start, Parameters2_1);
static uint8 Parameters2_2[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_Stop, 8, 19, ADbg_Do_BDM_Stop, Parameters2_2);
static uint8 Parameters2_3[5] = {4, WORD, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_Read, 9, 19, ADbg_Do_BDM_Read, Parameters2_3);
static uint8 Parameters2_4[] = {2, WORD, WORD, WORD, BYTEPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_Write, 10, 19, ADbg_Do_BDM_Write, Parameters2_4);
static uint8 Parameters2_5[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_GetInfo, 11, 19, ADbg_Do_BDM_GetInfo, Parameters2_5);
static uint8 Parameters2_6[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_GetFSInterface, 12, 19, ADbg_Do_BDM_GetFSInterface, Parameters2_6);
static uint8 Parameters2_7[5] = {4, WORD, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_BDM_Junk, 13, 19, ADbg_Do_BDM_Junk, Parameters2_7);

//Boot area manager
static uint8 Parameters3_1[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_BAM_Start, 14, 19, ADbg_Do_BAM_Start, Parameters3_1);
static uint8 Parameters3_2[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_BAM_Stop, 15, 19, ADbg_Do_BAM_Stop, Parameters3_2);
static uint8 Parameters3_3[5] = {4, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_BAM_Read, 16, 19, ADbg_Do_BAM_Read, Parameters3_3);
static uint8 Parameters3_4[6] = {5, WORD, WORD, WORD, BYTEPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_BAM_Write, 17, 19, ADbg_Do_BAM_Write, Parameters3_4);
static uint8 Parameters3_5[3] = {2, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_BAM_CompleteErase, 18, 19, ADbg_Do_BAM_CompleteErase, Parameters3_5);

//Cops data manager
static uint8 Parameters4_1[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_COPS_Start, 19, 19, ADbg_Do_COPS_Start, Parameters4_1);
static uint8 Parameters4_2[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_COPS_Stop, 20, 19, ADbg_Do_COPS_Stop, Parameters4_2);

//File system manager
/** TODO */

//Flash manager
static uint8 Parameters6_1[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_FPD_Start, 67, 19, ADbg_Do_FPD_Start, Parameters6_1);
static uint8 Parameters6_2[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_FPD_GetInfo, 68, 19, ADbg_Do_FPD_GetInfo, Parameters6_2);
static uint8 Parameters6_3[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_FPD_Stop, 69, 19, ADbg_Do_FPD_Stop, Parameters6_3);
static uint8 Parameters6_4[5] = {4, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_FPD_ReadPage, 70, 19, ADbg_Do_FPD_ReadPage, Parameters6_4);
static uint8 Parameters6_5[6] = {5, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_FPD_ReadConsecutivePages, 71, 19, ADbg_Do_FPD_ReadConsecutivePages, Parameters6_5);
static uint8 Parameters6_8[4] = {3, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_FPD_Erase, 74, 19, ADbg_Do_FPD_Erase, Parameters6_8);

//Global data manager
static uint8 Parameters7_1[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_GD_Start, 75, 19, ADbg_Do_GD_Start, Parameters7_1);
static uint8 Parameters7_2[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_GD_Stop, 76, 19, ADbg_Do_GD_Stop, Parameters7_2);
static uint8 Parameters7_3[4] = {3, SHORT, WORD};
ADBG_CASE_DEFINE(Test_Do_GD_GetUnitSize, 77, 19, ADbg_Do_GD_GetUnitSize, Parameters7_3);
static uint8 Parameters7_4[5] = {4, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_GD_ReadUnit, 78, 19, ADbg_Do_GD_ReadUnit, Parameters7_4);
static uint8 Parameters7_5[5] = {4, SHORT, WORD, BYTEPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_GD_WriteUnit, 79, 19, ADbg_Do_GD_WriteUnit, Parameters7_5);
static uint8 Parameters7_6[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_GD_Format, 80, 19, ADbg_Do_GD_Format, Parameters7_6);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1606644, 0, 19)

//Service management
ADBG_SUITE_ENTRY(Test_Do_Register_Service)
ADBG_SUITE_ENTRY(Test_Do_UnRegister_Service)
ADBG_SUITE_ENTRY(Test_Do_Stop_Service)
ADBG_SUITE_ENTRY(Test_IsStarted)
ADBG_SUITE_ENTRY(Test_GetReferenceCount)

//Block device manager
ADBG_SUITE_ENTRY(Test_Do_BDM_Start)
ADBG_SUITE_ENTRY(Test_Do_BDM_Stop)
ADBG_SUITE_ENTRY(Test_Do_BDM_Read)
ADBG_SUITE_ENTRY(Test_Do_BDM_Write)
ADBG_SUITE_ENTRY(Test_Do_BDM_GetInfo)
ADBG_SUITE_ENTRY(Test_Do_BDM_GetFSInterface)
ADBG_SUITE_ENTRY(Test_Do_BDM_Junk)

//Boot area manager
ADBG_SUITE_ENTRY(Test_Do_BAM_Start)
ADBG_SUITE_ENTRY(Test_Do_BAM_Stop)
ADBG_SUITE_ENTRY(Test_Do_BAM_Read)
ADBG_SUITE_ENTRY(Test_Do_BAM_Write)
ADBG_SUITE_ENTRY(Test_Do_BAM_CompleteErase)

//Cops data manager
ADBG_SUITE_ENTRY(Test_Do_COPS_Start)
ADBG_SUITE_ENTRY(Test_Do_COPS_Stop)

//File system manager
/** TODO */

//Flash manager
ADBG_SUITE_ENTRY(Test_Do_FPD_Start)
ADBG_SUITE_ENTRY(Test_Do_FPD_GetInfo)
ADBG_SUITE_ENTRY(Test_Do_FPD_Stop)
ADBG_SUITE_ENTRY(Test_Do_FPD_ReadPage)
ADBG_SUITE_ENTRY(Test_Do_FPD_ReadConsecutivePages)
ADBG_SUITE_ENTRY(Test_Do_FPD_Erase)

//Global data manager
ADBG_SUITE_ENTRY(Test_Do_GD_Start)
ADBG_SUITE_ENTRY(Test_Do_GD_Stop)
ADBG_SUITE_ENTRY(Test_Do_GD_GetUnitSize)
ADBG_SUITE_ENTRY(Test_Do_GD_ReadUnit)
ADBG_SUITE_ENTRY(Test_Do_GD_WriteUnit)
ADBG_SUITE_ENTRY(Test_Do_GD_Format)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1606644_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1606644, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/

/*******************************************************************************
 * Service management.
 ******************************************************************************/

/**
 * This function will test function:
 * Do_Register_Service.
 * Function used in: Test_Do_Register_Service.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Register_Service(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    Service_t Service = FPD_SERVICE;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(Service_t), &Var_p, &Service);

    Result = Do_Register_Service(Service);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_UnRegister_Service.
 * Function used in: Test_Do_UnRegister_Service.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_UnRegister_Service(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    Service_t Service = FPD_SERVICE;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(Service_t), &Var_p, &Service);

    Result = Do_UnRegister_Service(Service);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_Stop_Service.
 * Function used in: Test_Do_Stop_Service.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Stop_Service(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    Service_t Service = FPD_SERVICE;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(Service_t), &Var_p, &Service);

    Result = Do_Stop_Service(Service);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * IsStarted.
 * Function used in: Test_IsStarted.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_IsStarted(ADbg_Case_t *Case_p)
{
    boolean Result = E_GENERAL_FATAL_ERROR;
    Service_t Service = FPD_SERVICE;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(Service_t), &Var_p, &Service);

    Result = IsStarted(Service);
    Do_ADbg_Assert((boolean)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * GetReferenceCount.
 * Function used in: Test_GetReferenceCount.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_GetReferenceCount(ADbg_Case_t *Case_p)
{
    int Result = E_GENERAL_FATAL_ERROR;
    Service_t Service = FPD_SERVICE;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(Service_t), &Var_p, &Service);

    Result = GetReferenceCount(Service);
    Do_ADbg_Assert((int)(*Var_p) == Result, Case_p);
}

/*******************************************************************************
 * Block device manager.
 ******************************************************************************/
/**
 * This function will test function:
 * Do_BDM_Start.
 * Function used in: Test_Do_BDM_Start.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_Start(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_BDM_Start();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_BDM_Stop.
 * Function used in: Test_Do_BDM_Stop.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_Stop(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_BDM_Stop();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_BDM_Read.
 * Function used in: Test_Do_BDM_Read.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_Read(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Unit = 0;
    uint32 Pos = 0;
    uint32 Length = 0;
    uint32 FirstBad = 0;
    uint8 *Data_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Pos);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Length);

    Data_p = (uint8 *)malloc(Length);
    ASSERT(NULL != Data_p);

    Result = Do_BDM_Read(Unit, Pos, Length, &FirstBad, Data_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_BDM_Write.
 * Function used in: Test_Do_BDM_Write.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_Write(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Unit = 0;
    uint32 Pos = 0;
    uint32 Length = 0;
    uint32 FirstBad = 0;
    uint8 *Data_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Pos);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Length);

    Data_p = (uint8 *)Do_ADbg_GetDataPointer(Length, (void **)&Var_p);

    Result = Do_BDM_Write(Unit, Pos, Length, &FirstBad, Data_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_BDM_GetInfo.
 * Function used in: Test_Do_BDM_GetInfo.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_GetInfo(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Unit = 0;
    BDM_Unit_Config_t Configuration;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit);

    Result = Do_BDM_GetInfo(Unit, &Configuration);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_BDM_GetFSInterface.
 * Function used in: Test_Do_BDM_GetFSInterface.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_GetFSInterface(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Unit = 0;
    DdbHandle Device;
    DdbFunctions *Functions_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit);

    Result = Do_BDM_GetFSInterface(Unit, &Device, &Functions_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Functions_p);
}

/**
 * This function will test function:
 * Do_BDM_Junk.
 * Function used in: Test_Do_BDM_Junk.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BDM_Junk(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Unit = 0;
    uint32 Position = 0;
    uint32 NrOfSectors = 0;
    uint32 FirstBad = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Position);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &NrOfSectors);

    Result = Do_BDM_Junk(Unit, Position, NrOfSectors, &FirstBad);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/*******************************************************************************
 * Boot area manager.
 ******************************************************************************/
/**
 * This function will test function:
 * Do_BAM_Start.
 * Function used in: Test_Do_BAM_Start.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BAM_Start(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_BAM_Start();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 *  Do_BAM_Stop.
 * Function used in: Test_Do_BAM_Stop.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BAM_Stop(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_BAM_Stop();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_BAM_Read.
 * Function used in: Test_Do_BAM_Read.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BAM_Read(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Offset = 0;
    uint8 *Data_p = NULL;
    uint32 DataSize = 0;
    uint8 *Var_p = NULL;
    uint32 Unit = 0;  //TODO: check this variable.    //xvsvlpi

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Offset);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &DataSize);

    Data_p = (uint8 *)malloc(DataSize);
    ASSERT(NULL != Data_p);

    Result = Do_BAM_Read(Unit, Offset, Data_p, DataSize);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_BAM_Write.
 * Function used in: Test_Do_BAM_Write.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BAM_Write(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Offset = 0;
    uint8 *Data_p = NULL;
    uint32 DataSize = 0;
    uint8 *Var_p = NULL;
    uint32 Unit = 0;  //TODO: check this variable.    //xvsvlpi
    uint32 BlockNr = 0;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &BlockNr);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Offset);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &DataSize);

    Data_p = (uint8 *)Do_ADbg_GetDataPointer(DataSize, (void **)&Var_p);

    Result = Do_BAM_Write(Unit, BlockNr, Offset, Data_p, DataSize);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_BAM_CompleteErase.
 * Function used in: Test_Do_BAM_CompleteErase.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BAM_CompleteErase(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 Unit = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit);

    Result = Do_BAM_CompleteErase(Unit);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/*******************************************************************************
 * Cops data manager.
 ******************************************************************************/
/**
 * This function will test function:
 * Do_COPS_Start.
 * Function used in: Test_Do_COPS_Start.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_COPS_Start(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

#ifndef DISABLE_SECURITY
    Result = Do_COPS_Start();
#endif
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_COPS_Stop.
 * Function used in: Test_Do_COPS_Stop.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_COPS_Stop(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_COPS_Stop();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/*******************************************************************************
 * File system manager.
 ******************************************************************************/
/** TODO */

/*******************************************************************************
 * Flash manager.
 ******************************************************************************/
/**
 * This function will test function:
 * Do_FPD_Start.
 * Function used in: Test_Do_FPD_Start.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_FPD_Start(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_FPD_Start();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_FPD_GetInfo.
 * Function used in: Test_Do_FPD_GetInfo.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_FPD_GetInfo(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    FPD_Config_t Configuration;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_FPD_GetInfo(&Configuration);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_FPD_Stop.
 * Function used in: Test_Do_FPD_Stop.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_FPD_Stop(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_FPD_Stop();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_FPD_ReadPage.
 * Function used in: Test_Do_FPD_ReadPage.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_FPD_ReadPage(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPD_Config = {0};
    uint32 PageOffset = 0;
    uint8 *Data_p = NULL;
    uint8 *ExtraData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &PageOffset);

    (void)Do_FPD_GetInfo(&FPD_Config);

    Data_p = (uint8 *)malloc(FPD_Config.PageSize);
    ASSERT(NULL != Data_p);

    Result = Do_FPD_ReadPage(PageOffset, Data_p, ExtraData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_FPD_ReadConsecutivePages.
 * Function used in: Test_Do_FPD_ReadConsecutivePages.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_FPD_ReadConsecutivePages(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    FPD_Config_t FPD_Config = {0};
    uint32 PageOffset = 0;
    uint32 NbrOfPages = 0;
    uint8 *Data_p = NULL;
    uint8 *ExtraData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &PageOffset);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &NbrOfPages);

    (void)Do_FPD_GetInfo(&FPD_Config);

    Data_p = (uint8 *)malloc(NbrOfPages * FPD_Config.PageSize);
    ASSERT(NULL != Data_p);

    Result = Do_FPD_ReadConsecutivePages(PageOffset, NbrOfPages, Data_p, ExtraData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_FPD_Erase.
 * Function used in: Test_Do_FPD_Erase.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_FPD_Erase(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 PageOffset = 0;
    uint32 NbrOfBlocks = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &PageOffset);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &NbrOfBlocks);

    Result = Do_FPD_Erase(PageOffset, NbrOfBlocks);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/*******************************************************************************
 * Global data manager.
 ******************************************************************************/
/**
 * This function will test function:
 * Do_GD_Start.
 * Function used in: Test_Do_GD_Start.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GD_Start(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_GD_Start();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_GD_Stop.
 * Function used in: Test_Do_GD_Stop.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GD_Stop(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_GD_Stop();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_GD_GetUnitSize.
 * Function used in: Test_Do_GD_GetUnitSize.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GD_GetUnitSize(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Unit = 0;
    uint32 Size = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Unit);

    Result = Do_GD_GetUnitSize(Unit, &Size);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_GD_ReadUnit.
 * Function used in: Test_Do_GD_ReadUnit.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GD_ReadUnit(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Unit = 0;
    uint8 *Data_p = NULL;
    uint32 Size = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Unit);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Size);

    Data_p = (uint8 *)malloc(Size);
    ASSERT(NULL != Data_p);

    Result = Do_GD_ReadUnit(Unit, Data_p, Size);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_GD_WriteUnit.
 * Function used in: Test_Do_GD_WriteUnit.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GD_WriteUnit(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Unit = 0;
    uint8 *Data_p = NULL;
    uint32 Size = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Unit);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Size);

    Data_p = (uint8 *)Do_ADbg_GetDataPointer(Size, (void **)&Var_p);

    Result = Do_GD_WriteUnit(Unit, Data_p, Size);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_GD_Format.
 * Function used in: Test_Do_GD_Format.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GD_Format(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result = Do_GD_Format();
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/* @} */
