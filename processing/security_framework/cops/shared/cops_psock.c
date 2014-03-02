/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_common.h>
#include <cops_psock.h>
#include <cops_router.h>
#include <r_psock.h>
#include <s_psock.h>
#include <g_psock_psock.h>
#include <r_portmapper.h>
#include <r_os.h>
#include <r_serviceregister.h>
#include <r_icaif_utility_link.h>
#include <stdlib.h>

extern uint8_t run_mode;

static void cops_psock_trigger_ipc_state(void *aux);

/* PSock server callbacks */
static void cops_psock_server_accept_cb(const PSock_SocketFd_t sockfd,
                                        const PSock_UserDataP_t userdata);

static void cops_psock_server_shutdown_cb(const PSock_SocketFd_t sockfd,
                                          const PSock_UserDataP_t userdata,
                                          sint32 howto);

/* PSock client callbacks */
static void cops_psock_client_connect_cb(const PSock_SocketFd_t sockfd,
                                         const PSock_UserDataP_t userdata,
                                         const sint32 result);

static void cops_psock_client_close_cb(const PSock_SocketFd_t sockfd,
                                       const PSock_UserDataP_t userdata);

static void cops_psock_client_shutdown_cb(const PSock_SocketFd_t sockfd,
                                          const PSock_UserDataP_t userdata,
                                          sint32 howto);


int cops_psock_socket(bool server, struct cops_socket_data *data)
{
    PSock_SocketFd_t sockfd;
    PSock_Callbacks_t callbacks;
    uint32_t explicit_ready_for_read = 1;

    memset(&callbacks, 0, sizeof(callbacks));

    sockfd = PSock_Socket(PSOCK_AF_PSOCK, PSOCK_SOCK_PSOCK, PSOCK_PT_PSOCK);

    if (sockfd < 0) {
        COPS_LOG(LOG_ERROR, "PSock_Socket failed!\n");
        return -1;
    }

    if (server) {
        callbacks.AcceptCB = cops_psock_server_accept_cb;
        callbacks.ShutdownCB = cops_psock_server_shutdown_cb;
    } else {
        callbacks.ConnectCB = cops_psock_client_connect_cb;
        callbacks.CloseCB = cops_psock_client_close_cb;
        callbacks.ShutdownCB = cops_psock_client_shutdown_cb;
    }

    if (PSOCK_RESULT_OK != PSock_SetSockOpt(sockfd, PSOCK_SOL_SOCKET,
                                            PSOCK_SO_CALLBACKS, &callbacks,
                                            sizeof(callbacks))) {
        COPS_LOG(LOG_ERROR, "PSock_SetSockOpt failed!\n");
        (void)PSock_Close(sockfd);
        return -1;
    }

    if (PSOCK_RESULT_OK != PSock_SetSockOpt(sockfd, PSOCK_SOL_SOCKET,
                                            PSOCK_SO_EXPLICIT_READY_FOR_READ,
                                            &explicit_ready_for_read,
                                            sizeof(explicit_ready_for_read))) {
        COPS_LOG(LOG_ERROR, "PSock_SetSockOpt failed!\n");
        (void)PSock_Close(sockfd);
        return -1;
    }

    if (data != NULL &&
        PSOCK_RESULT_OK != PSock_SetSockOpt(sockfd,
                                            PSOCK_SOL_SOCKET,
                                            PSOCK_SO_USERDATA,
                                            &data, sizeof(data))) {
        COPS_LOG(LOG_ERROR, "PSock_SetSockOpt failed!\n");
        (void)PSock_Close(sockfd);
        return -1;
    }

    return sockfd;
}

