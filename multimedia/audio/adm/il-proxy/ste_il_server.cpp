#include <stdio.h>
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "ste_il_server.h"
#include <pthread.h>
#include <signal.h>
#include "ste_adm_omx_log.h" // just for enum translations, e.g. adm_log_err2str
#include <sys/time.h>

pthread_mutex_t il_srv_fd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

struct comp
{
    OMX_HANDLETYPE real_handle;
    int client_handle;
    int client_fd; // our side of the fd
    OMX_STATETYPE state; // used to verify that state is correct when FreeHandle is called
    char name[80];
};

void error_found()
{
    while(1) {
        fprintf(stderr, "ILSRV: Error detected, ilsrv does no longer accept requests.\n");
        fprintf(stderr, "ILSRV: (not exiting in order to avoid triggering CM/ENS/NMF cleanup)\n");
        usleep(5 * 1000 * 1000);
    }
}

// for debug prints
static const char *last_part(const char* buf)
{
   const int max_len = 25;
   size_t len = strlen(buf);
   if (len <= max_len) return buf;
   return buf + (len - max_len);
}

// list of all valid handles, only used to avoid crashes if given bad handle
#define MAX_COMPONENTS 200
struct comp* valid_comps[MAX_COMPONENTS];
int num_valid_comps = 0;

void add_component(struct comp* c)
{
    pthread_mutex_lock(&list_mutex);
        if (num_valid_comps >= MAX_COMPONENTS) { fprintf(stderr, "Too many components\n"); error_found(); }
        valid_comps[num_valid_comps++] = c;
    pthread_mutex_unlock(&list_mutex);
}

// must hold complist_mutex
int get_component_idx(struct comp* c)
{
    int i;
    for (i=0 ; i < num_valid_comps ; i++)
        if (valid_comps[i] == c) {
            return i;
        }

    return -1;
}

void verify_component_and_keep_lock(struct comp* c)
{
    pthread_mutex_lock(&list_mutex);
        int idx = get_component_idx(c);

    if (idx == -1) {
        fprintf(stderr, "verify_component failed for %X\n", c);
        error_found();
    }
}

void verify_component(struct comp* c)
{
    verify_component_and_keep_lock(c);
    pthread_mutex_unlock(&list_mutex);
}


void del_component(struct comp* c)
{
    pthread_mutex_lock(&list_mutex);
        int idx = get_component_idx(c);
        memmove(valid_comps+idx, valid_comps+idx+1, (num_valid_comps - (idx+1)) * sizeof(valid_comps[0]));
        num_valid_comps--;
    pthread_mutex_unlock(&list_mutex);
}

// list of all valid buffert headers, only used to avoid crashes if given bad data from client
#define MAX_BUFHDRS 200
struct OMX_BUFFERHEADERTYPE* valid_bufhdrs[MAX_BUFHDRS];
int num_valid_bufhdrs = 0;

void add_bufhdr(OMX_BUFFERHEADERTYPE* c)
{
    pthread_mutex_lock(&list_mutex);
        if (num_valid_bufhdrs >= MAX_BUFHDRS) { fprintf(stderr, "Too many buffers\n"); error_found(); }
        valid_bufhdrs[num_valid_bufhdrs++] = c;
    pthread_mutex_unlock(&list_mutex);
}

int get_bufhdr_idx(OMX_BUFFERHEADERTYPE* c)
{
    int i;
    for (i=0 ; i < num_valid_bufhdrs ; i++)
        if (valid_bufhdrs[i] == c)
            return i;
    return -1;
}

void del_bufhdr(OMX_BUFFERHEADERTYPE* c)
{
    pthread_mutex_lock(&list_mutex);
        int idx = get_bufhdr_idx(c);
        memmove(valid_bufhdrs+idx, valid_bufhdrs+idx+1, (num_valid_bufhdrs - (idx+1)) * sizeof(valid_bufhdrs[0]));
        num_valid_bufhdrs--;
    pthread_mutex_unlock(&list_mutex);
}


