/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define ADM_LOG_FILENAME "omx_io"
#define BEGINNING_OF_LOW_POWER_MODE_IN_MS 1000

#include "ste_adm_omx_log.h"
#include "ste_adm_dbg.h"
#include "ste_adm_msg.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <BSD_list.h>
#include <poll.h>
#include <sys/eventfd.h>
static void il_tool_io_empty_buffer_done(il_comp_t comp, ste_adm_omx_ioinfo_t *bufinfo_p);

static void il_tool_io_fill_buffer_done(il_comp_t comp,
            ste_adm_omx_ioinfo_t *bufinfo_p);

static void handle_request_deliver_data(il_comp_t comp,
                                        ste_adm_omx_ioinfo_t* bufinfo_p);


static pthread_mutex_t io_timer_mutex;
static pthread_t g_io_timer_thread;
static int g_io_timer_thread_alive;

typedef struct timer_entry_s
{
    msg_data_t* msg_p;
    uint64_t trigger_time;
    LIST_ENTRY(timer_entry_s) list_entry;
} timer_entry_t;

LIST_HEAD(timer_list_s, timer_entry_s);
static struct timer_list_s g_io_timer_event_list;
static int g_io_timer_wakeup_fd;


static uint64_t get_current_time_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ((uint64_t) ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}

static void subscribe_timer_event(msg_data_t* msg_p, uint64_t time)
{
    pthread_mutex_lock(&io_timer_mutex);

    timer_entry_t *entry = malloc(sizeof(timer_entry_t));
    if (entry == NULL) {
        ALOG_ERR("subscribe_timer_event failed to allocate memory");
        return;
    }

    entry->msg_p = msg_p;
    entry->trigger_time = time;
    LIST_INSERT_HEAD(&g_io_timer_event_list, entry, list_entry);

    const unsigned long long val = 1;
    write(g_io_timer_wakeup_fd, &val, sizeof(val));

    pthread_mutex_unlock(&io_timer_mutex);
}

static void* io_timer_thread(void *dummy)
{
    (void) dummy;
    int time_to_wait = -1;
    struct pollfd pollfd;

    pollfd.fd = g_io_timer_wakeup_fd;
    pollfd.events = POLLIN;

    while(g_io_timer_thread_alive) {
        int ret = poll(&pollfd, 1, time_to_wait);
        if (!g_io_timer_thread_alive) {
            ALOG_INFO_VERBOSE("io_timer_thread exiting");
            return NULL;
        }

        if (ret < 0) {
            ALOG_ERR("io_timer_thread: poll returned error, errno = %d (%s), revents = %x",
                    errno, strerror(errno), pollfd.revents);
            continue;
        } else if (ret > 0 && !(pollfd.revents & POLLIN)) {
            ALOG_ERR("io_timer_thread: poll returned POLLIN is not set, revents = %x",
                    pollfd.revents);
        } else if (ret != 0) {
            unsigned long long tmp;
            read(g_io_timer_wakeup_fd, &tmp, sizeof(tmp));
        }

        pthread_mutex_lock(&io_timer_mutex);

        uint64_t now = get_current_time_ms();
        ALOG_INFO_VERBOSE("io_timer_thread woke up from %s at %llu, revents=%x",
                ret == 0 ? "timeout" : "signal", now, pollfd.revents);

        timer_entry_t* temp;
        timer_entry_t* cur;
        uint64_t next_event_time = UINT64_MAX;
        LIST_FOREACH_SAFE(cur, &g_io_timer_event_list, list_entry, temp) {
            if (cur->trigger_time <= now) {
                LIST_REMOVE(cur, list_entry);
                srv_send_reply(&cur->msg_p->base);
                free(cur);
                cur = NULL;
            } else  if (cur->trigger_time < next_event_time) {
                next_event_time = cur->trigger_time;
            }
        }

        if (next_event_time == UINT64_MAX) {
            time_to_wait = -1;
            ALOG_INFO_VERBOSE("io_timer_thread no more components in list");
        } else {
            time_to_wait = next_event_time - now;
            ALOG_INFO_VERBOSE("io_timer_thread sleeping for %d ms at %llu",
                    time_to_wait, now);
        }

        pthread_mutex_unlock(&io_timer_mutex);
    }

    return NULL;
}

