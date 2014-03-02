/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : test.c
 * Description     : Testframework and tests for Sterc Daemon
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

/************************************/
/* Includes                         */
/************************************/
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include <linux/un.h>

#include "stecom_i.h"
#include "libstecom.h"
#include "string.h"
#include "sterc_msg.h"
#include "mpl_config.h"

/************************************/
/* Static variables and definitions */
/************************************/
#define REQUEST_STERC "/tmp/sterc_ctrl_server_socket"
#define EVENT_STERC   "/tmp/sterc_event_server_socket"

static struct stec_ctrl *ctrl_handle_p = NULL;
static struct stec_ctrl *event_handle_p = NULL;

/* 0 = no extra, 1 = function eval, 2 = bool eval */
static const int DEBUGLEVEL = 0;
/* This needs to be increased if the packets sent on the test-stub filedescriptors are bigger */
static const int MAX_READ_BUFFER_SIZE = 3000;
static const int TIMEOUTTIME = 1000;

/* should be used to check positive returns */
#define TESTNOTNEG(a) \
  if(DEBUGLEVEL > 0) \
    printf("[%s:%d] TEST: " #a " called \n", __FILE__, __LINE__);\
  if((a) < 0) {\
    printf("[%s:%d] ERROR: " #a " failed \n", __FILE__, __LINE__);\
  goto error;\
  };

/* should be used to check boolean evaluations */
#define TESTBOOL(a,b)\
  {\
    int _tempa = (int) (a); \
    int _tempb = (int) (b); \
    if(DEBUGLEVEL > 1) \
      printf("[%s:%d] TESTBOOL: " #a " (%d) vs " #b " (%d)\n", __FILE__, __LINE__, _tempa, _tempb);\
    if(!((_tempa) == (_tempb))) {\
      printf("[%s:%d] ERROR: " #a " (%d) does not match " #b " (%d)\n", __FILE__, __LINE__, _tempa, _tempb);\
    goto error;\
    };\
  }

/************************************/
/* Private functions definitions    */
/************************************/
int initStecom();
void closeStecom();
int sendToStecomCtrl(mpl_msg_t *sterc_msg_p);
int waitAndReadStecomResp(mpl_msg_t* resp_p);
int waitAndReadStecomEvent(mpl_msg_t* resp_p);
int waitForEvent(int fd);
void my_log_func(void* user_p, int level, const char* file, int line, const char *format, ...);

/**************************/
/* main function          */
/**************************/
int main() {
  int client_tag = 1;
  int handle;
  char lan_device_p[] = "usb0";
  char wan_device_p[] = "rmnet0";
  char dns_server_p[] = "10.10.10.10";

  printf("\n");
  printf("#############\n");
  printf("Starting test\n");
  printf("#############\n");

  if (initStecom() < 0)
  {
    printf("Failed to start stecom\n");
    goto error;
  }
  TESTNOTNEG(sterc_init(NULL, &my_log_func));

  printf("Test 1 - create\n");
  {
    mpl_msg_t           *request_p  = NULL;
    mpl_msg_t resp;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
    if (NULL == request_p) {
        printf("mpl_req_msg_alloc failed!\n");
        goto error;
    }

    request_p->req.id = sterc_create;
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_lan_device, lan_device_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_wan_device, wan_device_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_wan_dns, dns_server_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_ct, &client_tag));
    TESTNOTNEG(sendToStecomCtrl(request_p));
    mpl_msg_free((mpl_msg_t *)request_p);

    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type, sterc_msgtype_resp);
    TESTBOOL(resp.common.id, sterc_create);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_ct,(resp.common.param_list_p)), client_tag);
    handle = MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_handle,(resp.common.param_list_p));
    TESTBOOL(resp.resp.result, sterc_result_ok);
    mpl_param_list_destroy(&resp.common.param_list_p);

  }

  printf("Test 2 - set parameter req\n");
  {
    mpl_msg_t           *request_p  = NULL;
    mpl_msg_t resp;
    memcpy(lan_device_p, "usb1\0", 5);

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
    if (NULL == request_p) {
        printf("mpl_req_msg_alloc failed!\n");
        goto error;
    }

    request_p->req.id = sterc_set;
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_handle, &handle));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_lan_device, lan_device_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_ct, &client_tag));
    TESTNOTNEG(sendToStecomCtrl(request_p));
    mpl_msg_free((mpl_msg_t *)request_p);

    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type, sterc_msgtype_resp);
    TESTBOOL(resp.common.id, sterc_set);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_ct,(resp.common.param_list_p)), client_tag);
    TESTBOOL(resp.resp.result, sterc_result_ok);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  printf("Test 3 - get parameter req\n");
  {
    mpl_msg_t           *request_p  = NULL;
    mpl_msg_t resp;
    char* lan_device;
    mpl_param_element_t       *param_element_p  = NULL;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
    if (NULL == request_p) {
        printf("mpl_req_msg_alloc failed!\n");
        goto error;
    }

    request_p->req.id = sterc_get;
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_handle, &handle));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_lan_device, lan_device_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_ct, &client_tag));
    TESTNOTNEG(sendToStecomCtrl(request_p));
    mpl_msg_free((mpl_msg_t *)request_p);

    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type, sterc_msgtype_resp);
    TESTBOOL(resp.common.id, sterc_get);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_ct,(resp.common.param_list_p)), client_tag);

    param_element_p = mpl_param_list_find(sterc_paramid_lan_device, resp.common.param_list_p);
    if (param_element_p == NULL) {
      printf("Could not find lan device parameter in response\n");
      goto error;
    }
    lan_device = (char*) param_element_p->value_p;
    TESTBOOL(strncmp(lan_device, lan_device_p, 5),0);
    TESTBOOL(resp.resp.result, sterc_result_ok);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  printf("Test 4 - destroy req\n");
  {
    mpl_msg_t           *request_p  = NULL;
    mpl_msg_t resp;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
    if (NULL == request_p) {
        printf("mpl_req_msg_alloc failed!\n");
        goto error;
    }

    request_p->req.id = sterc_destroy;
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_handle, &handle));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_ct, &client_tag));
    TESTNOTNEG(sendToStecomCtrl(request_p));
    mpl_msg_free((mpl_msg_t *)request_p);

    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type, sterc_msgtype_resp);
    TESTBOOL(resp.common.id, sterc_destroy);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_ct,(resp.common.param_list_p)), client_tag);
    TESTBOOL(resp.resp.result, sterc_result_ok);
    mpl_param_list_destroy(&resp.common.param_list_p);

  }

  printf("Test 5 - create without wan_device\n");
  {
    mpl_msg_t           *request_p  = NULL;
    mpl_msg_t resp;

    request_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
    if (NULL == request_p) {
        printf("mpl_req_msg_alloc failed!\n");
        goto error;
    }

    request_p->req.id = sterc_create;
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_lan_device, lan_device_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_wan_dns, dns_server_p));
    TESTNOTNEG(mpl_add_param_to_list(&request_p->req.param_list_p, sterc_paramid_ct, &client_tag));
    TESTNOTNEG(sendToStecomCtrl(request_p));
    mpl_msg_free((mpl_msg_t *)request_p);

    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type, sterc_msgtype_resp);
    TESTBOOL(resp.common.id, sterc_create);
    TESTBOOL(resp.resp.result, sterc_result_failed_parameter_not_found);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  closeStecom();

  printf("#######################\n");
  printf("Testing done, all pass!\n");
  printf("#######################\n\n");

  return 0;
