/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
// FUTURE IMPROVEMENT: Write buffer for outgoing data?

#define ADM_LOG_FILENAME "srv"

#include "ste_adm_srv.h"
#include "ste_adm_omx_io.h"
#include "ste_adm_omx_core.h"
#include "ste_adm_omx_log.h"
#include "ste_adm_db.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/resource.h>

#define __USE_UNIX98
#include <pthread.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <signal.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#ifndef ADM_DBG_X86
  #include <hardware_legacy/power.h>
  #define STE_ADM_WAKE_LOCK "STE_ADM"
#endif

#include "ste_adm_omx_log.h"

#define STE_ADM_RECV_BUF_SIZE (1024*16)
#define ADM_MAX_CLIENTS       (16)
#define STE_ADM_LISTEN_BACKLOG (20)

#include "ste_adm_dbg.h"        // ALOG_ERR etc
#include "ste_adm_msg.h"        // Messages, as well as msg handler fn prototypes.


#include "ste_adm_api_cscall.h"
#include "ste_adm_api_d2d.h"
#include "ste_adm_api_devset.h"
#include "ste_adm_api_tg.h"
#include "ste_adm_api_util.h"
#include "ste_adm_omx_tool.h"


// We only support one request at a time per file descriptor.
// If some client does something ugly like connect to the
// ADM on thread A and gets fd = 12 as connection handle, and
// then issues at the same time one request on fd 12 from thread A
// and one from thread B, the result for the client is undefined.
// The ADM however should not crash (unless the result is some
// messed up data that goes undetected directly to IL and
// causes it to crash, but that's technically an IL crash).


// Mutex used to protect the list of received IL events.
// It is not allowed to hold g_srv_il_mutex and
// at the same time ask for another mutex/semaphore.
static pthread_mutex_t g_srv_il_mutex;

// Mutex used to protect the call to epoll_wait(), and
// the data returned from epoll_wait.
// Always take g_epoll_mutex before you take
// g_adm_threads_sem (if you need both).
static pthread_mutex_t g_epoll_mutex;

// Semaphore used to handle distinction between worker threads
// and normal threads in the ADM.
// Down this semaphore to become a normal threading
// Up this semaphore when a worker thread, or when
// waiting for events (epoll_wait())
static sem_t g_adm_threads_sem;

static int g_adm_wakeup_event_fd;


// Set to tell ADM to shut down (without closing connections)
// Used to deallocate resources, to be able to run Valgrind
static int g_dbg_srv_shutdown = 0;

static int srv_setup_listener_sck(void);
static int prepare_main_loop();
static int g_listen_sock;
static int g_epoll_fd;

static int g_any_requests_queued = 0;
static int g_disconnect_request_rescan = 0;

static struct client_info g_client_tbl[ADM_MAX_CLIENTS];

static int g_il_callback_pipe[2] = { -1, -1 };

static int g_srv_serialized_request_in_progress = 0;

// Before init is completed, we don't have any normal/worker
// thread distinction.
static int g_srv_dbg_init_done = 0;

// The pthread that has requested to do a serialized request.
static pthread_t g_dbg_srv_serialized_request_pthread;

static int try_begin_serialized_request()
{
    if (g_srv_serialized_request_in_progress) {
        return 0;
    }
    g_srv_serialized_request_in_progress = 1;
    g_dbg_srv_serialized_request_pthread = pthread_self();

#ifndef ADM_DBG_X86
    if (acquire_wake_lock(PARTIAL_WAKE_LOCK, STE_ADM_WAKE_LOCK) < 0) {
        ALOG_ERR("try_begin_serialized_request: Failed to acquire wakelock.");
    }
#endif

    return 1;
}

static void end_serialized_request()
{
    ADM_ASSERT(g_srv_serialized_request_in_progress);
    g_srv_serialized_request_in_progress = 0;
    g_dbg_srv_serialized_request_pthread = 0;

#ifndef ADM_DBG_X86
    if (release_wake_lock(STE_ADM_WAKE_LOCK) < 0) {
        ALOG_ERR("end_serialized_request: Failed to release wakelock.");
    }
#endif
}

void srv_dbg_assert_is_serialized_request(const char* f, int l)
{
   if (!g_srv_serialized_request_in_progress) { ALOG_ERR("srv_dbg_assert_is_serialized_request %s %d\n", f, l); }

    ADM_ASSERT(g_srv_serialized_request_in_progress);
    ADM_ASSERT(pthread_equal(pthread_self(),g_dbg_srv_serialized_request_pthread));
}


static int dispatch_il_messages();

static int g_dbg_num_worker_threads = 0;
static pthread_t g_dbg_worker_thread = 0;

void srv_become_worker_thread()
{
    // Allows another thread to start executing requests.
    ADM_ASSERT(g_dbg_num_worker_threads == 0);
    ADM_ASSERT(g_srv_dbg_init_done);

    // Only a serialized request may become a worker thread
    ADM_ASSERT(g_srv_serialized_request_in_progress);
    ADM_ASSERT(pthread_equal(pthread_self(), g_dbg_srv_serialized_request_pthread));

    ALOG_INFO_VERBOSE("Thread %X wants to become a worker thread..\n", pthread_self());
    sem_post(&g_adm_threads_sem);
    g_dbg_num_worker_threads++;
    ALOG_INFO_VERBOSE("Thread %X is now a worker thread\n", pthread_self());
    g_dbg_worker_thread = pthread_self();

    const unsigned long long val = 1;
    write(g_adm_wakeup_event_fd, &val, sizeof(val));

#ifndef ADM_DBG_X86
    if (setpriority(PRIO_PROCESS, gettid(), 10)) {
        ALOG_ERR("setpriority(PRIO_PROCESS, gettid(), 10) failed\n");
    }
#endif
}

void srv_become_normal_thread()
{
    ADM_ASSERT(g_dbg_num_worker_threads == 1);
    ADM_ASSERT(g_srv_dbg_init_done);
    ALOG_INFO_VERBOSE("Thread %X wants to become a normal thread..\n", pthread_self());
    sem_wait(&g_adm_threads_sem);
    g_dbg_num_worker_threads--;
    ALOG_INFO_VERBOSE("Thread %X is now a normal thread\n", pthread_self());
    g_dbg_worker_thread = 0;

#ifndef ADM_DBG_X86
    if (setpriority(PRIO_PROCESS, gettid(), -10)) {
        ALOG_ERR("setpriority(PRIO_PROCESS, gettid(), -10) failed\n");
    }
#endif
}

void srv_dbg_assert_is_not_worker_thread(const char* f, int l)
{
#ifdef ADM_DEBUG
    if( !(!g_srv_dbg_init_done ||
           g_dbg_num_worker_threads == 0 ||
           !pthread_equal(pthread_self(), g_dbg_worker_thread))) { ALOG_ERR("srv_dbg_assert_is_not_worker_thread %s %d\n", f, l); }

    ADM_ASSERT(!g_srv_dbg_init_done ||
           g_dbg_num_worker_threads == 0 ||
           !pthread_equal(pthread_self(), g_dbg_worker_thread));
#else
    (void) f;
    (void) l;
#endif
}

