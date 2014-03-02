/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : mal_gpds.c
 * Description     : stub of mal gpds
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "mal_gpds.h"
#include "shm_netlnk.h"
#include "util.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int gpds_fd; /* stub -> test */
int netlnk_fd; /* test -> stub */
mal_gpds_event_cb_t event_cb_p = NULL;
static int pscc_mal_fd_pipe[2] = {-1,-1};
static int mal_failure = 0;

#define offset_td_200 ((uint16_t)200)
#define offset_td_1000 ((uint16_t)1000)

#define Mbps ((uint16_t)1000)

#define base_octet_offset_64 ((uint16_t)64)
#define base_octet_offset_576 ((uint16_t)576)
#define extended_octet_offset_8600 ((uint16_t)8600)
#define extended_octet_offset_16 ((uint16_t)16)
#define extended_octet_offset_128 ((uint16_t)128)
#define extended_octet_maxval ((uint16_t)256)

uint16_t mal_sdu_size[5] = {0, 1502, 1510, 1520, 1555};
uint16_t mal_transfer_delay[4] = {4010, offset_td_1000, offset_td_200, 0};
uint16_t mal_max_bitrate[] = {0, 1, base_octet_offset_64, base_octet_offset_576, 8700, 17000};
int mal_gpds_init(int *fd_gpds, int *fd_netlnk, int *fd_pipe) {
  PSCC_test_mal_gpds_init_req_t request;
  PSCC_test_mal_gpds_init_resp_t *response_p;
  int retval = 0;
  static int i = 0;

  PRINTDEBUGENTER(mal_gpds_init);

  /* setup test framework connection */
  TESTBOOL(createSockets(0), 0);
  getGPDSFd(&gpds_fd, &netlnk_fd);
  *fd_gpds = gpds_fd;
  *fd_netlnk = netlnk_fd;

  /* we create a unidirectional pipe and we never write to the write end of it */
  if(pipe(pscc_mal_fd_pipe) < 0)
  {
    goto error;
  }
  /* return the read end of the pipe */
  *fd_pipe = pscc_mal_fd_pipe[0];

  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_INIT, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_INIT, (void**) &response_p));
  retval = response_p->retval;
  free(response_p);

  PRINTDEBUGEXIT(mal_gpds_init);
  return retval;
  error:
  return -1;
}

void mal_gpds_deinit(void) {
  PRINTDEBUGENTER(mal_gpds_deinit);
  /* here the testapplication should already be shutdown so we cannot send request */
  void closeSockets();

  close(pscc_mal_fd_pipe[0]);
  close(pscc_mal_fd_pipe[1]);
  PRINTDEBUGEXIT(mal_gpds_deinit);
}

void mal_gpds_register_callback(mal_gpds_event_cb_t event_cb) {
  PSCC_test_mal_gpds_register_callback_req_t request;
  PSCC_test_mal_gpds_register_callback_resp_t *response_p;
  PRINTDEBUGENTER(mal_gpds_register_callback);
  event_cb_p = event_cb;

  /* send request */
  memset(&request, 0, sizeof(request));
  request.event_cb = event_cb;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_REGISTER_CALLBACK, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_REGISTER_CALLBACK, (void**) &response_p));
  free(response_p);

  PRINTDEBUGEXIT(mal_gpds_register_callback);
  error:
  return;
}

int mal_gpds_request(uint8_t conn_id, mal_gpds_req_ids_t req_id, void *data) {
  PSCC_test_mal_gpds_request_req_t request;
  PSCC_test_mal_gpds_request_resp_t *response_p;
  int retval = 0;
  PRINTDEBUGENTER(mal_gpds_request);

  /* send request */
  memset(&request, 0, sizeof(request));
  request.conn_id = conn_id;
  request.req_id = req_id;

  switch (req_id)
  {
  case MAL_GPDS_REQ_RESOURCE_CONTROL:
    if (data != NULL)
      memcpy(request.data, data, sizeof(mal_gpds_resource_control_req_info_t));
    break;
  case MAL_GPDS_REQ_PSCONN_ACTIVATE:
    if (data != NULL)
      memcpy(request.data, data, sizeof(mal_gpds_phonet_conf_t));
    break;
  default:
    /* contains no data */
    break;
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_REQUEST, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_REQUEST, (void**) &response_p));
  retval = response_p->retval;
  free(response_p);

  PRINTDEBUGEXIT(mal_gpds_request);
  return retval;
  error:
  return -1;
}

