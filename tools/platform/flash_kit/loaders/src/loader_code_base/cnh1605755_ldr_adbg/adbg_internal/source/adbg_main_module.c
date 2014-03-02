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
#include <stdlib.h>
#include "r_adbg_result.h"
#include "command_ids.h"
#include "c_system.h"

/*******************************************************************************
 * Module runners
 ******************************************************************************/
#ifdef  CFG_ENABLE_ADBG_CNH1605194
void Do_CNH1605194_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605194_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605195
void Do_CNH1605195_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605195_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605196
void Do_CNH1605196_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605196_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605197
void Do_CNH1605197_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605197_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605198
void Do_CNH1605198_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605198_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605200
void Do_CNH1605200_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605200_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605203
void Do_CNH1605203_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605203_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605204
void Do_CNH1605204_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605204_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605205
void Do_CNH1605205_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605205_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605208
void Do_CNH1605208_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605208_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605551
void Do_CNH1605551_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605551_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605720
void Do_CNH1605720_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605720_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1605721
void Do_CNH1605721_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1605721_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1606344
void Do_CNH1606344_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1606344_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1606558
void Do_CNH1606558_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1606558_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1606559
void Do_CNH1606559_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1606559_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1606560
void Do_CNH1606560_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1606560_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1606644
void Do_CNH1606644_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1606644_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1606908
void Do_CNH1606908_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1606908_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif
#ifdef  CFG_ENABLE_ADBG_CNH1607471
void Do_CNH1607471_Module_Test_Run(ADbg_MainModule_t *MainModule_p);
void Do_CNH1607471_Module_Interface_Run(ADbg_MainModule_t *MainModule_p);
#endif


#ifdef  CFG_ENABLE_ADBG_CNH1605194
void Run_CNH1605194_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605194_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605194_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605194_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605194_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605194_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605194

#ifdef  CFG_ENABLE_ADBG_CNH1605195
void Run_CNH1605195_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605195_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605195_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605195_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605195_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605195_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605195

#ifdef  CFG_ENABLE_ADBG_CNH1605196
void Run_CNH1605196_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605196_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605196_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605196_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605196_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605196_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif // CFG_ENABLE_ADBG_CNH1605196

#ifdef  CFG_ENABLE_ADBG_CNH1605197
void Run_CNH1605197_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605197_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605197_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605197_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605197_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605197_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605197

#ifdef  CFG_ENABLE_ADBG_CNH1605198
void Run_CNH1605198_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605198_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605198_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605198_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605198_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605198_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605198

#ifdef  CFG_ENABLE_ADBG_CNH1605200
void Run_CNH1605200_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605200_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605200_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605200_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605200_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605200_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605200

#ifdef  CFG_ENABLE_ADBG_CNH1605203
void Run_CNH1605203_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605203_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605203_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605203_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605203_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605203_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605203

#ifdef  CFG_ENABLE_ADBG_CNH1605204
void Run_CNH1605204_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605204_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605204_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605204_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605204_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605204_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605204

#ifdef  CFG_ENABLE_ADBG_CNH1605205
void Run_CNH1605205_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605205_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605205_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605205_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605205_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605205_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605205

#ifdef  CFG_ENABLE_ADBG_CNH1605208
void Run_CNH1605208_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605208_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605208_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605208_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605208_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605208_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605208

#ifdef  CFG_ENABLE_ADBG_CNH1605551
void Run_CNH1605551_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605551_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605551_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605551_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605551_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605551_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605551

#ifdef  CFG_ENABLE_ADBG_CNH1605720
void Run_CNH1605720_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605720_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605720_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605720_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605720_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605720_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605720

#ifdef  CFG_ENABLE_ADBG_CNH1605721
void Run_CNH1605721_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1605721_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1605721_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1605721_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1605721_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1605721_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1605721

#ifdef  CFG_ENABLE_ADBG_CNH1606344
void Run_CNH1606344_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1606344_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1606344_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1606344_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1606344_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1606344_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1606344

