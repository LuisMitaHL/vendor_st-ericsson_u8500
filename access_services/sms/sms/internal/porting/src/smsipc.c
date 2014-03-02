/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "r_smslinuxporting.h"

#include <stdbool.h>            //for bool: true/false
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include "smsipc.h"
#include "g_sms.h"              // For SMS_SERVER_SOCK_PATH

struct ipc_client_context_t {
    unsigned int client_context_id;
    int request_socket;
    int event_socket;
    struct ipc_client_context_t *next;
};

#define MAX_THREAD_NAME_LENGTH  (3)     // for now.  Excl. NULL-termination
//IPC server context
struct ipc_server_context_t {
    //"Static info"
    struct ipc_client_context_t *client_context_list;
    int listen_socket;
    int internal_pipe_read_fd;
    int internal_pipe_write_fd;
    ipc_handle_fd_changes_cb_t handle_fd_changes_cb;
    ipc_build_fd_cb_t build_fd_cb;
    ipc_destroy_cb_t destroy_cb;

    char thread_name[MAX_THREAD_NAME_LENGTH + 1];       // just for traceing  TODO better

    //"Dynamic info"
    bool all_events_handled;
    fd_set read_fd_set;
    fd_set write_fd_set;
};

/*
 * Private signal header struct for the IPC library, containing the size of an OSE signal sent over a socket.
 *
 */
typedef struct {
    unsigned int size;
    struct ipc_client_context_t *client_context;
} private_signal_header_t;


#define IMS_SERVER_BACKLOG (5)

#define L_SMS_SERVER_SOCK_PATH SMS_SERVER_SOCK_PATH
#ifndef HAVE_ANDROID_OS
#define L_MAL_SMS_SOCK_PATH    "/tmp/socket_malsms"
#else
#define L_MAL_SMS_SOCK_PATH    "/dev/socket/malsms"
#endif

//Remember the last error
static ipc_result_t ipc_last_error = IPC_RESULT_OK;

static bool ipc_connect(
    int ims_instance_id,
    int *client_socket,
    const char *const path);

static bool ipc_create_server(
    int ipc_instance_id,
    int *server_socket,
    const char *const path);

static bool ipc_create_pipe(
    int *read_fd,
    int *write_fd);

static bool ipc_send_signal(
    int socket,
    void **signal);

static void *ipc_receive_signal(
    int socket);

static void ipc_set_client_context(
    void *signal,
    ipc_client_context_t * client_context);

static void ipc_client_context_add(
    struct ipc_client_context_t **list,
    struct ipc_client_context_t *item);

static void ipc_client_context_remove(
    struct ipc_client_context_t **list,
    unsigned int client_context_id);

static struct ipc_client_context_t *ipc_client_context_find_from_id(
    struct ipc_client_context_t *list,
    unsigned int client_context_id);

static int ipc_client_context_count(
    struct ipc_client_context_t *list);

static bool ipc_handle_new_connection(
    int listen_socket,
    struct ipc_client_context_t **client_context_list);

static void ipc_client_context_add_all_fds_to_fd_set(
    struct ipc_client_context_t *list,
    fd_set * file_descriptors,
    int *highest_socket_number);

static struct ipc_server_context_t *ipc_server_context_init(
    const int listen_socket,
    const int internal_pipe_read_fd,
    const int internal_pipe_write_fd,
    ipc_build_fd_cb_t build_fd_cb,
    ipc_handle_fd_changes_cb_t handle_fd_changes_cb,
    ipc_destroy_cb_t destroy_cb);

static unsigned int ipc_get_signal_size(
    void *signal);