int cops_psock_bind(int sockfd, const char *path)
{
    PSock_SockaddrPSock_t sockaddr;
    PSock_SockaddrPSock_t localaddr;
    PSock_SockLen_t adresslength;
    Portmapper_Result_t result;

    sockaddr.PSock_Family = PSOCK_AF_PSOCK;
    sockaddr.PSock_Addr = PSOCK_PSOCKADDR_ANY;
    sockaddr.PSock_Port = PSOCK_PORTADDR_ANY;

    if (PSOCK_RESULT_OK != PSock_Bind(sockfd,
                                      (struct PSock_Sockaddr_t *)&sockaddr,
                                      sizeof(sockaddr))) {
        COPS_LOG(LOG_ERROR, "PSock_Bind failed!\n");
        return -1;
    }

    memset(&localaddr, 0, sizeof(localaddr));
    adresslength = sizeof(localaddr);

    if (PSOCK_RESULT_OK != PSock_GetSockName(sockfd,
                                          (struct PSock_Sockaddr_t *)&localaddr,
                                          &adresslength)) {
        COPS_LOG(LOG_ERROR, "PSock_GetSockName failed!\n");
        return -1;
    }

    if (REQUEST_OK != Request_PortMapper_RegisterServer(WAIT_RESPONSE,
                                                        (uint8 *)path,
                                                        &localaddr,
                                                        &result)) {
        COPS_LOG(LOG_ERROR, "Request_PortMapper_RegisterServer failed!\n");
        return -1;
    }

    if (PORTMAPPER_RESULT_OK != result) {
        COPS_LOG(LOG_ERROR, "Portmapper result=%d\n", result);
        return -1;
    }

    return 0;
}

int cops_psock_listen(int sockfd, int backlog)
{
    if (PSOCK_RESULT_OK != PSock_Listen(sockfd, backlog)) {
        COPS_LOG(LOG_ERROR, "PSock_Listen failed!\n");
        return -1;
    }

    return 0;
}

int cops_psock_accept(int sockfd)
{
    int fd = -1;
    struct cops_socket_data *data = NULL;
    PSock_SockLen_t socklen = sizeof(data);

    if (PSOCK_RESULT_OK != PSock_GetSockOpt(sockfd, PSOCK_SOL_SOCKET,
                                            PSOCK_SO_USERDATA, &data,
                                            &socklen)) {
        COPS_LOG(LOG_ERROR, "PSock_GetSockOpt failed!\n");
        return -1;
    }

    /* We have received an accept callback and saved the fd */
    if (NULL != data) {
        fd = data->request_fd;
        data->request_fd = -1;
    }

    return fd;
}

int cops_psock_connect(int sockfd, const char *path)
{
    PSock_SockaddrPSock_t sockaddr;
    Portmapper_Result_t result;
    char *local_path = NULL;
    int ret_val = -1; /* failure */

    local_path = (char *)malloc(strlen(path) + 1);

    if (NULL == local_path) {
        COPS_LOG(LOG_ERROR, "malloc failed!\n");
        return ret_val;
    }

    strcpy(local_path, path);

    if (0 == strcmp(path, STR(SIPC_CLIENT)) &&
        (COPS_RUN_MODE_RELAY == run_mode || COPS_RUN_MODE_DAEMON == run_mode)) {
        char *temp = NULL;
        UtilityLink_Result_t ul_result;

        if (REQUEST_OK != R_Req_iCaif_RegisterRemoteServer(
                                              REQUEST_CONTROL_BLOCK_WAIT_MODE_p,
                                              (uint8 *)path, &ul_result)) {
            goto function_exit;
        }

        if (UTILITY_LINK_RESULT_OK != ul_result &&
            UTILITY_LINK_RESULT_FAILED_REGISTER_NAME_PRESENT != ul_result) {
            goto function_exit;
        }

        temp = (char *)realloc(local_path, strlen(path) + 3);

        if (NULL == temp) {
            COPS_LOG(LOG_ERROR, "relloc failed!\n");
            goto function_exit;
        }

        local_path = temp;
        strcat(local_path, "_L");
    }

    memset(&sockaddr, 0, sizeof(sockaddr));

    if (REQUEST_OK != Request_PortMapper_GetServerByName(WAIT_RESPONSE,
                                                         (uint8 *)local_path,
                                                         &sockaddr,
                                                         &result)) {
        COPS_LOG(LOG_ERROR, "Request_PortMapper_GetServerByName failed!\n");
        goto function_exit;
    }

    if (PORTMAPPER_RESULT_OK != result) {
        COPS_LOG(LOG_ERROR, "Portmapper result=%d\n", result);
        goto function_exit;
    }

    if (PSOCK_RESULT_OK != PSock_Connect(sockfd,
                                         (struct PSock_Sockaddr_t *)&sockaddr,
                                         sizeof(sockaddr))) {
        COPS_LOG(LOG_ERROR, "PSock_Connect failed!\n");
        goto function_exit;
    }

    ret_val = 0; /* success */

function_exit:
    /*lint -e449
     * Suppress "previously deallocated" since the buffer local_path
     * is not always deallocated when we are here */
    if (NULL != local_path) {
        free(local_path);
    }
    /*lint +e449 */

    return ret_val;
}

