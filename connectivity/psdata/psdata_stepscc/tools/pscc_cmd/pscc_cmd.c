/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control command tool
 */
#include <sys/param.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> /* must be included before sys/un.h, which is badly written */
#include <sys/un.h>     /* <-- */
#include <stdarg.h>

#include "pscc_msg.h"
#include "psccd.h"
#include "stecom_i.h"
#include "libstecom.h"

#define PSCC_TEST_CLIENT_CMD_STRING_SIZE (1*1024)
#define PSCC_TEST_CLIENT_CMD_MAX_ARGS (100)
#define PSCC_TEST_CLIENT_CMD_PRINT_HEADER "[PSCC]: "
/* max size of read buffer between sockets */
static const int MAX_READ_BUFFER_SIZE = 3000;
/* timeout time to wait for an event */
static const int RSP_TIMEOUTTIME = 2000;
static const int EVENT_TIMEOUTTIME = 5000;

#define REQUEST_SOCKET_PSCC PSCC_CTRL_PATH
#define EVENT_SOCKET_PSCC   PSCC_EVENT_PATH
#define SOCKET_NAME_MAX_LENGTH 107 /* declared in un.h */

struct psccclient_s {
  int pscc_fd_request;
  int pscc_fd_event;
  struct stec_ctrl *pscc_ctrl_request;
  struct stec_ctrl *pscc_ctrl_event;
};

typedef const struct {
  const char *name;
  int (*func)(int argc, char *argv[]);
  int (*response_handler)(mpl_msg_t *resp_p);
  const char* description;
  const char* Usage_p;
}pscc_command_t;

typedef struct psccclient_s psccclient_t;
static psccclient_t *psccclient_p;
int pscc_cmd_exec_command(char* CommandString_p, pscc_command_t **Cmd_pp);
static psccclient_t *psccclient_open_session(void);
static int handle_event();
static int cmd_help(int argc, char *argv[]);
static int cmd_query_attach_mode(int argc, char *argv[]);
static int query_attach_mode_rsp_handler(mpl_msg_t *resp_p);
static int cmd_attach_status(int argc, char *argv[]);
static int attach_status_rsp_handler(mpl_msg_t *resp_p);
static int cmd_create(int argc, char *argv[]);
static int create_rsp_handler(mpl_msg_t *resp_p);
static int cmd_list(int argc, char *argv[]);
static int list_rsp_handler(mpl_msg_t *resp_p);
static int cmd_destroy(int argc, char *argv[]);
static int destroy_rsp_handler(mpl_msg_t *resp_p);
static int cmd_set(int argc, char *argv[]);
static int set_rsp_handler(mpl_msg_t *resp_p);
static int cmd_get(int argc, char *argv[]);
static int get_rsp_handler(mpl_msg_t *resp_p);
static int cmd_attach(int argc, char *argv[]);
static int attach_rsp_handler(mpl_msg_t *resp_p);
static int cmd_detach(int argc, char *argv[]);
static int detach_rsp_handler(mpl_msg_t *resp_p);
static int cmd_modify(int argc, char *argv[]);
static int modify_rsp_handler(mpl_msg_t *resp_p);
static int cmd_connect(int argc, char *argv[]);
static int connect_rsp_handler(mpl_msg_t *resp_p);
static int cmd_disconnect(int argc, char *argv[]);
static int disconnect_rsp_handler(mpl_msg_t *resp_p);
static int cmd_fetch_data_count(int argc, char *argv[]);
static int fetch_data_count_rsp_handler(mpl_msg_t *resp_p);
static int cmd_rst_data_count_status(int argc, char *argv[]);
static int rst_data_count_rsp_handler(mpl_msg_t *resp_p);
static int cmd_config_attach_mode(int argc, char *argv[]);
static int config_attach_mode_rsp_handler(mpl_msg_t *resp_p);
static int cmd_uplink_data(int argc, char *argv[]);
static int uplink_data_rsp_handler(mpl_msg_t *resp_p);
static int cmd_init(int argc, char *argv[]);
static int init_rsp_handler(mpl_msg_t *resp_p);
static int cmd_basic_info(int argc, char *argv[]);
static int cmd_list_params(int argc, char *argv[]);
static int cmd_exit(int argc, char *argv[]);