bool ipc_connect_request(
    int ims_instance_id,
    int *client_socket,
    unsigned int *client_id,
    const char *const path)
{
    int new_socket = -1;
    unsigned int zero = 0;


    if (!ipc_connect(ims_instance_id, &new_socket, path)) {
        return false;
    }

    if (send(new_socket, &zero, sizeof(unsigned int), 0) != sizeof(unsigned int)) {
        SMS_A_(SMS_LOG_E("ipc_connect_request: failed to send 0"));
        close(new_socket);
        return false;
    }

    if (recv(new_socket, client_id, sizeof(unsigned int), 0) != sizeof(unsigned int)) {
        SMS_A_(SMS_LOG_E("ipc_connect_request - failed to receive client_id"));
        close(new_socket);
        return false;
    }

    *client_socket = new_socket;

    return true;
}

bool ipc_connect_event(
    int ims_instance_id,
    int *client_socket,
    unsigned int client_id,
    const char *const path)
{
    int new_socket = -1;

    if (!ipc_connect(ims_instance_id, &new_socket, path)) {
        return false;
    }

    if (send(new_socket, &client_id, sizeof(unsigned int), 0) != sizeof(unsigned int)) {
        SMS_A_(SMS_LOG_E("ipc_connect_event: failed to send client_id"));
        return false;
    }

    *client_socket = new_socket;

    return true;
}


static bool ipc_connect(
    int ims_instance_id,
    int *client_socket,
    const char *const path)
{
    int s;
    int r;

    struct sockaddr_un remote;

    s = socket(PF_UNIX, SOCK_STREAM, 0);

    if (s == -1) {
        return false;
    }

    memset(&remote, 0, sizeof(remote));
    remote.sun_family = AF_UNIX;
    strncpy(remote.sun_path, SMS_SERVER_SOCK_PATH, sizeof(remote.sun_path));
    SMS_B_(SMS_LOG_D("ipc_connect: '%s'", &remote.sun_path[1]));

    r = connect(s, (struct sockaddr *) &remote, sizeof(remote));

    if (r == -1) {
        // Clean-up and return error if something has gone wrong.
        SMS_A_(SMS_LOG_E("ipc_connect: connect() errno=%d: %s", errno, strerror(errno)));
        close(s);
        return false;
    }

    *client_socket = s;

    return true;
}


static bool ipc_create_server(
    int ims_instance_id,
    int *server_socket,
    const char *const path)
{
    struct sockaddr_un local;
    int s;
    int r;

    s = socket(PF_UNIX, SOCK_STREAM, 0);

    if (s == -1) {
        return false;
    }

    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, SMS_SERVER_SOCK_PATH, sizeof(local.sun_path) - 1);
    unlink(local.sun_path);
    SMS_B_(SMS_LOG_D("ipc_create_server: %s", &local.sun_path[1]));

    r = bind(s, (struct sockaddr *)&local, sizeof(local));

    if (r == -1) {
        // Clean up and return error indication to caller
        SMS_A_(SMS_LOG_E("ipc_create_server: bind() errno=%d: %s", errno, strerror(errno)));
        close(s);
        return false;
    }

    r = listen(s, IMS_SERVER_BACKLOG);

    if (r == -1) {
        // Clean up and return error indication to caller
        SMS_A_(SMS_LOG_E("ipc_create_server: listen() errno=%d: %s", errno, strerror(errno)));
        close(s);
        unlink(local.sun_path);
        return false;
    }

    *server_socket = s;

    return true;
}

static
bool ipc_send_signal(
    int socket,
    void **signal)
{
    int r;
    unsigned int size;

    if (socket == -1) {
        SMS_A_(SMS_LOG_E("ipc_send_signal: invalid socket %d", socket));
        return false;
    }

    size = ipc_get_signal_size(*signal);

    SMS_C_(SMS_LOG_V("ipc_send_signal: sending signal %08X with size %u over socket %d", (unsigned int)(*signal), size, socket));

    r = write(socket, &size, sizeof(unsigned int));

    if (r != (sizeof(unsigned int))) {
        SMS_A_(SMS_LOG_E("ipc_send_signal: write() errno=%d: %s", errno, strerror(errno)));
        return false;
    }

    r = write(socket, *signal, size);

    if (r == -1 || r != (int) size) {
        SMS_A_(SMS_LOG_E("ipc_send_signal: write() errno=%d: %s", errno, strerror(errno)));
        return false;
    }

    ipc_sig_free_buf(signal);

    SMS_C_(SMS_LOG_V("ipc_send_signal: signal sent successfully"));
    return true;
}