void srv_dbg_assert_is_worker_thread(const char* f, int l)
{
#ifdef ADM_DEBUG
    if( !(!g_srv_dbg_init_done ||
           (g_dbg_num_worker_threads == 1 && pthread_equal(pthread_self(), g_dbg_worker_thread)))) { ALOG_ERR("srv_dbg_assert_is_worker_thread %s %d\n", f, l); }

    ADM_ASSERT(!g_srv_dbg_init_done ||
           (g_dbg_num_worker_threads == 1 && pthread_equal(pthread_self(), g_dbg_worker_thread)));
#else
    (void) f;
    (void) l;
#endif
}


static void dispatch(struct client_info* client_p, msg_base_t* msg_p);



static struct client_info* alloc_client(int fd)
{
    ADM_ASSERT(fd >= 0);
    int i;
    for (i=0 ; i < ADM_MAX_CLIENTS ; i++) {
        if (g_client_tbl[i].fd == -1) {
            g_client_tbl[i].fd                 = fd;
            g_client_tbl[i].client_state_p     = NULL;
            g_client_tbl[i].busy               = 0;
            g_client_tbl[i].close_requested    = 0;
            g_client_tbl[i].recv_buf           = NULL;
            g_client_tbl[i].recv_buf_size      = 0;
            g_client_tbl[i].request_queued     = 0;
            g_client_tbl[i].disconnect_requested = 0;

            return &g_client_tbl[i];
        }
    }

    return NULL;
}

#ifdef ADM_DEBUG
static int dbg_num_clients()
{
    int c = 0;
    int i;
    for (i=0 ; i < ADM_MAX_CLIENTS ; i++)
        if (g_client_tbl[i].fd != -1)
            c++;

    return c;
}
#endif

static void free_client(struct client_info* client_p)
{
    ADM_ASSERT(client_p->fd != -1);

    if (client_p->recv_buf) {
        memset(client_p->recv_buf, 0xBF, client_p->recv_buf_size);
        free(client_p->recv_buf);
    }
    client_p->recv_buf = 0; // just for cleanliness
    client_p->recv_buf_size = 0;
    memset(client_p, 0xCA, sizeof(*client_p));
    client_p->fd = -1;
}

ste_adm_res_t srv_init()
{
    int i;
    for (i=0 ; i < ADM_MAX_CLIENTS ; i++) {
        g_client_tbl[i].fd = -1;
    }

    ste_adm_res_t res = STE_ADM_RES_OK;
    int res_io_pipe = 0;
    int res_thread_sem = 0;
    int res_srv_il_mutex = 0;
    int res_epoll_mutex = 0;
    int res_adm_sem = 0;
    int res_listen_sock = 0;


    int retval = pthread_mutex_init(&g_srv_il_mutex, 0);
    if (retval != 0) {
        ALOG_ERR("pthread_mutex_init failed, error = %d\n", retval);
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }
    res_srv_il_mutex=1;

    retval = pthread_mutex_init(&g_epoll_mutex, 0);
    if (retval != 0) {
        ALOG_ERR("pthread_mutex_init failed, error = %d\n", retval);
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }
    res_epoll_mutex = 1;

    retval = sem_init(&g_adm_threads_sem, 0, 1);
    if (retval != 0) {
        ALOG_ERR("Failed to create semaphore: %s\n", strerror(errno));
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }
    res_thread_sem=1;

    if (pipe(g_il_callback_pipe)) {
        ALOG_ERR("pipe() failed\n");
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }
    res_io_pipe=1;

    if (fcntl(g_il_callback_pipe[0], F_SETFL, O_NONBLOCK) != 0) {
        ALOG_ERR("Failed to set read end of I/O pipe to O_NONBLOCK\n");
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }

    retval = adm_sem_init(&g_adm_event_sem, 0);
    if (retval != 0) {
        ALOG_ERR("Failed to create semaphore: %s\n", strerror(errno));
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }
    res_adm_sem=1;


    int listener_fd = srv_setup_listener_sck();
    if (listener_fd == -1) {
        ALOG_ERR("srv_mainloop(): failed to setup listener socket\n");
        res = STE_ADM_RES_ERR_BASIC_RESOURCE;
        goto out;
    }
    res_listen_sock = 1;

    g_listen_sock = listener_fd;

    g_adm_wakeup_event_fd = eventfd(0, 0);

    retval = prepare_main_loop();

    if(retval != 0) {
      ALOG_ERR("prepare_main_loop: failed");
      res = STE_ADM_RES_ERR_BASIC_RESOURCE;
    }

out:
    if (res != STE_ADM_RES_OK) {
        if (res_io_pipe) {
            close(g_il_callback_pipe[0]);
            close(g_il_callback_pipe[1]);
        }

        if (res_thread_sem) {
            sem_destroy(&g_adm_threads_sem);
        }

        if (res_srv_il_mutex) {
            pthread_mutex_destroy(&g_srv_il_mutex);
        }

        if (res_epoll_mutex) {
            pthread_mutex_destroy(&g_epoll_mutex);
        }

        if (res_adm_sem) {
            adm_sem_destroy(&g_adm_event_sem);
        }

        if (res_listen_sock) {
            close(listener_fd);
        }
    }

    return res;
}

void srv_get_clients(int* max_clients, struct client_info** client_tbl)
{
    *max_clients = ADM_MAX_CLIENTS;
    *client_tbl = g_client_tbl;
}

// Transfer an entire buffer. Handle if write() fails or does partial
// transfers.
static void write_all(int fd, const void* buf_p, size_t size)
{
    size_t pos=0;
    while (pos < size) {
        ssize_t w = write(fd, ((const char*) buf_p)+pos, size-pos);
        if (w <= 0) {
            if (errno == EINTR) continue;
            if (errno == EPIPE) {
                ALOG_WARN("EPIPE (other end disconneted) when writing to fd %d\n", fd);
                return;
            }
            ALOG_ERR("write to fd %d failed, error %d, most likely this means big problems\n", fd, errno);
            return;
        }
        pos += (size_t) w;
    }
}


void srv_send_reply(msg_base_t* msg_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    write_all(msg_p->reply_fd, msg_p, msg_p->size);
}

void srv_send_reply_with_payload(msg_base_t* msg_p, const char* payload, unsigned int payloadsize)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    unsigned int original_size = msg_p->size;
    msg_p->size += payloadsize;
    write_all(msg_p->reply_fd, msg_p, original_size);
    write_all(msg_p->reply_fd, payload, payloadsize);
}

void srv_send_reply_and_fd(msg_base_t* msg_p, int fd)
{
    srv_send_reply(msg_p);
    util_fd_send_to_socket(msg_p->reply_fd, fd);
}



void srv_send_reply_msg(msg_base_t* msg_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    write_all(msg_p->client_fd, msg_p, msg_p->size);
}

