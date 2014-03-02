/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "ste_adm_omx_core.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define __USE_UNIX98
#include <pthread.h>
#include <unistd.h>
#include <float.h>

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Index.h"
#include "OMX_Types.h"
#include "OMX_Audio.h"
#include "ENS_Index.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"

#include "AFM_Index.h"
#include "AFM_Types.h"

#define ADM_LOG_FILENAME "core"
#include "ste_adm_dbg.h"

#include "ste_adm_omx_log.h"

#include "libeffects/libresampling/include/resample.h"
#include "api_base_audiolibs_types.h"

#include "libeffects/libmixer/include/mixer_include.h"

#include <sys/time.h>

#include "BSD_list.h"
#include <poll.h>
#include <errno.h>
#include <sys/eventfd.h>
#include <sys/stat.h>

#ifdef ADM_MMPROBE
#include "r_mm_probe.h"
#endif // ADM_MMPROBE

// #define DEBUG_PERF

#ifdef ADM_DBG_X86
    typedef unsigned int uint_t;
#endif

#ifdef DEBUG_PERF
static struct perflog
{
    struct timeval tv;
    const char* desc;
    int line;
} pl[80];

static int pl_cur=0;

static void perf_reset()
{
    pl_cur=0;
}

static void perf_log(const char*desc, int line)
{
  gettimeofday(&pl[pl_cur].tv, NULL);
  pl[pl_cur].desc = desc;
  pl[pl_cur].line = line;
  pl_cur++;
}

static int tv_diff_us(struct timeval *a, struct timeval *b)
{
    return 1000 * 1000 * (a->tv_sec - b->tv_sec) + (a->tv_usec - b->tv_usec);
}

static void perf_dump()
{
    int i;
    struct timeval tv_base;

    if (pl_cur <= 1) {
        return;
    }

    tv_base = pl[0].tv;

    for (i = 0 ; i < pl_cur ; i++) {
        int ts = tv_diff_us(&pl[i].tv, &tv_base);
        printf("PERF_LOG: %-4d us: %s (line %d)\n", ts, pl[i].desc, pl[i].line);
    }
}
#define PERF_LOG(__desc) do { perf_log(__desc, __LINE__); } while(0)
#else
static void perf_reset() {}
static void perf_dump() {}
#define PERF_LOG(__desc)
#endif


static int log_seq=1;
#define MAX_DATALOG 16
static struct datalog
{
    FILE* f;
    OMX_HANDLETYPE comp;
    OMX_U32 port;
} datalog[MAX_DATALOG];


static void adil_log_data(OMX_HANDLETYPE comp, OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32 port, int is_input)
{
    if (!ste_adm_debug_is_log_enabled(STE_ADM_LOG_ADIL_DUMP)) {
        return;
    }

    int i;
    for (i = 0 ; i < MAX_DATALOG ; i++) {
        if (datalog[i].comp == comp && datalog[i].port == port)
            break;
    }

    if (i == MAX_DATALOG) {
        for (i = 0 ; i < MAX_DATALOG ; i++) {
            if (!datalog[i].comp) {
                char name[80];
                char dirName[80];
                sprintf(dirName, "/sdcard/adil");
                if (mkdir(dirName, 0777) != 0 && errno != EEXIST) {
                    sprintf(dirName, "/mnt/external_sd/adil");
                    if (mkdir(dirName, 0777) != 0 && errno != EEXIST) {
                        sprintf(dirName, "/data/adil");
                        if (mkdir(dirName, 0777) != 0 && errno != EEXIST) {
                            ALOG_WARN("Failed to create directory /data/adil, err %s\n", strerror(errno));
                            break;
                        }
                    }
                }
                sprintf(name, "%s/dump-%d-%X.%d-%s.pcm",dirName, log_seq++, (int) comp, (int) port, is_input ? "in":"out");
                datalog[i].f = fopen(name, "wb");
                if (!datalog[i].f) {
                    ALOG_WARN("Failed to open file %s, errno=%s\n", name, strerror(errno));
                } else {
                    ALOG_ADIL_DUMP("Audiodump: Saving data to file %s\n", name);
                }

                // Keep them to avoid trying to open over and over in this case..
                datalog[i].comp = comp;
                datalog[i].port = port;
                break;
            }
        }
        if (i == MAX_DATALOG) {
            ALOG_WARN("Debug: Too many open audio dump files\n");
        }
    }

    if (datalog[i].f) {
        if (pBuffer->nFilledLen && !fwrite(pBuffer->pBuffer, 1, pBuffer->nFilledLen, datalog[i].f)) {
            ALOG_WARN("Audiodump: fwrite() failed while dumping data for %X.%u\n", datalog[i].comp, datalog[i].port);
            fclose(datalog[i].f);
            datalog[i].f = 0;
        }
    }
}


static void adil_log_data_close(OMX_HANDLETYPE comp, OMX_U32 port)
{
    if (!ste_adm_debug_is_log_enabled(STE_ADM_LOG_ADIL_DUMP)) {
        return;
    }

    int i;
    for (i = 0 ; i < MAX_DATALOG ; i++) {
        if (datalog[i].comp == comp && ((port == 0xFFFFFFFF || port == datalog[i].port))) {
            if (datalog[i].f) {
                ALOG_ADIL_DUMP("Audiodump: Stopped logging data on %X.%u (%d bytes)\n", (int) comp, datalog[i].port, ftell(datalog[i].f));
                fclose(datalog[i].f);
            }
            datalog[i].comp = 0;
        }
    }
}



// Recursion (e.g. comp A is ADIL, comp B is other IL. Comp A calls comp B which calls
// comp A which calls comp B ...) works (the call from ADIL is done when the component
// mutex is not held), but call stack growth is not handled. This is not a problem in the ADM,
// as both AFM and ADM has searate queues for callbacks, and so we do not end up with recursion.


// ADM: The ADM does not correctly wait for buffers to return to ADM when
// API.drain() is not called

// FUTURE IMPROVEMENT: Enabled but unused (unconnected) port? Introduce unused concept? check mixer, etc


// #define ALOG_ADIL_INT(...)

#define MIN(__x, __y) ((__x)<(__y) ? (__x) : (__y))
#define MAX(__a, __b) ((__a)>(__b) ? (__a) : (__b))

#ifdef assert
  #undef assert