ipc_result_t ipc_get_last_error(
    void)
{
    return ipc_last_error;
}

void *ipc_sig_alloc(
    unsigned int size)
{
    private_signal_header_t *sig_raw = (private_signal_header_t *) malloc(size + sizeof(private_signal_header_t));
    //NOTE: this is not really necessary, but keeps Valgrind quiet when we send the signals over the socket
    memset(sig_raw, 0, size + sizeof(private_signal_header_t));
    sig_raw->size = size;
    return (void *) (sig_raw + 1);
}

static
void *ipc_receive_signal(
    int socket)
{
    int n;
    unsigned int signal_size;
    unsigned int bytesleft;
    private_signal_header_t *sig_raw;
    void *incoming_signal;

    SMS_C_(SMS_LOG_V("ipc_receive_signal: read signal size from socket %d", socket));
    n = read(socket, &signal_size, sizeof(unsigned int));       // receive signal size

    if (n == 0) {
        SMS_A_(SMS_LOG_W("ipc_receive_signal: socket %d is closed", socket));
        ipc_last_error = IPC_RESULT_SOCKET_CLOSED;
        return NULL;
    } else if (n < 0) {
        SMS_A_(SMS_LOG_E("ipc_receive_signal: read(size=%d) errno=%d: %s", sizeof(unsigned int), errno, strerror(errno)));
        ipc_last_error = IPC_RESULT_ERRNO;
        return NULL;
    } else if (n != sizeof(unsigned int)) {
        SMS_A_(SMS_LOG_E("ipc_receive_signal: read(size=%d) got=%d", sizeof(unsigned int), n));
        ipc_last_error = IPC_RESULT_INTERNAL_SIGNAL_ERROR;
        return NULL;
    }

    sig_raw = (private_signal_header_t *) malloc(signal_size + sizeof(private_signal_header_t));

    if (sig_raw == 0) {
        return NULL;
    }

    sig_raw->size = signal_size;
    sig_raw->client_context = NULL;

    incoming_signal = (void *) (sig_raw + 1);

    bytesleft = signal_size;
    //Loop recv() until we have received the whole signal, or until error is returned...
    do {
        // receive signal size
        SMS_C_(SMS_LOG_V("ipc_receive_signal: reading signal from socket %d bytes left %d", socket, bytesleft));
        n = read(socket, (char *) (incoming_signal) + (signal_size - bytesleft), bytesleft);
        if (n == 0) {
            SMS_A_(SMS_LOG_W("ipc_receive_signal: socket %d is closed", socket));
            ipc_last_error = IPC_RESULT_SOCKET_CLOSED;
            free(sig_raw);
            return NULL;
        } else if (n < 0) {
            SMS_A_(SMS_LOG_E("ipc_receive_signal: read(size=%d) errno=%d: %s", bytesleft, errno, strerror(errno)));
            ipc_last_error = IPC_RESULT_ERRNO;
            free(sig_raw);
            return NULL;
        } else {
            bytesleft -= n;
        }
    } while (bytesleft);

    SMS_C_(SMS_LOG_V("ipc_receive_signal: returned signal %d", socket));
    return incoming_signal;
}

void *ipc_receive_signal_over_socket(
    int socket)
{
    return ipc_receive_signal(socket);
}

bool ipc_send_signal_over_socket(
    int socket,
    void **signal)
{
    return ipc_send_signal(socket, signal);
}

void *ipc_receive_signal_over_pipe(
    int fd)
{
    return ipc_receive_signal(fd);
}