static void send_simple_reply(int fd, ste_adm_res_t error_msg)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    msg_base_t msg; // TODO: Initiate
    msg.magic  = STE_ADM_MAGIC;
    msg.result = error_msg;
    msg.size   = sizeof(msg_base_t);
    write_all(fd, &msg, sizeof(msg_base_t));
}

// Return true when complete message is received
// IMP: Keep a pre-allocated recv_buf. Almost all time we just
// use one.
int recv_msg_parts(struct client_info* client_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    // Receive only header first to determine total payload size
    msg_base_t tmp_msg_base;
    ssize_t tmp_size = read(client_p->fd,
                 (char*) &tmp_msg_base,
                 sizeof(tmp_msg_base));

    //ALOG_INFO("read msg_base_t from socket. fd %d msg size %d. Result = %d, errno = %d\n", client_p->fd, tmp_msg_base.size, tmp_size, errno);
    if (tmp_size <= 0) {
        ALOG_ERR("read on socket fd %d failed. Result = %d, errno = %d\n", client_p->fd, tmp_size, errno);
        send_simple_reply(client_p->fd, STE_ADM_RES_ERR_MSG_IO);
        return 0;
    }

    if (!client_p->recv_buf) {
        client_p->recv_buf = malloc(tmp_msg_base.size);
        client_p->recv_buf_size = tmp_msg_base.size;
    } else {
        //ALOG_INFO("client_p->recv_buf already allocated!! size=%d\n", client_p->recv_buf_size);
        if (tmp_msg_base.size > client_p->recv_buf_size)
        {
            ALOG_INFO("Reallocating client_p->recv_buf from: %d to: %d  bytes\n", client_p->recv_buf_size, tmp_msg_base.size);
            client_p->recv_buf = realloc(client_p->recv_buf, tmp_msg_base.size);
            client_p->recv_buf_size = tmp_msg_base.size;
        }
    }

    if (!client_p->recv_buf) {
        ALOG_ERR("malloc() failed for %d bytes\n", tmp_msg_base.size);
        client_p->recv_buf_size = 0;
        send_simple_reply(client_p->fd, STE_ADM_RES_ERR_MALLOC);
        return 0;
    }

    memcpy(client_p->recv_buf, &tmp_msg_base, sizeof(tmp_msg_base));
    client_p->recv_buf_bytes_valid = sizeof(tmp_msg_base);

    // Read rest of message from socket
    while(client_p->recv_buf_bytes_valid < tmp_msg_base.size)
    {
        ssize_t r = read(client_p->fd,
                        (char*) client_p->recv_buf + client_p->recv_buf_bytes_valid,
                        tmp_msg_base.size - client_p->recv_buf_bytes_valid);

        if (r <= 0) {
            ALOG_ERR("read on socket fd %d failed. Result = %d, errno = %d\n", client_p->fd, r, errno);
            send_simple_reply(client_p->fd, STE_ADM_RES_ERR_MSG_IO);
            return 0;
        }
        client_p->recv_buf_bytes_valid += (size_t) r;
    }

    if (client_p->recv_buf_bytes_valid >= sizeof(msg_base_t)) {
        msg_base_t* msg_p = (msg_base_t*) client_p->recv_buf;
        if (msg_p->size == client_p->recv_buf_bytes_valid) {
            // ok, message received
            return 1;
        } else if (msg_p->size < client_p->recv_buf_bytes_valid) {
            // received too much data. Socket abuse.
            ALOG_ERR("Expected %d bytes, received %d bytes. Socket abuse?\n", msg_p->size, client_p->recv_buf_bytes_valid);
            send_simple_reply(client_p->fd, STE_ADM_RES_ERR_MSG_IO);
            return 0;
        }

        if (client_p->recv_buf_bytes_valid == STE_ADM_RECV_BUF_SIZE) {
            // can't receive more data, but whole message not received.
            // Bad.
            ALOG_ERR("Received maximum (%d) bytes of data, but whole msg (%d) not received\n", client_p->recv_buf_bytes_valid, msg_p->size);
            send_simple_reply(client_p->fd, STE_ADM_RES_ERR_MSG_IO);
            return 0;
        }

        ALOG_INFO_VERBOSE("Received %d bytes of %d\n", client_p->recv_buf_bytes_valid, msg_p->size);
    } else {
        ALOG_INFO_VERBOSE("Received %d bytes (not yet whole header)\n", client_p->recv_buf_bytes_valid);
    }

    // All data not yet received
    return 0;
}



static int prepare_main_loop()
{
    // bionic does not have epoll_create1
    g_epoll_fd = epoll_create(16);
    if (g_epoll_fd == -1) {
        ALOG_ERR("H: FATAL: high_prio_thread_loop - epoll_create1 failed, errno=%d\n", errno);
        return -1;
    }

    // Add listener socket
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev)); // Make valgrind quiet
    ev.events   = EPOLLIN;

    // Addr of local variable taken, but only used while function is executing
    ev.data.ptr = &g_listen_sock;

    if (epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_listen_sock, &ev)) {
        ALOG_ERR("H: FATAL: prepare_main_loop - epoll_ctl of listen_sock failed, errno=%d\n", errno);
        close(g_epoll_fd);
        return -1;
    }


    // Add fd for the pipe from IL callbacks
    ev.data.ptr = &g_il_callback_pipe[0];
    if (epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_il_callback_pipe[0], &ev)) {
        ALOG_ERR("H: FATAL: prepare_main_loop - epoll_ctl ADD of g_il_callback_pipe[0] failed, errno=%d\n", errno);
        close(g_epoll_fd);
        return -1;
    }

    return 0;
}

static void dispatch(struct client_info* client_p, msg_base_t* msg_p);



void assert_no_queued_requests()
{
#ifdef ADM_DEBUG
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int i;
    for (i=0 ; i < ADM_MAX_CLIENTS ; i++) {
        if (g_client_tbl[i].fd != -1) {
            ADM_ASSERT(!g_client_tbl[i].close_requested);
            ADM_ASSERT(!g_client_tbl[i].request_queued);
        }
    }
#endif
}