static bool check_return_value(const char *func_str,
                               int ret_code, const char *str);
static char *restore_param(int *argc, char **argv[]);
static void pscc_log(void *user_p, int level, const char *file, int line, const char *format_p, ...);
static void printusage();
int waitAndReadStecomResp(mpl_msg_t* resp_p);
int waitForEvent(int fd, int timeout);

pscc_command_t supported_commands[] = {
    {"create",
     cmd_create,
     create_rsp_handler,
     "Create new connection",
     NULL},

    {"destroy",
     cmd_destroy,
     destroy_rsp_handler,
     "Destroy connection",
     "Usage: destroy <connid>"},

    {"list",
     cmd_list,
     list_rsp_handler,
     "List all connections",
     NULL},

    {"connect",
     cmd_connect,
     connect_rsp_handler,
     "Connect PDP context",
     "Usage: connect <connid>"},

    {"disconnect",
     cmd_disconnect,
     disconnect_rsp_handler,
     "Disconnect PDP context",
     "Usage: disconnect <connid>"},

    {"set",
     cmd_set,
     set_rsp_handler,
     "Set parameter",
     "Usage: set <connid> <key> <value> \n"
     "Example: set 1001 attach_status attached (parameters are space delimited)"},

    {"get",
     cmd_get,
     get_rsp_handler,
     "Get parameter",
     "Usage: get <connid> <key>\n"
     "Example: get 1001 pdp_type"},

    {"attach",
     cmd_attach,
     attach_rsp_handler,
     "Perform PS attach",
     NULL},

    {"detach",
     cmd_detach,
     detach_rsp_handler,
     "Perform PS detach",
     NULL},

    {"modify",
     cmd_modify,
     modify_rsp_handler,
     "Modify PDP context",
     "Usage: modify <connid>"},

    {"init_handler",
     cmd_init,
     init_rsp_handler,
     "Initiate pscc handler",
     NULL},

    {"query_attach_mode",
     cmd_query_attach_mode,
     query_attach_mode_rsp_handler,
     "Query attach mode",
     NULL},

    {"query_attach_status",
     cmd_attach_status,
     attach_status_rsp_handler,
     "Query attach status",
     NULL},

    {"fetch_data_counters",
     cmd_fetch_data_count,
     fetch_data_count_rsp_handler,
     "Fetch data counters",
     "Usage: fetch_data_counters <connid>"},

    {"reset_data_counters",
     cmd_rst_data_count_status,
     rst_data_count_rsp_handler,
     "Reset data counters",
     "Usage: reset_data_counters <connid>"},

    {"configure_attach_mode",
     cmd_config_attach_mode,
     config_attach_mode_rsp_handler,
     "Configure attach mode",
     "Usage: configure_attach_mode <attach_mode>\n"
     "Example: configure_attach_mode manual (parameters are space delimited)"},

    {"generate_uplink_data",
     cmd_uplink_data,
     uplink_data_rsp_handler,
     "Generate uplink data",
     "Usage: generate_uplink_data <connid>"},

    {"basic_connection_info",
     cmd_basic_info,
     get_rsp_handler,
     "Get basic info for a connection",
     "Usage: basic_connection_info <connid>"},

    {"list_params",
     cmd_list_params,
     NULL,
     "List all valid params to be used for get/set",
     NULL},

    {"exit",
     cmd_exit,
     NULL,
     "exit the pscc_cmd tool",
     NULL},

    {"help",
     cmd_help,
     NULL,
     "Shows this help",
     NULL},
    // last entry
    { NULL, NULL, NULL, NULL, NULL }
};