void ste_adm_omx_io_init(void)
{
    pthread_mutex_init(&io_timer_mutex, NULL);
    g_io_timer_wakeup_fd = eventfd(0, 0);

    g_io_timer_thread_alive = 1;
    pthread_create(&g_io_timer_thread, NULL, io_timer_thread, NULL);
}

void ste_adm_omx_io_deinit(void)
{
    g_io_timer_thread_alive = 0;
    const unsigned long long val = 1;
    write(g_io_timer_wakeup_fd, &val, sizeof(val));
    pthread_join(g_io_timer_thread, 0);

    close(g_io_timer_wakeup_fd);

    pthread_mutex_destroy(&io_timer_mutex);
}


// return 0: device is busy
//        1: device is idle (drained, no buffers owned by IL)
int il_tool_io_is_idle(il_comp_t comp, OMX_U32 port)
{
    pthread_mutex_lock(&comp->mutex);
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED);

    int is_idle = (bufinfo_p->owned_by_adm == bufinfo_p->buf_count);
    pthread_mutex_unlock(&comp->mutex);

    return is_idle;
}


void il_tool_io_subscribe_idle_callback_once(il_comp_t comp, OMX_U32 port, io_idle_callback_t cb_fp, void* cb_param)
{
    pthread_mutex_lock(&comp->mutex);
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED);

    if (cb_fp == NULL && bufinfo_p->idle_callback_fp != NULL) {
        bufinfo_p->idle_callback_fp = NULL;
    } else if (cb_fp != NULL && bufinfo_p->idle_callback_fp == NULL) {
        bufinfo_p->idle_callback_fp    = cb_fp;
        bufinfo_p->idle_callback_param = cb_param;
    } else {
        ADM_ASSERT(0);
    }

    if (bufinfo_p->owned_by_adm == bufinfo_p->buf_count && !bufinfo_p->eos_queued) {
        if (bufinfo_p->idle_callback_fp) {
            ALOG_INFO("Idle callback requested, already satisfied, issuing..\n");
            bufinfo_p->idle_callback_fp(bufinfo_p->idle_callback_param);
            bufinfo_p->idle_callback_fp = NULL;
        }
    }

    pthread_mutex_unlock(&comp->mutex);
}

void il_tool_io_eos_send(il_comp_t comp, ste_adm_omx_ioinfo_t* bufinfo_p)
{
    ADM_ASSERT(bufinfo_p->owned_by_adm >= 1);
    // The data (bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->pBuffer) has
    // already been written to using shared memory.
    bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->nFilledLen = 0;
    bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->nFlags     = OMX_BUFFERFLAG_EOS;

    ste_adm_res_t res = il_tool_log_EmptyThisBuffer(comp,
            bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("OMX_EmptyThisBuffer failed\n");
        return;
    }

    bufinfo_p->owned_by_adm--;
}


void il_tool_io_eos(il_comp_t comp, OMX_U32 port)
{
    pthread_mutex_lock(&comp->mutex);
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED);


    ADM_ASSERT(!bufinfo_p->eos_queued);
    if (bufinfo_p->owned_by_adm >= 1) {
        il_tool_io_eos_send(comp, bufinfo_p);
    } else {
        bufinfo_p->eos_queued = 1;
    }
    pthread_mutex_unlock(&comp->mutex);
}


void il_tool_io_init(ste_adm_omx_ioinfo_t *bufinfo_p)
{
    int i;

    for (i = 0; i < ADM_MAX_IO_PER_COMPONENT; i++)
    {
        memset(&bufinfo_p[i], 0, sizeof(ste_adm_omx_ioinfo_t));
        bufinfo_p[i].magic = 0xABBAFEED;
        bufinfo_p[i].shm_fd = -1;
    }
}

