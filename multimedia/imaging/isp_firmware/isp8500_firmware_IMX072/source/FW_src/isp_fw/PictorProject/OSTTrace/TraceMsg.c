/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "DeviceParameter.h"
#include "STXP70_OPInterface.h"
#include "ITM.h"

//Default address of trace buffer
#define TRACE_BUFF_ADDR_DEFAULT                                   (0)

//Default size of trace buffer
#define TRACE_BUFF_SIZE_DEFAULT                                   (0)

// Default trace log level
#define TRACE_LOG_LEVEL_DEFAULT                                   (0xFFFF)

//Default trace log enable
#define TRACE_LOG_ENABLE_DEFAULT                                  (1)

//XTI Traces are enabled by default
#define DEFAULT_TRACEMECHANISM_SELECT                             (TraceMsgOutput_e_XTI)

//Initialization of g_TraceLogsControl page element
volatile TraceLogsControl_ts    g_TraceLogsControl =
{
    TRACE_BUFF_ADDR_DEFAULT,
    TRACE_BUFF_SIZE_DEFAULT,
    TRACE_LOG_LEVEL_DEFAULT,
    TRACE_LOG_ENABLE_DEFAULT,
    DEFAULT_TRACEMECHANISM_SELECT
};

#if TRACE_LOG
//Total number of log levels
#define NUM_LOG_LEVELS                                            (16)

//Log message prefix
#define LOG_PREFIX_LENGTH                                         (4)

// Header offsets in Trace buffer
#define BUF_SIZE_OFFSET                                           (0)
#define BLOCK_SIZE_OFFSET                                         (4)
#define BLOCK_ID_OFFSET                                           (8)
#define OVER_WRITE_CNT_OFFSET                                     (12)
#define TRACE_BUF_HEADER_SIZE                                     (16)

//Block size
#define BLOCK_SIZE                                                (128)
#define START_VAL_BLOCK_ID                                        (-1)

//Trace buffer status
typedef struct
{
    int32_t     s32_blockID;                            //Block ID
    uint32_t    u32_msgID;                              //Message ID
    uint32_t    u32_curPos;                             //Trace buffer write index
    uint32_t    u32_overWriteCnt;
} debugTrace_ts;

// Log level prefix
const int8_t                    g_MessageType[][LOG_PREFIX_LENGTH] =
{
    "ERR",
    "WAR",
    "FLO",
    "DBG",
    "API",
    "OAP",
    "OBF",
    "RSV",
    "UR1",
    "UR2",
    "UR3",
    "UR4",
    "UR5",
    "UR6",
    "UR7",
    "UR8"
};
debugTrace_ts                                                     g_TraceBuffer;

/**
* Function Name : TraceLog_Init
* Description     : Initialization of trace buffer
* Input           : None
* Return          : None
*/
int32_t
TraceLog_Init(void)
{
    if (TRACE_BUFF_ADDR_DEFAULT == g_TraceLogsControl.u32_BufferAddr)
    {
        return (Flag_e_FALSE);  // Memory not allocated for trace buffer
    }

    g_TraceBuffer.u32_curPos = TRACE_BUF_HEADER_SIZE;
    g_TraceBuffer.s32_blockID = START_VAL_BLOCK_ID;
    g_TraceBuffer.u32_msgID = 0;
    g_TraceBuffer.u32_overWriteCnt = 0;
    *((uint32_t *) (g_TraceLogsControl.u32_BufferAddr)) =
        (
            (g_TraceLogsControl.u32_BufferSize - TRACE_BUF_HEADER_SIZE) /
            BLOCK_SIZE
        );
    *((uint32_t *) (g_TraceLogsControl.u32_BufferAddr + BLOCK_SIZE_OFFSET)) = BLOCK_SIZE;
    *((uint32_t *) (g_TraceLogsControl.u32_BufferAddr + BLOCK_ID_OFFSET)) = START_VAL_BLOCK_ID;
    *((uint32_t *) (g_TraceLogsControl.u32_BufferAddr + OVER_WRITE_CNT_OFFSET)) = g_TraceBuffer.u32_overWriteCnt;

    return (Flag_e_TRUE);
}

