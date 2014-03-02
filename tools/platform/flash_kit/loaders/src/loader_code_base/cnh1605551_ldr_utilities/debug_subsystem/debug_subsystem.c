/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup   debug_subsytem
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "r_debug_subsystem.h"
#include "t_debug_subsystem.h"
#include "r_memory_utils.h"
#ifdef CFG_ENABLE_LOADER_TYPE
#include "r_debug_macro.h"
#include "cpu_support.h"
#include "r_queue.h"
#include "r_communication_service.h"
#endif
#ifdef ENABLE_DEBUG
#include "r_UART_abstraction.h"
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#ifdef printf
#undef printf
#endif

#ifndef CFG_ENABLE_LOADER_TYPE
void lcm_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fflush(stdout);
    va_end(args);
}
#else
/**< Debug context*/
extern Communication_t DebugCommunication;

/**< */
uint32 Debug_Output_Channel = OUTPUT_CHANNEL_DEBUG_BUFFER;

/**<Pointer on debug queue.*/
static void *DebugQueue_p;
/**<Flag that temporarily disable writing in debug queue.*/
static boolean IsDebugQueueAvaible = TRUE;
/**<Flag that serve as indicator for queue initialization.*/
static boolean IsDebugQueueInitialized = FALSE;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static int circular_printf(const char *format, ...);
static int uart_printf(const char *format, ...);
static void InitializeDebugQueue(void);
static void DestroyElement(void *Value_p);
static int get_sizef(const char *, const va_list);

#ifdef ENABLE_DEBUG
static void Convert32(uint32 Value, char *Buffer);
#endif

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
extern int vsnprintf(char *__restrict __s, size_t __maxlen,
                     __const char *__restrict __format, __builtin_va_list __arg);

int __wrap_puts(const char *s)
{
    debug_printf("%s\n", s);

    return 1;
}

int __wrap_printf(const char *format, ...)
{
    //don't know if this is correct but the same things is done in debug_printf
    debug_printf(format);

    //as debug printf doesn't return the proper
    //number this just return something !=0
    return 10;
}

/**
 * TODO: This function should return number of bytes readed and negative number
 *       otherwise. Must be fixed.
 */
void debug_printf(const char *format, ...)
{
    if (FALSE == IsDebugQueueInitialized) {
        InitializeDebugQueue();
        IsDebugQueueInitialized = TRUE;
    }

    switch (Debug_Output_Channel) {
    case OUTPUT_CHANNEL_UART0:
#ifndef FORCE_DEBUG_BUFFER
        (void)uart_printf(format);
        break;
#endif
    case OUTPUT_CHANNEL_DEBUG_BUFFER:
    default:
        (void)circular_printf(format);
        break;
    }
}

uint32 Do_ReadDebugQueue(void **Data_pp)
{
    void **HoldBuffer_pp = NULL;
    int NrOfEntries = 0;
    boolean IsQueueEmpty = TRUE;
    uint32 DataLength = 0;
    uint32 EntryLength = 0;
    uint32 Offset = 0;
    int i = 0;

    IsDebugQueueAvaible = FALSE;

    IsQueueEmpty = Do_Fifo_IsEmpty(NULL, DebugQueue_p);

    if (TRUE == IsQueueEmpty) {
        return DataLength;
    }

    NrOfEntries = Do_RFifo_GetNrOfElements(NULL, DebugQueue_p);
    HoldBuffer_pp = (void **)malloc(NrOfEntries * sizeof(void *));
    ASSERT(NULL != HoldBuffer_pp);

    for (i = 0; i < NrOfEntries; i++) {
        if (NULL == (HoldBuffer_pp[i] = Do_RFifo_Dequeue(NULL, DebugQueue_p))) {
            break;
        }

        DataLength += strlen((char *)HoldBuffer_pp[i]);
    }

    *Data_pp = (char *)malloc(DataLength);
    ASSERT(NULL != *Data_pp);

    for (i = 0; i < NrOfEntries; i++) {
        if (NULL != HoldBuffer_pp[i]) {
            EntryLength = strlen((char *)HoldBuffer_pp[i]);
            memcpy(((char *)*Data_pp) + Offset, ((char *)HoldBuffer_pp[i]), EntryLength);
            Offset += EntryLength;
            BUFFER_FREE(HoldBuffer_pp[i]);
        } else {
            break;
        }
    }

    IsDebugQueueAvaible = TRUE;

    BUFFER_FREE(HoldBuffer_pp);

    return DataLength;
}