//
// Returns 0 if nothing was done (no state changed), != 0 otherwise.
// In that case, the function expects to be called again before
// ADM waits for external events.
//
static int process_queued_requests()
{
    int i;

    if (!g_any_requests_queued) {
        assert_no_queued_requests();
        return 0;
    }

    int queued_request_found = 0;
    for (i=0 ; i < ADM_MAX_CLIENTS ; i++) {
        if (g_client_tbl[i].fd != -1) {
            if (g_client_tbl[i].close_requested) {
                queued_request_found = 1;
                g_client_tbl[i].request_queued = 0; // EPOLLHUP (unless socket abuse); ignore, convert to close

                ALOG_INFO_VERBOSE("Close requested for session with fd=%d\n", g_client_tbl[i].fd);
                if (g_client_tbl[i].client_state_p) {
                    int must_be_serialized = g_client_tbl[i].client_state_p->dtor_must_be_serialized;
                    ALOG_INFO_VERBOSE("Client with fd=%d has state, will need to call dtor\n", g_client_tbl[i].fd);
                    ADM_ASSERT(g_client_tbl[i].client_state_p->dtor_fp);
                    ADM_ASSERT(g_client_tbl[i].disconnect_requested == 0);
                    if (must_be_serialized) {
                        if (!try_begin_serialized_request()) {
                            ALOG_INFO_VERBOSE("Close for fd %d could not be carried out yet; queued..\n", g_client_tbl[i].fd);
                            continue;
                        }
                    }

                    ALOG_INFO("Closing session with fd=%d\n", g_client_tbl[i].fd);
                    g_client_tbl[i].close_requested = 0;

                    struct srv_session_s* client_state_p = g_client_tbl[i].client_state_p;
                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    // NOTE: The dtor function might convert to worker thread, causing
                    // process_queued_requests to be called again directly, from another thread!
                    // State must be prepared for that at this point!!
                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    client_state_p->dtor_fp(client_state_p);

                    if (must_be_serialized) {
                        end_serialized_request();
                    }
                }
                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
                ALOG_INFO("Closing DONE, fd = %d\n", g_client_tbl[i].fd);
                g_client_tbl[i].close_requested      = 0;
                g_client_tbl[i].disconnect_requested = 1;
                g_disconnect_request_rescan          = 1;

                return 1;
            }
            if (g_client_tbl[i].request_queued) {
                queued_request_found = 1;
                g_client_tbl[i].request_queued = 0;
                msg_base_t* msg_p = (msg_base_t*) g_client_tbl[i].recv_buf;

                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
                dispatch(&g_client_tbl[i], msg_p);
                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

                if (g_client_tbl[i].request_queued) {
                    ALOG_INFO_VERBOSE("Request for fd %d could still not be carried out; still queued...\n", g_client_tbl[i].fd);
                } else {
                    g_client_tbl[i].recv_buf_bytes_valid = 0;
                    return 1;
                }
            }
        }
    }

    g_any_requests_queued = queued_request_found;
    return 0;
}

static void disconnect_clients()
{
    if (!g_disconnect_request_rescan)
      return;
    g_disconnect_request_rescan = 0;

    int i;
    for (i=0 ; i < ADM_MAX_CLIENTS ; i++) {
        if (g_client_tbl[i].fd != -1) {
            if (g_client_tbl[i].disconnect_requested) {
                ALOG_INFO("H: No more epoll():ing on %d\n", g_client_tbl[i].fd);
                if (epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, g_client_tbl[i].fd, NULL) && errno != ENOENT) {
                    // We might already have deleted it when we got EPOLLIN
                    ALOG_ERR("H: EPOLL_CTL_DEL failed for client %d; keeping stuck client errno=%d\n", g_client_tbl[i].fd, errno);
                } else {
                    ALOG_INFO("H: Closing fd %d\n", g_client_tbl[i].fd);
                    close(g_client_tbl[i].fd);
                    free_client(&g_client_tbl[i]);
                }
            }
        }
    }
}


