/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
/**
 * @addtogroup ldr_communication_serv
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "error_codes.h"
#include <stdlib.h>
#include <stdio.h>
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_r15_family.h"
#include "r_r15_transport_layer.h"
#include "r_r15_network_layer.h"
#include "r_communication_service.h"
#include "r_basicdefinitions.h"
#include "r_memory_utils.h"

#ifdef CFG_ENABLE_A2_FAMILY
#include "r_a2_family.h"
#include "r_a2_transport.h"
#endif
#ifndef CFG_ENABLE_LOADER_TYPE
#include "r_protrom_family.h"
#include "r_z_family.h"
#include "r_protrom_transport.h"
#include "r_z_transport.h"
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define QUEUE_SIZE 32

Communication_t GlobalCommunication = {0};
Communication_t *GlobalCommunication_p = &GlobalCommunication;

#if !(defined(__MINGW32__) || defined(__linux__) || defined(__APPLE__))
#ifdef CFG_ENABLE_LOADER_TYPE
#ifndef MACH_TYPE_STN8500
char LCM_CurrentVersion[] = ""; // Generate Empty String for Loader Builds on ClearCase
#endif // MACH_TYPE_STN8500
#else
char LCM_CurrentVersion[] = "TestBuild"; // Generate Test String for unofficial builds in Windows environment
#endif // CFG_ENABLE_LOADER_TYPE
#endif // __MINGW32__ || __linux__ || __APPLE__

static FamilyDescriptor_t ProtocolFamilies[] = {
    {
        R15_Family_Init,
        R15_Family_Shutdown,
        R15_Transport_Poll,
        R15_Transport_Send,
        R15_SetProtocolTimeouts,
        R15_GetProtocolTimeouts,
        R15_CancelReceiver
    },
    {
#ifdef CFG_ENABLE_A2_FAMILY
        A2_Family_Init,
        A2_Family_Shutdown,
        A2_Transport_Poll,
        A2_Transport_Send,
        NULL,
        NULL,
        A2_CancelReceiver
#else
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
#endif
    },
#ifndef CFG_ENABLE_LOADER_TYPE
    {
        // we do not support TI protocol family
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        Protrom_Family_Init,
        Protrom_Family_Shutdown,
        Protrom_Transport_Poll,
        Protrom_Transport_Send,
        NULL,
        NULL,
        Protrom_CancelReceiver
    },
    {
        Z_Family_Init,
        Z_Family_Shutdown,
        Z_Transport_Poll,
        Z_Transport_Send,
        NULL,
        NULL,
        Z_CancelReceiver
    }
#endif
};

#define MAX_FAMILY (sizeof(ProtocolFamilies) / sizeof(ProtocolFamilies[0]))

/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/

static BuffersInterface_t *CreateBufferInterfaceHelpper(BuffersInterface_t *Buffers_p);
static TimersInterface_t  *CreateTimerInterfaceHelpper(TimersInterface_t *Timers_p);
static QueueInterface_t   *CreateQueueInterfaceHelpper(QueueInterface_t *Queue_p);

static void DestroyBufferInterface(const Communication_t *const Communication_p);
static void DestroyTimerInterface(const Communication_t *const Communication_p);
static void DestroyQueueInterface(const Communication_t *const Communication_p);
FamilyDescriptor_t *GetFamily(Family_t Family);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/**
 * Initialize an instance of the communication module.
 *
 * Given the initial family, already initialized hash device descriptor
 * and communication device descriptor, a new instance of the
 * communication module is created and bound to use the provided devices.
 *
 * @param [in]     Object_p              Instance which will use initialized communication.
 * @param [in,out] Communication_pp      Communication module context.
 * @param [in]     Family                Initial protocol family.
 * @param [in]     HashDevice_p          The device to use for checksum
 *                                       calculations and verifications.
 * @param [in]     CommunicationDevice_p The device to use for network
 *                                       transmission.
 * @param [in]     CommandCallback_p     Callback function for command handling.
 * @param [in]     Buffers_p             Pointer to buffer handling functions.
 * @param [in]     Timers_p              Pointer to timers handling functions.
 * @param [in]     Queue_p               Pointer to queue handling functions.
 *
 * @retval  E_SUCCESS If the module instance is successfully
 *                    initialized. A protocol family dependent error
 *                    code otherwise.
 */