bool ipc_send_signal_over_pipe(
    int fd,
    void **signal)
{
    return ipc_send_signal(fd, signal);
}


static void ipc_set_client_context(
    void *signal,
    struct ipc_client_context_t *client_context)
{
    private_signal_header_t *header = (private_signal_header_t *) signal;
    header -= 1;
    header->client_context = client_context;
}


int ipc_get_request_fd(
    void *signal)
{
    if (signal) {
        private_signal_header_t *header = (private_signal_header_t *) signal;
        header -= 1;
        if (header->client_context) {
            return header->client_context->request_socket;
        }
    }
    return -1;
}

int ipc_get_event_fd(
    void *signal)
{
    if (signal) {
        private_signal_header_t *header = (private_signal_header_t *) signal;
        header -= 1;
        if (header->client_context) {
            return header->client_context->event_socket;
        }
    }
    return -1;
}

static unsigned int ipc_get_signal_size(
    void *signal)
{
    private_signal_header_t *header = (private_signal_header_t *) signal;
    header -= 1;
    return header->size;
}


static void ipc_client_context_add(
    struct ipc_client_context_t **list,
    struct ipc_client_context_t *new_item)
{
    if (*list == NULL) {
        *list = new_item;
    } else {
        struct ipc_client_context_t *item = *list;

        while (item->next) {
            item = item->next;
        }

        item->next = new_item;
    }
}

static void ipc_client_context_remove(
    struct ipc_client_context_t **list,
    unsigned int client_context_id)
{
    struct ipc_client_context_t *item_curr = *list;
    struct ipc_client_context_t *item_prev = NULL;

    while (item_curr) {
        if (item_curr->client_context_id == client_context_id) {
            break;
        }
        item_prev = item_curr;
        item_curr = item_curr->next;
    }

    if (item_curr) {
        if (item_prev) {        // not first in list
            item_prev->next = item_curr->next;
        } else {                // first item in list
            *list = item_curr->next;
        }

        if (item_curr->request_socket != -1) {
            close(item_curr->request_socket);
        }

        if (item_curr->event_socket != -1) {
            close(item_curr->event_socket);
        }

        free(item_curr);
    } else {
        SMS_A_(SMS_LOG_E("ipc_client_context_remove - Client with client_context_id %08X not found", (unsigned int) client_context_id));
    }
}

static struct ipc_client_context_t *ipc_client_context_find_from_id(
    struct ipc_client_context_t *list,
    unsigned int client_context_id)
{
    while (list) {
        if (list->client_context_id == client_context_id) {
            break;
        }
        list = list->next;
    }
    return list;
}

static int ipc_client_context_count(
    struct ipc_client_context_t *list)
{
    int Num = 0;

    while (list) {
        ++Num;
        list = list->next;
    }

    return Num;
}

static bool ipc_handle_new_connection(
    int listen_socket,
    struct ipc_client_context_t **client_context_list)
{
    struct ipc_client_context_t *new_client;
    int new_socket;
    unsigned int client_context_id;
    struct sockaddr_un remote;
    socklen_t sock_addr_size;

    sock_addr_size = sizeof(remote);
    new_socket = accept(listen_socket, (struct sockaddr *) &remote, &sock_addr_size);

    if (new_socket == -1) {
        return false;
    }
    // receive client_context_id
    if (recv(new_socket, &client_context_id, sizeof(unsigned int), 0) != sizeof(unsigned int)) {
        return false;
    }
    // if client_context_id is 0 it indicates a new request socket connection
    // if client_context_id != 0 it indicates a new event socket connection

