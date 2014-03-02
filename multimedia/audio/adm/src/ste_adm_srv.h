/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
* The ste_adm_srv.c file provides a layer between the user API
* (which connects to the ADM using AF_UNIX socket)
* and the ADM implementation. Functionality includes:
*
*  - connecting / disconnecting sockets
*  - session concept for clients
*  - receiving and dispatching messages from clients
*  - serializing potentially dependent requests from different clients
*  - reading and writing data from/to sockets
*  - asynchronous client disconnections (EPOLLHUP)
*  - support for different thread modes (worker thread / normal thread)
*  - multiple threads
*  - queuing and dispatching IL messages
*
*
*  Thread modes
*  ============
*
*  When a request is dispatched from ste_adm_srv.c, it is executing
*  as a normal thread. In that mode, it is allowed to access ADM
*  state without restrictions. A thread that carries out a fast operation,
*  such as queueing a new audio buffer to an IL component, or calling
*  the OMX_SetConfig / OMX_GetConfig functions does all execution in
*  normal mode.
*
*  However, if the thread needs to execute an operation that takes a long
*  time, it needs to switch to worker thread mode during the slow operation.
*  This includes any operation that accesses the SQL database, or that
*  sends a command to OpenMAX IL (using OMX_SendCommand, such as enable
*  port or a request to change state) and then waits for the command to
*  finish.
*
*  The reason is that if the thread does not switch to worker
*  thread mode, the ADM is blocked for other requests during the slow
*  operation. Examples of operations that should not be blocked
*  include transfer of audio data to/from the ADM, and configuration
*  changes such as volume settings. If these were blocked, there
*  would for example be audio interruption while the ADM is
*  setting up a new audio graph.
*
*  As soon as a thread becomes a worker thread, another thread is allowed
*  to execute ADM requests. Thus, this other thread will be able to e.g.
*  transfer IO data while the worker thread is busy setting
*  up a new graph.
*
*  The idea is that a thread in the ADM is allowed to execute as a worker
*  thread when it follows these rules:
*   - it does not access (read or write) any ADM state
*   - the ADM state is consistent before it becomes a worker thread
*   - the ADM state is set up so that other threads will not interfere
*     with the work the worker thread does (e.g. if the worker thread will
*     remove an IL component, it must be locked/removed from ADM state before
*     the thread becomes a worker thread.
*   - IL components may be accessed both by worker threads and the normal
*     thread.
*
*  When a worker thread is finished with the slow part that does not require
*  access to ADM state, it asks to become a normal thread. That call will
*  block a while if another thread is executing inside the ADM as a normal thread
*  (only one normal thread is allowed to execute at a time, since the normal thread
*  is allowed to access global state).
*
*  For example, code to handle opening of a new device might look like this:
*
*    open_device(msg_p)
*    {
*       g_state.lock_devices();
*       copy_of_state = g_state;
*
*       srv_become_worker_thread();
*         slow_work(&copy_of_state)
*       srv_become_normal_thread();
*
*       if (ok) {
*         // If everything went OK, we commit to global state
*         g_state = copy_of_state;
*       }
*
*       g_state.unlock_devices();
*       srv_send_reply(msg_p)
*    }
*
*    transfer_data(msg_p)
*    {
*       send_to_component(msg_p);
*       srv_send_reply(msg_p);
*    }
*
*  While 'slow_work()' is executing in open_device(), transfer_data()
*  can be called from another thread.
*
*  This model helps keep the complexity of the ADM down. With
*
*
*
*  Serialized requests
*  ===================
*
*  For a number of requests, such as open device, there is
*  no reason for the ADM to be able to handle them in parallel.
*  There is no execution time to be gained from it, and the increased complexity
*  in the ADM would be huge. Thus, ste_adm_srv.c hides such attempts
*  from the rest of the ADM.
*
*  However, if a new request arrives to the ADM while another is in
*  progress, it needs to be queued so that it can be carried out by the
*  ADM once it is finished with the ongoing request (it would be unacceptable
*  behavior to return a 'BUSY' error code or similiar to ADM clients).
*
*  To support this, ste_adm_srv.c handles queuing of requests. To control if
*  queuing is desired, the 'serialized request' concept is introduced. To begin a serialized
*  request, currently the try_begin_serialized_request() call is used
*  internally in ste_adm_srv.c right before the requests is dispatched.
*
*  To summarize, at most one request of the type 'serialized' can be executed in
*  the ADM at a time. Non-serialized requests do not have any such limit.
*
*
*
* Session concept
* ===============
*
* ste_adm_srv.c provides a state associated with each active
* session. For example, if a device to device connection is made
* in a certain session, the devices involved needs to be saved
* somewhere so they can be disconnected once the session is closed
* (which may happen at any time and can not be allowed to fail,
* since it nmay happen due to a client crashing).
*
* All requests that are dispatched from ste_adm_srv.c have a
* prototype similiar to this one:
*
*    void devapi_open(msg_device_t* msg_p,
*                     struct srv_session_s** client_pp);
*
* The message data structure depends on the request type.
*
* When a new session is created (directly after a client has connected
* to the ADM), *client_pp is a NULL pointer. If a request
* is finished and *client_pp is NULL, ste_adm_srv interprets it
* as the session has no state. No destructor is run.
*
* To keep the session, the request implementation thus must
* set *client_pp to point to some internal state. Future requests
* from the same session will have *client_pp set to the same value.
*
* The first part of that state must be of type struct srv_session_s.
* Typically, implementation specific state data will follow.
*
* The dtor_fp and dtor_must_be_serialized attributes of
* struct srv_session_s are used by ste_adm_srv.c and must
* be initialized.
*
* dtor_fp points to a destructor function that is run when the
* session is closed, or when the connection is simply
* terminated (EPOLLHUP, e.g. due to client crash).
*
* dtor_must_be_serialized specifies if the dtor can be
* called independent of other requests (if == 0), or if it must
* be serialized together with other serialized requests (if != 0).
*
* api_group is not used by ste_adm_srv.c - it is intended to be
* used by request implementations to make sure calls that belong to
* one request category are not issued on another request category -
* e.g. to prevent Drain() to be called on a device to device connection
* session.
*
*
*/

#ifndef INCLUSION_GUARD_STE_ADM_SRV_H_
#define INCLUSION_GUARD_STE_ADM_SRV_H_

#include "ste_adm_msg.h"
#include "OMX_Core.h"
#include "ste_adm_omx_tool.h" // il_comp_t

// The ADM main loop. Only returns if a fatal error occurs,
// or if the ADM is shut down (mostly used for debugging - to
// be able to see that we have no memory leaks).
void srv_mainloop();

// TODO: Introduce ownership of the buffer. The IO parts
// keeps a copy of the message currently, and
// thus a new, socket-abusing client may overwrite it
// while it is owned by omx_io.
void srv_send_reply(msg_base_t* msg_p);
void srv_send_reply_and_fd(msg_base_t* msg_p, int fd);

void srv_send_reply_with_payload(msg_base_t* msg_p, const char* payload, unsigned int payloadsize);

struct srv_session_s;
typedef void (*srv_session_dtor_fp_t)(struct srv_session_s* param);



// Used to help clients discriminate between different
// API groups - e.g. to prevent a session that is created via OpenDevice
// from using D2D methods.
typedef enum
{
    API_GROUP_DEV,
    API_GROUP_D2D,
    API_GROUP_CSCALL,
    API_GROUP_CSCALL_DICT_PLAY,
    API_GROUP_CSCALL_DICT_REC,
    API_GROUP_CSCALL_VOIP_OUT,
    API_GROUP_CSCALL_VOIP_IN,
    API_GROUP_ATT
} api_group_t;



typedef struct srv_session_s
{
    srv_session_dtor_fp_t  dtor_fp;
    int                    dtor_must_be_serialized;
    api_group_t            api_group; // Used by clients, ignored by server
} srv_session_t;

struct client_info
{
    // NOTE: epoll_wait() code depends on fd being at the top of
    // the client_info struct!!
    int fd;

    srv_session_t* client_state_p;
    void* recv_buf;
    size_t recv_buf_size;
    unsigned int   recv_buf_bytes_valid;

    // The session should close (due to POLLHUP or close request)
    int  close_requested;

    // Nothing remains; socket should be closed and removed from
    // epoll set.
    int  disconnect_requested;

    // Detect socket abuse (new command arriving to client while already
    // processign a command).
    int busy;

    // A request that could have needed a worker thread was requested
    // while no worker thread was available. The request will be carried
    // out once a worker thread is again available.
    int request_queued;
};


// TODO: Rename to assert_srv_-?


// Worker thread must not access any shared state - only its own copy.

void srv_become_worker_thread();
void srv_become_normal_thread();

void srv_dbg_assert_is_worker_thread(const char* f, int l);
void srv_dbg_assert_is_not_worker_thread(const char* f, int l);
void srv_dbg_assert_is_serialized_request(const char* f, int l);

#define SRV_DBG_ASSERT_IS_WORKER_THREAD srv_dbg_assert_is_worker_thread(__FILE__,__LINE__)
#define SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD srv_dbg_assert_is_not_worker_thread(__FILE__,__LINE__)
#define SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST srv_dbg_assert_is_serialized_request(__FILE__,__LINE__)


typedef enum {
    CB_TYPE_BUFFER_FILLED,
    CB_TYPE_BUFFER_EMPTIED,
    CB_TYPE_IL_EVENT
} ste_adm_io_cb_type_t;

void srv_read_and_drop_il_messages(il_comp_t comp, OMX_U32 port, int drop_all, int release_component);

void srv_send_il_event_msg(il_comp_t comp, OMX_EVENTTYPE eEvent,
OMX_U32 Data1, OMX_U32 Data2, OMX_PTR pEventData);
void srv_send_il_buffer_msg(il_comp_t comp, OMX_BUFFERHEADERTYPE* buffer_done_p, ste_adm_io_cb_type_t type);

ste_adm_res_t srv_init();
void srv_get_clients(int* max_clients, struct client_info** client_tbl);

#endif // INCLUSION_GUARD_STE_ADM_SRV_H_
