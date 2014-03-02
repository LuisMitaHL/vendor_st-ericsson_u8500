/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "DeviceParameter.h"

#if (USE_OST_TRACES == PROTOCOL_ID_BINARY)  // We need below code only if Binary trace protocol is needed. So Dont compile it otherwise...
#   define SWAP_32(x, y)          \
    *x++ = y & 0x0000FFu;         \
    *x++ = (y & 0x00FF00u) >> 8;  \
    *x++ = (y & 0xFF0000u) >> 16; \
    *x++ = (y & 0xFF000000ul) >> 24

#   define MAKELONG(a, b, c, d)                                                                                  \
    ( unsigned long )                                                                                            \
        (                                                                                                        \
            ((( unsigned long ) (a)) << 24) | ((( unsigned long ) (b)) << 16) | ((( unsigned long ) (c)) << 8) | \
                ((( unsigned long ) (d)))                                                                        \
        )
#   define MAX_OST_FRAME_SIZE  256

// 25 (ost header basis) + 4 (extended btrace header) + 4 (multi part extra) + 8 (OMX handle and ID) + 4 (array size)
#   define MAX_OST_HEADER_SIZE 45

#   define KBTraceCategoryOpenSystemTrace      0xA2u
#   define KBTraceSubCategoryOpenSystemTrace   0x00u

#   define FIXED_GROUP_TRACE_ERROR             130
#   define FIXED_GROUP_TRACE_WARNING           131
#   define FIXED_GROUP_TRACE_FLOW              138
#   define FIXED_GROUP_TRACE_DEBUG             137
#   define FIXED_GROUP_TRACE_API               132
#   define FIXED_GROUP_TRACE_OMX_API           150
#   define FIXED_GROUP_TRACE_OMX_BUFFER        151
#   define FIXED_GROUP_TRACE_RESERVED          152
#   define FIXED_GROUP_TRACE_USER1             153
#   define FIXED_GROUP_TRACE_USER2             154
#   define FIXED_GROUP_TRACE_USER3             155
#   define FIXED_GROUP_TRACE_USER4             156
#   define FIXED_GROUP_TRACE_USER5             157
#   define FIXED_GROUP_TRACE_USER6             158
#   define FIXED_GROUP_TRACE_USER7             159
#   define FIXED_GROUP_TRACE_USER8             160

#   define TRACE_GROUP_BIT_MASK_0              FIXED_GROUP_TRACE_ERROR
#   define TRACE_GROUP_BIT_MASK_1              FIXED_GROUP_TRACE_WARNING
#   define TRACE_GROUP_BIT_MASK_2              FIXED_GROUP_TRACE_FLOW
#   define TRACE_GROUP_BIT_MASK_3              FIXED_GROUP_TRACE_DEBUG
#   define TRACE_GROUP_BIT_MASK_4              FIXED_GROUP_TRACE_API
#   define TRACE_GROUP_BIT_MASK_5              FIXED_GROUP_TRACE_OMX_API
#   define TRACE_GROUP_BIT_MASK_6              FIXED_GROUP_TRACE_OMX_BUFFER
#   define TRACE_GROUP_BIT_MASK_7              FIXED_GROUP_TRACE_RESERVED
#   define TRACE_GROUP_BIT_MASK_8              FIXED_GROUP_TRACE_USER1
#   define TRACE_GROUP_BIT_MASK_9              FIXED_GROUP_TRACE_USER2
#   define TRACE_GROUP_BIT_MASK_10             FIXED_GROUP_TRACE_USER3
#   define TRACE_GROUP_BIT_MASK_11             FIXED_GROUP_TRACE_USER4
#   define TRACE_GROUP_BIT_MASK_12             FIXED_GROUP_TRACE_USER5
#   define TRACE_GROUP_BIT_MASK_13             FIXED_GROUP_TRACE_USER6
#   define TRACE_GROUP_BIT_MASK_14             FIXED_GROUP_TRACE_USER7
#   define TRACE_GROUP_BIT_MASK_15             FIXED_GROUP_TRACE_USER8