/**
*
*/
int il_tool_io_alloc(il_comp_t      comp,
                     OMX_U32        port,
                     int            shm_fd_to_reuse)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    pthread_mutex_lock(&comp->mutex);

    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];

    // FUTURE IMPROVEMENT: Select size based on sample rate etc. Problems if
    // buffer mapping is not 1:1?
    // Also investigate limits and handling of number of buffers.
    memset(bufinfo_p, 0, sizeof(ste_adm_omx_ioinfo_t));
    bufinfo_p->magic = 0xABBAFEED;

    pthread_cond_init(&bufinfo_p->cond_idle, NULL);



    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    IL_TOOL_INIT_CONFIG_STRUCT(portdef);
    portdef.nPortIndex        = port;

    ste_adm_res_t result = il_tool_GetParameter(comp,
                OMX_IndexParamPortDefinition, &portdef);
    if (result != STE_ADM_RES_OK) {
        // TODO: cleanup
        ALOG_ERR("OMX_GetParameter OMX_IndexParamPortDefinition failed\n");
        pthread_mutex_unlock(&comp->mutex);
        return -1;
    }

    bufinfo_p->is_src_port = (portdef.eDir == OMX_DirOutput);
    bufinfo_p->buf_count    = (int) portdef.nBufferCountActual;
    bufinfo_p->owned_by_adm = bufinfo_p->buf_count;
    bufinfo_p->bufsz        = (int) portdef.nBufferSize;

    if (bufinfo_p->buf_count > OMX_IO_MAX_BUFFERS) {
        ALOG_ERR("Too many buffers requested, %d > %d\n", bufinfo_p->buf_count, OMX_IO_MAX_BUFFERS);
        pthread_mutex_unlock(&comp->mutex);
        return -1;
    }

    OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
    pcm_mode.nPortIndex = port;

    result = il_tool_GetParameter(comp, OMX_IndexParamAudioPcm, &pcm_mode);
    if (result != STE_ADM_RES_OK) {
        ALOG_ERR("OMX_GetParameter OMX_IndexParamAudioPcm failed\n");
        pthread_mutex_unlock(&comp->mutex);
        return -1;
    }

    bufinfo_p->data_rate            = pcm_mode.nChannels *
                                        (pcm_mode.nBitPerSample / 8) *
                                        pcm_mode.nSamplingRate;
    bufinfo_p->cur_response_time_ms = 0;

    int num_bufs = bufinfo_p->buf_count;

    int idx;

    bufinfo_p->shm_size = bufinfo_p->bufsz * num_bufs;
    bufinfo_p->shm_fd = util_shm_create(shm_fd_to_reuse, bufinfo_p->shm_size);
    if (bufinfo_p->shm_fd <= -1) {
        ALOG_ERR("util_shm_create failed - could not create shared memory\n");
        bufinfo_p->shm_fd = -1;
        pthread_mutex_unlock(&comp->mutex);
        return -1;
    }

    bufinfo_p->shm_mmap_buf = mmap(0, (size_t) bufinfo_p->shm_size,
                                   PROT_READ|PROT_WRITE,
                                   MAP_SHARED,
                                   bufinfo_p->shm_fd, 0);
    if (bufinfo_p->shm_mmap_buf == (void*) -1) {
        ALOG_ERR("mmap failed, errno = %s\n", strerror(errno));
        close(bufinfo_p->shm_fd);
        bufinfo_p->shm_mmap_buf = 0;
        bufinfo_p->shm_fd       = -1;
        pthread_mutex_unlock(&comp->mutex);
        return -1;
    }

    for (idx=0 ; idx < num_bufs ; idx++) {
        result = il_tool_log_UseBuffer(comp, &bufinfo_p->hdr_p[idx], port,
                                       NULL, (unsigned int) bufinfo_p->bufsz,
                                       (OMX_U8*) bufinfo_p->shm_mmap_buf + bufinfo_p->bufsz*idx);

        if (result != STE_ADM_RES_OK) {
            ALOG_ERR("OMX_UseBuffer failed to provide a buffer\n");
            close(bufinfo_p->shm_fd);
            munmap(bufinfo_p->shm_mmap_buf, (size_t) bufinfo_p->shm_size);
            bufinfo_p->shm_mmap_buf = 0;
            bufinfo_p->shm_fd       = -1;
            pthread_mutex_unlock(&comp->mutex);
            return -1;
        }
    }

    // Enable drop of first buffers for Mic and HSetIn
    // to avoid click/pop sound caused by mic regulator
    if (!strcmp(il_tool_io_get_last_opened_dev(), STE_ADM_DEVICE_STRING_MIC) ||
        !strcmp(il_tool_io_get_last_opened_dev(), STE_ADM_DEVICE_STRING_HSIN)) {
        bufinfo_p->drop_buf_enabled = 1;
    }

    pthread_mutex_unlock(&comp->mutex);
    return 0;
}

