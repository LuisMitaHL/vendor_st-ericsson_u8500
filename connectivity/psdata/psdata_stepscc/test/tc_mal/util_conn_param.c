/*
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : util_conn_param.c
 * Description     : wrapper for using mal get/set params
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "common.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mal_gpds.h>

/***************************************/
/********* GET functions ***************/
/***************************************/

/*
 * Method used when expecting a gpds set but ignore the values
 */
int waitAndRespondToGPDSSet_DontCare(int retval)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_APN(int retval, char apn[], int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_APN_ID);
  if (apn != (char*) DONT_CARE)
    TESTBOOL(strcmp((char*) request_p->data, (char*) apn), 0);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_AuthMeth(int retval, mal_gpds_auth_method_t auth_meth, int cid)
{  /* wait for stub gpds to send us the request, then answer */


  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));

  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_AUTH_METHOD_ID);

  if (auth_meth != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_type_t)request_p->data[0], auth_meth);
  free(request_p);

  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_PDPContextType(int retval, mal_gpds_pdp_context_type_t pdp_context_type, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_PDP_CONTEXT_TYPE_ID);
  if (pdp_context_type != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_type_t)request_p->data[0], pdp_context_type);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}


int waitAndRespondToGPDSSet_PDPType(int retval, mal_gpds_pdp_type_t pdp_type, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_PDP_TYPE_ID);
  if (pdp_type != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_type_t)request_p->data[0], pdp_type);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_HCMP(int retval, mal_gpds_pdp_hcmp_t hcmp, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_HCMP_ID);
  if (hcmp != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_hcmp_t)request_p->data[0], hcmp);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_HCMP_on(int retval, mal_gpds_pdp_hcmp_t hcmp, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_HCMP_ID);
  if (hcmp != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_hcmp_t)request_p->data[0], hcmp);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_IPAddress(int retval, ip_address_t ip, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IP_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_DCMP(int retval, mal_gpds_pdp_dcmp_t dcmp, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_DCMP_ID);
  if (dcmp != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_dcmp_t)request_p->data[0], dcmp);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}


int waitAndRespondToGPDSSet_DCMP_on(int retval, mal_gpds_pdp_dcmp_t dcmp, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_DCMP_ID);
  if (dcmp != DONT_CARE)
    TESTBOOL((mal_gpds_pdp_dcmp_t)request_p->data[0], dcmp);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_QoSReq(int retval, mal_gpds_qos_profile_t qosreq, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_QOS_REQ_ID);
  //TODO check qos
  //if (qosreq != DONT_CARE)
  //TESTBOOL(strcmp((char*) request_p->data_string, (char*) qosreq), 0);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_QoSRel5(int retval, mal_gpds_qos_profile_t qosrel5, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_QOS_REL5_ID);
  //TODO check qos
  //if (qosrel5 != DONT_CARE)
  //TESTBOOL(strcmp((char*) request_p->data_string, (char*) qosrel5), 0);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_QoSMin(int retval, mal_gpds_qos_profile_t qosmin, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_QOS_MIN_ID);
  //TODO check qos
  //if (qosmin != DONT_CARE)
  //TESTBOOL(strcmp((char*) request_p->data_string, (char*) qosmin), 0);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

/***************************************/
/********* GET functions ***************/
/***************************************/

/*
 * Method used when expecting a gpds get but ignore the values (return 0 struct)
 */
