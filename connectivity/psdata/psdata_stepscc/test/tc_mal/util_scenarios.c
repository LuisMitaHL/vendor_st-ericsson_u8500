/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util_scenarios.c
 * Description     : wrapper for using mal scenarios
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "common.h"
#include "util.h"
#include "util_scenarios.h"
#include "util_conn_param.h"
#include "util_requests.h"
#include "pscc_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int util_setup_stecom() {
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  ste_sim_t ste_sim;
  ste_sim_closure_t closure;
  uintptr_t clienttag;
  uint32_t testtag = 0x7F;
  TESTNOTNEG(initStecom());
  TESTNOTNEG(pscc_init(NULL, &my_log_func));

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_init_handler;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Init(0));
  TESTNOTNEG(waitAndRespondToGPDSReq_RegisterCallback(0));
  TESTNOTNEG(waitAndRespondToGPDSReq_RegisterCallback(0));
  TESTNOTNEG(waitAndRespondToSim_ste_sim_new_st(ste_sim, closure));
  TESTNOTNEG(waitAndRespondToSim_ste_sim_connect(0, ste_sim, 0)); // ct = 0 for init
  TESTNOTNEG(waitAndRespondToSim_ste_sim_fd(0, ste_sim));
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_CONFIG_CHANGE, NULL, 0));
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_PS_ATTACH_STATUS, NULL, 0));
  TESTNOTNEG(waitAndRespondToSim_ste_uicc_get_service_availability(0, ste_sim,  &clienttag, SIM_SERVICE_TYPE_CALL_CONTROL_ON_GPRS));
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONFIGURE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_init_handler);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send aol mode configuration event */
  {
    PSCC_test_mal_gpds_cb_t callback;
    mal_gpds_config_t mal_gpds_config;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DISABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;

    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler())

  /* Send PS attach status event */
  {
    PSCC_test_mal_gpds_cb_t callback;
    mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_ATTACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler())

 /* answer to service availability */
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

  TESTNOTNEG(waitAndRespondToSim_ste_sim_read(0,ste_sim));

  /* gpds resource config initiated */
  {
    PSCC_test_mal_gpds_cb_t callback;
    mal_gpds_configuration_status_t mal_gpds_configuration_status;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED;
    mal_gpds_configuration_status = MAL_GPDS_RESOURCE_CONF_READY;
    memcpy(&callback.data, &mal_gpds_configuration_status, sizeof(mal_gpds_configuration_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, 0, MAL_GPDS_REQ_RESOURCE_CONFIGURE, 0, 0));

  /* gpds resource config initiated */
  {
    PSCC_test_mal_gpds_cb_t callback;
    mal_gpds_configuration_status_t mal_gpds_configuration_status;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = 0;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONFIGURED;
    mal_gpds_configuration_status = MAL_GPDS_RESOURCE_CONF_RECONFIGURED;
    memcpy(&callback.data, &mal_gpds_configuration_status, sizeof(mal_gpds_configuration_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attached);
  TESTBOOL(resp.resp.result, 0);

  mpl_param_list_destroy(&resp.common.param_list_p);
  //TESTNOTNEG(waitAndRespondToSim_ste_sim_disconnect(0, ste_sim, 0));

  return 0;
error:
  return -1;
}


int util_create(int cid)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  uint32_t testtag = 0x7F;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_create;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_CREATE, 0, 0));

  {
    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type,pscc_msgtype_resp);
    TESTBOOL(resp.common.id,pscc_create);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);\
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
    TESTBOOL(resp.resp.result, 0);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  return 0;
  error:
  return -1;
}