int cops_psock_select(int ntds, fd_set *readfds, fd_set *writefds,
                      fd_set *exceptfds, const struct timeval *timeout)
{
    int fds_ready = 0;
    union SIGNAL *csignal = NIL;
    Event_PSock_ReadyForRead_t *async = NIL;
    static const SIGSELECT primitives[] = {11,
                                           EVENT_PSOCK_CLOSED,
                                           EVENT_PSOCK_READYFORWRITE,
                                           EVENT_PSOCK_READYFORREAD,
                                           DO_PSOCK_CONNECT,
                                           DONE_PSOCK_CONNECT,
                                           EVENT_PSOCK_PEER_CONTROL,
                                           DO_PSOCK_RESET,
                                           DO_PSOCK_DATA_WRITE,
                                           EVENT_PSOCK_CONNECT_TIMEOUT,
                                           EVENT_PSOCK_ACCEPT_TIMEOUT,
                                           EVENT_PSOCK_SEND_READ_READY};

sagain:
    do {
        if (NULL != timeout) {
            csignal = RECEIVE_W_TMO((timeout->tv_sec * 1000) +
                                   (timeout->tv_usec / 1000), primitives);

            if (NIL == csignal) {
                COPS_LOG(LOG_WARNING, "RECEIVE_W_TMO timed out\n");
                FD_ZERO(readfds);
                goto function_exit;
            }
        } else {
            csignal = RECEIVE(primitives);
        }

        /*
         * Let psock handle all internal signals
         * If the the signal is handled then it is FREEed, i.e. NIL
         */
        (void)PSock_HandleSignal(&csignal);

    /* Continue as long as we have not received a valid signal */
    } while (csignal == NIL);


    if (EVENT_PSOCK_READYFORREAD == csignal->sig_no ||
        EVENT_PSOCK_READYFORWRITE == csignal->sig_no ||
        EVENT_PSOCK_CLOSED == csignal->sig_no) {
        EventStatus_t status;
        PSock_SocketFd_t sockfd;
        PSock_UserDataP_t userdata;

        /*
         * Unpack the event signal into sock fd and user data
         * We use Event_PSock_ReadyForRead for all events since
         * they are identical
         */
        status = Event_PSock_ReadyForRead(csignal, &sockfd, &userdata);

        if (GS_EVENT_OK == status) {
            struct cops_socket_data *data = (struct cops_socket_data *)userdata;

            if (EVENT_PSOCK_READYFORWRITE == csignal->sig_no) {
                if (NULL != data) {
                    /*
                     * We have received an accept cb but we cant add
                     * the new fd to readfds here so set sockfd to listen_fd
                     * and it will be added through cops_accept
                     */
                    if (-1 != data->listen_fd) {
                        sockfd = data->listen_fd;
                        /* IPC is ready */
                        if (COPS_SOCKET_STATE_READY == data->state) {
                            cops_psock_trigger_ipc_state(data->aux);
                        }
                    }
                    /*
                     * "Client" has connected to "server"
                     * We should not do anything, so we set sockfd to FD_SETSIZE
                     * which will clear readfds below
                     */
                    else if (sockfd == data->request_fd) {
                        /*
                         * Set this to the last element in the array.
                         */
                        sockfd = (PSock_SocketFd_t)(FD_SETSIZE - 1);
                    }
                }
            } else if (EVENT_PSOCK_READYFORREAD == csignal->sig_no) {
                if (!FD_ISSET(sockfd, readfds)) {
                    COPS_LOG(LOG_INFO, "EVENT_PSOCK_READYFORREAD async fd\n");
                    async = SIGNAL_ALLOC(Event_PSock_ReadyForRead_t,
                                         EVENT_PSOCK_READYFORREAD);
                    memcpy(async, csignal, sizeof(Event_PSock_ReadyForRead_t));
                }
            } else if (EVENT_PSOCK_CLOSED == csignal->sig_no) {
                if (NULL != data && COPS_SOCKET_STATE_UNUSED != data->state) {
                    data->state = COPS_SOCKET_STATE_WAITING;
                    /* IPC is not ready */
                    cops_psock_trigger_ipc_state(data->aux);
                }

                /*
                 * When a client has disconnected we should not clear fd_set
                 * The next failed read in api_server will remove the client!!
                 */
            }

            /* Set/clear fd_set depending if sockfd is already set in fd_set */
            /*lint -e661 */
            if (NULL != readfds) {
                if (FD_ISSET(sockfd, readfds)) {
                    FD_ZERO(readfds);
                    FD_SET(sockfd, readfds);
                    fds_ready++;
                } else {
                    COPS_LOG(LOG_INFO, "goto sagain\n");
                    SIGNAL_FREE(&csignal);
                    goto sagain;
                }
            }
            /*lint +e661 */
        } else {
            COPS_LOG(LOG_ERROR, "Event_PSock_ReadyForRead() status not ok! "
                     "Returned error no: 0x%x\n", PSock_Errno());
        }
    } else {
        COPS_LOG(LOG_ERROR, "This should never happen, all internal "
                 "psock signals should be handled in PSock_HandleSignal()\n");
        fds_ready = -1;
    }

function_exit:
    if (NIL != csignal) {
        SIGNAL_FREE(&csignal);
    }

    if (NIL != async) {
        COPS_LOG(LOG_INFO, "Resending async EVENT_PSOCK_READYFORREAD\n");
        SEND(&async, CURRENT_PROC());
    }

    return fds_ready;
}

