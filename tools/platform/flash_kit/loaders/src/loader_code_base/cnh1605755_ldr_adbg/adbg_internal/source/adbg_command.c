/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/*
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_adbg_module.h"
#include "r_adbg_int_group.h"
#include "command_ids.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void Do_ADbg_Command_Init(uint8 Command,
                          uint32 ModuleId,
                          uint32 CaseId,
                          uint32 IntGroupId,
                          uint32 IntFunctionId,
                          uint32 RecoveryFlag,
                          uint8 *Data_p,
                          ADbg_Command_t *Command_p)
{
    Command_p->Command = Command;
    Command_p->ModuleId = ModuleId;
    Command_p->CaseId = CaseId;
    Command_p->IntGroupId = IntGroupId;
    Command_p->IntFunctionId = IntFunctionId;
    Command_p->RecoveryFlag = RecoveryFlag;
    Command_p->Data_p = Data_p;
}

void Do_ADbg_InModule_Command(ADbg_Module_t *Module_p)
{
    const ADbg_IntGroupDefinition_t *TestIntGroup_p;
    const ADbg_CaseDefinition_t *TestCase_p;

    switch (Module_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_ADbg_InModule_ListCase(Module_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_ADbg_InAllIntGroups(Module_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        TestIntGroup_p = Do_ADbg_FindIntGroup(Module_p);
        Do_ADbg_InOneIntGroup(TestIntGroup_p, Module_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        TestIntGroup_p = Do_ADbg_FindIntGroup(Module_p);
        Do_ADbg_InOneIntGroup(TestIntGroup_p, Module_p);
        break;
    case COMMAND_ADBG_RUN:
        TestCase_p = Do_ADbg_FindCase(Module_p);
        Do_ADbg_InModule_Run(TestCase_p, Module_p);
        break;
    default:
        break;
    }
}

void Do_ADbg_InIntGroup_Command(ADbg_IntGroup_t *IntGroup_p)
{
    ADbg_IntFunctionDefinition_t *TestIntFunction_p;

    switch (IntGroup_p->Command_p->Command) {
    case COMMAND_ADBG_LISTINTERFACE:
        Do_ADbg_InIntGroup_ListInterface(IntGroup_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        TestIntFunction_p = Do_ADbg_FindIntFunction(IntGroup_p);
        Do_ADbg_RunIntFunction(TestIntFunction_p, IntGroup_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        TestIntFunction_p = Do_ADbg_FindIntFunction(IntGroup_p);
        Do_ADbg_RunIntFunction(TestIntFunction_p, IntGroup_p);
        break;
    default:
        break;
    }
}

/* @} */
