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
#include "ste_il_server.h"
#include <pthread.h>
#include <assert.h>

#ifndef STE_ADM_EXPORT
  #define STE_ADM_EXPORT __attribute__((visibility("default")))
#endif

struct client_comp
{
    int              srv_handle;
    OMX_CALLBACKTYPE cb;
    OMX_PTR          pAppData;
    OMX_HANDLETYPE   comp_h;
};


int il_server_fd;
pthread_mutex_t il_server_fd_mutex = PTHREAD_MUTEX_INITIALIZER;

void dispatch_msg(struct msg* m)
{
    switch (m->type) {
        case MSG_EVENT: {
            struct client_comp* c = (struct client_comp*) m->cmd.event.client_handle;
            if (c->cb.EventHandler) {
                c->cb.EventHandler(c->comp_h, c->pAppData,
                                   m->cmd.event.ev,
                                   m->cmd.event.nData1,
                                   m->cmd.event.nData2,
                                   0);
            }
           return;
        }

        case MSG_EMPTY_BUFFER_DONE: {
            struct client_comp* c = (struct client_comp*) m->cmd.empty_buffer_done.client_handle;
            OMX_BUFFERHEADERTYPE* bufhdr = (OMX_BUFFERHEADERTYPE*) m->cmd.empty_buffer_done.client_bufhdr;
            if (c->cb.EmptyBufferDone) {
                c->cb.EmptyBufferDone(c->comp_h, c->pAppData, bufhdr);
            }
            return;
        }

        case MSG_FILL_BUFFER_DONE: {
            struct client_comp* c = (struct client_comp*) m->cmd.fill_buffer_done.client_handle;
            OMX_BUFFERHEADERTYPE* bufhdr = (OMX_BUFFERHEADERTYPE*) m->cmd.fill_buffer_done.client_bufhdr;

            bufhdr->nOffset    = m->cmd.fill_buffer_done.out_buf_hdr.nOffset;
            bufhdr->nTimeStamp = m->cmd.fill_buffer_done.out_buf_hdr.nTimeStamp;
            bufhdr->nFilledLen = m->cmd.fill_buffer_done.out_buf_hdr.nFilledLen;
            bufhdr->nFlags     = m->cmd.fill_buffer_done.out_buf_hdr.nFlags;

            if (bufhdr->nAllocLen <= sizeof(m->cmd.fill_buffer_done.out_buf)) {
                memcpy(bufhdr->pBuffer, m->cmd.fill_buffer_done.out_buf, bufhdr->nAllocLen);
            }

            if (c->cb.FillBufferDone) {
                c->cb.FillBufferDone(c->comp_h, c->pAppData, bufhdr);
            }
            return;
        }

        default:
        {
            // This would be bad, crash so it is found
            int*a = 0;
            *a=0;
        }
    }
}


void* il_callback_thread(void*dummy)
{
    fd_set fds;
    while(1) {
        struct msg m;

        FD_ZERO(&fds);
        FD_SET(il_server_fd, &fds);
        select(il_server_fd+1, &fds, NULL, NULL, NULL);

        pthread_mutex_lock(&il_server_fd_mutex);
            struct timeval tv;
            tv.tv_sec = tv.tv_usec = 0; // poll (somebody might have managed to sneak in a read)
            if (select(il_server_fd+1, &fds, NULL, NULL, &tv) == 1)
            {
                int t=0;
                while (t < sizeof(struct msg)) {
                    int w = recv(il_server_fd, ((char*)&m)+t, sizeof(struct msg)-t, 0);
                    if (w <= 0) {
                        printf("recv() failed for IL server, surrendering, %d\n", __LINE__);
                        exit(20);
                    }
                    t += w;
                }
                dispatch_msg(&m);
            }
        pthread_mutex_unlock(&il_server_fd_mutex);
    }
}