int util_create_error(int cid)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  int cid2 = 0;
  uint32_t testtag = 0x7F;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_resp;
  pscc_msg_p->common.id = pscc_create;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  {
    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type,pscc_msgtype_resp);
    TESTBOOL(resp.common.id,pscc_create);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
    TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_event_ping_block_mode_configured;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  {
    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type,pscc_msgtype_resp);
    TESTBOOL(resp.common.id,pscc_event_ping_block_mode_configured);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
    TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_create;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_CREATE, 0, 0));

  {
    TESTNOTNEG(waitAndReadStecomResp(&resp));
    TESTBOOL(resp.common.type,pscc_msgtype_resp);
    TESTBOOL(resp.common.id,pscc_create);
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), 1000);\
    TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
    TESTBOOL(resp.resp.result, 0);
    mpl_param_list_destroy(&resp.common.param_list_p);
  }

  cid2 = 1000;
  /* try to create a context with existing CID and receive pscc_result_failed_object_exists */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_create;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  printf("Here @ 1");
  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_create);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_exists);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* destroy with correct context id and receive pscc_result_ok*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_destroy;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_RELEASE,0,0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_destroy);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}

int util_unexpected_events(int cid)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;

  /* Activating event received in unexpected state */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVATING;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Activated event received in unexpected state */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Activation Failed event received in unexpected state */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Modification failed event received in unexpected state */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Deactivated event received in unexpected state */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_DEACTIVATED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());


  /* Hit the default cause in mal_event_cb */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_RESOURCE_CONTROL_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Receive PDP suspended event */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_SUSPENDED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Receive NWI PDP request */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_NWI_PDP_REQUESTED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  return 0;
  error:
  return -1;
}

int util_error_paths(int cid)
{
  pscc_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  int cid2 = 256;
  int attach_mode = 0;
  int connection_status = 0;
  uint32_t testtag = 0x7F;

  /* try pscc_get  without cid and receive pscc_result_failed_parameter_not_found*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* try pscc_get with wrong context id and receive pscc_result_failed_object_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* set cid parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* try pscc_get for unallowed parameter and receive pscc_result_failed_object_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,&attach_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);


  /* try pscc_get for pscc_paramid_connection_status and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connection_status,&connection_status);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* try pscc_get for pscc_paramid_attach_mode with client tag with client tag and receive pscc_result_failed_operation_not_allowed */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,&attach_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;

}

int util_check(int cid , ip_address_t ip , mal_gpds_qos_profile_t qosreq, \
mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosneg)
{
  pscc_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  char *neg_ip = NULL;
  char ip2[16];
  mpl_param_element_t* param_element_p = NULL;
  mal_gpds_qos_profile_t temp_qos;
  mal_gpds_qos_profile_t temp_qosrel5;
  memset(&temp_qos,0,sizeof(mal_gpds_qos_profile_t));
  memset(&temp_qosrel5,0,sizeof(mal_gpds_qos_profile_t));

    inet_ntop(AF_INET, ip, ip2, sizeof(ip_address_t));
  /* get parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_class,&qosreq.traffic_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_order,&qosreq.delivery_order);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_erroneous_sdu,&qosreq.delivery_of_error_sdu);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_sdu_size,&qosreq.maximum_sdu_size);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_uplink,&qosreq.maximum_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_downlink,&qosreq.maximum_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_residual_ber,&qosreq.residual_ber);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_sdu_error_ratio,&qosreq.sdu_error_ratio);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_transfer_delay,&qosreq.transfer_delay);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_handling_priority,&qosreq.trafic_handling_priority);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_uplink,&qosreq.guaranteed_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_downlink,&qosreq.guaranteed_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_signalling_indication,&qosrel5.sgn_ind);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_source_statistics_descriptor,&qosrel5.src_stat_desc);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_own_ip_address,NULL);

  TESTNOTNEG(sendToStecomCtrl((mpl_msg_t*)pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);
  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)),
cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_qos_type,(resp.common.param_list_p))
, qosneg.qos_type);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_precedence_class,(resp.common.param_list_p)), qosneg.precedence_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_delay_class,(resp.common.param_list_p)), qosneg.delay_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_reliability_class,(resp.common.param_list_p)), qosneg.reliability_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_peak_throughput_class,(resp.common.param_list_p)), qosneg.peak_throughput_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_mean_throughput_class,(resp.common.param_list_p)), qosneg.mean_throughput_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_traffic_class,(resp.common.param_list_p)), qosneg.traffic_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_delivery_order,(resp.common.param_list_p)), qosneg.delivery_order);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_delivery_erroneous_sdu,(resp.common.param_list_p)), qosneg.delivery_of_error_sdu);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_max_sdu_size,(resp.common.param_list_p)), qosneg.maximum_sdu_size);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_max_bitrate_uplink,(resp.common.param_list_p)), qosneg.maximum_bitrate_ul);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_max_bitrate_downlink,(resp.common.param_list_p)), qosneg.maximum_bitrate_dl);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_residual_ber,(resp.common.param_list_p)), qosneg.residual_ber);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint,pscc_paramid_req_qos_sdu_error_ratio,(resp.common.param_list_p)), qosneg.sdu_error_ratio);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_transfer_delay,(resp.common.param_list_p)), qosneg.transfer_delay);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_traffic_handling_priority,(resp.common.param_list_p)), qosneg.trafic_handling_priority);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_guaranteed_bitrate_uplink,(resp.common.param_list_p)), qosneg.guaranteed_bitrate_ul);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_guaranteed_bitrate_downlink,(resp.common.param_list_p)), qosneg.guaranteed_bitrate_dl);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_signalling_indication,(resp.common.param_list_p)), qosneg.sgn_ind);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_source_statistics_descriptor,(resp.common.param_list_p)), qosneg.src_stat_desc);

    /* Get own ip address */
  param_element_p = mpl_param_list_find(pscc_paramid_own_ip_address, resp.common.param_list_p)
;
  neg_ip = (char *)malloc(1 + strlen((char *)(param_element_p->value_p)));
  strcpy(neg_ip, (char *)(param_element_p->value_p));

  TESTBOOL(memcmp(neg_ip, ip2,sizeof(ip_address_t)),0);
  TESTBOOL(resp.resp.result, 0);
  free(neg_ip);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;

}


