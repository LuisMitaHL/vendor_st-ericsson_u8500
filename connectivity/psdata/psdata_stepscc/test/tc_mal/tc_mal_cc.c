/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : tc_mal_cc.c
 * Description     : call control over mal tests
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "tc_mal_basic.h"
#include "util.h"
#include "util_scenarios.h"
#include "util_conn_param.h"
#include "util_requests.h"
#include "pscc_msg.h"

psccd_test_state_type_t tc_cc_allow(int state)
{
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  ste_sim_t sim;
  char pdpactmsg[] = "apn1,pco1,nsapi1";
  mal_gpds_resource_control_info_t mal_gpds_resource_control_info;
  uintptr_t clienttag = 0;

  /* send a SIM Disconnect event to PSCC */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_SIM_CAUSE_DISCONNECT;
    callback.client_tag = clienttag;
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  /* send a SIM Connect event to PSCC */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_SIM_CAUSE_CONNECT;
    callback.client_tag = clienttag;
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  /* set service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  TESTNOTNEG(util_create(cid));

  /* perform connect */
  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* connect */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_APN(0, apn, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_DCMP(0, MAL_PDP_DCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_HCMP(0, MAL_PDP_HCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_resource_control_info, 0, sizeof(mal_gpds_resource_control_info));
    mal_gpds_resource_control_info.seq_id = 101;
    mal_gpds_resource_control_info.resource_control_data.data_length = strlen(pdpactmsg);
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED;
    memcpy(&callback.data, &mal_gpds_resource_control_info, sizeof(mal_gpds_resource_control_info));
    memcpy(&callback.extradata, &pdpactmsg, strlen(pdpactmsg));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* clienttag is same as seq_id */
  TESTNOTNEG(waitAndRespondToSim_ste_cat_call_control(0, sim, 101, (char*) pdpactmsg, (uint32_t) strlen(pdpactmsg)));

  {
    PSCC_test_ste_sim_cb_t callback;
    ste_cat_call_control_response_t ste_cat_call_control_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_cat_call_control_response, 0, sizeof(ste_cat_call_control_response));
    callback.cause = STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
    callback.client_tag = 101;
    ste_cat_call_control_response.cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
    memcpy(&callback.data, &ste_cat_call_control_response, sizeof(ste_cat_call_control_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  {
    mal_gpds_resource_control_req_info_t mal_gpds_resource_control_req_info;
    memset(&mal_gpds_resource_control_req_info, 0, sizeof(mal_gpds_resource_control_req_info));
    mal_gpds_resource_control_req_info.seq_id = mal_gpds_resource_control_info.seq_id;
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_ALLOWED;
    TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONTROL, &mal_gpds_resource_control_req_info, sizeof(mal_gpds_resource_control_req_info_t)));
  }

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* we are now resource controlled and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV4_MAP), cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IP(0, ip, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_PDNS(0, pdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_SDNS(0, sdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IFNAME(0, ifname, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  /* reset service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;

}

psccd_test_state_type_t tc_cc_not_allow(int state)
{
  int cid = 1;
  char apn[] = "default-apn.services";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  ste_sim_t sim;
  char pdpactmsg[] = "apn1,pco1,nsapi1";
  mal_gpds_resource_control_info_t mal_gpds_resource_control_info;
  uintptr_t clienttag = 0;

  /* set service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  TESTNOTNEG(util_create(cid));

  /* perform connect */
  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* connect */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_APN(0, apn, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_DCMP(0, MAL_PDP_DCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_HCMP(0, MAL_PDP_HCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_resource_control_info, 0, sizeof(mal_gpds_resource_control_info));
    mal_gpds_resource_control_info.seq_id = 101;
    mal_gpds_resource_control_info.resource_control_data.data_length = strlen(pdpactmsg);
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED;
    memcpy(&callback.data, &mal_gpds_resource_control_info, sizeof(mal_gpds_resource_control_info));
    memcpy(&callback.extradata, &pdpactmsg, strlen(pdpactmsg));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToSim_ste_cat_call_control( 0, sim, 101, (char*) pdpactmsg, (uint32_t) strlen(pdpactmsg)));

  {
    PSCC_test_ste_sim_cb_t callback;
    ste_cat_call_control_response_t ste_cat_call_control_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_cat_call_control_response, 0, sizeof(ste_cat_call_control_response));
    callback.cause = STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
    callback.client_tag = 101;
    ste_cat_call_control_response.cc_result = STE_CAT_CC_NOT_ALLOWED;
    memcpy(&callback.data, &ste_cat_call_control_response, sizeof(ste_cat_call_control_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  {
    mal_gpds_resource_control_req_info_t mal_gpds_resource_control_req_info;
    memset(&mal_gpds_resource_control_req_info, 0, sizeof(mal_gpds_resource_control_req_info));
    mal_gpds_resource_control_req_info.seq_id = mal_gpds_resource_control_info.seq_id;
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_REJECTED;
    TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONTROL, &mal_gpds_resource_control_req_info, sizeof(mal_gpds_resource_control_req_info_t)));
  }

  /* gpds will send a activation failed event */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_disconnected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_reason,(resp.common.param_list_p)), pscc_reason_pdp_context_activation_failed);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  TESTNOTNEG(util_destroy(cid));

  /* reset service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_cc_allow_mod(int state)
{
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  ste_sim_t sim;
  char pdpactmsg[] = "apn1,pco1,nsapi1";
  char pdpactmsg_mod[] = "apn2,pco2,nsapi2";
  mal_gpds_resource_control_info_t mal_gpds_resource_control_info;
  uintptr_t clienttag = 0;

  /* set service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  TESTNOTNEG(util_create(cid));

  /* perform connect */
  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* connect */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_APN(0, apn, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_DCMP(0, MAL_PDP_DCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_HCMP(0, MAL_PDP_HCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_resource_control_info, 0, sizeof(mal_gpds_resource_control_info));
    mal_gpds_resource_control_info.seq_id = 101;
    mal_gpds_resource_control_info.resource_control_data.data_length = strlen(pdpactmsg);
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED;
    memcpy(&callback.data, &mal_gpds_resource_control_info, sizeof(mal_gpds_resource_control_info));
    memcpy(&callback.extradata, &pdpactmsg, strlen(pdpactmsg));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToSim_ste_cat_call_control(0, sim, 101, (char*) pdpactmsg, (uint32_t) strlen(pdpactmsg)));

  {
    PSCC_test_ste_sim_cb_t callback;
    ste_cat_call_control_response_t ste_cat_call_control_response;
    ste_cat_cc_pdp_t ste_cat_cc_pdp;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_cat_call_control_response, 0, sizeof(ste_cat_call_control_response));
    callback.cause = STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
    callback.client_tag = 101;
    ste_cat_call_control_response.cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    ste_cat_cc_pdp.pdp_context.str_p = (uint8_t*) pdpactmsg_mod;
    ste_cat_cc_pdp.pdp_context.no_of_bytes = strlen(pdpactmsg_mod);
    memcpy(&callback.user_data, &ste_cat_cc_pdp, sizeof(ste_cat_cc_pdp_t));
    memcpy(&callback.data, &ste_cat_call_control_response, sizeof(ste_cat_call_control_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  {
    mal_gpds_resource_control_req_info_t mal_gpds_resource_control_req_info;
    memset(&mal_gpds_resource_control_req_info, 0, sizeof(mal_gpds_resource_control_req_info));
    mal_gpds_resource_control_req_info.seq_id = mal_gpds_resource_control_info.seq_id;
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_MODIFIED;
    mal_gpds_resource_control_req_info.resource_control_data.data = (uint8_t*) pdpactmsg_mod;
    mal_gpds_resource_control_req_info.resource_control_data.data_length = strlen(pdpactmsg_mod);
    TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONTROL, &mal_gpds_resource_control_req_info, sizeof(mal_gpds_resource_control_req_info_t)));
  }

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* we are now resource controlled and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV4_MAP), cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IP(0, ip, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_PDNS(0, pdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_SDNS(0, sdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IFNAME(0, ifname, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  /* reset service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_cc_denied_by_gpds(int state)
{
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t pdns;
  ip_address_t sdns;
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  ste_sim_t sim;
  char pdpactmsg[] = "apn1,pco1,nsapi1";
  char pdpactmsg_mod[] = "apn2,pco2,nsapi2";
  mal_gpds_resource_control_info_t mal_gpds_resource_control_info;
  uintptr_t clienttag = 0;


  /* set service availability with status code STE_UICC_STATUS_CODE_FAIL*/
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  /* send STE_UICC_CAUSE_SIM_STATE_CHANGED with state SIM_STATE_UNKNOWN*/
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_sim_state_changed_t ste_uicc_state_changed_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_state_changed_response, 0, sizeof(ste_uicc_state_changed_response));
    callback.cause = STE_UICC_CAUSE_SIM_STATE_CHANGED;
    callback.client_tag = clienttag;
    ste_uicc_state_changed_response.state = SIM_STATE_UNKNOWN;
    memcpy(&callback.data, &ste_uicc_state_changed_response, sizeof(ste_uicc_sim_state_changed_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

   TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

#if 1
  /* send STE_UICC_CAUSE_SIM_STATE_CHANGED with state EXE_SIM_STATE_UNKNOWN */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_sim_state_changed_t ste_uicc_state_changed_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_state_changed_response, 0, sizeof(ste_uicc_state_changed_response));
    callback.cause = STE_UICC_CAUSE_SIM_STATE_CHANGED;
    callback.client_tag = clienttag;
    ste_uicc_state_changed_response.state = SIM_STATE_READY;
    memcpy(&callback.data, &ste_uicc_state_changed_response, sizeof(ste_uicc_sim_state_changed_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  TESTNOTNEG(waitAndRespondToSim_ste_uicc_get_service_availability(-1, sim,  &clienttag, SIM_SERVICE_TYPE_CALL_CONTROL_ON_GPRS));


  /* send STE_CAT_CAUSE_SIM_EC_CALL_CONTROL with result STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_cat_call_control_response_t ste_cat_call_control_response;
    ste_cat_cc_pdp_t ste_cat_cc_pdp;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_cat_call_control_response, 0, sizeof(ste_cat_call_control_response));
    callback.cause = STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
    callback.client_tag = 101;
    ste_cat_call_control_response.cc_result = STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM;
    ste_cat_cc_pdp.pdp_context.str_p = (uint8_t*) pdpactmsg_mod;
    ste_cat_cc_pdp.pdp_context.no_of_bytes = strlen(pdpactmsg_mod);
    memcpy(&callback.user_data, &ste_cat_cc_pdp, sizeof(ste_cat_cc_pdp_t));
    memcpy(&callback.data, &ste_cat_call_control_response, sizeof(ste_cat_call_control_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }
#endif
  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));



  /* set service availability with status code STE_UICC_STATUS_CODE_OK */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  TESTNOTNEG(util_create(cid));

  /* perform connect */
  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* connect */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_APN(0, apn, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_DCMP(0, MAL_PDP_DCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_HCMP(0, MAL_PDP_HCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_resource_control_info, 0, sizeof(mal_gpds_resource_control_info));
    mal_gpds_resource_control_info.seq_id = 101;
    mal_gpds_resource_control_info.resource_control_data.data_length = strlen(pdpactmsg);
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED;
    memcpy(&callback.data, &mal_gpds_resource_control_info, sizeof(mal_gpds_resource_control_info));
    memcpy(&callback.extradata, &pdpactmsg, strlen(pdpactmsg));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToSim_ste_cat_call_control(0, sim, 101, (char*) pdpactmsg, (uint32_t) strlen(pdpactmsg)));

  {
    PSCC_test_ste_sim_cb_t callback;
    ste_cat_call_control_response_t ste_cat_call_control_response;
    ste_cat_cc_pdp_t ste_cat_cc_pdp;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_cat_call_control_response, 0, sizeof(ste_cat_call_control_response));
    callback.cause = STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
    callback.client_tag = 101;
    ste_cat_call_control_response.cc_result = STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS;
    ste_cat_cc_pdp.pdp_context.str_p = (uint8_t*) pdpactmsg_mod;
    ste_cat_cc_pdp.pdp_context.no_of_bytes = strlen(pdpactmsg_mod);
    memcpy(&callback.user_data, &ste_cat_cc_pdp, sizeof(ste_cat_cc_pdp_t));
    memcpy(&callback.data, &ste_cat_call_control_response, sizeof(ste_cat_call_control_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  {
    mal_gpds_resource_control_req_info_t mal_gpds_resource_control_req_info;
    memset(&mal_gpds_resource_control_req_info, 0, sizeof(mal_gpds_resource_control_req_info));
    mal_gpds_resource_control_req_info.seq_id = mal_gpds_resource_control_info.seq_id;
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_MODIFIED;
    mal_gpds_resource_control_req_info.resource_control_data.data = (uint8_t*) pdpactmsg_mod;
    mal_gpds_resource_control_req_info.resource_control_data.data_length = strlen(pdpactmsg_mod);
    TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONTROL, &mal_gpds_resource_control_req_info, sizeof(mal_gpds_resource_control_req_info_t)));
  }

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* gpds will send a activation failed event */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_disconnected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_reason,(resp.common.param_list_p)), pscc_reason_pdp_context_activation_failed);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  TESTNOTNEG(util_destroy(cid));

  /* reset service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;
}



psccd_test_state_type_t tc_hcmp_dcmp_ON(int state)
{
  int cid = 1;
  char apn[] = "default-apn.services";
  ip_address_t ip;
  ip_address_t pdns;
  ip_address_t sdns;
  char ifname[] = "calle0";
  mal_gpds_qos_profile_t qosreq;
  mal_gpds_qos_profile_t qosrel5;
  mal_gpds_qos_profile_t qosmin;
  mal_gpds_qos_profile_t qosneg;
  memset(&qosneg,0,sizeof(mal_gpds_qos_profile_t));
  memset(&ip,0,sizeof(ip_address_t));
  memset(&pdns,0,sizeof(ip_address_t));
  memset(&sdns,0,sizeof(ip_address_t));
  inet_pton(AF_INET, "192.168.1.13", &ip);
  inet_pton(AF_INET, "192.168.1.14", &pdns);
  inet_pton(AF_INET, "192.168.1.15", &sdns);
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  ste_sim_t sim;
  char pdpactmsg[] = "apn1,pco1,nsapi1";
  mal_gpds_resource_control_info_t mal_gpds_resource_control_info;
  uintptr_t clienttag = 0;

  int dcomp = MAL_PDP_DCMP_ON;
  int hcomp = MAL_PDP_HCMP_ON;
  /* set service availability */

  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
           sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  TESTNOTNEG(util_create(cid));

  /* perform connect */
  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn ,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_hcmp,&(hcomp));
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_dcmp,&(dcomp));
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* connect */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_APN(0, apn, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_DCMP_on(0, MAL_PDP_DCMP_ON, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_HCMP_on(0, MAL_PDP_HCMP_ON, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_resource_control_info, 0, sizeof(mal_gpds_resource_control_info));
    mal_gpds_resource_control_info.seq_id = 101;
    mal_gpds_resource_control_info.resource_control_data.data_length = strlen(pdpactmsg);
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_REQUESTED;
    memcpy(&callback.data, &mal_gpds_resource_control_info, sizeof(mal_gpds_resource_control_info));
    memcpy(&callback.extradata, &pdpactmsg, strlen(pdpactmsg));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* clienttag is same as seq_id */
  TESTNOTNEG(waitAndRespondToSim_ste_cat_call_control(0, sim, 101, (char*) pdpactmsg, (uint32_t) strlen(pdpactmsg)));

  {
    PSCC_test_ste_sim_cb_t callback;
    ste_cat_call_control_response_t ste_cat_call_control_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_cat_call_control_response, 0, sizeof(ste_cat_call_control_response));
    callback.cause = STE_CAT_CAUSE_SIM_EC_CALL_CONTROL;
    callback.client_tag = 101;
    ste_cat_call_control_response.cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;
    memcpy(&callback.data, &ste_cat_call_control_response, sizeof(ste_cat_call_control_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  {
    mal_gpds_resource_control_req_info_t mal_gpds_resource_control_req_info;
    memset(&mal_gpds_resource_control_req_info, 0, sizeof(mal_gpds_resource_control_req_info));
    mal_gpds_resource_control_req_info.seq_id = mal_gpds_resource_control_info.seq_id;
    mal_gpds_resource_control_req_info.cc_result = MAL_GPDS_RESOURCE_ALLOWED;
    TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONTROL, &mal_gpds_resource_control_req_info, sizeof(mal_gpds_resource_control_req_info_t)));
  }

  /* we are now connecting and need to send callback resource control */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_DONE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* we are now resource controlled and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV4_MAP), cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IP(0, ip, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_PDNS(0, pdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_SDNS(0, sdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IFNAME(0, ifname, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  TESTNOTNEG(util_disconnect(cid));
  TESTNOTNEG(util_destroy(cid));

  /* reset service availability */
  {
    PSCC_test_ste_sim_cb_t callback;
    ste_uicc_get_service_availability_response_t ste_uicc_get_service_availability_response;
    memset(&callback, 0, sizeof(callback));
    memset(&ste_uicc_get_service_availability_response, 0, sizeof(ste_uicc_get_service_availability_response));
    callback.cause = STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
    callback.client_tag = clienttag;
    ste_uicc_get_service_availability_response.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
    ste_uicc_get_service_availability_response.uicc_status_code = STE_UICC_STATUS_CODE_OK;
    memcpy(&callback.data, &ste_uicc_get_service_availability_response, sizeof(ste_uicc_get_service_availability_response_t));
    TESTNOTNEG(sendToSim(PSCCD_TEST_TEST_STUB, PSCC_STE_SIM_CB, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,sim));

  return PSCCD_TEST_SUCCESS;

  error:
  return PSCCD_TEST_FAIL;

}