int waitAndRespondToGPDSGet_DontCare(int retval)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_IPMAP(int retval, int ip_map, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IP_MAP_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (ip_map != 0)
  {
    memcpy((void*) response.data, (void*) &ip_map, sizeof(int));
    response.datasize = sizeof(int);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_IP(int retval, ip_address_t ip, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IP_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (ip != (void*) DONT_CARE)
  {
    memcpy((void*) response.data, (void*) ip, sizeof(ip_address_t));
    response.datasize = sizeof(ip_address_t);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_IPv6(int retval, ip_address_t ipv6, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IPV6_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (ipv6 != (void*) DONT_CARE)
  {
    memcpy((void*) response.data, (void*) ipv6, sizeof(ip_address_t));
    response.datasize = sizeof(ip_address_t);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_PDNS(int retval, ip_address_t pdns, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_PDNS_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (pdns != (void*) DONT_CARE)
  {
    memcpy((void*) response.data, (void*) pdns, sizeof(ip_address_t));
    response.datasize = sizeof(ip_address_t);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_PDNSv6(int retval, ip_address_t ipv6_pdns, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IPV6_PDNS_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (ipv6_pdns != (void*) DONT_CARE)
  {
    memcpy((void*) response.data, (void*) ipv6_pdns, sizeof(ip_address_t));
    response.datasize = sizeof(ip_address_t);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}


int waitAndRespondToGPDSGet_SDNS(int retval, ip_address_t sdns, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_SDNS_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (sdns != (void*)DONT_CARE)
  {
    memcpy((void*) response.data, (void*) sdns, sizeof(ip_address_t));
    response.datasize = sizeof(ip_address_t);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_SDNSv6(int retval, ip_address_t ipv6_sdns, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IPV6_SDNS_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (ipv6_sdns != (void*)DONT_CARE)
  {
    memcpy((void*) response.data, (void*) ipv6_sdns, sizeof(ip_address_t));
    response.datasize = sizeof(ip_address_t);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_IFNAME(int retval, char ifname[], int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_IF_NAME_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  if (ifname != (void*)DONT_CARE)
  {
    strcpy((char*) response.data, (char*) ifname);
    response.datasize = strlen(ifname);
  }
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSGet_QoSNeg(int retval, mal_gpds_qos_profile_t qosneg, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_get_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_GET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_QOS_NEG_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  memcpy((void*) response.data, &qosneg, sizeof(qosneg));
  response.datasize = sizeof(mal_gpds_qos_profile_t);
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_GET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

#if 1



int waitAndRespondToGPDSSet_ChapId(int retval, uint8_t ChapId, int cid)
{  /* wait for stub gpds to send us the request, then answer */
  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_CHAP_IDENTIFIER_ID);
  if (ChapId != DONT_CARE)
    TESTBOOL((uint8_t)request_p->data[0], ChapId);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_Username(int retval, void *user, int cid)
{  /* wait for stub gpds to send us the request, then answer */

  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_USERNAME_ID);
  if (user != DONT_CARE)
      TESTBOOL(strcmp((char*) request_p->data, (char*) user), 0);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}


int waitAndRespondToGPDSSet_Password(int retval, void *password, int cid)

{  /* wait for stub gpds to send us the request, then answer */

  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_PASSWORD_ID);
  if (password != DONT_CARE)
      TESTBOOL(strcmp((char*) request_p->data, (char*) password), 0);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

int waitAndRespondToGPDSSet_Challenge(int retval, void *challenge, int cid)
{  /* wait for stub gpds to send us the request, then answer */

  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_CHALLENGE_DATA_ID);
  if(NULL != challenge) {
     printf("%d:%d:%d:%d:%d", request_p->data[0],request_p->data[1],request_p->data[2],request_p->data[3],request_p->data[4]);
  }
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}



int waitAndRespondToGPDSSet_Response(int retval, void *challenge_response, int cid)
{  /* wait for stub gpds to send us the request, then answer */

  INITREQANDRESP_TEST(mal_gpds_set_param);
  TESTNOTNEG(waitAndReadGPDSEvent(PSCCD_TEST_STUB_TEST, PSCC_MAL_GPDS_SET_PARAM, (void**) &request_p));
  TESTBOOL(request_p->conn_id, cid);
  TESTBOOL(request_p->param_id, MAL_PARAMS_RESPONSE_DATA_ID);
  free(request_p);
  /* need to send the response back */
  response.retval = retval;
  TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_SET_PARAM, &response,
          sizeof(response)));

  return 0;
  error:
  return -1;
}

#endif
/***************************************/
/********* Misc functions **************/
/***************************************/


