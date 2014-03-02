/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control
 */


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pscc_msg.h"
#include "psccd.h"
#include "pscc_object.h"
#include "pscc_bearer.h"
#include "pscc_sim.h"
#include "mpl_config.h"
#include "pscc_runscript.h"
#include "pscc_netdev.h"
#include "pscc_nwifcfg.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#ifndef PSCC_CONNID_START
#define PSCC_CONNID_START 1000
#endif

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static pscc_msg_t *handle_req_create(pscc_req_create_t *req_create_p);
static pscc_msg_t *handle_req_destroy(pscc_req_destroy_t *req_destroy_p);
static pscc_msg_t *handle_req_connect(pscc_req_connect_t *req_connect_p);
static pscc_msg_t *handle_req_disconnect(pscc_req_disconnect_t *req_disconnect_p);
static pscc_msg_t *handle_req_set(pscc_req_set_t *req_set_p);
static pscc_msg_t *handle_req_get(pscc_req_get_t *req_get_p);
static pscc_msg_t *handle_req_fetch_data_counters(pscc_req_fetch_data_counters_t *req_fetch_data_counters_p);
static pscc_msg_t *handle_req_reset_data_counters(pscc_req_reset_data_counters_t *req_reset_data_counters_p);
static pscc_msg_t *handle_req_modify(pscc_req_modify_t *req_modify_p);
static pscc_msg_t *handle_req_list_all_connections(pscc_req_list_all_connections_t *req_list_all_connections_p);
static pscc_msg_t *handle_req_attach(pscc_req_attach_t *req_attach_p);
static pscc_msg_t *handle_req_attach_status(pscc_req_attach_status_t *req_attach_status_p);
static pscc_msg_t *handle_req_detach(pscc_req_detach_t *req_detach_p);
static pscc_msg_t *handle_req_generate_uplink_data(pscc_req_generate_uplink_data_t *req_generate_uplink_data_p);
static pscc_msg_t *handle_req_query_attach_mode(pscc_req_query_attach_mode_t *req_query_attach_mode_p);
static pscc_msg_t *handle_req_configure_attach_mode(pscc_req_configure_attach_mode_t *req_configure_attach_mode_p);
static pscc_msg_t *handle_req_query_ping_block_mode(pscc_req_query_ping_block_mode_t *req_query_ping_block_mode_p);
static pscc_msg_t *handle_req_configure_ping_block_mode(pscc_req_configure_ping_block_mode_t *req_configure_ping_block_mode_p);
static pscc_msg_t *handle_req_init_handler (pscc_req_init_handler_t *req_init_handler_p);

static int alloc_connid(void);
static void pscc_bearer_event(int connid, pscc_bearer_event_t event, pscc_reason_t reason, int cause);
static void pscc_bearer_attach_event(pscc_bearer_attach_event_t event, pscc_attach_event_params_t attach_event_params, int cause, uint32_t ct);
static void pscc_bearer_ping_block_mode_event(pscc_bearer_ping_block_mode_event_t event, pscc_ping_block_mode_t ping_mode, uint32_t ct);
static void pscc_bearer_rc_event(pscc_bearer_rc_event_t event, pscc_rc_t* pscc_rc_p);
static void handle_event_activating(pscc_object_t *object_p,pscc_msg_t *msg_p,bool *sendevent_p);
static void handle_event_activated(pscc_object_t *object_p,int cause, pscc_msg_t *msg,bool *sendevent);
static void handle_event_deactivating(pscc_object_t *object_p,pscc_msg_t *msg,bool *sendevent);
static void handle_event_deactivated(pscc_object_t *object_p,pscc_reason_t  reason, int cause, pscc_msg_t *msg_,bool *sendevent_p);
static void handle_event_modify_failed(pscc_object_t *object_p,pscc_reason_t  reason, int cause, pscc_msg_t *msg_p,bool *sendev);
static void handle_event_modified(pscc_object_t *object_p, pscc_msg_t *msg_p,bool *sendev);
static void handle_event_data_counters_fetched(pscc_object_t *object_p, pscc_msg_t *msg_p,bool *sendev);
static void handle_event_data_counters_reset(pscc_object_t *object_p, pscc_msg_t *msg_p,bool *sendev);
static void handle_event_rc_requested(pscc_rc_t* pscc_rc_p);
static int pscc_set_param(int connid, mpl_param_element_t *param_p);
static void pscc_delete_param(int connid, pscc_paramid_t id);
static void pscc_netdev_destroyed(int connid);
static int pscc_configure_network_interface(int connid);
static int pscc_reset_network_interface(int connid);
static int pscc_configure_network_interface_name(int connid);
static char *pscc_rename_network_interface(char *if_name, char *if_prefix, int if_index);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * pscc_handler()
 **/
int pscc_handler(char *req_p, size_t reqlen, char *resp_p, size_t *resplen_p)
{
  pscc_msg_t *req_msg_p;
  pscc_msg_t *resp_msg_p = NULL;
  int result = 0;
  pscc_msgid_t resp_msg_id;

  assert(resp_p != NULL);
  assert(resplen_p != NULL);

  PSCC_DBG_TRACE(LOG_DEBUG, "Received request message: %s\n", req_p);

  req_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == req_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR,
                   "Failed creating a response msg, request message is NULL, trying to send an emergency response...\n");
    goto SendResponse_and_Return;
  }

  if(0 > mpl_msg_unpack(req_p, reqlen, (mpl_msg_t*) req_msg_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to unpack message: %s\n", req_p);
    goto SendResponse_and_Return;
  }

  if (PSCC_CONNID_PRESENT(req_msg_p)) {
    PSCC_DBG_TRACE(LOG_DEBUG, "Received request: %s (connid=%d)\n",
       pscc_msg_id_get_string(req_msg_p->common.id),
       PSCC_GET_CONNID(req_msg_p));
  }
  else {
    PSCC_DBG_TRACE(LOG_DEBUG, "Received request: %s (connid=<not present>)\n",
       pscc_msg_id_get_string(req_msg_p->common.id));
  }

  if (pscc_msgtype_req != req_msg_p->common.type) {
    PSCC_DBG_TRACE(LOG_ERR, "Unexpected message type: %s\n",
                   mpl_param_value_get_string(pscc_paramid_message_type, &req_msg_p->common.type));
    goto SendResponse_and_Return;
  }

  switch(req_msg_p->common.id)
  {
  case pscc_create:
    resp_msg_p = handle_req_create(&req_msg_p->req_create);
    break;
  case pscc_destroy:
    resp_msg_p = handle_req_destroy(&req_msg_p->req_destroy);
    break;
  case pscc_connect:
    resp_msg_p = handle_req_connect(&req_msg_p->req_connect);
    break;
  case pscc_disconnect:
    resp_msg_p = handle_req_disconnect(&req_msg_p->req_disconnect);
    break;
  case pscc_set:
    resp_msg_p = handle_req_set(&req_msg_p->req_set);
    break;
  case pscc_get:
    resp_msg_p = handle_req_get(&req_msg_p->req_get);
    break;
  case pscc_fetch_data_counters:
    resp_msg_p = handle_req_fetch_data_counters(&req_msg_p->req_fetch_data_counters);
    break;
  case pscc_reset_data_counters:
    resp_msg_p = handle_req_reset_data_counters(&req_msg_p->req_reset_data_counters);
    break;
  case pscc_list_all_connections:
    resp_msg_p = handle_req_list_all_connections(&req_msg_p->req_list_all_connections);
    break;
  case pscc_modify:
    resp_msg_p = handle_req_modify(&req_msg_p->req_modify);
    break;
  case pscc_attach:
    resp_msg_p = handle_req_attach(&req_msg_p->req_attach);
    break;
  case pscc_attach_status:
    resp_msg_p = handle_req_attach_status(&req_msg_p->req_attach_status);
    break;
  case pscc_detach:
    resp_msg_p = handle_req_detach(&req_msg_p->req_detach);
    break;
  case pscc_generate_uplink_data:
    resp_msg_p = handle_req_generate_uplink_data(&req_msg_p->req_generate_uplink_data);
    break;
  case pscc_query_attach_mode:
    resp_msg_p = handle_req_query_attach_mode(&req_msg_p->req_query_attach_mode);
    break;
  case pscc_configure_attach_mode:
    resp_msg_p = handle_req_configure_attach_mode(&req_msg_p->req_configure_attach_mode);
    break;
  case pscc_query_ping_block_mode:
    resp_msg_p = handle_req_query_ping_block_mode(&req_msg_p->req_query_ping_block_mode);
    break;
  case pscc_configure_ping_block_mode:
    resp_msg_p = handle_req_configure_ping_block_mode(&req_msg_p->req_configure_ping_block_mode);
    break;
  case pscc_init_handler:
    resp_msg_p = handle_req_init_handler(&req_msg_p->req_init_handler);
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "Unsupported message: %s\n",
                   pscc_msg_id_get_string(req_msg_p->common.id));
    break;
  }

  if (NULL != resp_msg_p)
  {
    resp_msg_id = resp_msg_p->common.id;

    if(0 > mpl_msg_pack((mpl_msg_t*) resp_msg_p, &resp_p, resplen_p))
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed packing response message: %s\n",
                     pscc_msg_id_get_string(resp_msg_p->common.id));
      mpl_msg_free((mpl_msg_t*) resp_msg_p);
      resp_msg_p = NULL;
    }
  }

  SendResponse_and_Return:
  if (NULL == req_msg_p)
  {
    PSCC_DBG_TRACE(LOG_WARNING,
                   "Cant send an emergency response due to request message being NULL\n");
  }
  else if (NULL == resp_msg_p)
  {
    mpl_msg_resp_t *err_resp_p;

    /* Last resort: Respond with failed to keep client happy */
    PSCC_DBG_TRACE(LOG_WARNING,
                   "Failed creating a response to msg, trying to send an emergency response...\n");

    err_resp_p = (mpl_msg_resp_t*)req_msg_p;

    err_resp_p->type = pscc_msgtype_resp;
    err_resp_p->result = pscc_result_failed_unspecified;

    if (0 > mpl_msg_pack((mpl_msg_t *)err_resp_p, &resp_p, resplen_p))
    {
      int len;

      /* Fixme: Should we here just force out sending of one character to
       *        prevent the client from hanging? */
      PSCC_DBG_TRACE(LOG_ERR, "Failed packing emergency response message: %s\n",
                     pscc_msg_id_get_string(err_resp_p->id));
      len = snprintf(resp_p, *resplen_p, "%s", pscc_event_failure_buf);

      resp_msg_id = err_resp_p->id;

      if ((len < 0) ||
          (len >= (int)*resplen_p))
        result = -1;
      else
        *resplen_p = len;
    }
  }

  if (resp_p != NULL)
    PSCC_DBG_TRACE(LOG_DEBUG, "Sending response message: %s\n", resp_p);

  mpl_msg_free((mpl_msg_t*) resp_msg_p);
  mpl_msg_free((mpl_msg_t*) req_msg_p);
  return (result);
}