static void exchange(void* m)
{
    int size=sizeof(struct msg);
    int t=0;

    pthread_mutex_lock(&il_server_fd_mutex);

    while (t < sizeof(struct msg)) {
        int w = send(il_server_fd, ((char*)m)+t, sizeof(struct msg)-t, MSG_NOSIGNAL);
        if (w <= 0) {
            printf("send() failed for IL server, surrendering, %d\n", __LINE__);
            exit(20);
        }
        t += w;
    }

    while(1) {
        t=0;
        while (t < sizeof(struct msg)) {
            int w = recv(il_server_fd, ((char*)m)+t, sizeof(struct msg)-t, 0);
            if (w <= 0) {
                printf("recv() failed for IL server, surrendering %d\n", __LINE__);
                exit(20);
            }
            t += w;
        }

        // or bufferr
        if ( ((struct msg*) m)->type == MSG_EVENT ||
             ((struct msg*) m)->type == MSG_EMPTY_BUFFER_DONE ||
             ((struct msg*) m)->type == MSG_FILL_BUFFER_DONE) {
            dispatch_msg((struct msg*)m);
        } else {
            pthread_mutex_unlock(&il_server_fd_mutex);
            return;
        }
    }
}

void __attribute__ ((constructor)) my_init(void)
{
    printf("IL client library initializing...\n");

    il_server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un remote_sockaddr_un;
    remote_sockaddr_un.sun_family = AF_UNIX;
    strcpy(remote_sockaddr_un.sun_path, "/tmp/il-server-socket");

    while (1 ){
        /* try to connect our socket to the servers socket */
        if (connect(il_server_fd, (struct sockaddr *) &remote_sockaddr_un,
                sizeof(remote_sockaddr_un)) == -1) {
            printf("IL client: connect() %s\n", strerror(errno));
            printf("IL client: Sleep for 1 second and retry\n");
            sleep(1);
        } else {
            printf("IL client: successfully connected, fd=%d\n", il_server_fd);
            break;
        }
    }

    pthread_t thread;
    pthread_create(&thread, NULL, il_callback_thread, NULL);
    printf("Spawned IL callback thread: %X\n", thread);
}

void __attribute__ ((destructor)) my_fini(void)
{
    printf("IL client library destroyed\n");
}


OMX_API STE_ADM_EXPORT OMX_ERRORTYPE OMX_APIENTRY OMX_Init(void)
{
    return OMX_ErrorNone;
}



static OMX_ERRORTYPE proxy_SendCommand(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_U32 nParam1,
            OMX_IN  OMX_PTR pCmdData)
{
    OMX_COMPONENTTYPE* omx_comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*       c = (struct client_comp*) omx_comp->pComponentPrivate;

    if (pCmdData != 0) {
        return OMX_ErrorNotImplemented;
    }

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_SEND_COMMAND;
    m.cmd.command.srv_handle = c->srv_handle;
    m.cmd.command.cmd        = Cmd;
    m.cmd.command.nParam1    = nParam1;
    exchange(&m);
    return m.result;
}

static OMX_ERRORTYPE param_exchange(enum msg_type msg_type, OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR pComponentParameterStructure)
{
    OMX_COMPONENTTYPE* omx_comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*       c = (struct client_comp*) omx_comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = msg_type;
    m.cmd.param.srv_handle = c->srv_handle;
    m.cmd.param.index = nParamIndex;


    unsigned int in_size = *((int*) pComponentParameterStructure);
    if (in_size > sizeof(m.cmd.param.buf)) {
        printf("Param too big, %d\n", in_size);
        /* if (msg_type == MSG_GET_PARAM || msg_type == MSG_GET_CONFIG) {
            // Client doesn't always initialize size
        } */

        return OMX_ErrorVendorStartUnused;
    } else if (in_size <= 8) {
        printf("Warning: param/config size suspiciously small (%d, index = %X)\n", in_size, nParamIndex);
        assert(0);
    }
    memcpy(m.cmd.param.buf, pComponentParameterStructure, in_size);
    exchange(&m);

    if (m.result == OMX_ErrorNone) {
        unsigned int out_size = *((unsigned int*) m.cmd.param.buf);
        if (out_size <= sizeof(m.cmd.param.buf)) {
            memcpy(pComponentParameterStructure, m.cmd.param.buf, out_size);
        } else {
            printf("Param/Config size too large (%u bytes)\n", out_size);
            assert(0);
        }

    }

    return m.result;
}

static OMX_ERRORTYPE proxy_GetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR pComponentParameterStructure)
{
    return param_exchange(MSG_GET_PARAM, hComponent, nParamIndex, pComponentParameterStructure);
}


static OMX_ERRORTYPE proxy_SetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_IN  OMX_PTR pComponentParameterStructure)
{
    return param_exchange(MSG_SET_PARAM, hComponent, nIndex, pComponentParameterStructure);
}

