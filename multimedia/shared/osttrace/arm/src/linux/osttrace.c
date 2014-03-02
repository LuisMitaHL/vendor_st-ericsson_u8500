/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// For write
#include <unistd.h>
#include <time.h>
// for CPU ID
#include <sched.h>
#define LOG_TAG "OSTTRACE"
#include <linux_utils.h>
#include "osttrace.h"
#include "osttrace_p.h"
#include "ste_ost_group_map.h"
#include <sys/types.h>
#include "stm-trace.h"
#include <los/api/los_api.h>

#define MAX_OST_STRING_SIZE 128
#define MAX_OST_FRAME_SIZE 256
#define STM_CHANNEL_OFFSET 10
// 25 (ost header basis) + 4 (extended btrace header) + 4 (multi part extra) + 8 (OMX handle and ID) + 4 (array size)
#define MAX_OST_HEADER_SIZE 45
#define MAX_MSG_SIZE 1024


static unsigned short ost_group_id[16] = {
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

typedef struct {
    unsigned char master_id;
    unsigned char version_byte;
    unsigned char entity_id;
    unsigned char protocol_id;
    unsigned char length_byte;
    // optionally, insert four more bytes for length
    unsigned char timestamp[8];
    unsigned char payload[MAX_TRACE_CHUNK_SIZE];
    // unsigned char thread_id [4];
} t_ost_ascii_trace;

#define OST_TRACE_CHANNEL_ASCII 5

#define SWAP_32(x, y) \
    *x++ = y & 0x000000FFu; \
    *x++ = (y & 0x0000FF00u) >> 8; \
    *x++ = (y & 0x00FF0000u) >> 16; \
    *x++ = (y & 0xFF000000u) >> 24

#define SWAP_64(x, y) \
    *x++ = y & 0xFFul; \
    *x++ = (y & 0xFF00ul) >> 8; \
    *x++ = (y & 0xFF0000ul) >> 16; \
    *x++ = (y & 0xFF000000ul) >> 24; \
    *x++ = (y & 0xFF00000000ul) >> 32; \
    *x++ = (y & 0xFF0000000000ul) >> 40; \
    *x++ = (y & 0xFF000000000000ul) >> 48; \
    *x++ = (y & 0xFF00000000000000ul) >> 56


/********************** MMDSP types ***********************
 *      Type       Arithmetic size     Storage size     Alignment
 *      (bits, mode 16/24)       (MAUs)         (MAUs)
 *      -----------------------------------------------------------
 *      char              8                 1               1
 *      short           16/24               1               1
 *      int             16/24               1               1
 *      long            32/48               2               1
 *      long long       40/56               3               1
 *      pointer         16/24               1               1
 *      float             32                2               1
 *      double            32                2               1
 **********************************************************/

static unsigned char st_timestamp[8] = { 0xd0, 0x00, 0x03, 0x2f, 0x3e, 0x89, 0x6e, 0x3b };
volatile t_nml_hdr vh;

#define ll_trace_8(c, v) XtiSend_8((c), (v))
#define ll_tracet_8(c, v) XtiSend_8T((c), (v))
#define ll_trace_16(c, v) XtiSend_16((c), (v))
#define ll_tracet_16(c, v) XtiSend_16T((c), (v))
#define ll_trace_32(c, v) XtiSend_32((c), (v))
#define ll_tracet_32(c, v) XtiSend_32T((c), (v))
#define ll_trace_64(c, v) XtiSend_64((c), (v))
#define ll_tracet_64(c, v) XtiSend_64T((c), (v))

#define MAKELONG(a, b, c, d) \
    (unsigned int)((((unsigned int)(a)) << 24)  | (((unsigned int)(b)) << 16)  | (((unsigned int)(c)) << 8)  | (((unsigned int)(d))))

#define MAKELONGLONG(a, b, c, d, e, f, g, h) \
    (unsigned long)((((unsigned long long)(a)) << 56)  | (((unsigned long long)(b)) << 48)  | (((unsigned long long)(c)) << 40)  | (((unsigned long long)(d)) << 32) | (((unsigned long long)(e)) << 24)  | (((unsigned long long)(f)) << 16)  | (((unsigned long long)(g)) << 8)  | (((unsigned long long)(h))))

static int write_ost_chunk(unsigned char *buffer, unsigned int size, unsigned char enable_ts, unsigned char *is_remain_byte, unsigned char *remain_byte);

static int add_ost_header(t_nml_hdr *p_header, TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int has_array_size, unsigned int has_multipart, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle);

static int trace_to_stm = 0;
static int trace_to_log = 0;

static int wedge_X(unsigned short value);
static inline void trace_to_log_or_file(TRACEMSGID_TYPE group_id,  const char *fmt, ...);

int wedge_X(unsigned short value) {
    int i = 0;

    while (value) {
        value = value >> 1;
        i++;
    }
    return i - 1;
}

void OSTTraceInit(void) {

    LOGI("Executing OSTTraceInit\n");

    {
        int prop_value;
        // The Android properties
        GET_PROPERTY("ste.ost.arm.trace.tostm", value, "0");
        prop_value = atoi(value);
        if (prop_value == 1) {
            LOGI("Executing OSTTraceInit OST trace on STM enabled\n");
            trace_to_stm = 1;
            int status = 0;
            /* Try to use STM interface for tracing */
            status = stm_trace_open();
            if (status != 0) {
                LOGE("OSTTraceInit: ERROR! stm_trace_open error: %d: Tracing over STM disabled!\n", status);
            }
        }
        else
        {
            LOGI("Executing OSTTraceInit OST trace on STM disabled\n");
        }
    }


}
void OSTTraceDeInit(void) {
    LOGI("Executing OSTTraceDeInit\n");

#if defined (__ARM_LINUX)
    stm_trace_close();
#endif

}

void OSTTrace_Test(void) {
    unsigned int i;

    for (i = 0; i < 40; i++) {
        //OSAL_Write32(50, 0, i);
        XtiSend_32(50, i);
    }

    //OSAL_Write32(50, 1, i);
    XtiSend_32T(50, i);
}

////////////////////////////////////////////////////////////////////////////
// add_ost_header
////////////////////////////////////////////////////////////////////////////

static int add_ost_header(t_nml_hdr *p_header, TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, unsigned short has_handle, unsigned int has_array_size, unsigned int has_multipart, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {
    unsigned char *p;
    unsigned int i;
    unsigned int size = 0;
    unsigned long long ts;
    unsigned int group;
    unsigned int Eheader2;

    // populate the header
    p_header->master_id    = MASTER_TRACE_ID;
    p_header->version_byte = VERSION_BYTE;
    p_header->entity_id    = OST_TRACE_ENTITY;
    p_header->protocol_id  = PROTOCOL_ID_BINARY;
    //	header.length_byte  = sizeof (header) - 5 /* [master_id..length_byte] */ + 0 /* data bytes*/;
    ts = LOS_getSystemTime();
    for (i = 0; i < 8; ++i) {
        p_header->timestamp[i] = ts >> (8 * i);
    }
    p_header->component_id[0] = (componentId & 0xFF000000u) >> 24;
    p_header->component_id[1] = (componentId & 0x00FF0000u) >> 16;
    p_header->component_id[2] = (componentId & 0x0000FF00u) >> 8;
    p_header->component_id[3] = (componentId & 0x000000FFu);

    // Do not test case 0 since means trace not enable
    group = wedge_X((tracemsgId & 0xFFFF0000u) >> 16);
    group = ost_group_id[group];
    p_header->group_id[0] = (group & 0xFF00u) >> 8;
    p_header->group_id[1] = (group & 0xFFu);

    //	p_header->group_id [0] = (tracemsgId & 0xFF000000u) >> 24;
    //	p_header->group_id [1] = (tracemsgId & 0x00FF0000u) >> 16;
    // we consider only the 16 LS bits
    p_header->trace_id[0] = (tracemsgId & 0xFF00u) >> 8;
    p_header->trace_id[1] = (tracemsgId & 0x00FFu);
    // btrace_header combines length, flags?, category and subcategory
    p_header->btrace_header[0] = 4;      // length

    p_header->btrace_header[1] = 0;      // flags == 0
    p_header->btrace_header[2] = (group & 0xFFu);
    p_header->btrace_header[3] = KBTraceSubCategoryOpenSystemTrace & 0xFFu;

    size = 25;

    p = (unsigned char *)(p_header->payload);

    // In Android sched_getcpu() is not supported so can not provide
    // information on which core it is working

    if (has_multipart >> 2) {
        // Update header with EHeader2Present
        p_header->btrace_header[1] |= 0x1;

        // Compute EHeader2Present
        Eheader2 = (has_multipart & 0x3);

        // Add Header 2, that will contain CPU and multi part information
        SWAP_32(p, Eheader2);

        // Update btrace size to cope with CPU information
        p_header->btrace_header[0] += 4;          // length
        size += 4;

        //Update header with EExtraPresent
        p_header->btrace_header[1] |= 0x20;

        // Add Multi part ID
        SWAP_32(p, has_multipart >> 2);

        // Update btrace size to take into account EExtraPresent Word
        p_header->btrace_header[0] += 4;          // length
        size += 4;
    }

    if (has_handle) {
        // Update btrace size to cope with 2 more 32-bit word
        p_header->btrace_header[0] += 8;          // length

        // Add Parent Handle
        SWAP_32(p, aParentHandle);

        // Add This Handle
        SWAP_32(p, aComponentHandle);

        size += 8;

        // To be compatible with OST SF we need to provide a size information when
        // 1 or more 32-bit word is followed by a pointe
        // The size of the pointer must then be provided just before the data pointed
        if (has_array_size) {
            // Update btrace size to cope with 2 more 32-bit word
            p_header->btrace_header[0] += 4;              // length

            // Add array_size
            SWAP_32(p, has_array_size);

            size += 4;
        }

    }

    return size;

}

#define IS_ERROR(id) \
    ((((id & 0xFFFF0000u) >> 16) & 3) & TRACE_ERROR)

#define IS_WARNING(id) \
    ((((id & 0xFFFF0000u) >> 16) & 3) & TRACE_WARNING)

#define IS_API(id) \
    ((((id & 0xFFFF0000u) >> 16) & 3) & (TRACE_API | TRACE_OMX_API))

#define IS_FLOW(id) \
    ((((id & 0xFFFF0000u) >> 16) & 3) & (TRACE_FLOW))

#define IS_UNCONDITIONAL(id) \
    (IS_ERROR(id) || IS_WARNING(id))


#ifdef ANDROID
static int getAndroidLogPrio(TRACEMSGID_TYPE id)
{
    int prio = ( (id & 0xFFFF0000u) >> 16) & 3;

    // by default only ERROR and WARNING groups are remapped to ANDROID equivalent groups in order to get the traces in logcat
    // others groups use ANDROID_LOG_DEBUG in order to be filtered or not depending on config
    if (prio & TRACE_ERROR) return ANDROID_LOG_ERROR ;
    if (prio & TRACE_WARNING) return ANDROID_LOG_WARN ;
    return ANDROID_LOG_DEBUG;

}
#endif

void hexdump(char * dest,void *ptr, int buflen) {
  unsigned char *buf = (unsigned char*)ptr;
  int i;
  int size=0;
  for (i=0; i<buflen; i++) {
        size+=sprintf(dest+size,"%02x", buf[i]);
  }
}

static inline void trace_to_log_or_file(TRACEMSGID_TYPE id, const char *fmt, ...)                    {
    va_list args;
    char trace_buffer[MAX_MSG_SIZE];
    unsigned int size = 0;

    // Initialize variable arguments and calculate length of message.
    va_start(args, fmt);
    size = vsnprintf(trace_buffer+size, MAX_MSG_SIZE, fmt, args);
    va_end(args);


#ifdef ANDROID
    int prio = getAndroidLogPrio(id);
    __do_stelp_syslog(prio,"OSTTRACE",SYSLOG_FILTERED,trace_buffer);
#else 
    __do_stelp_log(trace_buffer);
#endif

}

#define TRACE_TO_LOG_OR_FILE(has_handle, id, fmt, phdl, chdl, p...)      \
    do {                                                              \
        if (has_handle) {                                         \
            trace_to_log_or_file(id, fmt, phdl, chdl, ##p); } \
        else {                                                    \
            trace_to_log_or_file(id, fmt, ##p); } }             \
    while (0)

////////////////////////////////////////////////////////////////////////////
// OSTTrace0
////////////////////////////////////////////////////////////////////////////
void OSTTrace0(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;

    // Send trace over STM
    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.length_byte  = size - 5;

    //LOS_Log("OSTTrace0: size: %d\n", size);
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);

}

////////////////////////////////////////////////////////////////////////////
// OSTTrace1
////////////////////////////////////////////////////////////////////////////

void OSTTrace1(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, unsigned int param, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = sizeof(int);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);

    buffer.btrace_header[0] += l_size;              // length
    buffer.length_byte  = (size + l_size) - 5;

    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( unsigned int* )p ) = param;
    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

}

////////////////////////////////////////////////////////////////////////////
// OSTTrace1
////////////////////////////////////////////////////////////////////////////

void OSTTraceFloat1(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, double param, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = sizeof(double);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);

    buffer.btrace_header[0] += l_size;              // length
    buffer.length_byte  = (size + l_size) - 5;

    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( double* )p ) = param;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

}

////////////////////////////////////////////////////////////////////////////
// OSTTrace2
////////////////////////////////////////////////////////////////////////////

void OSTTrace2(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, unsigned int param1, unsigned int param2, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param1, param2);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = 2*sizeof(int);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.btrace_header[0] += l_size;              // length

    // Adding 8 bytes from Param
    buffer.length_byte  = (size + l_size) - 5;

    // Transmit OST header
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( unsigned int* )p ) = param1;
    p+=sizeof(int);
    *( ( unsigned int* )p ) = param2;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    // Transmit 32-bit param1 and 32-bit param2
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

}