/**
 * pscc_get_param()
 **/
mpl_param_element_t* pscc_get_param(int connid,pscc_paramid_t id)
{
  pscc_object_t *object_p;
  mpl_list_t **param_list_pp;
  mpl_param_element_t *param_elem_p;

  /* find parent object */
  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "object with id %d not found\n",connid);
    return (NULL);
  }

  param_list_pp = pscc_obj_get_param_list_ref(object_p);

  param_elem_p = mpl_param_list_find(id, *param_list_pp);
  if (NULL == param_elem_p)
  {
    /* ...no get it from the default configuration instead */
    param_elem_p = mpl_config_get_para(id, &psccd_config);
    if (NULL == param_elem_p)
    {
      /* No value found for parameter! */
      return (NULL);
    }
  }
  return param_elem_p;
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * handle_req_create()
 **/
static pscc_msg_t *handle_req_create(pscc_req_create_t *req_create_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  int connid = -1;
  int bearer_handle;
  int sim_handle;

  assert(req_create_p != NULL);
  assert(req_create_p->type == pscc_msgtype_req);
  assert(req_create_p->id == pscc_create);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_create.id = req_create_p->id;
  resp_msg_p->resp_create.result = pscc_result_ok;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_create_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_create_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_create.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_create_p) || 0 == PSCC_GET_CONNID((pscc_msg_t*)req_create_p)) /* allocate connection id */
  {
    connid = alloc_connid();
    assert(connid != 0);
    PSCC_DBG_TRACE(LOG_DEBUG, "New connid generated: %d\n",
                   connid);
  }
  else
  {
    /* client has requested a conn id, check if it is available */
    connid = PSCC_GET_CONNID((pscc_msg_t*)req_create_p);
    if(NULL != pscc_obj_get(connid))
    {
      /* a PS connection object exist on this conn id */
      PSCC_DBG_TRACE(LOG_DEBUG, "Requested connid in use: %d\n", connid);
      resp_msg_p->resp_create.result = pscc_result_failed_object_exists;
      goto cleanup_and_return;
    }
  }

  /* add entry */
  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_create.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  object_p = pscc_obj_add(connid);
  if(NULL == object_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add ps connection object\n");
    resp_msg_p->resp_create.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  /* create bearer object */
  bearer_handle = pscc_bearer_alloc(connid);

  if(bearer_handle < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to create ps bearer object\n");
    resp_msg_p->resp_create.result = pscc_result_failed_unspecified;
    /* delete object */
    (void)pscc_obj_delete(connid);
    goto cleanup_and_return;
  }
  pscc_obj_set_bearer_handle(object_p,bearer_handle);

  /* create sim object */
  sim_handle = pscc_sim_alloc(connid);

  if(sim_handle < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to create pscc sim object\n");
    resp_msg_p->resp_create.result = pscc_result_failed_unspecified;
    /* delete object */
    if (pscc_sim_free(connid) < 0){
      PSCC_DBG_TRACE(LOG_ERR, "Failed to free sim object\n");
    }
    (void)pscc_obj_delete(connid);
    goto cleanup_and_return;
  }
  pscc_obj_set_sim_handle(object_p,sim_handle);

  cleanup_and_return:
  return (resp_msg_p);
}


/**
 * handle_req_destroy()
 **/
static pscc_msg_t *handle_req_destroy(pscc_req_destroy_t *req_destroy_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  int connid;

  assert(req_destroy_p != NULL);
  assert(req_destroy_p->type == pscc_msgtype_req);
  assert(req_destroy_p->id == pscc_destroy);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_destroy.id = req_destroy_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_destroy_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_destroy_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_destroy.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_destroy_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_destroy.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_destroy_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_destroy.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_destroy.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_DEBUG, "Requested conn id not found\n");
    resp_msg_p->resp_destroy.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  /* destroy bearer object */
  if(0 > pscc_bearer_free(pscc_obj_get_bearer_handle(object_p)))
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "Not allowed to delete ps object, bearer not idle\n");
    resp_msg_p->resp_destroy.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  /* destroy sim object */
  if(0 > pscc_sim_free(pscc_obj_get_sim_handle(object_p)))
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "Not allowed to delete ps object, sim not idle\n");
    resp_msg_p->resp_destroy.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  if(pscc_connection_status_disconnected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "Not allowed to delete ps object, not idle\n");
    resp_msg_p->resp_destroy.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  (void)pscc_obj_delete(connid);

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_connect()
 **/
static pscc_msg_t *handle_req_connect(pscc_req_connect_t *req_connect_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  int connid;

  assert(req_connect_p != NULL);
  assert(req_connect_p->type == pscc_msgtype_req);
  assert(req_connect_p->id == pscc_connect);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_connect.id = req_connect_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_connect_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_connect_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_connect.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_connect_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_connect.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_connect_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_connect.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_connect.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_connect.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }


  if(pscc_connection_status_disconnected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Not allowed to do connect on a non-idle ps object\n");
    resp_msg_p->resp_connect.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  /* try to connect */
  PSCC_DBG_TRACE(LOG_DEBUG, "activate bearer\n");
  if(0 > pscc_bearer_activate(pscc_obj_get_bearer_handle(object_p),pscc_get_param,pscc_set_param,pscc_delete_param,pscc_bearer_event))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to start bearer activation\n");
    resp_msg_p->resp_connect.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "connecting\n");
  pscc_obj_set_state(object_p,pscc_connection_status_connecting);

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_disconnect()
 **/
static pscc_msg_t *handle_req_disconnect(pscc_req_disconnect_t *req_disconnect_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  int connid;

  assert(req_disconnect_p != NULL);
  assert(req_disconnect_p->type == pscc_msgtype_req);
  assert(req_disconnect_p->id == pscc_disconnect);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_disconnect.id = req_disconnect_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_disconnect_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_disconnect_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_disconnect.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_disconnect_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_disconnect.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_disconnect_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_disconnect.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_disconnect.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_disconnect.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }


  switch(pscc_obj_get_state(object_p))
  {
  case pscc_connection_status_modifying:
  case pscc_connection_status_connecting:
  case pscc_connection_status_connected:
    break;

  case pscc_connection_status_disconnecting:
  case pscc_connection_status_disconnected:
  default:
    resp_msg_p->resp_disconnect.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  /* disconnect bearer */
  PSCC_DBG_TRACE(LOG_DEBUG, "deactivate bearer\n");
  if(0 > pscc_bearer_deactivate(pscc_obj_get_bearer_handle(object_p)))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to start bearer deactivation\n");
    resp_msg_p->resp_disconnect.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "disconnecting\n");
  pscc_obj_set_state(object_p,pscc_connection_status_disconnecting);

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_set()
 **/
