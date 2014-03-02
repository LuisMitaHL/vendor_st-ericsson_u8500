/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tc_mal_attach.h"
#include "pscc_msg.h"
#include "common.h"
#include "util.h"
#include "util_requests.h"


psccd_test_state_type_t tc_attach_set_default(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t mode;// = pscc_attach_mode_default;
  mal_gpds_config_t mal_gpds_config;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,
      (pscc_attach_mode_t*) &mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_ACCEPT;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  //TESTBOOL(resp.common.id,pscc_event_attach_mode_configured);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}


psccd_test_state_type_t tc_attach_set_manual(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t mode = pscc_attach_mode_manual;
  mal_gpds_config_t mal_gpds_config;
  uint32_t testtag = 0x7F;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,
      (pscc_attach_mode_t*) &mode);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
      mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
      mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
      mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
      memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_configured);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_set_automatic(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t mode = pscc_attach_mode_automatic;
  mal_gpds_config_t mal_gpds_config;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,
      (pscc_attach_mode_t*) &mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_AUTOMATIC;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_configured);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_set_automatic_fail(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t mode = pscc_attach_mode_automatic;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,
      (pscc_attach_mode_t*) &mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_configure_failed);
  TESTBOOL(resp.resp.result, 0);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_query_mode(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_config_t mal_gpds_config;
  uint32_t testtag = 0x7F;

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_attach_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_queried);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_attach_mode,(resp.common.param_list_p)), pscc_attach_mode_manual);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_attach_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DEFAULT;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_AUTOMATIC;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_queried);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_attach_mode,(resp.common.param_list_p)), pscc_attach_mode_automatic);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_query_status(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;
  uint32_t testtag = 0x7F;
  uint32_t testtag2 = 0x7E;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_DETACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach_status;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_attach_status);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_attach_status,(resp.common.param_list_p)), pscc_attach_status_detached);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  uint32_t testtag = 0x7F;
  uint32_t testtag2 = 0x7E;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;
  pscc_attach_mode_t mode = pscc_attach_mode_manual;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_DETACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send attach request */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PS_ATTACH, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_attach);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);
  /* Send attach request which will fail in pscc because of ongoing attach requests */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_attach);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag2);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* query attach mode  during ongoing attach and receive pscc_result_failed_unspecified */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_attach_mode);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

 /* query ping block mode during ongoing attach and receive pscc_result_failed_unspecified */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_ping_block_mode);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  /* try setting attach mode during ongoing attach and receive pscc_result_failed_unspecified */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode,(pscc_attach_mode_t*) &mode);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

 /* Send attached event from mal */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACHED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_fail(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_DETACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PS_ATTACH, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_attach);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_failed);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_already_attached(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_ATTACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_attach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_attach);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_detach(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;
  uint32_t testtag = 0x7F;
  uint32_t testtag2 = 0x7E;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_ATTACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_detach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PS_DETACH, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_detach);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send detach request which will fail in pscc because of ongoing detach requests */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_detach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag2);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_detach);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag2);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send detached event from mal */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_DETACHED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_detach_received(int state) {
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;
  mal_gpds_detach_info_t mal_gpds_detach_info;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_ATTACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_NWI_DETACHED;
    mal_gpds_detach_info.detach_cause = MAL_GPDS_CAUSE_UNKNOWN;
    mal_gpds_detach_info.detach_type = MAL_GPDS_DETACH_TYPE_MS;
    memcpy(&callback.data, &mal_gpds_detach_info, sizeof(mal_gpds_detach_info_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_detach_fail(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_ATTACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_detach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_PS_DETACH, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_detach);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_DETACH_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detach_failed);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_attach_already_detached(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  mal_gpds_ps_attach_status_t mal_gpds_ps_attach_status;

  /* Start by sending an attach status event.
   *  This is done to make sure we are in a correct initial state for the test */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_PS_ATTACH_STATUS;
    mal_gpds_ps_attach_status = MAL_GPDS_PS_STATUS_DETACHED;
    memcpy(&callback.data, &mal_gpds_ps_attach_status, sizeof(mal_gpds_ps_attach_status_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_detached);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_detach;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_detach);
  TESTBOOL(resp.resp.result, pscc_result_failed_operation_not_allowed);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_configure_attach_mode(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t pscc_attach_mode;
  mal_gpds_config_t mal_gpds_config;

  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_attach_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DISABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_attach_mode = pscc_attach_mode_automatic;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode, &pscc_attach_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send a mal event*/
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_AUTOMATIC;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_configured);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_configure_attach_mode_fail_1(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t pscc_attach_mode;
  mal_gpds_config_t mal_gpds_config;

  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_attach_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DISABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_attach_mode = pscc_attach_mode_automatic;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode, &pscc_attach_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send a mal event which should trigger an error since MAL "responds" ok but with the wrong attach mode set */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_configure_failed);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_configure_attach_mode_fail_2(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_attach_mode_t pscc_attach_mode;
  mal_gpds_config_t mal_gpds_config;

  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_attach_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DISABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_attach_mode = pscc_attach_mode_automatic;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_attach_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_attach_mode, &pscc_attach_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_attach_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send a mal event which should trigger an error since MAL "responds" ok but with the wrong attach mode set */
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback, sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_attach_mode_configure_failed);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_configure_ping_block_mode(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_ping_block_mode_t pscc_ping_block_mode;
  mal_gpds_config_t mal_gpds_config;
  uint32_t testtag = 0x7F;

  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_ping_block_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);


  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_ping_block_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_ENABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_ENABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());


  pscc_ping_block_mode = pscc_ping_block_mode_enabled;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ping_block_mode, &pscc_ping_block_mode);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_ping_block_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send a mal event*/
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DISABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_ENABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_configured);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* send configure ping block mode request without ping block mode and receive pscc_result_failed_unspecified */
  pscc_ping_block_mode = pscc_ping_block_mode_enabled;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_ping_block_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  TESTBOOL(resp.resp.result, pscc_result_failed_unspecified);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_configure_ping_block_mode_fail_1(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_ping_block_mode_t pscc_ping_block_mode;
  mal_gpds_config_t mal_gpds_config;
  uint32_t testtag = 0x7F;

  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ct,&testtag);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_ping_block_mode);
  TESTBOOL(MPL_GET_PARAM_VALUE_FROM_LIST(uint32_t,pscc_paramid_ct,(resp.common.param_list_p)), testtag);
  mpl_param_list_destroy(&resp.common.param_list_p);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_ping_block_mode = pscc_ping_block_mode_enabled;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ping_block_mode, &pscc_ping_block_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_ping_block_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send a mal event*/
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_DISABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_MANUAL;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_configure_failed);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