int cops_psock_close(int fd)
{
    if (PSOCK_RESULT_OK != PSock_Close(fd)) {
        COPS_LOG(LOG_ERROR, "PSock_Close failed!\n");
        return -1;
    }

    return 0;
}

ssize_t cops_psock_write(int fd, const void *buf, size_t count)
{
    PSock_SockLen_t optlen;
    PSock_SData_t *buffer;
    uint16_t min_tx_trail;
    uint16_t min_tx_offset;
    ssize_t result;

    optlen = sizeof(uint16_t);
    if (PSock_GetSockOpt(fd, PSOCK_SOL_SOCKET, PSOCK_SO_MIN_TX_OFFSET,
                         &min_tx_offset, &optlen) < 0) {
        COPS_LOG(LOG_ERROR, "PSock_GetSockOpt failed!\n");
        return -1;
    }

    optlen = sizeof(uint16_t);
    if (PSock_GetSockOpt(fd, PSOCK_SOL_SOCKET, PSOCK_SO_MIN_TX_TRAIL,
                         &min_tx_trail, &optlen) < 0) {
        COPS_LOG(LOG_ERROR, "PSock_GetSockOpt failed!\n");
        return -1;
    }

    buffer = PSock_SData_Alloc(count + min_tx_offset + min_tx_trail);

    if (NULL == buffer) {
        COPS_LOG(LOG_ERROR, "PSock_SData_Alloc failed!\n");
        return -1;
    }

    PSOCK_SDATA_SET_DATA_OFFSET(buffer, min_tx_offset);
    PSOCK_SDATA_SET_DATA_LENGTH(buffer, count);
    memcpy(PSOCK_SDATA_GET_DATA_START(buffer), buf,
           PSOCK_SDATA_GET_DATA_LENGTH(buffer));

    result = PSock_Write(fd, (void **)&buffer,
                         PSOCK_SDATA_GET_BUFFER_SIZE(buffer));

    if (PSOCK_RESULT_FAILED == result) {
        COPS_LOG(LOG_ERROR, "PSock_Write failed %s!\n",
                 PSock_Strerror(PSock_Errno()));
        return -1;
    }

    return result - (min_tx_offset + min_tx_trail);
}

ssize_t cops_psock_read(int fd, void *buf, size_t count)
{
    PSock_SData_t *psock_data;
    ssize_t result;

    result = PSock_Read(fd, (void **)&psock_data, 0);

    if (PSOCK_RESULT_FAILED == result) {
        COPS_LOG(LOG_ERROR, "PSock_Read failed!\n");
        return -1;
    }

    result = PSOCK_SDATA_GET_DATA_LENGTH(psock_data);
    memcpy(buf, PSOCK_SDATA_GET_DATA_START(psock_data),
           MIN(count, (size_t)result));
    PSOCK_SDATA_FREE(&psock_data);

    return MIN(count, (size_t)result);
}