static pscc_msg_t *
handle_req_set(pscc_req_set_t *req_set_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  mpl_param_element_t *param_elem_p;
  mpl_param_element_t *old_param_elem_p;
  mpl_list_t**param_list_pp;
  mpl_list_t *list_p,*tmp_p;
  int connid;

  assert(req_set_p != NULL);
  assert(req_set_p->type == pscc_msgtype_req);
  assert(req_set_p->id == pscc_set);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_set.id = req_set_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_set_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_set_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_set.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_set_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_set.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_set_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_set.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_set.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);
  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_set.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  param_list_pp = pscc_obj_get_param_list_ref(object_p);

  /* First loop through all parameters and check if they are OK */
  MPL_LIST_FOR_EACH(req_set_p->param_list_p, list_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (pscc_paramid_connid == param_elem_p->id)
      continue;

    if (pscc_paramid_ct == param_elem_p->id)
      continue;

    /* Check if the parameter is allowed to set */
    if (!mpl_param_allow_set(param_elem_p->id))
    {
      PSCC_DBG_TRACE(LOG_ERR,
                     "Trying to set unsettable parameter: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      resp_msg_p->resp_set.result = pscc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }
  }

  /* ... then set them all on the object parameter list */
  MPL_LIST_FOR_EACH_SAFE(req_set_p->param_list_p, list_p, tmp_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (pscc_paramid_connid == param_elem_p->id)
      continue;

    if (pscc_paramid_ct == param_elem_p->id)
      continue;

    /* Check if the parameter already exists */
    old_param_elem_p = mpl_param_list_find(param_elem_p->id, *param_list_pp);
    if (NULL != old_param_elem_p)
    {
      /* ...yes it does so remove and destroy it */
      (void)mpl_list_remove(param_list_pp, &old_param_elem_p->list_entry);
      mpl_param_element_destroy(old_param_elem_p);
    }

    /* Move the new parameter from the command */
    if (NULL != param_elem_p->value_p)
    {
      (void)mpl_list_remove(&req_set_p->param_list_p, &param_elem_p->list_entry);
      mpl_list_add(param_list_pp, &param_elem_p->list_entry);
    }
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_list_all_connections()
 **/
static pscc_msg_t *
handle_req_list_all_connections(pscc_req_list_all_connections_t *req_list_all_connections_p)
{
  pscc_msg_t *resp_msg_p;

  assert(req_list_all_connections_p != NULL);
  assert(req_list_all_connections_p->type == pscc_msgtype_req);
  assert(req_list_all_connections_p->id == pscc_list_all_connections);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_list_all_connections.id = pscc_list_all_connections;
  resp_msg_p->resp_list_all_connections.result = pscc_result_ok;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_list_all_connections_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_list_all_connections_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_list_all_connections.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if(0 != pscc_obj_get_all_connid(&resp_msg_p->resp_list_all_connections.param_list_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to fetch connid list");
    resp_msg_p->resp_list_all_connections.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  return (resp_msg_p);

  cleanup_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp_list_all_connections.param_list_p);
  if (PSCC_CT_PRESENT((pscc_msg_t*)req_list_all_connections_p))
  {
    uint32_t ct;
    /* Try to add the ct to the response */
    ct = PSCC_GET_CT((pscc_msg_t*)req_list_all_connections_p);
    (void)mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct);
  }
  return (resp_msg_p);
}


/**
 * handle_req_modify()
 **/
static pscc_msg_t *
handle_req_modify(pscc_req_modify_t *req_modify_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  mpl_param_element_t *param_elem_p;
  mpl_param_element_t *old_param_elem_p;
  mpl_list_t**param_list_pp;
  mpl_list_t *list_p,*tmp_p;
  int connid;

  assert(req_modify_p != NULL);
  assert(req_modify_p->type == pscc_msgtype_req);
  assert(req_modify_p->id == pscc_modify);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_modify.id = req_modify_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_modify_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_modify_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_set.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_modify_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_modify.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_modify_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_modify.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_modify.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_modify.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  /* Check if modification is allowed */
  if(pscc_connection_status_connected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Not allowed to do modify. PS object in illegal state\n");
    resp_msg_p->resp_modify.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  /* Store parameters */
  param_list_pp = pscc_obj_get_param_list_ref(object_p);

  /* First loop through all parameters and check if they are OK */
  MPL_LIST_FOR_EACH(req_modify_p->param_list_p, list_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (pscc_paramid_connid == param_elem_p->id)
      continue;

    if (pscc_paramid_ct == param_elem_p->id)
      continue;

    /* Check if the parameter is allowed to set */
    if (!mpl_param_allow_set(param_elem_p->id))
    {
      PSCC_DBG_TRACE(LOG_ERR,
                     "Trying to set unsettable parameter: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      resp_msg_p->resp_modify.result = pscc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }
  }

  /* ... then set them all on the object parameter list */
  MPL_LIST_FOR_EACH_SAFE(req_modify_p->param_list_p, list_p, tmp_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (pscc_paramid_connid == param_elem_p->id)
      continue;

    /* Check if the parameter already exists */
    old_param_elem_p = mpl_param_list_find(param_elem_p->id, *param_list_pp);
    if (NULL != old_param_elem_p)
    {
      /* ...yes it does so remove and destroy it */
      (void)mpl_list_remove(param_list_pp, &old_param_elem_p->list_entry);
      mpl_param_element_destroy(old_param_elem_p);
    }

    /* Move the new parameter from the command */
    if (NULL != param_elem_p->value_p)
    {
      (void)mpl_list_remove(&req_modify_p->param_list_p, &param_elem_p->list_entry);
      mpl_list_add(param_list_pp, &param_elem_p->list_entry);
    }
  }

  if (pscc_bearer_modify(pscc_obj_get_bearer_handle(object_p)) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to modify bearer\n");
    resp_msg_p->resp_modify.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "modifying\n");
  pscc_obj_set_state(object_p,pscc_connection_status_modifying);

  //Event will be sent when the bearer reports that the parameters are set

  return (resp_msg_p);

  cleanup_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp_modify.param_list_p);
  if (PSCC_CT_PRESENT((pscc_msg_t*)req_modify_p))
  {
    uint32_t ct;
    /* Try to add the ct to the response */
    ct = PSCC_GET_CT((pscc_msg_t*)req_modify_p);
    (void)mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct);
  }
  return (resp_msg_p);
}

/**
 * handle_req_get()
 **/
static pscc_msg_t *
handle_req_get(pscc_req_get_t *req_get_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  mpl_param_element_t *param_elem_p;
  mpl_param_element_t *old_param_elem_p;
  mpl_list_t**param_list_pp;
  mpl_list_t *list_p;
  int connid;

  assert(req_get_p != NULL);
  assert(req_get_p->type == pscc_msgtype_req);
  assert(req_get_p->id == pscc_get);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_get.id = req_get_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_get_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_get_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_get.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_get_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_get.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_get_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_get.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_get.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_get.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  param_list_pp = pscc_obj_get_param_list_ref(object_p);
  MPL_LIST_FOR_EACH(req_get_p->param_list_p, list_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (pscc_paramid_connid == param_elem_p->id)
      continue;

    if (pscc_paramid_ct == param_elem_p->id)
      continue;

    /* Check if the parameter is allowed to get */
    if (!mpl_param_allow_get(param_elem_p->id))
    {
      PSCC_DBG_TRACE(LOG_ERR, "Trying to get ungettable parameter: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      resp_msg_p->resp_get.result = pscc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }

    /* Somewhat strange, but we allow it ... */
    if (NULL != param_elem_p->value_p)
    {
      PSCC_DBG_TRACE(LOG_WARNING, "Parameter contains value in get command: %s (ignoring)\n",
                     mpl_param_id_get_string(param_elem_p->id));
    }

    /* Check if the parameter exists */
    old_param_elem_p = mpl_param_list_find(param_elem_p->id, *param_list_pp);
    if (NULL != old_param_elem_p)
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "parameter exist: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      do
      {
        /* ...yes copy all into the response parameter list */
        param_elem_p = mpl_param_element_clone(old_param_elem_p);
        if (NULL == param_elem_p)
        {
          PSCC_DBG_TRACE(LOG_ERR, "Failed cloning parameter\n");
          resp_msg_p->resp_get.result = pscc_result_failed_unspecified;
          goto cleanup_and_return;
        }
        mpl_list_add(&resp_msg_p->resp_get.param_list_p, &param_elem_p->list_entry);
      } while(NULL != (old_param_elem_p = mpl_param_list_find_next(param_elem_p->id, old_param_elem_p)));
    }
    else
    {
      PSCC_DBG_TRACE(LOG_DEBUG, "parameter does not exist: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      if(param_elem_p->id == pscc_paramid_connection_status)
      {/* For connection status, it's not a param we put in the list,
        we have to get it use get_state()*/
        pscc_connection_status_t status = pscc_obj_get_state(object_p);

        param_elem_p = mpl_param_element_create(pscc_paramid_connection_status, &status);
        if(NULL == param_elem_p)
        {
          PSCC_DBG_TRACE(LOG_ERR, "Failed creating parameter\n");
          resp_msg_p->resp_get.result = pscc_result_failed_unspecified;
          goto cleanup_and_return;
        }
      }
      else
      {/* Requested param is not connection_status and not in the list.
         go and check in config file list */
        old_param_elem_p = mpl_config_get_para(param_elem_p->id, &psccd_config);
        if (NULL == old_param_elem_p)
        {/* even not in the config list, create an empty one */
          PSCC_DBG_TRACE(LOG_WARNING, "No config value found for parameter: %s (continue)\n",
                         mpl_param_id_get_string(param_elem_p->id));
          param_elem_p = mpl_param_element_create_empty(param_elem_p->id);
        }
        else/*find requested param in the config list, clone it*/
          param_elem_p = mpl_param_element_clone(old_param_elem_p);
        if (NULL == param_elem_p)
        {
          PSCC_DBG_TRACE(LOG_ERR, "Failed cloning parameter\n");
          resp_msg_p->resp_get.result = pscc_result_failed_unspecified;
          goto cleanup_and_return;
        }
      }
      mpl_list_add(&resp_msg_p->resp_get.param_list_p, &param_elem_p->list_entry);
    }
  }

  return (resp_msg_p);

  cleanup_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp_get.param_list_p);
  if (PSCC_CT_PRESENT((pscc_msg_t*)req_get_p))
  {
    uint32_t ct;
    /* Try to add the ct to the response */
    ct = PSCC_GET_CT((pscc_msg_t*)req_get_p);
    (void)mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct);
  }
  return (resp_msg_p);
}

/**
 * handle_req_fetch_data_counters()
 **/
static pscc_msg_t *
handle_req_fetch_data_counters(pscc_req_fetch_data_counters_t *req_fetch_data_counters_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  int connid;

  assert(req_fetch_data_counters_p != NULL);
  assert(req_fetch_data_counters_p->type == pscc_msgtype_req);
  assert(req_fetch_data_counters_p->id == pscc_fetch_data_counters);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_fetch_data_counters.id = req_fetch_data_counters_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_fetch_data_counters_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_fetch_data_counters_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_fetch_data_counters.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_fetch_data_counters_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_fetch_data_counters.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_fetch_data_counters_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_fetch_data_counters.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_fetch_data_counters.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_fetch_data_counters.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  /* Check if uplink data generation is allowed */
  if(pscc_connection_status_connected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Not allowed to fetch data counters. PS object in illegal state\n");
    resp_msg_p->resp_fetch_data_counters.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  if (pscc_bearer_data_counters(pscc_obj_get_bearer_handle(object_p), true) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to fetch data counters in bearer\n");
    resp_msg_p->resp_fetch_data_counters.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_reset_data_counters()
 **/
static pscc_msg_t *
handle_req_reset_data_counters(pscc_req_reset_data_counters_t *req_reset_data_counters_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  int connid;

  assert(req_reset_data_counters_p != NULL);
  assert(req_reset_data_counters_p->type == pscc_msgtype_req);
  assert(req_reset_data_counters_p->id == pscc_reset_data_counters);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_reset_data_counters.id = req_reset_data_counters_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_reset_data_counters_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_reset_data_counters_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_reset_data_counters.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_reset_data_counters_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_reset_data_counters.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_reset_data_counters_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_reset_data_counters.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_reset_data_counters.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_reset_data_counters.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  /* Check if uplink data generation is allowed */
  if(pscc_connection_status_connected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Not allowed to reset data counters. PS object in illegal state\n");
    resp_msg_p->resp_reset_data_counters.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  if (pscc_bearer_data_counters(pscc_obj_get_bearer_handle(object_p), false) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to reset data counters in bearer\n");
    resp_msg_p->resp_reset_data_counters.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}


/**
 * handle_req_attach()
 **/
static pscc_msg_t *handle_req_attach (pscc_req_attach_t *req_attach_p)
{
  pscc_msg_t *resp_msg_p = NULL;
  uint32_t ct = 0;
  pscc_attach_status_t attach_status = pscc_attach_status_attached;

  assert(req_attach_p != NULL);
  assert(req_attach_p->type == pscc_msgtype_req);
  assert(req_attach_p->id == pscc_attach);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_attach.id = req_attach_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_attach_p))
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "ct provided in attach\n");
    ct = PSCC_GET_CT((pscc_msg_t*)req_attach_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_attach.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  /* Check if operation is allowed (synchronous, no query is sent to the modem)*/
  if (0 == pscc_bearer_attach_status(&attach_status)) {
    if (pscc_attach_status_attached == attach_status) {
      PSCC_DBG_TRACE(LOG_DEBUG, "handle_req_attach called when pscc is already attached. \n");
      resp_msg_p->resp_attach.result = pscc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }
  }

  resp_msg_p->resp_attach.result = pscc_result_ok;

  /* do ps attach */
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer attach\n");
  if (pscc_bearer_attach(pscc_bearer_attach_event, ct) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to do bearer attach\n");
    resp_msg_p->resp_attach.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_attach_status()
 **/
static pscc_msg_t *handle_req_attach_status(pscc_req_attach_status_t *req_attach_status_p)
{
  pscc_msg_t *resp_msg_p;
  uint32_t ct = 0;
  pscc_attach_status_t attach_status = pscc_attach_status_detached;

  assert(req_attach_status_p != NULL);
  assert(req_attach_status_p->type == pscc_msgtype_req);
  assert(req_attach_status_p->id == pscc_attach_status);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_attach_status.id = req_attach_status_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_attach_status_p))
  {
    ct = PSCC_GET_CT((pscc_msg_t*)req_attach_status_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_attach_status.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  resp_msg_p->resp_attach_status.result = pscc_result_ok;

  /* check ps attach status */
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer attach status\n");
  if (pscc_bearer_attach_status(&attach_status) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to get bearer attach status\n");
    resp_msg_p->resp_attach_status.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_attach_status, &attach_status) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add param to list (attach status)\n");
    resp_msg_p->resp_attach_status.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_detach()
 **/
static pscc_msg_t *handle_req_detach(pscc_req_detach_t *req_detach_p)
{
  pscc_msg_t *resp_msg_p = NULL;
  uint32_t ct = 0;
  pscc_attach_status_t attach_status = pscc_attach_status_detached;

  assert(req_detach_p != NULL);
  assert(req_detach_p->type == pscc_msgtype_req);
  assert(req_detach_p->id == pscc_detach);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_detach.id = req_detach_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_detach_p))
  {
    ct = PSCC_GET_CT((pscc_msg_t*)req_detach_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_detach.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  /* Check if operation is allowed. (synchronous, no query is sent to the modem) */
  if (0 == pscc_bearer_attach_status(&attach_status)) {
    if (pscc_attach_status_detached == attach_status) {
      PSCC_DBG_TRACE(LOG_DEBUG, "handle_req_detach called when pscc is already detached. \n");
      resp_msg_p->resp_detach.result = pscc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }
  }

  resp_msg_p->resp_detach.result = pscc_result_ok;

  /* do ps detach */
  PSCC_DBG_TRACE(LOG_DEBUG, "bearer detach\n");
  if (pscc_bearer_detach(pscc_bearer_attach_event, ct) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to do bearer detach\n");
    resp_msg_p->resp_detach.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_generate_uplink_data()
 **/
static pscc_msg_t *
handle_req_generate_uplink_data(pscc_req_generate_uplink_data_t *req_generate_uplink_data_p)
{
  pscc_msg_t *resp_msg_p;
  pscc_object_t *object_p;
  mpl_param_element_t *param_elem_p;
  int connid;

  assert(req_generate_uplink_data_p != NULL);
  assert(req_generate_uplink_data_p->type == pscc_msgtype_req);
  assert(req_generate_uplink_data_p->id == pscc_generate_uplink_data);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_generate_uplink_data.id = req_generate_uplink_data_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_generate_uplink_data_p))
  {
    uint32_t ct;

    ct = PSCC_GET_CT((pscc_msg_t*)req_generate_uplink_data_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_set.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!PSCC_CONNID_PRESENT((pscc_msg_t*)req_generate_uplink_data_p)) {
    PSCC_DBG_TRACE(LOG_ERR, "Connid not in request message\n");
    resp_msg_p->resp_generate_uplink_data.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }

  connid = PSCC_GET_CONNID((pscc_msg_t*)req_generate_uplink_data_p);

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_connid, &connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to add connid to message\n");
    resp_msg_p->resp_generate_uplink_data.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  resp_msg_p->resp_generate_uplink_data.result = pscc_result_ok;

  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Requested conn id not found\n");
    resp_msg_p->resp_generate_uplink_data.result = pscc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  /* Check if uplink data generation is allowed */
  if(pscc_connection_status_connected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Not allowed to generate uplink data. PS object in illegal state\n");
    resp_msg_p->resp_generate_uplink_data.result = pscc_result_failed_operation_not_allowed;
    goto cleanup_and_return;
  }

  /* Check if connection is initiated in test mode */
  param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_pdp_type);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    if(*(pscc_pdp_type_t*) param_elem_p->value_p != pscc_pdp_type_test)
    {
        PSCC_DBG_TRACE(LOG_ERR, "Not allowed to generate uplink data. Connection must be initiated with pdp type test\n");
        resp_msg_p->resp_generate_uplink_data.result = pscc_result_failed_operation_not_allowed;
        goto cleanup_and_return;
    }
  }

  if (pscc_bearer_generate_uplink_data(pscc_obj_get_bearer_handle(object_p)) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to generate uplink data in bearer\n");
    resp_msg_p->resp_generate_uplink_data.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_query_attach_mode()
 **/
static pscc_msg_t *handle_req_query_attach_mode(pscc_req_query_attach_mode_t *req_query_attach_mode_p)
{
  pscc_msg_t *resp_msg_p = NULL;
  uint32_t ct = 0;

  assert(req_query_attach_mode_p != NULL);
  assert(req_query_attach_mode_p->type == pscc_msgtype_req);
  assert(req_query_attach_mode_p->id == pscc_query_attach_mode);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_query_attach_mode.id = req_query_attach_mode_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_query_attach_mode_p))
  {
    ct = PSCC_GET_CT((pscc_msg_t*)req_query_attach_mode_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_query_attach_mode.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  resp_msg_p->resp_query_attach_mode.result = pscc_result_ok;

  /* query ps attach mode */
  PSCC_DBG_TRACE(LOG_DEBUG, "query ps attach mode\n");
  if (pscc_bearer_query_attach_mode(pscc_bearer_attach_event, ct) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to query ps attach mode\n");
    resp_msg_p->resp_query_attach_mode.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * handle_req_configure_attach_mode()
 **/
static pscc_msg_t *handle_req_configure_attach_mode(pscc_req_configure_attach_mode_t *req_configure_attach_mode_p)
{
  pscc_msg_t *resp_msg_p = NULL;
  uint32_t ct = 0;
  pscc_attach_mode_t attach_mode = pscc_attach_mode_manual;

  assert(req_configure_attach_mode_p != NULL);
  assert(req_configure_attach_mode_p->type == pscc_msgtype_req);
  assert(req_configure_attach_mode_p->id == pscc_configure_attach_mode);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_configure_attach_mode.id = req_configure_attach_mode_p->id;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_configure_attach_mode_p))
  {
    ct = PSCC_GET_CT((pscc_msg_t*)req_configure_attach_mode_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_configure_attach_mode.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  if (!MPL_MSG_PARAM_PRESENT((pscc_msg_t*)req_configure_attach_mode_p, pscc_paramid_attach_mode))
  {
    PSCC_DBG_TRACE(LOG_ERR, "Attach mode not in request message\n");
    resp_msg_p->resp_configure_attach_mode.result = pscc_result_failed_parameter_not_found;
    goto cleanup_and_return;
  }
  attach_mode = MPL_MSG_GET_PARAM_VALUE((pscc_msg_t*)req_configure_attach_mode_p,
                                        pscc_attach_mode_t, pscc_paramid_attach_mode);

  resp_msg_p->resp_configure_attach_mode.result = pscc_result_ok;

  /* configure ps attach mode */
  PSCC_DBG_TRACE(LOG_DEBUG, "configure ps attach mode\n");
  if (pscc_bearer_configure_attach_mode(pscc_bearer_attach_event, attach_mode, ct) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to query ps attach mode\n");
    resp_msg_p->resp_configure_attach_mode.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);

}

/**
 * handle_req_query_ping_block_mode()
 **/
static pscc_msg_t *handle_req_query_ping_block_mode(pscc_req_query_ping_block_mode_t *req_query_ping_block_mode_p)
{
  pscc_msg_t* req_msg_p = (pscc_msg_t*)req_query_ping_block_mode_p;
  pscc_msg_t* resp_msg_p = NULL;
  uint32_t ct = 0;

  assert(req_msg_p != NULL);
  assert(req_msg_p->common.type == pscc_msgtype_req);
  assert(req_msg_p->common.id == pscc_query_ping_block_mode);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(!resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp.id = req_msg_p->common.id;
  resp_msg_p->resp.result = pscc_result_failed_unspecified;

  if (PSCC_CT_PRESENT(req_msg_p)) {
    ct = PSCC_GET_CT(req_msg_p);
    if (mpl_add_param_to_list(&resp_msg_p->resp.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      return resp_msg_p; /* Return failure... */
    }
  }

  if (pscc_bearer_query_ping_block_mode(pscc_bearer_ping_block_mode_event, ct) >= 0) {
    resp_msg_p->resp.result = pscc_result_ok;
  } else {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to query ps filter icmp ping mode\n");
  }

  return (resp_msg_p);
}

/**
 * handle_req_configure_ping_block_mode()
 **/
static pscc_msg_t *handle_req_configure_ping_block_mode(pscc_req_configure_ping_block_mode_t* req_configure_ping_block_mode_p)
{
  pscc_msg_t* req_msg_p = (pscc_msg_t*)req_configure_ping_block_mode_p;
  pscc_msg_t* resp_msg_p = NULL;
  uint32_t ct = 0;

  assert(req_msg_p != NULL);
  assert(req_msg_p->common.type == pscc_msgtype_req);
  assert(req_msg_p->common.id == pscc_configure_ping_block_mode);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(!resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp.id = req_msg_p->common.id;
  resp_msg_p->resp.result = pscc_result_failed_unspecified;

  if (PSCC_CT_PRESENT(req_msg_p)) {
    ct = PSCC_GET_CT(req_msg_p);
    if (mpl_add_param_to_list(&resp_msg_p->resp.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      return resp_msg_p; /* Return failure... */
    }
  }

  if (MPL_MSG_PARAM_PRESENT(req_msg_p, pscc_paramid_ping_block_mode)) {
    pscc_ping_block_mode_t mode;
    mode = MPL_MSG_GET_PARAM_VALUE(req_msg_p, pscc_ping_block_mode_t, pscc_paramid_ping_block_mode);
    if (pscc_bearer_configure_ping_block_mode(pscc_bearer_ping_block_mode_event, mode, ct) >= 0) {
      resp_msg_p->resp.result = pscc_result_ok;
    } else {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to configure filter icmp ping mode\n");
    }
  } else {
    PSCC_DBG_TRACE(LOG_ERR, "Filter icmp ping mode not in request message\n");
  }

  return (resp_msg_p);
}

/**
 * handle_req_init_handler()
 **/
static pscc_msg_t *handle_req_init_handler (pscc_req_init_handler_t *req_init_handler_p)
{
  pscc_msg_t *resp_msg_p = NULL;
  uint32_t ct = 0;
  pscc_cc_gprs_availability_t pscc_cc_gprs_availability = pscc_cc_gprs_availability_unknown;
  mpl_param_element_t* param_elem_p;
  bool aol_active = false;

  assert(req_init_handler_p != NULL);
  assert(req_init_handler_p->type == pscc_msgtype_req);
  assert(req_init_handler_p->id == pscc_init_handler);

  resp_msg_p = mpl_resp_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp_init_handler.id = req_init_handler_p->id;
  resp_msg_p->resp_init_handler.result = pscc_result_ok;

  if (PSCC_CT_PRESENT((pscc_msg_t*)req_init_handler_p))
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "ct provided in init\n");
    ct = PSCC_GET_CT((pscc_msg_t*)req_init_handler_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, pscc_paramid_ct, &ct) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp_init_handler.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  /* set aol mode in the modem (if config not set or if disabled we set to not active) */
  param_elem_p = mpl_config_get_para(pscc_paramid_aol_mode, &psccd_config);
  if (NULL != param_elem_p)
  {
    aol_active = (NULL != param_elem_p->value_p) && *((bool*) param_elem_p->value_p);
    PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_configure_aol_mode: %d\n", aol_active);
    if (pscc_bearer_configure_aol_mode(aol_active))
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to configure_aol_mode\n");
      resp_msg_p->resp_init_handler.result = pscc_result_failed_unspecified;
      goto cleanup_and_return;
    }
  }

  /* query attach status */
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_query_attach_status\n");
  if (pscc_bearer_query_attach_status(pscc_bearer_attach_event, ct) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to query ps attach status\n");
    goto cleanup_and_return;
  }

  /* check call control for gprs availability */
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_sim_cc_gprs_availability\n");
  if (pscc_sim_cc_gprs_availability(&pscc_cc_gprs_availability) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to check sim call control gprs availability\n");
    resp_msg_p->resp_init_handler.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }
  if (pscc_cc_gprs_availability_unknown != pscc_cc_gprs_availability)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "CC GPRS availability should be unknown at init but was: %d\n", pscc_cc_gprs_availability);
  }

  /* enable resource control in modem */
  PSCC_DBG_TRACE(LOG_DEBUG, "pscc_bearer_enable_resource_control\n");
  if (pscc_bearer_configure_rc(pscc_bearer_rc_event) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to enable_resource_control\n");
    resp_msg_p->resp_init_handler.result = pscc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  cleanup_and_return:
  return (resp_msg_p);
}

/**
 * alloc_connid()
 **/
static int alloc_connid(void)
{
  int connid;

  /**/
  for(connid=1*PSCC_CONNID_START;;connid++)
  {
    if(NULL == pscc_obj_get(connid))
      return (connid);
  }
}

/**
 * pscc_bearer_event()
 **/
static void pscc_bearer_event(int connid, pscc_bearer_event_t event, pscc_reason_t reason, int cause)
{
  pscc_object_t *object_p;
  char *eventbuf_p=NULL;
  size_t len=0;
  pscc_msg_t *msg_p;
  bool sendevent = false;

  /* find parent object */
  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "Object with id %d not found\n",connid);
    return;
  }

  msg_p = mpl_event_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to alloc memory\n");
    return;
  }
  switch(event)
  {
  case pscc_bearer_event_activating:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_activating\n");
    handle_event_activating(object_p,msg_p,&sendevent);
    break;
  case pscc_bearer_event_activated:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_activated :cause=%d\n",cause);
    handle_event_activated(object_p,cause,msg_p,&sendevent);
    break;
  case pscc_bearer_event_deactivating:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_deactivating\n");
    handle_event_deactivating(object_p,msg_p,&sendevent);
    break;
  case pscc_bearer_event_deactivated:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_deactivated reason=%d, cause=%d\n",reason,cause);
    handle_event_deactivated(object_p,reason,cause,msg_p,&sendevent);
    break;
  case pscc_bearer_event_modify_failed:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_modify_failed reason=%d, cause=%d\n",reason,cause);
    handle_event_modify_failed(object_p,reason,cause,msg_p,&sendevent);
    break;
  case pscc_bearer_event_modified:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_modify_success\n");
    handle_event_modified(object_p,msg_p,&sendevent);
    break;
  case pscc_bearer_event_data_counters_fetched:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_data_counters_fetched\n");
    handle_event_data_counters_fetched(object_p,msg_p,&sendevent);
    break;
  case pscc_bearer_event_data_counters_reset:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_event_data_counters_reset\n");
    handle_event_data_counters_reset(object_p,msg_p,&sendevent);
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unknown event %d\n",event);
    goto error_return;
  }

  if(sendevent)
  {
    if (0 > mpl_msg_pack((mpl_msg_t*) msg_p, &eventbuf_p, &len))
    {
      PSCC_DBG_TRACE(LOG_ERR, "failed to pack event message\n");
      goto error_return;
    }
    PSCC_DBG_TRACE(LOG_INFO, "Sending event: %s\n", eventbuf_p);
    (void)psccd_send_event(eventbuf_p,len);
  }

error_return:
  if(NULL != eventbuf_p)
    free(eventbuf_p);
  mpl_msg_free((mpl_msg_t*)msg_p);
}

/**
 * pscc_sim_event()
 **/
static void pscc_sim_event(pscc_sim_event_t event, pscc_rc_t* pscc_rc_p)
{
  switch(event)
  {
  case pscc_sim_event_cc_accept:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_sim_event_cc_accept\n");
    if (0 > pscc_bearer_respond_resource_control(pscc_rc_p, true, false))
      PSCC_DBG_TRACE(LOG_ERR, "Failed to respond to resource control\n");
    break;

  case pscc_sim_event_cc_reject:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_sim_event_cc_reject\n");
    if (0 > pscc_bearer_respond_resource_control(pscc_rc_p, false, false))
      PSCC_DBG_TRACE(LOG_ERR, "Failed to respond to resource control\n");
    break;

  case pscc_sim_event_cc_modified:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_sim_event_cc_modified\n");
    if (0 > pscc_bearer_respond_resource_control(pscc_rc_p, true, true))
      PSCC_DBG_TRACE(LOG_ERR, "Failed to respond to resource control\n");
    break;

  default:
    PSCC_DBG_TRACE(LOG_ERR, "unknown pscc_sim_event %d\n",event);
    return;
  }
  return;
}

/**
 * handle_event_activating()
 **/
static void handle_event_activating(pscc_object_t *object_p,pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  if(pscc_connection_status_connecting != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "activating event received in unexpected state\n");
    *sendevent_p = false;
    return;
  }

  msg_p->event_connecting.id = pscc_event_connecting;
  connid = pscc_obj_get_connid(object_p);
  msg_p->event_connecting.param_list_p = NULL;
  (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  *sendevent_p = true;

}

/**
 * handle_event_activated()
 **/
static void handle_event_activated(pscc_object_t *object_p,int cause,pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;
  int netdev_handler;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  if(pscc_connection_status_connecting != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "activated event received in unexpected state\n");
    *sendevent_p = false;
    return;
  }

  /* configure the network interface */
  PSCC_DBG_TRACE(LOG_DEBUG, "configure network interface\n");
  if (pscc_configure_network_interface(object_p->connid) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Failed to configure network interface.\n");
  }

  netdev_handler = pscc_netdev_create(pscc_obj_get_connid(object_p),
                                      pscc_get_param,
                                      pscc_set_param,
                                      pscc_delete_param,
                                      pscc_netdev_destroyed);
  if (netdev_handler < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, " pscc_netdev_create failed, deactivate the pdp context\n");
    /* disconnect bearer */
    if(0 > pscc_bearer_deactivate(pscc_obj_get_bearer_handle(object_p)))
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to start bearer deactivation..what to do??\n");
      return;
    }
    pscc_obj_set_state(object_p,pscc_connection_status_disconnecting);
    *sendevent_p = false;
  }
  else
  {
    pscc_obj_set_netdev_handle(object_p,netdev_handler);
    msg_p->event_connected.id = pscc_event_connected;
    connid = pscc_obj_get_connid(object_p);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_cause, &cause);
    pscc_obj_set_state(object_p,pscc_connection_status_connected);
    *sendevent_p = true;
  }

}

/**
 * handle_event_deactivating()
 **/
static void handle_event_deactivating(pscc_object_t *object_p,pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  switch(pscc_obj_get_state(object_p))
  {
  case pscc_connection_status_modifying:
  case pscc_connection_status_connected:
  case pscc_connection_status_disconnecting:
    msg_p->event_disconnecting.id = pscc_event_disconnecting;
    connid = pscc_obj_get_connid(object_p);
    msg_p->event_disconnecting.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);
    pscc_obj_set_state(object_p,pscc_connection_status_disconnecting);
    *sendevent_p = true;
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "deactivating event received in unexpected state\n");
    *sendevent_p = false;
    break;
  }
}

/**
 * handle_event_deactivated()
 **/
static void handle_event_deactivated(pscc_object_t *object_p,pscc_reason_t  reason, int cause, pscc_msg_t *msg_p,bool *sendevent_p)
{
  pscc_event_disconnected_t *event_p = &msg_p->event_disconnected;
  int connid; int ip_map = 0;
  mpl_param_element_t *param_elem_p;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  switch(pscc_obj_get_state(object_p))
  {
  case pscc_connection_status_modifying:
  case pscc_connection_status_connected:
  case pscc_connection_status_connecting:
  case pscc_connection_status_disconnecting:
    /* the network interface will be reset by the initiator of the deactivation */

    /* Destroy net device */
    if (pscc_netdev_destroy(pscc_obj_get_netdev_handle(object_p)) < 0)
      PSCC_DBG_TRACE(LOG_ERR, "pscc_netdev_destroy failed\n");
    event_p->id = pscc_event_disconnected;
    connid = pscc_obj_get_connid(object_p);
    event_p->param_list_p = NULL;

    /* device reset must be handled in the case on nwi disconnect */
    if (pscc_reason_pdp_context_nw_deactivated == reason
            || pscc_reason_pdp_context_nw_deactivated_reset == reason
            || pscc_reason_pdp_context_ms_deactivated == reason) {
            /* reset network interface */
        PSCC_DBG_TRACE(LOG_DEBUG, "reset network interface\n");
        if (pscc_reset_network_interface(connid) < 0)
        {
          PSCC_DBG_TRACE(LOG_ERR, "Failed to reset network interface.\n");
        }
    }


    (void) mpl_add_param_to_list(&event_p->param_list_p, pscc_paramid_connid, &connid);
    (void) mpl_add_param_to_list(&event_p->param_list_p, pscc_paramid_cause, &cause);
    (void) mpl_add_param_to_list(&event_p->param_list_p, pscc_paramid_reason, &reason);

    /* Get the IP MAP to determine which addresses are to be packed */
    param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_ip_map);
    if (NULL != param_elem_p && NULL != param_elem_p->value_p)
    {
      ip_map = *(int*)(param_elem_p->value_p);
    }
    PSCC_DBG_TRACE(LOG_INFO, "Value of IP_MAP is :%d\n",ip_map);

    /* Add own ip address to the event */
    if ((ip_map & PSCC_IPV4_ADDRESS_PRESENT) == PSCC_IPV4_ADDRESS_PRESENT)
    {
      param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_own_ip_address);
      if (NULL != param_elem_p && NULL != param_elem_p->value_p)
      {
        (void) mpl_add_param_to_list(&event_p->param_list_p, pscc_paramid_own_ip_address, param_elem_p->value_p);
      }
    }

    /* Add own ipv6 address to the event */
    if ((ip_map & PSCC_IPV6_ADDRESS_PRESENT) == PSCC_IPV6_ADDRESS_PRESENT)
    {
      param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_own_ipv6_address);
      if (NULL != param_elem_p && NULL != param_elem_p->value_p)
      {
        (void) mpl_add_param_to_list(&event_p->param_list_p, pscc_paramid_own_ipv6_address, param_elem_p->value_p);
      }
    }

    /* Add pdp type to the event */
    param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_pdp_type);
    if (NULL != param_elem_p && NULL != param_elem_p->value_p)
    {
      (void) mpl_add_param_to_list(&event_p->param_list_p, pscc_paramid_pdp_type, param_elem_p->value_p);
    }

    pscc_obj_set_state(object_p,pscc_connection_status_disconnected);
    *sendevent_p = true;
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "deactivated event received in unexpected state\n");
    *sendevent_p = false;
    return;
    break;
  }
}

