/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "DeviceParameter.h"
#include "STXP70_OPInterface.h"
#include "ITM.h"

#if (USE_OST_TRACES == PROTOCOL_ID_ASCII)   // We need below code only if ascii trace protocol is needed. So Dont compile it otherwise...

#define MAX_ASCII_TRACE_CHUNK_SIZE    (256)
//#define OST_TRACE_CHANNEL_ASCII          (5)
typedef struct
{
    uint8_t   master_id;
    uint8_t   version_byte;
    uint8_t   entity_id;
    uint8_t   protocol_id;
    uint8_t   length_byte;     // optionally, insert four more bytes for length
    uint8_t   nost_timestamp[8];
    uint8_t   payload[MAX_ASCII_TRACE_CHUNK_SIZE];

    // unsigned char thread_id [4];
} t_ost_ascii_trace_ts;

/*-----------------------Private Function Declaration----------------------- */
static void     write_ost_chunk (unsigned char channel, unsigned char *pBuffer, unsigned int size);


/*-----------------------Private Function Definition----------------------- */
/**
* Function Name   : write_ost_chunk
* Description        : Writes data on the STM's desired channel
* Input                 :
    unsigned char channel : Channel no where data is to be written
    unsigned char *buffer: Data to be written
    unsigned int size: size of the data to be written
* Return              : None
*/
static void
write_ost_chunk(
unsigned char   channel,
unsigned char   *pBuffer,
unsigned int    size)
{
    unsigned char   *p,
                    c1,
                    c2,
                    c3,
                    c4;

    // send off the header less the last byte
    p = pBuffer;
    while (size > 2)
    {
        if (size > 4)
        {
            c1 = *p++;
            c2 = *p++;
            c3 = *p++;
            c4 = *p++;
            stm_trace_32(channel, (( int ) c1 << 24) | c2 << 16 | c3 << 8 | c4);
            size -= 4;
        }
        else if (size > 2)
        {
            c1 = *p++;
            c2 = *p++;

            //LOS_Log("data:= 0x%x\n",  ((short int) c1 << 8) | c2);
            stm_trace_16(channel, (( int ) c1 << 8) | c2);
            size -= 2;
        }
    }


    // now we are left with the last byte of the header to send and have
    // to make sure the last STP message is timestamped
    //stm_tracet_16 (channel, (short int) (*p) << 8);
    if (size > 1)
    {
        c1 = *p++;
        c2 = *p++;
        stm_tracet_16(channel, (( short int ) c1 << 8) | c2);
    }
    else
    {
        c1 = *p++;
        stm_tracet_8(channel, (( char ) c1));
    }
}


/**
* Function Name   : OST_Printf
* Description        : Writes the passed string and arrguments on STM
* Input                 :
    uint8_t * pString: String passed for display
    float_t f_Param1: First Parameter passed to be displayed
    float_t f_Param2 : Second Parameter passed to be displayed
    float_t f_Param3 : Third Parameter passed to be displayed
    float_t f_Param4 : Fourth Parameter passed to be displayed
* Return              : None
*/
void
OST_Printf(
uint8_t    Level,
uint8_t    *pString,
float_t   f_Param1,
float_t   f_Param2,
float_t   f_Param3,
float_t   f_Param4)
{
    //Check if the passed level of traces is enabled in PE or not
    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        // Disable all interrupts that are being used
        STXP70_DisableInterrupts(ITM_INTERRUPT_MASK);

            uint32_t        size;
            uint8_t       i, channel;
            t_ost_ascii_trace_ts   trace_buffer;
            size = 0;

            size = Write_Payload(
                trace_buffer.payload,
                pString,
                f_Param1,
                f_Param2,
                f_Param3,
                f_Param4,
                MAX_ASCII_TRACE_CHUNK_SIZE);

            size = sizeof(trace_buffer) - MAX_ASCII_TRACE_CHUNK_SIZE + size;

            // populate the header
            trace_buffer.master_id = MASTER_TRACE_ID;
            trace_buffer.version_byte = VERSION_BYTE;
            trace_buffer.entity_id = 0x01;
            trace_buffer.protocol_id = PROTOCOL_ID_ASCII;
            trace_buffer.length_byte = size - 5;

            for (i = 0; i < 8; ++i)
            {
                trace_buffer.nost_timestamp[i] = st_timestamp[i];
            }

          g_Channel++;
         channel = g_Channel;
          /* Increment the channel id here. Reason: As discussed with Atul, we should increment Channel ID at each write to hardware.
          Since this is a local variable, it will ensure that eveytime execution returns after serving an interrupt (which might have OSTTrace call),
          it will start writing the earlier PENDING data at same channel. Once the interrupt is served, it will decrement the global variable(at the end of this func),
          which is equivalent to show that this channel is free and we can use it if another interrupt comes in between.... <Sandeep Sharma>
*/
          write_ost_chunk(/*OST_TRACE_CHANNEL_ASCII*/channel , ( uint8_t * ) &trace_buffer, size);
          g_Channel--;
            // Enable all interrupts that are being used
            STXP70_EnableInterrupts(ITM_INTERRUPT_MASK);
        }
}

#endif //#if (USE_OST_TRACES == PROTOCOL_ID_ASCII)