    if (client_context_id == 0) {
        new_client = malloc(sizeof(struct ipc_client_context_t));
        memset(new_client, 0, sizeof(struct ipc_client_context_t));

        new_client->request_socket = new_socket;
        new_client->event_socket = -1;
        new_client->client_context_id = (unsigned int) new_client;

        if (send(new_socket, &new_client->client_context_id, sizeof(unsigned int), 0) != sizeof(unsigned int)) {
            free(new_client);
            return false;
        }

        ipc_client_context_add(client_context_list, new_client);

        SMS_B_(SMS_LOG_D("ipc_handle_new_connection - client %08X semi-connected, request socket: %d, event socket: ?",
                (unsigned int) new_client->client_context_id, new_client->request_socket));

        return true;
    } else {
        new_client = ipc_client_context_find_from_id(*client_context_list, client_context_id);

        if (!new_client) {
            return false;
        }

        new_client->event_socket = new_socket;

        SMS_B_(SMS_LOG_D("ipc_handle_new_connection - client 0x%ux fully connected, request socket: %d, event socket: %d",
                new_client->client_context_id, new_client->request_socket, new_client->event_socket));

        return true;
    }
}

static void ipc_client_context_add_all_fds_to_fd_set(
    struct ipc_client_context_t *list,
    fd_set * file_descriptors,
    int *highest_socket_number)
{
    struct ipc_client_context_t *client_context = list;

    while (client_context) {
        int request_socket;

        request_socket = client_context->request_socket;

        if (request_socket != -1) {
            FD_SET(client_context->request_socket, file_descriptors);

            if (request_socket > *highest_socket_number) {
                *highest_socket_number = request_socket;
            }
        }

        client_context = client_context->next;
    }
}

static bool ipc_create_pipe(
    int *read_fd,
    int *write_fd)
{
    int pipe_fds[2];

    if (pipe(pipe_fds) != 0) {
        return false;
    }

    *read_fd = pipe_fds[0];
    *write_fd = pipe_fds[1];

    return true;
}


/*
 * server_context_init
 *
 * server_context -       IPC server's context struct.
 * listen_socket -          The IPC server will listen for new clients on this socket.
 * internal_pipe_read_fd -  The IPC server can have a separate internal pipe read FD.
 * build_fd_cb -            The IPC server can have a separate function for adding more FDs to the Read/Write/Except FD sets.
 * handle_fd_changes_cb -   Call-back that will handle changes on the Read/Write/Except FD sets.
 *
 */
static struct ipc_server_context_t *ipc_server_context_init(
    const int listen_socket,
    const int internal_pipe_read_fd,
    const int internal_pipe_write_fd,
    ipc_build_fd_cb_t build_fd_cb,
    ipc_handle_fd_changes_cb_t handle_fd_changes_cb,
    ipc_destroy_cb_t destroy_cb)
{
    struct ipc_server_context_t *server_context = malloc(sizeof(struct ipc_server_context_t));
    if (server_context) {
        memset(server_context, 0, sizeof(struct ipc_server_context_t));
        server_context->client_context_list = NULL;
        server_context->listen_socket = listen_socket;
        server_context->internal_pipe_read_fd = internal_pipe_read_fd;
        server_context->internal_pipe_write_fd = internal_pipe_write_fd;
        server_context->handle_fd_changes_cb = handle_fd_changes_cb;
        server_context->build_fd_cb = build_fd_cb;
        server_context->destroy_cb = destroy_cb;
        server_context->all_events_handled = true;
    }
    return server_context;
}

void ipc_server_context_destroy(
    ipc_server_context_t * server_context)
{
    ipc_client_context_t *list;
    ipc_client_context_t *tmplist;

    SMS_B_(SMS_LOG_D("ipc_server_context_destroy"));

    if (server_context) {
        list = server_context->client_context_list;

        close(server_context->listen_socket);
        close(server_context->internal_pipe_read_fd);
        close(server_context->internal_pipe_write_fd);

        server_context->listen_socket = -1;
        server_context->internal_pipe_read_fd = -1;
        server_context->internal_pipe_write_fd = -1;

        while (list) {
            if (list->request_socket != -1) {
                close(list->request_socket);
                list->request_socket = -1;
            }
            if (list->event_socket != -1) {
                close(list->event_socket);
                list->event_socket = -1;
            }
            list = list->next;
        }
        list = server_context->client_context_list;
        while (list) {
            tmplist = list->next;
            free(list);
            list = tmplist;
        }

        free(server_context);
    }
    return;
}