static unsigned short   ost_group_id[16] =
{
    TRACE_GROUP_BIT_MASK_0,
    TRACE_GROUP_BIT_MASK_1,
    TRACE_GROUP_BIT_MASK_2,
    TRACE_GROUP_BIT_MASK_3,
    TRACE_GROUP_BIT_MASK_4,
    TRACE_GROUP_BIT_MASK_5,
    TRACE_GROUP_BIT_MASK_6,
    TRACE_GROUP_BIT_MASK_7,
    TRACE_GROUP_BIT_MASK_8,
    TRACE_GROUP_BIT_MASK_9,
    TRACE_GROUP_BIT_MASK_10,
    TRACE_GROUP_BIT_MASK_11,
    TRACE_GROUP_BIT_MASK_12,
    TRACE_GROUP_BIT_MASK_13,
    TRACE_GROUP_BIT_MASK_14,
    TRACE_GROUP_BIT_MASK_15,
};

typedef struct
{
    uint8_t   master_id;
    uint8_t   version_byte;
    uint8_t   entity_id;
    uint8_t   protocol_id;
    uint8_t   length_byte;
    uint8_t   timestamp[8];
    uint8_t   component_id[4];
    uint8_t   group_id[2];
    uint8_t   trace_id[2];
    uint8_t   btrace_header[4];
    uint8_t   payload[20];
} t_nml_hdr_ts;

static int  wedge_X (unsigned short value);
static int  add_ost_header (
            t_nml_hdr_ts       *p_header,
            TRACEMSGID_TYPE componentId,
            TRACEMSGID_TYPE tracemsgId,
            unsigned short  has_handle,
            unsigned long   has_array_size,
            unsigned long   has_multipart,
            unsigned int    aComponentHandle,
            TRACEMSGID_TYPE aParentHandle);
static int  write_ost_chunk (
            unsigned char   *buffer,
            unsigned int    size,
            unsigned char   enable_ts,
            unsigned char   *is_remain_byte,
            unsigned char   *remain_byte);

////////////////////////////////////////////////////////////////////////////

// wedge_X
////////////////////////////////////////////////////////////////////////////
static int
wedge_X(
unsigned short  value)
{
    int i = 0;

    while (value)
    {
        value = value >> 1;
        i++;
    }


    return (i - 1);
}


////////////////////////////////////////////////////////////////////////////

// add_ost_header
////////////////////////////////////////////////////////////////////////////
static int
add_ost_header(
t_nml_hdr_ts       *p_header,
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned short  has_handle,
unsigned long   has_array_size,
unsigned long   has_multipart,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned char   *p;
    unsigned int    i;
    unsigned int    size = 0;
    unsigned int    group;
    unsigned int    mode;

    // populate the header
    p_header->master_id = MASTER_TRACE_ID;
    p_header->version_byte = VERSION_BYTE;
    p_header->entity_id = OST_TRACE_ENTITY;
    p_header->protocol_id = PROTOCOL_ID_BINARY;

    //	header.length_byte  = sizeof (header) - 5 /* [master_id..length_byte] */ + 0 /* data bytes*/;
    p_header->timestamp[0] = 0;
    for (i = 1; i < 8; ++i)
    {
        p_header->timestamp[i] = st_timestamp[i];
    }


    // 		p_header->timestamp [i] = (timerValue >> (56-8*i)) & 0xFFu;
    p_header->component_id[0] = (componentId & 0xFF000000ul) >> 24;
    p_header->component_id[1] = (componentId & 0xFF0000ul) >> 16;
    p_header->component_id[2] = (componentId & 0x00FF00ul) >> 8;
    p_header->component_id[3] = (componentId & 0x0000FFul);

    group = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);
    group = ost_group_id[group];

    p_header->group_id[0] = (group & 0xFF00u) >> 8;
    p_header->group_id[1] = (group & 0xFFu);

    // we consider only the 16 LS bits
    p_header->trace_id[0] = (tracemsgId & 0xFF00u) >> 8;
    p_header->trace_id[1] = (tracemsgId & 0x00FFu);

    // btrace_header combines length, flags?, category and subcategory
    p_header->btrace_header[0] = 4;             // length
    p_header->btrace_header[1] = 0;             // flags == "no other field than data is present"
    p_header->btrace_header[2] = (group & 0xFFu);
    p_header->btrace_header[3] = KBTraceSubCategoryOpenSystemTrace & 0xFFu;

    size = 25;
    p = ( unsigned char * ) (p_header->payload);

    if (has_multipart >> 2)
    {
        // Update btrace size to cope with 4 bytes Eheader2 + 4 bytes EExtra data
        p_header->btrace_header[0] += 8;        // length

        // Update header with EExtraPresent and EHeader2Present
        p_header->btrace_header[1] |= 0x21;

        // Add Eheader2
        SWAP_32(p, has_multipart & 3);

        // Add Multi part ID
        SWAP_32(p, has_multipart >> 2);

        size += 8;
    }


    if (has_handle)
    {
        // Add Parent Handle
        SWAP_32(p, aParentHandle);

        // Add This Handle
        if (mode & 0x80)
        {
            *p++ = (aComponentHandle & 0x00FFu);
            *p++ = (aComponentHandle & 0xFF00u) >> 8;
            *p++ = 0;
        }
        else
        {
            *p++ = (aComponentHandle & 0x0000FFu);
            *p++ = (aComponentHandle & 0x00FF00u) >> 8;
            *p++ = (aComponentHandle & 0xFF0000u) >> 16;
        }


        *p++ = 0;

        // Update length with Parent Handle and ID
        p_header->btrace_header[0] += 8;        // length
        size += 8;

        // To be compatible with OST SF we need to provide a size information when
        // 1 or more 32-bit word is followed by a pointe
        // The size of the pointer must then be provided just before the data pointed
        if (has_array_size)
        {
            // Update btrace size to cope with 2 more 32-bit word
            p_header->btrace_header[0] += 4;    // length

            // Add array_size
            SWAP_32(p, has_array_size);
            size += 4;
        }
    }


    return (size);
}