static const size_t num_api_funcs = sizeof(supported_commands) / sizeof(supported_commands[0]);

static void handleCtrlC()
{
  printf("Caught SIGINT, exiting...\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  fd_set readfds;
  struct timeval tv;
  struct sigaction ctrl_c;
  char PSCC_TEST_CommandString_p[PSCC_TEST_CLIENT_CMD_STRING_SIZE];
  mpl_msg_t resp_p;
  pscc_command_t *cmd;
  int ret_code;
  int a;

  psccclient_p = psccclient_open_session();
  // Catch Ctrl-C
  ctrl_c.sa_handler = handleCtrlC;
  sigemptyset(&ctrl_c.sa_mask);
  ctrl_c.sa_flags = 0;
  sigaction(SIGINT, &ctrl_c, NULL);

  printf(PSCC_TEST_CLIENT_CMD_PRINT_HEADER);
  fflush(stdout);

  /* main loop */
  while (true) {
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(psccclient_p->pscc_fd_event, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int select_retval = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
    if (select_retval < 0) {
      perror("select");
    } else if (select_retval > 0) {

      /* command received */
      if (FD_ISSET(STDIN_FILENO, &readfds)) {
        if (fgets(PSCC_TEST_CommandString_p, PSCC_TEST_CLIENT_CMD_STRING_SIZE, stdin) != NULL) {
          // Remove newline etc at end
          for (a = strlen(PSCC_TEST_CommandString_p) - 1; a >= 0; a--) {
            if (PSCC_TEST_CommandString_p[a] == '\n' || PSCC_TEST_CommandString_p[a] == '\r')
              PSCC_TEST_CommandString_p[a] = 0;
            else
              a = 0;
          }
          ret_code = pscc_cmd_exec_command(PSCC_TEST_CommandString_p, &cmd);
          if (cmd && (ret_code == 0) && (cmd->response_handler)) {
            waitAndReadStecomResp(&resp_p);
            ret_code = cmd->response_handler(&resp_p);
          }
        }
      }

      /* Event received, handle it */
      if (FD_ISSET(psccclient_p->pscc_fd_event, &readfds)) {
        handle_event();
      }
      printf(PSCC_TEST_CLIENT_CMD_PRINT_HEADER);
      fflush(stdout);
    }
  }

  return ret_code;
}

static int handle_event()
{
  char buf[MAX_READ_BUFFER_SIZE];
  int size;
  pscc_msg_t* event_p;
  mpl_list_t *list_p;
  mpl_param_element_t* param_elem_p;
  char param[1000];
  int param_len = 1000;
  char idstr[20];

  event_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  size = stec_recv(psccclient_p->pscc_ctrl_event, buf, MAX_READ_BUFFER_SIZE, EVENT_TIMEOUTTIME);
  if (size < 0) {
    printf("%s : stec_recv failed\n", __func__);
    goto error;
  }
  if (mpl_msg_unpack(buf, size, (mpl_msg_t*)event_p) < 0) {
    printf("waitAndReadStecomEvent : mpl_msg_unpack failed\n");
    goto error;
  }

  if (PSCC_CONNID_PRESENT(event_p)) {
    sprintf(idstr, "%d", PSCC_GET_CONNID(event_p));
  }
  else {
    strcpy(idstr, "<not present>");
  }
  switch(event_p->common.id)
    {
  case pscc_event_connecting:
    printf("got pscc_event_connecting from connid %s\n", idstr);
    break;
  case pscc_event_connected:
    printf("got pscc_event_connected from connid %s\n", idstr);
    break;
  case pscc_event_disconnecting:
    printf("got pscc_event_disconnecting from connid %s\n", idstr);
    break;
  case pscc_event_disconnected:
    printf("got pscc_event_disconnected from connid %s\n", idstr);
    MPL_LIST_FOR_EACH(event_p->event_disconnected.param_list_p, list_p) {
      param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

      (void) mpl_param_pack(param_elem_p, param, param_len);
      printf("%s\n", param);
    }
    break;
  case pscc_event_modify_failed:
    printf("got pscc_event_modify_failed from connid %s\n", idstr);
    break;
  case pscc_event_data_counters_fetched:
    printf("got pscc_event_data_counters_fetched from connid %s\n", idstr);
    MPL_LIST_FOR_EACH(event_p->event_data_counters_fetched.param_list_p, list_p) {
      param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

      (void) mpl_param_pack(param_elem_p, param, param_len);
      printf("%s\n", param);
    }
    break;
  case pscc_event_data_counters_reset:
    printf("got pscc_event_data_counters_reset from connid %s\n", idstr);
    break;
  case pscc_event_modified:
    printf("got pscc_event_modified from connid %s\n", idstr);
    break;
  case pscc_event_attached:
    printf("got pscc_event_attached\n");
    break;
  case pscc_event_attach_failed:
    printf("got pscc_event_attach_failed\n");
    break;
  case pscc_event_detached:
    printf("got pscc_event_detached\n");
    MPL_LIST_FOR_EACH(event_p->event_detached.param_list_p, list_p) {
      param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

      (void) mpl_param_pack(param_elem_p, param, param_len);
      printf("%s\n", param);
    }
    break;
  case pscc_event_detach_failed:
    printf("got pscc_event_detach_failed\n");
    break;
  case pscc_event_attach_mode_queried:
    if (MPL_MSG_PARAM_PRESENT(event_p, pscc_paramid_attach_mode)) {
      sprintf(param, "%s", mpl_param_value_get_string(pscc_paramid_attach_mode,
          &MPL_MSG_GET_PARAM_VALUE(event_p, pscc_attach_mode_t, pscc_paramid_attach_mode)));
    }
    printf("got pscc_event_attach_mode_queried, attach_mode=%s\n", param);
    break;
  case pscc_event_attach_mode_configured:
    printf("got pscc_event_attach_mode_configured\n");
    break;
  case pscc_event_attach_mode_configure_failed:
    printf("got pscc_event_attach_mode_configure_failed\n");
    break;
  default:
    printf("Unknown event received\n");
    break;
  }
  return 0;

error:
  return -1;
}


// tokenizes the command string and executes the corresponding command
int pscc_cmd_exec_command(char* CommandString_p, pscc_command_t **Cmd_pp)
{
  int Argc = 0;
  int result = -1;
  char* Argv_p[PSCC_TEST_CLIENT_CMD_STRING_SIZE];
  const pscc_command_t* Cmd_p = NULL;
  char* CommandName_p = strtok(CommandString_p, " \t");
  if (CommandName_p != NULL) {
    int i;

    // find command
    for (i = 0; Cmd_p == NULL && supported_commands[i].name != NULL; i++) {
      if (strcmp(supported_commands[i].name, CommandName_p) == 0) {
        Cmd_p = &supported_commands[i];
      }
    }
    if (Cmd_p == NULL) {
      printf("Unknown command '%s'\n", CommandName_p);
      goto printusage;
    } else {
      // tokenize command string
      Argv_p[0] = CommandName_p;
      Argc = 1;
      while (Argc < PSCC_TEST_CLIENT_CMD_MAX_ARGS && (Argv_p[Argc] = strtok(NULL, " \t")) != NULL) {
        ++Argc;
      }
      // execute the command
      if (Cmd_p->func(Argc, Argv_p) != 0) {
        if (Cmd_p->Usage_p != NULL) {
          // incorrect usage of the command; show usage
          printf("Usage: %s\n", Cmd_p->Usage_p);
        }
        goto exit;
      }
    }
  } else {
    goto printusage;
  }
result = 0;

exit:
*Cmd_pp = Cmd_p;
  return result;

printusage:
  printusage();
  goto exit;
}

static void printusage() {
  size_t n;
  fprintf(stderr, "Usage:\n");
  for (n = 0; supported_commands[n].name != NULL; n++) {
    fprintf(stderr, "%21s%s%10s\n", supported_commands[n].name, " <args...> - ", supported_commands[n].description);
  }
}


/*
 * Method for listening to a fd
 */
int waitForEvent(int fd, int timeout) {
  fd_set rfds;
  struct timeval tv, *tvp;
  int result = 0;

  if (fd < 0) {
    printf("waitForEvent: fd < 0 \n");
    return -1;
  }

  /* Set up timeout structure */
  if (timeout < 0) {
    tvp = NULL;
  } else {
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    tvp = &tv;
  }

  FD_ZERO(&rfds);
  FD_SET(fd, &rfds );

  result = select(fd + 1, &rfds, NULL, NULL, tvp);

  if (result == -EINTR) {
    printf("waitForEvent: select returned because of a signal\n");
    return -1;
  } else if (result == 0) {
    printf("waitForEvent: Timeout waiting for event\n");
    return -1;
  } else if (FD_ISSET(fd, &rfds)) {
    /* we got an event before timeout */
    return 0;
  } else {
    printf("waitForEvent: Error, select returned without fd set\n");
    return -1;
  }
}

/**
 *  Method used to wait and read stecom response
 */
int waitAndReadStecomResp(mpl_msg_t* resp_p) {
  char buf[MAX_READ_BUFFER_SIZE];
  int size;

  if (waitForEvent((int) psccclient_p->pscc_fd_request, RSP_TIMEOUTTIME) < 0) {
    printf("waitAndReadStecomEvent : waitForEvent failed\n");
    goto error;
  }
  size = stec_recv(psccclient_p->pscc_ctrl_request, buf, 1000, RSP_TIMEOUTTIME);
  if (size < 0) {
    printf("waitAndReadStecomEvent : stec_recv failed\n");
    goto error;
  }
  if (mpl_msg_unpack(buf, size, resp_p) < 0) {
    printf("waitAndReadStecomEvent : mpl_msg_unpack failed\n");
    goto error;
  }

  return 0;
error:
  return -1;
}

static psccclient_t *psccclient_open_session(void) {
  psccclient_t *psccclient_p = (psccclient_t *) malloc(sizeof(psccclient_t));
  struct sockaddr_un pscc_addr_request;
  struct sockaddr_un pscc_addr_event;

  char *req_buffer_p = NULL;

  if (NULL == psccclient_p) {
    printf("pscc_open_session: psccclient is NULL!\n");
    goto error;
  }

  /* Initialize psccclient */
  psccclient_p->pscc_fd_request = 0;
  psccclient_p->pscc_fd_event = 0;

  psccclient_p->pscc_ctrl_request = NULL;
  psccclient_p->pscc_ctrl_event = NULL;

  /* Create and connect to PSCC request socket */
  pscc_addr_request.sun_family = AF_UNIX;
  strncpy(pscc_addr_request.sun_path, REQUEST_SOCKET_PSCC, SOCKET_NAME_MAX_LENGTH);
  pscc_addr_request.sun_path[SOCKET_NAME_MAX_LENGTH] = '\0';
  psccclient_p->pscc_ctrl_request = stec_open((struct sockaddr *) &pscc_addr_request,
      sizeof(struct sockaddr_un));

  /* Create and connect to PSCC event socket */
  pscc_addr_event.sun_family = AF_UNIX;
  strncpy(pscc_addr_event.sun_path, EVENT_SOCKET_PSCC, SOCKET_NAME_MAX_LENGTH);
  pscc_addr_event.sun_path[SOCKET_NAME_MAX_LENGTH] = '\0';
  psccclient_p->pscc_ctrl_event = stec_open((struct sockaddr *) &pscc_addr_event,
      sizeof(struct sockaddr_un));

  if (NULL == psccclient_p->pscc_ctrl_event) {
    printf("pscc_open_session: Failed to connect to the event socket: %s\n", EVENT_SOCKET_PSCC);
    goto error;
  }

  /* Save file descriptors in structure (so that the function calling select() can access them) */
  psccclient_p->pscc_fd_request = psccclient_p->pscc_ctrl_request->sock;
  psccclient_p->pscc_fd_event = psccclient_p->pscc_ctrl_event->sock;

  /* Initialize libmpl library through libpscc (a precondition for mpl calls) */
  if (pscc_init(NULL, pscc_log) == -1) {
    printf("pscc_open_session: pscc_init failed!\n");
    goto error;
  }

  /* Subscribe to events (this is a precondition for some of the RIL requests */
  if (stec_subscribe(psccclient_p->pscc_ctrl_event) != 0) {
    printf("pscc_open_session: failed to activate event subscription\n");
    goto error;
  }

  return psccclient_p;

error:
  if (NULL != psccclient_p) {
    stec_close(psccclient_p->pscc_ctrl_request);
    stec_close(psccclient_p->pscc_ctrl_event);
    free(psccclient_p);
  }

  return NULL;
}

/**
 *  Method used to send to stecom
 */
int sendToStecomCtrl(mpl_msg_t *pscc_msg_p) {

  char *packed_request = NULL;
  size_t packed_request_len;
  int ret;
  mpl_msg_pack((mpl_msg_t*) pscc_msg_p, &packed_request, &packed_request_len);

  ret = stec_send(psccclient_p->pscc_ctrl_request, packed_request, packed_request_len);
  if (ret < 0) {
    printf("sendToStecom(): stec_send returned < 0\n");
    goto error;
  }
  mpl_msg_buf_free((mpl_msg_t*) packed_request);

  return 0;

error:
  return -1;
}

static void pscc_log(void *user_p, int level, const char *file, int line, const char *format_p, ...)
{
    char string_buffer_p[200];
    va_list arg_list;
    va_start(arg_list, format_p);
    vsnprintf(string_buffer_p, 200, format_p, arg_list);
    va_end(arg_list);
    printf("pscc error: %s (%s line=%d)\n", string_buffer_p, file, line);
}

static int cmd_help(int argc, char *argv[])
{
  printusage();
  return 0;
}

static int cmd_query_attach_mode(int argc, char *argv[])
{
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);
  return 0;
}

static int query_attach_mode_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;

  if(resp_p->common.id != pscc_query_attach_mode){
    printf("%s wrong response\n", __func__);
    result = -1;
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));
  return result;
}

