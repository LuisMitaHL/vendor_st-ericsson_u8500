/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 *
 * IPC interface - An Inter Process Communication porting layer for OSE-to-Linux.
 * This library makes it possible to port OSE style processes to run on Linux with a limited effort.
 *
 * Note: this file should NOT include any OSE platform related files - only Linux related files are allowed here!
 *
 */
#ifndef _SMSIPC_H
#define _SMSIPC_H
#include <stdbool.h>            //for bool: true/false
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <dirent.h>
#include <signal.h>

/*
 *
 *
 * Type definitions and constants
 *
 *
 */

/*
 *
 * IPC result codes.
 *
 * IPC_RESULT_OK                      - Success
 * IPC_RESULT_SOCKET_CLOSED           - Failed, socket was closed.
 * IPC_RESULT_ERRNO                   - Failed, check errno variable for detailed error information.
 * IPC_RESULT_INTERNAL_SIGNAL_ERROR   - Failed, internal IPC protocol error.
 */
typedef enum {
    IPC_RESULT_OK,
    IPC_RESULT_SOCKET_CLOSED,
    IPC_RESULT_ERRNO,
    IPC_RESULT_INTERNAL_SIGNAL_ERROR
} ipc_result_t;

/*
 *
 * Opaque structs, defined in ipc.c
 *
 */
typedef struct ipc_client_context_t ipc_client_context_t;
typedef struct ipc_server_context_t ipc_server_context_t;


/*
 * Call-back function, invoked to handle FD changes in the server context that calls ipc_server_context_await_signal.
 */
typedef void (
    *ipc_handle_fd_changes_cb_t) (
    fd_set * const read_fd_set,
    fd_set * const write_fd_set);

/*
 * Call-back function, invoked to build up a Read/Write FD set for the ipc_server_context_await_signal.
 * The IPC server process shall add its specific FDs to the incoming FD sets.
 */
typedef void (
    *ipc_build_fd_cb_t) (
    fd_set * const read_fd_set,
    fd_set * const write_fd_set,
    int *const highest_fd);

/*
 * Call-back function, invoked to clean up when client disappears (socket is disconnected)
 */
typedef void (
    *ipc_destroy_cb_t) (
    const int event_socket);


/* ----------------------------------------------------------------------
 *
 *
 *
 *
 *
 * IPC server related functions (typically SwBP server provider process)
 *
 *
 *
 *
 *
 *----------------------------------------------------------------------*/

/*
 * ipc_server_context_create
 *
 * The IPC server (typically the SwBP service provider process) shall call this function once at process start-up.
 *
 * In: ipc_instance_id      - an ID that defines which demon/process that contains the IPC server implementation
 * In: socket_path          - SOCK_PATH_SIP, SOCK_PATH_RTP etc
 * In: Build_FD_Cb          - The IPC server can have a separate function for adding more FDs to the Read/Write/Except FD sets.
 * In: HandleFDChanges_Cb   - Call-back that will handle changes on the Read/Write/Except FD sets.
 * In: clean_up_cb          - Call-back that will handle clean up when a client context disappears (request/event socket closed).
 *
 * Returns                  - an IPC server context pointer to be used in future calls, e.g ipc_server_context_await_signal().
 */
struct ipc_server_context_t *ipc_server_context_create(
    int ipc_instance_id,
    const char *const socket_path,
    ipc_build_fd_cb_t build_fd_cb,
    ipc_handle_fd_changes_cb_t handle_fd_changes_cb,
    ipc_destroy_cb_t destroy_cb);

/*
 * ipc_server_context_await_signal
 *
 * The IPC server (typically the SwBP service provider process) will call this function (instead of the OSE RECEIVE) in the process' main loop.
 *
 * This function will
 * - handle new IPC client connections,
 * - read and convert incoming pipe/socket data to pSignals.
 * - put information in the pSignal header for the user to extract event receiver by using EVENT_RECEIVER(pSignal)
 *
 * In: pServerContext       - IPC server's context struct.
 *
 * Returns:                 - an OSE style signal
 *
 */
void *ipc_server_context_await_signal(
    struct ipc_server_context_t *server_context);

/*
 * ipc_server_context_get_internal_pipe_write_fd
 *
 * Each IPC server owns a general purpose pipe for writing and receiving data that is not necessarily SwBP request/response signals,
 * e.g if a process wants to send a signal to itself for driving a state machine, or other module internal signals.
 *
 * In: pServerContext       - IPC server's context struct.
 *
 * Returns:                 - the file descriptor that can be used for pipe WRITE operations
 *
 */