void il_tool_get_buffers(il_comp_t comp, OMX_U32 port, int* fd, int *bufsize, int* num_bufs)
{
    pthread_mutex_lock(&comp->mutex);

    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];

    if (fd)       *fd       = bufinfo_p->shm_fd;
    if (bufsize)  *bufsize  = bufinfo_p->bufsz;
    if (num_bufs) *num_bufs = bufinfo_p->buf_count;
    pthread_mutex_unlock(&comp->mutex);
}

void il_tool_io_move_towards_idle(il_comp_t comp, OMX_U32 port)
{
    pthread_mutex_lock(&comp->mutex);
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED);
    bufinfo_p->idle_state_wanted = 1;
    pthread_mutex_unlock(&comp->mutex);
}

void il_tool_io_wait_until_idle(il_comp_t comp, OMX_U32 port)
{
    pthread_mutex_lock(&comp->mutex);
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED);
    bufinfo_p->idle_state_wanted = 1;

    while ( !(bufinfo_p->owned_by_adm == bufinfo_p->buf_count)) {
        ALOG_INFO("il_tool_io_wait_until_idle: %X.%d not idle, waiting..\n", comp, port);
        pthread_cond_wait(&bufinfo_p->cond_idle, &comp->mutex);
    }

    pthread_mutex_unlock(&comp->mutex);
    ALOG_INFO("il_tool_io_wait_until_idle: %X.%d idle, returning\n", comp, port);
}

/**
* [in] device_id   Must be a valid device
*
*/
int il_tool_io_dealloc(il_comp_t comp, OMX_U32 port)
{
    pthread_mutex_lock(&comp->mutex);
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED && bufinfo_p->buf_count > 0);

    ADM_ASSERT(bufinfo_p->shm_fd != -1);
    ADM_ASSERT(bufinfo_p->shm_mmap_buf != 0);

    int result = 0;
    unsigned int idx;
    for (idx=0 ; idx < bufinfo_p->buf_count ; idx++) {
        OMX_ERRORTYPE omx_result = il_tool_log_FreeBuffer(comp, port, bufinfo_p->hdr_p[idx]);
        if (omx_result != OMX_ErrorNone) {
            ALOG_ERR("Failed to free OMX buffer!!!\n");
            pthread_mutex_unlock(&comp->mutex);
            result = -1;
        }
        bufinfo_p->hdr_p[idx] = NULL;
    }
    pthread_cond_destroy(&bufinfo_p->cond_idle);

    munmap(bufinfo_p->shm_mmap_buf, (size_t) bufinfo_p->shm_size);
    close(bufinfo_p->shm_fd);

    bufinfo_p->magic = 0xDEADBEEF;
    pthread_mutex_unlock(&comp->mutex);

    srv_read_and_drop_il_messages(comp, port, 0, 0);
    return result;
}