////////////////////////////////////////////////////////////////////////////
// OSTTrace2
////////////////////////////////////////////////////////////////////////////

void OSTTraceFloat2(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, double param1, double param2, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param1, param2);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = 2*sizeof(double);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.btrace_header[0] += l_size;              // length

    // Adding 8 bytes from Param
    buffer.length_byte  = (size + l_size) - 5;

    // Transmit OST header
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( double* )p ) = param1;
    p+=sizeof(double);
    *( ( double* )p ) = param2;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    // Transmit 64-bit param1 and 64-bit param2
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

}


////////////////////////////////////////////////////////////////////////////
// OSTTrace3
////////////////////////////////////////////////////////////////////////////

void OSTTrace3(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, unsigned int param1, unsigned int param2, unsigned int param3, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param1, param2, param3);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = 3*sizeof(int);

    //LOS_Log("OSTTrace3: size: %d\n", size);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.btrace_header[0] += l_size;              // length

    // Adding 12 bytes from Param
    buffer.length_byte  = (size + l_size) - 5;

    // Transmit OST header
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( unsigned int* )p ) = param1;
    p+=sizeof(int);
    *( ( unsigned int* )p ) = param2;
    p+=sizeof(int);
    *( ( unsigned int* )p ) = param3;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    // Transmit 32-bit param1, param2, param3
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;
}