int util_check_rel97(int cid , ip_address_t ip, mal_gpds_qos_profile_t qosreq,mal_gpds_qos_profile_t qosneg)
{
  pscc_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  char ip2[16];
  mal_gpds_qos_profile_t temp_qos;
  memset(&temp_qos,0,sizeof(mal_gpds_qos_profile_t));

  inet_ntop(AF_INET, ip, ip2, sizeof(ip_address_t));
  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_get;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);

  TESTNOTNEG(sendToStecomCtrl((mpl_msg_t*)pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);
  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_get);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)),cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_qos_type,(resp.common.param_list_p)), qosneg.qos_type);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_precedence_class,(resp.common.param_list_p)), qosneg.precedence_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_delay_class,(resp.common.param_list_p)), qosneg.delay_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_reliability_class,(resp.common.param_list_p)), qosneg.reliability_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_peak_throughput_class,(resp.common.param_list_p)), qosneg.peak_throughput_class);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_req_qos_mean_throughput_class,(resp.common.param_list_p)), qosneg.mean_throughput_class);

  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;


}

int util_connect(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  int cid2 = 256;
  int rx_lo = 0;
  uint32_t testtag = 0x7F;

  /* try setting unsettable parameters and receive pscc_result_failed_operation_not_allowed*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_rx_data_count_lo,&rx_lo);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

 /* try setting parameters without context id and receive pscc_result_failed_parameter_not_found*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_class,&qosreq.traffic_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_order,&qosreq.delivery_order);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_erroneous_sdu,&qosreq.delivery_of_error_sdu);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_sdu_size,&qosreq.maximum_sdu_size);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_uplink,&qosreq.maximum_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_downlink,&qosreq.maximum_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_residual_ber,&qosreq.residual_ber);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_sdu_error_ratio,&qosreq.sdu_error_ratio);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_transfer_delay,&qosreq.transfer_delay);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_handling_priority,&qosreq.trafic_handling_priority);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_uplink,&qosreq.guaranteed_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_downlink,&qosreq.guaranteed_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_signalling_indication,&qosrel5.sgn_ind);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_source_statistics_descriptor,&qosrel5.src_stat_desc);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* try connect without context id and receive pscc_result_failed_parameter_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);

  if (cid != cid2) {
  /* try connect with wrong context id and receive pscc_result_failed_object_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* try modifying before connect and receive pscc_result_failed_operation_not_allowed */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_modify;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_modify);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);


  /* connect */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
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

  /* try connect when already connected and receive pscc_result_failed_operation_not_allowed */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* send uplink data request for pdp type other than test and receive pscc_result_failed_operation_not_allowed*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_connect_255(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, uint8_t extended_ul_bitrate, uint8_t extended_dl_bitrate)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  int cid2 = 256;
  int rx_lo = 0;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_class,&qosreq.traffic_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_order,&qosreq.delivery_order);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_erroneous_sdu,&qosreq.delivery_of_error_sdu);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_sdu_size,&qosreq.maximum_sdu_size);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_uplink,&qosreq.maximum_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_downlink,&qosreq.maximum_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_extended_max_bitrate_uplink,&extended_ul_bitrate);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink,&extended_dl_bitrate);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_residual_ber,&qosreq.residual_ber);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_sdu_error_ratio,&qosreq.sdu_error_ratio);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_transfer_delay,&qosreq.transfer_delay);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_handling_priority,&qosreq.trafic_handling_priority);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_uplink,&qosreq.guaranteed_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_downlink,&qosreq.guaranteed_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_signalling_indication,&qosrel5.sgn_ind);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_source_statistics_descriptor,&qosrel5.src_stat_desc);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
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

  return 0;
  error:
  return -1;
}



