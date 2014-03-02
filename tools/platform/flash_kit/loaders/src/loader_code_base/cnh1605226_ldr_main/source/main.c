/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_main
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include "string.h"
#include "r_main.h"
#include "t_basicdefinitions.h"
#include "error_codes.h"

#include "ldr_init.h"
#include "cpu_support.h"
#include "r_mmu.h"

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_system_applications.h"
#include "r_communication_service.h"
#include "commands.h"
#include "chipid.h"

#include "r_loader_sec_lib.h"
#include "r_command_exec_service.h"
#include "r_io_subsystem.h"
#include "watchdog.h"

#include "r_service_management.h"

#ifdef PRINT_A_
extern const unsigned char CurrentDate[];
#endif //#PRINT_A_

#ifdef ENABLE_BASS_APP_TEST
#include "bass_app_test.h"
#endif

#ifdef ENABLE_EARLY_BREAKPOINT
extern void StopExecution(void);
#endif

//************************************************************************
/*
 *  This is the MAIN.
 */
//************************************************************************

int main(void)
{
    ErrorCode_e InitStatus = E_GENERAL_FATAL_ERROR;

#ifdef ENABLE_EARLY_BREAKPOINT
    /*
     * The function if for debug purposes only
     * It is used to enable 'early' debugging when executing main()
     *
     */
    StopExecution();
#endif

    InitStatus = Start_MMU();

    if (E_SUCCESS != InitStatus) {
        Do_LoaderInternalErrorHandler(InitStatus);
    }

    /* Init Hardware */
    InitStatus = Do_Loader_Hardware_Init();

    if (E_SUCCESS != InitStatus) {
        Do_LoaderInternalErrorHandler(InitStatus);
    }

    /* Init communication protocol */
    InitStatus = Do_Loader_CommunicationProtocolInit();

    if (E_SUCCESS != InitStatus) {
        Do_LoaderInternalErrorHandler(InitStatus);
    }

    /* Init Command Execution Handler */
    Do_CES_Init();

    /* Initialize Security Library */
    InitStatus = (ErrorCode_e)Do_LoaderSecLib_Init();

    if (E_SUCCESS != InitStatus) {
        Do_LoaderInternalErrorHandler(InitStatus);
    }

    (void)CPU_IrqEnable();

#ifdef ENABLE_BASS_APP_TEST
#ifndef DISABLE_SECURITY
    A_(printf("\nbass app test\n");)
    bass_app_test_all();
    A_(printf("\n bass test end!\n\n");)
#endif
#endif

    /* Send loader started signal */
    InitStatus = Do_System_R15LoaderStarted(InitStatus);

    if (InitStatus != E_SUCCESS) {
        A_(printf("\n# Loader not started!\n");)
        A_(printf("#---------------------------------------------------------\n");)
        Do_LoaderInternalErrorHandler(InitStatus);
    }

    Do_Init_Services();

    (void)Do_IO_Init(MAX_OPEN_IO);

    A_(printf("\n#---------------------------------------------------------\n");)
    A_(printf("# %s\n", CurrentDate);)
    A_(printf("# DBChipID: 0x%08X\n", GetDBAsicChipID());)
    A_(printf("# ABChipID: 0x%08X\n", GetABAsicChipID());)
    A_(printf("#---------------------------------------------------------\n");)
    A_(printf("# Loader started!\n");)
    A_(printf("#---------------------------------------------------------\n");)

    /* Start Process */
    /*
     * COVERITY_FALSE_POSITIVE: This is the main polling loop of the loader, and
     * it is designed to be an infinite loop. The Do_Process method inside the
     * loop calls the communication and command execution poll methods
     *
     * */
    /* coverity[no_escape] */
    while (1) {
        Do_Process();
    }

}


void Do_Process(void)
{
    /* Transport Layer TX process */
    ASSERT(E_SUCCESS == Do_Communication_Poll(GlobalCommunication_p));

    /* Command handler */
    Do_CES_CommandExecutionPoll();

#ifdef ENABLE_LOADER_HWI2C_WD
    /* Kick WD */
    HWI2C_Do_WatchDog_Kick();
#endif
}


void Do_LoaderInternalErrorHandler(const ErrorCode_e IntError)
{

    A_(printf("# Error Code: 0x%X !\n", IntError);)
    A_(printf("# Loader stoped!\n");)
    A_(printf("#---------------------------------------------------------\n");)

    StallExecution();
}

void StallExecution(void)
{
    /*
     * COVERITY_FALSE_POSITIVE: The infinite loop is written intentionally.
     * It is used to stop the loaders execution wherever needed (internal error handling,
     * in functions which never return, etc.)
     *
     * */
    /* coverity[no_escape] */
    while (1) {
        ;
    }
}

/** @} */