struct ipc_server_context_t *ipc_server_context_create(
    int ims_instance_id,
    const char *const socket_path,
    ipc_build_fd_cb_t build_fd_cb,
    ipc_handle_fd_changes_cb_t handle_fd_changes_cb,
    ipc_destroy_cb_t destroy_cb)
{
    int ipc_internal_pipe_read_fd = -1;
    int ipc_internal_pipe_write_fd = -1;
    int ipc_listen_socket = -1;
    struct ipc_server_context_t *server_context = NULL;

    SMS_B_(SMS_LOG_D("ipc_server_context_create: socket_path = %s", socket_path));

    // create the internal sip pipe
    if (!ipc_create_pipe(&ipc_internal_pipe_read_fd, &ipc_internal_pipe_write_fd)) {
        SMS_B_(SMS_LOG_D("ipc_server_context_create: failed to create internal pipe"));
        exit(1);
    }

    SMS_A_(SMS_LOG_I("ipc_server_context_create: internal pipe created, read: %d, write: %d", ipc_internal_pipe_read_fd, ipc_internal_pipe_write_fd));

    if (!ipc_create_server(ims_instance_id, &ipc_listen_socket, socket_path)) {
        SMS_A_(SMS_LOG_E("ipc_server_context_create: ipc_create_server() failed, fatal error, exiting"));
        exit(1);
    }

    server_context = ipc_server_context_init(ipc_listen_socket, ipc_internal_pipe_read_fd, ipc_internal_pipe_write_fd, build_fd_cb, handle_fd_changes_cb, destroy_cb);


    // For tracing
    if (0 == strcmp(socket_path, L_SMS_SERVER_SOCK_PATH)) {
        strncpy(server_context->thread_name, "SMS", MAX_THREAD_NAME_LENGTH);
    } else if (0 == strcmp(socket_path, L_MAL_SMS_SOCK_PATH)) {
        strncpy(server_context->thread_name, "MAL", MAX_THREAD_NAME_LENGTH);
    } else {
        strncpy(server_context->thread_name, "???", MAX_THREAD_NAME_LENGTH);
    }

    return server_context;
}

int ipc_server_context_get_internal_pipe_write_fd(
    struct ipc_server_context_t *server_context)
{
    return server_context->internal_pipe_write_fd;
}

void *ipc_server_context_await_signal(
    struct ipc_server_context_t *server_context)
{
    //struct timeval TimeVal;
    int highest_fd = -1;
    int num_changed_fds;
    struct ipc_client_context_t *client_context;
    fd_set *read_fd_set = &server_context->read_fd_set;
    fd_set *write_fd_set = &server_context->write_fd_set;

    if (server_context == NULL) {
        SMS_A_(SMS_LOG_E("ipc_server_context_await_signal: input param of server_context is NULL, FATAL"));
        return NULL;
    }
    SMS_C_(SMS_LOG_V
           ("ipc_server_context_await_signal(%s): <<ENTERING>> (client context count: %d)", server_context->thread_name, ipc_client_context_count(server_context->client_context_list)));