static void* main_loop(void* param)
{
#ifndef ADM_DBG_X86
    if (setpriority(PRIO_PROCESS, gettid(), -10)) {
        ALOG_ERR("setpriority(PRIO_PROCESS, gettid(), -10) failed\n");
    }
#endif

    (void) param;
    while(1) {
        // The g_adm_threads_sem semaphore makes sure we just have one thread
        // running in a non-worker thread region.
        //
        // When a thread leaves the worker-thread region, it needs to become
        // a non-worker thread.
        //
        // This semaphore limits the number of non-worker threads to one.
        // It starts out with the value one.
        // Each time a normal thread becomes a worker thread, it is increased by one, and
        // each time a worker thread becomes a normal thread, it id decremented by one.
        // To allow a worker thread to decrement the semaphore in order to become a normal
        // thread, the inactive normal threads must be outside the semaphore (also,
        // they must not be stuck in epoll). Thus, the semaphore can not be used to synchronize
        // access to epoll_wait. However, we just want to allow one thread to be inside epoll.
        // A separate mutex is used for this.
        //
        // Common scenarios (thread A and B are equals):
        //  ADM idle:
        //  - Thread A owns g_epoll_mutex and is waiting inside epoll_wait.
        //  - g_adm_threads_sem = 1.
        //  - Thread B is waiting at the mutex_lock below, on g_epoll_mutex.
        //
        //  ADM just received request (before dispatch):
        //  - Thread A owns g_epoll_mutex and is just outside epoll_wait, inside
        //    ste_adm_srv.c.
        //  - Thread B is waiting at the mutex_lock below, on g_epoll_mutex.
        //  - g_adm_threads_sem = 0.
        //
        //  ADM executing standard request (no worker thread), no new request pending.
        //  - Thread A is executing standard-prio code in the ADM.
        //  - g_adm_threads_sem = 0.
        //  - Nobody owns g_epoll_mutex. Thread B can take it and get stuck in epoll_wait.
        //
        //  ADM executing standard request (no worker thread), new request pending.
        //  - Thread A is executing standard-prio code in the ADM.
        //  - g_adm_threads_sem = 0.
        //  - Thread B owns g_epoll_mutex, and is waiting on g_adm_threads_sem right
        //  - after epoll_wait (and will do so until the standard-prio code has returned
        //    to ste_adm_srv.c).
        //
        //  ADM is executing worker thread request, no new request pending.
        //  - Thread A is executing as a worker thread
        //  - g_adm_threads_sem = 1.
        //  - Nobody owns g_epoll_mutex. Thread B can take it and get stuck in epoll_wait.
        //
        //  ADM is executing worker thread request, new request pending.
        //  - Thread A is executing as a worker thread
        //  - g_adm_threads_sem = 0.
        //  - Thread B owns g_epoll_mutex, and is executing code in ste_adm_srv.c to
        //    prepare the new request.
        //
        //  ADM is executing both worker thread and a standard request.
        //  - Thread A is executing as a worker thread
        //  - Thread B is executing standard request
        //  - g_adm_threads_sem = 0.
        //  - Nobody owns g_epoll_mutex.
        //
        //  ADM is executing a standard request. The worker thread wants to become a standard thread.
        //  - Thread A executed as a worker thread, now waits on g_adm_threads_sem
        //  - g_adm_threads_sem = 0.
        //  - Thread B is executing standard request. When it is finished, it will increment
        //    g_adm_threads_sem releasing thread A, which thus becomes a normal thread.
        //

        sem_wait(&g_adm_threads_sem);

        // Keep looping as long as either process_queued_requests() or
        // dispatch_il_messages() changed some state; the old action might
        // have made it possible to carry out a queued action (e.g. vc is
        // now finished setting up the graph, it is possible to proceed with
        // opening dictaphone device).
        int did_something;
        do {
            did_something = 0;

            while (process_queued_requests()) {
                did_something = 1;
            }

            while (dispatch_il_messages()) {
                did_something = 1;
            }
        } while (did_something);
        sem_post(&g_adm_threads_sem);


        // We might get stuck here waiting for the mutex to be released
        // so we can remove a file descriptor that is going to be closed.
        // Completely OK, since we shouldn't remove it while epoll_wait is
        // holding the other thread.
        // We don't need to re-scan the state (as above for process_queued_requests() and
        // dispatch_il_messages() after disconnecting; it just closes file descriptors,
        // which nobody could be waiting for.

        while (pthread_mutex_trylock(&g_epoll_mutex)) {
            char dummy[8];
            ALOG_INFO("ADM EPOLL MUTEX: waiting for wakeup\n");
            read(g_adm_wakeup_event_fd, dummy, 8);
            ALOG_INFO("ADM EPOLL MUTEX: DONE waiting for wakeup\n");
        }

        sem_wait(&g_adm_threads_sem);
        disconnect_clients();
        sem_post(&g_adm_threads_sem);
        pthread_mutex_unlock(&g_epoll_mutex);


        // The g_epoll_mutex mutex is used to avoid allowing more than one thread to enter epoll_wait.
        // The rest of this function is synchronized via the semaphore; it only allows one thread
        // here at a time. The other thread is inside a worker thread region, OR trying to take
        // the epoll mutex above. We can't protect epoll_wait with the semaphore since a thread might
        // get stuck in epoll_wait() while the other thread is executing as a worker thread; in that case,
        // the worker thread can not change back to non-worker state. The criteria is that any thread
        // holding the g_adm_threads_sem semaphore must not block.
        //
        // g_epoll_mutex is just used for this purpose. The other data structures inside ste_adm_srv
        // are protected via the semaphore. The g_epoll_mutex essentially protects the
        // 'struct epoll_event event' structure.


        struct epoll_event event;
        pthread_mutex_lock(&g_epoll_mutex);

        if (g_dbg_srv_shutdown) {
            pthread_mutex_unlock(&g_epoll_mutex);
            ALOG_INFO("DBG Shutdown: Thread %X leaving main loop\n", pthread_self());

            // In valgrind, read() does not return if this is just closed? Strange.
            const unsigned long long val = 1;
            write(g_adm_wakeup_event_fd, &val, sizeof(val));

            return NULL;
        }

        ALOG_INFO_VERBOSE("****** waiting on epoll - %d clients, %d worker threads ******\n",dbg_num_clients(), g_dbg_num_worker_threads);

        int num_events;
        while (1) {
            num_events = epoll_wait(g_epoll_fd, &event, 1, -1);
            if ( !(num_events == -1 && errno == EINTR)) {
                break;
            }
            // This happens a lot in gdb.. annoying, avoid prints due to it,,
            // ALOG_INFO("EPOLL returned EINTR, retrying..\n");
        }


        sem_wait(&g_adm_threads_sem);
        if (num_events == -1) {
            if (errno == EAGAIN) {
                ALOG_WARN("epoll_failed, errno = EAGAIN. This error code is not documented in epoll_wait manual?! Retrying\n");
                usleep(100);
                pthread_mutex_unlock(&g_epoll_mutex);
                sem_post(&g_adm_threads_sem);
                continue;
            }
            ALOG_ERR("epoll_failed, errno = '%s' = %d\n", strerror(errno), errno);

            // Oops. epoll_wait failed in the main loop.
            // Not much to do.
            usleep(200 * 1000);
            pthread_mutex_unlock(&g_epoll_mutex);
            sem_post(&g_adm_threads_sem);
            continue;
        }
        ALOG_INFO_VERBOSE("H: epoll_wait returned %d events\n", num_events);


        int fd = *((int*) event.data.ptr);
        if (fd == g_listen_sock) {
            dbg_set_logs_from_properties();

            // Try to accept the client connection. We need to always accept
            // and then close connection if we have an overflow of clients.
            int new_fd = accept(fd, NULL, NULL);
            if (new_fd == -1) {
                ALOG_ERR("H: accept failed, errno=%d\n", errno);
                pthread_mutex_unlock(&g_epoll_mutex);
                sem_post(&g_adm_threads_sem);
                continue;
            }

            // TODO: Set new_fd to nonblocking mode
            struct client_info* client_p = alloc_client(new_fd);
            if (!client_p) {
                ALOG_ERR("H: Too many clients; client with fd %d dropped\n", new_fd);
                send_simple_reply(new_fd, STE_ADM_RES_ERR_TOO_MANY_CLIENTS);
                close(new_fd);
                pthread_mutex_unlock(&g_epoll_mutex);
                sem_post(&g_adm_threads_sem);
                continue;
            }

            // Add new socket to epoll set
            struct epoll_event ev;
            memset(&ev, 0, sizeof(ev)); // Make Valgrind happy
            ev.data.ptr = client_p;
            ev.events   = EPOLLIN | EPOLLHUP;
            if (epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, new_fd, &ev)) {
                ALOG_ERR("H: Failed to add socket %d to epoll set, errno=%d\n", new_fd, errno);
                free_client(client_p);
                send_simple_reply(new_fd, STE_ADM_RES_ERR_MSG_IO);
                close(new_fd);
            } else {
                ALOG_INFO("H: Added new client connection. fd=%d, client=%X\n", new_fd, client_p);
            }

            pthread_mutex_unlock(&g_epoll_mutex);
            sem_post(&g_adm_threads_sem);
            continue;
        }

        // Handle data either from client, or from low-prio thread. Most of
        // the logic here is for handling closedown as well as socket abuse...

        struct client_info* client_p = NULL;

        if (fd == g_il_callback_pipe[0]) {
            srv_read_and_drop_il_messages(NULL, 0, 0, 0);

            // Done with request. Post sem to signal that we are idle.
            // Messages will be processed at top of loop
            pthread_mutex_unlock(&g_epoll_mutex);
            sem_post(&g_adm_threads_sem);
            continue;
        }

        // The epoll event is associated with a client fd
        client_p = (struct client_info*) event.data.ptr;
        ALOG_INFO_VERBOSE("H: epoll event on fd %d\n", client_p->fd);

        int got_msg = 0;
        if (event.events & EPOLLHUP) {
            // Client hung up (process crashed?). We need to close all associated resources.
            ALOG_INFO("H: EPOLLHUP on fd %d\n", client_p->fd);

            // ouch. The low-prio thread could be busy executing a command.
            // Queue the close. Do not close the FD yet. Unmask EPOLLHUP from
            // epoll.
            // TODO: Might get next EPOLL_CTL_DEL to fail? Just unmask instead?
            // TODO: Always do this
            if (epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, client_p->fd, NULL)) {
                // TODO
                ALOG_ERR("H: EPOLL_CTL_DEL failed for client %d\n", client_p->fd);
            }

            client_p->close_requested = 1;
            g_any_requests_queued     = 1;
        } else if (event.events & EPOLLIN) {
            // We got data from a client.
            ALOG_INFO_VERBOSE("H: Received (some) data from client with fd %d\n", client_p->fd);

            if (client_p->busy) {
                ALOG_ERR("Client socket (fd=%d) received data while processing request, socket abuse? Bad.\n", client_p->fd);

                // TODO: We must get rid of the data, otherwise epoll will re-trig :( Close connection?
                pthread_mutex_unlock(&g_epoll_mutex);
                sem_post(&g_adm_threads_sem);
                continue;
            }

            got_msg = 1;
        }


        ADM_ASSERT(client_p);
        if (got_msg) {
            int all_data_received = recv_msg_parts(client_p);
            // We are done with the event. Let's release the mutex here
            // to makes things easier. We are still protected by g_adm_threads_sem.
            // Note that epoll_wait might run as soon as g_epoll_mutex is released!
            // All reads that the previous event were related to must be completed!
            pthread_mutex_unlock(&g_epoll_mutex);

            if (all_data_received) {
                msg_base_t* msg_p = (msg_base_t*) client_p->recv_buf;

                msg_p->reply_fd = client_p->fd;
                ALOG_INFO_VERBOSE("Dispatching message ID = %d\n", msg_p->cmd_id);

                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

                client_p->request_queued = 1;
                g_any_requests_queued    = 1;
                client_p->busy           = 1;
            }
        } else {
            // We are done with the event. Let's release the mutex here
            // to makes things easier. We are still protected by g_adm_threads_sem.
            // Note that epoll_wait might run as soon as g_epoll_mutex is released!
            // All reads that the previous event were related to must be completed!
            pthread_mutex_unlock(&g_epoll_mutex);
        }

        // Request done
        sem_post(&g_adm_threads_sem);
    }
}