int cops_psock_errno(void)
{
    return PSock_Errno();
}

const unsigned char *cops_psock_strerror(int errnum)
{
    return PSock_Strerror(errnum);
}

int cops_psock_error_is_eagain(int errnum)
{
    if (PSOCK_ERROR_EAGAIN == errnum ||
        PSOCK_ERROR_EWOULDBLOCK == errnum) {
        return 1;
    }
    return 0;
}

static void cops_psock_trigger_ipc_state(void *aux)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sipc_message_t *msg = NULL;

    COPS_CHK_RC(cops_sipc_alloc_message(ret_code, 0,
                                        COPS_SIPC_IPC_STATE_REQ,
                                        COPS_SENDER_UNSECURE,
                                        &msg));

    COPS_LOG(LOG_INFO, "Triggering SIPC. Type = 0x%x, length = %u\n",
             msg->msg_type, msg->length);

    COPS_CHK_RC(cops_router_handle_msg(aux, NO_FD, msg, msg));

function_exit:
    cops_sipc_free_message(&msg);
}

/* PSock server callbacks */
static void cops_psock_server_accept_cb(const PSock_SocketFd_t sockfd,
                                        const PSock_UserDataP_t userdata)
{
    struct cops_socket_data *data;
    PSock_SockaddrPSock_t sockaddr;
    PSock_SockLen_t socklen = sizeof(PSock_SockaddrPSock_t);
    int fd;

    COPS_LOG(LOG_INFO, "cops_psock_server_accept_cb\n");

    if (NULL == userdata) {
        COPS_LOG(LOG_ERROR, "Error: UserData cannot be null!\n");
        return;
    }

    data = (struct cops_socket_data *)userdata;

    fd = PSock_Accept(sockfd, (struct PSock_Sockaddr_t *)&sockaddr, &socklen);

    if (-1 == fd) {
        COPS_LOG(LOG_ERROR, "PSock_Accept failed!\n");
    }

    data->request_fd = fd;

    if (COPS_SOCKET_STATE_INITIATED == data->state) {
        data->state = COPS_SOCKET_STATE_READY;
    }
}

static void cops_psock_server_shutdown_cb(const PSock_SocketFd_t sockfd,
                                          const PSock_UserDataP_t userdata,
                                          sint32 howto)
{
    (void) userdata;
    (void) howto;

    COPS_LOG(LOG_INFO, "cops_psock_server_shutdown_cb\n");

    (void)PSock_Close(sockfd);
}

/* PSock client callbacks */
static void cops_psock_client_connect_cb(const PSock_SocketFd_t sockfd,
                                         const PSock_UserDataP_t userdata,
                                         const sint32 result)
{
    (void) sockfd;
    (void) result;

    COPS_LOG(LOG_INFO, "cops_psock_client_connect_cb\n");

    if (NULL != userdata) {
        struct cops_socket_data *data;
        data = (struct cops_socket_data *)userdata;

        if (COPS_SOCKET_STATE_INITIATED == data->state) {
            data->state = COPS_SOCKET_STATE_READY;
        }
    }
}

static void cops_psock_client_close_cb(const PSock_SocketFd_t sockfd,
                                       const PSock_UserDataP_t userdata)
{
    (void) sockfd;

    COPS_LOG(LOG_INFO, "cops_psock_client_close_cb\n");

    if (NULL != userdata) {
        struct cops_socket_data *data;
        data = (struct cops_socket_data *)userdata;

        if (COPS_SOCKET_STATE_UNUSED != data->state) {
            data->state = COPS_SOCKET_STATE_WAITING;
        }
    }
}

static void cops_psock_client_shutdown_cb(const PSock_SocketFd_t sockfd,
                                          const PSock_UserDataP_t userdata,
                                          sint32 howto)
{
    (void) sockfd;
    (void) userdata;
    (void) howto;

    COPS_LOG(LOG_INFO, "cops_psock_client_shutdown_cb\n");

    (void)PSock_Close(sockfd);
}