ErrorCode_e Do_Communication_Initialize(void *Object_p, Communication_t **Communication_pp, Family_t Family, HashDevice_t *HashDevice_p, CommunicationDevice_t *CommunicationDevice_p, Do_CEH_Call_t CommandCallback_p, BuffersInterface_t *Buffers_p, TimersInterface_t *Timers_p, QueueInterface_t *Queue_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    Communication_t *Communication_p = NULL;

    VERIFY((NULL != HashDevice_p) && (NULL != CommunicationDevice_p), E_INVALID_INPUT_PARAMETERS);

#ifdef CFG_ENABLE_COMMUNICATION_SINGLETON
    Communication_p = GlobalCommunication_p;
#else
    VERIFY(NULL != Communication_pp, E_INVALID_INPUT_PARAMETERS);

    Communication_p = (Communication_t *)malloc(sizeof(Communication_t));
    VERIFY(NULL != Communication_p, E_ALLOCATE_FAILED);
    memset(Communication_p, 0, sizeof(Communication_t));

    Communication_p->Object_p = Object_p;
    *Communication_pp = Communication_p;
#endif

    Communication_p->Functions_p = (FunctionInterface_t *)malloc(sizeof(FunctionInterface_t));

    VERIFY(NULL != Communication_p->Functions_p, E_ALLOCATE_FAILED);

    /* init buffer functions*/
    Communication_p->Functions_p->BufferFunctions_p = CreateBufferInterfaceHelpper(Buffers_p);

    VERIFY(NULL != Communication_p->Functions_p->BufferFunctions_p, E_FAILED_TO_ALLOCATE_COMM_BUFFER);

    /* init Timer functions*/
    Communication_p->Functions_p->TimerFunctions_p = CreateTimerInterfaceHelpper(Timers_p);

    VERIFY(NULL != Communication_p->Functions_p->TimerFunctions_p, E_FAILED_TO_INTIALIZE_TIMER_FUNCTIONS);

    /* init Queue functions*/
    Communication_p->Functions_p->QueueFunctions_p = CreateQueueInterfaceHelpper(Queue_p);

    VERIFY(NULL != Communication_p->Functions_p->QueueFunctions_p, E_FAILED_TO_INTIALIZE_QUEUE_FUNCTIONS);

    /* TODO: FIFO init can fail, but there's currently no way of detecting it */
    QUEUE(Communication_p, FifoCreate_Fn)(OBJECT_QUEUE(Communication_p), &Communication_p->Inbound_p, QUEUE_SIZE, NULL);
    QUEUE(Communication_p, FifoCreate_Fn)(OBJECT_QUEUE(Communication_p), &Communication_p->Outbound_p, QUEUE_SIZE, NULL);

    Communication_p->CommunicationDevice_p = CommunicationDevice_p;
    Communication_p->HashDevice_p = HashDevice_p;
    Communication_p->HashDevice_p->Object_p = HashDevice_p->Object_p;
    Communication_p->CurrentFamilyHash = HASH_NONE;
    Communication_p->BackupCommBufferSize = 0;

    CommunicationDevice_p->Object_p = Communication_p;

    ReturnValue = Do_Communication_SetFamily(Communication_p, Family, CommandCallback_p);

    C_(printf("#---------------------------------------------------------\n");)
    C_(printf("# Loader Communication Module Started!\n");)
    C_(printf("#---------------------------------------------------------\n");)

ErrorExit:
    return ReturnValue;
}

/*
 * Shutdown an instance of the communication module.
 *
 * Shutdown the initialized protocol family, cancel current transmission and
 * receiving data thru the communication device and shutdown the transport and
 * network layer.
 *
 * @param [in,out] Communication_pp    Communication module context.
 *
 * @retval  E_SUCCESS If the module instance is successfully
 *                    shut down. A protocol family dependent error
 *                    code otherwise.
 */
ErrorCode_e Do_Communication_Shutdown(Communication_t **Communication_pp)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    Communication_t *Communication_p = NULL;

    VERIFY((NULL != Communication_pp) && (NULL != *Communication_pp), E_INVALID_INPUT_PARAMETERS);

    Communication_p = *Communication_pp;

    (void)Communication_p->CommunicationDevice_p->Cancel(Communication_p->CommunicationDevice_p);
    Communication_p->HashDevice_p->Cancel(OBJECT_HASH(Communication_p), (void **)&Communication_p->HashDevice_p);

    if (NULL != Communication_p->Family_p) {
        ReturnValue = Communication_p->Family_p->FamilyShutdown_fn(Communication_p);
    }

    QUEUE(Communication_p, FifoDestroy_Fn)(OBJECT_QUEUE(Communication_p), &Communication_p->Inbound_p);
    QUEUE(Communication_p, FifoDestroy_Fn)(OBJECT_QUEUE(Communication_p), &Communication_p->Outbound_p);

    DestroyBufferInterface(Communication_p);
    DestroyTimerInterface(Communication_p);
    DestroyQueueInterface(Communication_p);

    if (NULL != Communication_p->Functions_p) {
        free(Communication_p->Functions_p);
        Communication_p->Functions_p = NULL;
    }

    memset(Communication_p, 0, sizeof(Communication_t));

    if (NULL != Communication_p->BackupCommBuffer_p) {
        free(Communication_p->BackupCommBuffer_p);
        Communication_p->BackupCommBuffer_p = NULL;
        Communication_p->BackupCommBufferSize = 0;
    }

    free(Communication_p);
    Communication_p = NULL;
    *Communication_pp = NULL;