/**
 * handle_event_modify_failed()
 **/
static void handle_event_modify_failed(pscc_object_t *object_p, pscc_reason_t reason, int cause, pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  PSCC_IDENTIFIER_NOT_USED(reason);

  switch(pscc_obj_get_state(object_p))
  {
  case pscc_connection_status_modifying:
    msg_p->event_modify_failed.id = pscc_event_modify_failed;
    connid = pscc_obj_get_connid(object_p);
    msg_p->event_modify_failed.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_cause, &cause);

    pscc_obj_set_state(object_p,pscc_connection_status_connected);
    *sendevent_p = true;
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "modify failed event received in unexpected state\n");
    *sendevent_p = false;
    break;
  }
}


/**
 * handle_event_modified()
 **/
static void handle_event_modified(pscc_object_t *object_p, pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  switch(pscc_obj_get_state(object_p))
  {
  case pscc_connection_status_modifying:
    pscc_obj_set_state(object_p,pscc_connection_status_connected);
    /*fallthrough*/
  case pscc_connection_status_connected:
    msg_p->event_modified.id = pscc_event_modified;
    connid = pscc_obj_get_connid(object_p);
    msg_p->event_modified.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);
    *sendevent_p = true;
    break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "modified event received in unexpected state\n");
    *sendevent_p = false;
    break;
  }
}

