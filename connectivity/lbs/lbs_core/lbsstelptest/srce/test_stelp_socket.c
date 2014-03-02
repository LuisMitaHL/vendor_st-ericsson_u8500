/*
 * STELP Socket Manager
 *
 * test_engine.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#include <test_stelp_socket.h>

#include <agpsosa.h>


/*
 * Client linked list struct
 */
typedef struct LBSSTELP_socket_client_s {
    int fd;
    t_OsaThreadHandle thread;
    TSTENG_handle_t handle;
    TSTENG_command_item_t *cmd_p;
    struct TSTENG_client_s *next;
} TSTENG_client_t;


static void *LBSSTELP_socket_server_thread(void *args);
static void *LBSSTELP_socket_client_thread(void *args);
//static void LBSSTELP_socket_handle_new_message(const TSTENG_socket_msg_t * msg, const TSTENG_client_t * client);
//static void LBSSTELP_socket_remove_client(const TSTENG_handle_t handle);
static void LBSSTELP_socket_add_client(TSTENG_client_t * client_p);




static t_OsaThreadHandle LBSSTELP_socket_thread_handle;
static TSTENG_client_t *TSTENG_clients = NULL;



/*
 * LBSSTELP_socket_init()
 *
 * Startup LBS STELP Socket
 */
void LBSSTELP_socket_init(void)
{
    INF("LBS STELP Socket init called\n");

    (void) OSA_ThreadCreate(LBSSTELP_socket_thread_handle, LBSSTELP_socket_server_thread, NULL);
}


/*
 * LBSSTELP_socket_server_thread
 *
 * Single thread to bring up server port and accept new clients
 */
