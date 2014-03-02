/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 *  @{
 *      \addtogroup Interface
 *      \brief Interface for configuration of R15 emulator
 *      @{
*/
#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#ifdef WIN32
#include <windows.h>
#endif //WIN32

#include "c_system_v2.h"
#include "error_codes.h"
#include "t_basicdefinitions.h"
#include "r_debug.h"
#include "r_emulator.h"
#include "r_emul_internal.h"
#include "t_emul_threads.h"
#include "r_main.h"

/** This macro defines 1ms for timer in Windows */
#define TIME_1MS  -000100000LL

//ErrorCode_e emul_interface(uint32 Argc, char *Argv_p[])
ErrorCode_e EMUL_Config(uint32 Argc, char *Argv_p[])
{
    FILE *ConfigFile_p;
    Line_t FileLine;
    uint32 i = 0;
    uint8 *Argument_p;
    uint8 *NextArgument_p;

    FileLine.LineNr = 0;
    FileLine.ArgNr = 0;

    for (i = 0; i < MAX_NR_ARGUMENTS; i++) {
        FileLine.Arg_p[i] = NULL;
    }

    //  emul_interfaceInit();
    EMUL_ConfigInit();

    if (Argc < 2) {
        A_(printf("emul_interface.c (%d): Configuration file not specified\n", __LINE__);)
        return E_CONFIG_FILE_NOT_SPECIFIED;
    }

    ConfigFile_p = fopen(Argv_p[1], "r");

    if (NULL == ConfigFile_p) {
        A_(printf("emul_interface.c (%d): Failed to open configuration file: %s\n", __LINE__, Argv_p[1]);)
        return E_CANNOT_OPEN_FILE;
    }

    strncpy(FileLine.FileName, Argv_p[1], MAX_FILE_NAME);

    i = 1;

    while (fgets(FileLine.LineData, MAX_LINE_LENGTH, ConfigFile_p) != NULL) {
        FileLine.LineNr = i;
        i++;

        Argument_p = strtok_s(FileLine.LineData,  Separators, &NextArgument_p);

        while (Argument_p != NULL) {
            FileLine.Arg_p[FileLine.ArgNr] = Argument_p;
            FileLine.ArgNr++;
            Argument_p = strtok_s(NULL,  Separators, &NextArgument_p);
        }

        if (0 == strcmp(FileLine.Arg_p[0], "-nand")) {
            C_(printf("emul_interface.c (%d): Started configuring nand memory\n", __LINE__);)
            //      emul_interfaceNand(&FileLine);
        } else if (0 == strcmp(FileLine.Arg_p[0], "\r\n")) {
            continue;
        } else {
            A_(printf("emul_interface.c(%d): Error unrecognized argument in file %s, line %d, argument no %d, argument %s\n", __LINE__, FileLine.FileName, FileLine.LineNr, 1, FileLine.Arg_p[0]);)
        }
    }

    if (EOF == fclose(ConfigFile_p)) {
        A_(printf("emul_interface.c (%d): Error failed to close configuration file\n", __LINE__);)
    }

    return E_SUCCESS;
}

ErrorCode_e EMUL_Init()
{
    uint32 i;
    LARGE_INTEGER DueTime;

    UART_Handles_p  = (UART_Handles_t *) malloc(sizeof(UART_Handles_t));
    assert(NULL != UART_Handles_p);
    Timer_Handles_p = (Timer_Handles_t *) malloc(sizeof(Timer_Handles_t));
    assert(NULL != Timer_Handles_p);

    Timer_Handles_p->Timer_Event_h = CreateWaitableTimer(NULL, FALSE, NULL);
    DueTime.QuadPart = TIME_1MS;

    if (!SetWaitableTimer(Timer_Handles_p->Timer_Event_h, &DueTime, 1, NULL, NULL, 0)) {
        printf("SetWaitableTimer failed (%d)\n", GetLastError());
        return E_FAILED_TO_SET_TIMER;
    }

    UART_Handles_p->UDev = HL1UART_DEVICE_0;
    UART_Handles_p->UART_NrEvents = NR_OF_UART_EVENTS;
    UART_Handles_p->UART_ShutDown = FALSE;

    for (i = 0; i < NR_OF_UART_EVENTS; i++) {
        UART_Handles_p->UART_Events_h[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (NULL == UART_Handles_p->UART_Events_h[i]) {
            printf("emul_interface.c (%d): Error failed to create event no %d\n", __LINE__, i);
        } else {
            ResetEvent(UART_Handles_p->UART_Events_h[i]);
        }
    }

    UART_Handles_p->UART_NrInterrupts = NR_OF_UART_INTERRUPTS;

    for (i = 0; i < NR_OF_UART_INTERRUPTS; i++) {
        UART_Handles_p->UART_Interrupts_h[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        if (NULL == UART_Handles_p->UART_Interrupts_h[i]) {
            printf("emul_interface.c (%d): Error failed to create event no %d\n", __LINE__, i);
        } else {
            ResetEvent(UART_Handles_p->UART_Interrupts_h[i]);
        }
    }

    UART_Handles_p->UART_EnableInterrupts_h = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (NULL == UART_Handles_p->UART_EnableInterrupts_h) {
        printf("emul_interface.c (%d): Error failed to create event hUART_EnableInterrupts\n", __LINE__);
    } else {
        SetEvent(UART_Handles_p->UART_EnableInterrupts_h);
    }

    return E_SUCCESS;
}
/**      @}*/
/**@}*/
