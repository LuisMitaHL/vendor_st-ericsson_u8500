/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * \addtogroup ldr_communication_buffer
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "c_system.h"
#include "t_basicdefinitions.h"
#include "r_communication_buffer.h"
#include "error_codes.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_queue.h"
#include "t_r15_network_layer.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

#define MAX_BUFFER_SIZE BULK_BUFFER_SIZE

typedef struct {
    int Size;
    int NrOfBuffers;
} BufferInfo_t;

static void *FreeQueue_p[NUMBER_OF_BUFFER_TYPES] = {NULL, NULL};

/* Buffer types must be in acceding order by size */
static BufferInfo_t BuffersInfo[NUMBER_OF_BUFFER_TYPES] = {
    {COMMAND_BUFFER_SIZE, COMMAND_BUFFER_COUNT},
    {BULK_BUFFER_SIZE, BULK_BUFFER_COUNT}
};

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static int GetBufferType(uint32 BufferSize);
/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

static int GetBufferType(uint32 BufferSize)
{
    int BufferType = 0;

    do {
        if ((int)BufferSize <= BuffersInfo[BufferType].Size) {
            break;
        }

        BufferType++;
    } while (BufferType < NUMBER_OF_BUFFER_TYPES);

    return BufferType;
}
/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ErrorCode_e Do_Buffer_BuffersInit(void *Object_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    //lint -e{429}
    static void *Buffer_p = NULL;
    int i = 0;
    int j = 0;
    int Offset = 0;

    extern unsigned int Image$$STATIC_BUFFERS$$Base;
    uint8 *Buffer_Base_p = (uint8 *)&Image$$STATIC_BUFFERS$$Base;

    for (i = 0; i < NUMBER_OF_BUFFER_TYPES; i++) {
        Do_RFifo_Create(Object_p, &FreeQueue_p[i], BuffersInfo[i].NrOfBuffers + 1, NULL);
    }

    for (i = 0; i < NUMBER_OF_BUFFER_TYPES; i++) {
        for (j = 0; j < BuffersInfo[i].NrOfBuffers; j++) {
            Buffer_p = (void *)(Buffer_Base_p + Offset);
            ASSERT(NULL != Buffer_p);
            Offset += BuffersInfo[i].Size;

            ReturnValue = Do_RFifo_Enqueue(Object_p, FreeQueue_p[i], Buffer_p);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        }
    }

ErrorExit:
    return ReturnValue;
}

void *Do_Buffer_BufferAllocate(void *Object_p, int BufferSize)
{
    void *Buffer_p = NULL;
    int BufferType = GetBufferType(BufferSize);

    if (BufferType < NUMBER_OF_BUFFER_TYPES) {
        if (Do_RFifo_IsEmpty(Object_p, FreeQueue_p[BufferType])) {
            return NULL;
        }

        Buffer_p = Do_RFifo_Dequeue(Object_p, FreeQueue_p[BufferType]);
    }

    return Buffer_p;
}


ErrorCode_e Do_Buffer_BufferRelease(void *Object_p, void *Buffer_p, int BufferSize)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    int BufferType = 0;

    VERIFY(NULL != Buffer_p, E_INVALID_INPUT_PARAMETERS);

    VERIFY(BufferSize <= MAX_BUFFER_SIZE, E_INVALID_TYPE_OF_BUFFER);

    BufferType = GetBufferType(BufferSize);   //new
    VERIFY(BufferType < NUMBER_OF_BUFFER_TYPES, E_INVALID_TYPE_OF_BUFFER);

    ReturnValue = Do_RFifo_Enqueue(Object_p, FreeQueue_p[BufferType], Buffer_p);

ErrorExit:
    return  ReturnValue;
}


uint32 Do_Buffer_BuffersAvailable(void *Object_p, int BufferSize)
{
    int BufferType = 0;

    /* sanity check */
    if (BufferSize > MAX_BUFFER_SIZE) {
        A_(printf("communication_buffer.c (%d): ** Invalid buffer size! **\n", __LINE__);)
        return 0;
    }

    BufferType = GetBufferType(BufferSize);   //new

    if (BufferType >= NUMBER_OF_BUFFER_TYPES) {
        A_(printf("communication_buffer.c (%d): ** Invalid type of buffer! **\n", __LINE__);)
        return 0;
    }

    return Do_RFifo_GetNrOfElements(Object_p, FreeQueue_p[BufferType]);
}

void Do_Buffer_BuffersDeinit(void *Object_p)
{
    int i = 0;

    for (i = 0; i < NUMBER_OF_BUFFER_TYPES; i++) {
        while (!Do_RFifo_IsEmpty(Object_p, FreeQueue_p[i])) {
            (void)Do_RFifo_Dequeue(Object_p, FreeQueue_p[i]);
        }

        Do_RFifo_Destroy(Object_p, &FreeQueue_p[i]);
    }
}


/* @} */