////////////////////////////////////////////////////////////////////////////
// OSTTrace3
////////////////////////////////////////////////////////////////////////////

void OSTTraceFloat3(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, double param1, double param2, double param3, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param1, param2, param3);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = 3*sizeof(double);

    //LOS_Log("OSTTrace3: size: %d\n", size);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.btrace_header[0] += l_size;              // length

    // Adding 12 bytes from Param
    buffer.length_byte  = (size + l_size) - 5;

    // Transmit OST header
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( double* )p ) = param1;
    p+=sizeof(double);
    *( ( double* )p ) = param2;
    p+=sizeof(double);
    *( ( double* )p ) = param3;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    // Transmit 32-bit param1, param2, param3
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;
}



////////////////////////////////////////////////////////////////////////////
// OSTTrace4
////////////////////////////////////////////////////////////////////////////

void OSTTrace4(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param1, param2, param3, param4);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = 4*sizeof(int);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.btrace_header[0] += l_size;              // length

    // Adding 16 bytes from Param
    buffer.length_byte  = (size + l_size) - 5;

    // Transmit OST header
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( unsigned int* )p ) = param1;
    p+=sizeof(int);
    *( ( unsigned int* )p ) = param2;
    p+=sizeof(int);
    *( ( unsigned int* )p ) = param3;
    p+=sizeof(int);
    *( ( unsigned int* )p ) = param4;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    // Transmit 32-bit param1, param2, param3
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;
}
////////////////////////////////////////////////////////////////////////////
// OSTTrace4
////////////////////////////////////////////////////////////////////////////