/**
* Feed data to the specified IL component.
*
*
*/
void il_tool_io_feed(il_comp_t      comp,
                        OMX_U32        port,
                        msg_data_t* msg_p)
{
    pthread_mutex_lock(&comp->mutex);
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ADM_ASSERT(msg_p != NULL);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED && bufinfo_p->buf_count > 0);
    ADM_ASSERT(msg_p->buf_idx >= 0 && (unsigned int) msg_p->buf_idx < bufinfo_p->buf_count);

    ALOG_INFO_VERBOSE("il_tool_io_feed %X.%d\n", comp, port);


    if (bufinfo_p->owned_by_adm == 0) {
        // Should not happen, since the feed API would block last time until
        // at least one buffer is free. Could happen if the API socket
        // is accessed directly, though, so we can't assert it.
        ALOG_ERR("io_cmd_data_feed: bufinfo_p->owned_by_adm == 0. Direct socket abuse?\n");
        pthread_mutex_unlock(&comp->mutex);
        return;
    }

    if (bufinfo_p->hdr_p[msg_p->buf_idx]->nAllocLen < msg_p->data_size) {
        ALOG_ERR("io_cmd_data_feed: buffer not large enough: %d < %d\n", bufinfo_p->hdr_p[msg_p->buf_idx]->nAllocLen, msg_p->data_size);

        msg_p->base.result = STE_ADM_RES_INVALID_BUFFER_SIZE;
        srv_send_reply(&msg_p->base);
        pthread_mutex_unlock(&comp->mutex);
        return;
    }


    ADM_ASSERT(bufinfo_p->hdr_p[msg_p->buf_idx]->pBuffer != 0);
    ADM_ASSERT(bufinfo_p->hdr_p[msg_p->buf_idx]->nOffset == 0);

    // The data (bufinfo_p->hdr_p[msg_p->buf_idx]->pBuffer) has
    // already been written to using shared memory.
    bufinfo_p->hdr_p[msg_p->buf_idx]->nFilledLen = msg_p->data_size;
    bufinfo_p->hdr_p[msg_p->buf_idx]->nFlags     = 0;

    ste_adm_res_t res = il_tool_log_EmptyThisBuffer(comp,
            bufinfo_p->hdr_p[msg_p->buf_idx]);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("OMX_EmptyThisBuffer failed\n");
        msg_p->base.result = STE_ADM_RES_IL_UNKNOWN_ERROR;
        srv_send_reply(&msg_p->base);
        pthread_mutex_unlock(&comp->mutex);
        return;
    }

    // Increase next_buf_idx so we know which buffer to use in case EOS shall be sent
    bufinfo_p->next_buf_idx = (msg_p->buf_idx + 1) % bufinfo_p->buf_count;
    bufinfo_p->owned_by_adm--;

    if (bufinfo_p->cur_response_time_ms == 0) {
        bufinfo_p->cur_response_time_ms = get_current_time_ms();
    }

    if (msg_p->lpa_mode != 1) {
        bufinfo_p->time_entering_lpa_mpde = 0;
    }
    else if ((msg_p->lpa_mode == 1) && (bufinfo_p->time_entering_lpa_mpde==0)) {
        bufinfo_p->time_entering_lpa_mpde = get_current_time_ms();
    }

    if (bufinfo_p->owned_by_adm == 0) {
        ALOG_INFO_VERBOSE("IO: Buffers owned by ADM are now 0, ack delayed\n");
        ADM_ASSERT(bufinfo_p->msg_p == NULL);
        bufinfo_p->msg_p = msg_p;
        pthread_mutex_unlock(&comp->mutex);
        return;
    }

    ALOG_INFO_VERBOSE("IO: Buffers owned by ADM are now %d, sending ack\n",
            bufinfo_p->owned_by_adm);
    ADM_ASSERT(bufinfo_p->msg_p == NULL);

    uint64_t now = get_current_time_ms();
    if (msg_p->lpa_mode == 1) {

        unsigned int buf_duration_ms = (msg_p->data_size * 1000 /
                                            bufinfo_p->data_rate) ;

        /* Provide response to ANM immediately in case of Low Power Audio.
         * Except the initial period of time in Low Power Audio.This in order to
         * avoid a peak in data consumption.
         */
        if ((now - bufinfo_p->time_entering_lpa_mpde) > BEGINNING_OF_LOW_POWER_MODE_IN_MS) {
            bufinfo_p->cur_response_time_ms = now;
        }
        else {
            bufinfo_p->cur_response_time_ms = now + (buf_duration_ms/2);
        }
    } else  if (bufinfo_p->owned_by_adm == bufinfo_p->buf_count - 1) {

        // Upper hard limit (all buffers are owned by ADM), respond immediately
        // in order to fill audio chain quickly
        ALOG_INFO_VERBOSE("IO: All buffers owned by ADM, provide response to " \
                "ANM immediately in order to fill audio chain with buffers");
        bufinfo_p->cur_response_time_ms = now;
    } else {

        // Normal case, calculate when to provide the response
        unsigned int buf_duration_ms = msg_p->data_size * 1000 /
                                            bufinfo_p->data_rate;
        if (bufinfo_p->owned_by_adm >= 2) {
            // Alot of buffers are owned by ADM, reduce buffer duration to catch
            // up a bit
            buf_duration_ms += -1;
        }

        bufinfo_p->cur_response_time_ms += buf_duration_ms;
    }

    int sleep_time_ms = bufinfo_p->cur_response_time_ms - now;
    if (sleep_time_ms == 0) {

        // Respond immediately
        srv_send_reply(&msg_p->base);
    } else {

        // Use timer to respond after a while
        subscribe_timer_event(msg_p, bufinfo_p->cur_response_time_ms);
    }

    pthread_mutex_unlock(&comp->mutex);
}