static OMX_ERRORTYPE proxy_GetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure)
{
    return param_exchange(MSG_GET_CONFIG, hComponent, nIndex, pComponentConfigStructure);
}

static OMX_ERRORTYPE proxy_SetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_IN  OMX_PTR pComponentConfigStructure)
{
    return param_exchange(MSG_SET_CONFIG, hComponent, nIndex, pComponentConfigStructure);
}

static OMX_ERRORTYPE proxy_GetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    return OMX_ErrorNotImplemented;
}

static OMX_ERRORTYPE proxy_GetState(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState)
{
    OMX_COMPONENTTYPE* comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*   c = (struct client_comp*) comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_GET_STATE;
    m.cmd.get_state.in_srv_handle = c->srv_handle;

    exchange(&m);
    if (m.result == OMX_ErrorNone) {
        *pState = m.cmd.get_state.out_state;
    }

    return m.result;
}

static OMX_ERRORTYPE proxy_UseBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer)
{
    return OMX_ErrorNotImplemented;
}

static OMX_ERRORTYPE proxy_AllocateBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes)
{
    OMX_COMPONENTTYPE* comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*   c = (struct client_comp*) comp->pComponentPrivate;

    OMX_BUFFERHEADERTYPE* bufhdr = (OMX_BUFFERHEADERTYPE*) malloc(sizeof(OMX_BUFFERHEADERTYPE));

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_ALLOC_BUFFER;
    m.cmd.alloc_buffer.in_srv_handle = c->srv_handle;
    m.cmd.alloc_buffer.port          = nPortIndex;
    m.cmd.alloc_buffer.size          = nSizeBytes;
    m.cmd.alloc_buffer.in_client_buf_hdr = (int) bufhdr;
    exchange(&m);

    memcpy(bufhdr, &m.cmd.alloc_buffer.out_buf_hdr, sizeof(OMX_BUFFERHEADERTYPE));

    bufhdr->pBuffer = (OMX_U8*) malloc(bufhdr->nAllocLen);
    bufhdr->pPlatformPrivate = (void*) m.cmd.alloc_buffer.out_srv_buf_hdr;
    bufhdr->pAppPrivate = pAppPrivate;
    *ppBuffer = bufhdr;
    return m.result;
}

static OMX_ERRORTYPE proxy_FreeBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* bufhdr)
{
    OMX_COMPONENTTYPE* comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*   c = (struct client_comp*) comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_FREE_BUFFER;
    m.cmd.free_buffer.in_srv_handle  = c->srv_handle;
    m.cmd.free_buffer.in_srv_buf_hdr = (int) bufhdr->pPlatformPrivate;
    m.cmd.free_buffer.in_port        = nPortIndex;
    exchange(&m);

    if (m.result == OMX_ErrorNone) {
        free(bufhdr->pBuffer);
        free(bufhdr);
    }

    return m.result;
}

static OMX_ERRORTYPE proxy_EmptyThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* bufhdr)
{
    OMX_COMPONENTTYPE* comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*   c = (struct client_comp*) comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_EMPTY_BUFFER;
    m.cmd.empty_buffer.in_srv_handle     = c->srv_handle;
    memcpy(&m.cmd.empty_buffer.in_buf_hdr, bufhdr, sizeof(OMX_BUFFERHEADERTYPE)); // includes bufhdr->pPlatformPrivate == srv_buf_hdr
    if (bufhdr->nAllocLen <= sizeof(m.cmd.empty_buffer.in_buf)) {
        memcpy(m.cmd.empty_buffer.in_buf, bufhdr->pBuffer, bufhdr->nAllocLen);
    } else {
        assert(0);
    }
    exchange(&m);

    return m.result;
}

static OMX_ERRORTYPE proxy_FillThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    OMX_COMPONENTTYPE* comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*   c = (struct client_comp*) comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_FILL_BUFFER;
    m.cmd.fill_buffer.in_srv_handle     = c->srv_handle;
    memcpy(&m.cmd.fill_buffer.in_buf_hdr, pBuffer, sizeof(OMX_BUFFERHEADERTYPE));

    exchange(&m);
    return m.result;
}