/**
 * handle_event_data_counters_fetched()
 **/
static void handle_event_data_counters_fetched(pscc_object_t *object_p, pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;
  mpl_param_element_t *param_elem_p;

  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  if (pscc_connection_status_connected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "data counters fetched event received in unexpected state\n");
    *sendevent_p = false;
    return;
  }

  msg_p->event_data_counters_fetched.id = pscc_event_data_counters_fetched;
  connid = pscc_obj_get_connid(object_p);
  (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);

  /* Fetch the rx count set by the bearer */
  param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_rx_data_count_lo);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    (void) mpl_add_param_to_list(&msg_p->event_data_counters_fetched.param_list_p, pscc_paramid_rx_data_count_lo, param_elem_p->value_p);
  }
  param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_rx_data_count_hi);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    (void) mpl_add_param_to_list(&msg_p->event_data_counters_fetched.param_list_p, pscc_paramid_rx_data_count_hi, param_elem_p->value_p);
  }
  /* Fetch the tx count set by the bearer */
  param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_tx_data_count_lo);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    (void) mpl_add_param_to_list(&msg_p->event_data_counters_fetched.param_list_p, pscc_paramid_tx_data_count_lo, param_elem_p->value_p);
  }
  param_elem_p = pscc_get_param(object_p->connid, pscc_paramid_tx_data_count_hi);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    (void) mpl_add_param_to_list(&msg_p->event_data_counters_fetched.param_list_p, pscc_paramid_tx_data_count_hi, param_elem_p->value_p);
  }

  *sendevent_p = true;
  return;
}