static int srv_setup_listener_sck(void)
{
    /* Socket address struct needed by system calls */
    struct sockaddr_un local_sockaddr_un;
    int listener_sck_fd = -1;
    ADM_ASSERT(strlen(STE_ADM_SOCK_PATH) <=
       sizeof(local_sockaddr_un.sun_path));

    listener_sck_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (listener_sck_fd <= -1) {
        ALOG_ERR("socket() %s\n", strerror(errno));
        return -1;
    }

    ALOG_INFO("Successfully created socket,"
             "bind to Unix Domain address.\n");

    /* Setup local sock_address structure for binding to STE_ADM_SOCK_PATH */
    /* The socket exists in the AF_UNIX namespace but has
       no address assigned to it */
    memset(&local_sockaddr_un, 0, sizeof(local_sockaddr_un));
    local_sockaddr_un.sun_family = AF_UNIX;
    strncpy(local_sockaddr_un.sun_path, STE_ADM_SOCK_PATH, sizeof(local_sockaddr_un.sun_path));

    /* Unlink any previous socket */
    unlink(STE_ADM_SOCK_PATH);

    /* Now bind the socket to the file system, cast the second argument to the
       more general struct sockaddr */

    if (bind(listener_sck_fd,
         (struct sockaddr *) &local_sockaddr_un,
         sizeof(local_sockaddr_un)) < 0) {
        ALOG_ERR("bind() %s\n", strerror(errno));
        return -1;
    }

    // TODO: Remove this race
    ALOG_INFO("Successfully bound socket to %s.\n", STE_ADM_SOCK_PATH);
    if (chown(STE_ADM_SOCK_PATH, ADM_SOCKET_UID, ADM_SOCKET_GID)) {
        ALOG_ERR("chown() %s\n", strerror(errno));
    }
    if (chmod(STE_ADM_SOCK_PATH, ADM_SOCKET_MODE)) {
        ALOG_ERR("chmod() %s\n", strerror(errno));
    }

    ALOG_INFO("Successfully bound socket to %s.\n", STE_ADM_SOCK_PATH);

    /* Start listening for connections, mark the socket as passive
       (used to accept connections) */
    if (listen(listener_sck_fd, STE_ADM_LISTEN_BACKLOG)) {
        ALOG_ERR("listen() %s\n", strerror(errno));
        return -1;
    }

    ALOG_INFO("Server up and running, listener_sck_fd (%d) MAX NO"
             "Clients: %u.\n",
             listener_sck_fd, STE_ADM_LISTEN_BACKLOG);

    return listener_sck_fd;
}


#include "ste_adm_util.h"
adm_sem_t g_adm_event_sem;

void srv_mainloop()
{
    pthread_t thread[2];

    g_srv_dbg_init_done = 1;
    pthread_create(&thread[0], 0, main_loop, 0);
    pthread_create(&thread[1], 0, main_loop, 0);

    pthread_join(thread[1], NULL);
    pthread_join(thread[0], NULL);

    ALOG_INFO("All threads have left main_loop; shutting down\n");
    g_srv_dbg_init_done = 0; // TODO: Rename. (-> worker_thread_concept_active or something)
    srv_read_and_drop_il_messages(NULL, OMX_ALL, 1, 0);

    pthread_mutex_destroy(&g_srv_il_mutex);
    pthread_mutex_destroy(&g_epoll_mutex);
    close(g_listen_sock);
    close(g_epoll_fd);
    adm_sem_destroy(&g_adm_event_sem);
}



#include "BSD_list.h"
// TODO: Move to IL tool? Only get calls and pipe fd's from server?
typedef struct io_callback_msg_s
{
    il_comp_t comp;

    ste_adm_io_cb_type_t type;


    // Valid if type == CB_TYPE_BUFFER_EMPTIED
    // or if type == CB_TYPE_BUFFER_FILLED
    OMX_BUFFERHEADERTYPE* buffer_done_p;
    OMX_U32 port;

    // The following members are valid
    // if type == CB_TYPE_IL_EVENT
    OMX_EVENTTYPE eEvent;
    OMX_U32 Data1;
    OMX_U32 Data2;
    OMX_PTR pEventData;

    // For use by the receiver, to link messages together
    LIST_ENTRY(io_callback_msg_s) list_entry;
} io_callback_msg_t;

void srv_send_il_buffer_msg(il_comp_t comp, OMX_BUFFERHEADERTYPE* buffer_done_p, ste_adm_io_cb_type_t type)
{
    ADM_ASSERT(comp != NULL);
    ADM_ASSERT(buffer_done_p != NULL);
    ADM_ASSERT(type == CB_TYPE_BUFFER_FILLED || type == CB_TYPE_BUFFER_EMPTIED);


    io_callback_msg_t* msg_p;
    do
    {
        msg_p = malloc(sizeof(io_callback_msg_t));
        if (msg_p == NULL) {
            ALOG_ERR("Failed to allocate memory for interthread message, sleeping & retrying..\n");
            usleep(1000*200);
        }
    } while(msg_p == NULL);

    msg_p->type          = type;
    msg_p->comp          = comp;
    msg_p->buffer_done_p = buffer_done_p;

    if (type == CB_TYPE_BUFFER_FILLED) {
        msg_p->port = buffer_done_p->nOutputPortIndex;
    } else if (type == CB_TYPE_BUFFER_EMPTIED) {
        msg_p->port = buffer_done_p->nInputPortIndex;
    }

    int result = write(g_il_callback_pipe[1], &msg_p, sizeof(msg_p));
    if (result < 0) {
        ALOG_ERR("send failed, error %s\n", strerror(errno));
    }
}

