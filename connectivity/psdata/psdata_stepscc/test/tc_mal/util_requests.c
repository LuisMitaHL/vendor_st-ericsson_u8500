/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util_requests.c
 * Description     : wrapper for using mal request functions
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "common.h"
#include "util.h"
#include "util_requests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int waitAndRespondToGPDSReq_Init(int retval)
{
  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_init);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_INIT, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_INIT, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSReq_RegisterCallback()
{
  INITREQANDRESP_TEST(mal_gpds_register_callback);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_REGISTER_CALLBACK, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_REGISTER_CALLBACK, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSReq_Request(int retval, int connid, mal_gpds_req_ids_t req_id, void* data, int datalen)
{
  INITREQANDRESP_TEST(mal_gpds_request);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_REQUEST, (void**) &request_p));
  TESTBOOL(request_p->conn_id, connid);
  TESTBOOL(memcmp(request_p->data, data, datalen), 0);
  TESTBOOL(request_p->req_id, req_id);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_REQUEST, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSReq_ResponseHandler()
{
  INITREQANDRESP_TEST(mal_gpds_response_handler);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_RESPONSE_HANDLER, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_RESPONSE_HANDLER, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_sim_new_st(ste_sim_t ste_sim, ste_sim_closure_t closure)
{
  INITREQANDRESP_TEST(ste_sim_new_st);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_NEW_ST, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_NEW_ST, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_sim_connect(int retval, ste_sim_t sim, uintptr_t client_tag)
{
  INITREQANDRESP_TEST(ste_sim_connect);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_CONNECT, (void**) &request_p));
  TESTBOOL(request_p->client_tag, client_tag);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CONNECT, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_sim_fd(int fd, ste_sim_t sim)
{
  INITREQANDRESP_TEST(ste_sim_fd);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_FD, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_FD, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_sim_disconnect(int retval, ste_sim_t sim, uintptr_t client_tag)
{
  INITREQANDRESP_TEST(ste_sim_disconnect);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_DISCONNECT, (void**) &request_p));
  TESTBOOL(request_p->client_tag, client_tag);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_DISCONNECT, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_sim_delete(int retval, ste_sim_t sim, uintptr_t client_tag)
{
  INITREQANDRESP_TEST(ste_sim_delete);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_DELETE, (void**) &request_p));
  TESTBOOL(request_p->client_tag, client_tag);
  free(request_p);
  /* need to send the response back */
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_DELETE, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_sim_read(int retval, ste_sim_t sim)
{
  INITREQANDRESP_TEST(ste_sim_read);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_READ, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_READ, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_uicc_get_service_availability(int retval, ste_sim_t sim, uintptr_t *client_tag_p,sim_service_type_t service_type)
{
  INITREQANDRESP_TEST(ste_uicc_get_service_availability);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_UICC_GET_SERVICE_AVAILABILITY, (void**) &request_p));
  *client_tag_p = request_p->client_tag;
  TESTBOOL(request_p->service_type, service_type);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_UICC_GET_SERVICE_AVAILABILITY, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}

int waitAndRespondToSim_ste_cat_call_control(int retval, ste_sim_t sim, uintptr_t client_tag, char* pdpactdata_p, uint32_t strlen)
{
  INITREQANDRESP_TEST(ste_cat_call_control);
  TESTNOTNEG(waitAndReadSimEvent(PSCCD_TEST_STUB_TEST, PSCC_STE_SIM_CALL_CONTROL, (void**) &request_p));
  TESTBOOL(strcmp((char*) request_p->str_p, pdpactdata_p), 0);
  TESTBOOL(request_p->client_tag, client_tag);
  TESTBOOL(request_p->strlen, strlen);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CALL_CONTROL, &response,
          sizeof(response)));
  return 0;
  error:
  return -1;
}