/**
 * handle_event_data_counters_reset()
 **/
static void handle_event_data_counters_reset(pscc_object_t *object_p, pscc_msg_t *msg_p,bool *sendevent_p)
{
  int connid;
  assert(sendevent_p != NULL);
  assert(object_p != NULL);
  assert(msg_p != NULL);

  if (pscc_connection_status_connected != pscc_obj_get_state(object_p))
  {
    PSCC_DBG_TRACE(LOG_ERR, "data counters reset event received in unexpected state\n");
    *sendevent_p = false;
    return;
  }

  msg_p->event_data_counters_reset.id = pscc_event_data_counters_reset;
  connid = pscc_obj_get_connid(object_p);
  msg_p->event_modify_failed.param_list_p = NULL;
  (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_connid, &connid);
  *sendevent_p = true;
  return;
}

/**
 * handle_event_rc_requested()
 **/
static void handle_event_rc_requested(pscc_rc_t* pscc_rc_p)
{
  pscc_cc_gprs_availability_t pscc_cc_gprs_availability;
  assert(pscc_rc_p != NULL);

  if (pscc_sim_cc_gprs_availability(&pscc_cc_gprs_availability) < 0){
    PSCC_DBG_TRACE(LOG_ERR, "Failed to get sim cc gprs availability\n");
    return;
  }

  /* only perform call control if service is available */
  if (pscc_cc_gprs_availability_available == pscc_cc_gprs_availability)
  {
    if (pscc_sim_perform_cc(pscc_rc_p, pscc_sim_event) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to perform sim call control\n");
    }
  }
  /* if service is unavailable then we tell bearer that it is ok to use this rc */
  else if (pscc_cc_gprs_availability_unavailable == pscc_cc_gprs_availability)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "allow bearer resource control request\n");
    if(pscc_bearer_respond_resource_control(pscc_rc_p, true, false) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to tell bearer to allow resource control\n");
    }
  }
  /* if service is unknown we reject the activation attempt */
  else
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "deny bearer resource control request\n");
    if(pscc_bearer_respond_resource_control(pscc_rc_p, false, false) < 0)
    {
      PSCC_DBG_TRACE(LOG_ERR, "Failed to tell bearer to deny resource control\n");
    }
  }

  return;
}