psccd_test_state_type_t tc_configure_ping_block_mode_fail_2(int state) {
  mpl_msg_t *pscc_msg_p = NULL;
  int cid = 0;
  mpl_msg_t resp;
  pscc_ping_block_mode_t pscc_ping_block_mode;
  mal_gpds_config_t mal_gpds_config;

  /* Start by sending an query attach mode request.
   *  This is done to make sure we are in a correct initial state for the test */
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_query_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_QUERY_CONFIG, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.id,pscc_query_ping_block_mode);

  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    memset(&mal_gpds_config, 0, sizeof(mal_gpds_config_t));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIG_QUERIED;
    mal_gpds_config.aol_mode = MAL_GPDS_AOL_CONTEXT_ENABLE;
    mal_gpds_config.nwi_act_mode = MAL_GPDS_NWI_CONTEXT_DEFAULT;
    mal_gpds_config.ps_attach_mode = MAL_GPDS_PS_ATTACH_DEFAULT;
    mal_gpds_config.drop_icmp_mode = MAL_GPDS_ICMP_DROP_MODE_DISABLED;
    memcpy(&callback.data, &mal_gpds_config, sizeof(mal_gpds_config_t));
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback,
            sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_queried);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  pscc_ping_block_mode = pscc_ping_block_mode_enabled;
  pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  pscc_msg_p->common.type = pscc_msgtype_req;
  pscc_msg_p->common.id = pscc_configure_ping_block_mode;
  pscc_msg_p->common.param_list_p = NULL;
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_connid, &cid);
  mpl_add_param_to_list(&pscc_msg_p->common.param_list_p, pscc_paramid_ping_block_mode, &pscc_ping_block_mode);
  TESTNOTNEG(sendToStecomCtrl(pscc_msg_p));
  mpl_msg_free((mpl_msg_t*) pscc_msg_p);

  TESTNOTNEG(waitAndRespondToGPDSReq_Request(0, cid, MAL_GPDS_REQ_CONFIG_CHANGE, 0, 0));

  TESTNOTNEG(waitAndReadStecomResp(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_resp);
  TESTBOOL(resp.common.id,pscc_configure_ping_block_mode);
  mpl_param_list_destroy(&resp.common.param_list_p);

  /* Send a mal event*/
  {
    PSCC_test_mal_gpds_cb_t callback;
    memset(&callback, 0, sizeof(callback));
    callback.conn_id = cid;
    callback.event_id = MAL_GPDS_EVENT_ID_CONFIGURATION_FAILED;
    TESTNOTNEG(sendToGPDS(PSCCD_TEST_TEST_STUB, PSCC_MAL_GPDS_CALLBACK, &callback, sizeof(callback)));
  }
  TESTNOTNEG(waitAndRespondToGPDSReq_ResponseHandler());

  TESTNOTNEG(waitAndReadStecomEvent(&resp));
  TESTBOOL(resp.common.type,pscc_msgtype_event);
  TESTBOOL(resp.common.id,pscc_event_ping_block_mode_configure_failed);
  TESTBOOL(resp.resp.result, pscc_result_ok);
  mpl_param_list_destroy(&resp.common.param_list_p);

  return PSCCD_TEST_SUCCESS;

  error: return PSCCD_TEST_FAIL;
}