static int cmd_attach_status(int argc, char *argv[])
{
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach_status;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);
  return 0;
}

static int attach_status_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_attach_status_t attach_status;

  if (resp_p->common.id != pscc_attach_status) {
    printf("%s wrong response\n", __func__);
    result - 1;
    goto exit;
  }
  printf("attach status is: %s\n", mpl_param_value_get_string(pscc_paramid_attach_status,
      &MPL_MSG_GET_PARAM_VALUE(resp_p, pscc_attach_status_t, pscc_paramid_attach_status)));
  mpl_param_list_destroy(&resp_p->common.param_list_p);
  result = 0;

exit:
  return result;
}

static int cmd_create(int argc, char *argv[])
{
  mpl_msg_t *pscc_msg_p = NULL;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_create;
  pscc_msg_p->common.param_list_p = NULL;
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);
  return 0;
}

static int create_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid;

  if (resp_p->common.id != pscc_create) {
    printf("%s wrong response\n", __func__);
    result - 1;
    goto exit;
  }
  connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));

  printf("connid: %d\n", connid);
  mpl_param_list_destroy(&resp_p->common.param_list_p);
  result = 0;

exit:
  return result;
}

static int cmd_list(int argc, char *argv[])
{
  mpl_msg_t *pscc_msg_p = NULL;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_list_all_connections;
  pscc_msg_p->common.param_list_p = NULL;
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);
  return 0;
}