/**
 * pscc_bearer_attach_event()
 **/
static void pscc_bearer_attach_event(pscc_bearer_attach_event_t event, pscc_attach_event_params_t attach_event_params, int cause, uint32_t ct)
{
  char *eventbuf_p=NULL;
  size_t len=0;
  pscc_msg_t *msg_p;

  msg_p = mpl_event_msg_alloc(PSCC_PARAM_SET_ID);
  if(NULL == msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to alloc memory\n");
    return;
  }

  switch(event)
  {
  case pscc_bearer_attach_event_attached:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_attached ct=%d\n", ct);
    msg_p->event_attached.id = pscc_event_attached;
    msg_p->event_attached.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_attach_failed:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_attach_failed, cause=%d ct=%d\n", cause, ct);
    msg_p->event_attach_failed.id = pscc_event_attach_failed;
    msg_p->event_attach_failed.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->event_attach_failed.param_list_p, pscc_paramid_cause, &cause);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_detached:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_detached ct=%d\n", ct);
    msg_p->event_detached.id = pscc_event_detached;
    msg_p->event_detached.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
    /* Treat network initiated detach the same way as event_detached except but passing the cause */
  case pscc_bearer_attach_event_nwi_detached:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_nwi_detached, cause=%d ct=%d\n", cause, ct);
    msg_p->event_detached.id = pscc_event_detached;
    msg_p->event_detached.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->event_detach_failed.param_list_p, pscc_paramid_cause, &cause);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_detach_failed:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_detach_failed, cause=%d ct=%d\n", cause, ct);
    msg_p->event_detach_failed.id = pscc_event_detach_failed;
    msg_p->event_detach_failed.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->event_detach_failed.param_list_p, pscc_paramid_cause, &cause);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_mode_queried:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_mode_queried, mode=%d, ct=%d\n", attach_event_params.attach_mode, ct);
    msg_p->event_attached.id = pscc_event_attach_mode_queried;
    msg_p->event_attached.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->event_attach_mode_queried.param_list_p, pscc_paramid_attach_mode, &(attach_event_params.attach_mode));
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_mode_configured:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_mode_configured ct=%d\n", ct);
    msg_p->event_attached.id = pscc_event_attach_mode_configured;
    msg_p->event_attached.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_mode_configure_failed:
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_mode_configure_failed, cause=%d  ct=%d\n", cause, ct);
    msg_p->event_attached.id = pscc_event_attach_mode_configure_failed;
    msg_p->event_attached.param_list_p = NULL;
    (void) mpl_add_param_to_list(&msg_p->event_attach_mode_configure_failed.param_list_p, pscc_paramid_cause, &cause);
    (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
    break;
  case pscc_bearer_attach_event_attach_status:
      PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_attach_event_attach_status ct=%d\n", ct);
      if(attach_event_params.attach_status == pscc_attach_status_attached){
          msg_p->event_attached.id = pscc_event_attached;
      } else {
          msg_p->event_attached.id = pscc_event_detached;
      }
      msg_p->event_attached.param_list_p = NULL;
      (void) mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
      break;
  default:
    PSCC_DBG_TRACE(LOG_ERR, "unknown attach event %d, ct=%d\n", event, ct);
    goto error_return;
  }

  if (0 > mpl_msg_pack((mpl_msg_t*) msg_p, &eventbuf_p, &len))
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to pack event message\n");
    goto error_return;
  }

  PSCC_DBG_TRACE(LOG_INFO, "Sending event: %s\n", eventbuf_p);
  (void)psccd_send_event(eventbuf_p,len);

error_return:
  if(NULL != eventbuf_p)
    free(eventbuf_p);
  mpl_msg_free((mpl_msg_t*)msg_p);
}

/**
 * pscc_bearer_ping_block_mode_event()
 **/
static void pscc_bearer_ping_block_mode_event(pscc_bearer_ping_block_mode_event_t event, pscc_ping_block_mode_t ping_mode, uint32_t ct)
{
  char*  eventbuf_p = NULL;
  size_t len        = 0;
  pscc_msg_t* msg_p = mpl_event_msg_alloc(PSCC_PARAM_SET_ID);
  if(!msg_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to alloc memory\n");
    return;
  }

  switch(event)
  {
    case pscc_bearer_ping_block_mode_queried_event:
      PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_ping_block_mode_queried_event, mode=%d, ct=%d\n", ping_mode, ct);
      msg_p->event_attached.id = pscc_event_ping_block_mode_queried;
      msg_p->event_attached.param_list_p = NULL;
      (void)mpl_add_param_to_list(&msg_p->event_ping_block_mode_queried.param_list_p,
                                  pscc_paramid_ping_block_mode, &ping_mode);
      (void)mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
      break;

     case pscc_bearer_ping_block_mode_configured_event:
       PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_ping_block_mode_configured_event, ct=%d\n", ct);
       msg_p->event_attached.id = pscc_event_ping_block_mode_configured;
       msg_p->event_attached.param_list_p = NULL;
       (void)mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
       break;

     case pscc_bearer_ping_block_mode_configure_failed_event:
       PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_ping_block_mode_configure_failed_event, ct=%d\n", ct);
       msg_p->event_attached.id = pscc_event_ping_block_mode_configure_failed;
       msg_p->event_attached.param_list_p = NULL;
       (void)mpl_add_param_to_list(&msg_p->common.param_list_p, pscc_paramid_ct, &ct);
       break;

     default:
       PSCC_DBG_TRACE(LOG_ERR, "unknown filter icmp ping mode event %d ct=%d\n", event, ct);
       goto error_return;
  }

  if (mpl_msg_pack((mpl_msg_t*) msg_p, &eventbuf_p, &len) >= 0)
  {
    PSCC_DBG_TRACE(LOG_INFO, "Sending event: %s\n", eventbuf_p);
    (void)psccd_send_event(eventbuf_p,len);
  } else {
    PSCC_DBG_TRACE(LOG_ERR, "failed to pack event message\n");
  }

error_return:
  if(eventbuf_p) {
    free(eventbuf_p);
  }
  mpl_msg_free((mpl_msg_t*)msg_p);
}

/**
 * pscc_bearer_rc_event()
 **/
static void pscc_bearer_rc_event(pscc_bearer_rc_event_t event, pscc_rc_t* pscc_rc_p)
{
  switch(event)
  {
  case pscc_bearer_rc_event_requested:
    assert(NULL != pscc_rc_p);
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_rc_event_requested, seq_id:%d\n", pscc_rc_p->seq_id);
    handle_event_rc_requested(pscc_rc_p);
    break;
  case pscc_bearer_rc_event_done:
    /* the user is not interested in resource control so we ignore it */
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_rc_event_done\n");
    break;
  case pscc_bearer_rc_event_failed:
    /* modem will send activation failed event if this happens */
    PSCC_DBG_TRACE(LOG_INFO, "pscc_bearer_rc_event_failed\n");
    break;

  default:
    PSCC_DBG_TRACE(LOG_ERR, "unknown resource control event:%d\n",event);
  }
  return;
}

/**
 * pscc_set_param()
 **/
static int pscc_set_param(int connid, mpl_param_element_t *param)
{
  pscc_object_t *object_p;
  mpl_list_t **param_list_pp;

  assert(param != NULL);

  /* find parent object */
  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "object with id %d not found\n",connid);
    return (-1);
  }

  param_list_pp = pscc_obj_get_param_list_ref(object_p);
  mpl_list_add(param_list_pp, &param->list_entry);
  return (0);
}

/**
 * pscc_delete_param()
 **/
static void pscc_delete_param(int connid, pscc_paramid_t id)
{
  pscc_object_t *object_p;
  mpl_list_t **param_list_pp;
  mpl_param_element_t *param_p;

  /* find parent object */
  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id.
       Could be because the handler is already deleted*/
    PSCC_DBG_TRACE(LOG_DEBUG, "object with id %d not found\n",connid);
    return;
  }

  param_list_pp = pscc_obj_get_param_list_ref(object_p);

  while(NULL != (param_p = mpl_param_list_find(id, *param_list_pp)))
  {
    (void)mpl_list_remove(param_list_pp, &param_p->list_entry);
    mpl_param_element_destroy(param_p);
  }
}


static void pscc_netdev_destroyed(int connid)
{
  pscc_object_t *object_p;

  /* find parent object */
  object_p = pscc_obj_get(connid);

  if(NULL == object_p)
  {
    /* a PS connection object does not exist for this conn id */
    PSCC_DBG_TRACE(LOG_ERR, "object with id %d not found\n",connid);
    return;
  }

  //we only expect this while connected
  if (pscc_obj_get_state(object_p) != pscc_connection_status_connected)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Received pscc_netdev_destroyed in an unexpected state\n",connid);
    return;
  }

  /* disconnect bearer */
  PSCC_DBG_TRACE(LOG_DEBUG, "deactivate bearer\n");
  if(pscc_bearer_deactivate(pscc_obj_get_bearer_handle(object_p)) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "Failed to start bearer deactivation\n");
    return;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "disconnecting\n");
  pscc_obj_set_state(object_p,pscc_connection_status_disconnecting);
}

/**
 * pscc_configure_network_interface()
 **/