void OSTTraceFloat4(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, double param1, double param2, double param3, double param4, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {

    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, aTraceText, aParentHandle, aComponentHandle, param1, param2, param3, param4);

    if (!trace_to_stm) { return; }

    // for this simple type of OST trace, we know from the beginning
    // that a normal header is enough
    t_nml_hdr buffer;
    unsigned char *p;
    unsigned int size = 0;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned char l_size = 4*sizeof(double);

    size = add_ost_header(&buffer, componentId, tracemsgId, has_handle, 0, 0, aComponentHandle, aParentHandle);
    buffer.btrace_header[0] += l_size;              // length

    // Adding 16 bytes from Param
    buffer.length_byte  = (size + l_size) - 5;

    // Transmit OST header
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    // Reuse buffer since header already send
    p = ((unsigned char *)&buffer);
    *( ( double* )p ) = param1;
    p+=sizeof(double);
    *( ( double* )p ) = param2;
    p+=sizeof(double);
    *( ( double* )p ) = param3;
    p+=sizeof(double);
    *( ( double* )p ) = param4;

    size = l_size;
    if (is_remain_byte) {
        size++;
    }

    // Transmit 64-bit param1, param2, param3
    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;
}

////////////////////////////////////////////////////////////////////////////
// OSTTraceData
////////////////////////////////////////////////////////////////////////////
void OSTTraceData(TRACEMSGID_TYPE componentId, TRACEMSGID_TYPE tracemsgId, const char *aTraceText, unsigned char *ptr, unsigned int length, unsigned short has_handle, unsigned int aComponentHandle, TRACEMSGID_TYPE aParentHandle) {
    t_nml_hdr buffer;
    unsigned int size;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;
    unsigned int ost_length;
    unsigned int nb_loop = (length / (MAX_OST_FRAME_SIZE - MAX_OST_HEADER_SIZE));
    unsigned int reminder = (length % (MAX_OST_FRAME_SIZE - MAX_OST_HEADER_SIZE));
    unsigned int has_multipart = 0;
    unsigned int i = 0;
    unsigned int ran = ((unsigned int)random() >> 2) << 2;
    unsigned short l_has_handle;

#if (defined(__arm__) || defined(__linux__))
    char buf[MAX_MSG_SIZE+1];
    char bufdata[MAX_MSG_SIZE +1];
    hexdump(bufdata,ptr,length);
    int msgsize=snprintf(buf,MAX_MSG_SIZE, "%s data[%d]=", aTraceText,length);
    msgsize+=snprintf(buf+msgsize,MAX_MSG_SIZE-msgsize,bufdata);
    if (msgsize>=MAX_MSG_SIZE) {
        sprintf(buf+MAX_MSG_SIZE-strlen("[TRUNC]"),"[TRUNC]");
    }
    TRACE_TO_LOG_OR_FILE(has_handle, tracemsgId, buf, aParentHandle, aComponentHandle );
#endif

    for (i = 0; i <= nb_loop; i++) {
        if (nb_loop == 0) {
            has_multipart = 0;
            ost_length = length;
            l_has_handle = has_handle;
        } else   {
            ran = (ran >> 2) << 2;
            ost_length = MAX_OST_FRAME_SIZE - MAX_OST_HEADER_SIZE;
            // By default except if first part of multi part
            l_has_handle = 0;

            if (i == 0) {
                has_multipart = 1;
                l_has_handle = has_handle;
            } else if (i != nb_loop)   {
                has_multipart = 2;
            } else if (i == nb_loop) {
                has_multipart = 3;
                ost_length = reminder;
            }
            ran |= has_multipart;
            has_multipart = ran;
        }

        if (l_has_handle) {
            size = add_ost_header(&buffer, componentId, tracemsgId, l_has_handle, length, has_multipart, aComponentHandle, aParentHandle);
        } else {
            size = add_ost_header(&buffer, componentId, tracemsgId, l_has_handle, ost_length, has_multipart, aComponentHandle, aParentHandle);
        }

        buffer.btrace_header[0] += ost_length;          // length
        buffer.length_byte  = (size + ost_length) - 5;

        // Transmit OST header
        MASK_INTERRUPTS;
        write_ost_chunk((unsigned char *)&buffer, size, 0, &is_remain_byte, &remain_byte);
        UNMASK_INTERRUPTS;

        size = ost_length;
        if (is_remain_byte) {
            size++;
        }

        // Transmit buffer
        MASK_INTERRUPTS;
        write_ost_chunk(ptr, size, 1, &is_remain_byte, &remain_byte);
        ptr += ost_length;
        UNMASK_INTERRUPTS;
    }
}