void il_tool_io_transfer_done(il_comp_t comp, OMX_U32 port, OMX_BUFFERHEADERTYPE* buf_p)
{
    pthread_mutex_lock(&comp->mutex);
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED && bufinfo_p->buf_count > 0);

    (void) buf_p;
    if (bufinfo_p->is_src_port) {
        il_tool_io_fill_buffer_done(comp, bufinfo_p);
    } else {
        il_tool_io_empty_buffer_done(comp, bufinfo_p);
    }
    pthread_mutex_unlock(&comp->mutex);
}

static void il_tool_io_empty_buffer_done(il_comp_t comp, ste_adm_omx_ioinfo_t *bufinfo_p)
{
    // mutex already taken
    bufinfo_p->owned_by_adm++;

    // Assert the IL components do not issue incorrect callbacks
    ADM_ASSERT(bufinfo_p->owned_by_adm <= bufinfo_p->buf_count);
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED && bufinfo_p->buf_count > 0);

    if (bufinfo_p->eos_queued) {
        il_tool_io_eos_send(comp, bufinfo_p);
        bufinfo_p->eos_queued = 0;
    }

    // If we reached 0 available buffers before the callback, the API call blocked, if
    // there is a message (otherwise it could be EOS frame)
    if (bufinfo_p->owned_by_adm == 1 && bufinfo_p->msg_p) {
        ALOG_INFO_VERBOSE("IO: Available buffers were 0, sending delayed ack\n");

        uint64_t now = get_current_time_ms();
        if (bufinfo_p->msg_p->lpa_mode == 1) {

            // Provide response to ANM immediately in case of Low Power Audio
            bufinfo_p->cur_response_time_ms = now;
        } else {

            // Normal case, calculate when to provide the response
            unsigned int buf_duration_ms = bufinfo_p->msg_p->data_size * 1000 /
                                                bufinfo_p->data_rate;
            bufinfo_p->cur_response_time_ms += buf_duration_ms;
        }

        int sleep_time_ms = bufinfo_p->cur_response_time_ms - now;
        if (sleep_time_ms <= 0) {

            // If the sleep time is negative it means that buffers are fed
            // faster than they are consumed by the audio chain. If so, slow it
            // down a bit by resetting the time base to current time (next
            // response will be delayed a bit).
            if (sleep_time_ms < 0) {
                ALOG_INFO_VERBOSE("IO: Response sleep time negative, reset time base");
                bufinfo_p->cur_response_time_ms = now;
            }

            // Respond immediately
            srv_send_reply(&bufinfo_p->msg_p->base);
        } else {

            // Use timer to respond after a while
            subscribe_timer_event(bufinfo_p->msg_p, bufinfo_p->cur_response_time_ms);
        }
        bufinfo_p->msg_p = NULL;
    }

    if (bufinfo_p->owned_by_adm == bufinfo_p->buf_count) {
        // The two threads involved:
        // Normal thread: executes here. Signals the condition variable below.
        // Worker thread: blocks on cond_wait.
        pthread_cond_signal(&bufinfo_p->cond_idle);
    }

    // A non-worker thread has subscribed to the idle callback
    if (bufinfo_p->owned_by_adm == bufinfo_p->buf_count) {
        if (bufinfo_p->idle_callback_fp) {
            ADM_ASSERT(!bufinfo_p->eos_queued);
            ALOG_INFO("il_tool_io_empty_buffer_done: Idle callback requested, issuing..\n");
            bufinfo_p->idle_callback_fp(bufinfo_p->idle_callback_param);
            bufinfo_p->idle_callback_fp = NULL;
        }
    }

    ALOG_INFO_VERBOSE("IO: Buffers available to ADM now %d, ack sent before when buffer provided\n", bufinfo_p->owned_by_adm);
}
















