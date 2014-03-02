/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : sim.c
 * Description     : sim stub
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "sim.h"
#include "util.h"
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

ste_sim_t ste_sim;
int sim_fd = -1;
/* callback pointer */
ste_sim_closure_t cb_func_p;

/*******************/
/* Implementation */
/*******************/

ste_sim_t* ste_sim_new_st(const ste_sim_closure_t * closure)
{
  PSCC_test_ste_sim_new_st_req_t request;
  PSCC_test_ste_sim_new_st_resp_t *response_p;
  PRINTDEBUGENTER(ste_sim_new_st);

  cb_func_p = *closure;

  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToSim(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_NEW_ST, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_NEW_ST, (void**) &response_p));
  free(response_p);

  /* fetch initialized fd */
  getSimFd(&sim_fd);
  ste_sim.fd = sim_fd;

  PRINTDEBUGEXIT(ste_sim_new_st);
  return &ste_sim;
  error:
  return NULL;
}

int ste_sim_connect(ste_sim_t * sim,uintptr_t client_tag)
{
  PSCC_test_ste_sim_connect_req_t request;
  PSCC_test_ste_sim_connect_resp_t *response_p;
  int retval;
  PRINTDEBUGENTER(ste_sim_connect);

  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToSim(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_CONNECT, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CONNECT, (void**) &response_p));
  retval = response_p->retval;
  free(response_p);

  PRINTDEBUGEXIT(ste_sim_connect);
  return retval;
  error:
  return -1;
}

int ste_sim_fd(const ste_sim_t* sim)
{
  PSCC_test_ste_sim_fd_req_t request;
  PSCC_test_ste_sim_fd_resp_t *response_p;
  PRINTDEBUGENTER(ste_sim_fd);

  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToSim(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_FD, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_FD, (void**) &response_p));
  free(response_p);

  PRINTDEBUGEXIT(ste_sim_fd);
  return sim_fd;
  error:
  return -1;
}

int ste_sim_disconnect(ste_sim_t * sim, uintptr_t client_tag)
{
  PRINTDEBUGENTER(ste_sim_disconnect);
  /* here the testapplication should already be shutdown so we cannot send request */
  PRINTDEBUGEXIT(ste_sim_disconnect);
  return 0;
}

void ste_sim_delete(ste_sim_t * sim, uintptr_t client_tag)
{
  PRINTDEBUGENTER(ste_sim_delete);
  /* here the testapplication should already be shutdown so we cannot send request */
  PRINTDEBUGEXIT(ste_sim_delete);
  return;
}

int ste_cat_call_control(ste_sim_t* cat, uintptr_t client_tag, ste_cat_call_control_t * cc_p)
{
  PSCC_test_ste_cat_call_control_req_t request;
  PSCC_test_ste_cat_call_control_resp_t *response_p;
  int retval = 0;
  ste_cat_cc_pdp_t* pdp_p = cc_p->cc_data.pdp_p;
  PRINTDEBUGENTER(ste_cat_call_control);

  /* send request */
  memset(&request, 0, sizeof(request));
  request.client_tag = client_tag;
  memcpy(&request.str_p, pdp_p->pdp_context.str_p, pdp_p->pdp_context.no_of_bytes);
  request.strlen = pdp_p->pdp_context.no_of_bytes;
  TESTNOTNEG(sendToSim(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_CALL_CONTROL, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CALL_CONTROL, (void**) &response_p));
  retval = response_p->retval;
  free(response_p);

  PRINTDEBUGEXIT(ste_cat_call_control);
  return retval;
  error:
  return -1;
}

int ste_sim_read(ste_sim_t* sim)
{
  PSCC_test_ste_sim_read_req_t request;
  PSCC_test_ste_sim_read_resp_t *response_p;
  PSCC_test_ste_sim_cb_t *callback_p;
  PRINTDEBUGENTER(ste_sim_read);
  int retval = 0;

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, (void**) &callback_p));

  /* send request */
  memset(&request, 0, sizeof(request));
  TESTNOTNEG(sendToSim(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_READ, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_READ, (void**) &response_p));
  switch(callback_p->cause)
  {
    case STE_CAT_CAUSE_SIM_EC_CALL_CONTROL:
    {
      /* special treatment */
      ste_cat_call_control_response_t* resp_p = (ste_cat_call_control_response_t*) callback_p->data;
      ste_cat_cc_pdp_t* ste_cat_cc_pdp_p = (ste_cat_cc_pdp_t*) callback_p->user_data;
      resp_p->cc_info.cc_data.pdp_p = ste_cat_cc_pdp_p;
      cb_func_p.func(callback_p->cause, callback_p->client_tag, callback_p->data, NULL);
    }
    break;
    default:
      cb_func_p.func(callback_p->cause, callback_p->client_tag, callback_p->data, NULL);
  }
  free(response_p);
  free(callback_p);

  PRINTDEBUGEXIT(ste_sim_read);
  return retval;
  error:
  return -1;
}

uicc_request_status_t ste_uicc_get_service_availability ( ste_sim_t * uicc,
                                                          uintptr_t client_tag,
                                                          sim_service_type_t service_type)
{
  PSCC_test_ste_uicc_get_service_availability_req_t request;
  PSCC_test_ste_uicc_get_service_availability_resp_t *response_p;
  uicc_request_status_t retval;
  PRINTDEBUGENTER(ste_uicc_get_service_availability);

  /* send request */
  memset(&request, 0, sizeof(request));
  request.client_tag = client_tag;
  request.service_type = service_type;
  TESTNOTNEG(sendToSim(PSCCD_TEST_STUB_TEST, PSCC_STE_UICC_GET_SERVICE_AVAILABILITY, &request, sizeof(request)));

  /* wait for the response */
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_TEST_STUB, PSCC_STE_UICC_GET_SERVICE_AVAILABILITY, (void**) &response_p));
  retval = response_p->retval;
  free(response_p);

  PRINTDEBUGEXIT(ste_uicc_get_service_availability);
  return retval;
  error:
  return UICC_REQUEST_STATUS_FAILED;
}