static int list_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int i, found_records = 0;
  mpl_param_element_t *param_elem_connid_p  = NULL;

  if (resp_p->common.id != pscc_list_all_connections) {
    printf("%s wrong response\n", __func__);
    result - 1;
    goto exit;
  }
  found_records = mpl_list_len(resp_p->common.param_list_p);
  printf("found connids: \n");
  for (i = 0; i < found_records; i++) {
    if (0 == i) {
      param_elem_connid_p = mpl_param_list_find(pscc_paramid_connid, resp_p->common.param_list_p);
    } else {
      param_elem_connid_p = mpl_param_list_find_next(pscc_paramid_connid, param_elem_connid_p);
    }
    printf("%d\n", *((int *)param_elem_connid_p->value_p));
  }

  printf("total number of connections: %d\n", found_records);
  mpl_param_list_destroy(&resp_p->common.param_list_p);
  result = 0;

exit:
  return result;
}

static int cmd_destroy(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_destroy;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int destroy_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_destroy) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("destroyed connid: %d\n", connid);
  } else {
    printf("could not destroy connection. result: %d\n", pscc_result);
  }

  result = 0;

exit:
  return result;
}

static int cmd_set(int argc, char *argv[])
{
  int result = 0;
  int connid;
  char *key, *value;
  pscc_msg_t *pscc_msg_p = NULL;
  mpl_param_element_t* param_elem_p;

  if (argc < 4) {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);
  key = argv[2];
  value = argv[3];
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;

  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);

  if (mpl_param_unpack_param_set(key, value, &param_elem_p, PSCC_PARAM_SET_ID) < 0) {
    printf("Syntax error\n");
    result = -1;
    goto exit;
  }
  mpl_list_add(&pscc_msg_p->req_set.param_list_p, &param_elem_p->list_entry);

  sendToStecomCtrl((mpl_msg_t*) pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int set_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_set) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    printf("connid: %d\n", PSCC_GET_CONNID((pscc_msg_t*)resp_p));
  }
  else {
    printf("connid not present\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));

  result = 0;

exit:
  return result;
}

static int cmd_get(int argc, char *argv[])
{
  int result = 0;
  int connid;
  char *key = NULL, *value = NULL;
  pscc_msg_t *pscc_msg_p = NULL;
  mpl_param_element_t* param_elem_p;

  if (argc < 3) {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);
  key = argv[2];
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;

  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);

  if (mpl_param_unpack_param_set(key, value, &param_elem_p, PSCC_PARAM_SET_ID) < 0) {
    printf("Syntax error\n");
    result = -1;
    goto exit;
  }
  mpl_list_add(&pscc_msg_p->req_set.param_list_p, &param_elem_p->list_entry);

  sendToStecomCtrl((mpl_msg_t*) pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int get_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_result_t pscc_result;
  mpl_list_t *list_p;
  mpl_param_element_t* param_elem_p;

  if (resp_p->common.id != pscc_get) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));

  MPL_LIST_FOR_EACH(resp_p->resp.param_list_p, list_p) {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    printf("%-16s : %s\n", mpl_param_id_get_string(param_elem_p->id), mpl_param_value_get_string(
        param_elem_p->id, param_elem_p->value_p));
  }
  result = 0;