// TODO: Fix protocol, should send size and timestamp
// TODO: remove fill_active mechanism (fill when buffers are allocated) when timestamps in place?
//       (user can then throw away too old buffers)

// Caller is not responsible for sending reply msg
void il_tool_io_request_data(il_comp_t comp, OMX_U32 port, msg_data_t* msg_p)
{
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(msg_p != NULL);
    ADM_ASSERT(port < ADM_MAX_IO_PER_COMPONENT);
    ste_adm_omx_ioinfo_t *bufinfo_p = &comp->bufinfo[port];
    ADM_ASSERT(bufinfo_p->magic == 0xABBAFEED);
    ADM_ASSERT(!bufinfo_p->idle_state_wanted);

    ALOG_INFO_VERBOSE("il_tool_io_request_data %X.%d\n", comp, port);

    if (bufinfo_p->msg_p) {
        // Can not happen through normal API, so no use sending a reply msg
        ALOG_ERR("request data done when request is in progress; socket abused?\n");
        return;
    }

    if (bufinfo_p->has_error) {
        // Report an old error
        msg_p->base.result = STE_ADM_RES_IL_UNKNOWN_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    bufinfo_p->msg_p = msg_p;

    if (!bufinfo_p->fill_active) {
        ADM_ASSERT(bufinfo_p->owned_by_adm == bufinfo_p->buf_count);
        while (bufinfo_p->owned_by_adm) {
            bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->nFilledLen = 0;
            ste_adm_res_t res = il_tool_log_FillThisBuffer(comp,
                    bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("FillThisBuffer failed!\n");
                msg_p->base.result = STE_ADM_RES_IL_UNKNOWN_ERROR;
                srv_send_reply(&msg_p->base);
                bufinfo_p->msg_p = 0;
                return;
            }

            // Will just roll around, ultimately pointing to the first sent buffer..
            bufinfo_p->next_buf_idx = (bufinfo_p->next_buf_idx + 1) % bufinfo_p->buf_count;
            bufinfo_p->owned_by_adm--;
        }
        bufinfo_p->fill_active = 1;
        ADM_ASSERT(bufinfo_p->owned_by_adm == 0);
    }

    // if we have at least one buffer, return it to user and then pass it to IL
    if (bufinfo_p->owned_by_adm > 0) {
        handle_request_deliver_data(comp, bufinfo_p);
    }
}



static void il_tool_io_fill_buffer_done(il_comp_t comp,
        ste_adm_omx_ioinfo_t *bufinfo_p)
{
    // mutex already taken

    bufinfo_p->owned_by_adm++;

    if (bufinfo_p->msg_p) {
        handle_request_deliver_data(comp, bufinfo_p);
    } else if (bufinfo_p->idle_state_wanted) {
        // Just drop the data. Point to next buffer as the next one to deliver,
        // for completeness.
        bufinfo_p->next_buf_idx = (bufinfo_p->next_buf_idx+1) % bufinfo_p->buf_count;
    }

    if (bufinfo_p->idle_state_wanted) {
        if (bufinfo_p->owned_by_adm == bufinfo_p->buf_count) {
            pthread_cond_signal(&bufinfo_p->cond_idle);
        }
    }

    // A non-worker thread has subscribed to the idle callback
    if (bufinfo_p->owned_by_adm == bufinfo_p->buf_count) {
        if (bufinfo_p->idle_callback_fp) {
            ALOG_INFO("il_tool_io_fill_buffer_done: Idle callback requested, issuing..\n");
            bufinfo_p->idle_callback_fp(bufinfo_p->idle_callback_param);
            bufinfo_p->idle_callback_fp = NULL;
        }
    }

}

static int convert_timestamp(OMX_TICKS omx_ticks, uint64_t* adm_ts)
{
    ADM_ASSERT(adm_ts != NULL);

    // TODO: figure out what format timestamps from IL have
    if (omx_ticks < 0) {
        ALOG_INFO("NOTE: Negative timestamp: %ld\n", omx_ticks);
        *adm_ts = 0;
        return 0;
    }
    *adm_ts = (uint64_t) omx_ticks;
    return 0;
}

static void handle_request_deliver_data(il_comp_t comp, ste_adm_omx_ioinfo_t* bufinfo_p)
{
    // Assumes IL does not deliverbuffers out-of-order
    ADM_ASSERT(bufinfo_p->owned_by_adm > 0);
    ADM_ASSERT(bufinfo_p->msg_p);

    // Send header
    bufinfo_p->msg_p->data_size = bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->nFilledLen;
    convert_timestamp(bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->nTimeStamp, &bufinfo_p->msg_p->ts);

    int nbr_of_bufs_to_drop;
    if (bufinfo_p->drop_buf_enabled && !bufinfo_p->drop_buf_finished) {
        // Calculate number of buffers to drop to avoid click/pop sound caused by mic regulator.
        // Drop buffers corresponding to IL_TOOL_DROP_DATA_MS ms.
        // +1 since IL_DROP_DATA_MS might not be equally divided by buf_duration_ms
        int buf_duration_ms = bufinfo_p->msg_p->data_size * 1000 / bufinfo_p->data_rate;
        nbr_of_bufs_to_drop = IL_TOOL_DROP_DATA_MS / buf_duration_ms + 1;
    }

    if (bufinfo_p->drop_buf_enabled && !bufinfo_p->drop_buf_finished &&
        bufinfo_p->drop_buf_cnt < nbr_of_bufs_to_drop) {
        bufinfo_p->drop_buf_cnt++;
        ALOG_INFO("handle_request_deliver_data: Drop buffer %d of %d", bufinfo_p->drop_buf_cnt, nbr_of_bufs_to_drop);
    } else {
        bufinfo_p->msg_p->buf_idx = bufinfo_p->next_buf_idx;
        srv_send_reply(&bufinfo_p->msg_p->base);
        bufinfo_p->msg_p = 0;
        bufinfo_p->drop_buf_finished = 1;
    }


    if (!bufinfo_p->idle_state_wanted) {
        // give it to IL to be filled
        bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]->nFilledLen = 0;
        ste_adm_res_t result = il_tool_log_FillThisBuffer(comp,
                bufinfo_p->hdr_p[bufinfo_p->next_buf_idx]);
        if (result != STE_ADM_RES_OK) {
            ALOG_ERR("OMX_FillThisBuffer failed\n");
            bufinfo_p->has_error = 1;
            return;
        }

        bufinfo_p->owned_by_adm--;
    }

    bufinfo_p->next_buf_idx = (bufinfo_p->next_buf_idx+1) % bufinfo_p->buf_count;
}






