#ifdef  CFG_ENABLE_ADBG_CNH1606558
void Run_CNH1606558_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1606558_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1606558_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1606558_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1606558_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1606558_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1606558

#ifdef  CFG_ENABLE_ADBG_CNH1606559
void Run_CNH1606559_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1606559_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1606559_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1606559_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1606559_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1606559_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1606559

#ifdef  CFG_ENABLE_ADBG_CNH1606560
void Run_CNH1606560_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1606560_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1606560_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1606560_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1606560_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1606560_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1606560

#ifdef  CFG_ENABLE_ADBG_CNH1606644
void Run_CNH1606644_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1606644_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1606644_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1606644_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1606644_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1606644_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1606644

#ifdef  CFG_ENABLE_ADBG_CNH1606908
void Run_CNH1606908_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1606908_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1606908_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1606908_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1606908_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1606908_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1606908

#ifdef  CFG_ENABLE_ADBG_CNH1607471
void Run_CNH1607471_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    switch (MainModule_p->Command_p->Command) {
    case COMMAND_ADBG_LISTCASE:
        Do_CNH1607471_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RUN:
        Do_CNH1607471_Module_Test_Run(MainModule_p);
        break;
    case COMMAND_ADBG_LISTINTERFACE:
        Do_CNH1607471_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_SETPRECONDITION:
        Do_CNH1607471_Module_Interface_Run(MainModule_p);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:
        Do_CNH1607471_Module_Interface_Run(MainModule_p);
        break;
    default:
        break;
    }
}
#endif //CFG_ENABLE_ADBG_CNH1607471

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void Do_ADbg_MainModule_Init(const ADbg_ModuleDefinition_t **const TestModule_pp,
                             ADbg_Command_t                 *Command_p,
                             ADbg_Result_t                  *Result_p,
                             ADbg_MainModule_t              *MainModule_p)
{
    MainModule_p->ModuleCounter = 0;
    MainModule_p->TestModule_pp = TestModule_pp;
    MainModule_p->Command_p = Command_p;
    MainModule_p->Result_p = Result_p;
}

const ADbg_ModuleDefinition_t *Do_ADbg_FindModule(ADbg_MainModule_t *MainModule_p)
{
    const ADbg_ModuleDefinition_t **TestModule_pp = MainModule_p->TestModule_pp;
    uint32 ModuleId = MainModule_p->Command_p->ModuleId;

    while (*TestModule_pp != NULL) {
        if ((*TestModule_pp)->ModuleId == ModuleId) {
            return *TestModule_pp;
        }

        TestModule_pp++;
    }

    return NULL;
}

void Do_ADbg_InOneModule(const ADbg_ModuleDefinition_t *TestModule_p,
                         ADbg_MainModule_t       *MainModule_p)
{
    if (TestModule_p != NULL) {
        TestModule_p->Interface.Run(MainModule_p);
    } else {
        MainModule_p->Result_p->ErrorValue = E_MODULE_LIST_EMPTY;
    }
}

void Do_ADbg_InAllModules(ADbg_MainModule_t *MainModule_p)
{
    const ADbg_ModuleDefinition_t **TestModule_pp = MainModule_p->TestModule_pp;

    TestModule_pp++;

    if (*TestModule_pp != NULL) {
        //Leave one byte for number of modules.
        MainModule_p->Result_p->Data_p++;
        MainModule_p->Result_p->Size++;

        while (*TestModule_pp != NULL) {
            const ADbg_ModuleDefinition_t *TestModule_p = *TestModule_pp;
            //Module name and Id
            Do_CopyModule_List_Result(TestModule_p, MainModule_p);

            Do_ADbg_InOneModule(TestModule_p, MainModule_p);
            TestModule_pp++;
            MainModule_p->ModuleCounter++;
        }

        *MainModule_p->Result_p->StartData_p = MainModule_p->ModuleCounter;
    } else {
        MainModule_p->Result_p->ErrorValue = E_MODULE_LIST_EMPTY;
    }
}

/* @} */