static OMX_ERRORTYPE proxy_UseEGLImage(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN void* eglImage)
{
    return OMX_ErrorNotImplemented;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY STE_ADM_EXPORT OMX_GetHandle(
    OMX_OUT OMX_HANDLETYPE* pHandle,
    OMX_IN  OMX_STRING cComponentName,
    OMX_IN  OMX_PTR pAppData,
    OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
{
    printf("IL proxy: OMX_GetHandle %s\n", cComponentName);
    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_GET_HANDLE;
    strncpy(m.cmd.get_handle.name, cComponentName, sizeof(m.cmd.get_handle.name));
    m.cmd.get_handle.name[sizeof(m.cmd.get_handle.name)-1]=0;


    struct client_comp*       c = (struct client_comp*) malloc(sizeof(client_comp));
    m.cmd.get_handle.in_client_handle = (int) c;
    exchange(&m);

    OMX_COMPONENTTYPE* comp_h = (OMX_COMPONENTTYPE*) malloc(sizeof(OMX_COMPONENTTYPE));
    comp_h->pApplicationPrivate = pAppData;
    comp_h->pComponentPrivate = c;
    comp_h->SendCommand = proxy_SendCommand;
    comp_h->GetParameter = proxy_GetParameter;
    comp_h->SetParameter = proxy_SetParameter;
    comp_h->GetConfig = proxy_GetConfig;
    comp_h->SetConfig = proxy_SetConfig;
    comp_h->GetExtensionIndex = proxy_GetExtensionIndex;
    comp_h->GetState = proxy_GetState;
    comp_h->UseBuffer = proxy_UseBuffer;
    comp_h->AllocateBuffer = proxy_AllocateBuffer;
    comp_h->FreeBuffer = proxy_FreeBuffer;
    comp_h->EmptyThisBuffer = proxy_EmptyThisBuffer;
    comp_h->FillThisBuffer = proxy_FillThisBuffer;
    comp_h->UseEGLImage = proxy_UseEGLImage;


    c->cb       = *pCallBacks;
    c->pAppData = pAppData;
    c->srv_handle = m.cmd.get_handle.out_srv_handle;
    c->comp_h   = comp_h;


    *pHandle = (OMX_HANDLETYPE) comp_h;
    return m.result;
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY STE_ADM_EXPORT OMX_FreeHandle(
    OMX_IN  OMX_HANDLETYPE hComponent)
{
    OMX_COMPONENTTYPE* omx_comp = (OMX_COMPONENTTYPE*) hComponent;
    struct client_comp*       c = (struct client_comp*) omx_comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_FREE_HANDLE;
    m.cmd.free_handle.srv_handle = c->srv_handle;
    exchange(&m);
    if (m.result == OMX_ErrorNone) {
        memset(c, 0xCD, sizeof(*c));
        free(c);

        memset(hComponent, 0xCD, sizeof(OMX_COMPONENTTYPE));
        free(hComponent);
        printf("FreeHandle: private=%X comp=%X\n", c, hComponent);
    }
    return m.result;
}


OMX_API OMX_ERRORTYPE OMX_APIENTRY STE_ADM_EXPORT OMX_SetupTunnel(
    OMX_IN  OMX_HANDLETYPE hOutput,
    OMX_IN  OMX_U32 nPortOutput,
    OMX_IN  OMX_HANDLETYPE hInput,
    OMX_IN  OMX_U32 nPortInput)
{
    printf("IL proxy: OMX_SetupTunnel %X.%u --> %X.%u\n", (int) hOutput, nPortOutput, (int) hInput, nPortInput);

    OMX_COMPONENTTYPE* out_comp = (OMX_COMPONENTTYPE*) hOutput;
    struct client_comp*       c_out = (struct client_comp*) out_comp->pComponentPrivate;
    OMX_COMPONENTTYPE* in_comp = (OMX_COMPONENTTYPE*) hInput;
    struct client_comp*       c_in = (struct client_comp*) in_comp->pComponentPrivate;

    struct msg m;
    memset(&m,0,sizeof(m));
    m.type = MSG_SETUP_TUNNEL;
    m.cmd.tunnel.srv_handle_in =  c_in->srv_handle;
    m.cmd.tunnel.srv_handle_out = c_out->srv_handle;
    m.cmd.tunnel.port_in = nPortInput;
    m.cmd.tunnel.port_out = nPortOutput;

    exchange(&m);
    return m.result;
}