////////////////////////////////////////////////////////////////////////////

// OSTTrace0
////////////////////////////////////////////////////////////////////////////
void
OSTTrace0(
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned short  has_handle,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned int    Level;
    Level = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);

    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        // for this simple type of OST trace, we know from the beginning
        // that a normal header is enough
        t_nml_hdr_ts       buffer;
        unsigned int    size = 0;
        unsigned char   is_remain_byte = 0;
        unsigned char   remain_byte;

        size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
        buffer.length_byte = size - 5;

        write_ost_chunk(( unsigned char * ) &buffer, size, 1, &is_remain_byte, &remain_byte);
    }
}


////////////////////////////////////////////////////////////////////////////

// OSTTrace1
////////////////////////////////////////////////////////////////////////////
void
OSTTrace1(
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned int    param,
unsigned short  has_handle,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned int    Level;
    Level = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);

    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        // for this simple type of OST trace, we know from the beginning
        // that a normal header is enough
        t_nml_hdr_ts       buffer;
        unsigned char   *p;
        unsigned int    size = 0;
        unsigned char   is_remain_byte = 0;
        unsigned char   remain_byte;

        size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
        buffer.btrace_header[0] += 4;   // length

        // Adding 4 bytes from Param
        buffer.length_byte = (size + 4) - 5;

        // Transmit OST header
        write_ost_chunk(( unsigned char * ) &buffer, size, 0, &is_remain_byte, &remain_byte);

        // Reuse buffer since header already send
        p = (( unsigned char * ) &buffer);
        SWAP_32(p, param);
        size = 4;
        if (is_remain_byte)
        {
            size++;
        }


        // Transmit 32-bit param
        write_ost_chunk(( unsigned char * ) &buffer, size, 1, &is_remain_byte, &remain_byte);
    }
}


////////////////////////////////////////////////////////////////////////////

