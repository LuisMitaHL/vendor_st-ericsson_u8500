/*
 * Positioning Manager
 *
 * test_engine.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#include <agpsosa.h>
#include <test_engine_process.h>
#include <test_engine_int.h>
#include <test_engine_util.h>
//#include <pos_api.h>


/*
 * Client linked list struct
 */
typedef struct TSTENG_client_s {
    int fd;
    t_OsaThreadHandle thread;
    TSTENG_handle_t handle;
    TSTENG_command_item_t *cmd_p;
    struct TSTENG_client_s *next;
} TSTENG_client_t;


TSTENG_table_t *TSTENG_registered_tables = NULL;

static void *TSTENG_server_thread(void *args);
static void *TSTENG_client_thread(void *args);
static void TSTENG_add_client(TSTENG_client_t * client);
static void TSTENG_handle_new_message(const TSTENG_socket_msg_t * msg, const TSTENG_client_t * client);
static TSTENG_client_t *TSTENG_find_client(const TSTENG_handle_t handle);

static int TSTENG_cmd_list(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TSTENG_toggle_inf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int TSTENG_toggle_dbg(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

static t_OsaThreadHandle TSTENG_thread_handle;
static TSTENG_client_t *TSTENG_clients = NULL;

static int TSTENG_port_no = TSTENG_PORT;

TSTENG_TEST_TABLE_BEGIN(TSTENG)
    TSTENG_TEST_TABLE_CMD("list", TSTENG_cmd_list, "Lists available commands")
    TSTENG_TEST_TABLE_CMD("toggle_inf", TSTENG_toggle_inf, "Toggle INF prints")
    TSTENG_TEST_TABLE_CMD("toggle_dbg", TSTENG_toggle_dbg, "Toggle DBG prints")
TSTENG_TEST_TABLE_END

/*
 * Module test command tables
 */
void TSTENG_set_server_port(const int port)
{
    INF("setting test server port to %d\n", port);
    TSTENG_port_no = port;
}

/*
 * TSTENG_init()
 *
 * Startup test engine
 */
void TSTENG_init(void)
{
    INF("init called\n");

    TSTENG_REGISTER_TABLE(TSTENG);

    (void) OSA_ThreadCreate(TSTENG_thread_handle, TSTENG_server_thread, NULL);
}


/*
 * TSTENG_test_result_cb
 *
 * Called by sub-module under test to send time information to client
 */
void TSTENG_test_result_cb(TSTENG_handle_t handle, TSTENG_result_t result)
{
    TSTENG_client_t *client_p=NULL;
    TSTENG_socket_rtn_msg_t *msg_p=NULL;
    int len;

    if ((client_p = TSTENG_find_client(handle)) == NULL) {
    ERR("invalid test handle\n");
    return;
    }

    len = 0;

    if (client_p->cmd_p != NULL) {
    if (client_p->cmd_p->description != NULL) {
        len = strlen(client_p->cmd_p->description);
    }
    }

    if (len < 0) {
    ERR("impossible!\n");
    return;
    }

    msg_p = OSA_Malloc(sizeof(*msg_p) + len);
    if (msg_p == NULL) {
    CRI("unable to allocate memory\n");
    return;
    }

    bzero(msg_p, sizeof(*msg_p) + len);

    msg_p->type = TSTENG_RTN_RESULT;
    msg_p->result = result;
    msg_p->len = sizeof(*msg_p) + len;

    if (len == 0)
    msg_p->string = NULL;
    else
    memcpy(&(msg_p->string), client_p->cmd_p->description, len);

// XXX OSA_SocketSend

    if (write(client_p->fd, msg_p, sizeof(*msg_p) + len) < 0) {
    ERR("failed to write all data to client\n");
    }

    free(msg_p);
    msg_p = NULL;
}

/*
 * TSTENG_test_info_cb
 *
 * Called by test cases to provide feedback during a test
 */
void TSTENG_test_info_cb(TSTENG_handle_t handle, TSTENG_result_t result, const char *str)
{
    TSTENG_client_t *client=NULL;
    TSTENG_socket_rtn_msg_t *msg=NULL;
    int len;

    client = TSTENG_find_client(handle);
    if (client == NULL) {
    ERR("invalid test handle\n");
    return;
    }

    if (str != NULL)
    len = strlen(str);
    else
    len = 0;

    msg = OSA_Malloc(sizeof(*msg) + len);
    if (msg == NULL) {
    CRI("Unable to allocate enough memory\n");
    return;
    }

    bzero(msg, sizeof(*msg) + len);

    msg->type = TSTENG_RTN_INFO;
    msg->result = result;
    msg->len = sizeof(*msg) + len;

    if (len == 0) {
    msg->string = NULL;
    } else {
    memcpy(&(msg->string), str, len);
    }

    if (write(client->fd, msg, sizeof(*msg) + len) < 0) {
    ERR("failed to write all data to client\n");
    }

    free(msg);
    msg = NULL;
}

/*
 * TSTENG_server_thread
 *
 * Single thread to bring up server port and accept new clients
 */
static void *TSTENG_server_thread(void *args)
{
    int skt;
    int on = 1;
    struct sockaddr_in svraddr, cliaddr;
    TSTENG_client_t *pl_client_t=NULL;
    struct sockaddr *pl_soc=NULL;

    args=args;
    INF("bringing up test engine...\n");

    // allocate a socket fd
    if ((skt = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    CRI("TSTENG_server_thread: unable to create a socket\n");
    exit(errno);
    }
    // setup port information
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(TSTENG_port_no);
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    INF("starting on port %d\n", TSTENG_port_no);

    memset(&(svraddr.sin_zero), '\0', 8);

    // allow us to re-use a public facing port
    if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0) {
    CRI("unable to set socket options\n");
    exit(errno);
    }
    // request port
    if (bind(skt, (struct sockaddr *) &svraddr, sizeof(*pl_soc)) < 0) {
    CRI("socket bind failed\n");
    exit(errno);
    }
    // start listening for incoming messages
    if (listen(skt, TSTENG_MAX_CLIENT) != 0) {
    CRI("cant listen on socket");
    exit(errno);
    }

    INF("ready to accept clients...\n");

    while (1) {
    int fd;
    TSTENG_client_t *client_p;
    socklen_t len = sizeof(*pl_soc);

    // wait for new client
    if ((fd = accept(skt, (struct sockaddr *) &cliaddr, &len)) == -1) {
        INF("accept gives error...\n");
        ERR("Socket accept\n");
        exit(errno);
    }

    INF("socket accepted new connection, create client fd = %d\n", fd);
    // allocate client info
    if ((client_p = OSA_Malloc(sizeof(*pl_client_t))) == NULL) {
        CRI("Unable to allocate memory\n");
        close(fd);
        continue;
    }

    client_p->fd = fd;
    client_p->handle = fd;    // for now, may need to pass handle in the msg from the LTP process
    client_p->next = NULL;

    TSTENG_add_client(client_p);

    (void) OSA_ThreadCreate(client_p->thread, TSTENG_client_thread, (void *) client_p);

    }

    // if we ever get here, try and close down everything
    // gracefully
    TSTENG_close_down();

    INF("closing down socket\n");
    (void) close(skt);

}

/*
 * TSTENG_add_client()
 *
 * Add a new client to our linked list
 */
static void TSTENG_add_client(TSTENG_client_t * client_p)
{
    if (TSTENG_clients == NULL) {
    TSTENG_clients = client_p;
    } else {
    TSTENG_client_t *nxt_p = TSTENG_clients;
    while (nxt_p->next != NULL) {
        nxt_p = nxt_p->next;
    }
    nxt_p->next = client_p;
    }
}

static void TSTENG_remove_client(const TSTENG_handle_t handle)
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
 * TSTENG_close_down()
 *
 * Close down all clients gracefully
 */
void TSTENG_close_down(void)
{
    TSTENG_client_t *client_p, *this;

    client_p = TSTENG_clients;
    while (client_p != NULL) {
    // close port
    close(client_p->fd);

    // remove & free
    this = client_p;
    client_p = client_p->next;
    free(this);
    this = NULL;
    }
}

/*
 * TSTENG_find_client
 *
 * Finds client in linked list
 */
static TSTENG_client_t *TSTENG_find_client(const TSTENG_handle_t handle)
{
    TSTENG_client_t *nxt;

    nxt = TSTENG_clients;
    while (nxt != NULL) {
    if (nxt->handle == handle)
        return nxt;

    nxt = nxt->next;
    }
    return NULL;
}


/*
 * TSTENG_client_thread()
 *
 * Each client gets allocated a thread to handle all incoming
 * messages
 */
static void *TSTENG_client_thread(void *args)
{
    TSTENG_socket_msg_t msg;
    TSTENG_client_t *client_p=NULL;
    int len;

    // get our client details
    client_p = (TSTENG_client_t *) args;
    if (client_p == NULL) {
    ERR("failed to retrive client information\n");
    return NULL;
    }

    INF("fd=%d client thread\n", client_p->fd);

    while (1) {
    len = OSA_RecvSocket(client_p->fd, &msg, sizeof(msg));

    if (len == 0) {
        INF("client disconnected\n");
        break;
    } else if (len == -1) {
        if (errno != ECONNRESET)
        CRI("unexpected error %d\n", errno);

        break;
    } else if (len > (int) sizeof(msg)) {
        CRI("Invalid read length!!! %d\n", len);
        exit(errno);
    }

    DBG("received command string %s on fd=%d \n", msg.testcase_str, client_p->fd);

    TSTENG_handle_new_message(&msg, client_p);
    }

    TSTENG_remove_client(client_p->handle);

    return NULL;
}


/*
 * TSTENG_handle_new_message()
 *
 * correct
 * sub module
 */
static void TSTENG_handle_new_message(const TSTENG_socket_msg_t * msg, const TSTENG_client_t * client_p)
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

static int TSTENG_cmd_list(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_table_t *table = TSTENG_registered_tables;
    TSTENG_command_item_t *nxt=NULL;

    cmd_buf = cmd_buf;
    arg_index = arg_index;
    args_found = args_found;
    while (table != NULL) {

    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, table->name);

    nxt = table->table;
    while (nxt->command_p != NULL) {
        char *str;

        // + 10 to cater from white space, new lines etc
        // it's not an exact number but will hopefully make like
        // a little safer.
        // IF YOU CHANGE THE sprintf BELOW PLEASE CHECK +10 STILL
        // GIVES ENOUGH LEEWAY
        str = OSA_Malloc(strlen(nxt->command_p) + strlen(nxt->description) + 10);

        sprintf(str, "+ %s - %s", nxt->command_p, nxt->description);

        TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, str);

        OSA_Free(str);

        nxt++;
    }

    table = table->next;
    }

    return TSTENG_RESULT_OK;
}

static int TSTENG_toggle_inf(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{

    cmd_buf = cmd_buf;
    arg_index = arg_index;
    if (args_found == 1) {
    TSTENG_test_result_cb(handle, TSTENG_RESULT_OK);
    } else {
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "correct usage TSTENG toggle_inf <0|1>");
    TSTENG_test_result_cb(handle, TSTENG_RESULT_FAIL);
    }

    return TSTENG_RESULT_OK;
}

static int TSTENG_toggle_dbg(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    cmd_buf = cmd_buf;
    arg_index = arg_index;
    if (args_found == 1) {
    TSTENG_test_result_cb(handle, TSTENG_RESULT_OK);
    } else {
    TSTENG_test_info_cb(handle, TSTENG_RESULT_OK, "correct usage TSTENG toggle_dbg <0|1>");
    TSTENG_test_result_cb(handle, TSTENG_RESULT_FAIL);
    }

    return TSTENG_RESULT_OK;
}