int mal_gpds_set_param(uint8_t conn_id, mal_gpds_params_id_t param_id,
    void *data) {
  PSCC_test_mal_gpds_set_param_req_t request;
  PSCC_test_mal_gpds_set_param_resp_t *response_p;
  int retval = 0;
  PRINTDEBUGENTER(mal_gpds_set_param);

  /* send request */
  memset(&request, 0, sizeof(request));
  request.conn_id = conn_id;
  request.param_id = param_id;
  switch (param_id)
  {
/* treat strings differently to avoid over memory reading (warnings in valgrind)*/
  case MAL_PARAMS_APN_ID:
  case MAL_PARAMS_USERNAME_ID:
  case MAL_PARAMS_PASSWORD_ID:
    strcpy((void*) request.data, (char *)data);
    break;
  case MAL_PARAMS_CHALLENGE_DATA_ID:
  case MAL_PARAMS_RESPONSE_DATA_ID:
    memcpy((void*)request.data, data, sizeof(mal_gpds_chap_auth_data_t));
    break;
/* and int differently */
  case MAL_PARAMS_PDP_TYPE_ID:
  case MAL_PARAMS_PDP_CONTEXT_TYPE_ID:
  case MAL_PARAMS_HCMP_ID:
  case MAL_PARAMS_DCMP_ID:
  case MAL_PARAMS_AUTH_METHOD_ID:
    memcpy((void*) request.data, data, sizeof(int));
    break;
/* qos structs */
  case MAL_PARAMS_QOS_REQ_ID:
  case MAL_PARAMS_QOS_MIN_ID:
  case MAL_PARAMS_QOS_REL5_ID:
  case MAL_PARAMS_QOS_NEG_ID:
    memcpy((void*) request.data, data, sizeof(mal_gpds_qos_profile_t));
    break;
  default:
    memcpy((void*) request.data, data, sizeof(request.data));
    break;
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, (void**) &response_p));
  retval = response_p->retval;
  free(response_p);

  PRINTDEBUGEXIT(mal_gpds_set_param);
  return retval;
  error:
  return -1;
}

int mal_gpds_get_param(uint8_t conn_id, mal_gpds_params_id_t param_id,
    void *data) {
  PSCC_test_mal_gpds_get_param_req_t request;
  PSCC_test_mal_gpds_get_param_resp_t *response_p;
  int retval = 0;
  static uint8_t coverage_idx = 0;
  PRINTDEBUGENTER(mal_gpds_get_param);

  /* send request */
  memset(&request, 0, sizeof(request));
  request.conn_id = conn_id;
  request.param_id = param_id;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, (void**) &response_p));
  memcpy(data, response_p->data, response_p->datasize);
  switch(param_id)
  {
     case MAL_PARAMS_QOS_NEG_ID:
        ((mal_gpds_qos_profile_t*)data)->maximum_sdu_size = mal_sdu_size[coverage_idx%5];
        ((mal_gpds_qos_profile_t*)data)->transfer_delay = mal_transfer_delay[coverage_idx%4];
        ((mal_gpds_qos_profile_t*)data)->maximum_bitrate_ul = mal_max_bitrate[coverage_idx%6];
        ((mal_gpds_qos_profile_t*)data)->maximum_bitrate_dl = mal_max_bitrate[coverage_idx%6];
        coverage_idx++;
        break;
     default:
        break;
  }
  free(response_p);

  PRINTDEBUGEXIT(mal_gpds_get_param);
  return retval;
  error:
  return -1;
}

void mal_gpds_response_handler(int fd) {
  PSCC_test_mal_gpds_response_handler_req_t request;
  PSCC_test_mal_gpds_response_handler_resp_t *response_p;
  PSCC_test_mal_gpds_cb_t *callback_p;
  PRINTDEBUGENTER(mal_gpds_response_handler);

  TESTNOTNEG(gpds_fd == fd);

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, (void**) &callback_p));

  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_RESPONSE_HANDLER, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_RESPONSE_HANDLER, (void**) &response_p));

  switch (callback_p->event_id)
  {
  case MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED:
  {
    /* need to set pointer to correct data */
    mal_gpds_resource_control_info_t* mal_gpds_resource_control_info_p = (mal_gpds_resource_control_info_t*) callback_p->data;
    mal_gpds_resource_control_info_p->resource_control_data.data = (uint8_t*) callback_p->extradata;
  }
    break;
  default:
    break;
  }
  event_cb_p(callback_p->conn_id, callback_p->event_id, &callback_p->data);

  free(callback_p);
  free(response_p);

  PRINTDEBUGEXIT(mal_gpds_response_handler);
  error:
  return;
}

int32_t netlnk_socket_recv(int32_t netlnkfd, int *msg) {
  PSCC_test_netlnk_socket_recv_req_t request;
  PSCC_test_netlnk_socket_recv_resp_t *response_p;
  PSCC_test_netlnk_cb_t *callback_p;
  int retval = 0;
  PRINTDEBUGENTER(netlnk_socket_recv);

  TESTNOTNEG(netlnk_fd == netlnk_fd);

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_NETLNK_CALLBACK, (void**) &callback_p));
  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_STUB_TEST, PSCC_NETLNK_SOCKET_RECV, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_TEST_STUB, PSCC_NETLNK_SOCKET_RECV, (void**) &response_p));
  *msg = callback_p->msg;//MODEM_RESET_IND;
  free(callback_p);
  free(response_p);

  PRINTDEBUGEXIT(netlnk_socket_recv);
  return retval;
  error:
  return -1;
}

ssize_t sendmsg(int socket, const struct msghdr *message, int flags)
{
    static int i = 0;
    if(i == 0) {
      i++;
      printf("returnging -1");
      return -1;
    } else if (i == 1) {
      i++;
      printf("returnging 0");
      return 0;
    } else {
      printf("returnging 512");
      return 512;
    }
}

int ioctl(void)
{
 return 0;
}