exit:
  return result;
}

static int cmd_attach(int argc, char *argv[])
{
  int result = 0;
  mpl_msg_t *pscc_msg_p = NULL;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach;
  pscc_msg_p->common.param_list_p = NULL;
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int attach_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_attach) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));
  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result != pscc_result_ok) {
    result = -1;
    goto exit;
  }
  result = 0;

exit:
  return result;
}

static int cmd_detach(int argc, char *argv[])
{
  int result = 0;
  mpl_msg_t *pscc_msg_p = NULL;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_detach;
  pscc_msg_p->common.param_list_p = NULL;
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}
static int detach_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_detach) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));
  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result != pscc_result_ok) {
    result = -1;
    goto exit;
  }
  result = 0;

exit:
  return result;
}

static int cmd_modify(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_modify;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}
static int modify_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_modify) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("Modified connid: %d\n", connid);
  } else {
    printf("could not modify connection\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_connect(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int connect_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_connect) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("Connecting connid: %d\n", connid);
  } else {
    printf("could not connect\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_disconnect(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_disconnect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int disconnect_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_disconnect) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("Disconnecting connid: %d\n", connid);
  } else {
    printf("could not disconnect\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_fetch_data_count(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int fetch_data_count_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_fetch_data_counters) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("fetching data counters for connid: %d\n", connid);
  } else {
    printf("could not fetch data counters\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_rst_data_count_status(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int rst_data_count_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_reset_data_counters) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("reseting data counters for connid: %d\n", connid);
  } else {
    printf("could not reset data counters\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_config_attach_mode(int argc, char *argv[])
{
  int result = 0;
  char *mode;
  pscc_msg_t *pscc_msg_p = NULL;
  mpl_param_element_t* param_elem_p;

  if (argc < 2) {
    result = -1;
    goto exit;
  }

  mode = argv[1];
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;

  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, NULL);

  if (mpl_param_unpack_param_set("attach_mode", mode, &param_elem_p, PSCC_PARAM_SET_ID) < 0) {
    printf("Syntax error\n");
    result = -1;
    goto exit;
  }
  mpl_list_add(&pscc_msg_p->req_set.param_list_p, &param_elem_p->list_entry);
  if (sendToStecomCtrl((mpl_msg_t*) pscc_msg_p) < 0) {
    result = -1;
  }
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int config_attach_mode_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_configure_attach_mode) {
    printf("%s wrong response: %s\n", __func__, pscc_msg_id_get_string(resp_p->common.id));

    result = -1;
    goto exit;
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("configuring attach mode\n");
  } else {
    printf("could not configure attach mode\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_uplink_data(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int uplink_data_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  int connid = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_generate_uplink_data) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  if (PSCC_CONNID_PRESENT((pscc_msg_t*)resp_p)) {
    connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_p->common.param_list_p));
  }

  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result == pscc_result_ok) {
    printf("Uplink data generated connid: %d\n", connid);
  } else {
    printf("could not generate uplink data\n");
  }
  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));


  result = 0;

exit:
  return result;
}