error:

  printf("##############\n");
  printf("Testing failed\n");
  printf("##############\n\n");
  return -1;
}


/************************************/
/* Private functions implementation */
/************************************/

/**
 *  Method used to initalize stecom
 */
int initStecom() {
  struct sockaddr_un addr_un;
  if (ctrl_handle_p != NULL)
  {
    printf("initStecom(): ctrl_handle_p already initialized \n");
    return -1;
  }
  memset(&addr_un, 0, sizeof(addr_un));
  addr_un.sun_family = AF_UNIX;
  strncpy(addr_un.sun_path, REQUEST_STERC, UNIX_PATH_MAX);
  ctrl_handle_p = stec_open((struct sockaddr*) &addr_un, sizeof(addr_un));
  if (ctrl_handle_p == NULL) {
    printf("initStecom(): ctrl_handle_p == NULL\n");
    goto error;
  }

  if (event_handle_p != NULL)
  {
    printf("initStecom(): event_handle_p already initialized \n");
    goto error;
  }
  memset(&addr_un, 0, sizeof(addr_un));
  addr_un.sun_family = AF_UNIX;
  strncpy(addr_un.sun_path, EVENT_STERC, UNIX_PATH_MAX);
  event_handle_p = stec_open((struct sockaddr*) &addr_un, sizeof(addr_un));
  if (event_handle_p == NULL) {
    printf("initStecom(): event_handle_p == NULL\n");
    goto error;
  }
  if (stec_subscribe(event_handle_p) != 0) {
      printf("initStecom(): failed to activate event subscription");
      goto error;
  }
  return 0;

error:
  return -1;
}