    if (server_context->all_events_handled) {
        FD_ZERO(read_fd_set);
        FD_ZERO(write_fd_set);

        FD_SET(server_context->internal_pipe_read_fd, read_fd_set);
        FD_SET(server_context->listen_socket, read_fd_set);

        highest_fd = server_context->listen_socket;
        if (highest_fd < server_context->internal_pipe_read_fd)
            highest_fd = server_context->internal_pipe_read_fd;

        ipc_client_context_add_all_fds_to_fd_set(server_context->client_context_list, read_fd_set, &highest_fd);

        //Invoke IPC server call-back to add caller specific Read/Write FDs, if specified...
        if (server_context->build_fd_cb) {
            server_context->build_fd_cb(read_fd_set, write_fd_set, &highest_fd);
        }

        SMS_C_(SMS_LOG_V("ipc_server_context_await_signal(%s): select()ing", server_context->thread_name));

        num_changed_fds = select(highest_fd + 1, read_fd_set, write_fd_set, NULL, NULL /*&TimeVal */ );

        SMS_C_(SMS_LOG_V("ipc_server_context_await_signal(%s): num_changed_fds: %d", server_context->thread_name, num_changed_fds));

        if (num_changed_fds == -1) {
            SMS_A_(SMS_LOG_E("ipc_server_context_await_signal(%s): select() returned -1, but should block, FATAL", server_context->thread_name));
            exit(1);
        }

        server_context->all_events_handled = false;
    }
    //Invoke caller handle_fd_changes_cb, if specified...
    if (server_context->handle_fd_changes_cb) {
        server_context->handle_fd_changes_cb(read_fd_set, write_fd_set);
    }

    if (FD_ISSET(server_context->internal_pipe_read_fd, read_fd_set)) {
        void *signal;

        FD_CLR(server_context->internal_pipe_read_fd, read_fd_set);

        signal = ipc_receive_signal_over_pipe(server_context->internal_pipe_read_fd);

        if (signal == NULL) {
            SMS_A_(SMS_LOG_E("ipc_server_context_await_signal(%s): Got NULL-signal from internal pipe read fd, FATAL", server_context->thread_name));
            exit(1);
        }

        SMS_C_(SMS_LOG_V("ipc_server_context_await_signal(%s): got signal from internal pipe fd", server_context->thread_name));

        return signal;
    }

    if (FD_ISSET(server_context->listen_socket, read_fd_set)) {
        FD_CLR(server_context->listen_socket, read_fd_set);

        if (!ipc_handle_new_connection(server_context->listen_socket, &server_context->client_context_list)) {
            SMS_A_(SMS_LOG_E("ipc_server_context_await_signal(%s): call to HandleNewIPC_Connection failed", server_context->thread_name));
        }

        return NULL;
    }

    client_context = server_context->client_context_list;

    while (client_context) {
        if (FD_ISSET(client_context->request_socket, read_fd_set)) {
            void *signal;

            FD_CLR(client_context->request_socket, read_fd_set);

            signal = ipc_receive_signal_over_socket(client_context->request_socket);

            if (signal == NULL) {
                SMS_A_(SMS_LOG_W("ipc_server_context_await_signal(%s): Client context with address 0x%ux disconnected, request socket: %d, client context count: %d",
                     server_context->thread_name, (unsigned int) client_context, client_context->request_socket, ipc_client_context_count(server_context->client_context_list)));

                if (server_context->destroy_cb) {
                    server_context->destroy_cb(client_context->event_socket);
                }

                ipc_client_context_remove(&server_context->client_context_list, client_context->client_context_id);
            } else {
                SMS_C_(SMS_LOG_V("ipc_server_context_await_signal(%s): got REQUEST from client %08uX", server_context->thread_name, (unsigned int) client_context));
                ipc_set_client_context(signal, client_context);
            }

            return signal;
        }

        client_context = client_context->next;
    }

    SMS_C_(SMS_LOG_V("ipc_server_context_await_signal(%s): No more changes.", server_context->thread_name));

    server_context->all_events_handled = true;

    return NULL;
}

void ipc_sig_free_buf(
    void **sig)
{
    private_signal_header_t *sig_raw = (private_signal_header_t *) * sig;
    if (sig_raw) {
        sig_raw -= 1;
        SMS_C_(SMS_LOG_V("(free_buf) freeing %08X", (unsigned int) *sig));
        free(sig_raw);
        *sig = NULL;
    }
}