void srv_send_il_event_msg(il_comp_t comp, OMX_EVENTTYPE eEvent,
OMX_U32 Data1, OMX_U32 Data2, OMX_PTR pEventData)
{
    ADM_ASSERT(comp != NULL);
    io_callback_msg_t* msg_p;
    do
    {
        msg_p = malloc(sizeof(io_callback_msg_t));
        if (msg_p == NULL) {
            ALOG_ERR("Failed to allocate memory for interthread message, sleeping & retrying..\n");
            usleep(1000*200);
        }
    } while(msg_p == NULL);

    msg_p->type       = CB_TYPE_IL_EVENT;
    msg_p->comp       = comp;
    msg_p->eEvent     = eEvent;
    msg_p->Data1      = Data1;
    msg_p->Data2      = Data2;
    msg_p->pEventData = pEventData;


    int result = write(g_il_callback_pipe[1], &msg_p, sizeof(msg_p));
    if (result < 0) {
        ALOG_ERR("send failed, error %s\n", strerror(errno));
    }
}



LIST_HEAD(il_msg_queue_s, io_callback_msg_s);
static struct il_msg_queue_s g_il_msg_queue;

// Might get called from IL-tool after a component has been deleted, to
// read all messages of the removed component, so that they can later
// be dropped by ste_adm_drop_il_io_messages
//
// Can be called both from worker thread and
// normal thread.
//
void srv_read_and_drop_il_messages(il_comp_t comp, OMX_U32 port, int drop_all, int release_component)
{
    pthread_mutex_lock(&g_srv_il_mutex);


    if (release_component) {
        ADM_ASSERT(comp != NULL);
        il_tool_log_FreeHandle(comp); // TODO: ouch
    }

    // Find last element in queue
    io_callback_msg_t* cur;
    io_callback_msg_t* last = NULL;
    LIST_FOREACH(cur, &g_il_msg_queue, list_entry) {
        if (cur != NULL) {
            last = cur;
        }
    }

    while(1) {
        io_callback_msg_t* msg_p;
        int retval = read(g_il_callback_pipe[0], &msg_p, sizeof(msg_p));
        if (retval == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            break;
        }

        if (last != NULL) {
            LIST_INSERT_AFTER(last, msg_p, list_entry);
        }
        else {
            LIST_INSERT_HEAD(&g_il_msg_queue, msg_p, list_entry);
        }
        last = msg_p;
    }

    // Now drop unwanted messages (typically, removed component)
    io_callback_msg_t* temp;
    LIST_FOREACH_SAFE(cur, &g_il_msg_queue, list_entry, temp) {
        int drop = 0;
        if (drop_all) {
            drop = 1;
        }

        if (cur->comp == comp) {
            if (cur->type == CB_TYPE_BUFFER_EMPTIED || cur->type == CB_TYPE_BUFFER_FILLED) {
                if (cur->port == port || port == OMX_ALL) {
                    drop = 1;
                }
            }
            if (cur->type == CB_TYPE_IL_EVENT) {
                drop = 1;
            }
        }

        if (drop) {
            if (cur->type == CB_TYPE_IL_EVENT) {
                ALOG_INFO("Dropped IL event msg from comp %X\n", cur->comp);
            } else {
                ALOG_INFO("Dropped buf msg of type %d to %X.%d\n", cur->type, cur->comp, cur->port);
            }
            LIST_REMOVE(cur, list_entry);
            memset(cur, 0xCE, sizeof(io_callback_msg_t));
            free(cur);
        }
    }

    pthread_mutex_unlock(&g_srv_il_mutex);
}



//
// Returns 0 if nothing was done (no state changed), != 0 otherwise.
// In that case, the function expects to be called again before
// ADM waits for external events.
//
static int dispatch_il_messages()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    // To protect the linked list, we take the lock, go through the linked list
    // until we find a message to remove, and remove it. Every time the lock
    // has been released, we need to re-scan the linked list from the
    // beginning.
    io_callback_msg_t* msg_p;
    io_callback_msg_t* temp;

    pthread_mutex_lock(&g_srv_il_mutex);
    LIST_FOREACH_SAFE(msg_p, &g_il_msg_queue, list_entry, temp) {
        int is_serialized_event = 0;
        if (msg_p->type == CB_TYPE_IL_EVENT) {
            if (ste_adm_omx_tool_handle_event_must_be_serialized(msg_p->comp)) {
                is_serialized_event = 1;
            }
        }

        int skip = 0;
        if (is_serialized_event) {
            if (!try_begin_serialized_request()) {
                ALOG_INFO_VERBOSE("Event on comp %X must be serialized; skipping for now\n", msg_p->comp);
                skip = 1;
            }
        }

        if (!skip) {
            LIST_REMOVE(msg_p, list_entry);
            pthread_mutex_unlock(&g_srv_il_mutex);

            if (msg_p->type == CB_TYPE_BUFFER_FILLED) {
                ALOG_INFO_VERBOSE("IL dispatch: Buffer filled: %X.%d\n", msg_p->comp, msg_p->port);
                il_tool_io_transfer_done(msg_p->comp, msg_p->port, msg_p->buffer_done_p);
                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
            } else if (msg_p->type == CB_TYPE_BUFFER_EMPTIED) {
                ALOG_INFO_VERBOSE("IL dispatch: Buffer emptied: %X.%d\n", msg_p->comp, msg_p->port);
                il_tool_io_transfer_done(msg_p->comp, msg_p->port, msg_p->buffer_done_p);
                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
            } else {
                ADM_ASSERT(msg_p->type == CB_TYPE_IL_EVENT);
                ALOG_INFO_VERBOSE("IL dispatch: Event\n");
                ste_adm_omx_tool_handle_event(msg_p->comp, msg_p->eEvent, msg_p->Data1, msg_p->Data2, msg_p->pEventData);
                SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
            }

            memset(msg_p, 0xCD, sizeof(*msg_p));
            free(msg_p);
            if (is_serialized_event) {
                end_serialized_request();
            }

            // Re-scan the list from the beginning, to be sure it is not messed
            // up by any other thread (not sure if the list can handle threading).
            return 1;
        }
    }

    pthread_mutex_unlock(&g_srv_il_mutex);
    return 0;
}