/**
 *  Method used to close stecom
 */
void closeStecom() {
  stec_close(ctrl_handle_p);
  stec_close(event_handle_p);
  ctrl_handle_p = NULL;
  event_handle_p = NULL;
}

/**
 *  Method used to send to stecom
 */
int sendToStecomCtrl(mpl_msg_t *sterc_msg_p) {

  char *packed_request = NULL;
  size_t packed_request_len;
  int ret;
  mpl_msg_pack((mpl_msg_t*) sterc_msg_p, &packed_request, &packed_request_len);

  ret = stec_send(ctrl_handle_p, packed_request, packed_request_len);
  if (ret < 0) {
    printf("sendToStecom(): stec_send returned <0\n");
    goto error;
  }
  mpl_msg_buf_free((mpl_msg_t*) packed_request);

  return 0;

  error: return -1;
}

/**
 *  Method used to wait and read stecom response
 */
int waitAndReadStecomResp(mpl_msg_t* resp_p) {
  char buf[MAX_READ_BUFFER_SIZE];
  int size;

  if (waitForEvent((int) ctrl_handle_p->sock) < 0) {
    printf("waitAndReadStecomEvent : waitForEvent failed\n");
    goto error;
  }
  size = stec_recv(ctrl_handle_p, buf, 1000, TIMEOUTTIME);
  if (size < 0) {
    printf("waitAndReadStecomEvent : stec_recv failed\n");
    goto error;
  }
  if (mpl_msg_unpack(buf, size, resp_p) < 0) {
    printf("waitAndReadStecomEvent : mpl_msg_unpack failed\n");
    goto error;
  }

  return 0;
  error: return -1;
}

/**
 *  Method used to wait and read stecom event
 */
int waitAndReadStecomEvent(mpl_msg_t* resp_p) {
  char buf[MAX_READ_BUFFER_SIZE];
  int size;

  if (waitForEvent((int) event_handle_p->sock) < 0) {
    printf("waitAndReadStecomEvent : waitForEvent failed\n");
    goto error;
  }
  size = stec_recv(event_handle_p, buf, 1000, TIMEOUTTIME);
  if (size < 0) {
    printf("waitAndReadStecomEvent : stec_recv failed\n");
    goto error;
  }
  if (mpl_msg_unpack(buf, size, resp_p) < 0) {
    printf("waitAndReadStecomEvent : mpl_msg_unpack failed\n");
    goto error;
  }

  return 0;
  error: return -1;
}
/*
 * Method for listening to a fd
 */
int waitForEvent(int fd) {
  fd_set rfds;
  struct timeval tv, *tvp;
  int result = 0;

  if (fd < 0) {
    printf("waitForEvent: fd < 0 \n");
    return -1;
  }

  /* Set up timeout structure */
  if (TIMEOUTTIME < 0) {
    tvp = NULL;
  } else {
    tv.tv_sec = TIMEOUTTIME / 1000;
    tv.tv_usec = (TIMEOUTTIME % 1000) * 1000;
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
 *  Logfunction that is being registered to stecom
 */
void my_log_func(void* user_p, int level, const char* file, int line,
    const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vsyslog(level, format, ap);
  vprintf(format, ap);
  va_end(ap);
}