int ipc_server_context_get_internal_pipe_write_fd(
    struct ipc_server_context_t *server_context);


/*
 * Send an OSE signal over IPC pipe.
 *
 * In:      fd              - the file descriptor id where OSE style request/event signals will be sent
 * In:      signal          - the signal to send. Note: this buffer will be freed, just as with OSE SIGNAL in the SEND() call.
 *
 * Returns: success/fail    - if this call fails, the function ipc_get_last_error() can be called to get a result code: ipc_result_t.
 */
bool ipc_send_signal_over_pipe(
    int fd,
    void **signal);

/*
 * Receive an OSE signal over IPC pipe.
 *
 * In:      fd          - the file descriptor id where OSE style signals will be received
 *
 * Returns: OSE style signal
 */
void *ipc_receive_signal_over_pipe(
    int fd);

/*
 * Get request-socket file descriptor from signal struct.
 *
 * In:      signal   - an OSE signal where we want to find out the destination "process" for the SwBP response signal
 *
 * Returns: File Descriptor
 */
int ipc_get_request_fd(
    void *signal);

/*
 * Get event-socket file descriptor from signal struct.
 *
 * In:      signal   - an OSE signal where we want to find out the destination "process" for a SwBP event signal
 *
 * Returns:          - event file descriptor
 */
int ipc_get_event_fd(
    void *signal);


void ipc_server_context_destroy(
    ipc_server_context_t * server_context);

/* ----------------------------------------------------------------------
 *
 *
 *
 *
 *
 * IPC client related functions (the user of a SwBP service)
 *
 *
 *
 *
 *
 *----------------------------------------------------------------------*/

/*
 * Connect the request-socket towards an IPC server provider (typically an OSE SwBP server implementation).
 *
 * In:      ipc_instance_id - an ID that defines which demon/process that contains the IPC server implementation
 *                            (if >1 server is running on the same device)
 * Out:     client socket   - socket to use for sending OSE "SwBP requests"
 * Out:     client_id       - unique ID for this client context
 * In:      path            - socket path for the demon to connect with, e.g. SOCK_PATH_SIP, SOCK_PATH_RTP etc
 *
 * Returns:                 - true/false
 */
bool ipc_connect_request(
    int ipc_instance_id,
    int *client_socket,
    unsigned int *client_id,
    const char *const path);

/*
 * Connect the request-socket towards an IPC server provider (typically an OSE SwBP server implementation).
 *
 * In:      ipc_instance_id - an ID that defines which demon/process that contains the IPC server implementation
 *                            (if >1 server is running on the same device)
 * Out:     client socket   - socket to use for receiving OSE "SwBP events"
 * Out:     client_id       - unique ID for this client context
 * In:      path            - socket path for the demon to connect with
 *
 * Returns:                 - true/false
 */
bool ipc_connect_event(
    int ipc_instance_id,
    int *client_socket,
    unsigned int client_id,
    const char *const path);

/* ----------------------------------------------------------------------
 *
 *
 *
 *
 *
 * Misc IPC functions
 *
 *
 *
 *
 *
 *----------------------------------------------------------------------*/

/*
 * Allocate a signal buffer to be used for sending an OSE style signal.
 *
 * In:      size            - size of the OSE style signal
 *
 * Returns: pointer to the start of the OSE style signal
 */
void *ipc_sig_alloc(
    unsigned int size);

/*
 * Receive an OSE signal over IPC socket.
 *
 * In:      socket          - the socket id where OSE style request signals will be received
 *
 * Returns: OSE style signal
 */
void *ipc_receive_signal_over_socket(
    int socket);

/*
 * Send an OSE signal over IPC socket.
 *
 * In:      socket          - the socket id where OSE style request/event signals will be sent
 * In:      signal          - the signal to send. Note: this buffer will be freed, just as with OSE SIGNAL in the SEND() call.
 *
 * Returns: success/fail    - if this call fails, the function ipc_get_last_error() can be called to get a result code: ipc_result_t.
 */
bool ipc_send_signal_over_socket(
    int socket,
    void **signal);

/*
 * Fetch the last error result code from the IPC library.
 *
 * Returns: ipc_result_t    - if IPC_RESULT_ERRNO is returned, the errno variable can be read to get the last pipe/socket error code.
 */
ipc_result_t ipc_get_last_error(
    void);


void ipc_sig_free_buf(
    void **sig);

#endif                          //_SMSIPC_H