int util_connect_rel97(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);

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
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
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

  return 0;
  error:
  return -1;
}

int util_connect_for_ipv4v6(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  uint32_t testtag = 0x7F;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
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
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4V6, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  switch(addr_type) {
    case 1: /* Only IPv4 addresses are allcoated by the network*/
      TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV4_MAP), cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_IP(0, ip, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_PDNS(0, pdns, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_SDNS(0, sdns, cid));
	  break;

    case 2: /* Only IPv6 addresses are allcoated by the network*/
      TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV6_MAP), cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_IPv6(0, ipv6, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_PDNSv6(0, ipv6_pdns, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_SDNSv6(0, ipv6_sdns, cid));
	  break;

    case 3: /* Only IPv4v6 addresses are allcoated by the network*/
      TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV4V6_MAP), cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_IP(0, ip, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_IPv6(0, ipv6, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_PDNS(0, pdns, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_PDNSv6(0, ipv6_pdns, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_SDNS(0, sdns, cid));
      TESTNOTNEG(waitAndRespondToGPDSGet_SDNSv6(0, ipv6_sdns, cid));
	  break;

    default:
      printf("Unsupported PDP Types");
      break;

  }

  TESTNOTNEG(waitAndRespondToGPDSGet_IFNAME(0, ifname, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_connect_for_ipv6(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);
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
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV6, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Only IPv6 addresses are allcoated by the network*/
  TESTNOTNEG(waitAndRespondToGPDSGet_IPMAP(0, (int)(IPV6_MAP), cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_IPv6(0, ipv6, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_PDNSv6(0, ipv6_pdns, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_SDNSv6(0, ipv6_sdns, cid));


  TESTNOTNEG(waitAndRespondToGPDSGet_IFNAME(0, ifname, cid));
  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_connect_for_ipv4(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);
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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Receive Modification failure as unexpected event */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Only IPv4 addresses are allcoated by the network*/
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

  return 0;
  error:
  return -1;
}

int util_connect_for_chap_generation(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, char user[])
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  uint8_t ChapId = 1;
  mal_gpds_auth_method_t auth_method = MAL_AUTH_METHOD_SECURE;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_uid ,(char*)user);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_chap_id,&ChapId);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_method,&auth_method);

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
  TESTNOTNEG(waitAndRespondToGPDSSet_ChapId(0, ChapId, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Username(0, user, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Challenge(0, NULL, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Response(0, NULL, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_SECURE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());


  /* Only IPv4 addresses are allcoated by the network*/
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

  return 0;
  error:
  return -1;
}


int util_connect_for_chap(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, char user[])
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  mal_gpds_chap_auth_data_t challenge;
  mal_gpds_chap_auth_data_t response;
  memset(&challenge, '\0', sizeof(mal_gpds_chap_auth_data_t));
  memset(&response, '\0', sizeof(mal_gpds_chap_auth_data_t));
  //challenge = (mal_gpds_chap_auth_data_t*)malloc(sizeof(mal_gpds_chap_auth_data_t));
  challenge.length = 3;
  challenge.data = "123";
  //strcpy(challenge->data,"123");
  uint8_t ChapId = 1;
  //response = (mal_gpds_chap_auth_data_t*)malloc(sizeof(mal_gpds_chap_auth_data_t));
  response.length = 2;
  //response->data = malloc(3*sizeof(uint8_t));
  response.data = "12";

  mal_gpds_auth_method_t auth_method = MAL_AUTH_METHOD_SECURE;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_uid ,(char*)user);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_chap_id,&ChapId);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_chap_challenge,&challenge);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_method,&auth_method);
//  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_chap_response,(void *)response);

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
  TESTNOTNEG(waitAndRespondToGPDSSet_ChapId(0, ChapId, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Username(0, user, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Challenge(0, NULL, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Response(0, NULL, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_SECURE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());


  /* Only IPv4 addresses are allcoated by the network*/
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

  return 0;
  error:
  return -1;
}

int util_connect_for_pap(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg, char user[], char password[])
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  mal_gpds_auth_method_t auth_method = MAL_AUTH_METHOD_NORMAL;
  uint8_t ChapId = 2;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_uid ,(char*)user);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_pwd,(char*)password);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_chap_id,&ChapId);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_auth_method,&auth_method);

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
  TESTNOTNEG(waitAndRespondToGPDSSet_ChapId(0, ChapId, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Username(0, user, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_Password(0, password, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NORMAL, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());


  /* Only IPv4 addresses are allcoated by the network*/
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

  return 0;
  error:
  return -1;
}


int util_connect_and_fetch_counters(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  mal_gpds_data_counter_info_t mal_gpds_data_counter_info;
  int cid2 = 256;
  uint32_t testtag = 0x7F;
  memset(&mal_gpds_data_counter_info,0x00,sizeof(mal_gpds_data_counter_info_t));

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* fetch data counters before pscc connect and receive pscc_result_failed_operation_not_allowed */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* fetch data counters without CID  and receive pscc_result_failed_parameter_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVATING;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connecting);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Only IPv4 addresses are allcoated by the network*/
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

  /* fetch data counters with wrong cid and receive pscc_result_failed_object_not_found */
  if (cid != cid2) {
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* fetch data counters and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_DATA_COUNTER,&mal_gpds_data_counter_info, sizeof(mal_gpds_data_counter_info_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* fetch data counters during ongoing fetech data counter and receive pscc_result_failed_unspecified */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we have now received the data counters  */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_DATA_COUNTER;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_data_counters_fetched);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* fetch data counters and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_fetch_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_DATA_COUNTER,&mal_gpds_data_counter_info, sizeof(mal_gpds_data_counter_info_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we have now received the data counters  */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_DATA_COUNTER;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_data_counters_fetched);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_connect_and_reset_counters(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  mal_gpds_data_counter_info_t mal_gpds_data_counter_info;
  int cid2 = 256;
  uint32_t testtag = 0x7F;
  memset(&mal_gpds_data_counter_info,0x00,sizeof(mal_gpds_data_counter_info_t));

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* reset data counters before pscc connect and receive pscc_result_failed_operation_not_allowed */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* reset data counters without CID  and receive pscc_result_failed_parameter_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVATING;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_connecting);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Only IPv4 addresses are allcoated by the network*/
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

  /* reset data counters with wrong cid and receive pscc_result_failed_object_not_found */
  if (cid != cid2) {
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* reset data counters and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_DATA_COUNTER,&mal_gpds_data_counter_info, sizeof(mal_gpds_data_counter_info_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* reset data counters during ongoing fetech data counter and receive pscc_result_failed_unspecified */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we have now reset the data counters  */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_DATA_COUNTER;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_data_counters_reset);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* reset data counters and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_reset_data_counters;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_DATA_COUNTER,&mal_gpds_data_counter_info, sizeof(mal_gpds_data_counter_info_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_DATA_COUNTER;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_data_counters_reset);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_connect_for_uplink(int cid, char apn[], ip_address_t ip, ip_address_t ipv6, ip_address_t pdns,
    ip_address_t ipv6_pdns, ip_address_t sdns, ip_address_t ipv6_sdns, char ifname[], mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosrel5, mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,
    mal_gpds_pdp_type_t pdp_type, int addr_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  int max_size =512;
  int cid2 = 256;
  uint32_t testtag = 0x7F;
  uint32_t testtag2 = 0x7E;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_uplink_data_size,&max_size);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* send uplink data request without activating the context and receive pscc_result_failed_operation_not_allowed*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_TEST;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_ACTIVE;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  /* Only IPv4 addresses are allcoated by the network*/
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

  /* send uplink data with wrong conn id and receive pscc_result_failed_object_not_found*/
  if (cid != cid2) {
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* send uplink data with no conn id and receive pscc_result_failed_parameter_not_found*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* send uplink data and receive pscc_result_failed_unspecified due to sending failed */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  waitAndRespondToGPDSGet_DontCare(0);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* send uplink data and receive pscc_result_failed_unspecified due to sending successful but 0 bytes sent */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  waitAndRespondToGPDSGet_DontCare(0);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* send uplink data and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_generate_uplink_data;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  waitAndRespondToGPDSGet_DontCare(0);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_generate_uplink_data);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}

int util_list_all_connection(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  uint32_t testtag = 0x7F;

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

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
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

  /* list all connection and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_list_all_connections;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}



int util_static_ipv4(int cid, char apn[], ip_address_t ip, ip_address_t pdns,
    ip_address_t sdns, char ifname[], mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg,char static_ip[],mal_gpds_pdp_type_t pdp_type)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_apn,(char*) apn);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_pdp_type,&pdp_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_static_ip_address,(char *)static_ip);

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
  TESTNOTNEG(waitAndRespondToGPDSSet_IPAddress(0, static_ip, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_DCMP(0, MAL_PDP_DCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_HCMP(0, MAL_PDP_HCMP_OFF, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPType(0, MAL_PDP_TYPE_IPV4, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_AuthMeth(0, MAL_AUTH_METHOD_NONE, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_PDPContextType(0, MAL_PDP_CONTEXT_TYPE_PRIMARY, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_ACTIVATE, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
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

  return 0;
  error:
  return -1;
}

int util_modify_failure(int cid, mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_class,&qosreq.traffic_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_order,&qosreq.delivery_order);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_erroneous_sdu,&qosreq.delivery_of_error_sdu);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_sdu_size,&qosreq.maximum_sdu_size);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_uplink,&qosreq.maximum_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_downlink,&qosreq.maximum_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_residual_ber,&qosreq.residual_ber);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_sdu_error_ratio,&qosreq.sdu_error_ratio);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_transfer_delay,&qosreq.transfer_delay);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_handling_priority,&qosreq.trafic_handling_priority);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_uplink,&qosreq.guaranteed_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_downlink,&qosreq.guaranteed_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_signalling_indication,&qosrel5.sgn_ind);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_source_statistics_descriptor,&qosrel5.src_stat_desc);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* modify */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_modify;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_MODIFY, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_modify);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Receive Modification failure */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONTEXT_MODIFICATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_modify_failed);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}

int util_modify(int cid, mal_gpds_qos_profile_t qosreq, mal_gpds_qos_profile_t qosrel5,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;
  int cid2 = 256;
  uint32_t testtag = 0x7F;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_class,&qosreq.traffic_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_order,&qosreq.delivery_order);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delivery_erroneous_sdu,&qosreq.delivery_of_error_sdu);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_sdu_size,&qosreq.maximum_sdu_size);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_uplink,&qosreq.maximum_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_max_bitrate_downlink,&qosreq.maximum_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_residual_ber,&qosreq.residual_ber);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_sdu_error_ratio,&qosreq.sdu_error_ratio);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_transfer_delay,&qosreq.transfer_delay);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_traffic_handling_priority,&qosreq.trafic_handling_priority);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_uplink,&qosreq.guaranteed_bitrate_ul);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_downlink,&qosreq.guaranteed_bitrate_dl);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_signalling_indication,&qosrel5.sgn_ind);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_source_statistics_descriptor,&qosrel5.src_stat_desc);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* try modifying without context id and receive pscc_result_failed_parameter_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_modify;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_modify);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);


  if (cid != cid2) {
  /* try modifying with wrong context id and receive pscc_result_failed_object_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_connect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_connect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* modify */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_modify;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSRel5(0, qosrel5, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_MODIFY, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_modify);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_modified);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_modify_rel97(int cid, mal_gpds_qos_profile_t qosreq,
    mal_gpds_qos_profile_t qosmin, mal_gpds_qos_profile_t qosneg)
{
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  mal_gpds_phonet_conf_t mal_gpds_phonet_conf;

  /* set parameters */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_set;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_qos_type,&qosreq.qos_type);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_precedence_class,&qosreq.precedence_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_delay_class,&qosreq.delay_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_reliability_class,&qosreq.reliability_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_peak_throughput_class,&qosreq.peak_throughput_class);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_req_qos_mean_throughput_class,&qosreq.mean_throughput_class);

  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_set);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* modify */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_modify;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  /* has to be in the same order that pscc calls mal */
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSReq(0, qosreq, cid));
  TESTNOTNEG(waitAndRespondToGPDSSet_QoSMin(0, qosmin, cid));

  mal_gpds_phonet_conf = MAL_GPDS_CONFIG_PHONET_NORMAL;
  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_MODIFY, &mal_gpds_phonet_conf, sizeof(mal_gpds_phonet_conf_t)));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_modify);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* we are now connecting and need to send callback connected */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONTEXT_MODIFIED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }

  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndRespondToGPDSGet_QoSNeg(0, qosneg, cid));

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_modified);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_disconnect(int cid) {
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  int cid2 = 256;
  uint32_t testtag = 0x7F;

  /* try disconnect without context id and receive pscc_result_failed_parameter_not_found */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_disconnect;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_disconnect);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* try disconnect with wrong context id and receive pscc_result_failed_object_not_found */
  if (cid != cid2) {
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_disconnect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_disconnect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* disconnect with correct context id and receive pscc_result_ok */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_disconnect;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_DEACTIVATE,0,0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_disconnect);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PDP_DEACTIVATED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_disconnected);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_reason,(resp.common.param_list_p)), pscc_reason_pdp_context_ms_deactivated);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}


int util_destroy(int cid) {
  mpl_msg_t *pscc_msg_p=NULL;
  mpl_msg_t resp;
  int cid2 = 256;
  uint32_t testtag = 0x7F;

  /* try to destroy without context id and receive pscc_result_failed_object_not_found*/
  if (cid != cid2) {
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_destroy;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid2);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_destroy);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid2);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_object_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);
  }

  /* try to destroy without context id and receive pscc_result_failed_parameter_not_found*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_destroy;
  pscc_msg_p->common.param_list_p = NULL;
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_destroy);
  TESTBOOL(resp.resp.result, pscc_result_failed_parameter_not_found);
  mpl_param_list_destroy(&resp.common.param_list_p);


  /* destroy with correct context id and receive pscc_result_ok*/
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_destroy;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid,&cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*)pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PSCONN_RELEASE,0,0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_destroy);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp.common.param_list_p)), cid);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return 0;
  error:
  return -1;
}

int util_cleanup() {
  closeStecom();
  return 0;
}