ErrorExit:
    return ReturnValue;
}

/*
 * Change the current protocol family.
 *
 * Shuts down the old protocol family and initializes the new one.
 *
 * @param [in] Communication_p  Communication module context.
 * @param [in] Family           New protocol family identifier.
 * @param [in] CEHCallback      Callback that will handle the commands.
 *
 * @retval  E_SUCCESS If the family is successfully changed. A
 *                    protocol family dependent error code otherwise.
 */
ErrorCode_e Do_Communication_SetFamily(Communication_t *Communication_p, Family_t Family, Do_CEH_Call_t CEHCallback)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    FamilyDescriptor_t *Family_p = GetFamily(Family);

    VERIFY(NULL != Family_p, E_INVALID_INPUT_PARAMETERS);

    if (NULL != Communication_p->Family_p) {
        (void)Communication_p->CommunicationDevice_p->Cancel(Communication_p->CommunicationDevice_p);
        ReturnValue = Communication_p->Family_p->FamilyShutdown_fn(Communication_p);

        VERIFY_CONDITION(E_SUCCESS == ReturnValue);
    }

    Communication_p->Family_p = Family_p;
    Communication_p->Do_CEH_Call_Fn = CEHCallback;

    ReturnValue = Communication_p->Family_p->FamilyInit_fn(Communication_p);
ErrorExit:
    return ReturnValue;
}


/*
 * Poll the network for inbound packets.
 *
 * When a packet is received it is put on the inbound queue. This
 * function should be called whenever there is time to handle a new
 * packet. It passes all the elements in the queue to the family
 * specific polling function, one by one.
 *
 * @param [in] Communication_p  Communication module context.
 *
 *
 * @retval  E_SUCCESS If all packets are successfully handled. A
 *                    protocol family dependent error code otherwise.
 */
ErrorCode_e Do_Communication_Poll(Communication_t *Communication_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

    VERIFY_CONDITION(NULL != Communication_p->Family_p);

    ReturnValue = Communication_p->Family_p->Process_fn(Communication_p);

ErrorExit:
    return ReturnValue;
}

/*
 * Function for sending packet.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  InputData_p     Pointer to the data for transmission.
 *
 * @retval E_SUCCESS                        After successful execution.
 * @retval E_FAILED_TO_ALLOCATE_COMM_BUFFER Failed to allocate communication
 *                                          buffer.
 */