void OST_Printf(const char *fmt, ...) {
    va_list args;
    t_ost_ascii_trace trace_buffer;
    unsigned int size = 0;
    int i;
    unsigned char is_remain_byte = 0;
    unsigned char remain_byte;

    trace_to_log_or_file(FIXED_GROUP_TRACE_DEBUG, fmt);

    if (!trace_to_stm) { return; }

    // Initialize variable arguments and calculate length of message.
    va_start(args, fmt);
    size = vsnprintf((char *)&trace_buffer.payload, MAX_TRACE_CHUNK_SIZE, fmt, args);

    size = sizeof(trace_buffer) - MAX_TRACE_CHUNK_SIZE + size;

    // populate the header
    trace_buffer.master_id    = MASTER_TRACE_ID;
    trace_buffer.version_byte = VERSION_BYTE;
    trace_buffer.entity_id    = 0x01;
    trace_buffer.protocol_id  = PROTOCOL_ID_ASCII;
    trace_buffer.length_byte  = size - 5;
    for (i = 0; i < 8; ++i) {
        trace_buffer.timestamp[i] = st_timestamp[i];
    }

    MASK_INTERRUPTS;
    write_ost_chunk((unsigned char *)&trace_buffer, size, 1, &is_remain_byte, &remain_byte);
    UNMASK_INTERRUPTS;

    va_end(args);

}