static int pscc_configure_network_interface(int connid)
{
  char* ipaddress = NULL;
  char* ipv6address = NULL;
  char* nwifname;
  pscc_pdp_type_t pdp_type;
  uint32_t txqueuelen;
  uint32_t mtu;
  mpl_param_element_t *param_elem_p;
  int ip_map = 0;

  /* fetch pdp type */
  param_elem_p = pscc_get_param(connid, pscc_paramid_pdp_type);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    pdp_type = *(pscc_pdp_type_t*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter pdp_type.\n");
    return -1;
  }

  /* fetch ip map*/
  param_elem_p = pscc_get_param(connid, pscc_paramid_ip_map);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    ip_map = *(int*)param_elem_p->value_p;
  }
  PSCC_DBG_TRACE(LOG_INFO, "Value of IP_MAP is :%d\n",ip_map);

  /* fetch ipaddress */
  if ((ip_map & PSCC_IPV4_ADDRESS_PRESENT) == PSCC_IPV4_ADDRESS_PRESENT)
  {
    param_elem_p = pscc_get_param(connid, pscc_paramid_own_ip_address);
    if (NULL != param_elem_p && NULL != param_elem_p->value_p)
    {
      ipaddress = (char*) param_elem_p->value_p;
    }
    else
    {
      PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter ipaddress.\n");
      return -1;
    }
  }

  /* fetch ipv6address */
  if ((ip_map & PSCC_IPV6_ADDRESS_PRESENT) == PSCC_IPV6_ADDRESS_PRESENT)
  {
    param_elem_p = pscc_get_param(connid, pscc_paramid_own_ipv6_address);
    if (NULL != param_elem_p && NULL != param_elem_p->value_p)
    {
      ipv6address = (char*) param_elem_p->value_p;
    }
    else
    {
      PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter ipv6address.\n");
      return -1;
    }
  }

  /* fetch network interface name */
  param_elem_p = pscc_get_param(connid, pscc_paramid_netdev_name);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    nwifname = (char*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter netdev_name.\n");
    return -1;
  }

  /* fetch txqueuelen */
  param_elem_p = pscc_get_param(connid, pscc_paramid_nwif_txqueuelen);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    txqueuelen = *(pscc_pdp_type_t*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter txqueuelen.\n");
    return -1;
  }

  /* fetch mtu */
  param_elem_p = pscc_get_param(connid, pscc_paramid_nwif_mtu);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    mtu = *(pscc_pdp_type_t*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter txqueuelen.\n");
    return -1;
  }

  PSCC_DBG_TRACE(LOG_INFO, "Configuring network interface %s with ip %s.\n", nwifname, ipaddress);

  /* open nwifcfg socket */
  if (pscc_nwifcfg_open(pdp_type) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed to open nwifcfg socket.\n");
    return -1;
  }

  /* take down interface */
  if (pscc_nwifcfg_if_down(nwifname) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed to take down network interface %s.\n", nwifname);
    goto error;
  }

  if (pscc_configure_network_interface_name(connid) >= 0) {
    /* we need to update nwifname */
    param_elem_p = pscc_get_param(connid, pscc_paramid_netdev_name);
    if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
      nwifname = (char*) param_elem_p->value_p;
    }
    else
    {
      PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter netdev_name.\n");
      return -1;
    }
  }

  /* assign ipv4 address to interface */
  if (ipaddress != NULL)
  {
    if (pscc_nwifcfg_assign_ip(nwifname, ipaddress, pscc_pdp_type_ipv4) < 0)
    {
      PSCC_DBG_TRACE(LOG_WARNING, "failed to assign ipv4 address %s to network interface %s.\n", ipaddress, nwifname);
      goto error;
    }
  }

  /* assign ipv6 address to interface */
  if (ipv6address != NULL)
  {
    if (pscc_nwifcfg_assign_ip(nwifname, ipv6address, pscc_pdp_type_ipv6) < 0)
    {
      PSCC_DBG_TRACE(LOG_WARNING, "failed to assign ipv6 address %s to network interface %s.\n", ipv6address, nwifname);
      goto error;
    }
  }

  /* take up interface */
  if (pscc_nwifcfg_if_up(nwifname) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed to take up network interface %s.\n", nwifname);
    goto error;
  }

  /* set default route */
  if (pscc_nwifcfg_set_default_route(nwifname) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed set default route for %s.\n", nwifname);
    goto error;
  }

  /* set txqueue length */
  if (pscc_nwifcfg_set_txqueuelen(nwifname, txqueuelen) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed set txqueuelen for %s.\n", nwifname);
    goto error;
  }

  /* set mtu */
  if (pscc_nwifcfg_set_mtu(nwifname, mtu) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed set mtu for %s.\n", nwifname);
    goto error;
  }

  /* close nwifcfg socket */
  if (pscc_nwifcfg_close() < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to close nwifcfg socket.\n");
    return -1;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "Successfully configured.\n");

  return 0;

error:
  /* try to close socket to avoid memory leak */
  if (pscc_nwifcfg_close() < 0)
  {
     PSCC_DBG_TRACE(LOG_ERR, "failed to close nwifcfg socket.\n");
  }
  return -1;

}

/**
 * pscc_reset_network_interface()
 **/
static int pscc_reset_network_interface(int connid)
{
  char* nwifname;
  pscc_pdp_type_t pdp_type;
  mpl_param_element_t *param_elem_p;

  /* fetch network interface name */
  param_elem_p = pscc_get_param(connid, pscc_paramid_netdev_name);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    nwifname = (char*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter netdev_name.\n");
    return -1;
  }

  /* fetch pdp type */
  param_elem_p = pscc_get_param(connid, pscc_paramid_pdp_type);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p)
  {
    pdp_type = *(pscc_pdp_type_t*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter pdp_type.\n");
    return -1;
  }

  PSCC_DBG_TRACE(LOG_INFO, "Taking down network interface %s.\n", nwifname);

  /* open nwifcfg socket */
  if (pscc_nwifcfg_open(pdp_type) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed to open nwifcfg socket.\n");
    return -1;
  }

  /* reset connections */
  if (pscc_nwifcfg_reset_connections(nwifname) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed to reset connections.\n");
  }

  /* take down interface */
  if (pscc_nwifcfg_if_down(nwifname) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "failed to take down network interface %s.\n", nwifname);
    /* try to close socket to avoid memory leak */
    if (pscc_nwifcfg_close() < 0)
    {
       PSCC_DBG_TRACE(LOG_ERR, "failed to close nwifcfg socket.\n");
    }
    return -1;
  }

  /* close nwifcfg socket */
  if (pscc_nwifcfg_close() < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to close nwifcfg socket.\n");
    return -1;
  }

  PSCC_DBG_TRACE(LOG_DEBUG, "Successfully reset.\n");

  return 0;
}

/**
 * pscc_configure_network_interface_name()
 **/
static int pscc_configure_network_interface_name(int connid)
{
  char* nwifprefix;
  char* nwifname;
  char* numbers = "1234567890";
  char* new_nwifname_p = NULL;
  mpl_param_element_t *param_elem_p;
  int i = 0;

  /* fetch network interface name */
  param_elem_p = pscc_get_param(connid, pscc_paramid_netdev_name);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
    nwifname = (char*) param_elem_p->value_p;
  }
  else
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter netdev_name.\n");
    goto error;
  }

  /* fetch network interface prefix */
  param_elem_p = pscc_get_param(connid, pscc_paramid_netdev_name_prefix);
  if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
    nwifprefix = (char*) param_elem_p->value_p;
  }
  else {
    PSCC_DBG_TRACE(LOG_ERR, "could not fetch parameter netdev_prefix.\n");
    goto error;
  }

  if (0 == strlen(nwifprefix)) {
    goto error;
  }

  i = strcspn(nwifname, numbers);

  /* check if prefix is same as current */
  if (strncmp(nwifname, nwifprefix, i) == 0) {
    PSCC_DBG_TRACE(LOG_INFO, "New prefix is the same as current.\n");
    return 0;
  }

  /* try to rename network if */
  new_nwifname_p = pscc_rename_network_interface(nwifname, nwifprefix, atoi(&nwifname[i]));
  if (new_nwifname_p == NULL) {
    goto error;
  }

  PSCC_DBG_TRACE(LOG_INFO, "Renamed network interface from %s to %s.\n", nwifname, new_nwifname_p);

  /* update network interface name */
  pscc_delete_param(connid, pscc_paramid_netdev_name);

  param_elem_p = mpl_param_element_create_stringn(pscc_paramid_netdev_name, new_nwifname_p, strlen(new_nwifname_p));
  if (NULL == param_elem_p) {
    PSCC_DBG_TRACE(LOG_ERR, "failed to create parameter netdev_name address\n");
    goto error;
  }

  if (pscc_set_param(connid, param_elem_p) < 0) {
    PSCC_DBG_TRACE(LOG_ERR, "not able to store netdev_name parameter\n");
    mpl_param_element_destroy(param_elem_p);
    goto error;
  }

  free(new_nwifname_p);
  return 0;

error:
  free(new_nwifname_p);
  PSCC_DBG_TRACE(LOG_WARNING, "failed to rename network interface.\n");
  return -1;
}

/**
 * pscc_rename_network_interface()
 **/
static char *pscc_rename_network_interface(char *if_name, char *if_prefix, int if_index)
{
  int i = 0;
  char* new_if_name_p = NULL;

  if ((if_index < 0) || (if_index > 9)) {
     PSCC_DBG_TRACE(LOG_ERR, "Unvalid network interface index: %d.\n", if_index);
     return NULL;
 }

  /* alloc string */
  new_if_name_p = calloc(strlen(if_prefix) +2, sizeof(char));
  if (new_if_name_p == NULL) {
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return NULL;
  }

  for (i=0; i<10; i++)
  {
    sprintf(new_if_name_p, "%s%d", if_prefix, if_index);

    if (pscc_nwifcfg_rename_nwif(if_name, new_if_name_p) == 0) {
      return new_if_name_p;
    }

    if_index = (if_index==9) ? 0 : if_index+1;
  }

  free(new_if_name_p);
  return NULL;
}
