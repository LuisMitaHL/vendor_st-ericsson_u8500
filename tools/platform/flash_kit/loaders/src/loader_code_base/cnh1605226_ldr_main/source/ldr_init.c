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
#include "ldr_init.h"
#include <stdlib.h>

#include "r_communication_service.h"
#include "r_communication_buffer.h"
#include "t_r15_family.h"
#include "r_time_utilities.h"
#include "r_queue.h"
#include "r_security_algorithms.h"
#include "r_communication_abstraction.h"
#include "commands.h"

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
Communication_t DebugCommunication;

/*******************************************************************************
 * Delaration of internal functions
 ******************************************************************************/
static ErrorCode_e SetupBuffersInterface(BuffersInterface_t *BufferInterface_p);
static ErrorCode_e SetupTimersInterface(TimersInterface_t *TimerInterface_p);
static ErrorCode_e SetupQueuesInterface(QueueInterface_t *QueueInterface_p);
static ErrorCode_e ConfigureDeviceTimeouts(CommDevId_t CommDevice);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e Do_Loader_CommunicationProtocolInit(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    HashDevice_t *HashDevice_p = NULL;
    BuffersInterface_t BufferFunctions = {0};
    TimersInterface_t TimerFunctions = {0};
    QueueInterface_t QueueFunctions = {0};
    CommunicationDevice_t *CommDevice_p = NULL;
    CommDevId_t CommDevice = DEV_UNKNOWN;

    ReturnValue = Do_Buffer_BuffersInit(NULL);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    HashDevice_p = Do_Hash_DeviceInit(NULL);
    VERIFY_NO_DEBUG(HashDevice_p != NULL, E_GENERAL_FATAL_ERROR);

#ifdef ENABLE_DEBUG
    Do_CommDeviceDebugoutInit(&DebugCommunication);
    //  if (DebugCommunication.CommunicationDevice_p == NULL)
    //  {
    //    TODO: If is not initialized debug channel then retarget all printouts to the print server.
    //  }
#endif

    CommDevice_p = Do_CommDeviceInit();
    VERIFY_NO_DEBUG(CommDevice_p != NULL, E_GENERAL_FATAL_ERROR);

    /* Configure interface functions for using buffers */
    ReturnValue = SetupBuffersInterface(&BufferFunctions);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    /* Configure interface functions for using timers */
    ReturnValue = SetupTimersInterface(&TimerFunctions);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    /* Configure interface functions for using queues */
    ReturnValue = SetupQueuesInterface(&QueueFunctions);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    /* Initialize LCM */
    ReturnValue = Do_Communication_Initialize(NULL, NULL, R15_FAMILY, HashDevice_p, CommDevice_p, Do_CEH_Call, &BufferFunctions, &TimerFunctions, &QueueFunctions);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    /* Get Device ID */
    CommDevice = ((CommDev_t *)CommDevice_p->Context_p)->Descriptor_p->DevType;
    VERIFY_NO_DEBUG(DEV_UNKNOWN != CommDevice, E_GENERAL_COMMUNICATION_ERROR);

    /* Configure Device Timeout Values */
    ReturnValue = ConfigureDeviceTimeouts(CommDevice);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;
    //lint +e429
ErrorExit:
    BUFFER_FREE(HashDevice_p);

    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e SetupBuffersInterface(BuffersInterface_t *BufferInterface_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    BufferInterface_p->BuffersInit_Fn = Do_Buffer_BuffersInit;
    BufferInterface_p->BufferAllocate_Fn = Do_Buffer_BufferAllocate;
    BufferInterface_p->BufferRelease_Fn = Do_Buffer_BufferRelease;
    BufferInterface_p->BuffersAvailable_Fn = Do_Buffer_BuffersAvailable;
    BufferInterface_p->BuffersDeinit_Fn = Do_Buffer_BuffersDeinit;

    return ReturnValue;
}

static ErrorCode_e SetupTimersInterface(TimersInterface_t *TimerInterface_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    TimerInterface_p->TimersInit_Fn = Do_Timer_TimersInit;
    TimerInterface_p->TimerGet_Fn = Do_Timer_TimerGet;
    TimerInterface_p->TimerRelease_Fn = Do_Timer_TimerRelease;
    TimerInterface_p->ReadTime_Fn = Do_Timer_ReadTime;
    TimerInterface_p->GetSystemTime_Fn = Do_Timer_GetSystemTime;

    return ReturnValue;
}

static ErrorCode_e SetupQueuesInterface(QueueInterface_t *QueueInterface_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    QueueInterface_p->FifoCreate_Fn = Do_Fifo_Create;
    QueueInterface_p->FifoDestroy_Fn = Do_Fifo_Destroy;
    QueueInterface_p->FifoEnqueue_Fn = Do_Fifo_Enqueue;
    QueueInterface_p->FifoDequeue_Fn = Do_Fifo_Dequeue;
    QueueInterface_p->Fifo_SetCallback_Fn = Do_Fifo_SetCallback;
    QueueInterface_p->Fifo_IsEmpty_Fn = Do_Fifo_IsEmpty;
    QueueInterface_p->Fifo_IsMember_Fn = Do_Fifo_IsMember;
    QueueInterface_p->Fifo_GetNrOfElements_Fn = Do_Fifo_GetNrOfElements;

    QueueInterface_p->RFifoCreate_Fn = Do_RFifo_Create;
    QueueInterface_p->RFifoDestroy_Fn = Do_RFifo_Destroy;
    QueueInterface_p->RFifoEnqueue_Fn = Do_RFifo_Enqueue;
    QueueInterface_p->RFifoDequeue_Fn = Do_RFifo_Dequeue;
    QueueInterface_p->RFifo_SetCallback_Fn = Do_RFifo_SetCallback;
    QueueInterface_p->RFifo_IsEmpty_Fn = Do_RFifo_IsEmpty;
    QueueInterface_p->RFifo_IsMember_Fn = Do_RFifo_IsMember;
    QueueInterface_p->RFifo_GetNrOfElements_Fn = Do_RFifo_GetNrOfElements;

    return ReturnValue;
}

/* Configure device timeouts.
 *
 * Configure the LCM timeouts for the given device based on the device type.
 *
 * @param [in] CommDevice - The ID of the communication device used in LCM.
 *
 * @retval  E_INVALID_INPUT_PARAMETERS - if unknown device is given as input,
 *                                       otherwise the value returned by the
 *                                       LCM Timeout configuration function.
 */
static ErrorCode_e ConfigureDeviceTimeouts(CommDevId_t CommDevice)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    R15_Timeouts_t LcmTimeouts = {0};

    switch (CommDevice) {
    case DEV_UART0:
    case DEV_UART1:
    case DEV_UART2:
        LcmTimeouts.TCACK = 6000;
        LcmTimeouts.TBCR  = 6000;
        LcmTimeouts.TBDR  = 120000;
        ReturnValue = Do_Communication_SetProtocolTimeouts(GlobalCommunication_p, (void *)(&LcmTimeouts));
        break;
    case DEV_USB:
        LcmTimeouts.TCACK = 500;
        LcmTimeouts.TBCR  = 500;
        LcmTimeouts.TBDR  = 5000;
        ReturnValue = Do_Communication_SetProtocolTimeouts(GlobalCommunication_p, (void *)(&LcmTimeouts));
        break;
    default:
        break;
    }

    return ReturnValue;
}

/** @} */