void verify_bufhdr(OMX_BUFFERHEADERTYPE* c)
{
    pthread_mutex_lock(&list_mutex);
        int idx = get_bufhdr_idx(c);
    pthread_mutex_unlock(&list_mutex);
    if (idx == -1) {
        fprintf(stderr, "verify_bufhdr failed for %X\n", c);
        error_found();
    }
}


static void send_message(int fd, struct msg* msg)
{
    pthread_mutex_lock(&il_srv_fd_mutex);
    int t=0;
    while (t < (int) sizeof(*msg)) {
        int w = send(fd, ((char*)msg)+t, sizeof(*msg)-t, MSG_NOSIGNAL);
        if (w == -1) {
            fprintf(stderr, "send of event: Socket %d not working (error: '%s')\n", fd, strerror(errno));
            error_found();
        }
        t += w;
    }
    pthread_mutex_unlock(&il_srv_fd_mutex);
}

OMX_ERRORTYPE ste_adm_omx_callback_event_handler(
                            OMX_IN OMX_HANDLETYPE   component_handle,
                            OMX_IN OMX_PTR          app_data_p,
                            OMX_IN OMX_EVENTTYPE    ev,
                            OMX_IN OMX_U32          data1,
                            OMX_IN OMX_U32          data2,
                            OMX_IN OMX_PTR          event_data_p)
{
    if (event_data_p != NULL) { fprintf(stderr, "Received event with event_data_p != NULL\n"); error_found(); }

    fprintf(stderr, "ILSRV: Received event: %X, %X, %X, %X, %X, %X\n", component_handle, app_data_p,
           ev, data1, data2, event_data_p);

    struct comp* c = (struct comp*) app_data_p;
    verify_component_and_keep_lock(c);


    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_EVENT;
    m.cmd.event.client_handle = c->client_handle;
    m.cmd.event.ev            = ev;
    m.cmd.event.nData1        = data1;
    m.cmd.event.nData2        = data2;


    if (ev == OMX_EventCmdComplete) {
        if (data1 == OMX_CommandStateSet) {
            c->state = (OMX_STATETYPE) data2;
            fprintf(stderr, "ILSRV: %08X [%20s] Dispatching CmdComplete StateSet %s\n", component_handle, last_part(c->name), adm_log_state2str((OMX_STATETYPE) data2));
        } else  if (data1 == OMX_CommandPortDisable) {
            fprintf(stderr, "ILSRV: %08X [%20s] Dispatching CmdComplete PortDisable %d\n", component_handle, last_part(c->name), data2);
        } else  if (data1 == OMX_CommandPortEnable) {
            fprintf(stderr, "ILSRV: %08X [%20s] Dispatching CmdComplete PortEnable %d\n", component_handle, last_part(c->name), data2);
        } else {
            fprintf(stderr, "ILSRV: %08X [%20s] Dispatching %s nData2=%X\n", component_handle, last_part(c->name), adm_log_cmd2str((OMX_COMMANDTYPE) data1), data2);
        }
    } else if (ev == OMX_EventError) {
        fprintf(stderr, "ILSRV: %08X [%20s] Dispatching EventError %s nData2=%d\n", component_handle, last_part(c->name), adm_log_err2str((OMX_ERRORTYPE) data1), data2);
    } else {
        fprintf(stderr, "ILSRV: %08X [%20s] Dispatching Event %s nData1=%X nData2=%X\n", component_handle, last_part(c->name), adm_log_event2str(ev), data1,data2);
    }

    send_message(c->client_fd, &m);
    pthread_mutex_unlock(&list_mutex);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "ILSRV: (%d.%d) Ready to dispatch new events\n", tv.tv_sec, tv.tv_usec/1000);

    return OMX_ErrorNone; // Pointless
}


OMX_ERRORTYPE ste_adm_omx_callback_empty_buffer_done(
                            OMX_IN OMX_HANDLETYPE           component_handle,
                            OMX_IN OMX_PTR                  app_data_p,
                            OMX_IN OMX_BUFFERHEADERTYPE*    buffer_done_p)
{
    struct comp* c = (struct comp*) app_data_p;
    verify_component(c);
    verify_bufhdr(buffer_done_p);


    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_EMPTY_BUFFER_DONE;
    m.cmd.empty_buffer_done.client_handle = (int) c->client_handle;
    m.cmd.empty_buffer_done.client_bufhdr = (int) buffer_done_p->pAppPrivate;

    fprintf(stderr, "ILSRV: %08X [%20s] Dispatching empty_buffer_done\n", component_handle, last_part(c->name));
    send_message(c->client_fd, &m);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "ILSRV: (%d.%d) Ready to dispatch new events\n", tv.tv_sec, tv.tv_usec/1000);
}