static int write_ost_chunk(unsigned char *buffer, unsigned int size, unsigned char enable_ts, unsigned char *is_remain_byte, unsigned char *remain_byte) {
    unsigned char *p, tab[8];
    unsigned int first_time = 1;
    int channel;
    unsigned int i = 0;

#if defined (__ARM_LINUX)
    if (stm_trace_get_thread_channel(&channel) != 0) {
        return -1;
    }
#endif

    channel += STM_CHANNEL_OFFSET;

    p = buffer;
    if (*is_remain_byte) {
        tab[0] = *remain_byte;
    } else {
        tab[0] = *p++;
    }

    *is_remain_byte = 0;
    *remain_byte = 0;

    while (size > 0) {
        if (size > 8) {
            for (i = first_time; i < 8; i++) {
                tab[i] = *p++;
            }
            ll_trace_64(channel, (long long)htonl(*(int *)tab) << 32 | htonl(*(int *)(tab + 4)));
            size -= 8;
        } else {
            if (size == 8) {
                for (i = first_time; i < 8; i++) {
                    tab[i] = *p++;
                }
                if (enable_ts) {
                    ll_tracet_64(channel, (long long)htonl(*(int *)tab) << 32 | htonl(*(int *)(tab + 4)));
                } else {
                    ll_trace_64(channel, (long long)htonl(*(int *)tab) << 32 | htonl(*(int *)(tab + 4)));
                }
                size -= 8;
                return 0;
            }
            if (size > 4) {
                for (i = first_time; i < 4; i++) {
                    tab[i] = *p++;
                }
                if (first_time) {
                    first_time = 0;
                }
                ll_trace_32(channel, htonl(*(int *)tab));
                size -= 4;
            }
            if (size == 4) {
                for (i = first_time; i < 4; i++) {
                    tab[i] = *p++;
                }
                if (enable_ts) {
                    ll_tracet_32(channel, htonl(*(int *)tab));
                } else {
                    ll_trace_32(channel, htonl(*(int *)tab));
                }
                size -= 4;
                return 0;
            }
            if (size > 2) {
                for (i = first_time; i < 2; i++) {
                    tab[i] = *p++;
                }
                if (first_time) {
                    first_time = 0;
                }
                ll_trace_16(channel, htons(*(short *)tab));
                size -= 2;
            }
            if (size == 2) {
                for (i = first_time; i < 2; i++) {
                    tab[i] = *p++;
                }
                if (enable_ts) {
                    ll_tracet_16(channel, htons(*(short *)tab));
                } else {
                    ll_trace_16(channel, htons(*(short *)tab));
                }
                size -= 2;
                return 0;
            }

            if (size /* == 1 */) {
                if (first_time) {
                    first_time = 0;
                } else {
                    tab[0] = *p++;
                }
                if (enable_ts) {
                    ll_tracet_8(channel, tab[0]);
                    return 0;
                } else   {
                    *remain_byte = tab[0];
                    *is_remain_byte = 1;
                    return 0;
                }
            }
        }
        first_time = 0;
    }
    return 0;
}