static void* LBSSTELP_socket_server_thread(void *args)
{
    int skt;
    int on = 1;
    struct sockaddr_in svraddr, cliaddr;
    TSTENG_client_t *pl_client_t=NULL;
    struct sockaddr *pl_soc=NULL;
//    static int SocketCreated = FALSE;

    TSTENG_client_t *client_p;
    socklen_t len = sizeof(*pl_soc);


    INF("bringing up test engine...\n");
    if(!socket_conn.SocketCreated)
    {
        // allocate a socket fd
        if ((skt = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            CRI("TSTENG_server_thread: unable to create a socket\n");
            exit(errno);
        }
        // setup port information
        svraddr.sin_family = AF_INET;
        svraddr.sin_port = htons(PORT_STELP_AMIL_SERVER_HOST);
        svraddr.sin_addr.s_addr = htonl(INADDR_STELP_AMIL_SERVER_HOST);

        INF("starting on port %d\n", PORT_STELP_AMIL_SERVER_HOST);

        memset(&(svraddr.sin_zero), '\0', 8);

        // allow us to re-use a public facing port
        if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
        {
            CRI("unable to set socket options\n");
            exit(errno);
        }
        // request port
        if (bind(skt, (struct sockaddr *) &svraddr, sizeof(*pl_soc)) < 0)
        {
            CRI("socket bind failed\n");
            exit(errno);
        }
        // start listening for incoming messages
        if (listen(skt, TSTENG_MAX_CLIENT) != 0)
        {
            CRI("cant listen on socket");
            exit(errno);
        }

        INF("ready to accept clients...\n");



        // wait for new client
        if ((socket_conn.fd_stelp_socket = accept(skt, (struct sockaddr *) &cliaddr, &len)) == -1) {
            INF("accept gives error...\n");
            ERR("Socket accept\n");
            exit(errno);
        }
        else
        {
            socket_conn.SocketCreated = TRUE;
        }

        INF("socket accepted new connection, create client fd = %d\n", socket_conn.fd_stelp_socket);
        // allocate client info
    }

    if( socket_conn.SocketCreated )
    {
        if ((client_p = OSA_Malloc(sizeof(*pl_client_t))) == NULL) {
            CRI("Unable to allocate memory\n");
            close(socket_conn.fd_stelp_socket);
        }

        client_p->fd = socket_conn.fd_stelp_socket;
        client_p->handle = socket_conn.fd_stelp_socket;    // for now, may need to pass handle in the msg from the LTP process
        client_p->next = NULL;

        LBSSTELP_socket_add_client(client_p);

        (void) OSA_ThreadCreate(client_p->thread, LBSSTELP_socket_client_thread, (void *) client_p);


    }

    return NULL;
}



/*
 * LBSSTELP_socket_client_thread()
 *
 * Each client gets allocated a thread to handle all incoming
 * messages
 */
static void *LBSSTELP_socket_client_thread(void *args)
{
    TSTENG_client_t *client_p=NULL;
    int len;

    INF("LBSSTELP_socket_client_thread");

    // get our client details
    client_p = (TSTENG_client_t *) args;
    if (client_p == NULL) {
    ERR("failed to retrive client information\n");
    return NULL;
    }

    INF("fd=%d client thread\n", client_p->fd);

    while (1) {
        ALOGD("1. LBSSTELP_socket_client_thread socket_conn.stelp_socket_data_read = %d \n", socket_conn.stelp_socket_data_read);
        len = OSA_RecvSocket(client_p->fd, &msg, sizeof(msg));

        if (len == 0) {
            ALOGD("client disconnected\n");

            INF("client disconnected\n");
            break;
        }
        else if (len == -1) {
            if (errno != ECONNRESET)
            CRI("unexpected error %d\n", errno);
            ALOGD("unexpected error %d\n", errno);

            break;
        }
        else if (len > (int) sizeof(msg)) {
            CRI("Invalid read length!!! %d\n", len);
            ALOGD("Invalid read length!!! %d\n", len);

            exit(errno);
        }
        else
        {
            //firstdatareceived = TRUE;

            socket_conn.stelp_socket_data_read = 1;

            //ALOGD("received command string %s on fd=%d \n", msg.testcase_str, client_p->fd);
            //ALOGD("2. socket_conn.stelp_socket_data_read = %d \n", socket_conn.stelp_socket_data_read);
            //LBSSTELP_socket_Read_client_data(msg);
            ALOGD ("DEBUG 2. LBSSTELP_socket_client_thread socket_conn.stelp_socket_data_read = %d  ", socket_conn.stelp_socket_data_read);
            while(socket_conn.stelp_socket_data_read)
            {
                sleep(2);
                ALOGD("DEBUG LBSSTELP_socket_client_thread. STILL STUCK IN WHILE LOOP");
            }
            ALOGD ("DEBUG 3. LBSSTELP_socket_client_thread socket_conn.stelp_socket_data_read = %d  ", socket_conn.stelp_socket_data_read);


        }

    }


    return NULL;
}



static void LBSSTELP_socket_add_client(TSTENG_client_t * client_p)
{
    if (TSTENG_clients == NULL) {
    TSTENG_clients = client_p;
    } else {
    TSTENG_client_t *nxt_p = TSTENG_clients;
    while (nxt_p->next != NULL) {
        nxt_p = (TSTENG_client_t *)nxt_p->next;
    }
    nxt_p->next = (struct TSTENG_client_s *)client_p;
    }
}



/*
 * LBSSTELP_socket_close_down()
 *
 * Close down all clients gracefully
 */
void LBSSTELP_socket_close_down(void)
{
    TSTENG_client_t *client_p, *this;

    client_p = TSTENG_clients;
    while (client_p != NULL) {
    // close port
    close(client_p->fd);

    // remove & free
    this = client_p;
    client_p = (TSTENG_client_t *)client_p->next;
    free(this);
    this = NULL;
    }
}
#if 0

static void LBSSTELP_socket_remove_client(const TSTENG_handle_t handle)
{
    TSTENG_client_t *nxt=NULL;
    TSTENG_client_t *client = NULL;

    if (TSTENG_clients->handle == handle) {
    client = TSTENG_clients;
    TSTENG_clients = TSTENG_clients->next;
    } else {
    nxt = TSTENG_clients;
    while (nxt != NULL) {
        if (nxt->handle == handle) {
        client = nxt;
        if (nxt->next != NULL) {
            nxt->next = nxt->next->next;
        }
        break;
        }
        nxt = nxt->next;
    }
    }

    if (client == NULL) {
    ERR("unable to find client\n");
    return;
    }

    (void) OSA_CloseSocket(client->fd);
    OSA_Free(client);
}



/*
 * LBSSTELP_socket_handle_new_message()
 *
 * correct
 * sub module
 */
static void LBSSTELP_socket_handle_new_message(const TSTENG_socket_msg_t * msg, const TSTENG_client_t * client_p)
{
    TSTENG_socket_rtn_msg_t end;
    int result;
    TSTENG_rtn_type_t *pl_rtn_type_t=NULL;

    if (client_p == NULL) {
    ERR("invalid client\n");
    return;
    }

    result = TSTENG_execute_command(client_p->handle, msg->testcase_str, (TSTENG_command_item_t **) &(client_p->cmd_p));

    TSTENG_test_result_cb(client_p->handle, result);

    end.type = TSTENG_RTN_END;
    end.result = result;
    end.len = sizeof(*pl_rtn_type_t);
    end.string = NULL;

    if (write(client_p->fd, &end, sizeof(end)) < 0) {
    ERR("failed to write all data to client\n");
    }
}
#endif