OMX_ERRORTYPE ste_adm_omx_callback_fill_buffer_done(
                            OMX_IN OMX_HANDLETYPE           component_handle,
                            OMX_IN OMX_PTR                  app_data_p,
                            OMX_IN OMX_BUFFERHEADERTYPE*    pBuffer)
{
    struct comp* c = (struct comp*) app_data_p;
    verify_component(c);

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_FILL_BUFFER_DONE;
    m.cmd.fill_buffer_done.client_handle = (int) c->client_handle;
    m.cmd.fill_buffer_done.client_bufhdr = (int) pBuffer->pAppPrivate;

    m.cmd.fill_buffer_done.out_buf_hdr.nOffset = pBuffer->nOffset;
    m.cmd.fill_buffer_done.out_buf_hdr.nTimeStamp = pBuffer->nTimeStamp;
    m.cmd.fill_buffer_done.out_buf_hdr.nFilledLen = pBuffer->nFilledLen;
    m.cmd.fill_buffer_done.out_buf_hdr.nFlags = pBuffer->nFlags;
    if (pBuffer->nAllocLen <= sizeof(m.cmd.fill_buffer_done.out_buf)) {
        memcpy(m.cmd.fill_buffer_done.out_buf, pBuffer->pBuffer, pBuffer->nAllocLen);
    }

    fprintf(stderr, "ILSRV: %08X [%20s] Dispatching fill_buffer_done\n", component_handle, last_part(c->name));
    send_message(c->client_fd, &m);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "ILSRV: (%d.%d) Ready to dispatch new events\n", tv.tv_sec, tv.tv_usec/1000);
}


static OMX_CALLBACKTYPE callbacks = {
    ste_adm_omx_callback_event_handler,
    ste_adm_omx_callback_empty_buffer_done,
    ste_adm_omx_callback_fill_buffer_done
};


static void handle_request(int fd);

int main()
{
    fprintf(stderr, "IL server starting up\n");
    OMX_Init();

    struct sockaddr_un local_sockaddr_un;
    int listener_sck_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&local_sockaddr_un, 0, sizeof(local_sockaddr_un));
    local_sockaddr_un.sun_family = AF_UNIX;
    strcpy(local_sockaddr_un.sun_path, "/tmp/il-server-socket");

    /* Unlink any previous socket */
    unlink("/tmp/il-server-socket");

    if (bind(listener_sck_fd,
         (struct sockaddr *) &local_sockaddr_un,
         sizeof(local_sockaddr_un)) < 0) {
        fprintf(stderr, "bind() %s\n", strerror(errno));
        error_found();
    }

    if (listen(listener_sck_fd, 10)) {
        fprintf(stderr, "listen() %s\n", strerror(errno));
        error_found();
    }

    fd_set master_socket_fds;
    int highest_fd = listener_sck_fd;
    FD_ZERO(&master_socket_fds);
    FD_SET(listener_sck_fd, &master_socket_fds);

    signal(SIGPIPE, SIG_IGN);

    for (;;) {
        int num_active_fds;
        fd_set tmp_read_socket_fds;
        tmp_read_socket_fds = master_socket_fds;

        /* Poll for new requests on the FD_sets */
        num_active_fds =
            select(highest_fd+1, &tmp_read_socket_fds, NULL, NULL, NULL);

        if (num_active_fds == -1) {
            fprintf(stderr, "num_active_fds == -1\n");
            exit(10);
            }

        /* If listener socket is ready for read, this means a
        client is trying to connect to it */
        if (FD_ISSET(listener_sck_fd, &tmp_read_socket_fds)) {
            int client_sck_fd;

            /* Try to accept the client connection. We need to always accept
            and then close connection if we have an overflow of clients */
            client_sck_fd = accept(listener_sck_fd,
                                    NULL,/* Don't care remote address */
                                    NULL /* Don't care remote size */);

            if (listener_sck_fd == -1) {
                fprintf(stderr, "accept() %s\n", strerror(errno));
                error_found();
            }

            /* check if we need to update upper bound used by the select()
            function */
            if (client_sck_fd > highest_fd) {
                highest_fd = client_sck_fd;
            }

            /* add the socket descriptor to the master set */
            FD_SET(client_sck_fd, &master_socket_fds);
            fprintf(stderr, "New client connected; got fd=%d\n", client_sck_fd);
            continue;
        }

        int cur_fd;
        for (cur_fd=3 ; cur_fd <= highest_fd ; cur_fd++) {
            if (FD_ISSET(cur_fd, &tmp_read_socket_fds)) {
                handle_request(cur_fd);
            }
        }
    }

    return 0;
}