void Do_DestroyDebugQueue(void **const Queue_pp)
{
    Do_RFifo_Destroy(NULL, Queue_pp);
}

/* This is for the crash handler and is hardcoded to use UART2 for now
 * this can be change in the function DirectPrint */
#ifdef ENABLE_DEBUG
void UART_PrintCrash(uint32 Exception, uint32 Registers_p, uint32 SPSR, uint32 Link_Reg, uint32 CPSR)
{
    char ASCII[8];

    DirectPrint("!!!!! CRASCH !!!!!\n", 19);

    /*DirectPrint("\nException: 0x",14);
    Convert32(Exception, ASCII);
    DirectPrint(ASCII,8);*/

    switch (Exception) {
    case 1: {
        DirectPrint("Undefined", 9);
        break;
    }
    case 2: {
        DirectPrint("Prefetch", 8);
        break;
    }
    case 3: {
        DirectPrint("Abort", 5);
        break;
    }
    default: {
        DirectPrint("????", 4);
    }
    }

    DirectPrint("\nsp: 0x", 7);
    Convert32(Registers_p, ASCII);
    DirectPrint(ASCII, 8);

    DirectPrint("\nSPSR: 0x", 9);
    Convert32(SPSR, ASCII);
    DirectPrint(ASCII, 8);

    DirectPrint("\nlr: 0x", 7);
    Convert32(Link_Reg, ASCII);
    DirectPrint(ASCII, 8);

    DirectPrint("\nCPSR: 0x", 9);
    Convert32(CPSR, ASCII);
    DirectPrint(ASCII, 8);

    DirectPrint("\n!!!!END!!!!!\n", 14);

    while (1) {
        ;
    }
}
#endif

#ifdef ENABLE_EARLY_BREAKPOINT
/*
 * The function if for debug purposes only
 * It is used to enable 'early' debugging when executing main()
 *
 * USAGE: The function should be declared as external in the main module
 * to avoid compiler optimization which can ignore the function
 *
 * */
void StopExecution(void)
{
    while (1) {
        ;
    }
}
#endif

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/* This is for the crash handler  */
#ifdef ENABLE_DEBUG
static void Convert32(uint32 Value, char *Buffer)
{
    uint8 Temp = 0;
    uint32 i;

    for (i = 0; i < 8; i++) {
        Temp = 0xf & (Value >> (4 * i));

        if (Temp < 0xA) {
            Buffer[7 - i] = Temp + '0';
        } else {
            Buffer[7 - i] = Temp + 'A' - 0xA;
        }
    }
}


#endif

static int circular_printf(const char *format, ...)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    char *EntryBuffer_p = NULL;
    char *TrashEntry_p = NULL;
    int StringSize = 0;
    va_list args = {NULL};
    va_start(args, format);
    CPU_Irq_State_t OldIrQState = CPU_IRQ_DISABLE;

    if (FALSE == IsDebugQueueAvaible) {
        goto ErrorExit;
    }

    StringSize = get_sizef(format, args);

    if (StringSize < 0) {
        goto ErrorExit;
    }

    EntryBuffer_p = (char *)malloc(StringSize + sizeof(char));
    ASSERT(NULL != EntryBuffer_p);

    OldIrQState = CPU_IrqDisable();
    StringSize = vsnprintf(EntryBuffer_p, StringSize + sizeof(char), format, args);

    if (StringSize < 0) {
        BUFFER_FREE(EntryBuffer_p);
        goto ErrorExit;
    }

    if (CPU_IRQ_ENABLE == OldIrQState) {
        CPU_IrqEnable();
    }

    ReturnValue = Do_RFifo_Enqueue(NULL, DebugQueue_p, EntryBuffer_p);

    if (E_FAILED_TO_STORE_IN_FIFO == ReturnValue) {
        OldIrQState = CPU_IrqDisable();
        TrashEntry_p = Do_Fifo_Dequeue(NULL, DebugQueue_p);
        BUFFER_FREE(TrashEntry_p);

        ReturnValue = Do_Fifo_Enqueue(NULL, DebugQueue_p, EntryBuffer_p);

        if (E_SUCCESS != ReturnValue) {
            BUFFER_FREE(EntryBuffer_p);
            goto ErrorExit;
        }
    }

    if (CPU_IRQ_ENABLE == OldIrQState) {
        CPU_IrqEnable();
    }