/**
* Function Name   : TraceMemoryDump
* Description        : Writes data into trace buffer
* Input                 :
    TraceLevel_te level : Message class
    uint8_t *pString    : Input buffer
    uint8_t *buffer   : Data to be written
    uint32_t size       : Size of the data to be written
    float_t f_Param1    : Argument of debug message
    float_t f_Param2    : Argument of debug message
    float_t f_Param3    : Argument of debug message
    float_t f_Param4    : Argument of debug message
* Return                : Flag_e_TRUE or Flag_e_FALSE
*/
int32_t
TraceMemoryDump(
int8_t                *pString,
float_t               f_Param1,
float_t               f_Param2,
float_t               f_Param3,
float_t               f_Param4,
OST_TraceTypes_te   level)
{
    uint8_t     *p;
    uint8_t     *traceBufPtr = ( uint8_t * ) g_TraceLogsControl.u32_BufferAddr;
    uint32_t    size = 0,
                blockIndex = 0;

    if ((g_TraceLogsControl.u32_BufferAddr == TRACE_BUFF_ADDR_DEFAULT) || (level >= NUM_LOG_LEVELS))
    {
        return (Flag_e_FALSE);  //Memory not allocated for trace buffer or invalid log level selected
    }

    //Checking global message enable flag  && Requested level message enable flag
    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << level)))
    {
        // Disable all interrupts that are being used
        STXP70_DisableInterrupts(ITM_INTERRUPT_MASK);

        // Assumed initial value of g_TraceBuffer.blockID is -1
        //Writing msgid
        *((uint32_t *) (traceBufPtr + g_TraceBuffer.u32_curPos)) = g_TraceBuffer.u32_msgID++;   //(g_TraceBuffer.blockID+1);
        blockIndex = BLOCK_SIZE_OFFSET;

        traceBufPtr[g_TraceBuffer.u32_curPos + blockIndex++] = g_MessageType[level][0];
        traceBufPtr[g_TraceBuffer.u32_curPos + blockIndex++] = g_MessageType[level][1];
        traceBufPtr[g_TraceBuffer.u32_curPos + blockIndex++] = g_MessageType[level][2];
        traceBufPtr[g_TraceBuffer.u32_curPos + blockIndex++] = ':';

        //Updates the payload buffer with the passed string and arguments
        p = traceBufPtr + g_TraceBuffer.u32_curPos + blockIndex;
        size = Write_Payload(p, pString, f_Param1, f_Param2, f_Param3, f_Param4, BLOCK_SIZE);

        // Writing blockID
        g_TraceBuffer.s32_blockID++;
        *((uint32_t *) (traceBufPtr + BLOCK_ID_OFFSET)) = g_TraceBuffer.s32_blockID;

        //Write overWriteCount
        *((uint32_t *) (g_TraceLogsControl.u32_BufferAddr + OVER_WRITE_CNT_OFFSET))
        = g_TraceBuffer.u32_overWriteCnt;

        //Resetting blockID
        if
        (
            g_TraceBuffer.s32_blockID >=
                (((g_TraceLogsControl.u32_BufferSize - TRACE_BUF_HEADER_SIZE) / BLOCK_SIZE) - 1)
        )
        {
            g_TraceBuffer.s32_blockID = START_VAL_BLOCK_ID;
            g_TraceBuffer.u32_overWriteCnt++;
        }

        // Pointing curPos to the next block start address
        g_TraceBuffer.u32_curPos = ((g_TraceBuffer.s32_blockID + 1) * BLOCK_SIZE) + TRACE_BUF_HEADER_SIZE;

        // Enable all interrupts that are being used
        STXP70_EnableInterrupts(ITM_INTERRUPT_MASK);
    }

    return (Flag_e_TRUE);
}

#else

/**
* Function Name   : TraceMemoryDump
* Description        : Writes data into trace buffer
* Input                 :
    OST_TraceTypes_te level : Message class
    uint8_t *pString    : Input buffer
    uint8_t *buffer   : Data to be written
    uint32_t size       : Size of the data to be written
    float_t f_Param1    : Argument of debug message
    float_t f_Param2    : Argument of debug message
    float_t f_Param3    : Argument of debug message
    float_t f_Param4    : Argument of debug message
* Return                : Flag_e_TRUE or Flag_e_FALSE
*/
int32_t
TraceMemoryDump(
int8_t                *pString,
float_t               f_Param1,
float_t               f_Param2,
float_t               f_Param3,
float_t               f_Param4,
OST_TraceTypes_te   level)
{
    return (Flag_e_TRUE);
}

#endif