static void handle_msg(int fd, struct msg* m)
{
    switch (m->type) {
    case MSG_GET_HANDLE: {
        struct comp* c = (struct comp*) malloc(sizeof(struct comp));
        fprintf(stderr, "ILSRV: GetHandle '%s'\n", m->cmd.get_handle.name);
        m->result = OMX_GetHandle(&c->real_handle, m->cmd.get_handle.name, c, &callbacks);
        m->cmd.get_handle.out_srv_handle = (int) c;
        c->client_handle = m->cmd.get_handle.in_client_handle;
        c->client_fd     = fd;
        c->state         = OMX_StateLoaded;
        strncpy(c->name, m->cmd.get_handle.name, sizeof(c->name));
        c->name[sizeof(c->name)-1]=0;
        add_component(c);
        fprintf(stderr, "ILSRV: %08X [%20s] --> %s%s\n", c->real_handle, last_part(c->name), m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_SET_PARAM:{
        struct comp* c = (struct comp*) m->cmd.param.srv_handle;
        verify_component(c);
        fprintf(stderr, "ILSRV: %08X [%20s] SetParam %s\n", c->real_handle, last_part(c->name), adm_log_index2str(m->cmd.param.index));
        m->result = OMX_SetParameter(c->real_handle, (OMX_INDEXTYPE) m->cmd.param.index, m->cmd.param.buf); // TODO: Protect
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }
    case MSG_GET_PARAM:{
        struct comp* c = (struct comp*) m->cmd.param.srv_handle;
        verify_component(c);
        fprintf(stderr, "ILSRV: %08X [%20s] GetParam %s\n", c->real_handle, last_part(c->name), adm_log_index2str(m->cmd.param.index));
        m->result = OMX_GetParameter(c->real_handle, (OMX_INDEXTYPE)m->cmd.param.index, m->cmd.param.buf); // TODO: Protect
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_GET_CONFIG:{
        struct comp* c = (struct comp*) m->cmd.param.srv_handle;
        verify_component(c);
        fprintf(stderr, "ILSRV: %08X [%20s] GetConfig %s\n", c->real_handle, last_part(c->name), adm_log_index2str(m->cmd.param.index));
        m->result = OMX_GetConfig(c->real_handle, (OMX_INDEXTYPE)m->cmd.param.index, m->cmd.param.buf); // TODO: Protect
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_SET_CONFIG: {
        struct comp* c = (struct comp*) m->cmd.param.srv_handle;
        verify_component(c);
        fprintf(stderr, "ILSRV: %08X [%20s] SetConfig %s\n", c->real_handle, last_part(c->name), adm_log_index2str(m->cmd.param.index));
        m->result = OMX_SetConfig(c->real_handle, (OMX_INDEXTYPE)m->cmd.param.index, m->cmd.param.buf); // TODO: Protect
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_FREE_HANDLE: {
        struct comp* c = (struct comp*) m->cmd.free_handle.srv_handle;
        verify_component(c);
        if (c->state != OMX_StateLoaded) {
            fprintf(stderr, "ILSRV: FreeHandle called for %X which is in wrong state (%s)\m", c->real_handle, adm_log_state2str(c->state));
            error_found();
        }
        fprintf(stderr, "ILSRV: %08X [%20s] Freehandle (internal=%X)\n", c->real_handle, last_part(c->name), c);
        m->result = OMX_FreeHandle(c->real_handle);
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        del_component(c);
        memset(c, 0xFE, sizeof(*c));
        free(c);
        return;
    }

    case MSG_SEND_COMMAND: {
        struct comp* c = (struct comp*) m->cmd.free_handle.srv_handle;
        verify_component(c);
        if (m->cmd.command.cmd == OMX_CommandStateSet) {
            fprintf(stderr, "ILSRV: %08X [%20s] SendCommand StateSet %s\n", c->real_handle, last_part(c->name), adm_log_state2str((OMX_STATETYPE) m->cmd.command.nParam1));
        } else {
            fprintf(stderr, "ILSRV: %08X [%20s] SendCommand StateSet %s 0x%X\n", c->real_handle, last_part(c->name), adm_log_cmd2str(m->cmd.command.cmd), m->cmd.command.nParam1);
        }

        m->result = OMX_SendCommand(c->real_handle, m->cmd.command.cmd, m->cmd.command.nParam1, 0);
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_SETUP_TUNNEL: {
        struct comp* c_in = (struct comp*) m->cmd.tunnel.srv_handle_in;
        struct comp* c_out = (struct comp*) m->cmd.tunnel.srv_handle_out;
        verify_component(c_in);
        verify_component(c_out);

        fprintf(stderr, "ILSRV: SetupTunnel %08X [%20s].%d --> %08X [%20s].%d\n", c_out->real_handle, last_part(c_out->name), m->cmd.tunnel.port_out, c_in->real_handle, last_part(c_in->name), m->cmd.tunnel.port_in);
        m->result = OMX_SetupTunnel(c_out->real_handle, m->cmd.tunnel.port_out, c_in->real_handle, m->cmd.tunnel.port_in);
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_GET_STATE: {
        struct comp* c = (struct comp*) m->cmd.get_state.in_srv_handle;
        verify_component(c);
        fprintf(stderr, "ILSRV: %08X [%20s] GetState\n", c->real_handle, last_part(c->name));
        m->result = OMX_GetState(c->real_handle, &m->cmd.get_state.out_state);
        fprintf(stderr, "ILSRV: --> %s %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result), adm_log_state2str(m->cmd.get_state.out_state));
        return;
    }

    case MSG_ALLOC_BUFFER: {
        struct comp* c = (struct comp*) m->cmd.alloc_buffer.in_srv_handle;
        verify_component(c);

        OMX_BUFFERHEADERTYPE* bufhdr;
        fprintf(stderr, "ILSRV: %08X [%20s] AllocBuf port=%d size=%d\n", c->real_handle, last_part(c->name), m->cmd.alloc_buffer.port, m->cmd.alloc_buffer.size);
        m->result = OMX_AllocateBuffer(c->real_handle, &bufhdr, m->cmd.alloc_buffer.port,
                     (void*) m->cmd.alloc_buffer.in_client_buf_hdr, m->cmd.alloc_buffer.size);

        if (m->result == OMX_ErrorNone) {
            memcpy(&m->cmd.alloc_buffer.out_buf_hdr, bufhdr, sizeof(OMX_BUFFERHEADERTYPE));
            m->cmd.alloc_buffer.out_srv_buf_hdr = (int) bufhdr;
            add_bufhdr(bufhdr);
        }
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    case MSG_EMPTY_BUFFER: {
        struct comp* c = (struct comp*) m->cmd.empty_buffer.in_srv_handle;
        verify_component(c);

        OMX_BUFFERHEADERTYPE* client_bufhdr = (OMX_BUFFERHEADERTYPE*) &m->cmd.empty_buffer.in_buf_hdr;
        OMX_BUFFERHEADERTYPE* bufhdr = (OMX_BUFFERHEADERTYPE*) client_bufhdr->pPlatformPrivate;

        verify_bufhdr(bufhdr);

        if (bufhdr->nAllocLen <= sizeof(m->cmd.empty_buffer.in_buf)) {
            bufhdr->nOffset    = client_bufhdr->nOffset;
            bufhdr->nTimeStamp = client_bufhdr->nTimeStamp;
            bufhdr->nFilledLen = client_bufhdr->nFilledLen;
            bufhdr->nFlags     = client_bufhdr->nFlags;

            memcpy(bufhdr->pBuffer, &m->cmd.empty_buffer.in_buf, bufhdr->nAllocLen);
            fprintf(stderr, "ILSRV: %08X [%20s] EmptyBuffer port=%d size=%d\n", c->real_handle, last_part(c->name), bufhdr->nInputPortIndex, bufhdr->nAllocLen);
            m->result = OMX_EmptyThisBuffer(c->real_handle, bufhdr);
            fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        } else {
            assert(0);
        }

        return;
    }

    case MSG_FILL_BUFFER: {
        struct comp* c = (struct comp*) m->cmd.empty_buffer.in_srv_handle;
        verify_component(c);

        OMX_BUFFERHEADERTYPE* client_bufhdr = (OMX_BUFFERHEADERTYPE*) &m->cmd.fill_buffer.in_buf_hdr;
        OMX_BUFFERHEADERTYPE* bufhdr = (OMX_BUFFERHEADERTYPE*) client_bufhdr->pPlatformPrivate;

        verify_bufhdr(bufhdr);

        fprintf(stderr, "ILSRV: %08X [%20s] FillBuffer port=%d size=%d\n", c->real_handle, last_part(c->name), bufhdr->nOutputPortIndex, bufhdr->nAllocLen);
        m->result = OMX_FillThisBuffer(c->real_handle, bufhdr);
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));

        return;
    }

    case MSG_FREE_BUFFER: {
        struct comp* c = (struct comp*) m->cmd.empty_buffer.in_srv_handle;
        verify_component(c);

        OMX_BUFFERHEADERTYPE* bufhdr = (OMX_BUFFERHEADERTYPE*) m->cmd.free_buffer.in_srv_buf_hdr;
        verify_bufhdr(bufhdr);

        del_bufhdr(bufhdr);
        fprintf(stderr, "ILSRV: %08X [%20s] FreeBuffer\n", c->real_handle, last_part(c->name));
        m->result = OMX_FreeBuffer(c->real_handle, m->cmd.free_buffer.in_port, bufhdr);
        fprintf(stderr, "ILSRV: --> %s %s\n", m->result == OMX_ErrorNone ? "" : "ERROR ", adm_log_err2str(m->result));
        return;
    }

    default:
       fprintf(stderr, "ILSRV: Unhandled request received\n");
       m->result = OMX_ErrorNotImplemented;
       error_found();
       return;
    }
}

static void handle_request(int fd)
{
    pthread_mutex_lock(&il_srv_fd_mutex);
    struct msg m;
    int t=0;
    while(t < (int) sizeof(m)) {
        int r = recv(fd, ((char*) &m)+t, sizeof(m)-t, 0);
        if (r <= 0) {
            if (r == 0) {
                fprintf(stderr, "recv: Socket %d disconnected\n", fd);
            } else {
                fprintf(stderr, "recv: Socket %d no longer active (result:%d errno:'%s')\n", fd, r, strerror(errno));
            }

            fprintf(stderr, "IL_ILSRV: IL server halted\n");
            error_found();
        }
        t += r;
    }

    pthread_mutex_unlock(&il_srv_fd_mutex);
    handle_msg(fd, &m);
    pthread_mutex_lock(&il_srv_fd_mutex);

    t=0;
    while (t < sizeof(m)) {
        int w = send(fd, ((char*) &m)+t, sizeof(m)-t, MSG_NOSIGNAL);
        if (w == -1) {
            fprintf(stderr, "send: Socket %d not working (error: '%s')\n", fd, strerror(errno));
            fprintf(stderr, "IL_ILSRV: IL server halted\n");
            error_found();
        }

        t += w;
    }
    pthread_mutex_unlock(&il_srv_fd_mutex);


    struct timeval tv;
    gettimeofday(&tv, NULL);
    fprintf(stderr, "ILSRV: (%d.%d) Request completed, serving new calls. Tracking %d components. \n", tv.tv_sec, tv.tv_usec/1000, num_valid_comps);
}