ErrorCode_e Do_Communication_Send(Communication_t *Communication_p, void *InputData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

    VERIFY_CONDITION(NULL != Communication_p->Family_p);

    (void)Communication_p->Family_p->Send_fn(Communication_p, InputData_p);

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Function for setting the timeouts in the protocols.
 *
 * Timeouts that should be set depend of the protocol family. Some protocol
 * family do not have timeouts.
 *
 * @param [in]  Communication_p Communication module context.
 * @param [in]  TimeoutData_p   Pointer to the data with all timeouts.
 *
 * @retval E_SUCCESS            After successful execution.
 */
ErrorCode_e Do_Communication_SetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

    VERIFY_CONDITION(NULL != Communication_p->Family_p);

    (void)Communication_p->Family_p->SetProtocolTimeouts_fn(Communication_p, TimeoutData_p);

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Function for getting the timeouts in the protocols.
 *
 * Timeouts that should be get depend of the protocol family. Some protocol
 * family do not have timeouts.
 *
 * @param [in]  Communication_p  Communication module context.
 * @param [out] TimeoutData_p    Return data with all timeouts.
 *
 * @retval E_SUCCESS             After successful execution.
 */
ErrorCode_e Do_Communication_GetProtocolTimeouts(Communication_t *Communication_p, void *TimeoutData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

    VERIFY_CONDITION(NULL != Communication_p->Family_p);

    (void)Communication_p->Family_p->GetProtocolTimeouts_fn(Communication_p, TimeoutData_p);

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Function for getting the communication device for the given LCM instance.
 *
 * If communication is Singleton the function can be called with NULL pointer
 * for Communication_p.
 *
 * @param [in]  Communication_p        Communication module context.
 * @param [out] CommunicationDevice_pp Pointer to the initialized communication
 *                                     device for the given LCM context.
 *
 * @retval E_SUCCESS                 After successful execution.
 * @retval E_INVALID_INPUT_PARAMTERS In case when communication is not Singleton
 *                                   and Communication_p is NULL pointer.
 */
ErrorCode_e Do_Communication_GetCommunicationDevice(Communication_t *Communication_p, CommunicationDevice_t **CommunicationDevice_pp)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    VERIFY(NULL != Communication_p && CommunicationDevice_pp != NULL, E_INVALID_INPUT_PARAMETERS);

    *CommunicationDevice_pp = Communication_p->CommunicationDevice_p;

ErrorExit:
    return ReturnValue;
}

/*
 * Function for setting the communication device for the given LCM instance.
 *
 * If communication is Singleton the function can be called with NULL pointer
 * for Communication_p.
 *
 * @param [in]  Communication_p        Communication module context.
 * @param [out] CommunicationDevice_p  Pointer to the initialized communication
 *                                     device to be set for the given LCM context.
 *
 * @retval E_SUCCESS                 After successful execution.
 * @retval E_INVALID_INPUT_PARAMTERS In case when communication is not Singleton
 *                                   and Communication_p is NULL pointer.
 */
ErrorCode_e Do_Communication_SetCommunicationDevice(Communication_t *Communication_p, CommunicationDevice_t *CommunicationDevice_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    VERIFY(NULL != Communication_p && CommunicationDevice_p != NULL, E_INVALID_INPUT_PARAMETERS);

    Communication_p->CommunicationDevice_p = CommunicationDevice_p;

ErrorExit:
    return ReturnValue;
}

/*
 * Cancel Receiving new packets
 *
 * @param [in] Communication_p  Communication module context.
 * @param [in] PacketsBeforeReceiverStop  Number of packets to be send before stopping the receiver.
 *
 * @retval  E_SUCCESS If all packets are successfully handled.
 *                    A protocol family dependent error code otherwise.
 */
ErrorCode_e Do_Communication_Cancel_Receiver(Communication_t *Communication_p, uint8 PacketsBeforeReceiverStop)
{
    ErrorCode_e ReturnValue = E_GENERAL_COMMUNICATION_ERROR;

    VERIFY_CONDITION(NULL != Communication_p->Family_p);

    ReturnValue = Communication_p->Family_p->CancelReceiver_fn(Communication_p, PacketsBeforeReceiverStop);

ErrorExit:
    return ReturnValue;
}

/*
 * Function used to get the LCM version.
 *
 * LCM version string for the current version is placed in lcm_version.c file
 *
 * @retval char LCM_CurrentVersion[]             After successful execution.
 */
char *Do_Communication_GetVersion()
{
    return LCM_CurrentVersion;
}

void Do_CommunicationInternalErrorHandler(const ErrorCode_e IntError)
{
#ifndef PRINT_A_
    IDENTIFIER_NOT_USED(IntError);
#endif
    A_(printf("# Error Code: 0x%X !\n", IntError);)
    A_(printf("# Loader stopped!\n");)
    A_(printf("#---------------------------------------------------------\n");)

    /* coverity[no_escape] */
    while (1);
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static BuffersInterface_t *CreateBufferInterfaceHelpper(BuffersInterface_t *Buffers_p)
{
    BuffersInterface_t *BufferFunctions_p = (BuffersInterface_t *)malloc(sizeof(BuffersInterface_t));

    if (NULL == BufferFunctions_p) {
        return NULL;
    }

    BufferFunctions_p->BuffersInit_Fn = Buffers_p->BuffersInit_Fn;
    BufferFunctions_p->BufferAllocate_Fn = Buffers_p->BufferAllocate_Fn;
    BufferFunctions_p->BufferRelease_Fn = Buffers_p->BufferRelease_Fn;
    BufferFunctions_p->BuffersAvailable_Fn = Buffers_p->BuffersAvailable_Fn;
    BufferFunctions_p->BuffersDeinit_Fn = Buffers_p->BuffersDeinit_Fn;
#ifndef CFG_ENABLE_LOADER_TYPE
    BufferFunctions_p->Object_p = Buffers_p->Object_p;
#endif

    return BufferFunctions_p;
}

static TimersInterface_t *CreateTimerInterfaceHelpper(TimersInterface_t *Timers_p)
{
    TimersInterface_t *TimerFunctions_p = (TimersInterface_t *)malloc(sizeof(TimersInterface_t));

    if (NULL == TimerFunctions_p) {
        return NULL;
    }

    TimerFunctions_p->TimersInit_Fn = Timers_p->TimersInit_Fn;
    TimerFunctions_p->TimerGet_Fn = Timers_p->TimerGet_Fn;
    TimerFunctions_p->TimerRelease_Fn = Timers_p->TimerRelease_Fn;
    TimerFunctions_p->ReadTime_Fn = Timers_p->ReadTime_Fn;
    TimerFunctions_p->GetSystemTime_Fn = Timers_p->GetSystemTime_Fn;
#ifndef CFG_ENABLE_LOADER_TYPE
    TimerFunctions_p->Object_p = Timers_p->Object_p;
#endif

    return TimerFunctions_p;
}

static QueueInterface_t *CreateQueueInterfaceHelpper(QueueInterface_t *Queue_p)
{
    QueueInterface_t *QueueFunctions_p = (QueueInterface_t *)malloc(sizeof(QueueInterface_t));

    if (NULL == QueueFunctions_p) {
        return NULL;
    }

    QueueFunctions_p->FifoCreate_Fn = Queue_p->FifoCreate_Fn;
    QueueFunctions_p->FifoDestroy_Fn = Queue_p->FifoDestroy_Fn;
    QueueFunctions_p->FifoEnqueue_Fn = Queue_p->FifoEnqueue_Fn;
    QueueFunctions_p->FifoDequeue_Fn = Queue_p->FifoDequeue_Fn;
    QueueFunctions_p->Fifo_SetCallback_Fn = Queue_p->Fifo_SetCallback_Fn;
    QueueFunctions_p->Fifo_IsEmpty_Fn = Queue_p->Fifo_IsEmpty_Fn;
    QueueFunctions_p->Fifo_IsMember_Fn = Queue_p->Fifo_IsMember_Fn;
    QueueFunctions_p->Fifo_GetNrOfElements_Fn = Queue_p->Fifo_GetNrOfElements_Fn;

    QueueFunctions_p->RFifoCreate_Fn = Queue_p->RFifoCreate_Fn;
    QueueFunctions_p->RFifoDestroy_Fn = Queue_p->RFifoDestroy_Fn;
    QueueFunctions_p->RFifoEnqueue_Fn = Queue_p->RFifoEnqueue_Fn;
    QueueFunctions_p->RFifoDequeue_Fn = Queue_p->RFifoDequeue_Fn;
    QueueFunctions_p->RFifo_SetCallback_Fn = Queue_p->RFifo_SetCallback_Fn;
    QueueFunctions_p->RFifo_IsEmpty_Fn = Queue_p->RFifo_IsEmpty_Fn;
    QueueFunctions_p->RFifo_IsMember_Fn = Queue_p->RFifo_IsMember_Fn;
    QueueFunctions_p->RFifo_GetNrOfElements_Fn = Queue_p->RFifo_GetNrOfElements_Fn;

#ifndef CFG_ENABLE_LOADER_TYPE
    QueueFunctions_p->Object_p = Queue_p->Object_p;
#endif
    return QueueFunctions_p;
}

static void  DestroyBufferInterface(const Communication_t *const Communication_p)
{
    if (NULL != Communication_p->Functions_p->BufferFunctions_p) {
        free(Communication_p->Functions_p->BufferFunctions_p);
        Communication_p->Functions_p->BufferFunctions_p = NULL;
    }
}

static void DestroyTimerInterface(const Communication_t *const Communication_p)
{
    if (NULL != Communication_p->Functions_p->TimerFunctions_p) {
        free(Communication_p->Functions_p->TimerFunctions_p);
        Communication_p->Functions_p->TimerFunctions_p = NULL;
    }
}

static void DestroyQueueInterface(const Communication_t *const Communication_p)
{
    if (NULL != Communication_p->Functions_p->QueueFunctions_p) {
        free(Communication_p->Functions_p->QueueFunctions_p);
        Communication_p->Functions_p->QueueFunctions_p = NULL;
    }
}

FamilyDescriptor_t *GetFamily(Family_t Family)
{
    uint32 IntFamily = (uint32)Family;

    if (IntFamily > (MAX_FAMILY - 1)) {
        return NULL;
    }

    return (FamilyDescriptor_t *)&ProtocolFamilies[Family];
}

/** @}*/