//
// msg_p buffer owned by caller.
//
static void dispatch(struct client_info* client_p, msg_base_t* msg_p)
{
    // Initiate result to OK
    msg_p->result = STE_ADM_RES_OK;

    // Save fd of sender in message. Critically needed by high prio thread,
    // but generally convenient to be able to send reply from anywhere.
    msg_p->client_fd = client_p->fd;

    // Command handlers are allowed to modify the contents
    // of this pointer, to associate their own state with the
    // client connection. It may also be left untouched (NULL).
    srv_session_t** client_pp = &client_p->client_state_p;


    ADM_ASSERT(client_p->busy);

    switch (msg_p->cmd_id) {
    case MSG_ID_OPEN_DEVICE:
        dbg_set_logs_from_properties();
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO_VERBOSE("OpenDevice from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        topapi_open((msg_device_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_CLOSE_DEVICE:
        dbg_set_logs_from_properties();
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO_VERBOSE("CloseDevice from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        topapi_close((msg_device_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_DATA_FEED:
        topapi_feed((msg_data_t*) msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_DATA_REQ:
        topapi_datareq((msg_data_t*) msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_UPSTREAM_MUTE:
        csapi_set_upstream_mute((msg_cscall_mute_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_CSCALL_UPSTREAM_MUTE:
        csapi_get_upstream_mute((msg_cscall_mute_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_DOWNSTREAM_MUTE:
        csapi_set_downstream_mute((msg_cscall_mute_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_CSCALL_DOWNSTREAM_MUTE:
        csapi_get_downstream_mute((msg_cscall_mute_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_UPSTREAM_VOLUME:
        csapi_set_upstream_volume((msg_cscall_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_CSCALL_UPSTREAM_VOLUME:
        csapi_get_upstream_volume((msg_cscall_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_DOWNSTREAM_VOLUME:
        csapi_set_downstream_volume((msg_cscall_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_INIT_CSCALL_DOWNSTREAM_VOLUME:
        csapi_init_downstream_volume((msg_cscall_init_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_CSCALL_DOWNSTREAM_VOLUME:
        csapi_get_downstream_volume((msg_cscall_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_DICTAPHONE_MODE:
        csapi_dict_set_mode((msg_cscall_dict_mode_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_DICTAPHONE_MUTE:
        csapi_dict_set_mute((msg_cscall_dict_mute_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_CSCALL_DICTAPHONE_MUTE:
        csapi_dict_get_mute((msg_cscall_dict_mute_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_APP_VOLUME:
        devapi_set_app_vol((msg_app_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_APP_VOLUME:
        devapi_get_app_vol((msg_app_volume_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_SINK_LATENCY:
        devapi_get_sink_latency((msg_sink_latency_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_PCM_PROBE:
        devapi_set_pcm_probe((msg_set_pcm_probe_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_DEVICES:
        dbg_set_logs_from_properties();
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            client_p->request_queued = 1;
            ALOG_INFO_VERBOSE("SetCSCallDevices could not be started now; queued\n");
            return;
        }
        csapi_set_devices((msg_cscall_devices_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_TTY:
        if (!try_begin_serialized_request()) {
            client_p->request_queued = 1;
            ALOG_INFO_VERBOSE("SetCSCallTTY could not be started now; queued\n");
            return;
        }
        csapi_set_tty((msg_cscall_tty_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_GET_CSCALL_TTY:
        csapi_get_tty((msg_cscall_tty_t*) msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_GET_MODEM_TYPE:
        csapi_get_modem_type((msg_get_modem_type_t*) msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_VOIP:
        if (!try_begin_serialized_request()) {
            client_p->request_queued = 1;
            ALOG_INFO_VERBOSE("csapi_set_voip could not be started now; queued\n");
            return;
        }
        csapi_set_voip((msg_cscall_voip_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_DBG_SHUTDOWN:
        g_dbg_srv_shutdown = 1;
        client_p->busy = 0;
        client_p->close_requested = 1; // TODO: Always do if no state?
        g_any_requests_queued = 1;
        return;

    case MSG_DBG_DUMP_STATE:
        ALOG_STATUS("ADM state dump requested\n");
        dbg_print_debug_information();
        srv_send_reply(msg_p);
        client_p->busy = 0;
        return;

    case MSG_ID_DRAIN:
        topapi_drain(msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_D2D_CONNNECT:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO_VERBOSE("D2D connect from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        d2d_connect((msg_d2d_connect_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_D2D_DISCONNECT:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO_VERBOSE("D2D disconnect from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        d2d_disconnect((msg_d2d_disconnect_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_SET_CSCALL_LOOPBACK:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO_VERBOSE("SetCSCallLoopback from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        csapi_set_loopback((msg_cscall_loopback_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

#ifndef ADM_DISABLE_TONEGEN
    case MSG_ID_START_COMFORT_TONE:
        tgapi_start_tone((msg_start_comfort_tone_t*)msg_p, client_pp);
        client_p->busy = 0;
        return;

    case MSG_ID_STOP_COMFORT_TONE:
        tgapi_stop_tone(msg_p, client_pp);
        client_p->busy = 0;
        return;
#endif

    case MSG_ID_GET_TOPLEVEL_MAP:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("util_api_get_toplevel_map from client %d could not be started now\n",
                      client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        util_api_get_toplevel_map((msg_get_toplevel_map_t*)msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_SET_TOPLEVEL_MAP:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("util_api_set_toplevel_map from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        csapi_remap((msg_set_toplevel_map_t*)msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_GET_TOPLEVEL_DEVICE:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("util_api_get_toplevel_device from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        util_api_get_toplevel_device((msg_get_toplevel_device_t*)msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_GET_LATENCY:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("util_api_get_max_out_latency from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        util_api_get_max_out_latency((msg_get_max_out_latency_t*)msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_SET_EXTERNAL_DELAY:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("util_api_set_external_delay from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        util_api_set_external_delay((msg_set_external_delay_t*)msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_RESCAN_CONFIG:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("rescan config from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        csapi_rescan_config((msg_rescan_config_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_RESCAN_SPEECHPROC:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("rescan speechproc from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        csapi_rescan_speechproc((msg_base_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;
    case MSG_RELOAD_DEVICE_SETTINGS:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("reloaddevice settings from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        devapi_reload_settings((msg_base_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_EXECUTE_SQL:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("execute sql from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        util_api_execute_sql((msg_execute_sql_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_GET_ACTIVE_DEVICES:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("get active devices from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        devapi_get_active_devices((msg_get_active_devices_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    case MSG_ID_REOPEN_DB:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("reopen db from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }

        ALOG_INFO("reopening db\n");

        {
            ste_adm_res_t res = STE_ADM_RES_OK;
            {
                res = adm_db_disconnect();
                if ((res == STE_ADM_RES_OK))
                {
                    res = adm_db_init(); // Reinitialize  DB
                }

            }
            end_serialized_request();
            client_p->busy = 0;
            send_simple_reply(client_p->fd, res);
        }
        return;

    case MSG_ID_SET_TUNING_MODE:
        adm_db_set_tuning_mode(((msg_set_tuning_mode_t*) msg_p)->enabled);
        client_p->busy = 0;
        send_simple_reply(client_p->fd, STE_ADM_RES_OK);
        return;
    case MSG_ID_MODEM_VC_STATE:
        if (!try_begin_serialized_request()) {
            // We just want to execute one lowprio request at a time, to
            // make the code in the ADM simpler. Also, we just have one worker
            // thread available. The request will be postponed.
            ALOG_INFO("get active devices from client %d could not be started now\n", client_p->fd);
            client_p->request_queued = 1;
            return;
        }
        csapi_request_modem_vc_state((msg_modem_vc_state_t*) msg_p, client_pp);
        end_serialized_request();
        client_p->busy = 0;
        return;

    default:
        ALOG_ERR("Received unknown command!\n");
        return;
    }

    return;
}