// OSTTrace2
////////////////////////////////////////////////////////////////////////////
void
OSTTrace2(
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned int    param1,
unsigned int    param2,
unsigned short  has_handle,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned int    Level;
    Level = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);

    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        t_nml_hdr_ts       buffer;
        unsigned char   *p;
        unsigned int    size = 0;
        unsigned char   is_remain_byte = 0;
        unsigned char   remain_byte;

        size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
        buffer.btrace_header[0] += 8;   // length

        // Adding 8 bytes from Param
        buffer.length_byte = (size + 8) - 5;

        // Transmit OST header
        write_ost_chunk(( unsigned char * ) &buffer, size, 0, &is_remain_byte, &remain_byte);

        // Reuse buffer since header already send
        p = (( unsigned char * ) &buffer);
        SWAP_32(p, param1);
        SWAP_32(p, param2);
        size = 8;
        if (is_remain_byte)
        {
            size++;
        }


        // Transmit 32-bit param1 and 32-bit param2
        write_ost_chunk(( unsigned char * ) &buffer, size, 1, &is_remain_byte, &remain_byte);
    }
}


////////////////////////////////////////////////////////////////////////////

// OSTTrace3
////////////////////////////////////////////////////////////////////////////
void
OSTTrace3(
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned int    param1,
unsigned int    param2,
unsigned int    param3,
unsigned short  has_handle,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned int    Level;
    Level = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);

    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        t_nml_hdr_ts       buffer;
        unsigned char   *p;
        unsigned int    size = 0;
        unsigned char   is_remain_byte = 0;
        unsigned char   remain_byte;

        size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
        buffer.btrace_header[0] += 12;  // length

        // Adding 12 bytes from Param
        buffer.length_byte = (size + 12) - 5;

        // Transmit OST header
        write_ost_chunk(( unsigned char * ) &buffer, size, 0, &is_remain_byte, &remain_byte);

        // Reuse buffer since header already send
        p = (( unsigned char * ) &buffer);
        SWAP_32(p, param1);
        SWAP_32(p, param2);
        SWAP_32(p, param3);
        size = 12;
        if (is_remain_byte)
        {
            size++;
        }


        // Transmit 32-bit param1, param2, param3
        write_ost_chunk(( unsigned char * ) &buffer, size, 1, &is_remain_byte, &remain_byte);
    }
}


////////////////////////////////////////////////////////////////////////////

// OSTTrace4
////////////////////////////////////////////////////////////////////////////
void
OSTTrace4(
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned int    param1,
unsigned int    param2,
unsigned int    param3,
unsigned int    param4,
unsigned short  has_handle,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned int    Level;
    Level = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);

    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        t_nml_hdr_ts       buffer;
        unsigned char   *p;
        unsigned int    size = 0;
        unsigned char   is_remain_byte = 0;
        unsigned char   remain_byte;

        size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
        buffer.btrace_header[0] += 16;  // length

        // Adding 16 bytes from Param
        buffer.length_byte = (size + 16) - 5;

        // Transmit OST header
        write_ost_chunk(( unsigned char * ) &buffer, size, 0, &is_remain_byte, &remain_byte);

        // Reuse buffer since header already send
        p = (( unsigned char * ) &buffer);
        SWAP_32(p, param1);
        SWAP_32(p, param2);
        SWAP_32(p, param3);
        SWAP_32(p, param4);
        size = 16;
        if (is_remain_byte)
        {
            size++;
        }


        // Transmit 32-bit param1, param2, param3
        write_ost_chunk(( unsigned char * ) &buffer, size, 1, &is_remain_byte, &remain_byte);
    }
}


////////////////////////////////////////////////////////////////////////////

// OSTTraceData
////////////////////////////////////////////////////////////////////////////
void
OSTTraceData(
TRACEMSGID_TYPE componentId,
TRACEMSGID_TYPE tracemsgId,
unsigned char   *ptr,
unsigned int    length,
unsigned short  has_handle,
unsigned int    aComponentHandle,
TRACEMSGID_TYPE aParentHandle)
{
    unsigned int    Level;
    Level = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);

    if ((g_TraceLogsControl.u8_LogEnable == 1) && (g_TraceLogsControl.u32_LogLevels & (1 << Level)))
    {
        t_nml_hdr_ts       buffer;
        unsigned int    size = 0;
        unsigned char   is_remain_byte = 0;
        unsigned char   remain_byte;
        unsigned int    ost_length;

        ost_length = length;

        if (length > ost_length)
        {
            ost_length = MAX_OST_FRAME_SIZE - MAX_OST_HEADER_SIZE;
        }


        size = add_ost_header(
            &buffer,
            componentId,
            tracemsgId,
            has_handle,
            ost_length,
            0,
            aComponentHandle,
            aParentHandle);
        buffer.btrace_header[0] += ost_length;  // length
        buffer.length_byte = (size + ost_length) - 5;

        // Transmit OST header
        write_ost_chunk(( unsigned char * ) &buffer, size, 0, &is_remain_byte, &remain_byte);
        size = ost_length;
        if (is_remain_byte)
        {
            size++;
        }


        // Transmit buffer
        write_ost_chunk(ptr, size, 1, &is_remain_byte, &remain_byte);
    }
}