#endif
#define assert(__X) \
    do { if (!(__X)) \
    {printf("ASSERT: (" #__X ") %s %d\n", __FILE__, __LINE__); int*d=0; *d=0;}} while(0)

#ifndef ADM_DBG_X86
    // Assert that the given argument looks reasonably like a pointer (within the range
    // allowed for user space pointers).
    #define assert_ptr(__X) \
        do { if (!(((unsigned int)(__X)) < 0xC0000000 && ((unsigned int)(__X)) >= 0x8000)) \
        {printf("ASSERT PTR: (" #__X ") %s %d\n", __FILE__, __LINE__); int*d=0; *d=0;}} while(0)
#else
    #define assert_ptr(__X) \
        do { if (!(((unsigned int)(__X)) >= 0x8000)) \
        {printf("ASSERT PTR: (" #__X ") %s %d\n", __FILE__, __LINE__); int*d=0; *d=0;}} while(0)
#endif

static pthread_mutex_t mutex;
static pthread_mutex_t mutex_outcmd;


#define DBG_BUFSIZE 160
static int g_dbg_indent = 0;
static char g_dbg_buf[DBG_BUFSIZE];
static void dbg_printf(const char* fmt, ...)
{
    int curpos = strlen(g_dbg_buf);

    va_list ap;
    va_start(ap, fmt);
    curpos += vsnprintf(g_dbg_buf + curpos, DBG_BUFSIZE - curpos, fmt, ap);
    va_end(ap);

    if (curpos > 0 && g_dbg_buf[curpos-1] == '\n') {
        ALOG_WARN(g_dbg_buf);
        g_dbg_buf[0] = 0;
    }
}

static void dbg_indent(void)
{
    dbg_printf("%*s", g_dbg_indent, "");
}

// must not exceed number of bits in int
#define MAX_PORTS 32
#define MAX_COMPS 32


#define INIT_CONFIG_STRUCT(__il_config_struct) \
        memset(&__il_config_struct, 0, sizeof(__il_config_struct)); \
        __il_config_struct.nSize             = sizeof(__il_config_struct); \
        __il_config_struct.nVersion.nVersion = OMX_VERSION;

/**
* Macro for basic parameters in set/get parameter/config
* PARAMETER_WITH_PORT_COMMON_CODE(__type).
*
* Intended to be used within case statement in a switch block.
* Executes a 'break' statement if an error occurs.
*
*  Defines a variable 'param' of type '__type'.
*  Checks that the port number is valid
*  Checks that the size of the structure is correct
*  Checks that the component and port is in correct state
*
* Preconditions - defined variables:
*    isParam    - set if parameters is related to OMX_SetParameter/OMX_GetParameter,
*                 not set if parameter is related to OMX_SetConfig/OMX_GetConfig
*    isSet      - true if OMX_Setparameter is called, false if OMX_GetParameter is called
*
*    comp       - IL component, of type comp_t*
*    res        - error code, of type OMX_ERRORTYPE. Will be set on error.
*    pParamRaw  - the un-typed (void*) pointer to the parameter structure
*
* Results - defined variables:
*    pParam       - of type __type* (e.g. OMX_AUDIO_PARAM_PCMMODETYPE*)
*    port         - the affected port
*
*/
#define ONSETGET_PARAMETER_WITH_PORT_COMMON_CODE(__type) \
        __type* pParam = (__type*) pParamRaw; \
        if (!isParam) { \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        if (pParam->nSize != sizeof(__type)) { \
            ALOG_ERR("Size of param/config wrong: %d != %d\n", pParam->nSize, sizeof(__type)); \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        if (pParam->nPortIndex >= comp->ports) { \
            ALOG_ERR("%X: Invalid port number, %d >= %d\n", (int) comp, pParam->nPortIndex, comp->ports); \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        port_t* port = &comp->port[pParam->nPortIndex]; \
        if (isSet && comp->state != OMX_StateLoaded && port->isEnabled) { \
            ALOG_ERR("%X: Invalid state operation. Comp is %s, port is %s\n", (int) comp, adm_log_state2str(comp->state), port->isEnabled ? "Enabled" : "Disabled"); \
            res = OMX_ErrorIncorrectStateOperation; \
            break; \
        }


#define ONSETGET_PARAMETER_COMMON_CODE(__type) \
        __type* pParam = (__type*) pParamRaw; \
        if (!isParam) { \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        if (pParam->nSize != sizeof(__type)) { \
            ALOG_ERR("Size of param/config wrong: %d != %d\n", pParam->nSize, sizeof(__type)); \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        if (isSet && comp->state != OMX_StateLoaded) { \
            ALOG_ERR("%X: Invalid state operation. Comp is %s\n", (int) comp, adm_log_state2str(comp->state)); \
            res = OMX_ErrorIncorrectStateOperation; \
            break; \
        }

#define ONSETGET_CONFIG_WITH_PORT_COMMON_CODE(__type) \
        __type* pParam = (__type*) pParamRaw; \
        if (pParam->nSize != sizeof(__type)) { \
            ALOG_ERR("Size of param/config wrong: %d != %d\n", pParam->nSize, sizeof(__type)); \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        if (pParam->nPortIndex >= comp->ports) { \
            ALOG_ERR("%X: Invalid port number, %d >= %d\n", (int) comp, pParam->nPortIndex, comp->ports); \
            res = OMX_ErrorBadParameter; \
            break; \
        } \
        port_t* port = &comp->port[pParam->nPortIndex];



static pthread_t g_dbg_dumpthread;
static int g_dbg_dumpthread_alive;
static int g_dbg_dumpthread_dumpdog;


typedef struct
{
    unsigned freq     : 16;
    unsigned chan_cfg : 8;
} buf_format_t;

static int buf_format_same(buf_format_t a, buf_format_t b) {
    return (a.chan_cfg == b.chan_cfg) && (a.freq == b.freq);
}

typedef struct buf
{
    buf_format_t format;
    char* data;
    unsigned int bytes_valid;
    unsigned int bytes_available;
    int eos;
    int refcnt;
    void (*dtor)(struct buf* buf);
    struct comp* comp;
    void* dtor_param2;
    struct CompBuf* dtor_param_compbuf;
    struct comp* death_trackers[MAX_COMPS];
    uint_t num_death_trackers;
    LIST_ENTRY(buf) buf_list_node;
} buf_t;

LIST_HEAD(buf_list_head_s, buf);
static struct buf_list_head_s g_buf_list_head;

static void Buf_Release(buf_t*);
static void Buf_Dump(buf_t*);
static void buf_dtor_FillThisBuffer(buf_t* buf);
static void buf_dtor_EmptyBufferDone(buf_t* buf);
static void buf_dtor_CompBuf(buf_t* buf);
static void buf_dtor_DetachedBuffer(buf_t* buf);

typedef struct omx_buf
{
    unsigned int busy_in_peer : 1;      // Peer is tunneled component, or IL client
    unsigned int allocated : 1;         // The buffer is allocated
    unsigned int allocated_by_us : 1;   // ADIL allocated buffer, so it should free it.
    unsigned int to_be_returned : 1;    // Buffer queued to be returned
    OMX_BUFFERHEADERTYPE* bh;
    buf_t* shared_buf;                  // Buffer header that was shared by overriding bh->pBuffer
    char* original_buf;                 // Original value of bh->pBuffer.
    buf_t* adil_buf;
} omx_buf_t;


#define BUFHDRPOOL_MAX (128)
#define MAX_OMX_BUF 4
#define BUFQUEUE_MAX (16)

typedef struct
{
    buf_t* queue[BUFQUEUE_MAX];
    unsigned int num;
    unsigned int read_idx;
} BufQueue_t;

typedef struct CompBuf
{
  struct comp* comp;
  BufQueue_t busy;
  uint_t size;
  uint_t count;
  buf_t* next;
} CompBuf_t;

typedef enum {
    IL_METHOD_NONE,
    IL_METHOD_COPY,
    IL_METHOD_SRC
} il_method_t;

typedef struct
{
    omx_buf_t* queue[MAX_OMX_BUF];
    unsigned int num;
    unsigned int read_idx;
} OMXBufQueue_t;

typedef struct port
{
    unsigned int isDirect : 1;     // Direct connection (to another ADIL component)
    unsigned int isSupplier : 1;
    unsigned int target_state : 1; // 0=disabled 1=enabled
    unsigned int isEnabled : 1;
    unsigned int isEos : 1;
    unsigned int isInput : 1;
    unsigned int silenceOnUnderrun : 1; // Generate silence when all buffers are in peer
    unsigned int isStarted : 1; // Has received first buffer
    OMX_HANDLETYPE peer;
    uint_t peerport;
    OMX_AUDIO_PARAM_PCMMODETYPE omx_format;
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    OMX_PARAM_PORTDEFINITIONTYPE peer_portdef;
    buf_format_t format;
    BufQueue_t bq;

    struct comp* comp; // Back-reference to owning component
    uint_t       idx;  // Index of the port

    unsigned int num_omx_buf; // TODO port->num_omx_buf <=> port->portdef.nBufferCountActual, almost duplicate state? ENS uses nBufferCountActual as current count...
    omx_buf_t omx_buf[MAX_OMX_BUF];
    OMXBufQueue_t omx_buf_queue;
    uint_t startup_margin;

    CompBuf_t zerobuf;
    uint64_t underrun_time; // Time in ms that the port got underrun

    // Data only used by parts that push data out of
    // the IL output port
    //
    SRC_QUALITY_t SRC_mode;
    char* SRC_input_buf;

    // Doing SRC against DSP buffer kills performance, since it is write-through.
    // Until that is changed, use a intermediate buffer.
    char* SRC_output_buf;
    char* SRC_heap;
    uint_t SRC_input_buf_size;
    uint_t SRC_input_buf_bytes_valid;
    uint_t SRC_block_size;
    ResampleContext SRC_ctx;
    int SRC_Ready;

    il_method_t il_method;
    buf_format_t il_last_format;
    int il_src_eos;


    unsigned int nbr_of_buffers_to_use;
    unsigned int buffer_fill_time;

    buf_t* cur_src_buf;
    omx_buf_t* cur_dst_buf;
    unsigned int cur_src_buf_offset;
#ifdef ADM_MMPROBE
    int pcm_probe_enabled;
    int pcm_probe_id;
#endif // ADM_MMPROBE
} port_t;

typedef struct comp
{
    OMX_COMPONENTTYPE omx;  // Code assumes this to be at beginning of struct
    OMX_CALLBACKTYPE  cb;
    port_t port[MAX_PORTS];
    unsigned int ports;
    OMX_STATETYPE state;
    OMX_STATETYPE target_state;
    unsigned int dirty_bit;
    int track_outstanding_buffers;

    // buffers that have passed through out port of this component,
    // but not been released.
    uint_t outstanding_buffers;


    void (*process)(struct comp*);
    void (*dump)(struct comp*);
    void (*onDeactivated)(struct comp*, OMX_U32 port);
    void (*onActivated)(struct comp*, OMX_U32 port);
    OMX_ERRORTYPE (*onSetGet)(struct comp* comp, ADM_CORE_INDEXTYPE nIndex, OMX_PTR pParam, int isSet, int isParam);
    void (*dtor)(struct comp*);

    char dbg_name[80];
} comp_t;


struct out_cmd_item
{
    union {
        struct {
            OMX_HANDLETYPE hComponent;
            OMX_U32 nPortIndex;
            OMX_BUFFERHEADERTYPE* pBuffer;
        } freebuffer;
        struct {
            OMX_HANDLETYPE hComponent;
            OMX_U32 nPortIndex;
            OMX_PTR pAppPrivate;
            OMX_U32 nSizeBytes;
            OMX_U8* pBuffer;
            comp_t* parent_comp;
            uint_t  buf_index;
            OMX_U32 parent_port_index;
        } usebuffer;
        struct {
            OMX_HANDLETYPE hComponent;
            OMX_PTR pAppData;
            OMX_BUFFERHEADERTYPE* pBuffer;
        } buf;
    } param;
    enum {
        CMD_FREE_BUFFER,
        CMD_USE_BUFFER,
        CMD_EMPTY_THIS_BUFFER,
        CMD_FILL_THIS_BUFFER,
    } cmd;
};


static int isActive(const port_t* port)
{
    if (port->isInput && !port->isStarted) {
        return 0;
    }

    if (port->isDirect) {
        return port->isEnabled && ((comp_t*)port->peer)->port[port->peerport].isEnabled;
    } else {
        return port->isEnabled;
    }
}
#ifdef ADM_MMPROBE
static void omx_format2mmprobe_format(OMX_AUDIO_PARAM_PCMMODETYPE omx_format, DataFormat_t *mmprobe_format)
{
    switch (omx_format.nSamplingRate) {
    case 48000: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_48KHZ; break;
    case 44100: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_44_1KHZ; break;
    case 32000: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_32KHZ; break;
    case 24000: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_24KHZ; break;
    case 22050: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_22_05KHZ; break;
    case 16000: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_16KHZ; break;
    case 12000: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_12KHZ; break;
    case 11025: mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_11_025KHZ; break;
    case 8000:  mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_8KHZ; break;
    default:
        ALOG_ERR("Unsupported sample rate, %d", (int)omx_format.nSamplingRate);
        mmprobe_format->SampleRate = MM_PROBE_SAMPLE_RATE_UNKNOWN;
        break;
    }

    mmprobe_format->NoChannels = (uint8_t)omx_format.nChannels;
    mmprobe_format->Interleave = omx_format.bInterleaved ? MM_PROBE_INTERLEAVED : MM_PROBE_INTERLEAVED_NONE;

    switch (omx_format.nBitPerSample) {
    case 16:
        if (omx_format.eNumData == OMX_NumericalDataSigned && omx_format.eEndian == OMX_EndianBig) {
            mmprobe_format->Resolution = MM_PROBE_FORMAT_S16_BE;
        } else if (omx_format.eNumData == OMX_NumericalDataSigned && omx_format.eEndian == OMX_EndianLittle) {
            mmprobe_format->Resolution = MM_PROBE_FORMAT_S16_LE;
        } else if (omx_format.eNumData == OMX_NumericalDataUnsigned && omx_format.eEndian == OMX_EndianBig) {
            mmprobe_format->Resolution = MM_PROBE_FORMAT_U16_BE;
        } else {
            mmprobe_format->Resolution = MM_PROBE_FORMAT_U16_LE;
        }
        break;
    default:
        ALOG_ERR("Unsupported format, nBitPerSample = %d", (int)omx_format.nBitPerSample);
        mmprobe_format->Resolution = MM_PROBE_PCM_FORMAT_UNKNOWN;
    }
}
#endif // ADM_MMPROBE

///////////////////////////////////////////////////////////////////////////////
//
// Needed declarations of file scope functions
//
///////////////////////////////////////////////////////////////////////////////
static OMX_ERRORTYPE Mixer_Factory(comp_t** comp_pp, const char* name);
static OMX_ERRORTYPE Splitter_Factory(comp_t** comp_pp, const char* name);
#ifdef ADM_ENABLE_FEATURE_FAT_MODEM
static OMX_ERRORTYPE modem_src_Factory(comp_t** comp_pp, const char* name);
static OMX_ERRORTYPE modem_sink_Factory(comp_t** comp_pp, const char* name);
#endif

static OMX_ERRORTYPE allocate_single_buffer(OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes, OMX_IN OMX_U8* pBuffer, int allocated_by_us);

static void queueEmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
static void queueFillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer);
static void queueUseBuffer(comp_t* parent_comp, OMX_U32 parent_port_index, uint_t bufidx,
            OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex, OMX_PTR pAppPrivate,
            OMX_U32 nSizeBytes, OMX_U8* pBuffer);
static void queueFreeBuffer(
            OMX_HANDLETYPE hComponent,
            OMX_U32 nPortIndex,
            OMX_BUFFERHEADERTYPE* pBuffer);
static void queueExecute();

static void Comp_SetDirty(comp_t* comp);
static uint_t Buf_GetDurationQ8(buf_t* buf);

static OMX_ERRORTYPE Comp_onSetGet(comp_t* comp, OMX_INDEXTYPE nIndex, OMX_PTR pParam, int isSet, int isParam);

static void ste_adm_omx_core_dump_locked(void);
static void IL_Core_Process();



///////////////////////////////////////////////////////////////////////////////
//
// File-scope variables
//
///////////////////////////////////////////////////////////////////////////////
static comp_t* comps[MAX_COMPS];
static unsigned int dirty_comp_mask = 0;
static uint_t bufhdrpool_cur = 0;
static buf_t* bufhdrpool[BUFHDRPOOL_MAX];
static buf_t prealloc_bufhdr[BUFHDRPOOL_MAX];

static pthread_mutex_t mutex_bufpool;


#define ADM_FIFO_SIZE 256
static struct
{
    unsigned int items_valid;
    unsigned int read_pos;
    struct out_cmd_item fifo[ADM_FIFO_SIZE];
} adm_fifo;


///////////////////////////////////////////////////////////////////////////////
//
// Timer handling
//
///////////////////////////////////////////////////////////////////////////////
#define TIME_BEFORE_SILENCE 20

static pthread_t g_timer_thread;
static int g_timer_thread_alive;

typedef struct timer_entry_s
{
    comp_t*  comp;
    uint64_t time;
    LIST_ENTRY(timer_entry_s) list_entry;
} timer_entry_t;

LIST_HEAD(timer_list_s, timer_entry_s);
static struct timer_list_s g_timer_event_list;
static int g_timer_wakeup_fd;



static uint64_t get_time_of_day_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return ((uint64_t) tv.tv_sec) * 1000 + tv.tv_usec/1000;
}


static void subscribe_timer_event(comp_t *comp, uint64_t time)
{
    ALOG_ADIL_INT("ADIL: subscribe_timer_event comp %X, at %llu", (int) comp, time);

    timer_entry_t *entry = malloc(sizeof(timer_entry_t));
    if (entry == NULL)
    {
        ALOG_ERR("ADIL: set_timer_event failed to allocate memory");
        return;
    }

    entry->comp = comp;
    entry->time = time;
    LIST_INSERT_HEAD(&g_timer_event_list, entry, list_entry);

    const unsigned long long val = 1;
    write(g_timer_wakeup_fd, &val, sizeof(val));
}


static void unsubscribe_timer_event(comp_t *comp)
{
    ALOG_ADIL_INT("ADIL: unsubscribe_timer_event comp %X", (int) comp);

    timer_entry_t* temp;
    timer_entry_t* cur;
    LIST_FOREACH_SAFE(cur, &g_timer_event_list, list_entry, temp) {
        if (cur->comp == comp) {
            LIST_REMOVE(cur, list_entry);
            free(cur);
            cur = NULL;
        }
    }

    if (!LIST_EMPTY(&g_timer_event_list)) {
        const unsigned long long val = 1;
        write(g_timer_wakeup_fd, &val, sizeof(val));
    }
}


static void* timer_thread(void *dummy)
{
    (void) dummy;
    int time_to_wait = -1;
    struct pollfd pollfd;

    pollfd.fd = g_timer_wakeup_fd;
    pollfd.events = POLLIN;

    while(g_timer_thread_alive) {
        int ret = poll(&pollfd, 1, time_to_wait);
        if (!g_timer_thread_alive) {
            ALOG_ADIL_INT("ADIL: timer_thread exiting");
            return NULL;
        }

        if (ret < 0) {
            ALOG_ERR("ADIL: timer_thread: poll returned error, errno = %d (%s), revents = %x", errno, strerror(errno), pollfd.revents);
            continue;
        }
        else if (ret > 0 && !(pollfd.revents & POLLIN))
        {
            ALOG_ERR("ADIL: timer_thread: poll returned POLLIN is not set, revents = %x", pollfd.revents);
        }
        else if (ret != 0) {
            unsigned long long tmp;
            read(g_timer_wakeup_fd, &tmp, sizeof(tmp));
        }

        pthread_mutex_lock(&mutex);

        uint64_t now = get_time_of_day_ms();
        ALOG_ADIL_INT("ADIL: timer_thread woke up from %s at %llu, revents=%x", ret==0?"timeout":"signal", now, pollfd.revents);



        timer_entry_t* temp;
        timer_entry_t* cur;
        uint64_t next_event_time = UINT64_MAX;
        LIST_FOREACH_SAFE(cur, &g_timer_event_list, list_entry, temp) {

            if (cur->time <= now) {
                LIST_REMOVE(cur, list_entry);
                ALOG_ADIL_INT("ADIL: timer_thread waking component %X at %llu", cur->comp, now);
                Comp_SetDirty(cur->comp);
                free(cur);
                cur = NULL;
            }
            else {
                if (cur->time < next_event_time) {
                    next_event_time = cur->time;
                }
            }

        }

        if (next_event_time == UINT64_MAX) {
            time_to_wait = -1;
            ALOG_ADIL_INT("ADIL: timer_thread no more components in list");
        }
        else {
            time_to_wait = next_event_time - now;
            ALOG_ADIL_INT("ADIL: timer_thread sleeping for %d ms at %llu", time_to_wait, now);
        }

        IL_Core_Process();
        pthread_mutex_unlock(&mutex);
        queueExecute();
    }

    return NULL;
}




///////////////////////////////////////////////////////////////////////////////
//
// BufQueue
//
///////////////////////////////////////////////////////////////////////////////
static void BufQueue_Init(BufQueue_t* this)
{
    this->num = 0;
    this->read_idx = 0;
}


static void BufQueue_Push(BufQueue_t* this, buf_t* buf)
{
    assert(buf->refcnt >= 0);
    assert(buf->bytes_valid == 0 || buf->data);
    assert(buf->bytes_valid <= buf->bytes_available);
    assert(this->num < BUFQUEUE_MAX);
    unsigned int write_idx = (this->read_idx + this->num) % BUFQUEUE_MAX;
    this->queue[write_idx] = buf;
    this->num++;
}


static buf_t* BufQueue_Peek(BufQueue_t* this)
{
    if (this->num == 0) {
        return 0;
    }

    buf_t* buf = this->queue[this->read_idx];
    assert(buf->bytes_valid <= buf->bytes_available);

    return buf;
}


static void BufQueue_Drop(BufQueue_t* this)
{
    assert(this->num > 0);
    this->read_idx = (this->read_idx + 1) % BUFQUEUE_MAX;
    this->num--;
}


static buf_t* BufQueue_Pop(BufQueue_t* this)
{
    buf_t* buf = BufQueue_Peek(this);
    if (buf) BufQueue_Drop(this);

    return buf;
}

static void BufQueue_ReleaseBuffers(BufQueue_t* this)
{
    buf_t* buf;
    while((buf = BufQueue_Pop(this))) {
        Buf_Release(buf);
    }
}

static uint_t BufQueue_GetCount(BufQueue_t* this)
{
    return this->num;
}

static buf_t* BufQueue_PeekIdx(BufQueue_t* this, uint_t external_idx)
{
    assert (external_idx < this->num);
    uint_t internal_idx = (this->read_idx + external_idx) % BUFQUEUE_MAX;
    buf_t* buf = this->queue[internal_idx];
    assert_ptr(buf);

    return buf;
}

static void BufQueue_UnorderedRemove(BufQueue_t* this, buf_t* buf)
{
    uint_t i;
    for (i = 0 ; i < this->num ; i++) {
        uint_t idx = (this->read_idx + i) % BUFQUEUE_MAX;
        if (this->queue[idx] == buf) {
            if (i == 0) {
                // first element, also handles this->num == 1
                this->read_idx = (this->read_idx + 1) % BUFQUEUE_MAX;
            } else if (i == this->num-1) {
                // last element, just remove it
            } else {
                // in the middle. Swap in first element
                this->queue[idx] = this->queue[this->read_idx];
                this->read_idx = (this->read_idx + 1) % BUFQUEUE_MAX;
            }
            this->num--;
            return;
        }
    }

    assert(0);
}

static uint_t BufQueue_GetBufferedTime(BufQueue_t* this)
{
    uint_t buffered_timeQ8 = 0;
    uint_t i;

    for (i = 0 ; i < this->num ; i++) {
        buf_t* buf = BufQueue_PeekIdx(this, i);
        buffered_timeQ8 += Buf_GetDurationQ8(buf);
    }

    return buffered_timeQ8 / 256;
}


static void BufQueue_Dump(BufQueue_t* this)
{
    unsigned int i;

    for (i = 0 ; i < this->num ; i++) {
        buf_t* buf = BufQueue_PeekIdx(this, i);
        assert(buf);
        dbg_indent(); dbg_printf("Buf %d: ", i);
        int tmp = g_dbg_indent;
        g_dbg_indent = 0;
        Buf_Dump(buf);
        g_dbg_indent = tmp;
    }
}


static void CompBuf_Destroy(CompBuf_t* this);

///////////////////////////////////////////////////////////////////////////////
//
// OMXBufQueue
//
///////////////////////////////////////////////////////////////////////////////
static void OMXBufQueue_Init(OMXBufQueue_t* this)
{
    this->num = 0;
    this->read_idx = 0;
}


static void OMXBufQueue_Push(OMXBufQueue_t* this, omx_buf_t* buf)
{
    assert(this->num < MAX_OMX_BUF);
    unsigned int write_idx = (this->read_idx + this->num) % MAX_OMX_BUF;
    this->queue[write_idx] = buf;
    this->num++;
}


static omx_buf_t* OMXBufQueue_Peek(OMXBufQueue_t* this)
{
    if (this->num == 0) {
        return 0;
    }

    omx_buf_t* buf = this->queue[this->read_idx];

    return buf;
}


static void OMXBufQueue_Drop(OMXBufQueue_t* this)
{
    assert(this->num > 0);
    this->read_idx = (this->read_idx + 1) % MAX_OMX_BUF;
    this->num--;
}


static omx_buf_t* OMXBufQueue_Pop(OMXBufQueue_t* this)
{
    omx_buf_t* buf = OMXBufQueue_Peek(this);
    if (buf) OMXBufQueue_Drop(this);

    return buf;
}


static unsigned int OMXBufQueue_GetCount(OMXBufQueue_t* this)
{
    return this->num;
}




///////////////////////////////////////////////////////////////////////////////
//
// Port
//
///////////////////////////////////////////////////////////////////////////////
static void Port_Push(port_t* port, buf_t* buf);



static void Port_Dump(port_t* port)
{
    if (!port->isInput && port->isEnabled && !port->isDirect) {
        dbg_indent(); dbg_printf("SRC_block_size=%d SRC_input_buf_bytes_valid=%d cur_src_buf_offset=%d\n", port->SRC_block_size, port->SRC_input_buf_bytes_valid, port->cur_src_buf_offset);
        if (port->cur_src_buf) {
            dbg_indent(); dbg_printf("cur_src_buf\n");
            g_dbg_indent += 2;
            Buf_Dump(port->cur_src_buf);
            g_dbg_indent -= 2;
        } else {
            dbg_indent(); dbg_printf("(has no cur_src_buf)\n");
        }
    }
}


static void Port_CheckForStartedState(port_t* port)
{
    assert(!port->isStarted);
    assert(!isActive(port));
    assert(port->isInput);

    if (port->startup_margin == 0) {
        if (BufQueue_GetCount(&port->bq) > 0) {
            port->isStarted = 1;
            Comp_SetDirty(port->comp);
        }
    } else {
        if (BufQueue_GetBufferedTime(&port->bq) >= port->startup_margin) {
            ALOG_ADIL("%X [%s] Satisfied startup margin criteria (%u >= %u ms), now starting\n", (int) port->comp, port->comp->dbg_name, BufQueue_GetBufferedTime(&port->bq), port->startup_margin);
            port->isStarted = 1;
            Comp_SetDirty(port->comp);
        } else {
            ALOG_ADIL_INT("%X [%s] Only has %u ms data, need %u. Not yet starting.\n", (int) port->comp, port->comp->dbg_name, BufQueue_GetBufferedTime(&port->bq), port->startup_margin);
        }
    }

}


static void Port_PutBuf(port_t* port, buf_t* buf)
{
    assert(buf->refcnt >= 0);
    BufQueue_Push(&port->bq, buf);
    if (port->isInput && !port->isStarted) {
        Port_CheckForStartedState(port);
    }
}


static buf_t* Port_PeekBuf(port_t* port)
{
    return BufQueue_Peek(&port->bq);
}


static buf_t* Port_PopBuf(port_t* port)
{
    return BufQueue_Pop(&port->bq);
}


static void Port_ReleaseBuffers(port_t* port)
{
    // Release all buffers on the port
    buf_t* buf;
    while ((buf = Port_PopBuf(port))) {
        ALOG_ADIL_INT("Port_ReleaseBuffers - releasing output port buffer %X\n", (int) buf);
        Buf_Release(buf);
    }
    if (port->cur_src_buf) {
        ALOG_ADIL_INT("Port_ReleaseBuffers - releasing port->cur_src_buf buffer %X\n", (int) port->cur_src_buf);
        Buf_Release(port->cur_src_buf);
        port->cur_src_buf = 0;
    }
}


// Need to keep track of order buffers are given by client, so we
// can give them back in same order. Otherwise client gets confused.
// Thus, sometimes we can't return a buffer because we're not finished
// with the buffer the client gave us before the buffer we want to return.
// In practice, this can only happen when the graph is torn down in the
// middle while data flow is active.
static void Port_queueReturnBuffer(port_t* port_p, OMX_BUFFERHEADERTYPE* pBuffer)
{
    omx_buf_t* cur_omx_buf;
    if (port_p->isInput) {
        cur_omx_buf = &port_p->omx_buf[(OMX_U32) pBuffer->pInputPortPrivate];
    } else {
        cur_omx_buf = &port_p->omx_buf[(OMX_U32) pBuffer->pOutputPortPrivate];
    }

    assert(!cur_omx_buf->to_be_returned);
    assert(!cur_omx_buf->busy_in_peer);
    assert_ptr(OMXBufQueue_Peek(&port_p->omx_buf_queue));
    cur_omx_buf->to_be_returned = 1;

    if (!(OMXBufQueue_Peek(&port_p->omx_buf_queue)->to_be_returned)) {
        ALOG_ADIL("Top buffer not marked as to_be_returned; returning this buffer delayed\n");
    }

    omx_buf_t* top;
    while ((top = OMXBufQueue_Peek(&port_p->omx_buf_queue)) && top->to_be_returned) {
        if (port_p->isInput) {
            if (port_p->peer) {
                ALOG_ADIL_FLOW("[OUT] %X.%d [%s] queueFillThisBuffer peer=%X.%d bh=%X nAllocLen=%d\n", (int) port_p->comp, port_p->idx, port_p->comp->dbg_name, port_p->peer, port_p->peerport, top->bh, top->bh->nAllocLen);
                queueFillThisBuffer(port_p->peer, top->bh);
            } else {
                ALOG_ADIL_FLOW("[OUT] %X.%d EmptyBufferDone bh=%X nAllocLen=%d\n", port_p->comp, port_p->idx, top->bh, top->bh->nAllocLen);
                port_p->comp->cb.EmptyBufferDone(&port_p->comp->omx, port_p->comp->omx.pApplicationPrivate, top->bh);
            }
        } else {
            if (port_p->peer) {
                ALOG_ADIL_FLOW("[OUT] %X.%d [%s] queueEmptyThisBuffer peer=%X.%d bh=%X %d/%d %s\n", (int) port_p->comp, port_p->idx, port_p->comp->dbg_name, port_p->peer, port_p->peerport, top->bh, top->bh->nFilledLen, top->bh->nAllocLen, pBuffer->nFlags & OMX_BUFFERFLAG_EOS ? "EOS" : "");
#ifdef ADM_MMPROBE
                // check if port should be probed using mmprobe
                if (port_p->pcm_probe_enabled) {
                    DataFormat_t data_format;
                    omx_format2mmprobe_format(port_p->omx_format, &data_format);
                    ALOG_ADIL_FLOW("probe pcm data, probe_id=%d, data=%d, bytes=%d, format=%d,%d,%d,%d",
                        port_p->pcm_probe_id, top->bh->pBuffer, top->bh->nFilledLen,
                        data_format.SampleRate, data_format.NoChannels,
                        data_format.Interleave, data_format.Resolution);
                    mmprobe_probe_V2(port_p->pcm_probe_id, top->bh->pBuffer, top->bh->nFilledLen, &data_format);
                }
#endif // ADM_MMPROBE
                adil_log_data((OMX_HANDLETYPE) port_p->comp, top->bh, port_p->idx, 0);
                queueEmptyThisBuffer(port_p->peer, top->bh);
            } else {
                ALOG_ADIL_FLOW("[OUT] %X.%d [%s] FillBufferDone bh=%X %d/%d\n", (int) port_p->comp, port_p->idx, port_p->comp->dbg_name, top->bh, top->bh->nFilledLen, top->bh->nAllocLen);
#ifdef ADM_MMPROBE
                // check if port should be probed using mmprobe
                if (port_p->pcm_probe_enabled) {
                    DataFormat_t data_format;
                    omx_format2mmprobe_format(port_p->omx_format, &data_format);
                    ALOG_ADIL_FLOW("probe pcm data, probe_id=%d, data=%d, bytes=%d, format=%d,%d,%d,%d",
                        port_p->pcm_probe_id, top->bh->pBuffer, top->bh->nFilledLen,
                        data_format.SampleRate, data_format.NoChannels,
                        data_format.Interleave, data_format.Resolution);
                    mmprobe_probe_V2(port_p->pcm_probe_id, top->bh->pBuffer, top->bh->nFilledLen, &data_format);
                }
#endif // ADM_MMPROBE
                adil_log_data((OMX_HANDLETYPE) port_p->comp, top->bh, port_p->idx, 0);
                port_p->comp->cb.FillBufferDone(&port_p->comp->omx, port_p->comp->omx.pApplicationPrivate, top->bh);
            }
        }

        top->to_be_returned = 0;
        top->busy_in_peer   = 1;
        OMXBufQueue_Pop(&port_p->omx_buf_queue);
    }
}


static unsigned char hz2esaafreq(uint_t hz)
{
    switch (hz) {
        case 48000: return ESAA_FREQ_48KHZ;
        case 44100: return ESAA_FREQ_44_1KHZ;
        case 32000: return ESAA_FREQ_32KHZ;
        case 24000: return ESAA_FREQ_24KHZ;
        case 22050: return ESAA_FREQ_22_05KHZ;
        case 16000: return ESAA_FREQ_16KHZ;
        case 12000: return ESAA_FREQ_12KHZ;
        case 11025: return ESAA_FREQ_11_025KHZ;
        case 8000:  return ESAA_FREQ_8KHZ;
        default:    return 0xFF;
    }
}


static uint_t esaafreq2hz(uint_t esaa)
{
    switch (esaa) {
        case ESAA_FREQ_48KHZ:     return 48000;
        case ESAA_FREQ_44_1KHZ:   return 44100;
        case ESAA_FREQ_32KHZ:     return 32000;
        case ESAA_FREQ_24KHZ:     return 24000;
        case ESAA_FREQ_22_05KHZ:  return 22050;
        case ESAA_FREQ_16KHZ:     return 16000;
        case ESAA_FREQ_12KHZ:     return 12000;
        case ESAA_FREQ_11_025KHZ: return 11025;
        case ESAA_FREQ_8KHZ:      return 8000;
        default:  assert(0);      return 0;
    }
}


static const char*esaafreq2str(uint_t esaa)
{
    switch (esaa) {
        case ESAA_FREQ_48KHZ:     return "48kHz";
        case ESAA_FREQ_44_1KHZ:   return "44.1kHz";
        case ESAA_FREQ_32KHZ:     return "32kHz";
        case ESAA_FREQ_24KHZ:     return "24kHz";
        case ESAA_FREQ_22_05KHZ:  return "22.05kHz";
        case ESAA_FREQ_16KHZ:     return "16kHz";
        case ESAA_FREQ_12KHZ:     return "12kHz";
        case ESAA_FREQ_11_025KHZ: return "11.025kHz";
        case ESAA_FREQ_8KHZ:      return "8kHz";
        default:                  return "<? Hz>";
    }
}


static int is_hdmi_format(const OMX_AUDIO_PARAM_PCMMODETYPE* mode)
{
    if (mode->nChannels != 6) {
        return 0;
    }

    if (mode->eChannelMapping[0] != OMX_AUDIO_ChannelLF  ||
        mode->eChannelMapping[1] != OMX_AUDIO_ChannelRF  ||
        mode->eChannelMapping[2] != OMX_AUDIO_ChannelLFE ||
        mode->eChannelMapping[3] != OMX_AUDIO_ChannelCF  ||
        mode->eChannelMapping[4] != OMX_AUDIO_ChannelLS  ||
        mode->eChannelMapping[5] != OMX_AUDIO_ChannelRS)
    {
        return 0;
    }

    return 1;
}


static OMX_ERRORTYPE Port_UpdatePcmMode(port_t* port, OMX_AUDIO_PARAM_PCMMODETYPE* pcm_mode)
{
    OMX_ERRORTYPE result = OMX_ErrorNone;

    if (pcm_mode->eNumData != OMX_NumericalDataSigned ||
        pcm_mode->eEndian  != OMX_EndianLittle || pcm_mode->bInterleaved != OMX_TRUE ||
        !(pcm_mode->nBitPerSample == 16 || (!port->isInput && pcm_mode->nBitPerSample == 32)) ||
        pcm_mode->ePCMMode != OMX_AUDIO_PCMModeLinear)
    {
        ALOG_ERR("ADIL: Invalid PCM format\n");
        result = OMX_ErrorBadParameter;
        goto exit;
    }

    if (hz2esaafreq(pcm_mode->nSamplingRate) == 0xFF) {
        ALOG_ERR("ADIL: Invalid sample rate (%d Hz)\n", pcm_mode->nSamplingRate);
        result = OMX_ErrorBadParameter;
        goto exit;
    }

    port->format.freq = hz2esaafreq((uint_t) pcm_mode->nSamplingRate);
    if (pcm_mode->nChannels == 1 && pcm_mode->eChannelMapping[0] == OMX_AUDIO_ChannelCF)
    {
        port->format.chan_cfg = 1;
    } else if (pcm_mode->nChannels == 2 && pcm_mode->eChannelMapping[0] == OMX_AUDIO_ChannelLF &&
        pcm_mode->eChannelMapping[1] == OMX_AUDIO_ChannelRF)
    {
        port->format.chan_cfg = 2;
    }
    else if (pcm_mode->nChannels == 6)
    {
        port->format.chan_cfg = 6;
    } else {
        ALOG_ERR("ADIL: Invalid sample format\n");
        result = OMX_ErrorBadParameter;
        goto exit;
    }

exit:
    return result;
}


static void InplaceMoveChannels(short* buf, uint_t frames, const OMX_AUDIO_PARAM_PCMMODETYPE* format, int input)
{
    // Convert to LF RF LFE CF LS RS
    int map[6]; // map[output] = input index
    memset(map,0,sizeof(map));

    if (input) {
        int in;
        for (in = 0 ; in < 6 ; in++) {
            switch (format->eChannelMapping[in]) {
                case OMX_AUDIO_ChannelLF:  map[0] = in; break;
                case OMX_AUDIO_ChannelRF:  map[1] = in; break;
                case OMX_AUDIO_ChannelLFE: map[2] = in; break;
                case OMX_AUDIO_ChannelCF:  map[3] = in; break;
                case OMX_AUDIO_ChannelLS:  map[4] = in; break;
                case OMX_AUDIO_ChannelRS:  map[5] = in; break;
                default: ALOG_WARN("Unhandled format mapping %d (at pos %d)\n", format->eChannelMapping[in], in); break;
            }
        }
    } else {
        int out;
        for (out = 0 ; out < 6 ; out++) {
            switch (format->eChannelMapping[out]) {
                case OMX_AUDIO_ChannelLF:  map[out] = 0; break;
                case OMX_AUDIO_ChannelRF:  map[out] = 1; break;
                case OMX_AUDIO_ChannelLFE: map[out] = 2; break;
                case OMX_AUDIO_ChannelCF:  map[out] = 3; break;
                case OMX_AUDIO_ChannelLS:  map[out] = 4; break;
                case OMX_AUDIO_ChannelRS:  map[out] = 5; break;
                default: ALOG_WARN("Unhandled format mapping %d (at pos %d)\n", format->eChannelMapping[out], out); break;
            }
        }
    }

    short* cur = buf;
    short* end = buf + frames * 6;
    while (cur != end) {
        short tmp[6];
        int i;
        for (i = 0 ; i<6 ; i++) {
            tmp[i] = cur[map[i]];
        }
        memcpy(cur, tmp, sizeof(tmp));
        cur += 6;
    }
}




///////////////////////////////////////////////////////////////////////////////
//
// Pool of buffer headers
//
///////////////////////////////////////////////////////////////////////////////

/**
* Allocates buffer header. Refcnt is set to 1.
* Initializes the buffer header except for the following
* attributes, which needs to be set by caller:
*  data, bytes_available, format, dtor
*/
static buf_t* BufPool_AllocHdr()
{
    buf_t* buf;
    pthread_mutex_lock(&mutex_bufpool);
    if (bufhdrpool_cur >= BUFHDRPOOL_MAX) {
        ALOG_ERR("Buffer pool exhausted - buffer leak? Limit is %u\n", BUFHDRPOOL_MAX);
        pthread_mutex_unlock(&mutex_bufpool);
        assert(0);
        return 0;
    }
    bufhdrpool[bufhdrpool_cur]->refcnt             = 1;
    bufhdrpool[bufhdrpool_cur]->bytes_valid        = 0;
    bufhdrpool[bufhdrpool_cur]->num_death_trackers = 0;
    LIST_INSERT_HEAD(&g_buf_list_head, bufhdrpool[bufhdrpool_cur], buf_list_node);

    buf = bufhdrpool[bufhdrpool_cur++];
    pthread_mutex_unlock(&mutex_bufpool);
    return buf;
}


static void BufPool_FreeHdr(buf_t* buf)
{
    pthread_mutex_lock(&mutex_bufpool);
    assert(buf->refcnt == 0);
    assert(buf->num_death_trackers == 0);
    assert(bufhdrpool_cur >= 1);
    LIST_REMOVE(buf, buf_list_node);
    bufhdrpool[--bufhdrpool_cur] = buf;
    pthread_mutex_unlock(&mutex_bufpool);
}


static void BufPool_Init(void)
{
    int i;
    for (i = 0 ; i < BUFHDRPOOL_MAX ; i++) {
        bufhdrpool[i] = &prealloc_bufhdr[i];
    }
}


static void BufPool_RemoveComponentReference(comp_t* comp)
{
    buf_t* cur;
    pthread_mutex_lock(&mutex_bufpool);
    LIST_FOREACH(cur, &g_buf_list_head, buf_list_node) {
        uint_t i;
        for (i = 0 ; i < cur->num_death_trackers ; i++) {
            if (cur->death_trackers[i] == comp) {
                ALOG_ADIL_INT("BufPool_RemoveComponentReference: Removed reference to comp %X from buffer %X\n", (int) comp, (int) cur);
                cur->death_trackers[i] = 0;
            }
        }
    }
    pthread_mutex_unlock(&mutex_bufpool);
}


static void BufPool_Dump(void)
{
    buf_t* cur;
    pthread_mutex_lock(&mutex_bufpool);
    LIST_FOREACH(cur, &g_buf_list_head, buf_list_node) {
        Buf_Dump(cur);
    }
    pthread_mutex_unlock(&mutex_bufpool);
}




///////////////////////////////////////////////////////////////////////////////
//
// Component buffer - used when a component needs to output to a new buffer
//
///////////////////////////////////////////////////////////////////////////////
static void buf_dtor_CompBuf(buf_t* buf)
{
    assert_ptr(buf);
    assert_ptr(buf->data);
    assert(buf->refcnt == 0);
    comp_t* comp = buf->comp;

    ALOG_ADIL_INT("buf_dtor_CompBuf for buf %X, comp %X\n", (int) buf, (int) comp);
    free(buf->data);
    if (comp != NULL) {
        BufQueue_UnorderedRemove(&buf->dtor_param_compbuf->busy, buf);
        Comp_SetDirty(comp);
    } else {
        ALOG_ADIL_INT("buf_dtor_CompBuf: orphaned buffer released\n");
    }
}


static void CompBuf_Init(CompBuf_t* this, comp_t* comp, uint_t size, uint_t count)
{
    assert_ptr(comp);
    this->comp  = comp;
    this->size  = size;
    this->count = count;
    this->next  = 0;
    BufQueue_Init(&this->busy);
}


static buf_t* CompBuf_Peek(CompBuf_t* this)
{
    assert_ptr(this);
    assert_ptr(this->comp);

    if (this->next) {
        return this->next;
    }

    if (BufQueue_GetCount(&this->busy) == this->count) {
        return 0;
    }

    char* data = malloc(this->size);
    if (data) {
        this->next = BufPool_AllocHdr();
        this->next->data = data;
        this->next->bytes_available = this->size;
        this->next->bytes_valid = 0;
        this->next->refcnt = 1;
        this->next->eos = 0;
        this->next->comp = this->comp;
        this->next->dtor = buf_dtor_CompBuf;
        this->next->dtor_param_compbuf = this;
    } else {
        ALOG_ERR("CompBuf_Peek - malloc failed, address space exhausted?\n");
    }

    return this->next;
}


static void CompBuf_Dump(CompBuf_t* this)
{
    uint_t i;
    dbg_indent();
    uint_t num = BufQueue_GetCount(&this->busy);

    if (num == 0 && !this->next) {
        dbg_printf("CompBuf unused\n");
    } else {
        dbg_printf("CompBuf ");
    }

    if (this->next) {
        dbg_printf("{ N %X %d/%d ref=%d%s} ", (int) this->next, this->next->bytes_valid, this->next->bytes_available, this->next->refcnt, this->next->comp ? "" : " Orphan");
    }

    for (i = 0 ; i < num ; i++) {
        buf_t* buf = BufQueue_PeekIdx(&this->busy, i);
        dbg_printf("{%X %d/%d %d%s} ", (int) buf, buf->bytes_valid, buf->bytes_available, buf->refcnt, buf->comp ? "" : " Orphan");
    }
    dbg_printf("\n");
}


static buf_t* CompBuf_Pop(CompBuf_t* this)
{
    buf_t* buf = CompBuf_Peek(this);
    assert(buf == this->next);
    if (buf) BufQueue_Push(&this->busy, buf);
    this->next = 0;

    return buf;
}


static void CompBuf_Drop(CompBuf_t* this)
{
    buf_t* buf = CompBuf_Pop(this);
    assert_ptr(buf);
    (void) buf;
}


static void CompBuf_OrphanBuffers(CompBuf_t* this)
{
    uint_t num = BufQueue_GetCount(&this->busy);
    uint_t i;
    for (i = 0 ; i < num ; i++) {
        buf_t* buf = BufQueue_PeekIdx(&this->busy, i);
        ALOG_ADIL_INT("CompBuf_OrphanBuffers buf=%X comp=%X\n", (int) buf, (int) buf->comp);
        buf->comp = 0;
    }
}


static void CompBuf_Destroy(CompBuf_t* this)
{
    buf_t* buf;
    while((buf = BufQueue_Pop(&this->busy))) {
        ALOG_ADIL_INT("CompBuf_Destroy: Orphaning busy buffer %X, prev comp %X\n", (int) buf, (int) buf->comp);
        buf->comp = 0;
    }
    if (this->next) {
        free(this->next->data);
        assert(this->next->refcnt == 1);
        this->next->refcnt = 0;
        BufPool_FreeHdr(this->next);
    }
}




///////////////////////////////////////////////////////////////////////////////
//
// Buf - the internal buffer used inside ADIL
//
///////////////////////////////////////////////////////////////////////////////
static void Buf_UpdateRefcount(buf_t* buf, int delta)
{
    assert(buf->refcnt > 0);
    buf->refcnt += delta;
    assert(buf->refcnt >= 0);
    if (buf->refcnt == 0) {
        uint_t i;
        for (i = 0 ; i < buf->num_death_trackers ; i++) {
            if (buf->death_trackers[i]) {
                ALOG_ADIL_INT("outstanding update refcnt: decreasing ref cnf buf=%X comp=%X outstanding=%d ndt=%d\n", buf, buf->death_trackers[i], buf->death_trackers[i]->outstanding_buffers, buf->num_death_trackers);
                assert(buf->death_trackers[i]->outstanding_buffers > 0);
                buf->death_trackers[i]->outstanding_buffers--;
            }
        }
        buf->num_death_trackers = 0;
        if (buf->dtor) buf->dtor(buf);
        BufPool_FreeHdr(buf);
    }
}


static void Buf_Release(buf_t* buf)
{
    assert(buf);
    assert(buf->refcnt > 0);
    Buf_UpdateRefcount(buf, -1);
}


static void Buf_AddOutstandingTracking(buf_t* buf, comp_t* comp)
{
    assert(buf->num_death_trackers < MAX_COMPS);
    buf->death_trackers[buf->num_death_trackers++] = comp;
    ALOG_ADIL_INT("AddOutstandingTracking buf=%X comp=%X ndt=%d outstand=%d\n", buf, comp, buf->num_death_trackers, comp->outstanding_buffers);
}


static uint_t Buf_GetDurationQ8(buf_t* buf)
{
    uint_t freq_kHz_Q4 = esaafreq2hz(buf->format.freq) * 16 / 1000;

    return 256 * 16 * buf->bytes_valid / (2 * buf->format.chan_cfg * freq_kHz_Q4);
}


static void Buf_Dump(buf_t* buf)
{
    dbg_indent(); dbg_printf("buf=%X ref=%d %d/%d %X ", (int) buf, buf->refcnt, buf->bytes_valid, buf->bytes_available, (int) buf->comp);
    if (buf->comp == 0) {
        dbg_printf("Orphaned");
    } else if (buf->dtor == buf_dtor_FillThisBuffer) {
        dbg_printf("dtor FillThisBuffer");
    } else if (buf->dtor == buf_dtor_EmptyBufferDone) {
        OMX_BUFFERHEADERTYPE* pBuffer = (OMX_BUFFERHEADERTYPE*) buf->dtor_param2;
        OMX_U32 idx = pBuffer->nInputPortIndex;
        dbg_printf("dtor EmptyBufferDone %X.%d ilbuf=%X", (int) buf->comp, (int) idx, (int) pBuffer);
    } else if (buf->dtor == buf_dtor_CompBuf) {
        dbg_printf("dtor CompBuf");
    } else if (buf->dtor == buf_dtor_DetachedBuffer) {
        dbg_printf("dtor DetachedBuffer");
    }

    uint_t i;
    for (i = 0 ; i < buf->num_death_trackers ; i++) {
        dbg_printf(" %X", (int) buf->death_trackers[i]);
    }

    dbg_printf("\n");
}


static void Buf_Detach(buf_t* buf)
{
    char* new_buf = malloc(buf->bytes_available);
    int refcnt_copy = buf->refcnt;
    if(!new_buf) {
        ALOG_ERR("malloc() failed, address space exhaused?\n");
    }
    memcpy(new_buf, buf->data, buf->bytes_valid);

    ALOG_ADIL_INT("DetachBuffer: %X, data at %X, refcnt = %d\n", (int) buf, (int) buf->data, buf->refcnt);

    uint_t i;
    for (i = 0 ; i < buf->num_death_trackers ; i++) {
        if (buf->death_trackers[i]) {
            ALOG_ADIL_INT("detach update refcnt: decreasing ref cnf buf=%X comp=%X outstanding=%d\n", buf, buf->death_trackers[i], buf->death_trackers[i]->outstanding_buffers);
            assert(buf->death_trackers[i]->outstanding_buffers > 0);
            buf->death_trackers[i]->outstanding_buffers--;
        }
    }
    buf->num_death_trackers = 0;

    assert(buf->dtor == buf_dtor_FillThisBuffer || buf->dtor == buf_dtor_EmptyBufferDone);
    buf->refcnt = 0; // Checked by dtor, TODO do other way?
    buf->dtor(buf);
    buf->data = new_buf;
    buf->dtor = buf_dtor_DetachedBuffer;
    buf->refcnt = refcnt_copy;
    ALOG_ADIL_INT("DetachBuffer: %X, data now at %X\n", (int) buf, (int) new_buf);
}


static void buf_dtor_DetachedBuffer(buf_t* buf)
{
    free(buf->data);
}




///////////////////////////////////////////////////////////////////////////////
//
//  Port_IL - output data using IL tunneling, and insert silence on underrun
//  on an input.
//
///////////////////////////////////////////////////////////////////////////////
static void expand_16to32(int* __restrict dst, short* __restrict src, unsigned int samples)
{
    unsigned int i;
    for (i = 0 ; i < samples ; i++) {
        dst[i] = src[i] << 16;
    }
}


static void shrink_32to16(unsigned short* __restrict dst, unsigned int* __restrict src, unsigned int samples)
{
    unsigned int i;
    unsigned int* __restrict dst_int = (unsigned int*) dst;

    if (samples % 8 == 0) {
        // dst is write-through when it is the DSP buffer, so we
        // need to be careful when accessing it
        for (i = 0 ; i < samples ; i+=8) {
            unsigned long tmp[4];
            tmp[0] = (src[0] >> 16)  | (src[1] & 0xFFFF0000);
            tmp[1] = (src[2] >> 16)  | (src[3] & 0xFFFF0000);
            tmp[2] = (src[4] >> 16)  | (src[5] & 0xFFFF0000);
            tmp[3] = (src[6] >> 16)  | (src[7] & 0xFFFF0000);
            memcpy(dst_int, tmp, 16);
            dst_int += 4;
            src     += 8;
        }
    } else {
        for (i = 0 ; i < samples ; i++) {
            *dst++ = *src++ >> 16;
        }
    }
}


static void Port_IL_DestroySRC(port_t* port)
{
    if (port->SRC_input_buf)  free(port->SRC_input_buf);
    if (port->SRC_output_buf) free(port->SRC_output_buf);
    if (port->SRC_heap)       free(port->SRC_heap);
    if (port->cur_src_buf) {
        Buf_Release(port->cur_src_buf);
        port->cur_src_buf = 0;
    }
    port->SRC_input_buf = 0;
    port->SRC_output_buf = 0;
    port->SRC_heap = 0;
    port->SRC_Ready = 0;
    port->SRC_input_buf_bytes_valid = 0;
}


static void Port_IL_Deactivate(port_t* port)
{
    CompBuf_Destroy(&port->zerobuf);
    Port_IL_DestroySRC(port);
    port->num_omx_buf = 0;

    while(OMXBufQueue_Pop(&port->omx_buf_queue));
}


static void Port_IL_DetachBuffers(port_t* port)
{
    uint_t i;

    for (i = 0 ; i < port->num_omx_buf ; i++) {
        if (port->omx_buf[i].bh && port->omx_buf[i].adil_buf) {
            Buf_Detach(port->omx_buf[i].adil_buf);
            port->omx_buf[i].adil_buf = 0;
        }
    }
}


static void Port_IL_Activate(comp_t* comp, port_t* port)
{
    assert(port->num_omx_buf > 0);
        port->SRC_block_size = port->omx_buf[0].bh->nAllocLen / 2; // One quarter of buffersize but for 32bit samples

    CompBuf_Init(&port->zerobuf, comp, port->omx_buf[0].bh->nAllocLen, port->num_omx_buf);
    port->isEos = 0;
}


static void Port_Activate(comp_t* comp, OMX_U32 port_idx)
{
    port_t* port = &comp->port[port_idx];
    port->isStarted = 0;
    if (comp->onActivated) comp->onActivated(comp, port_idx);
    if (!port->isDirect) {
        Port_IL_Activate(comp, port);
    }
}


static int Port_IL_TryPush_SRC_Prepare(port_t* port)
{
    assert(port->cur_src_buf);
    if (port->SRC_Ready) {
        return 1;
    }

    int infreq  = port->cur_src_buf->format.freq;
    int outfreq = port->format.freq;
    ALOG_ADIL_INT("Port_IL_TryPush_SRC_Prepare infreq=%d outfreq=%d\n", esaafreq2hz(infreq), esaafreq2hz(outfreq));

    // Set to ripple mode on output port for streams 44.1kHz --> 48 kHz
    if(esaafreq2hz(infreq) == 44100 && esaafreq2hz(outfreq) == 48000) {
        ALOG_ADIL("Port_IL_TryPush_SRC_Prepare Low Ripple activated! previous mode: %d", port->SRC_mode);
        port->SRC_mode = SRC_LOW_RIPPLE;
    }

    // TODO: Handle re-initialization, flush
    int heapsize;
    int status = resample_calc_max_heap_size_fixin_fixout(infreq, outfreq,
                       port->SRC_mode, &heapsize, port->SRC_block_size,
                       port->omx_format.nChannels, 0);
    if (status) {
        ALOG_ERR("Port_IL_TryPush_SRC_Prepare resample_calc_max_heap_size_fixin_fixout failed\n");
        ALOG_ERR("  infreq=%d outfreq=%d blocksize=%d channels=%d\n", infreq, outfreq, port->SRC_block_size, port->omx_format.nChannels);
        return 0;
    }

    port->SRC_input_buf  = malloc(port->SRC_block_size);
    port->SRC_output_buf = malloc(port->SRC_block_size);
    port->SRC_heap       = malloc((size_t) heapsize);
    port->SRC_input_buf_bytes_valid = 0;

    if (!port->SRC_input_buf || !port->SRC_output_buf || !port->SRC_heap) {
        ALOG_ERR("Port_IL_TryPush_SRC_Prepare resample_calc_max_heap_size_fixin_fixout out of memory\n");
        return 0;
    }

    status = resample_x_init_ctx_low_mips_fixin_fixout(port->SRC_heap, heapsize, &port->SRC_ctx,
                                                  infreq, outfreq, port->SRC_mode,
                                                  port->omx_format.nChannels, port->SRC_block_size);

    ALOG_ADIL_INT("SRC LIB INIT: Inrate=%d Outrate=%d lomips_to_out48=%d channel_nb=%d blocksiz=%d\n", infreq, outfreq, SRC_STANDARD_QUALITY, port->omx_format.nChannels, port->SRC_block_size);

    if (status) {
        ALOG_ERR("Port_IL_TryPush_SRC_Prepare init failed, %d, infreq=%d outfreq=%d channels=%d\n", status, infreq, outfreq, port->omx_format.nChannels);
        return 0;
    }
    port->SRC_Ready = 1;
    port->il_src_eos = 0;

    return 1;
}


static void Port_IL_TryPush_SRC(port_t* port)
{
    // SRC code expects 32-bit samples, so we need to convert
    ALOG_ADIL_INT("ADIL: Port_IL_TryPush_SRC il_src_eos=%d\n", port->il_src_eos);
    assert(port->SRC_block_size > 0);

    while (1) {
        // Find IL buffer to output data to (FUTURE IMPROVEMENT common code TryPushIL_Copy)
        uint_t nbr_buffers_to_use = (port->nbr_of_buffers_to_use==0?port->num_omx_buf:port->nbr_of_buffers_to_use);

        if (port->cur_dst_buf == NULL && OMXBufQueue_GetCount(&port->omx_buf_queue) > (port->num_omx_buf - nbr_buffers_to_use)) {
            omx_buf_t* buf = OMXBufQueue_Peek(&port->omx_buf_queue);
            if (buf->bh && !buf->busy_in_peer && !buf->to_be_returned) {
                buf->bh->nFilledLen = 0;
                port->cur_dst_buf = buf;
            }
        }
        ALOG_ADIL_INT("port->cur_dst_buf=%X, OMXBufQueue_Size=%d, port->nbr_of_buffers_to_use=%d",
                port->cur_dst_buf, OMXBufQueue_GetCount(&port->omx_buf_queue), port->nbr_of_buffers_to_use);

        // Make sure we have input data. Convert to 32-bit format.
        // FUTURE IMPROVEMENTS: SRC algorithm will work on 16-bit samples
        // FUTURE IMPROVEMENT: Try to factor out this kind of common code
        // FUTURE IMPROVEMENT: Try to remove port->cur_src_buf and use Port_PeekBuf instead?
        while (port->SRC_input_buf_bytes_valid < port->SRC_block_size && !port->il_src_eos)  {
            // Find buffer to get data from
            if (!port->cur_src_buf) {
                port->cur_src_buf = Port_PopBuf(port);
                port->cur_src_buf_offset = 0;
            }

            if (!port->cur_src_buf) {
                break;
            }

            if (!Port_IL_TryPush_SRC_Prepare(port)) {
                ALOG_ADIL_INT("IL: Port_IL_TryPush_SRC_Prepare failed\n");
                return;
            }

            uint_t out_samples = (port->SRC_block_size - port->SRC_input_buf_bytes_valid) / 4;
            uint_t in_samples  = (port->cur_src_buf->bytes_valid - port->cur_src_buf_offset) / 2;
            uint_t samples_to_copy = MIN(out_samples, in_samples);

            PERF_LOG("pre-SRC 16->32 BEGIN");
            expand_16to32((int*) (port->SRC_input_buf + port->SRC_input_buf_bytes_valid),
                          (short*) (port->cur_src_buf->data + port->cur_src_buf_offset),
                          samples_to_copy);
            PERF_LOG("pre-SRC 16->32 DONE");

            port->SRC_input_buf_bytes_valid += samples_to_copy * 4;
            port->cur_src_buf_offset        += samples_to_copy * 2;

            port->il_src_eos = port->cur_src_buf->eos;

            if (port->cur_src_buf_offset == port->cur_src_buf->bytes_valid || port->cur_src_buf->eos) {
                Buf_Release(port->cur_src_buf);
                port->cur_src_buf = 0;
            }
        }

        if (port->cur_dst_buf == NULL ||
                (port->SRC_input_buf_bytes_valid != port->SRC_block_size &&
                !port->il_src_eos))
        {
            ALOG_ADIL_INT("Port_IL_TryPush_SRC port->cur_dst_buf=%X port->SRC_input_buf_bytes_valid=%d\n",port->cur_dst_buf,port->SRC_input_buf_bytes_valid);
            return;
        }

        // We have all input data and an empty output buffer, run SRC algorithm
        if (port->SRC_input_buf_bytes_valid == port->SRC_block_size || port->il_src_eos) {
            if (port->il_src_eos) {
                if (port->SRC_input_buf_bytes_valid > 0) {
                    memset(port->SRC_input_buf + port->SRC_input_buf_bytes_valid, 0,
                           port->SRC_block_size - port->SRC_input_buf_bytes_valid);
                    port->SRC_input_buf_bytes_valid = port->SRC_block_size;
                } else {
                    port->SRC_input_buf_bytes_valid = 0;
                }
            }

            int nSamples = port->SRC_input_buf_bytes_valid / (4 * port->omx_format.nChannels);
            int nbOut = nSamples;
            int flush = port->il_src_eos;

            ALOG_ADIL_INT("IL OUT src_ctx.processing nSamples=%d nbOut=%d flush=%d\n", nSamples, nbOut, flush);

            PERF_LOG("SRC Begin");
            // assert(port->omx_buf[port->cur_dst_buf_idx].bh->nAllocLen >= port->SRC_block_size);
            ADM_ASSERT(port->SRC_input_buf);

            int status = resample_x_process_fixin_fixout(&port->SRC_ctx, (int*) port->SRC_input_buf, port->omx_format.nChannels,
                                            &nSamples, (int*) port->SRC_output_buf,
                                            &nbOut, &flush);
            PERF_LOG("SRC DONE");

            ALOG_ADIL_INT("IL OUT src_ctx.processing status=%d nbOut=%d nSamples=%d\n", status, nbOut, nSamples);

            if (nSamples != 0 || flush) {
                // Only support either consuming no data, or all data
                assert(port->SRC_block_size / (4*port->omx_format.nChannels) == (uint_t) nSamples);
                port->SRC_input_buf_bytes_valid = 0;
            }

            if (nbOut) {
                int samples_to_copy = nbOut *  port->omx_format.nChannels;
                // Recover if the port was configured in 16-bit mode
                if (port->omx_format.nBitPerSample == 16) {
                    ALOG_ADIL_INT("IL OUT SRC: Output port is 16-bit, will have to downconvert\n");
                    PERF_LOG("shrink_32to16 Begin");
                    shrink_32to16((unsigned short*)(port->cur_dst_buf->bh->pBuffer +
                                  port->cur_dst_buf->bh->nFilledLen),
                                  (unsigned int*) port->SRC_output_buf,
                                  samples_to_copy);

                    PERF_LOG("shrink_32to16 DONE");
                }

                port->cur_dst_buf->bh->nFilledLen += samples_to_copy * 2;
            }

            int send_buf = 0;
            if (nbOut) {
                unsigned int bytes_to_fill;
                if (port->buffer_fill_time == 0) {
                    bytes_to_fill = port->cur_dst_buf->bh->nAllocLen;
                } else {
                    bytes_to_fill = MIN((port->buffer_fill_time * port->omx_format.nChannels * port->omx_format.nSamplingRate * 2) / 1000,
                                        port->cur_dst_buf->bh->nAllocLen);
                }
                if (port->cur_dst_buf->bh->nFilledLen >= bytes_to_fill) {
                    send_buf = 1;
                }
            } else if (port->il_src_eos) {
                send_buf = 1;
            }

            if (send_buf) {
                if (port->omx_format.nChannels == 6 && !is_hdmi_format(&port->omx_format)) {
                // Internally, only handle 5.1 audio in HDMI channel order to reduce complexity
                    ALOG_ADIL_INT("Re-arranging 5.1 channels on output\n");
                    InplaceMoveChannels((short*) port->cur_dst_buf->bh->pBuffer,
                         port->cur_dst_buf->bh->nFilledLen/12, &port->omx_format, 0);
                }

                if (nbOut == 0 && port->il_src_eos) {
                    ALOG_ADIL_INT("IL OUT SRC: Sending EOS\n");
                    port->cur_dst_buf->bh->nFlags |= OMX_BUFFERFLAG_EOS;
                }

                Port_queueReturnBuffer(port, port->cur_dst_buf->bh);
                port->cur_dst_buf = NULL;
                if (nbOut == 0 && port->il_src_eos) {
                    Port_IL_DestroySRC(port);
                    port->il_src_eos = 0;
                    return;
                }
            }
        } //if (port->SRC_input_buf_bytes_valid == port->SRC_block_size || port->il_src_eos)
    }
}


static void Port_IL_TryPush_Copy(port_t* port)
{
    ALOG_ADIL_INT("Port_IL_TryPush_Copy\n");

    while(1) {
        // Find buffer to get data from
        if (!port->cur_src_buf) {
            port->cur_src_buf = Port_PopBuf(port);
            port->cur_src_buf_offset = 0;
        }

        // Find IL buffer to output data to (FUTURE IMPROVEMENT common code TryPushIL_Copy)
        uint_t nbr_buffers_to_use = (port->nbr_of_buffers_to_use==0?port->num_omx_buf:port->nbr_of_buffers_to_use);

        if (port->cur_dst_buf == NULL &&
            OMXBufQueue_GetCount(&port->omx_buf_queue) > (port->num_omx_buf - nbr_buffers_to_use)) {
            omx_buf_t* buf = OMXBufQueue_Peek(&port->omx_buf_queue);
            if (buf->bh && !buf->busy_in_peer && !buf->to_be_returned) {
                buf->bh->nFilledLen = 0;
                port->cur_dst_buf = buf;
            }
        }


        if (!(port->cur_src_buf && port->cur_dst_buf)) {
            ALOG_ADIL_INT("Port_IL_TryPush: Not two buffer, do nothing (%X %X)\n", (int)port->cur_src_buf, port->cur_dst_buf);
            return;
        }

        int out_is_32 = (port->omx_format.nBitPerSample == 32);
        uint_t out_bytes_per_sample = out_is_32 ? 4 : 2;

        // Ok, we have two buffers. Do something.
        uint_t out_samples_available = (port->cur_dst_buf->bh->nAllocLen - port->cur_dst_buf->bh->nFilledLen) / out_bytes_per_sample;
        uint_t in_samples_available  = (port->cur_src_buf->bytes_valid - port->cur_src_buf_offset) / 2;
        uint_t samples_to_copy = MIN(in_samples_available, out_samples_available);

        port->cur_dst_buf->bh->pBuffer = (OMX_U8*) port->cur_dst_buf->original_buf;
        const char* input_pos = port->cur_src_buf->data + port->cur_src_buf_offset;
        char*      output_pos = (char*) port->cur_dst_buf->bh->pBuffer + port->cur_dst_buf->bh->nFilledLen;

        assert(out_samples_available >= samples_to_copy);
        if (samples_to_copy) {
            if (!out_is_32) {
                PERF_LOG("Port_IL_TryPush_Copy memcpy() BEGIN");
                memcpy(output_pos, input_pos, samples_to_copy * 2);
                PERF_LOG("Port_IL_TryPush_Copy memcpy() DONE");
            } else {
                uint_t i;
                ALOG_ADIL_INT("Copying %d samples\n", samples_to_copy);
                PERF_LOG("Port_IL_TryPush_Copy expand to 32-bit BEGIN");
                for (i = 0 ; i < samples_to_copy ; i++) {
                        ((int*) output_pos)[i] = (int) ((const short*) input_pos)[i] * 65536;
                } //if (!out_is_32)
            PERF_LOG("Port_IL_TryPush_Copy expand to 32-bit DONE");
            }
        }

        port->cur_dst_buf->bh->nFilledLen += samples_to_copy * out_bytes_per_sample;
        port->cur_src_buf_offset          += samples_to_copy * 2;

        // If the IL buffer is full, send it away.
        unsigned int bytes_to_fill;
        if (port->buffer_fill_time == 0) {
            bytes_to_fill = port->cur_dst_buf->bh->nAllocLen;
        }
        else {
            bytes_to_fill = MIN((port->buffer_fill_time * port->omx_format.nChannels * port->omx_format.nSamplingRate * 2) / 1000,
                                 port->cur_dst_buf->bh->nAllocLen);
        }

        if (port->cur_dst_buf->bh->nFilledLen >= bytes_to_fill || port->cur_src_buf->eos) {
            if (port->omx_format.nChannels == 6 && !is_hdmi_format(&port->omx_format)) {
                // Internally, only handle 5.1 audio in HDMI channel order to reduce complexity
                ALOG_ADIL_INT("Re-arranging 5.1 channels on output\n");
                InplaceMoveChannels((short*) port->cur_dst_buf->bh->pBuffer,
                                    port->cur_dst_buf->bh->nFilledLen/12, &port->omx_format, 0);
            }

            if (port->cur_src_buf->eos) {
                port->cur_dst_buf->bh->nFlags |= OMX_BUFFERFLAG_EOS;
            }
            Port_queueReturnBuffer(port, port->cur_dst_buf->bh);
            port->cur_dst_buf = NULL;
        }

        // We're done with the buffer if there is no more data in it..
        if (port->cur_src_buf_offset == port->cur_src_buf->bytes_valid) {
            ALOG_ADIL_INT("ADIL: We're done with the input buffer %X, refcnt before release = %d\n", (int) port->cur_src_buf, port->cur_src_buf->refcnt);
            Buf_Release(port->cur_src_buf);
            port->cur_src_buf = NULL;
        }
    } //while(1)
}


static void Port_IL_TryPush(port_t* port)
{
    if (port->il_method != IL_METHOD_NONE) {
        buf_t* buf = Port_PeekBuf(port);
        if (buf && !buf_format_same(port->il_last_format,buf->format)) {
            ALOG_ADIL_INT("ADIL: Buffer format changed, new method choice\n");
            ALOG_ADIL_INT("        old: freq=%d chan=%d   new: freq=%d chan=%d\n", port->il_last_format.freq, port->il_last_format.chan_cfg, buf->format.freq, buf->format.chan_cfg);
            port->il_method = IL_METHOD_NONE;
        }
    }

    if (port->il_method == IL_METHOD_NONE) {
        buf_t* buf = Port_PeekBuf(port);

        if (!buf) {
            return;
        }

        if (buf_format_same(buf->format, port->format)) {
            ALOG_ADIL_INT("ADIL: Will use method IL_METHOD_COPY\n");
            port->il_method = IL_METHOD_COPY;
        } else {
            ALOG_ADIL_INT("ADIL: Will use method IL_METHOD_SRC\n");
            port->il_method = IL_METHOD_SRC;
        }
        port->il_last_format = buf->format;
    }

    switch (port->il_method) {
        case IL_METHOD_COPY:
            Port_IL_TryPush_Copy(port);
            return;

        case IL_METHOD_SRC:
            Port_IL_TryPush_SRC(port);
            return;

        default:
            assert(0);
    }
}


static void Port_IL_generateSilenceIfUnderrun(port_t* port)
{
    assert(port->isInput && port->isEnabled);
    ALOG_ADIL_INT("checking for silence due to underrun/EOS, port=%X\n", (int) port);

    // If all IL buffers are "on the other side", emit silence
    if (port->isEos && port->isStarted) {
        ALOG_ADIL_INT("Port is EOS, candidate for silence insertion\n");
    } else if (!port->silenceOnUnderrun || !port->isStarted) {
        return;
    } else {
        unsigned int i;
        for (i = 0 ; i < port->num_omx_buf ; i++) {
            if (!port->omx_buf[i].busy_in_peer && !port->omx_buf[i].to_be_returned) {
                ALOG_ADIL_INT(" ... component had buffer which was not in peer, no underrun\n");
                return;
            }
        }
    }

    uint64_t now = get_time_of_day_ms();

    if (port->underrun_time == 0) {
        port->underrun_time = now;
        subscribe_timer_event(port->comp, now + TIME_BEFORE_SILENCE);
    }
    else if (now - port->underrun_time >= TIME_BEFORE_SILENCE) {

        port->underrun_time = now;
        subscribe_timer_event(port->comp, now + TIME_BEFORE_SILENCE);

        buf_t* zerobuf = CompBuf_Pop(&port->zerobuf);
        if (zerobuf) {
            uint_t framesize = (uint_t) (port->format.chan_cfg * 2);
            memset(zerobuf->data, 0, zerobuf->bytes_available);
            zerobuf->bytes_valid = (zerobuf->bytes_available / framesize) * framesize;
            ALOG_WARN("underrun -> inserted %d bytes of silence, buf=%X, sr=%d, chan=%d\n", zerobuf->bytes_valid, (int) zerobuf, port->format.freq, port->format.chan_cfg);
            zerobuf->format = port->format;
            Port_Push(port, zerobuf);
            Comp_SetDirty(port->comp);
        } else {
            ALOG_ADIL_INT("underrun -> no zerobuf, could not insert silence\n");
        }
    }
}


static void Comp_IL_Process(comp_t* comp)
{
    ALOG_ADIL_INT("ADIL: Comp_IL_Process %X\n", (int) comp);
    assert(comp->state != OMX_StateLoaded);
    unsigned int i;

    for (i = 0 ; i < comp->ports ; i++) {
        port_t* port = &comp->port[i];
        if (port->isEnabled && port->target_state && !port->isDirect) {
            if (port->isInput) {
                Port_IL_generateSilenceIfUnderrun(port);
            } else {
                Port_IL_TryPush(port);
            }
        }
    }
}


static void Port_Push(port_t* port, buf_t* buf)
{
    if (!port->isInput && port->comp->track_outstanding_buffers) {
        Buf_AddOutstandingTracking(buf, port->comp);
        port->comp->outstanding_buffers++;
        assert(port->comp->outstanding_buffers <= BUFHDRPOOL_MAX);
    }

    if (port->isDirect) {
        comp_t* peercomp = (comp_t*) port->peer;
        ALOG_ADIL_INT("Port_Push PROP --> %X\n", (int) peercomp);
        if (peercomp->port[port->peerport].isEnabled) {
            assert(peercomp->port[port->peerport].isDirect);
            Port_PutBuf(&peercomp->port[port->peerport], buf);
            Comp_SetDirty(peercomp);
        } else {
            ALOG_ADIL_INT("ADIL: Port_Push buf=%X to %X.%d, which was disabled. Released buffer\n",(int) buf, (int) peercomp, port->peerport);
            Buf_Release(buf);
        }
    } else {
        ALOG_ADIL_INT("Port_Push\n");
        Port_PutBuf(port, buf);
    }
}




///////////////////////////////////////////////////////////////////////////////
//
// Core support functions
//
///////////////////////////////////////////////////////////////////////////////
static int hasAllResources(port_t* port)
{
    if (port->isDirect) {
        return 1;
    }

    if (port->num_omx_buf < (unsigned int) port->portdef.nBufferCountActual) {
        ALOG_ADIL_INT("hasAllResources: num_omx_buf %d < port->portdef.nBufferCountActual %d\n", port->num_omx_buf, port->portdef.nBufferCountActual);
        return 0;
    }

    unsigned int i;
    for (i = 0 ; i < port->num_omx_buf ; i++) {
        if (!port->omx_buf[i].allocated) {
            ALOG_ADIL_INT("hasAllResources: buf %d not allocated\n", i);
            return 0;
        }
    }

    return 1;
}


static int hasNoResources(port_t* port)
{
    if (port->isDirect) {
        return 1;
    }

    unsigned int i;
    for (i = 0 ; i < port->num_omx_buf ; i++) {
        if (port->omx_buf[i].allocated) {
            return 0;
        }
    }

    return 1;
}


static int portIsReadyForIdle(port_t* port)
{
    if (port->isDirect) {
        return 1;
    }

    unsigned int i;
    for (i = 0 ; i < port->num_omx_buf ; i++) {
        if (port->isEnabled) {
            if (port->isSupplier) {
                if (port->omx_buf[i].busy_in_peer || port->omx_buf[i].to_be_returned || port->omx_buf[i].adil_buf) {
                    return 0;
                }
            } else {
                if (!port->omx_buf[i].busy_in_peer) {
                    return 0;
                }
            }
        }
    }

    return 1;
}


static void IL_Core_Process()
{
    int first_set_bit;

    while ((first_set_bit = ffs((int) dirty_comp_mask))) {
        unsigned int comp_idx = (unsigned int) (first_set_bit-1);
        dirty_comp_mask &= ~(1U << comp_idx);
        comp_t* comp = comps[comp_idx];
        if (comp->state != OMX_StateLoaded && comp->target_state != OMX_StateLoaded) {
            comp->process(comp);
            Comp_IL_Process(comp);
        }
    }
}


// Check if the ports have aquired/lost resources, causing a state transition
// of the component to be completed.
static void check_comp_state(comp_t* comp)
{
    // Check port state transitions
    OMX_U32 i;
    for (i = 0 ; i < comp->ports ; i++) {
        port_t* port = &comp->port[i];

        if (!port->isEnabled && port->target_state == 1) {
            if (comp->state == OMX_StateLoaded || hasAllResources(port)) {
                port->isEnabled = port->target_state;
                if (comp->state != OMX_StateLoaded) {
                    Port_Activate(comp, i);
                }

                if (comp->state != OMX_StateLoaded && port->peer) {
                    ALOG_ADIL("[OUT] %X.%d [%s] CmdComplete PortEnable (connected to %X.%d)\n", (int) comp, i, comp->dbg_name, (int) port->peer, port->peerport);
                } else {
                    ALOG_ADIL("[OUT] %X.%d [%s] CmdComplete PortEnable\n", (int) comp, i, comp->dbg_name);
                }
                comp->cb.EventHandler(&comp->omx, comp->omx.pApplicationPrivate,
                                      OMX_EventCmdComplete, OMX_CommandPortEnable, i, 0);
                Comp_SetDirty(comp);
            } else {
                ALOG_ADIL_INT("ADMIL check_comp_state %X.%d port enable not completed, not all resources yet\n", (int) comp, i);
                unsigned int idx;

                for (idx = 0 ; idx < port->num_omx_buf ; idx++) {
                    ALOG_ADIL_INT("ADMIL  buf %d: allocated=%d\n", idx, port->omx_buf[idx].allocated);
                }
            }
        }
        if (port->isEnabled && port->target_state == 0) {
            Port_ReleaseBuffers(port);
            if (hasNoResources(port)) {
                port->isEnabled = port->target_state;
                port->peer = 0;
                port->isDirect = 0;
                if (comp->state != OMX_StateLoaded) {
                    if (comp->onDeactivated) comp->onDeactivated(comp, i);
                    Port_IL_Deactivate(port);
                }
                ALOG_ADIL("[OUT] %X.%d [%s] CmdComplete PortDisable\n", (int) comp, i, comp->dbg_name);
                adil_log_data_close((OMX_HANDLETYPE) comp, port->idx);
                comp->cb.EventHandler(&comp->omx, comp->omx.pApplicationPrivate,
                                      OMX_EventCmdComplete, OMX_CommandPortDisable, i, 0);
                Comp_SetDirty(comp);
            } else {
                ALOG_ADIL_INT("check_comp_state %X.%d port disable not completed, not lost all resources yet\n", (int) comp, i);
                unsigned int idx;

                for (idx = 0 ; idx < port->num_omx_buf ; idx++) {
                    ALOG_ADIL_INT("ADMIL  buf %d: allocated=%d\n", idx, port->omx_buf[idx].allocated);
                }
            }
        }
    }

    // Check component state transitions
    int state_changed = 0;
    if (comp->state == OMX_StateExecuting && comp->target_state == OMX_StateIdle) {
        for (i = 0 ; i < comp->ports ; i++) {
            port_t* port = &comp->port[i];
            if (!portIsReadyForIdle(port)) {
                return;
            }
        }
        state_changed=1;
    }

    if (comp->state == OMX_StateIdle && comp->target_state == OMX_StateLoaded) {
        int has_resouces = 0;

        for (i = 0 ; i < comp->ports ; i++) {
            port_t* port = &comp->port[i];
            Port_ReleaseBuffers(port);
            if (!hasNoResources(port)) {
                has_resouces = 1;
            }
        }

        if (has_resouces) {
            return;
        }

        state_changed=1;
        for (i = 0 ; i < comp->ports ; i++) {
            port_t* port = &comp->port[i];
            if (port->isEnabled) {
                if (comp->onDeactivated) comp->onDeactivated(comp, i);
                Port_IL_Deactivate(port);
            }
        }
        adil_log_data_close((OMX_HANDLETYPE) comp, 0xFFFFFFFF);
    }

    if (comp->state == OMX_StateLoaded && comp->target_state == OMX_StateIdle) {
        for (i = 0 ; i < comp->ports ; i++) {
            port_t* port = &comp->port[i];
            if (port->isEnabled && !hasAllResources(port)) {
                ALOG_ADIL_INT("Loaded->Idle; not yet, %X.%u not done\n", (int) comp, i);

                return;
            }
        }
        state_changed=1;
        for (i = 0 ; i < comp->ports ; i++) {
            port_t* port = &comp->port[i];
            if (comp->onActivated && port->isEnabled) {
                Port_Activate(comp, i);
                if (!port->isDirect && port->peer) {
                    ALOG_ADIL("Port %X.%d is connected to %X.%d\n", (int) comp, i, (int) port->peer, port->peerport);
                }
            }
        }
    }

    if (state_changed) {
        comp->state = comp->target_state;
        ALOG_ADIL("[OUT] %X [%s] CmdComplete StateSet %s\n", (int) comp, comp->dbg_name, adm_log_state2str(comp->state));
        comp->cb.EventHandler(&comp->omx, comp->omx.pApplicationPrivate,
                                OMX_EventCmdComplete, OMX_CommandStateSet, comp->state, 0);
    }
}


static void AquirePortResources(comp_t* comp, uint_t idx)
{
    ALOG_ADIL_INT("AquirePortResources port %X.%d\n", (int) comp, idx);
    port_t* port = &comp->port[idx];
    assert(port->target_state); // Port must at least want to be enabled

    if (port->isDirect) {
        check_comp_state(comp);
        return;
    }

    if (port->peer && port->isSupplier) {
        ALOG_ADIL("Peer (%X.%u): We supply to it. Requires >= %d bufs of >= %d bytes\n", (int) port->peer, port->peerport, port->peer_portdef.nBufferCountActual, port->peer_portdef.nBufferSize);

        // tunnelled port, we are supplier
        // If we give more buffers than it expects, it will enter a state in which it does
        // not accept buffers.

        // TODO: Is this the way we want to handle if peer requests a different
        // number of buffers
        port->portdef.nBufferCountActual = port->peer_portdef.nBufferCountActual;

        uint_t b;
        for (b = 0 ; b < port->peer_portdef.nBufferCountActual ; b++) {
            char* buf = calloc(1, port->peer_portdef.nBufferSize);
            queueUseBuffer(comp, idx, b, port->peer, port->peerport, comp, port->peer_portdef.nBufferSize, (OMX_U8*) buf);
            ALOG_ADIL("[OUT] %X.%u queueUseBuffer, %d bytes\n", (int) port->peer, port->peerport, port->peer_portdef.nBufferSize);
        }
    }
    else
    {
        // non-supplier (tunneled or basic), wait for buffers
        ALOG_ADIL("%X.%d [%s] waiting for %d buffers of at least %d bytes\n", (int) comp,idx, comp->dbg_name,port->portdef.nBufferCountActual, port->portdef.nBufferSize);
    }

    check_comp_state(comp);
}


static void UseBufferCompletedCallback(comp_t* comp, OMX_U32 parent_port_index, uint_t bufidx, OMX_BUFFERHEADERTYPE* bufhdr, char* buf, OMX_ERRORTYPE res)
{
    if (res != OMX_ErrorNone) {
        ALOG_ADIL_INT("ADMIL [OUT] OMX_UseBuffer failed %s\n", adm_log_err2str(res));
        free(buf);
    } else {
        port_t* port = &comp->port[parent_port_index];

        if (port->isInput) {
            bufhdr->pInputPortPrivate  = (void*) bufidx;
            bufhdr->nInputPortIndex    = parent_port_index;
        } else {
            bufhdr->pOutputPortPrivate = (void*) bufidx;
            bufhdr->nOutputPortIndex   = parent_port_index;
        }

        port->omx_buf[bufidx].bh   = bufhdr;
        port->omx_buf[bufidx].adil_buf  = 0;
        port->omx_buf[bufidx].original_buf = buf;

        // the buffer initially belongs to the supplier port
        port->omx_buf[bufidx].busy_in_peer = 0;
        port->omx_buf[bufidx].to_be_returned = 0;
        port->omx_buf[bufidx].allocated    = 1;
        port->omx_buf[port->num_omx_buf].allocated_by_us = 1;

        if (port->isSupplier) {
            // If we supply, we start with the buffer
            OMXBufQueue_Push(&port->omx_buf_queue, &port->omx_buf[port->num_omx_buf]);
        }

        port->num_omx_buf++;
        ALOG_ADIL_INT("ADMIL [OUT] OMX_UseBuffer %X.%d ok (in callback), buf=%X\n", port->peer, port->peerport, port->omx_buf[bufidx].bh);
        ALOG_ADIL_INT("--> nInputPortIndex=%d, OutputPortIndex=%d\n", port->omx_buf[bufidx].bh->nInputPortIndex, port->omx_buf[bufidx].bh->nOutputPortIndex);
    }

    check_comp_state(comp);
}


static void ReturnBuffers(comp_t* comp, port_t* port)
{
    unsigned int i;
    for (i = 0 ; i < port->num_omx_buf ; i++) {
        if (!port->isSupplier) {
            ALOG_ADIL_INT("ReturnBuffers %X buf %d: busy_in_peer=%d adil_buf=%X tbr=%d\n", (int) comp, i, port->omx_buf[i].busy_in_peer, (int) port->omx_buf[i].adil_buf, port->omx_buf[i].to_be_returned);
            if (!port->omx_buf[i].to_be_returned && !port->omx_buf[i].busy_in_peer && !port->omx_buf[i].adil_buf) {
                Port_queueReturnBuffer(port, port->omx_buf[i].bh);
            }
        }
    }
}

static void DropPortResources(comp_t* comp, OMX_U32 port_idx)
{
    port_t* port = &comp->port[port_idx];
    assert(port->target_state == 0 || comp->target_state == OMX_StateLoaded);

    if (port->isDirect) {
        check_comp_state(comp);
        return;
    }

    if (port->isSupplier) {
        // Check if all buffers have arrived.
        unsigned int i;
        for (i = 0 ; i < port->num_omx_buf ; i++) {
            if (port->omx_buf[i].busy_in_peer || port->omx_buf[i].to_be_returned || port->omx_buf[i].adil_buf)
                return;
        }

        ALOG_ADIL_INT("DropPortResources %X.%d: All %d buffers are non-busy\n",(int) comp, (int) port_idx, port->num_omx_buf);

        // All buffers received, done.
        if (port->peer) {
            // If we are tunnelled using standard tunnelling
            for (i = 0 ; i < port->num_omx_buf ; i++) {
                //
                if (port->omx_buf[i].allocated_by_us && port->omx_buf[i].original_buf) {
                    free(port->omx_buf[i].original_buf);
                    port->omx_buf[i].original_buf = NULL;
                }
                // OMX_ERRORTYPE res;
                queueFreeBuffer(port->peer, port->peerport, port->omx_buf[i].bh);
                ALOG_ADIL("[OUT] queueFreeBuffer %X.%d, buf=%X\n", (int) port->peer, (int) port->peerport, (int) port->omx_buf[i].bh);
                port->omx_buf[i].bh = 0;
                port->omx_buf[i].allocated = 0;
            }
        }

        ALOG_ADIL_INT("Disabling of supplier port %X.%d complete\n", (int) comp, (int) port_idx);
    } else {
        // Other port is supplier. Give away all buffers.
        ALOG_ADIL_INT("DropPortResources non-supplier port %X.%d\n", (int) comp, (int) port_idx);
        ReturnBuffers(comp, port);
    }

    check_comp_state(comp);
}


static void buf_dtor_FillThisBuffer(buf_t* buf)
{
    assert(buf->refcnt == 0);
    comp_t* comp = buf->comp;
    OMX_BUFFERHEADERTYPE* pBuffer = (OMX_BUFFERHEADERTYPE*) buf->dtor_param2;
    int omx_buf_idx = (int) pBuffer->pInputPortPrivate;
    OMX_U32 idx = pBuffer->nInputPortIndex;
    port_t*  port = &comp->port[idx];
    if (port->target_state == 0 || comp->target_state == OMX_StateLoaded) {
        // Port is releasing resources
        port->omx_buf[omx_buf_idx].adil_buf = 0;
        DropPortResources(comp,idx);
    } else {
        pBuffer->nFilledLen = 0;
        port->omx_buf[omx_buf_idx].adil_buf = 0;
        Port_queueReturnBuffer(port, pBuffer);
    }
}


static void buf_dtor_EmptyBufferDone(buf_t* buf)
{
    assert(buf->refcnt == 0);
    comp_t* comp = buf->comp;
    OMX_BUFFERHEADERTYPE* pBuffer = (OMX_BUFFERHEADERTYPE*) buf->dtor_param2;

    int omx_buf_idx = (int) pBuffer->pInputPortPrivate;
    OMX_U32 idx = pBuffer->nInputPortIndex;
    port_t*  port = &comp->port[idx];
    port->omx_buf[omx_buf_idx].adil_buf = 0;
    assert(!port->omx_buf[omx_buf_idx].busy_in_peer);
    Comp_SetDirty(comp);
    Port_queueReturnBuffer(port, pBuffer);
}


// TODO? Join with UseBufferCompletedCallback and queueUseBuffer?
static OMX_ERRORTYPE allocate_single_buffer(OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes, OMX_IN OMX_U8* pBuffer, int allocated_by_us)

{
    comp_t* comp = (comp_t*) hComponent;
    port_t* port = &comp->port[nPortIndex];

    if (comp->state == OMX_StateLoaded) {
        if ( !(comp->target_state == OMX_StateIdle && port->isEnabled && port->target_state)) {
            return OMX_ErrorIncorrectStateOperation;
        }
    } else {
        if ( !(comp->target_state == comp->state && !port->isEnabled && port->target_state)) {
            return OMX_ErrorIncorrectStateOperation;
        }
    }

    *ppBuffer = calloc(1, sizeof(OMX_BUFFERHEADERTYPE));
    if (!*ppBuffer) {
        return OMX_ErrorInsufficientResources;
    }

    (*ppBuffer)->pBuffer = pBuffer;
    (*ppBuffer)->pAppPrivate = pAppPrivate;
    (*ppBuffer)->nAllocLen = nSizeBytes;

    if (port->isInput) {
        (*ppBuffer)->nInputPortIndex = nPortIndex;
        (*ppBuffer)->pInputPortPrivate  = (void*) port->num_omx_buf;
        (*ppBuffer)->pOutputPortPrivate  = NULL;
    } else {
        (*ppBuffer)->nOutputPortIndex = nPortIndex;
        (*ppBuffer)->pOutputPortPrivate = (void*) port->num_omx_buf;
        (*ppBuffer)->pInputPortPrivate = NULL;
    }

    port->omx_buf[port->num_omx_buf].bh = *ppBuffer;
    port->omx_buf[port->num_omx_buf].allocated = 1;
    port->omx_buf[port->num_omx_buf].allocated_by_us = (unsigned char) (allocated_by_us & 1);
    port->omx_buf[port->num_omx_buf].original_buf = (char*) pBuffer;
    port->omx_buf[port->num_omx_buf].to_be_returned = 0;

    if (port->peer) {
        port->omx_buf[port->num_omx_buf].busy_in_peer = !port->isSupplier;
    } else {
        // If no tunneling, buffer ownership starts in client
        port->omx_buf[port->num_omx_buf].busy_in_peer = 1;
    }

    if (port->isSupplier) {
        // If we supply, we start with the buffer
        ALOG_ADIL("We supply, so put buffer in OMXBufQueue\n");
        OMXBufQueue_Push(&port->omx_buf_queue, &port->omx_buf[port->num_omx_buf]);
    }

    port->num_omx_buf++;
    check_comp_state(comp);

    return OMX_ErrorNone;
}




///////////////////////////////////////////////////////////////////////////////
//
// IL API
//
///////////////////////////////////////////////////////////////////////////////
static OMX_ERRORTYPE adm_omxcore_SendCommand(
    OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd,
    OMX_U32 nParam1, OMX_PTR pCmdData)
{
    (void) pCmdData;
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    comp_t* comp = (comp_t*) hComponent;
    OMX_ERRORTYPE res = OMX_ErrorNone;
    int state_changed = 0;
    unsigned int i;

    if (Cmd == OMX_CommandStateSet || Cmd == OMX_CommandPortEnable || Cmd == OMX_CommandPortDisable) {
        for (i = 0 ; i < comp->ports ; i++) {
            if (comp->port[i].isEnabled != comp->port[i].target_state) {
                ALOG_ERR("ADIL: ERROR SendCommand busy, state transition already in progress for %X.%d\n", (int) comp, i);
                res = OMX_ErrorIncorrectStateTransition;
            }
        }
    }

    if (res == OMX_ErrorNone && Cmd == OMX_CommandStateSet) {
        if (comp->state != comp->target_state) {
            ALOG_ERR("ADIL: ERROR SendCommand busy, state transition already in progress\n");
            res = OMX_ErrorIncorrectStateTransition;
        } else if (comp->state == nParam1) {
            ALOG_ERR("ADIL: ERROR SendCommand - same state requested\n");
            res = OMX_ErrorSameState;
        }
        else if (comp->state == OMX_StateIdle && nParam1 == OMX_StateExecuting)
        {
            ALOG_ADIL("[IN] %X [%s] Idle -> Executing\n", (int) comp, comp->dbg_name);
            comp->state = comp->target_state = nParam1;
            state_changed = 1;
        }
        else if (comp->state == OMX_StateExecuting && nParam1 == OMX_StateIdle)
        {
            // IL expects all buffers to be returned to owner when going to Idle, so
            // we'll have to track this..
            ALOG_ADIL("[IN] %X [%s] Executing -> Idle\n", (int) comp, comp->dbg_name);
            comp->target_state = nParam1;

            for (i = 0 ; i < comp->ports ; i++) {
                if (comp->port[i].isEnabled && !comp->port[i].isDirect) {
                    Port_ReleaseBuffers(&comp->port[i]);
                    Port_IL_DetachBuffers(&comp->port[i]);
                    ReturnBuffers(comp, &comp->port[i]);
                }
            }
            check_comp_state(comp);
        }
        else if (comp->state == OMX_StateLoaded && nParam1 == OMX_StateIdle)
        {
            ALOG_ADIL("[IN] %X [%s] Loaded -> Idle\n", (int) comp, comp->dbg_name);
            comp->target_state = nParam1;

            for (i = 0 ; i < comp->ports ; i++) {
                if (comp->port[i].isEnabled) {
                    AquirePortResources(comp, i);
                }
            }
            check_comp_state(comp);
        }
        else if (comp->state == OMX_StateIdle && nParam1 == OMX_StateLoaded)
        {
            ALOG_ADIL("[IN] %X [%s] Idle -> Loaded\n", (int) comp, comp->dbg_name);
            comp->target_state = nParam1;

            for (i = 0 ; i < comp->ports ; i++) {
                DropPortResources(comp, i);
            }
            check_comp_state(comp);
        } else {
            ALOG_ERR("%X [%s] ERROR SendCommand unsupported state\n", (int) comp, comp->dbg_name);
            res = OMX_ErrorIncorrectStateTransition;
        }
    } //if (res == OMX_ErrorNone && Cmd == OMX_CommandStateSet)

    if (res == OMX_ErrorNone && Cmd == OMX_CommandPortEnable) {
        ALOG_ADIL("[IN] %X.%d [%s] PortEnable\n", (int) hComponent, nParam1, comp->dbg_name);
        if (nParam1 >= comp->ports) {
            res = OMX_ErrorBadParameter;
        } else {
            port_t* port = &comp->port[nParam1];
            if (port->isEnabled) {
                res = OMX_ErrorBadParameter;
            } else if (port->isDirect && port->peer && !port->isInput &&
                       !((comp_t*) port->peer)->port[port->peerport].isEnabled)
            {
                // Must enable input first.
                res = OMX_ErrorIncorrectStateTransition;
            } else {
                port->target_state = 1;

                if (comp->state == OMX_StateIdle || comp->state == OMX_StateExecuting) {
                    AquirePortResources(comp, nParam1);
                }
                check_comp_state(comp);
            }
        }
    } //if (res == OMX_ErrorNone && Cmd == OMX_CommandPortEnable)

    if (res == OMX_ErrorNone && Cmd == OMX_CommandPortDisable) {
        ALOG_ADIL("[IN] %X.%d [%s] PortDisable\n", (int) hComponent, nParam1, comp->dbg_name);
        if (nParam1 >= comp->ports) {
            res = OMX_ErrorBadParameter;
        } else {
            port_t* port = &comp->port[nParam1];
            if (!port->isEnabled) {
                res = OMX_ErrorBadParameter;
            } else {
                port->target_state = 0;
                Port_ReleaseBuffers(port);
                Port_IL_DetachBuffers(port);
                DropPortResources(comp, nParam1);
            }
        }
    } //(res == OMX_ErrorNone && Cmd == OMX_CommandPortDisable)

    IL_Core_Process();
    if (state_changed) {
        ALOG_ADIL("[OUT] %X [%s] CmdComplete StateSet %s\n", (int) comp, comp->dbg_name, adm_log_state2str(comp->state));
        comp->cb.EventHandler(hComponent, comp->omx.pApplicationPrivate,
                                OMX_EventCmdComplete, Cmd, nParam1, 0);
    }
    pthread_mutex_unlock(&mutex);
    queueExecute();

    return res;
}


static OMX_ERRORTYPE adm_omxcore_GetParameter(
            OMX_HANDLETYPE hComponent,
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure)
{
    pthread_mutex_lock(&mutex);
    ALOG_ADIL_INT("[ADMIL] [IN] %X GetParameter %s\n", (int) hComponent, adm_log_index2str(nParamIndex));

    comp_t* comp = (comp_t*) hComponent;
    OMX_ERRORTYPE res = Comp_onSetGet(comp, nParamIndex, pComponentParameterStructure, 0, 1);

    pthread_mutex_unlock(&mutex);

    return res;
}


static OMX_ERRORTYPE adm_omxcore_GetConfig(
            OMX_HANDLETYPE hComponent,
            OMX_INDEXTYPE nIndex,
            OMX_PTR pComponentConfigStructure)
{
    pthread_mutex_lock(&mutex);
    comp_t* comp = (comp_t*) hComponent;
    ALOG_ADIL_INT("[ADMIL] %X [%s] GetConfig %s\n", (int) hComponent, comp->dbg_name, adm_log_index2str(nIndex));
    OMX_ERRORTYPE res = Comp_onSetGet(comp, nIndex, pComponentConfigStructure, 0, 0);
    pthread_mutex_unlock(&mutex);

    return res;
}


static OMX_ERRORTYPE adm_omxcore_SetParameter(
    OMX_IN  OMX_HANDLETYPE hComponent,
    OMX_IN  OMX_INDEXTYPE nIndex,
    OMX_IN  OMX_PTR pParam)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);

    comp_t* comp = (comp_t*) hComponent;
    ALOG_ADIL_INT("[ADMIL] %X [%s] SetParameter %s\n", (int) hComponent, comp->dbg_name, adm_log_index2str(nIndex));
    OMX_ERRORTYPE res = Comp_onSetGet(comp, nIndex, pParam, 1, 1);
    IL_Core_Process();
    pthread_mutex_unlock(&mutex);

    return res;
}


static OMX_ERRORTYPE adm_omxcore_SetConfig(
    OMX_IN  OMX_HANDLETYPE hComponent,
    OMX_IN  OMX_INDEXTYPE nIndex,
    OMX_IN  OMX_PTR pComponentConfigStructure)
{
    (void) pComponentConfigStructure;
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    comp_t* comp = (comp_t*) hComponent;
    ALOG_ADIL_INT("[ADMIL] %X [%s] SetConfig %s\n", (int) hComponent, comp->dbg_name, adm_log_index2str(nIndex));
    OMX_ERRORTYPE res = Comp_onSetGet(comp, nIndex, pComponentConfigStructure, 1, 0);
    IL_Core_Process();
    pthread_mutex_unlock(&mutex);

    return res;
}


static OMX_ERRORTYPE adm_omxcore_EmptyThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    perf_reset();
    PERF_LOG("IN EmptyThisBuffer");

    OMX_ERRORTYPE res = OMX_ErrorNone;
    comp_t* comp = (comp_t*) hComponent;
    OMX_U32 idx = pBuffer->nInputPortIndex;
    int omx_buf_idx = (int) pBuffer->pInputPortPrivate;
    port_t*  port = &comp->port[idx];
    ALOG_ADIL_FLOW("[IN] %X.%d [%s] EmptyThisBuffer bh=%X len=%d/%d data=%X %s\n", hComponent, idx, comp->dbg_name, (int) pBuffer, pBuffer->nFilledLen, pBuffer->nAllocLen, (int) pBuffer->pBuffer, pBuffer->nFlags & OMX_BUFFERFLAG_EOS ? "EOS" : "");
    if (!pBuffer) {
        ALOG_ERR("%X.%d [%s] EmptyThisBuffer given NULL buffer header\n", (int) hComponent, idx, comp->dbg_name);
        res = OMX_ErrorBadParameter;
    } else if (!pBuffer->pBuffer) {
        ALOG_ERR("%X.%d [%s] EmptyThisBuffer given NULL pointer as buffer\n", (int) hComponent, idx, comp->dbg_name);
        res = OMX_ErrorBadParameter;
    } else if (pBuffer->nFilledLen > pBuffer->nAllocLen) {
        ALOG_ERR("%X.%d  [%s] EmptyThisBuffer: nFilledLen > nAllocLen in buffer %X\n", (int) hComponent, idx, comp->dbg_name, (int) pBuffer);
        res = OMX_ErrorBadParameter;
    } else if (!port->omx_buf[omx_buf_idx].busy_in_peer) {
        ALOG_ERR("%X.%d [%s] EmptyThisBuffer: Buffer %X already owned by IL\n", (int) hComponent, idx, comp->dbg_name, (int) pBuffer);
        res = OMX_ErrorBadParameter;
    } else if (pBuffer->nFilledLen % (port->format.chan_cfg * 2U) != 0) {
        // TODO: Check new sample unit sizes as required by 32->16 step after SRC
        ALOG_ERR("ADM IL: Error, EmptyThisBuffer called with split frames. nFilledLen=%d chan_cfg=%d\n", pBuffer->nFilledLen, port->format.chan_cfg);
        res = OMX_ErrorBadParameter;
    } else {
        assert(!port->omx_buf[omx_buf_idx].adil_buf);
        assert(!port->omx_buf[omx_buf_idx].to_be_returned);
        assert(port->omx_buf[omx_buf_idx].busy_in_peer);
        assert_ptr(pBuffer->pBuffer); // this does more checks on the pointer in addition to != NULL
        port->omx_buf[omx_buf_idx].busy_in_peer = 0;

        // Need to keep track of order buffers are given by client, so we
        // can give them back in same order. Otherwise client gets confused.
        OMXBufQueue_Push(&port->omx_buf_queue, &port->omx_buf[omx_buf_idx]);

        if (comp->target_state == OMX_StateIdle) {
            ALOG_ADIL("ADMIL: EmptyThisBuffer, Idle or going Idle, ReturnBuffers\n");
            ReturnBuffers(comp,port);
        } else if (port->target_state == 0 || comp->target_state == OMX_StateLoaded) {
            ALOG_ADIL("EmptyThisBuffer, port busy or comp to Loaded\n");
            DropPortResources(comp,idx);
        } else {
            buf_t* buf = BufPool_AllocHdr();
            buf->eos = 0;
            buf->format = port->format;
            buf->data = (char*) pBuffer->pBuffer;

            port->isEos = (pBuffer->nFlags & OMX_BUFFERFLAG_EOS) ? 1u : 0u;

            if (port->omx_format.nChannels == 6 && !is_hdmi_format(&port->omx_format)) {
                // Internally, only handle 5.1 audio in HDMI channel order to reduce complexity
                ALOG_ADIL_INT("Re-arranging 5.1 channels on input\n");
                InplaceMoveChannels((short*) pBuffer->pBuffer, pBuffer->nFilledLen/12, &port->omx_format, 1);
            }

            ALOG_ADIL_INT("ADM IL EmptyThisBuffer new pool-buffer, %X %X\n", (int) buf, (int) buf->data);
            buf->bytes_valid = pBuffer->nFilledLen;
            buf->bytes_available = pBuffer->nAllocLen;
            if (port->peer) {
                // tunneled, so FillThisBuffer should be called on peer
                // component to return buffer
                buf->dtor = buf_dtor_FillThisBuffer;
            } else {
                // non-tunneled, issue EmptyBufferDone callback to client
                buf->dtor = buf_dtor_EmptyBufferDone;
            }
            buf->comp = (comp_t*) hComponent;
            buf->dtor_param2 = (void*) pBuffer;
#ifdef ADM_MMPROBE
            // check if port should be probed using mmprobe
            if (port->pcm_probe_enabled) {
                DataFormat_t data_format;
                omx_format2mmprobe_format(port->omx_format, &data_format);
                ALOG_ADIL_FLOW("probe pcm data, probe_id=%d, data=%d, bytes=%d, format=%d,%d,%d,%d",
                    port->pcm_probe_id, pBuffer->pBuffer, pBuffer->nFilledLen,
                    data_format.SampleRate, data_format.NoChannels,
                    data_format.Interleave, data_format.Resolution);
                mmprobe_probe_V2(port->pcm_probe_id, pBuffer->pBuffer, pBuffer->nFilledLen, &data_format);
            }
#endif // ADM_MMPROBE
            adil_log_data(hComponent, pBuffer, idx, 1);

            port->omx_buf[omx_buf_idx].adil_buf = buf;
            port->underrun_time = 0;
            Port_PutBuf(port, buf);
            Comp_SetDirty(comp);
        }

        IL_Core_Process();
    }
    PERF_LOG("IN EmptyThisBuffer DONE");
    perf_dump();
    pthread_mutex_unlock(&mutex);
    queueExecute();

    return res;
}


static  OMX_ERRORTYPE adm_omxcore_FillThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    perf_reset();
    PERF_LOG("IN FillThisBuffer");
    comp_t* comp = (comp_t*) hComponent;
    OMX_U32 idx = pBuffer->nOutputPortIndex;
    port_t* port = &comp->port[idx];
    int omx_buf_idx = (int) pBuffer->pOutputPortPrivate;
    ALOG_ADIL_FLOW("[IN] %X.%d [%s] FillThisBuffer omx_buf_idx=%d buf=%X\n", (int) hComponent, (int) pBuffer->nOutputPortIndex, comp->dbg_name, omx_buf_idx, (int) pBuffer);
    assert_ptr(pBuffer);
    assert_ptr(pBuffer->pBuffer);
    assert(pBuffer->nFilledLen <= pBuffer->nAllocLen);
    assert(!port->omx_buf[omx_buf_idx].to_be_returned);

    if (port->omx_buf[omx_buf_idx].shared_buf) {
        Buf_Release(port->omx_buf[omx_buf_idx].shared_buf);
        port->omx_buf[omx_buf_idx].shared_buf = 0;
    }

    if (!port->omx_buf[omx_buf_idx].busy_in_peer) {
        ALOG_WARN("FillThisBuffer called by buffer that was not marked as busy in peer\n");
    }

    port->omx_buf[omx_buf_idx].adil_buf = 0;
    port->omx_buf[omx_buf_idx].busy_in_peer = 0;
    pBuffer->nFlags = 0;

    // Need to keep track of order buffers are given by client, so we
    // can give them back in same order. Otherwise client gets confused.
    OMXBufQueue_Push(&port->omx_buf_queue, &port->omx_buf[omx_buf_idx]);

    if (comp->target_state == OMX_StateIdle) {
        ALOG_ADIL("%X.%u bh=%X FillThisBuffer, Idle or going Idle, ReturnBuffers\n", (int) hComponent, pBuffer->nOutputPortIndex, (int) pBuffer);
        ReturnBuffers(comp,port);
        check_comp_state(comp);
    } else if (port->target_state == 0 || comp->target_state == OMX_StateLoaded) {
        DropPortResources(comp,idx);
    } else {
        Comp_SetDirty(comp);
    }
    IL_Core_Process();
    PERF_LOG("IN FillThisBuffer DONE");
    perf_dump();
    pthread_mutex_unlock(&mutex);
    queueExecute();

    return OMX_ErrorNone;
}


static OMX_ERRORTYPE adm_omxcore_GetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    (void) hComponent;
    (void) pIndexType;
    if (strcmp(cParameterName, "STE.ADM") == 0) {
        return OMX_ErrorNone;
    }

    return OMX_ErrorNotImplemented;
}


static OMX_ERRORTYPE adm_omxcore_AllocateBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    comp_t* comp = (comp_t*) hComponent;
    OMX_ERRORTYPE res;
    OMX_U8* pBuffer = malloc(nSizeBytes);
    if (!pBuffer) {
        res = OMX_ErrorInsufficientResources;
        ALOG_ERR("malloc(%d) failed\n", nSizeBytes);
    } else {
        res = allocate_single_buffer(hComponent, ppBuffer, nPortIndex,
                                     pAppPrivate, nSizeBytes, pBuffer, 1);
        if (res != OMX_ErrorNone) {
            ALOG_ERR("AllocateBuffer: allocate_single_buffer failed\n");
            free(pBuffer);
        } else {
            ALOG_ADIL("[IN] %X.%u [%s] AllocateBuffer size=%d bh=%X buf=%X\n", (int) hComponent, nPortIndex, comp->dbg_name, (int) *ppBuffer, (int) pBuffer);
        }
    }

    IL_Core_Process();
    pthread_mutex_unlock(&mutex);
    queueExecute();

    return res;
}


static OMX_ERRORTYPE adm_omxcore_UseBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    comp_t* comp = (comp_t*) hComponent;
    ALOG_ADIL("[IN] %X.%d [%s] UseBuffer: nSizeBytes=%d buf=%X", (int) hComponent, nPortIndex, comp->dbg_name, nSizeBytes, (int) pBuffer);
    OMX_ERRORTYPE res = allocate_single_buffer(hComponent, ppBuffer, nPortIndex,
                                          pAppPrivate, nSizeBytes, pBuffer, 0);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("UseBuffer failed, %s\n", adm_log_err2str(res));
    } else {
        ALOG_ADIL_INT("UseBuffer resulted in buffer header %X\n", (int) *ppBuffer);
    }
    IL_Core_Process();
    pthread_mutex_unlock(&mutex);
    queueExecute();

    return res;
}


static OMX_ERRORTYPE adm_omxcore_FreeBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    comp_t* comp = (comp_t*) hComponent;
    ALOG_ADIL("[IN] %X.%u [%s] FreeBuffer bh=%X\n", (int) hComponent, nPortIndex, comp->dbg_name, (int) pBuffer);

    port_t* port = &comp->port[nPortIndex];

    int buf_idx = (int) (port->isInput ? pBuffer->pInputPortPrivate : pBuffer->pOutputPortPrivate);
    ALOG_ADIL_INT("buf_idx = %d\n", buf_idx);
    if (port->omx_buf[buf_idx].adil_buf || !port->omx_buf[buf_idx].allocated) {
        ALOG_ERR("ERROR - OMX_FreeBuffer called for adil_buf != 0 (%X) or not allocated (%d) buffer! omx_buf_idx=%d\n", (int) port->omx_buf[buf_idx].adil_buf, port->omx_buf[buf_idx].allocated, buf_idx);
        pthread_mutex_unlock(&mutex);
        return OMX_ErrorBadParameter;
    }

    if (port->omx_buf[buf_idx].adil_buf) {
        // Actual buffer is busy elsewhere inside ADM.
        if (port->target_state != 0) {
            ALOG_ERR("ADM IL FreeBuffer called for busy buffer while not disabling port\n");
            pthread_mutex_unlock(&mutex);
            return OMX_ErrorBadParameter;
        }

        assert(0);
    } else {
        port->omx_buf[buf_idx].allocated = 0;
        if (port->omx_buf[buf_idx].allocated_by_us) {
            if (port->omx_buf[buf_idx].original_buf) {
                free(port->omx_buf[buf_idx].original_buf);
                port->omx_buf[buf_idx].original_buf = 0;
            }
        }
        free(pBuffer);
    }

    check_comp_state(comp);
    IL_Core_Process();
    pthread_mutex_unlock(&mutex);
    queueExecute();

    return OMX_ErrorNone;
}


static OMX_ERRORTYPE adm_omxcore_GetState(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState)
{
    pthread_mutex_lock(&mutex);
    *pState = ((comp_t*) hComponent)->state;
    pthread_mutex_unlock(&mutex);

    return OMX_ErrorNone;
}


static OMX_ERRORTYPE adm_omxcore_GetComponentVersion(
        OMX_HANDLETYPE hComponent,
        OMX_STRING pComponentName,
        OMX_VERSIONTYPE* pComponentVersion,
        OMX_VERSIONTYPE* pSpecVersion,
        OMX_UUIDTYPE* pComponentUUID)
{
    (void) hComponent;
    (void) pComponentName;
    (void) pComponentVersion;
    (void) pSpecVersion;
    (void) pComponentUUID;

    return OMX_ErrorNotImplemented;
}


static OMX_ERRORTYPE is_peer_port_compatible(const port_t* port,
                                             const OMX_PARAM_PORTDEFINITIONTYPE* remote_portdef,
                                             const OMX_AUDIO_PARAM_PCMMODETYPE*  remote_pcmmode,
                                             OMX_HANDLETYPE dbg_hComp, OMX_U32 dbg_nPort)
{
    if (remote_portdef->eDomain != OMX_PortDomainAudio) {
        ALOG_ERR("Port %X.%d has wrong port domain (%d)\n", dbg_hComp, dbg_nPort, remote_portdef->eDomain);
        return OMX_ErrorPortsNotCompatible;
    }

    if (remote_portdef->format.audio.eEncoding != OMX_AUDIO_CodingPCM) {
        ALOG_ERR("Port %X.%d has wrong audio encoding (%d)\n", dbg_hComp, dbg_nPort, remote_portdef->format.audio.eEncoding);
        return OMX_ErrorPortsNotCompatible;
    }

    if ((port->isInput && !remote_portdef->eDir == OMX_DirOutput) || (!port->isInput && !remote_portdef->eDir == OMX_DirInput)) {
        ALOG_ERR("Port %X.%d: input to input or output to output connection\n", dbg_hComp, dbg_nPort);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.nChannels != remote_pcmmode->nChannels) {
        ALOG_ERR("Port %X.%d has wrong number of channels, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->nChannels, port->omx_format.nChannels);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.eNumData != remote_pcmmode->eNumData) {
        ALOG_ERR("Port %X.%d has wrong eNumData, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->eNumData, port->omx_format.eNumData);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.eEndian != remote_pcmmode->eEndian) {
        ALOG_ERR("Port %X.%d has wrong eEndian, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->eEndian, port->omx_format.eEndian);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.bInterleaved != remote_pcmmode->bInterleaved) {
        ALOG_ERR("Port %X.%d has wrong bInterleaved, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->bInterleaved, port->omx_format.bInterleaved);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.nBitPerSample != remote_pcmmode->nBitPerSample) {
        ALOG_ERR("Port %X.%d has wrong bits per sample, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->nBitPerSample, port->omx_format.nBitPerSample);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.nSamplingRate != remote_pcmmode->nSamplingRate) {
        ALOG_ERR("Port %X.%d has wrong sample rate, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->nSamplingRate, port->omx_format.nSamplingRate);
        return OMX_ErrorPortsNotCompatible;
    }

    if (port->omx_format.ePCMMode != remote_pcmmode->ePCMMode) {
        ALOG_ERR("Port %X.%d has ePCMMode, %d != %d\n", dbg_hComp, dbg_nPort, remote_pcmmode->ePCMMode, port->omx_format.ePCMMode);
        return OMX_ErrorPortsNotCompatible;
    }

    OMX_U32 i;
    for (i = 0 ; i < port->omx_format.nChannels ; i++ ){
        if (port->omx_format.eChannelMapping[i] != remote_pcmmode->eChannelMapping[i]) {
            ALOG_ERR("Port %X.%d has different channel mapping on chan %d: %d != %d\n", dbg_hComp, dbg_nPort, i, remote_pcmmode->eChannelMapping[i], port->omx_format.eChannelMapping[i]);
            return OMX_ErrorPortsNotCompatible;
        }
    }

    return OMX_ErrorNone;
}


static OMX_ERRORTYPE adm_omxcore_ComponentTunnelRequest(
        OMX_HANDLETYPE hComp, OMX_U32 nPort, OMX_HANDLETYPE hTunneledComp,
        OMX_U32 nTunneledPort, OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
    g_dbg_dumpthread_dumpdog = 0;
    OMX_ERRORTYPE res = OMX_ErrorNone;

    OMX_AUDIO_PARAM_PCMMODETYPE remote_pcmmode;
    INIT_CONFIG_STRUCT(remote_pcmmode);

    OMX_PARAM_PORTDEFINITIONTYPE remote_portdef;
    INIT_CONFIG_STRUCT(remote_portdef);

    int update_supplier = 0;
    OMX_PARAM_BUFFERSUPPLIERTYPE bufsup;
    INIT_CONFIG_STRUCT(bufsup);

    // All calls to the other component are done while not holding the
    // ADM core mutex.
    int is_adm_comp = 0;
    OMX_INDEXTYPE dummy;
    if (hTunneledComp && OMX_GetExtensionIndex(hTunneledComp, "STE.ADM", &dummy) == OMX_ErrorNone) {
        is_adm_comp = 1;
    }

    if (hTunneledComp && !is_adm_comp) {
        remote_portdef.nPortIndex = nTunneledPort;
        res = OMX_GetParameter(hTunneledComp, OMX_IndexParamPortDefinition, &remote_portdef);
        if (res != OMX_ErrorNone) {
            ALOG_ERR("CTR OMX_GetParameter OMX_IndexParamPortDefinition failed on %X.%d: %s\n", hComp, nPort, adm_log_err2str(res));
            return res;
        }

        remote_pcmmode.nPortIndex = nTunneledPort;
        res = OMX_GetParameter(hTunneledComp, OMX_IndexParamAudioPcm, &remote_pcmmode);
        if (res != OMX_ErrorNone) {
            ALOG_ERR("CTR OMX_GetParameter OMX_IndexParamAudioPcm failed on %X.%d: %s\n", hComp, nPort, adm_log_err2str(res));
            return res;
        }
    }

    // Aquire mutex, set up tunnel if acceptable
    pthread_mutex_lock(&mutex);
    comp_t* cur = (comp_t*) hComp;
    if (nPort > cur->ports) {
        ALOG_ERR("ComponentTunnelRequest: port %X.%d invalid\n", (int) hComp, (int) nPort);
        res = OMX_ErrorBadParameter;
    } else {
        port_t* port = &cur->port[nPort];
        port->peer_portdef = remote_portdef;
        if (hTunneledComp == NULL) {
            if (cur->state != OMX_StateLoaded && port->isEnabled) {
                ALOG_ERR("Error, trying to untunnel active port %X.%d\n", (int) hComp, (int) nPort);
                res = OMX_ErrorIncorrectStateOperation;
            } else {
                ALOG_ADIL("Untunneling %X.%d\n", (int) hComp, (int) nPort);
                cur->port[nPort].peer = 0;
            }
        } else if (is_adm_comp) {
            ALOG_ADIL_INT("Tunnel: Direct connection %X.%d --> %X.%d\n", (int) hComp, (int) nPort, (int) hTunneledComp, (int) nTunneledPort);
            port->isDirect = 1;
            port->peer = hTunneledComp;
            port->peerport = nTunneledPort;
        } else {
            res = is_peer_port_compatible(port, &port->peer_portdef, &remote_pcmmode, hTunneledComp, nTunneledPort);
            if (res == OMX_ErrorNone) {
                if (port->isInput) {
                    // Input port, called after output port in SetupTunnel sequence
                    cur->port[nPort].peer = hTunneledComp;
                    cur->port[nPort].peerport = nTunneledPort;
                    cur->port[nPort].isSupplier = (pTunnelSetup->eSupplier == OMX_BufferSupplyInput);

                    bufsup.eBufferSupplier = pTunnelSetup->eSupplier;
                    bufsup.nPortIndex = nTunneledPort;
                    update_supplier = 1;

                    ALOG_ADIL("%X.%u [%s] TunnelReq input %X.%d --> %X.%d, %s\n", (int) cur, (int) nPort, cur->dbg_name, (int) hTunneledComp, (int) nTunneledPort, (int) hComp, (int) nPort,
                                    pTunnelSetup->eSupplier == OMX_BufferSupplyOutput ? "OMX_BufferSupplyOutput" : "OMX_BufferSupplyInput");
                } else {
                    // we are output port. Output port is called first in SetupTunnel sequence.
                    // In LPA, we want input port to provide us the buffers to avoid a memcpy() step
                    // (they will already be mapped on the DSP if provided by DSP port)
                    pTunnelSetup->eSupplier = OMX_BufferSupplyOutput;
                    cur->port[nPort].peer = hTunneledComp;
                    cur->port[nPort].peerport = nTunneledPort;

                    ALOG_ADIL("%X.%d [%s] TunnelReq output --> %X.%d, we suggest %s\n", (int) hComp, (int) nPort, cur->dbg_name, (int) hTunneledComp, (int) nTunneledPort,
                                    pTunnelSetup->eSupplier == OMX_BufferSupplyOutput ? "OMX_BufferSupplyOutput" : "OMX_BufferSupplyInput");
                }
            } //if (res == OMX_ErrorNone)
        } //if (hTunneledComp == NULL)
    } //if (nPort > cur->ports)

    pthread_mutex_unlock(&mutex);

    if (res == OMX_ErrorNone && update_supplier) {
        res = OMX_SetParameter(hTunneledComp, OMX_IndexParamCompBufferSupplier, &bufsup);
        if (res != OMX_ErrorNone) {
            ALOG_ERR("OMX_SetParameter %X.%d failed during ComponentTunnelRequest, %s\n", (int) hTunneledComp, (int) nTunneledPort, adm_log_err2str(res));
        }
    }

    return res;
}




///////////////////////////////////////////////////////////////////////////////
//
// Component
//
///////////////////////////////////////////////////////////////////////////////
static void Comp_SetDirty(comp_t* comp)
{
    assert_ptr(comp);
    dirty_comp_mask |= comp->dirty_bit;
}


static OMX_ERRORTYPE Comp_onSetGet(comp_t* comp, OMX_INDEXTYPE nIndex, OMX_PTR pParamRaw, int isSet, int isParam)
{
    OMX_ERRORTYPE res = OMX_ErrorUnsupportedIndex;

    switch ((int)nIndex) {
        case OMX_IndexParamAudioPcm: {
            ONSETGET_PARAMETER_WITH_PORT_COMMON_CODE(OMX_AUDIO_PARAM_PCMMODETYPE);
            if (isSet) {
                res = Port_UpdatePcmMode(port, pParam);
                if (res == OMX_ErrorNone) {
                    port->omx_format = *pParam;
                }
            } else {
                *pParam = port->omx_format;
                res = OMX_ErrorNone;
            }
            break;
        }

        case OMX_IndexParamCompBufferSupplier: {
            ONSETGET_PARAMETER_WITH_PORT_COMMON_CODE(OMX_PARAM_BUFFERSUPPLIERTYPE);
            if (isSet) {
                // TODO: Only allow on output port if not tunneled? If tunneled, forward from
                // input to output?
                port->isSupplier = 0;
                if (pParam->eBufferSupplier == OMX_BufferSupplyInput) {
                    if (port->isInput) {
                        port->isSupplier = 1;
                    }
                    ALOG_ADIL("[IN] %X.%d [%s] SetParameter OMX_BufferSupplyInput\n", comp, pParam->nPortIndex, comp->dbg_name);
                } else {
                    if (!port->isInput) {
                        port->isSupplier = 1;
                    }
                    ALOG_ADIL("[IN] %X.%d [%s] SetParameter OMX_BufferSupplyOutput\n", comp, pParam->nPortIndex, comp->dbg_name);
                }
            } else {
                if (port->isDirect) {
                    // ADM IL client will enable supplier last. Setting supplier to output
                    // ensures that the input is enabled and ready to receive data when output
                    // is enabled.
                    pParam->eBufferSupplier = OMX_BufferSupplyOutput;
                } else {
                    if (port->isInput) {
                        if (port->isSupplier) pParam->eBufferSupplier = OMX_BufferSupplyInput;
                        else                  pParam->eBufferSupplier = OMX_BufferSupplyOutput;
                    } else {
                        if (port->isSupplier) pParam->eBufferSupplier = OMX_BufferSupplyOutput;
                        else                  pParam->eBufferSupplier = OMX_BufferSupplyInput;
                    }
                }
            }
            res = OMX_ErrorNone;
            break;
        }

        case OMX_IndexParamPortDefinition: {
            ONSETGET_PARAMETER_WITH_PORT_COMMON_CODE(OMX_PARAM_PORTDEFINITIONTYPE);
            res = OMX_ErrorNone;
            if (isSet) {
                if (pParam->nBufferCountActual == 0 || pParam->nBufferCountActual > 4) {
                    res = OMX_ErrorBadParameter;
                } else if (pParam->nBufferSize == 0 || pParam->nBufferSize > 65536) {
                    // TODO: Check % 64 == 0 or something.
                    res = OMX_ErrorBadParameter;
                } else {
                    port->portdef.nBufferCountActual = pParam->nBufferCountActual;
                    port->portdef.nBufferSize        = pParam->nBufferSize;
                    port->portdef.nBufferCountMin    = port->portdef.nBufferCountActual;
                }
            } else {
                *pParam = port->portdef;
                // Need to report state we are transitioning to, not the one we are on.
                // ENS checks this to make sure we asked for enable/disable on non-supplier/
                // supplier first.
                pParam->bEnabled = port->target_state;
                pParam->eDir     = port->isInput ? OMX_DirInput : OMX_DirOutput;
            }
            break;
        }

        case OMX_IndexParamAudioInit: {
            ONSETGET_PARAMETER_COMMON_CODE(OMX_PORT_PARAM_TYPE);
            if (!isSet) {
                pParam->nPorts = comp->ports;
                pParam->nStartPortNumber = 0;
                res = OMX_ErrorNone;
            }
            break;
        }

        case OMX_IndexParamVideoInit:
        case OMX_IndexParamImageInit:
        case OMX_IndexParamOtherInit: {
            ONSETGET_PARAMETER_COMMON_CODE(OMX_PORT_PARAM_TYPE);
            if (!isSet) {
                pParam->nPorts = 0;
                res = OMX_ErrorNone;
            }
            break;
        }

        case OMX_IndexParamAudioSynchronized: {
            ONSETGET_CONFIG_WITH_PORT_COMMON_CODE(OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE);
            if (isSet) {
                port->silenceOnUnderrun = !pParam->bIsSynchronized;
                ALOG_ADIL("[IN] %X.%d [%s] SetParameter OMX_IndexParamAudioSynchronized --> silenceOnUnderrun=%d\n", comp, pParam->nPortIndex, comp->dbg_name, port->silenceOnUnderrun);
            } else {
                pParam->bIsSynchronized = !port->silenceOnUnderrun;
            }
            res = OMX_ErrorNone;
            break;
        }

        case ADM_IndexConfigStartupJitterMargin: {
            ONSETGET_CONFIG_WITH_PORT_COMMON_CODE(ADM_CONFIG_STARTUPJITTERMARGINTYPE);
            if (isSet) {
                ALOG_ADIL("[IN] %X.%d [%s] SetConfig ADM_IndexConfigStartupJitterMargin --> startupJitterMargin=%d\n", comp, pParam->nPortIndex, comp->dbg_name, pParam->startupJitterMargin);
                port->startup_margin = pParam->startupJitterMargin;
            } else {
                pParam->startupJitterMargin = port->startup_margin;
            }
            res = OMX_ErrorNone;
            break;
        }

        case ADM_IndexConfigLatencySettings: {
            ONSETGET_CONFIG_WITH_PORT_COMMON_CODE(ADM_CONFIG_LATENCYSETTINGTYPE);
            if (isSet) {
                if (pParam->nbrBuffersToUse > port->num_omx_buf) {
                    ALOG_ERR("SetParameter ADM_IndexConfigLatencySettings nbrBuffersToUse has invalid value %d", pParam->nbrBuffersToUse);
                    return OMX_ErrorBadParameter;
                }
                port->nbr_of_buffers_to_use = pParam->nbrBuffersToUse;
                port->buffer_fill_time      = pParam->bufferFillTime;
            }
            else {
                pParam->nbrBuffersToUse = port->nbr_of_buffers_to_use;
                pParam->bufferFillTime  = port->buffer_fill_time;
            }
            res = OMX_ErrorNone;
            break;
        }

        case AFM_IndexParamSrcMode: {
            ONSETGET_PARAMETER_WITH_PORT_COMMON_CODE(AFM_AUDIO_PARAM_SRC_MODE);
            if (isSet) {
                res = OMX_ErrorNone;
                SRC_QUALITY_t mode;
                switch (pParam->nSrcMode) {
                    case AFM_SRC_MODE_CUSTOM1: mode = SRC_LOW_RIPPLE;       break;
                    case AFM_SRC_MODE_NORMAL:  mode = SRC_STANDARD_QUALITY; break;
                    default:                   res = OMX_ErrorBadParameter; break;
                }

                if (res == OMX_ErrorNone) {
                    comp->port[pParam->nPortIndex].SRC_mode = mode;
                    ALOG_ADIL("[IN] %X.%d [%s] SetParam AFM_IndexParamSrcMode --> nSrcMode=%d\n", comp, pParam->nPortIndex, comp->dbg_name, pParam->nSrcMode);
                }
            } else {
                switch (comp->port[pParam->nPortIndex].SRC_mode) {
                    case SRC_STANDARD_QUALITY: pParam->nSrcMode = AFM_SRC_MODE_NORMAL;  break;
                    case SRC_LOW_RIPPLE:       pParam->nSrcMode = AFM_SRC_MODE_CUSTOM1; break;
                    default: assert(0);
                }
                res = OMX_ErrorNone;
            }
            break;
        }

#ifdef ADM_MMPROBE
        case AFM_IndexConfigPcmProbe: {
            ONSETGET_CONFIG_WITH_PORT_COMMON_CODE(AFM_AUDIO_CONFIG_PCM_PROBE);
            if (isSet) {
                port->pcm_probe_enabled = pParam->bEnable;
                port->pcm_probe_id = pParam->nProbeIdx;
            } else {
                pParam->bEnable = port->pcm_probe_enabled;
                pParam->nProbeIdx = port->pcm_probe_id;
            }
            res = OMX_ErrorNone;
            break;
        }
#endif // ADM_MMPROBE

        default: {
            if (comp->onSetGet) {
                res = comp->onSetGet(comp, nIndex, pParamRaw, isSet, isParam);
            }
        } //switch (nIndex)
    }

    return res;
}


static void Comp_Init(struct comp* comp)
{
    comp->omx.nSize = sizeof(comp->omx);

    comp->omx.SendCommand            = adm_omxcore_SendCommand;
    comp->omx.GetParameter           = adm_omxcore_GetParameter;
    comp->omx.GetConfig              = adm_omxcore_GetConfig;
    comp->omx.SetParameter           = adm_omxcore_SetParameter;
    comp->omx.SetConfig              = adm_omxcore_SetConfig;
    comp->omx.EmptyThisBuffer        = adm_omxcore_EmptyThisBuffer;
    comp->omx.FillThisBuffer         = adm_omxcore_FillThisBuffer;
    comp->omx.GetExtensionIndex      = adm_omxcore_GetExtensionIndex;
    comp->omx.AllocateBuffer         = adm_omxcore_AllocateBuffer;
    comp->omx.FreeBuffer             = adm_omxcore_FreeBuffer;
    comp->omx.UseBuffer              = adm_omxcore_UseBuffer;
    comp->omx.GetState               = adm_omxcore_GetState;
    comp->omx.GetComponentVersion    = adm_omxcore_GetComponentVersion;
    comp->omx.ComponentTunnelRequest = adm_omxcore_ComponentTunnelRequest;

    comp->target_state = comp->state = OMX_StateLoaded;

    OMX_U32 i;
    for (i = 0 ; i < MAX_PORTS ; i++) {
        INIT_CONFIG_STRUCT(comp->port[i].omx_format);
        comp->port[i].omx_format.nPortIndex = i;
        comp->port[i].omx_format.nChannels = 2;
        comp->port[i].omx_format.eNumData = OMX_NumericalDataSigned;
        comp->port[i].omx_format.eEndian = OMX_EndianLittle;
        comp->port[i].omx_format.bInterleaved = OMX_TRUE;
        comp->port[i].omx_format.nBitPerSample = 16;
        comp->port[i].omx_format.nSamplingRate = 48000;
        comp->port[i].omx_format.ePCMMode = OMX_AUDIO_PCMModeLinear;
        comp->port[i].omx_format.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
        comp->port[i].omx_format.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

        INIT_CONFIG_STRUCT(comp->port[i].portdef);
        comp->port[i].portdef.nPortIndex  = i;
        comp->port[i].portdef.eDir = OMX_DirInput;

        comp->port[i].comp = comp;
        comp->port[i].idx  = i;

        // In ENS, this is the number of buffers the port will have
        comp->port[i].portdef.nBufferCountActual = 3;
        comp->port[i].portdef.nBufferCountMin = comp->port[i].portdef.nBufferCountActual;
        comp->port[i].portdef.nBufferSize = 4096;
        comp->port[i].portdef.bEnabled = 1;
        comp->port[i].portdef.bPopulated = 0;
        comp->port[i].portdef.eDomain = OMX_PortDomainAudio;
        comp->port[i].portdef.bBuffersContiguous = 0;
        comp->port[i].portdef.nBufferAlignment = 0;
        comp->port[i].portdef.format.audio.eEncoding = OMX_AUDIO_CodingPCM;

        (void) Port_UpdatePcmMode(&comp->port[i], &comp->port[i].omx_format);
        BufQueue_Init(&comp->port[i].bq);
        OMXBufQueue_Init(&comp->port[i].omx_buf_queue);

        comp->port[i].target_state = 1; // enabled
        comp->port[i].isEnabled = 1;
        comp->port[i].isInput = 1;
        comp->port[i].cur_src_buf = NULL;
        comp->port[i].cur_dst_buf = NULL;
        comp->port[i].SRC_block_size = 6144 * 2; // TODO
        comp->port[i].SRC_mode = SRC_STANDARD_QUALITY;
        comp->port[i].il_method = IL_METHOD_NONE;
        comp->port[i].nbr_of_buffers_to_use = 0;
        comp->port[i].buffer_fill_time = 0;

        comp->port[i].underrun_time = 0;
    }
}


OMX_ERRORTYPE adm_omxcore_OMX_GetHandle(
    OMX_OUT OMX_HANDLETYPE* pHandle,
    OMX_IN  OMX_STRING cComponentName,
    OMX_IN  OMX_PTR pAppData,
    OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
{
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);

    int i;
    comp_t** comp_pp = NULL;
    for (i = 0 ; i < MAX_COMPS ; i++) {
        if (!comps[i]) {
            comp_pp = &comps[i];
            break;
        }
    }

    typedef OMX_ERRORTYPE (*Factory_fp)(comp_t**, const char* name);
    static const Factory_fp factory[] = {
        Mixer_Factory,
        Splitter_Factory,
#ifdef ADM_ENABLE_FEATURE_FAT_MODEM
        modem_src_Factory,
        modem_sink_Factory,
#endif
        NULL
    };

    const Factory_fp* cur_factory = &factory[0];
    OMX_ERRORTYPE res = OMX_ErrorComponentNotFound;
    while (*cur_factory && res == OMX_ErrorComponentNotFound) {
        res = (*cur_factory++)(comp_pp, cComponentName);
    }

    if (res == OMX_ErrorNone) {
        (*comp_pp)->dirty_bit               = 1U<<i;
        (*comp_pp)->omx.pApplicationPrivate = pAppData;
        (*comp_pp)->cb                      = *pCallBacks;
        strcpy((*comp_pp)->dbg_name, cComponentName);
        *pHandle = (OMX_HANDLETYPE) (*comp_pp);
        ALOG_ADIL("[IN] GetHandle %s --> %X\n", cComponentName, (int) *pHandle);
    }

    pthread_mutex_unlock(&mutex);

    return res;
}


OMX_ERRORTYPE adm_omxcore_OMX_FreeHandle(OMX_HANDLETYPE hComponent)
{
    ALOG_STATUS("adm_omxcore_OMX_FreeHandle start\n");
    OMX_ERRORTYPE res = OMX_ErrorBadParameter;
    g_dbg_dumpthread_dumpdog = 0;
    pthread_mutex_lock(&mutex);
    int i;

    for (i = 0 ; i < MAX_COMPS ; i++) {
        if (comps[i] && comps[i] == hComponent) {
        	ALOG_STATUS("find hComponent i:%d\n",i);
            if (comps[i]->state != comps[i]->target_state ||
                comps[i]->state != OMX_StateLoaded)
            {
            	ALOG_STATUS("OMX_ErrorIncorrectStateOperation comps[i]->state:%d\n",comps[i]->state);
                res = OMX_ErrorIncorrectStateOperation;
            } else {
            	 ALOG_STATUS("[IN] %X [%s] FreeHandle\n", (int) hComponent, comps[i]->dbg_name);
                ALOG_ADIL("[IN] %X [%s] FreeHandle\n", (int) hComponent, comps[i]->dbg_name);
                BufPool_RemoveComponentReference(comps[i]);
                if (comps[i]->dtor) comps[i]->dtor(comps[i]);
                unsubscribe_timer_event(comps[i]);
                free(comps[i]);
                comps[i] = 0;
                res = OMX_ErrorNone;
            }
            break;
        }
    } //for (i = 0 ; i < MAX_COMPS ; i++)

    pthread_mutex_unlock(&mutex);

    return res;
}

static void *dump_thread(void*a)
{
    (void) a;

    while(g_dbg_dumpthread_alive) {
        if (g_dbg_dumpthread_dumpdog == 0) {
            g_dbg_dumpthread_dumpdog = 1;
        }
        usleep(5 * 1000 * 1000); // g_dbg_dumpthread_dumpdog -> 0 on action
        if (g_dbg_dumpthread_dumpdog == 1) {
            // If nobody cleared it, and any component exists we dump info
            int ci;
            int comp_exists = 0;
            pthread_mutex_lock(&mutex);
            for (ci = 0 ; ci < MAX_COMPS ; ci++) {
                if (comps[ci]) {
                    comp_exists = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);

            if (comp_exists) {
                ALOG_WARN("ADM il core has seen no activity for at least 5 seconds. Dumping state.");
                g_dbg_dumpthread_dumpdog = 2;
                dbg_print_debug_information();
            }
        }
    }

    return 0;
}


void ste_adm_omx_core_init(void)
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex_outcmd, NULL);
    pthread_mutex_init(&mutex_bufpool, NULL);
    g_timer_wakeup_fd = eventfd(0, 0);

    g_dbg_dumpthread_alive = 1;
    pthread_create(&g_dbg_dumpthread, NULL, dump_thread, NULL);
    g_timer_thread_alive = 1;
    pthread_create(&g_timer_thread, NULL, timer_thread, NULL);

    BufPool_Init();
}


void ste_adm_omx_core_deinit(void)
{
    g_timer_thread_alive = 0;
    const unsigned long long val = 1;
    write(g_timer_wakeup_fd, &val, sizeof(val));
    pthread_join(g_timer_thread, 0);

    close(g_timer_wakeup_fd);

    pthread_mutex_destroy(&mutex_outcmd);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex_bufpool);
    g_dbg_dumpthread_alive = 0;

    // This takes up to 5 sec, but is only used during debugging
    pthread_join(g_dbg_dumpthread, 0);
}




///////////////////////////////////////////////////////////////////////////////
//
//  Queue for outbound commands - to send them while not holding main mutex
//
///////////////////////////////////////////////////////////////////////////////
static struct out_cmd_item* queueNewAndPush()
{
    if (adm_fifo.items_valid >= ADM_FIFO_SIZE) { ALOG_ADIL_INT("ADM_FIFO_FULL\n"); }
    unsigned int write_pos = (adm_fifo.read_pos + adm_fifo.items_valid) % ADM_FIFO_SIZE;
    adm_fifo.items_valid++;

    return &adm_fifo.fifo[write_pos];
}


static int queuePop(struct out_cmd_item* dst_buf)
{
    if (adm_fifo.items_valid == 0) {
        return 0;
    }
    
    *dst_buf = adm_fifo.fifo[adm_fifo.read_pos];
    adm_fifo.items_valid--;
    adm_fifo.read_pos = (adm_fifo.read_pos + 1) % ADM_FIFO_SIZE;

    return 1;
}


static void queueEmptyThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer)
{
    pthread_mutex_lock(&mutex_outcmd);
    struct out_cmd_item* cmd = queueNewAndPush();
    cmd->cmd = CMD_EMPTY_THIS_BUFFER;
    cmd->param.buf.hComponent = hComponent;
    cmd->param.buf.pBuffer    = pBuffer;
    pthread_mutex_unlock(&mutex_outcmd);
}


static void queueFillThisBuffer(OMX_HANDLETYPE hComponent, OMX_BUFFERHEADERTYPE* pBuffer)
{
    pthread_mutex_lock(&mutex_outcmd);
    struct out_cmd_item* cmd = queueNewAndPush();
    cmd->cmd = CMD_FILL_THIS_BUFFER;
    cmd->param.buf.hComponent = hComponent;
    cmd->param.buf.pBuffer    = pBuffer;
    pthread_mutex_unlock(&mutex_outcmd);
}


static void queueUseBuffer(comp_t* parent_comp, OMX_U32 parent_port_index, uint_t buf_index, OMX_HANDLETYPE hComponent,
            OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer)
{
    pthread_mutex_lock(&mutex_outcmd);
    struct out_cmd_item* cmd = queueNewAndPush();
    cmd->cmd = CMD_USE_BUFFER;
    cmd->param.usebuffer.parent_comp       = parent_comp;
    cmd->param.usebuffer.parent_port_index = parent_port_index;
    cmd->param.usebuffer.buf_index         = buf_index;
    cmd->param.usebuffer.hComponent        = hComponent;
    cmd->param.usebuffer.nPortIndex        = nPortIndex;
    cmd->param.usebuffer.pAppPrivate       = pAppPrivate;
    cmd->param.usebuffer.nSizeBytes        = nSizeBytes;
    cmd->param.usebuffer.pBuffer           = pBuffer;
    pthread_mutex_unlock(&mutex_outcmd);
}


static void queueFreeBuffer(
            OMX_HANDLETYPE hComponent,
            OMX_U32 nPortIndex,
            OMX_BUFFERHEADERTYPE* pBuffer)
{
    pthread_mutex_lock(&mutex_outcmd);
    struct out_cmd_item* cmd = queueNewAndPush();
    cmd->cmd = CMD_FREE_BUFFER;
    cmd->param.freebuffer.hComponent = hComponent;
    cmd->param.freebuffer.nPortIndex = nPortIndex;
    cmd->param.freebuffer.pBuffer    = pBuffer;
    pthread_mutex_unlock(&mutex_outcmd);
}


// To be executed without holding main ADM IL-core mutex.
static void queueExecute()
{
    pthread_mutex_lock(&mutex_outcmd);
    struct out_cmd_item copy;
    while (queuePop(&copy)) {
        OMX_ERRORTYPE res;
        pthread_mutex_unlock(&mutex_outcmd);
        
        switch (copy.cmd) {
            case CMD_USE_BUFFER: {
                OMX_BUFFERHEADERTYPE* pBufHdr = 0;
                res=OMX_UseBuffer(copy.param.usebuffer.hComponent,
                    &pBufHdr, copy.param.usebuffer.nPortIndex,
                    copy.param.usebuffer.pAppPrivate, copy.param.usebuffer.nSizeBytes,
                    copy.param.usebuffer.pBuffer);
                if (res != OMX_ErrorNone) {
                    ALOG_ERR("OMX_UseBuffer failed, %s\n", adm_log_err2str(res));
                }

                pthread_mutex_lock(&mutex);
                UseBufferCompletedCallback(copy.param.usebuffer.parent_comp,
                                           copy.param.usebuffer.parent_port_index,
                                           copy.param.usebuffer.buf_index,
                                           pBufHdr,
                                           (char*) copy.param.usebuffer.pBuffer,
                                           res);
                IL_Core_Process();
                pthread_mutex_unlock(&mutex);
                break;
            }

            case CMD_FREE_BUFFER: {
                res = OMX_FreeBuffer(copy.param.freebuffer.hComponent,
                    copy.param.freebuffer.nPortIndex, copy.param.freebuffer.pBuffer);
                break;
            }

            case CMD_FILL_THIS_BUFFER: {
                res = OMX_FillThisBuffer(copy.param.buf.hComponent, copy.param.buf.pBuffer);
                break;
            }

            case CMD_EMPTY_THIS_BUFFER: {
                res = OMX_EmptyThisBuffer(copy.param.buf.hComponent, copy.param.buf.pBuffer);
                break;
            }

            default: {
                assert(0);
            }
        } //switch (copy.cmd)

        pthread_mutex_lock(&mutex_outcmd);
    } //while (queuePop(&copy))

    pthread_mutex_unlock(&mutex_outcmd);
}


static void ste_adm_omx_core_dump_locked(void)
{
    int any_component=0;
    int ci;
    for (ci = 0 ; ci < MAX_COMPS ; ci++) {
        comp_t* comp = comps[ci];
        if (!comp) continue;
        if (!any_component) {
            dbg_printf("-------------------------------------------------------------------------\n");
            any_component = 1;
        }
        const char* target_state = "";
        if (comp->state != comp->target_state) {
            target_state = adm_log_state2str(comp->target_state);
        }
        dbg_printf(" %X %s state=%s %s%s\n", (int) comp, comp->dbg_name, adm_log_state2str(comp->state),comp->state != comp->target_state ? "-->":"",target_state);
        unsigned int pi;
        for (pi = 0 ; pi < comp->ports ; pi++) {
            port_t* port = &comp->port[pi];
            target_state = "";
            if (port->isEnabled != port->target_state) {
                target_state = port->target_state ? " -->Enabled" : " -->Disabled";
            }
            if (port->isEnabled && port->peer && !port->isDirect) {
                // IL port
                dbg_printf("  .%d IL-tunnel %s %X.%d %s%s, req %d bufs of >= %d bytes, %s %u ch %s%s\n",
                    pi, port->isInput ? "from" : "to", (int) port->peer, port->peerport,
                    port->isSupplier ? "Supplier" : "nonSupplier", target_state,
                    (int) port->portdef.nBufferCountActual, (int) port->portdef.nBufferSize,
                    esaafreq2str(port->format.freq), port->format.chan_cfg,
                    port->isEos ? "EOS " : "", port->SRC_Ready ? "SRC " : "!SRC ");
            } else if (port->isEnabled && port->isDirect && port->peer) {
                dbg_printf("  .%d direct connection %s %X.%d (%s) %s\n", pi, port->isInput ? "from" : "to", (int) port->peer, port->peerport, ((comp_t*) port->peer)->dbg_name, target_state);
            } else if (port->isEnabled && !port->isDirect && !port->peer) {
                dbg_printf("  .%d IL basic %s%s, req %d bufs of >= %d bytes, %s %u ch %s\n",
                        pi, port->isInput ? "input" : "output", target_state, (int) port->portdef.nBufferCountActual,
                        (int) port->portdef.nBufferSize, esaafreq2str(port->format.freq), port->format.chan_cfg,
                        port->silenceOnUnderrun ? "SilenceOnUnderrun" : "");
            } else if (!port->isEnabled) {
                dbg_printf("  .%d disabled %s %s\n", pi, port->isInput ? "input" : "output", target_state);
            } else {
                assert(0);
            }
            
            g_dbg_indent = 6;
            Port_Dump(port);
            unsigned int i;

            for (i = 0 ; i < port->num_omx_buf ; i++) {
                dbg_printf("      IL-buf %u: %u/%u bh=%X adil_buf=%X tbr=%d busy_in_peer=%d allocated=%d allocated_by_us=%d\n",
                        i, (uint_t) port->omx_buf[i].bh->nFilledLen, (uint_t) port->omx_buf[i].bh->nAllocLen, (int) port->omx_buf[i].bh,
                        (int) port->omx_buf[i].adil_buf,port->omx_buf[i].to_be_returned,port->omx_buf[i].busy_in_peer,
                        port->omx_buf[i].allocated, port->omx_buf[i].allocated_by_us);
                if (port->omx_buf[i].shared_buf) {
                    dbg_printf("      (shared from) ");
                    g_dbg_indent = 0;
                    Buf_Dump(port->omx_buf[i].shared_buf);
                }
            }

            g_dbg_indent = 6;
            if (port->isEnabled && !port->isDirect && port->isInput) {
                CompBuf_Dump(&port->zerobuf);
            }
            BufQueue_Dump(&port->bq);
        }

        if (comp->dump) {
            g_dbg_indent = 4;
            dbg_printf("  Component state\n");
            comp->dump(comp);
        }
    }

    if (any_component) {
        dbg_printf(" Active buffers\n");
        g_dbg_indent = 2; BufPool_Dump();
        dbg_printf("-------------------------------------------------------------------------\n");
    }
}


void ste_adm_omx_core_dump(void)
{
    pthread_mutex_lock(&mutex);
    ste_adm_omx_core_dump_locked();
    pthread_mutex_unlock(&mutex);
}




///////////////////////////////////////////////////////////////////////////////
//
// SRC2 - sample rate converter
//
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BufQueue_t in;
    unsigned int inbufoffs;
    int init_done;
    int* srcinbuf;
    unsigned int srcinbuf_bytes_valid;
    int* srcoutbuf;
    unsigned int srcoutbufoffs;
    unsigned int srcoutbuf_bytes_valid;
    char* heap;
    unsigned int srcbufsize;
    int channels;
    ResampleContext src;
    CompBuf_t compbuf;
    SRC_QUALITY_t mode;
} SRC2_t;



static void SRC2_Dump(SRC2_t* this)
{
    dbg_indent(); dbg_printf("SRC2 %X\n", (int) this);
    g_dbg_indent += 2;
    dbg_indent(); dbg_printf("inbufoffs=%u srcinbuf_bytes_valid=%u srcoutbufoffs=%u srcoutbuf_bytes_valid=%u srcbufsize=%u\n",
           this->inbufoffs,this->srcinbuf_bytes_valid,this->srcoutbufoffs,this->srcoutbuf_bytes_valid,this->srcbufsize);
    CompBuf_Dump(&this->compbuf);
    BufQueue_Dump(&this->in);
    g_dbg_indent -= 2;
}


static void SRC2_SetMode(SRC2_t* this, SRC_QUALITY_t mode)
{
    this->mode = mode;
}


static void SRC2_Init(SRC2_t* this, comp_t* comp)
{
    memset(this, 0, sizeof(*this));
    BufQueue_Init(&this->in);
    CompBuf_Init(&this->compbuf, comp, 2040, 1);
    this->mode = SRC_STANDARD_QUALITY;
}


static void SRC2_Destroy(SRC2_t* this)
{
    if (this->srcinbuf)  free(this->srcinbuf);
    if (this->srcoutbuf) free(this->srcoutbuf);
    if (this->heap)      free(this->heap);

    BufQueue_ReleaseBuffers(&this->in);
    CompBuf_Destroy(&this->compbuf);
    memset(this, 0xAA, sizeof(*this));
}


static void SRC2_Push(SRC2_t* this, buf_t* buf)
{
    BufQueue_Push(&this->in, buf);
}


static void SRC2_Setup(SRC2_t* this, int infreq, int outfreq, int channels)
{
    assert(!this->init_done);

    int heapsize = 0;
    int blocksize = 256 * channels;
    int status = resample_calc_max_heap_size_fixin_fixout(infreq, outfreq,
                       this->mode, &heapsize, blocksize, channels, 2); // TODO. type reflecst upsample / downsample

    if (status) {
        ALOG_ADIL_INT("SRC2_Setup resample_calc_max_heap_size_fixin_fixout failed\n");
        ALOG_ADIL_INT("  infreq=%d outfreq=%d blocksize=%d channels=%d\n", infreq, outfreq, blocksize, channels);
        return;
    }

    this->srcbufsize = blocksize * sizeof(int);
    this->srcinbuf  = malloc(this->srcbufsize);
    this->srcoutbuf = malloc(this->srcbufsize);
    this->heap   = malloc((size_t)heapsize);
    this->srcoutbuf_bytes_valid = 0;
    this->inbufoffs = 0;
    this->channels = channels;

    if (!this->srcinbuf || !this->srcoutbuf || !this->heap) {
        ALOG_ADIL_INT("SRC_init resample_calc_max_heap_size_fixin_fixout out of memory\n");
        return;
    }

    status = resample_x_init_ctx_low_mips_fixin_fixout(this->heap, heapsize, &this->src,
                                                  infreq, outfreq, this->mode,
                                                  channels, blocksize);
    if (status) {
        ALOG_ADIL_INT("SRC2_Setup resample_x_init_ctx_low_mips_fixin_fixout failed\n");
        return;
    }


    ALOG_ADIL_INT("SRC2_Setup OK heapsize=%d channels=%d blocksize=%d mode=%d\n", heapsize, channels, blocksize, this->mode);
    this->init_done = 1;
}


static buf_t* SRC2_Pop(SRC2_t* this, unsigned short outfreq)
{
    ALOG_ADIL_INT("SRC2_Pop\n");
    if (!this->init_done) {
        // Need first buffer to know input sample freq
        if (!BufQueue_Peek(&this->in)) {
            return 0;
        }

        int infreq = BufQueue_Peek(&this->in)->format.freq;
        if (infreq == outfreq) {
            ALOG_ADIL_INT("SRC2: Sample rate is same, just forwarding buffer\n");
            return BufQueue_Pop(&this->in);
        }

        uint_t chans  = BufQueue_Peek(&this->in)->format.chan_cfg;
        SRC2_Setup(this, infreq, outfreq, chans);
    }
    if (!this->init_done) {
        return 0;
    }

    int did_something = 0;
    do {
        buf_t* out = CompBuf_Peek(&this->compbuf);
        if (!out) {
            return 0;
        }

        // Produce data, if we have any. Always copy all data before
        // calling SRC
        // TODO: Fix naming for samples per channel vs. samples totally

        // Copy data from temporary 32-bit buffer to output buffer with 16-bit samples
        if (this->srcoutbuf_bytes_valid > this->srcoutbufoffs) {
            int in_samples  = (this->srcoutbuf_bytes_valid - this->srcoutbufoffs) / 4;
            int out_samples = (out->bytes_available - out->bytes_valid) / 2;

            // Number of samples totally (not per channel)
            int samples     = MIN(in_samples, out_samples);

            int i;
            for (i = 0 ; i < samples ; i++) {
                ((short*)out->data)[out->bytes_valid/2+i] = this->srcoutbuf[this->srcoutbufoffs/4+i] / 65536;
            }

            out->bytes_valid    += samples * 2;
            this->srcoutbufoffs += samples * 4;

            assert(out->bytes_valid <= out->bytes_available);
            if (out->bytes_valid == out->bytes_available) {
                out->format.chan_cfg = this->channels;
                out->format.freq = outfreq;
                return CompBuf_Pop(&this->compbuf);
            }

            did_something = 1;
        }
        assert(this->srcoutbuf_bytes_valid == this->srcoutbufoffs);
        this->srcoutbuf_bytes_valid = 0;
        this->srcoutbufoffs        = 0;

        // At this point, the output buffer (srcoutbuf) is empty

        // Copy samples to the temporary 32-bit input buffer from the
        // input frame
        while (BufQueue_Peek(&this->in) && this->srcinbuf_bytes_valid < this->srcbufsize) {
            buf_t* inbuf = BufQueue_Peek(&this->in);
            int input_samples = (inbuf->bytes_valid - this->inbufoffs) / 2;
            int output_samples = (this->srcbufsize - this->srcinbuf_bytes_valid) / sizeof(int);

            // Number of samples totally (not per channel)
            int samples = MIN(input_samples, output_samples);

            int i;
            for (i = 0 ; i < samples ; i++) {
                this->srcinbuf[i+this->srcinbuf_bytes_valid/4] = 65536 * ((short*)inbuf->data)[i+this->inbufoffs/2];
            }
            this->inbufoffs            += samples*2;
            this->srcinbuf_bytes_valid += samples*4;
            if (this->inbufoffs == inbuf->bytes_valid) {
                Buf_Release(BufQueue_Pop(&this->in));
                this->inbufoffs = 0;
            }

            did_something = 1;
        }

        if (this->srcinbuf_bytes_valid < this->srcbufsize) {
            // Wait for more data before calling SRC
            return 0;
        }

        // At this point, the src input buffer is full and the src output buffer
        // is empty
        ALOG_ADIL_INT("C) srcbufsize=%d srcinbuf_bytes_valid=%d srcoutbufoffs=%d srcoutbuf_bytes_valid=%d\n",this->srcbufsize,this->srcinbuf_bytes_valid, this->srcoutbufoffs, this->srcoutbuf_bytes_valid);

        assert(this->srcinbuf_bytes_valid == this->srcbufsize);
        assert(this->srcoutbufoffs == 0);
        assert(this->srcoutbuf_bytes_valid == 0);

        // Number of samples PER CHANNEL
        int nSamples = this->srcinbuf_bytes_valid / (4 * this->channels);
        int flush=0;
        int nbOut = nSamples;
        ALOG_ADIL_INT("src_ctx.processing nSamples=%d nbOut=%d\n", nSamples, nbOut);

        int status = this->src.processing(&this->src, this->srcinbuf, this->channels, &nSamples, this->srcoutbuf, &nbOut, &flush);
        ALOG_ADIL_INT("src_ctx.processing status=%d nbOut=%d nSamples=%d\n", status, nbOut, nSamples);

        if (nSamples != 0) {
            // Only support either consuming no data, or all data
            assert(this->srcinbuf_bytes_valid / (4*this->channels) == (unsigned int) nSamples);
            this->srcinbuf_bytes_valid = 0;
        }

        this->srcoutbuf_bytes_valid = nbOut * 4 * this->channels;
        did_something = 1;
    } while(did_something);

    return 0;
}




// More optimized version:
// 1) downmix (2->1, 5->2, etc)
// 2) if !diff_freq -->
//     2.1) upmix (1-->2, 2-->5, etc) to highest common number to be mixed, unless done by mix step
//     2.2) mix
//     2.3) SRC to output freq
//     2.4) upmix rest
// 3) if diff_freq
//     3.1) downmix (2-->1, 5-->2, etc)
//     3.2) SRC
//     3.3) upmix (1-->2, 2-->5, etc) unless done by mix step
//     3.4) mix
// mix, channelconvert, samplerate convert and apply volumes
//
// completely unoptimized version
//
// If a buffer is consumed, it is released and set to NULL.
// Do volume apply and sample rate convert on as few channels as possible
//
// 1) downmix
// 2) SRC
//
//
// volume not supported


///////////////////////////////////////////////////////////////////////////////
//
// UpDownMix
//
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    BufQueue_t in;
    CompBuf_t compbuf;
    unsigned int inoffs;
} UpDownMix_t;



static void UpDownMix_Init(UpDownMix_t* this, comp_t* comp)
{
    BufQueue_Init(&this->in);
    CompBuf_Init(&this->compbuf, comp, 2040, 1);
}


// Should be called when a port is disabled. Will restore
// the component to the state it had after Init. Releases/orphans
// all buffers.
static void UpDownMix_Reset(UpDownMix_t* this)
{
    BufQueue_ReleaseBuffers(&this->in);
    CompBuf_OrphanBuffers(&this->compbuf);
    this->inoffs = 0;
}


static void UpDownMix_Destroy(UpDownMix_t* this) // FUTURE IMPROVEMENT: Rename --> ChanMixSplit? ChanConv?
{
    UpDownMix_Reset(this);
    CompBuf_Destroy(&this->compbuf);
}


static void UpDownMix_Dump(UpDownMix_t* this)
{
    dbg_indent(); dbg_printf("UpDownMix %X inoffs=%d\n", (int) this, this->inoffs);
    g_dbg_indent += 2;
    BufQueue_Dump(&this->in);
    CompBuf_Dump(&this->compbuf);
    g_dbg_indent -= 2;
}


static void UpDownMix_Push(UpDownMix_t* this, buf_t* buf)
{
    BufQueue_Push(&this->in, buf);
}


#define UPDOWNMIX_ITERATE_SETUP_POINTERS(__in_channels, __out_channels) \
    unsigned int input_size   = __in_channels * sizeof(short); \
    unsigned int output_size  = __out_channels * sizeof(short); \
    unsigned int samples_in   = (in->bytes_valid - this->inoffs) / input_size; \
    unsigned int samples_out  = (out->bytes_available - out->bytes_valid) / output_size; \
    unsigned int samples      = (samples_in < samples_out) ? samples_in : samples_out; \
    short* __restrict inp     = (short*) (in->data + this->inoffs); \
    short* __restrict outp    = (short*) (out->data + out->bytes_valid); \
    short* inp_end            = inp + samples * __in_channels; \
    this->inoffs             += samples * input_size; \
    out->bytes_valid         += samples * output_size;


static void UpDownMix_Iterate(UpDownMix_t* this, buf_t* in, buf_t* out)
{
    if (in->format.chan_cfg == 1 && out->format.chan_cfg == 2) {
        UPDOWNMIX_ITERATE_SETUP_POINTERS(1,2)
        while (inp != inp_end) {
            *outp++ = *inp;
            *outp++ = *inp++;
        }
    } else if (in->format.chan_cfg == 1 && out->format.chan_cfg == 6) {
        UPDOWNMIX_ITERATE_SETUP_POINTERS(1,6)
        while (inp != inp_end) {
            // 5.1 order: LF RF LFE CF LS RS
            // see 2 --> 6 conversion for coefficients
            outp[0] = inp[0];
            outp[1] = inp[0];
            outp[2] = (short) ((inp[0] * 246) / 256);
            outp[3] = (short) ((inp[0] * 246) / 256);
            outp[4] = (short) ((inp[0] * 246) / 256);
            outp[5] = (short) ((inp[0] * 246) / 256);

            outp += 6;
            inp  += 1;
        }
    } else if (in->format.chan_cfg == 2 && out->format.chan_cfg == 1) {
        UPDOWNMIX_ITERATE_SETUP_POINTERS(2,1);
        while (inp != inp_end) {
            *outp++ = (short) (((int) inp[0] + (int) inp[1]) / 2);
            inp += 2;
        }
    } else if (in->format.chan_cfg == 2 && out->format.chan_cfg == 6) {
        UPDOWNMIX_ITERATE_SETUP_POINTERS(2,6);
        // 5.1 order: LF RF LFE CF LS RS
        // stereo: LF RF
        // non-normalized formulas:
        //   LFE=CF = (LF + RF)/sqrt(2)
        //   LS     = (1/5) * (LF*sqrt(19) + RF*sqrt(6))
        //   RS     = (1/5) * (RF*sqrt(19) + RF*sqrt(6))
        // To avoid possible overflow, we multiply with 1/sqrt(2)
        // FUTURE IMPROVEMENT: Better to us dynamic damping to avoid
        // overflow, instead of using 1/sqrt(2) factor?
        while (inp != inp_end) {
            outp[0] = inp[0];
            outp[1] = inp[1];
            outp[2] = (short) (((int) inp[0] + (int) inp[1]) / 2);
            outp[3] = (short) (((int) inp[0] + (int) inp[1]) / 2);
            outp[4] = (short) ((158 * inp[0] + 88 * inp[1]) / 256); // LF*sqrt(19/25) + RF*sqrt(6/25)
            outp[5] = (short) ((88 * inp[0] + 158 * inp[1]) / 256); // LF*sqrt(6/25)  + RF*sqrt(19/25)

            outp += 6;
            inp  += 2;
        }
    } else if (in->format.chan_cfg == 6 && out->format.chan_cfg == 1) {
        UPDOWNMIX_ITERATE_SETUP_POINTERS(6,1)
        while (inp != inp_end) {
            *outp++ = (short) (((int) inp[0] + (int) inp[1]) / 2);
            inp += 6;
        }
    } else if (in->format.chan_cfg == 6 && out->format.chan_cfg == 2) {
        UPDOWNMIX_ITERATE_SETUP_POINTERS(6,2)
        while (inp != inp_end) {
            *outp++ = inp[0];
            *outp++ = inp[1];
            inp += 6;
        }
    } else {
        ALOG_ADIL_INT("%d --> %d not supported\n", in->format.chan_cfg, out->format.chan_cfg);
        assert(0);
    }
}


static buf_t* UpDownMix_Pop(UpDownMix_t* this, unsigned char out_cfg)
{
    int did_something;

    do {
        did_something = 0;
        buf_t* in = BufQueue_Peek(&this->in);
        if (in) {
            if (in->format.chan_cfg != out_cfg) {
                buf_t* out = CompBuf_Peek(&this->compbuf);

                if (!out) {
                    return 0;
                }

                out->format.chan_cfg = out_cfg;
                out->format.freq     = in->format.freq;

                ALOG_ADIL_INT("UpDownMix_Pop: inbuf=%X outbuf=%X this->inoffs=%d in->bytes_valid=%d\n", (int) in, (int) out, this->inoffs, in->bytes_valid);
                ALOG_ADIL_INT("UpDownMix_Pop: out->bytes_valid=%d out->bytes_available=%d\n", out->bytes_valid, out->bytes_available);
                UpDownMix_Iterate(this, in, out);
                did_something = 1;

                if (this->inoffs == in->bytes_valid) {
                    this->inoffs = 0;
                    Buf_Release(BufQueue_Pop(&this->in));
                }

                if (out->bytes_valid == out->bytes_available) {
                    return CompBuf_Pop(&this->compbuf);
                }
            } else {
                // Same format, just forward buffer
                ALOG_ADIL_INT("UpDownMix_Pop - forarding buffer\n");
                BufQueue_Drop(&this->in);
                return in;
            }
        }
    } while(did_something);

    return 0;
}




///////////////////////////////////////////////////////////////////////////////
//
// Convert
//
///////////////////////////////////////////////////////////////////////////////
#define CONVERT_MAX_INPUTS (8)

typedef enum
{
    CONVERT_INQUEUE_STATE_DISABLED,
    CONVERT_INQUEUE_STATE_STARTING,
    CONVERT_INQUEUE_STATE_RUNNING
} ConvertInqueueState_t;

typedef struct
{
    UpDownMix_t    updownmixctx[CONVERT_MAX_INPUTS];
    SRC2_t         srcctx[CONVERT_MAX_INPUTS];
    BufQueue_t     inqueue[CONVERT_MAX_INPUTS];
    unsigned int   inqueueoffs[CONVERT_MAX_INPUTS];
    ConvertInqueueState_t inqueuestate[CONVERT_MAX_INPUTS];
    CompBuf_t      compbuf;
    unsigned short output_freq;
    int            output_freq_valid;

    MIXER_LOCAL_STRUCT_T mixalgo;
    int          unit_gain;
    comp_t*      dbg_comp;
    int          dbg_first_forward;
} Convert_t;


static void Convert_SetInqueueActive(comp_t* comp, Convert_t* this, OMX_U32 idx, int activate)
{
    assert(idx <= 7);
    if (activate) {
        UpDownMix_Init(&this->updownmixctx[idx], comp);
        SRC2_Init(&this->srcctx[idx], comp);
        BufQueue_Init(&this->inqueue[idx]);
        this->inqueueoffs[idx] = 0;
        assert(this->inqueuestate[idx] == CONVERT_INQUEUE_STATE_DISABLED);
        this->inqueuestate[idx] = CONVERT_INQUEUE_STATE_STARTING;
    } else {
        SRC2_Destroy(&this->srcctx[idx]);
        UpDownMix_Destroy(&this->updownmixctx[idx]);
        BufQueue_ReleaseBuffers(&this->inqueue[idx]);
        assert(this->inqueuestate[idx] == CONVERT_INQUEUE_STATE_STARTING ||
               this->inqueuestate[idx] == CONVERT_INQUEUE_STATE_RUNNING);
        this->inqueuestate[idx] = CONVERT_INQUEUE_STATE_DISABLED;
    }
}


static void Convert_SetSRCMode(Convert_t* this, OMX_U32 port, SRC_QUALITY_t mode)
{
    assert(port <= 7);
    SRC2_SetMode(&this->srcctx[port], mode);
}


static void Convert_Push(Convert_t* this, buf_t* buf, int idx)
{
    if (!this->output_freq_valid) {
        // FUTURE IMPROVEMENT: Reset output_freq_valid once all ports are disabled
        ALOG_ADIL_INT("%X Convert_Push: setting output freq to %d\n", (int) this->dbg_comp, buf->format.freq);
        if (esaafreq2hz(buf->format.freq) < 44100) {
            this->output_freq = ESAA_FREQ_44_1KHZ;
        }
        else {
            this->output_freq = buf->format.freq;
        }
        this->output_freq_valid = 1;
    }

    if (this->inqueuestate[idx] == CONVERT_INQUEUE_STATE_STARTING) {
        ALOG_ADIL_INT("Convert %X: idx %d changed state STARTING -> RUNNING\n", (int) this->dbg_comp, idx);
        this->inqueuestate[idx] = CONVERT_INQUEUE_STATE_RUNNING;
    }

    assert(this->inqueuestate[idx] == CONVERT_INQUEUE_STATE_RUNNING);
    SRC2_Push(&this->srcctx[idx], buf);
}


static void Convert_Init(Convert_t* this, comp_t* comp)
{
    assert_ptr(comp);
    CompBuf_Init(&this->compbuf, comp, 2040, 1);
    this->output_freq_valid  = 0;
    this->dbg_comp = comp;
    this->dbg_first_forward = 0;
    this->unit_gain = 1; // If we start with 1 channel, we are in unit gain config.

    memset(&this->mixalgo, 0, sizeof(this->mixalgo));

    this->mixalgo.alpha_dw  = ALPHA_DW_DEFAULT;
    this->mixalgo.alpha_up  = ALPHA_UP_DEFAULT;
    this->mixalgo.one_minus_alpha_dw = (0x800000U - ALPHA_DW_DEFAULT);
    this->mixalgo.one_minus_alpha_up = (0x800000U - ALPHA_UP_DEFAULT);

    this->mixalgo.coef_poly[0] = COEF_1;
    this->mixalgo.coef_poly[1] = (MMlong)COEF_2;
    this->mixalgo.coef_poly[2] = COEF_3;
    this->mixalgo.coef_poly[3] = COEF_4;

    // We use 16-bit samples, so divide the threashold by 2^16
    this->mixalgo.threshold = THRESHOLD_DEFAULT / 65536;
}


static void Convert_Destroy(Convert_t* this)
{
    CompBuf_Destroy(&this->compbuf);
}


static void Convert_Dump(Convert_t* this)
{
    dbg_indent(); dbg_printf("Convert %X. ", (int) this);
    g_dbg_indent += 2;
    buf_t* out = CompBuf_Peek(&this->compbuf);
    if (out) {
        dbg_indent(); dbg_printf("compbuf %X, %d/%d", (int) out, out->bytes_valid, out->bytes_available);
    } else {
        dbg_indent(); dbg_printf("(owns no compbuf)\n");
    }
    dbg_printf("\n");

    int i;
    for (i = 0 ; i < CONVERT_MAX_INPUTS ; i++) {
        const char* state = NULL;
        switch (this->inqueuestate[i]) {
            case CONVERT_INQUEUE_STATE_DISABLED: break;
            case CONVERT_INQUEUE_STATE_STARTING: state = "STARTING"; break;
            case CONVERT_INQUEUE_STATE_RUNNING:  state = "RUNNING";  break;
            default: assert(0); break;
        }

        if (state) {
            dbg_indent(); dbg_printf("%d %s inqueueoffs: %d\n", i, state, this->inqueueoffs[i]);
            g_dbg_indent += 2;
            UpDownMix_Dump(&this->updownmixctx[i]);
            SRC2_Dump(&this->srcctx[i]);
            dbg_indent(); dbg_printf("Convert inqueue\n");
            g_dbg_indent += 2;
            BufQueue_Dump(&this->inqueue[i]);
            g_dbg_indent -= 2 + 2;
        }
    }
    g_dbg_indent -= 2;
}


static uint_t Convert_MixInto(Convert_t* this, buf_t *out)
{
    assert_ptr(out);
    assert_ptr(this);
    uint_t num_inputs=0;
    short* buf_data[CONVERT_MAX_INPUTS];
    double gainRampUp = 0;

    uint_t dbg_num_adj  = 0;
    double dbg_max_gain = 0;
    double dbg_min_gain = 10;
    uint_t dbg_cut_up   = 0;
    uint_t dbg_cut_down = 0;

    unsigned int sample_size = sizeof(short);
    unsigned int samples = (out->bytes_available - out->bytes_valid) / sample_size;
    unsigned int i;

    for (i = 0 ; i < CONVERT_MAX_INPUTS ; i++) {
        if (this->inqueuestate[i] == CONVERT_INQUEUE_STATE_RUNNING) {
            buf_t* in = BufQueue_Peek(&this->inqueue[i]);

            if (!in) {
                return 0;
            }

            buf_data[num_inputs] = (short*) (in->data + this->inqueueoffs[i]);
            num_inputs++;
            unsigned int in_samples = (in->bytes_valid - this->inqueueoffs[i]) / sample_size;
            if (in_samples < samples) {
                samples = in_samples;
            }
        }
    }

    assert(num_inputs);
    uint_t num_chan = out->format.chan_cfg;

    //gainRampUp is used to ensure a smooth transion of the gain to unit_gain before we exit the mixing.
    if (num_inputs == 1){
        gainRampUp = 1.0f/(1.0f * (samples/num_chan));
    }

    if (samples)
    {
        this->unit_gain = 1.0f;
    }

    // Mix it
    ALOG_ADIL_INT("MixInto num_inputs=%d buf_data[0]=%X samples=%d num_chan=%d\n", num_inputs, buf_data[0], samples, num_chan);
    short* outptr = (short*) (out->data + out->bytes_valid);
    for (i = 0 ; i < samples ; i+=num_chan) {
        int ack[CONVERT_MAX_INPUTS];
        uint_t ch;
        for (ch=0 ; ch < num_chan ; ch++) {
            ack[ch] = buf_data[0][i+ch];
        }

        uint_t j;
        for (j = 1 ; j < num_inputs; j++) {
            for (ch=0 ; ch < num_chan ; ch++) {
                ack[ch] += buf_data[j][i+ch];
            }
        }

        double env_max = -DBL_MAX;
        uint_t env_max_ch = 0;
        for (ch = 0 ; ch < num_chan ; ch++) {
            double env = mixer_envelop_detection(ack[ch], &this->mixalgo, (int) ch);
            if (env > env_max) {
                env_max_ch = ch;
                env_max = env;
            }
        }

        double gain = mixer_compute_gain(&this->mixalgo, (int) env_max_ch);

        if (gainRampUp != 0){
            gain += i*gainRampUp;

            if (gain > 1.0f){
                gain = 1.0f;
                this->unit_gain = 1.0f;
            }
        }

        if (gain < 1.0 - 1.0/65536) {
            this->unit_gain = 0;
            dbg_num_adj += num_chan;

            for (ch = 0 ; ch < num_chan ; ch++) {
                ack[ch] = (short) round(gain * ack[ch]);
            }
        }

        for (ch = 0 ; ch < num_chan ; ch++) {
            if (ack[ch] > 32767) {
                ack[ch] = 32767;
                dbg_cut_up++;
            }

            if (ack[ch] < -32768) {
                ack[ch] = -32768;
                dbg_cut_down++;
            }

            *outptr++ = (short) ack[ch];
        }
    }

    // Update consumed samples, and drop emptied buffers
    for (i = 0 ; i < CONVERT_MAX_INPUTS ; i++) {
        if (this->inqueuestate[i] == CONVERT_INQUEUE_STATE_RUNNING) {
            this->inqueueoffs[i] += samples * sample_size;
            buf_t* in = BufQueue_Peek(&this->inqueue[i]);

            if (this->inqueueoffs[i] == in->bytes_valid) {
                Buf_Release(BufQueue_Pop(&this->inqueue[i]));
                this->inqueueoffs[i] = 0;
            }
        }
    }

    if (samples) {
        if (dbg_num_adj > 0) {
            ALOG_ADIL_INT("MIXALG %X Adjusted gain on %u of %d samples (%d inputs). Gain in range [%f,%f]. Cut %d up, %d down\n",
                (int) this->dbg_comp, dbg_num_adj, samples, num_inputs, dbg_min_gain, dbg_max_gain, dbg_cut_up, dbg_cut_down);
        } else {
            ALOG_ADIL_INT("MIXALG %X No adjustments. %d inputs. %d channels.\n", (int) this->dbg_comp, num_inputs, num_chan);
        }

        if (dbg_cut_up > 0 || dbg_cut_down > 0) {
            ALOG_WARN("Mixer %X: Cut %d + %d samples during mixing!\n", (int) this->dbg_comp, dbg_cut_up, dbg_cut_down);
        }
    }

    out->bytes_valid += samples * sample_size;

    return samples;
}


static buf_t* Convert_Pop(Convert_t* this, unsigned char format)
{
    int did_something;
    int input_idx; // index of the single input, if there is just one

    // This means we've received no data
    if (!this->output_freq_valid) {
        ALOG_ADIL_INT("%X Convert_Pop - !this->output_freq_valid\n", (int) this->dbg_comp);
        return 0;
    }

    buf_t* out = CompBuf_Peek(&this->compbuf);
    if (!out) {
        // Do this check in bypass mode as well to avoid have too many
        // outstanding buffers during transition
        return 0;
    }

    out->format.chan_cfg = format;
    out->format.freq     = this->output_freq;

    do {
        did_something=0;
        int num_inputs=0;
        int idx;

        for (idx = 0 ; idx < CONVERT_MAX_INPUTS ; idx++) {
            if (this->inqueuestate[idx] == CONVERT_INQUEUE_STATE_RUNNING) {
                input_idx = idx;
                num_inputs++;

                do {
                    did_something=0;
                    // FUTURE IMPROVEMENT: Optimizations. First SRC to output freq, then
                    // up/downmix to output format, then mix everything.
                    // A first obvious optimization would be to do the steps in an order depending
                    // on what needed to be done, e.g. to mix [12 kHz 2 chan and 24 kHz 1 chan] to
                    // 48 kHz 6 chan, first do SRC to 48 kHz, then mix
                    // everything (do upmix 1--> 2 while mixing), and finally upmix to 6 chan.
                    // If the refcnt of the buffer is 1, nobody else is using it, and some operations
                    // can be done inplace

                    // Step 1.1) Sample rate conversion. If 1:1, just passes through.
                    assert(this->output_freq_valid);
                    buf_t* post_src = SRC2_Pop(&this->srcctx[idx], this->output_freq);
                    if (post_src) {
                        // Step 1.2) up/downmix. If 1:1, just passes through.
                        UpDownMix_Push(&this->updownmixctx[idx], post_src);
                        did_something=1;
                    }

                    buf_t* post_udm = UpDownMix_Pop(&this->updownmixctx[idx], format);
                    if (post_udm) {
                        BufQueue_Push(&this->inqueue[idx], post_udm);
                        did_something=1;
                    }
                } while(did_something);
            }
        }

        if (num_inputs == 1 && this->unit_gain && this->inqueueoffs[input_idx] == 0 && out->bytes_valid == 0) {
            // Just one input, and no data remains in mixer; simply pass
            // on buffer, if we have one
            ALOG_ADIL_INT("MIXALG %X Convert_Pop, one input, unit gain, all flushed, just forwarding buffer %X (if any)\n", (int) this->dbg_comp, (int) BufQueue_Peek(&this->inqueue[input_idx]));
            if (!this->dbg_first_forward) {
                ALOG_ADIL("MIXALG %X Convert_Pop - forwarding for first time after mixing\n", (int) this->dbg_comp);
                this->dbg_first_forward = 1;
            }

            return BufQueue_Pop(&this->inqueue[input_idx]);
        }

        // Much slower path; we need to do mixing.
        if (num_inputs > 0 && Convert_MixInto(this, out)) {
            this->dbg_first_forward = 0;
            did_something = 1;
        }

        if( (out->bytes_valid == out->bytes_available) ||
            (out->bytes_valid > 0 && num_inputs == 1 && this->unit_gain && this->inqueueoffs[input_idx] == 0))
        {
            // (out->bytes_valid > 0 && num_inputs == 1 && this->unit_gain && this->inqueueoffs[input_idx] == 0)
            // --> we are transitioning from mixing to a situation where we can just forward the buffer without
            // touching it. We need to get rid of the potentially half-filled output buffer first.
            ALOG_ADIL_INT("MIXALG %X: Convert produced buffer %X, %d of %d bytes\n", (int) this->dbg_comp, (int) out, out->bytes_valid, out->bytes_available);
            CompBuf_Drop(&this->compbuf);
            return out;
        }
    } while(did_something);

    return 0;
}




///////////////////////////////////////////////////////////////////////////////
//
// Mixer
//
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    comp_t base;
    Convert_t convert;
    int has_sent_eos;
    buf_format_t last_sent_buf_format;
    int is_inited;
} mixer_comp_t;



static void Mixer_Dump(comp_t* comp)
{
    mixer_comp_t* this = (mixer_comp_t*) comp;
    if (this->is_inited) {
        g_dbg_indent += 2;
        Convert_Dump(&this->convert);
        g_dbg_indent -= 2;
    }
}


static void Mixer_onDeactivated(comp_t* comp, OMX_U32 port)
{
    mixer_comp_t* this = (mixer_comp_t*) comp;
    if (port >= 1 && port <= 8) {
        Convert_SetInqueueActive(comp, &this->convert, port-1, 0);
    }
}


static void Mixer_onActivated(comp_t* comp, OMX_U32 port)
{
    mixer_comp_t* this = (mixer_comp_t*) comp;
    if (port >= 1 && port <= 8) {
        Convert_SetInqueueActive(comp, &this->convert, port-1, 1);
        Convert_SetSRCMode(&this->convert, port-1, comp->port[port].SRC_mode);
    } else {
        // output port starts in EOS mode..
        this->has_sent_eos = 1;
    }
}


static void Mixer_Process(comp_t* comp)
{
    mixer_comp_t* this = (mixer_comp_t*) comp;
    int i;
    int num_inputs = 0;
    port_t* single_port;

    for (i = 1 ; i <= 8 ; i++) {
        port_t* port = &comp->port[i];
        if (isActive(port)) {
            single_port = port;
            num_inputs++;
        }
    }

    ALOG_ADIL_INT("Mixer_Process %X, %d active inputs\n", (int) comp, num_inputs);

    if (num_inputs > 0 && !this->is_inited) {
        ALOG_ADIL_INT("Initializing convert component\n");
        Convert_Init(&this->convert, &this->base);
        this->is_inited = 1;
    }

    if (num_inputs >= 1) {
        for (i = 1 ; i <= 8 ; i++) {
            port_t* port = &comp->port[i];

            if (isActive(port)) {
                while (Port_PeekBuf(port)) {

#ifdef ADM_MMPROBE
                    // check if input port should be probed using mmprobe
                    if (port->pcm_probe_enabled) {
                        buf_t* buf = Port_PeekBuf(port);
                        if (port->pcm_probe_enabled && buf) {
                            DataFormat_t data_format;
                            omx_format2mmprobe_format(port->omx_format, &data_format);
                            ALOG_ADIL_FLOW("probe pcm data, probe_id=%d, data=%d, bytes=%d, format=%d,%d,%d,%d",
                                port->pcm_probe_id, buf->data, buf->bytes_valid,
                                data_format.SampleRate, data_format.NoChannels,
                                data_format.Interleave, data_format.Resolution);
                            mmprobe_probe_V2(port->pcm_probe_id, buf->data, buf->bytes_valid, &data_format);
                        }
                    }
#endif // ADM_MMPROBE

                    Convert_Push(&this->convert, Port_PopBuf(port), i-1);
                } //while
            } //if (isActive(port))
        } //for (i = 1 ; i <= 8 ; i++)

        int did_something;
        do {
            did_something = 0;

            // We always make the mixer deliver the data format of comp->port[0].omx_format.nChannels.
            // POSSIBLE FUTURE IMPROVEMENT: Do this in the IL output instead. But this works fine
            // today. The ADM never puts anything after this mixer.
            buf_t* out = Convert_Pop(&this->convert, (unsigned char) comp->port[0].omx_format.nChannels);

            if (out) {
                ALOG_ADIL_INT("Mixer_Process: Convert_Pop returned buffer %X. %u channels.\n", (int) out, comp->port[0].omx_format.nChannels);
                this->has_sent_eos = 0;
                this->last_sent_buf_format = out->format;
                Port_Push(&comp->port[0], out);
                did_something = 1;
            }
        } while(did_something);
    }

    if (num_inputs == 0 && !this->has_sent_eos) {
        // Workaround for an issue where Android opens an input, but does not send
        // data to it.
        ALOG_ADIL_INT("Mixer_Process: No longer any active inputs, sending EOS buffer\n");
        buf_t* eos_buf = BufPool_AllocHdr();
        eos_buf->bytes_valid = 0;
        eos_buf->eos = 1;
        eos_buf->dtor = 0;
        eos_buf->format = this->last_sent_buf_format;
        Port_Push(&comp->port[0], eos_buf);
        this->has_sent_eos = 1;
        if (this->is_inited) {
            Convert_Destroy(&this->convert);
            this->is_inited = 0;
        }
    }
}


static OMX_ERRORTYPE Mixer_onSetGet(comp_t* comp, ADM_CORE_INDEXTYPE nIndex, OMX_PTR pParamRaw, int isSet, int isParam)
{
    (void) comp;
    (void) pParamRaw;
    (void) isSet;
    (void) isParam;
    (void) nIndex;

    return OMX_ErrorUnsupportedIndex;
}


static void Mixer_dtor(comp_t* comp)
{
    mixer_comp_t* this = (mixer_comp_t*) comp;
    if (this->is_inited) {
        Convert_Destroy(&this->convert);
    }
}


static OMX_ERRORTYPE Mixer_Factory(comp_t** comp_pp, const char* name)
{
    OMX_ERRORTYPE result = OMX_ErrorNone;

    *comp_pp = 0;

    if (strcmp(name, "ADM.mixer") != 0) {
        result = OMX_ErrorComponentNotFound;
        goto Exit;
    }

    if (comp_pp == NULL) {
        result =  OMX_ErrorInsufficientResources;
        goto Exit;
    }

    mixer_comp_t* mixer = calloc(1, sizeof(mixer_comp_t));
    if (!mixer) {
        result = OMX_ErrorInsufficientResources;
        goto Exit;
    }

    Comp_Init(&mixer->base);
    mixer->base.process         = Mixer_Process;
    mixer->base.dump            = Mixer_Dump;
    mixer->base.onActivated     = Mixer_onActivated;
    mixer->base.onDeactivated   = Mixer_onDeactivated;
    mixer->base.onSetGet        = Mixer_onSetGet;
    mixer->base.dtor            = Mixer_dtor;

    mixer->base.ports           = 9;
    mixer->base.port[0].isInput = 0;

    *comp_pp = &mixer->base;

Exit:
    return result;
}




///////////////////////////////////////////////////////////////////////////////
//
// Splitter
//
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    comp_t             base;
    int                ready;
} splitter_comp_t;



static void Splitter_Process(comp_t* comp)
{
    splitter_comp_t* this = (splitter_comp_t*) comp;

    ALOG_ADIL_INT("Splitter_Process %X, outstanding_buffers=%u\n", (int) comp, this->base.outstanding_buffers);
    if (!isActive(&this->base.port[0])){
        return;
    }

    int progress;
    do {
        progress = 0;
        buf_t* buf = Port_PeekBuf(&this->base.port[0]);

        if (buf == 0) {
            ALOG_ADIL_INT("Splitter %X: no input buffer\n", (int) comp);
            return;
        }

        uint_t num_active = 0;
        uint_t i;
        for (i = 1 ; i <= 2 ; i++) {
            if (isActive(&this->base.port[i])) {
                num_active++;
            }
        }

        if (num_active == 2 && !this->ready && this->base.outstanding_buffers > 0) {
            ALOG_ADIL_INT("Splitter %X: %u outstanding buffers with 2 active ports. Waiting.\n", 
                (int) this, this->base.outstanding_buffers);
            return;
        }
        this->ready = 1;

        // Distribute buf to clients.
        // Refcnt when only running splitter is 1 here.
        for (i = 1 ; i <= 2 ; i++) {
            if (isActive(&this->base.port[i])) {
                Buf_UpdateRefcount(buf, 1);
                ALOG_ADIL_INT("Splitter %X: Forwarded buffer %X to port %u\n", (int) this, (int) buf, i);
                Port_Push(&this->base.port[i], buf);
            }
        }

        if (num_active > 0) {
            // The splitter needs to keep its own reference during operation
            Buf_Release(Port_PopBuf(&this->base.port[0]));
            progress = 1;
        } else {
            ALOG_ADIL_INT("Splitter %X: no outputs, kept buffer\n", (int) comp);
        }
    } while(progress);
}


static void Splitter_onActivated(comp_t* comp, OMX_U32 port)
{
    (void) port;
    splitter_comp_t* this = (splitter_comp_t*) comp;
    uint_t num_active = 0;
    uint_t i;

    for (i = 1 ; i <= 2 ; i++) {
        if (isActive(&this->base.port[i])) {
            num_active++;
        }
    }

    if (num_active == 2) {
        ALOG_ADIL_INT("Splitter %X: Splitter_onActivated num_active=2 --> "
            "ready=0 outstanding buffers: %d\n", 
            (int) this, this->base.outstanding_buffers);

        this->ready = 0;
    }
}


static OMX_ERRORTYPE Splitter_Factory(comp_t** comp_pp, const char* name)
{
    OMX_ERRORTYPE result = OMX_ErrorNone;

    if (strcmp(name, "ADM.splitter") != 0) {
        result = OMX_ErrorComponentNotFound;
        goto Exit;
    }

    if (!comp_pp) return OMX_ErrorInsufficientResources;

    splitter_comp_t* splitter = calloc(1, sizeof(splitter_comp_t));
    if (!splitter) {
        result = OMX_ErrorInsufficientResources;
        goto Exit;
    }

    Comp_Init(&splitter->base);
    splitter->base.process     = Splitter_Process;
    splitter->base.ports       = 3;
    splitter->base.onActivated = Splitter_onActivated;

    splitter->base.track_outstanding_buffers = 1;

    unsigned int i;
    for (i = 1 ; i < splitter->base.ports ; i++) {
        splitter->base.port[i].isInput = 0;
    }
    *comp_pp = &splitter->base;

Exit:
    return result;
}

#ifdef ADM_ENABLE_FEATURE_FAT_MODEM
///////////////////////////////////////////////////////////////////////////////
//
// Modem source
//
///////////////////////////////////////////////////////////////////////////////
#include "ste_adm_api_modem.h"

OMX_ERRORTYPE adm_dev_modem_init(comp_t* comp);
void adm_dev_modem_destroy(comp_t* comp);
void adm_dev_modem_init_buffer(comp_t* comp, int size_bytes);
void signalDownlinkAction(void);

typedef struct
{
    comp_t base;
    CompBuf_t compbuf;
    OMX_U32 dict_mode;
    OMX_U32 dict_mute;
} modem_src_comp_t;

static void modem_src_onDeactivated(comp_t* comp, OMX_U32 port)
{
    (void) port;
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    ALOG_ADIL("> modem_src_onDeactivated");

    switch(this->dict_mode){
        case STE_ADM_DICTAPHONE_UPLINK_DOWNLINK :
            modem_set_rec_rx_path(ADM_MODEM_PATH_CLOSE);
            modem_set_rec_tx_path(ADM_MODEM_PATH_CLOSE);
            break;
        case STE_ADM_DICTAPHONE_DOWNLINK :
            modem_set_rec_rx_path(ADM_MODEM_PATH_CLOSE);
            break;
        case STE_ADM_DICTAPHONE_UPLINK :
            modem_set_rec_tx_path(ADM_MODEM_PATH_CLOSE);
            break;
        default :
            assert(0);
            break;
    }
    ALOG_ADIL("< modem_src_onDeactivated");

}

static void modem_src_onActivated(comp_t* comp, OMX_U32 port)
{
    (void) port;
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    ALOG_ADIL("> modem_src_onActivated");

    if (isActive(&this->base.port[0])) {

        // modem output port sampling rate has been set according to cscall sample rate
        // initialize the modem src buffer size accordingly
        switch(this->base.port[0].omx_format.nSamplingRate){
            case 8000:
                adm_dev_modem_init_buffer(comp, 320);
                break;
            case 16000:
                adm_dev_modem_init_buffer(comp, 640);
                break;
            default:
                ALOG_ERR("ADIL: Unexpected modem sample rate\n");
                assert(0);
                break;
        }

        switch(this->dict_mode){
            case STE_ADM_DICTAPHONE_UPLINK_DOWNLINK :
                modem_set_rec_rx_path(ADM_MODEM_PATH_OPEN);
                modem_set_rec_tx_path(ADM_MODEM_PATH_OPEN);
                break;
            case STE_ADM_DICTAPHONE_DOWNLINK :
                modem_set_rec_rx_path(ADM_MODEM_PATH_OPEN);
                break;
            case STE_ADM_DICTAPHONE_UPLINK :
                modem_set_rec_tx_path(ADM_MODEM_PATH_OPEN);
                break;
            default :
                assert(0);
                break;
        }
    }
    ALOG_ADIL("< modem_src_onActivated");
}

static OMX_ERRORTYPE modem_src_onSetGet(comp_t* comp, ADM_CORE_INDEXTYPE nIndex, OMX_PTR pParamRaw, int isSet, int isParam)
{
    OMX_ERRORTYPE res = OMX_ErrorUnsupportedIndex;
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    switch (nIndex) {
        case ADM_IndexParamDictRecMode: {
            ONSETGET_PARAMETER_COMMON_CODE(ADM_PARAM_DICTRECMODETYPE);
            if (isSet) {
                this->dict_mode = pParam->nMode;
                res = OMX_ErrorNone;
            } else {
                pParam->nMode = this->dict_mode;
                res = OMX_ErrorNone;
            }
            break;
        }
        case ADM_IndexParamDictRecMute: {
            ONSETGET_CONFIG_WITH_PORT_COMMON_CODE(ADM_PARAM_DICTRECMUTETYPE);
            (void) port;
            if (isSet) {
                this->dict_mute = pParam->nMute;
                res = OMX_ErrorNone;
            } else {
                pParam->nMute = this->dict_mute;
                res = OMX_ErrorNone;
            }
            break;
        }
        default:
            res = OMX_ErrorUnsupportedIndex;
            break;
        }
    return res;
}

static void modem_src_Process(comp_t* comp)
{
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    ALOG_ADIL_INT("modem_src_Process %X", (int) this);

    if (isActive(&this->base.port[0])) {
        signalDownlinkAction();
    }
}

static void modem_src_dtor(comp_t* comp)
{
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    adm_dev_modem_destroy(&this->base);
}

static OMX_ERRORTYPE modem_src_Factory(comp_t** comp_pp, const char* name)
{
    OMX_ERRORTYPE res = OMX_ErrorNone;

    ALOG_ADIL_INT("Enter modem_src_Factory %s", name);

    if (strcmp(name, "ADM.modem_src") != 0) {
        return OMX_ErrorComponentNotFound;
    }

    if (comp_pp == NULL) {
        return OMX_ErrorInsufficientResources;
    }

    modem_src_comp_t* modem_src = calloc(1, sizeof(modem_src_comp_t));
    if (!modem_src) return OMX_ErrorInsufficientResources;

    Comp_Init(&modem_src->base);
    modem_src->base.process         = modem_src_Process;
    modem_src->base.onActivated     = modem_src_onActivated;
    modem_src->base.onDeactivated   = modem_src_onDeactivated;
    modem_src->base.onSetGet        = modem_src_onSetGet;
    modem_src->base.dtor            = modem_src_dtor;

    modem_src->base.ports           = 1;
    modem_src->base.port[0].isInput = 0;

    res = adm_dev_modem_init(&modem_src->base);

    *comp_pp = &modem_src->base;
    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
// Modem sink
//
///////////////////////////////////////////////////////////////////////////////
typedef struct
{
    comp_t base;
    int ready;
} modem_sink_comp_t;


static void modem_sink_onDeactivated(comp_t* comp, OMX_U32 port)
{
    (void) port;
    (void) comp;

    ALOG_ADIL("> modem_sink_onDeactivated");
}

static void modem_sink_onActivated(comp_t* comp, OMX_U32 port)
{
    (void) port;
    modem_sink_comp_t* this = (modem_sink_comp_t*) comp;
    uint_t num_active = 0;

    ALOG_ADIL("> modem_sink_onActivated");

    if (isActive(&this->base.port[0])) {
        num_active++;
    }

    if (num_active) {
        ALOG_ADIL_INT("modem sink %X: modem_sink_onActivated --> "
            "ready=0 outstanding buffers: %d\n",
            (int) this, this->base.outstanding_buffers);

        this->ready = 0;
    }
}

static void modem_sink_Process(comp_t* comp)
{
    (void) comp;
}

static OMX_ERRORTYPE modem_sink_Factory(comp_t** comp_pp, const char* name)
{
    if (strcmp(name, "ADM.modem_sink") != 0) {
        return OMX_ErrorComponentNotFound;
    }

    if (comp_pp == NULL) {
        return OMX_ErrorInsufficientResources;
    }

    modem_sink_comp_t* modem_sink = calloc(1, sizeof(modem_sink_comp_t));
    if (!modem_sink) return OMX_ErrorInsufficientResources;

    Comp_Init(&modem_sink->base);
    modem_sink->base.process         = modem_sink_Process;
    modem_sink->base.onActivated     = modem_sink_onActivated;
    modem_sink->base.onDeactivated   = modem_sink_onDeactivated;

    modem_sink->base.ports           = 1;
    modem_sink->base.port[0].isInput = 0;

    *comp_pp = &modem_sink->base;
    return OMX_ErrorNone;
}


///////////////////////////////////////////////////////////////////////////////
//
// Modem device
//
///////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include "linux_utils.h"
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/time.h>
#include "libcaifsocketbroker.h"

#define AUDIO_MODEM_OUT_BUF_FRAME_NB    2
#define AUDIO_MODEM_FRAME_SIZE_MS        20                                        // 20ms at 8k or 16k
#define AUDIO_MODEM_FRAME_SIZE_NS        AUDIO_MODEM_FRAME_SIZE_MS*1000000
#define AUDIO_MODEM_TIMEOUT_NS           AUDIO_MODEM_FRAME_SIZE_NS*2             // timeout for modem frame reception


int adm_dev_modem_open_fd(void);
void adm_dev_modem_close_fd(void);
OMX_ERRORTYPE adm_dev_modem_init(comp_t* comp);
void adm_dev_modem_destroy(comp_t* comp);

void * RxThreadEntry(void* arg);
void * TxThreadEntry(void* arg);

typedef struct dev_modem_s {
    int                 mAudioModemFd;
    modem_src_comp_t*   modem_src_comp;
    pthread_t           mRxThread;
    pthread_cond_t      mRxCond;
    pthread_mutex_t     mRxMutex;
    int dev_modem_initialized;
    int mIsFinishing;
    int mFrameSizeBytes;
} dev_modem_t;

#define AF_CAIF 37
#define PF_CAIF AF_CAIF
#define SOL_CAIF 278

dev_modem_t audioModemDev;
dev_modem_t *amh = &audioModemDev;;



OMX_ERRORTYPE adm_dev_modem_init(comp_t* comp)
{
    int thread_error;
    pthread_attr_t attr;
    struct sched_param sched_param;
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    ALOG_ADIL("> adm_dev_modem_init ");

    memset(amh, 0, sizeof(dev_modem_t));
    amh->modem_src_comp = this;
    this->dict_mute = 0;

    if(adm_dev_modem_open_fd() < 0)
    {
        return OMX_ErrorBadParameter;
    }

    pthread_attr_init(&attr);

    // init the condition variables
    pthread_cond_init(&amh->mRxCond, NULL);

    pthread_mutexattr_t mattr;
    int err = pthread_mutexattr_init(&mattr);
    if (err !=0) return OMX_ErrorBadParameter;

    err = pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    if (err !=0) return OMX_ErrorBadParameter;

    err = pthread_mutex_init(&amh->mRxMutex, &mattr);
    if (err !=0) return OMX_ErrorBadParameter;

    // get max priority from the scheduler
    int base_priority = sched_get_priority_max(SCHED_RR);
    err = pthread_attr_init(&attr);
    if (err !=0 ) return OMX_ErrorBadParameter;

    err = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (err !=0) return OMX_ErrorBadParameter;

    // set the priority to MAX_SCHEDULER_PRIORITY_NUMBER
    sched_param.sched_priority = base_priority ;

    err = pthread_attr_setschedparam(&attr, &sched_param);
    if (err !=0) return OMX_ErrorBadParameter;

    // create RX threads
    thread_error = pthread_create(&amh->mRxThread, &attr,
            &RxThreadEntry, this);

    if (thread_error != 0) {
        ALOG_ERR("ADIL: Failed to create the RX thread\n");
        return OMX_ErrorBadParameter;
    }

    amh->dev_modem_initialized=1;

    ALOG_ADIL("< adm_dev_modem_init done");

    return OMX_ErrorNone;

}


void adm_dev_modem_destroy(comp_t* comp)
{
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    ALOG_ADIL("> adm_dev_modem_destroy\n") ;

    if(amh->dev_modem_initialized){

        /*  wait for the thread to finish */
        amh->mIsFinishing = 1;
        signalDownlinkAction();

        pthread_join(amh->mRxThread, NULL);

        adm_dev_modem_close_fd();

        CompBuf_Destroy(&this->compbuf);

        amh->dev_modem_initialized=0;
    }

    ALOG_ADIL("< adm_dev_modem_destroy\n") ;
}

void adm_dev_modem_init_buffer(comp_t* comp, int size_bytes)
{
    modem_src_comp_t* this = (modem_src_comp_t*) comp;

    amh->mFrameSizeBytes = size_bytes;
    CompBuf_Init(&this->compbuf, &this->base, amh->mFrameSizeBytes*AUDIO_MODEM_OUT_BUF_FRAME_NB, 1);
}

int adm_dev_modem_open_fd(void)
{
    int fd;
    struct caif_socket_broker_error error;

#ifdef ADM_DEV_MODEM_STUB
    amh->mAudioModemFd = NULL;
    return 0;
#endif

    ALOG_ADIL("adm_dev_modem_open_fd");

    if (!caif_socket_broker_get_socket(&fd, &error, CAIF_SOCKET_BROKER_AUDIO_TAP))
    {
      ALOG_ERR("ADIL: Caif socket broker failed: %s%s%s\n",
          error.message, error.cause?": ":"",
          error.cause?error.cause:"");
      return -1;
    }
    amh->mAudioModemFd = fd;

    return 0;
}

void adm_dev_modem_close_fd(void)
{
    int res;

    if(amh->mAudioModemFd >= 0)
    {
        res = close(amh->mAudioModemFd);
        if (res != 0)
        {
            ALOG_ERR("ADIL: Modem audio driver closure failed [%d]\n", errno);
        }
        amh->mAudioModemFd = -1;
    }
}

void signalDownlinkAction(void) {
    int err = pthread_mutex_lock(&amh->mRxMutex);
    if (err !=0) {
        ALOG_ERR("ADIL: Error while unlocking the rx mutex: %d\n", err);
    }
    err = pthread_cond_signal(&amh->mRxCond);
    if (err !=0) {
        ALOG_ERR("ADIL: Error while unlocking the rx mutex: %d\n", err);
    }
    err = pthread_mutex_unlock(&amh->mRxMutex);
    if (err !=0) {
        ALOG_ERR("ADIL: Error while unlocking the rx mutex: %d\n", err);
    }
}

void waitDownlinkAction() {
    int err = pthread_mutex_lock(&amh->mRxMutex);
    if (err !=0) {
        ALOG_ERR("ADIL: Error while locking the rx mutex: %d\n", err);
    }

    err = pthread_cond_wait(&amh->mRxCond, &amh->mRxMutex);
    if (err !=0) {
        ALOG_ERR("ADIL: Error while waiting the rx cond: %d\n", err);
    }

    err = pthread_mutex_unlock(&amh->mRxMutex);
    if (err !=0) {
        ALOG_ERR("ADIL: Error while unlocking the rx mutex: %d\n", err);
    }
}

int get_time_of_day_ns(uint64_t* t_ns)
{
    struct timespec tv={0,0};

    if(clock_gettime(CLOCK_REALTIME,&tv)<0)
        return -1;
    else
        *t_ns = (uint64_t) (tv.tv_sec * 1000000000ULL + tv.tv_nsec);

    return 0;
}


#include <sys/resource.h>

void * RxThreadEntry(void* arg)
{
    int modem_fd = amh->mAudioModemFd;
    modem_src_comp_t* this = (modem_src_comp_t *) arg;
    int retval;
    int max_fd;
    buf_t* out;
    char* out_ptr;
    int bytes_free;
    int nbBytesRead;
    struct timespec tv;
    uint64_t now;
    uint64_t currentTime=0;
    uint64_t previousTime=0;

    uint64_t startTime=0;
    uint64_t diffTime=0;
    int frameCnt=0;
    int frameLost=0;
    int frameExpected=0;

    tv.tv_sec = 0;
    tv.tv_nsec = AUDIO_MODEM_TIMEOUT_NS;

    if(!this){
        ALOG_ERR("error : invalid modem source component handle");
        return NULL;
        }

    ALOG_ADIL("ModemSrc. start Rx Thread %X\n", (int) this);
    amh->mIsFinishing = 0;

    while(!amh->mIsFinishing)
    {
        // wait for filling request
        waitDownlinkAction();

        if(amh->mIsFinishing)
            continue;

        unsubscribe_timer_event(&this->base);

        ALOG_ADIL("ModemSrc Rx Thread %X. fill request received\n", (int) this);

        out = CompBuf_Peek(&this->compbuf);
        if(!out){
            ALOG_WARN("ADIL: ModemSrc Rx Thread %X. Could not get buffer\n", (int) this);
            continue;
        }

        out_ptr = out->data;
        bytes_free=out->bytes_available;
        out->bytes_valid=0;

        out->format.chan_cfg = (unsigned char) this->base.port[0].omx_format.nChannels;
        out->format.freq     = hz2esaafreq((uint_t) this->base.port[0].omx_format.nSamplingRate);

        while ((out->bytes_valid < out->bytes_available) && (!amh->mIsFinishing))
        {
            fd_set rd;
            FD_ZERO(&rd);
            FD_SET(modem_fd, &rd);
            max_fd = modem_fd+1;

            retval = pselect(max_fd, &rd, NULL, NULL, &tv, NULL);

            if (retval == -1)
            {
                if(errno == EINTR)
                    ALOG_ERR("ADIL: ModemSrc Rx Thread %X. pselect interrupted by signal", (int) this);
                else
                    ALOG_ERR("ADIL: ModemSrc Rx Thread %X. pselect error %d\n", (int) this, errno);
            }
            else if (retval)
            {
                if (FD_ISSET(modem_fd, &rd))
                {

#ifdef ADM_DEV_MODEM_STUB
                    // in stub case, fd is null. in this case pselect returns immediately
                    nbBytesRead = out->bytes_available;
#else
                    nbBytesRead = read(modem_fd, out_ptr, amh->mFrameSizeBytes);
#endif
                    if(!startTime)
                        startTime = get_time_of_day_ms();

                    if(get_time_of_day_ns(&currentTime)>=0){
                        if (previousTime && ((currentTime-previousTime)>2*AUDIO_MODEM_FRAME_SIZE_NS)){
                            ALOG_WARN("ADIL: ModemSrc Rx Thread %X. At least one frame lost. (wait time=%llu) (diff=%llu)\n", (int) this, currentTime-previousTime, currentTime-previousTime-AUDIO_MODEM_FRAME_SIZE_NS);
                        }
                        previousTime=currentTime;
                    }

                    if (nbBytesRead >= 0)
                    {
                        if (nbBytesRead == 0)
                        {
                            ALOG_ADIL_FLOW("ModemSrc Rx Thread: RXThreadEntry: Number of read bytes is null!\n");
                            usleep(100000);
                        }
                        else
                        {
                            bytes_free -= nbBytesRead;
                            out->bytes_valid += nbBytesRead;
                            out_ptr += nbBytesRead;
                            ALOG_ADIL_FLOW("ModemSrc Rx Thread %X: read %d, valid %lu, free %d, avalaible %lu\n", (int) this, nbBytesRead, out->bytes_valid, bytes_free, out->bytes_available);
                        }
                    }
                    else if(errno == EAGAIN)
                    {
                        ALOG_ERR("ADIL: ModemSrc Rx Thread %X: read error [EAGAIN]\n", (int) this);
                        usleep(100000);
                    }
                    else
                    {
                        ALOG_ERR("ADIL: ModemSrc Rx Thread %X: read error [%d]\n", (int) this, errno);
                    }
                }
            }
            else{
                // timeout expired
                ALOG_WARN("ADIL: ModemSrc Rx Thread %X: Timeout expired. No data read. Generate %d bytes of silence\n", (int) this, bytes_free);
                memset(out_ptr, 0, bytes_free);
                out->bytes_valid = out->bytes_available;
            }
        }

        {
            currentTime = get_time_of_day_ms();

            diffTime=currentTime - startTime;
            frameExpected = (int) (diffTime/AUDIO_MODEM_FRAME_SIZE_MS);
            frameCnt = frameCnt + AUDIO_MODEM_OUT_BUF_FRAME_NB;

            ALOG_ADIL_FLOW("ADIL: ModemSrc Rx Thread %X: frame count (%d), expected nb frames (%d)", this, frameCnt, frameExpected);
            if(frameExpected > (frameCnt+frameLost)){
                frameLost += (frameExpected - (frameCnt+frameLost));
                ALOG_WARN("ADIL: ModemSrc Rx Thread %X: %d frames lost", this, frameLost);
            }
        }

        if(this->dict_mute)
            memset(out->data, 0, out->bytes_available);

        if (isActive(&this->base.port[0])) {
            ALOG_ADIL_FLOW("ModemSrc Rx Thread %X: Forwarded buffer %X to port 0\n", (int) this, (int) out);
            Port_Push(&this->base.port[0], out);

            // subscribe timer event. used to wake up peer port (mixer input port) when mixer is no more scheduled.
            // this is required when all fill requests have been received by mixer component from application, before a new buffer from modem is available
            now = get_time_of_day_ms();
            subscribe_timer_event((comp_t *) this->base.port[0].peer, now + AUDIO_MODEM_FRAME_SIZE_MS/2);
        }
        CompBuf_Drop(&this->compbuf);
    }


    ALOG_ADIL("ModemSrc. Leave Rx Thread %X", (int) this);
    return NULL;
}

void * TxThreadEntry(void* arg)
{
    (void) arg;

    while(!amh->mIsFinishing)
    {
        // to be implemented
    }

    return NULL;
}
#endif
