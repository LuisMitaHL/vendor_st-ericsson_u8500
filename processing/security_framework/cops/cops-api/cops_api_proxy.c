/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <r_psock.h>
#include <cops_api_proxy.h>
#include <cops_log.h>
#include <queue.h>

struct cops_api_proxy_client {
    union SIGNAL *signal;
    TAILQ_ENTRY(cops_api_proxy_client) link;
};

/*lint -e659 */
TAILQ_HEAD(cops_api_proxy_client_list, cops_api_proxy_client);
/*lint +e659 */

static void cops_api_proxy_handle(union SIGNAL *signal,
                                  cops_context_id_t *context);

OS_PROCESS(COPS_API_PROXY_Process)
{
    cops_context_id_t *context = NULL;
    bool ipc_ready = FALSE;
    struct cops_api_proxy_client_list client_list;

    /* At this point run_mode is not set so this log will get an own file */
    COPS_LOG(LOG_INFO, "Starting up\n");
    TAILQ_INIT(&client_list);

    while (TRUE) {
        static const SIGSELECT primitives[] = {0};
        union SIGNAL *signal = NIL;
        struct cops_api_proxy_client *client = client_list.tqh_first;

#ifdef COPS_IM_STUB_LEVEL_API_PROXY
        /* To be able to handle the request signals. */
        ipc_ready = TRUE;
#else
        /* Create client context when daemon is up and running. */
        if (TRUE == ipc_ready && NULL == context) {
            cops_api_proxy_startup(&context);

            if (NULL == context) {
                COPS_LOG(LOG_ERROR, "Fatal error, could not create context\n");
                return;
            }
        }
#endif
        /* Handle signals from internal queue first */
        if (TRUE == ipc_ready && NULL != client) {
            cops_api_proxy_handle(client->signal, context);
            TAILQ_REMOVE(&client_list, client, link);
            free(client);
        } else {
            /* TODO: Should we handle events first? */
            signal = RECEIVE(primitives);

            if (COPS_IPC_READY_PRIMITIVE == signal->Primitive) {
                ipc_ready = TRUE;
                SIGNAL_FREE(&signal);
            } else if (COPS_IPC_NOT_READY_PRIMITIVE == signal->Primitive) {
                ipc_ready = FALSE;
                SIGNAL_FREE(&signal);
            } else if (!ipc_ready) {
                struct cops_api_proxy_client *new_client = NULL;
                new_client = calloc(1, sizeof(*new_client));

                if (NULL == new_client) {
                    COPS_LOG(LOG_ERROR,
                             "Error, calloc failed! Cannot handle signal\n");
                    SIGNAL_FREE(&signal);
                    continue;
                }

                new_client->signal = signal;
                TAILQ_INSERT_TAIL(&client_list, new_client, link);
            } else {
                cops_api_proxy_handle(signal, context);
            }
        }
    }
}

static void cops_api_proxy_handle(union SIGNAL *signal,
                                  cops_context_id_t *context)
{
    if (EVENT_PSOCK_READYFORREAD == signal->sig_no) {
        EventStatus_t status;
        PSock_SocketFd_t sockfd;
        PSock_UserDataP_t userdata;

        /* Unpack the event signal into sock fd and user data */
        status = Event_PSock_ReadyForRead(signal, &sockfd, &userdata);

        if (GS_EVENT_OK == status) {

            if (NULL != context && sockfd == context->async_fd) {
                (void) cops_context_invoke_callback(context);
            }

        } else {
            COPS_LOG(LOG_ERROR, "Event_PSock_ReadyForRead() status not ok!\n");
        }
    } else {

        if (!PSock_HandleSignal(&signal)) {

            if (!cops_api_proxy_handle_signal(signal, context)) {
                char buf[50];
                (void) GET_PROCESS_NAME(SENDER(&signal), buf, 50);
                COPS_LOG(LOG_WARNING,
                        "Unhandled signal:0x%x from:%s\n", signal->sig_no, buf);
            }
        }
    }

    if (NIL != signal) {
        SIGNAL_FREE(&signal);
    }
}