ErrorExit:

    if (CPU_IRQ_ENABLE == OldIrQState) {
        CPU_IrqEnable();
    }

    va_end(args);

    /*
     * EntryBuffer_p is freed when the debug buffer is read
     * from PC side by calling the CollectData command
     *
     */

    /* coverity[leaked_storage] */
    return StringSize;
}

static int uart_printf(const char *format, ...)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    char *EntryBuffer_p = NULL;
    int StringSize = 0;
    CommunicationDevice_t *DebugDevice_p = NULL;

    va_list args = {NULL};
    va_start(args, format);
    CPU_Irq_State_t OldIrQState = CPU_IRQ_DISABLE;


    if (E_SUCCESS != Do_Communication_GetCommunicationDevice(&DebugCommunication, &DebugDevice_p)) {
        goto ErrorExit;
    }

    StringSize = get_sizef(format, args);

    if (StringSize < 0) {
        goto ErrorExit;
    }

    EntryBuffer_p = (char *)malloc(StringSize + sizeof(char));
    ASSERT(NULL != EntryBuffer_p);

    OldIrQState = CPU_IrqDisable();
    StringSize = vsnprintf(EntryBuffer_p, StringSize + sizeof(char), format, args);

    if (StringSize < 0) {
        goto ErrorExit;
    }

    if (CPU_IRQ_ENABLE == OldIrQState) {
        CPU_IrqEnable();
    }

    do {
        ReturnValue = DebugDevice_p->Write(EntryBuffer_p, StringSize, NULL, DebugDevice_p);
    } while (ReturnValue == E_COM_DEVICE_BUSY);

ErrorExit:
    BUFFER_FREE(EntryBuffer_p);

    if (CPU_IRQ_ENABLE == OldIrQState) {
        CPU_IrqEnable();
    }

    va_end(args);

    //lint -e429
    return StringSize;
}
//lint +e429

static void InitializeDebugQueue(void)
{
    Do_RFifo_Create(NULL, &DebugQueue_p, MAX_NUMBER_OF_MESSAGES + 1, DestroyElement);

    (void)Do_Fifo_SetCallback(NULL, DebugQueue_p, QUEUE_EMPTY, NULL, NULL);
    (void)Do_Fifo_SetCallback(NULL, DebugQueue_p, QUEUE_NONEMPTY, NULL, NULL);
}

static void DestroyElement(void *Value_p)
{
    BUFFER_FREE(Value_p);
}

static int get_sizef(const char *format, const va_list args)
{
    int Size = -1;
    char *EntryBuffer_p = NULL;
    int Count = 1;
    int InitialDrift = 0;

    InitialDrift = 2 * strlen(format);

    while (Size == -1) {
        int AllocSize = InitialDrift + Count * sizeof(char);
        EntryBuffer_p = (char *)realloc(EntryBuffer_p, AllocSize);

        if (!EntryBuffer_p) {
            return -1;
        } else {
            Size = vsnprintf(EntryBuffer_p, AllocSize, format, args);
        }

        Count *= 2;
    }

    BUFFER_FREE(EntryBuffer_p);
    return Size;
}
#endif /*CFG_ENABLE_LOADER_TYPE*/
/* @} */
/* @} */