static int
write_ost_chunk(
unsigned char   *buffer,
unsigned int    size,
unsigned char   enable_ts,
unsigned char   *is_remain_byte,
unsigned char   *remain_byte)
{
    unsigned char   *p,
                    c1,
                    c2,
                    c3,
                    c4;
    unsigned int    first_time = 1;
    unsigned char   channel;

    channel = ++g_Channel;
    /* Increment the channel id here. Reason: As discussed with Atul, we should increment Channel ID at each write to hardware.
    Since this is a local variable, it will ensure that eveytime execution returns after serving an interrupt (which might have OSTTrace call),
    it will start writing the earlier PENDING data at same channel. Once the interrupt is served, it will decrement the global variable(at the end of this func),
    which shows that this channel is free and we can use it if another interrupt comes in between.... <Sandeep Sharma>
    */
    // send off the header less the last byte
    p = buffer;
    if (*is_remain_byte)
    {
        c1 = *remain_byte;
    }
    else
    {
        c1 = *p++;
    }


    *is_remain_byte = 0;
    *remain_byte = 0;

    while (size > 0)
    {
        if (size > 4)
        {
            if (first_time)
            {
                c2 = *p++;
                c3 = *p++;
                c4 = *p++;
            }
            else
            {
                c1 = *p++;
                c2 = *p++;
                c3 = *p++;
                c4 = *p++;
            }


            stm_trace_32(channel, MAKELONG(c1, c2, c3, c4));
            size -= 4;
        }
        else
        {
            if (size == 4)
            {
                if (first_time)
                {
                    c2 = *p++;
                    c3 = *p++;
                    c4 = *p++;
                }
                else
                {
                    c1 = *p++;
                    c2 = *p++;
                    c3 = *p++;
                    c4 = *p++;
                }


                if (enable_ts)
                {
                    stm_tracet_32(channel, MAKELONG(c1, c2, c3, c4));
                }
                else
                {
                    stm_trace_32(channel, MAKELONG(c1, c2, c3, c4));
                }


                size -= 4;
                g_Channel--; // Explanation at start of this function
                return (0);
            }


            if (size > 2)
            {
                if (first_time)
                {
                    c2 = *p++;
                }
                else
                {
                    c1 = *p++;
                    c2 = *p++;
                }


                stm_trace_16(channel, (( unsigned int ) c1 << 8) | c2);
                size -= 2;
            }


            if (size == 2)
            {
                if (first_time)
                {
                    c2 = *p++;
                }
                else
                {
                    c1 = *p++;
                    c2 = *p++;
                }


                if (enable_ts)
                {
                    stm_tracet_16(channel, (( unsigned int ) c1 << 8) | c2);
                }
                else
                {
                    stm_trace_16(channel, (( unsigned int ) c1 << 8) | c2);
                }


                size -= 2;
                g_Channel--; // Explanation at start of this function
                return (0);
            }


            if (size /* == 1 */ )
            {
                if (first_time)
                {
                    c2 = 0;
                }
                else
                {
                    c1 = *p++;
                    c2 = 0;
                }


                if (enable_ts)
                {
                    stm_tracet_16(channel, (( unsigned int ) c1 << 8) | c2);
                    g_Channel--; // Explanation at start of this function
                    return (0);
                }
                else
                {
                    *remain_byte = c1;
                    *is_remain_byte = 1;
                    g_Channel--; // Explanation at start of this function
                    return (0);
                }
            }
        }


        first_time = 0;
    }

    g_Channel--; // Explanation at start of this function
    return (0);
}


#endif //#if (USE_OST_TRACES == PROTOCOL_ID_BINARY )