static int cmd_init(int argc, char *argv[])
{
  mpl_msg_t *pscc_msg_p = NULL;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_init_handler;
  pscc_msg_p->common.param_list_p = NULL;
  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);
  return 0;
}

static int init_rsp_handler(mpl_msg_t *resp_p)
{
  int result = 0;
  pscc_result_t pscc_result;

  if (resp_p->common.id != pscc_init_handler) {
    printf("%s wrong response\n", __func__);
    result = -1;
    goto exit;
  }

  printf("result: %s\n", mpl_param_value_get_string(pscc_paramid_result, &resp_p->resp.result));
  pscc_result = (pscc_result_t) (resp_p->resp.result);

  if (pscc_result != pscc_result_ok) {
    result = -1;
    goto exit;
  }
  result = 0;

exit:
  return result;
}

static int cmd_basic_info(int argc, char *argv[])
{
  int result = 0;
  int connid;
  mpl_msg_t *pscc_msg_p = NULL;

  if (argc != 2)
  {
    result = -1;
    goto exit;
  }

  connid = atoi(argv[1]);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn, NULL);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_own_ip_address, NULL);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connection_status, NULL);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_dns_address, NULL);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_gw_address, NULL);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_netdev_name, NULL);

  sendToStecomCtrl(pscc_msg_p);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

exit:
  return result;
}

static int cmd_list_params(int argc, char *argv[])
{
  int i;

  printf("valid params are:\n");
  for (i = pscc_paramid_base + 1; i < pscc_end_of_paramids; i++) {
    printf("%s\n", mpl_param_id_get_string(i));
  }

  return 0;
}

static int cmd_exit(int argc, char *argv[])
{
  printf("bye...\n");
  exit(EXIT_SUCCESS);
}
