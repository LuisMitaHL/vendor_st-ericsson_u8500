/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <assert.h>
#include <errno.h>
#include "sterc_msg.h"
#include "stercd.h"
#include "mpl_config.h"
#include "sterc_pscc.h"
#include "sterc_if.h"
#include "sterc_if_ppp.h"
#include "sterc_runscript.h"
#include "libstecom.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/un.h>
/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define CID_PRESENT_IN_LIST(param_list_p) MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_cid, (param_list_p))
#define GET_CID_FROM_LIST(param_list_p) MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_cid,(param_list_p))
#define PPPD_PID_PRESENT_IN_LIST(param_list_p) MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_pppd_pid, (param_list_p))
#define GET_PPPD_PID_FROM_LIST(param_list_p) MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_pppd_pid,(param_list_p))
#define HANDLE_PRESENT_IN_LIST(param_list_p) MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_handle, (param_list_p))
#define GET_HANDLE_FROM_LIST(param_list_p) MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_handle,(param_list_p))


#define sterc_msg_id_get_string(msg_id) \
  mpl_param_value_get_string(sterc_paramid_message, &(msg_id))

#define sterc_get_param_list_ref(self) &((self)->sterc_params_p)

/**
 * please note that the debug trace in this macro is
 * used in the test scripts. If the printout is changed
 * please update the test pattern in the unix.exp file
**/
#define sterc_handler_change_state(obj_p,new_state)                     \
  do                                                                    \
  {                                                                     \
    assert((new_state) < sterc_handler_number_of_states);               \
    assert((obj_p)->state < sterc_handler_number_of_states);            \
    assert((obj_p) != NULL);                                            \
    STERC_DBG_TRACE(LOG_INFO, "sterc state change: %s->%s\n",           \
                    sterc_handler_state_names[(obj_p)->state],          \
                    sterc_handler_state_names[(new_state)]);            \
    (obj_p)->state = new_state;                                         \
    (void)sterc_runscript_call((obj_p)->handle, &sterc_get_param, new_state); \
  } while(0)

typedef struct
{
  int handle;
  int sterc_pscc_handle;
  int lan_if_handle;
  sterc_mode_t mode;
  sterc_if_ctrl_block_t lan_if_funcs;
  sterc_handler_state_t state;
  bool lan_connected;
  bool wan_connected;
  mpl_list_t *sterc_params_p;
  mpl_list_t list_entry;
  sterc_reason_t reason;
}sterc_handler_t;


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static mpl_list_t *sterc_handler_list_p = NULL;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static sterc_handler_t* sterc_handler_alloc(void);
static void sterc_handler_free(sterc_handler_t *obj_p);
static mpl_msg_t *handle_req_create(mpl_msg_t *req_create_p);
static mpl_msg_t *handle_req_destroy(mpl_msg_t *req_destroy_p);
static mpl_msg_t *handle_req_set(mpl_msg_t *req_set_p);
static mpl_msg_t *handle_req_get(mpl_msg_t *req_set_p);

static sterc_handler_t* sterc_handler_find(sterc_paramid_t param_id, void* value);

static void sterc_delete_param(int handle, sterc_paramid_t parmid);
static int sterc_set_param(int handle, mpl_param_element_t *param);
static mpl_param_element_t* sterc_get_param(int handle,sterc_paramid_t paramid, char* key);

static void sterc_if_connected(int handle);
static void sterc_if_disconnected(int handle);
static void sterc_pscc_connected(int handle);
static void sterc_pscc_disconnected(int handle);

static void sterc_handler_disconnect(sterc_handler_t *obj_p);
static void sterc_handler_destroy(sterc_handler_t *obj_p);

static void sterc_send_disconnect_event(sterc_handler_t *obj_p);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * sterc_handler()
 **/
int sterc_handler(struct stec_ctrl *ctrl, char *req_p, size_t reqlen, char *resp_p, size_t * resplen_p)
{
  mpl_msg_t *msg_p;
  mpl_msg_t *resp_msg_p = NULL;

  assert(ctrl != NULL);
  assert(req_p != NULL);
  assert(resp_p != NULL);
  assert(resplen_p != NULL);

  STERC_DBG_TRACE(LOG_DEBUG, "Received request message: %s\n", req_p);

  msg_p = mpl_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == msg_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed allocating msg_p ...\n");
    goto error_send_response_and_return;
  }

  if(0 > mpl_msg_unpack(req_p, reqlen, (mpl_msg_t*) msg_p))
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed to unpack message: %s\n", req_p);
    mpl_msg_free(msg_p);
    goto error_send_response_and_return;
  }

  //PPP handle function
  if (sterc_ppp_handler(ctrl, msg_p, &resp_msg_p) > 0) {
    if (NULL == resp_msg_p)
      *resplen_p = 0;

    //mesage handled
    mpl_msg_free(msg_p);
    return 0;
  }

  if (sterc_msgtype_req == msg_p->common.type)
  {
    switch(msg_p->common.id)
    {
    case sterc_create:
      resp_msg_p = handle_req_create(msg_p);
      break;
    case sterc_destroy:
      resp_msg_p = handle_req_destroy(msg_p);
      break;
    case sterc_get:
      resp_msg_p = handle_req_get(msg_p);
      break;
    case sterc_set:
      resp_msg_p = handle_req_set(msg_p);

      break;
    default:
      STERC_DBG_TRACE(LOG_ERR, "Unsupported request: %s\n",
                   sterc_msg_id_get_string(msg_p->common.id));
      break;
    }

    if (NULL == resp_msg_p)
    {
      //Something went wrong. Send error
      resp_msg_p = msg_p; //Use the received msg
      resp_msg_p->common.type = sterc_msgtype_resp;
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
    }


    //Pack the message
    if (NULL != resp_msg_p)
    {
      if(0 > mpl_msg_pack((mpl_msg_t*) resp_msg_p, &resp_p, resplen_p))
      {
        STERC_DBG_TRACE(LOG_ERR, "Failed packing response message: %s\n",
                        sterc_msg_id_get_string(resp_msg_p->common.id));
        mpl_msg_free((mpl_msg_t*) resp_msg_p);
        resp_msg_p = NULL;
        mpl_msg_free(msg_p);
        goto error_send_response_and_return;
      }
    }
  }
  else
  {
    STERC_DBG_TRACE(LOG_ERR, "Message not supported");
    mpl_msg_free(msg_p);
    goto error_send_response_and_return;
  }


  mpl_msg_free((mpl_msg_t*) resp_msg_p);
  resp_msg_p = NULL;
  mpl_msg_free(msg_p);
  return 0;

  error_send_response_and_return:
  if (NULL == resp_msg_p)
  {
    int len;

    len = snprintf(resp_p, *resplen_p, "%s", sterc_event_failure_buf);

    if ((len < 0) || (len >= (int)*resplen_p))
      return (-1);
    else
      *resplen_p = len;
  }

  return 0;
}

/**
 * sterc_handler_connect_wan()
 **/
void sterc_handler_connect_wan(int handle)
{
  sterc_handler_t *obj_p;

  //Find object
  obj_p = sterc_handler_find(sterc_paramid_handle, &handle);
  if (NULL == obj_p) {
    STERC_DBG_TRACE(LOG_ERR, "Sterc object not found\n");
    return;
  }

  //Set state to connecting
  if (obj_p->state == sterc_handler_created) {
    sterc_handler_change_state(obj_p,sterc_handler_connecting);
  }

  //Start pscc
  if (sterc_pscc_connect(obj_p->sterc_pscc_handle,sterc_pscc_connected,sterc_pscc_disconnected) < 0 ) {
    STERC_DBG_TRACE(LOG_ERR, "Failed to connect PSCC\n");
    sterc_handler_disconnect(obj_p);
    return;
  }
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * sterc_handler_alloc - allocate a sterc handler object
 **/
static sterc_handler_t* sterc_handler_alloc(void)
{
  static int sterc_handler_identifier = 0;
  sterc_handler_t *obj_p = malloc(sizeof(sterc_handler_t));

  if(NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return (NULL);
  }
  memset(obj_p,0,sizeof(sterc_handler_t));
  obj_p->handle = ++sterc_handler_identifier;
  mpl_list_add(&sterc_handler_list_p,&obj_p->list_entry);
  sterc_handler_change_state(obj_p,sterc_handler_idle);
  STERC_DBG_TRACE(LOG_DEBUG, "sterc handler allocated\n");
  return (obj_p);
}


/**
 * sterc_handler_free
 **/
static void sterc_handler_free(sterc_handler_t *obj_p)
{
  assert(obj_p != NULL);
  assert(obj_p->state == sterc_handler_idle);

  //Free parameters
  mpl_param_list_destroy(sterc_get_param_list_ref(obj_p));

  mpl_list_remove(&sterc_handler_list_p,&obj_p->list_entry);
  free(obj_p);
  STERC_DBG_TRACE(LOG_DEBUG, "sterc handler freed\n");
  return;
}

/**
 * sterc_handler_disconnect
 **/
static void sterc_handler_disconnect(sterc_handler_t *obj_p)
{
  assert(obj_p != NULL);

  if (sterc_handler_created == obj_p->state) {
    //We are already disconnected
    STERC_DBG_TRACE(LOG_INFO, "handler disconnect already initiated, returning..");
    return;
  }

  sterc_handler_change_state(obj_p,sterc_handler_disconnecting);

  if (NULL != obj_p->lan_if_funcs.disconnect) {
    obj_p->lan_if_funcs.disconnect(obj_p->lan_if_handle);
  }

  sterc_pscc_disconnect(obj_p->sterc_pscc_handle);

  if (obj_p->lan_connected == true) {
    obj_p->reason = sterc_reason_disconnected_by_nw;
  }
  else {
    obj_p->reason = sterc_reason_disconnected_by_dce;
  }
}


/**
 * sterc_handler_destroy
 **/
static void sterc_handler_destroy(sterc_handler_t *obj_p)
{
  assert(obj_p != NULL);

  if (sterc_handler_created != obj_p->state) {
    //Call disconnect first
    sterc_handler_disconnect(obj_p);
  }

  if (NULL != obj_p->lan_if_funcs.destroy) {
    obj_p->lan_if_funcs.destroy(obj_p->lan_if_handle);
  }

  sterc_pscc_destroy(obj_p->sterc_pscc_handle);

  obj_p->lan_if_handle = 0;
  obj_p->sterc_pscc_handle = 0;
  obj_p->lan_connected = false;
  obj_p->wan_connected = false;

  sterc_handler_change_state(obj_p,sterc_handler_idle);
}


/**
 * handle_req_create()
 **/
static mpl_msg_t *handle_req_create(mpl_msg_t *req_create_p)
{
  sterc_handler_t *obj_p;
  mpl_msg_t *resp_msg_p;
  mpl_list_t**param_list_pp;
  mpl_list_t *list_p,*tmp_p;
  mpl_param_element_t *param_elem_p;
  sterc_mode_t mode;

  assert(req_create_p != NULL);
  assert(req_create_p->req.type == mpl_msg_type_req);
  assert(req_create_p->req.id == sterc_create);

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed allocating memory for response message\n");
    return NULL;
  }

  resp_msg_p->resp.id = req_create_p->req.id;
  resp_msg_p->resp.result = sterc_result_ok;

  if (STERC_CT_PRESENT(req_create_p))
  {
    uint32_t ct;

    ct = STERC_GET_CT(req_create_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, sterc_paramid_ct, &ct) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
      return resp_msg_p;
    }
  }

  //Must have the mode, store it in the paramlist
  if (!MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_mode,req_create_p->req.param_list_p))
  {
    STERC_DBG_TRACE(LOG_ERR, "MODE not present in list\n");
    resp_msg_p->resp.result = sterc_result_failed_parameter_not_found;
    return resp_msg_p;
  }

  mode =  MPL_GET_PARAM_VALUE_FROM_LIST(sterc_mode_t,sterc_paramid_mode,req_create_p->req.param_list_p);

  if (sterc_mode_usb_eth_nap == mode) {
    //Must have the wan_device, store it in the paramlist
    if (!MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_wan_device,req_create_p->req.param_list_p))
    {
      STERC_DBG_TRACE(LOG_ERR, "WAN_DEVICE not present in list\n");
      resp_msg_p->resp.result = sterc_result_failed_parameter_not_found;
      return resp_msg_p;
    }

    //Must have the lan_device, store it in the paramlist
    if (!MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_lan_device,req_create_p->req.param_list_p))
    {
      STERC_DBG_TRACE(LOG_ERR, "LAN_DEVICE not present in list\n");
      resp_msg_p->resp.result = sterc_result_failed_parameter_not_found;
      return resp_msg_p;
    }
  }

  //Alles in ordnung.
  obj_p = sterc_handler_alloc();
  obj_p->mode =  mode;

  //Store the complete param list
  param_list_pp = sterc_get_param_list_ref(obj_p);
  MPL_LIST_FOR_EACH_SAFE(req_create_p->req.param_list_p, list_p, tmp_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    /* Move the new parameter from the command */
    if (NULL != param_elem_p->value_p)
    {
      (void)mpl_list_remove(&req_create_p->req.param_list_p, &param_elem_p->list_entry);
      mpl_list_add(param_list_pp, &param_elem_p->list_entry);
    }
  }

  //Create pscc object and lan_if when it's psdun.
  if (sterc_mode_psdun == mode) {
    obj_p->sterc_pscc_handle = sterc_pscc_create(obj_p->handle,
                                                 sterc_get_param,
                                                 sterc_set_param,
                                                 sterc_delete_param);

    if (obj_p->sterc_pscc_handle < 0) {
      STERC_DBG_TRACE(LOG_ERR, "Failed to create sterc_pscc. Aborting\n");
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
      goto error_free_and_return;
    }

    obj_p->lan_if_funcs = sterc_ppp_if;
    obj_p->lan_if_handle = obj_p->lan_if_funcs.create(obj_p->handle,
                                                      sterc_get_param,
                                                      sterc_set_param,
                                                      sterc_delete_param);
    if (obj_p->lan_if_handle < 0) {
      STERC_DBG_TRACE(LOG_ERR, "Failed to create lan if. Aborting\n");
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
      goto error_free_and_return;
    }
  }

  //All ok. Set state to Created
  sterc_handler_change_state(obj_p,sterc_handler_created);

  if (sterc_mode_psdun == mode) {
    if (NULL != obj_p->lan_if_funcs.connect)
    {
      if (obj_p->lan_if_funcs.connect(obj_p->lan_if_handle, sterc_if_connected, sterc_if_disconnected) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "Failed to connect lan if. Aborting\n");
        resp_msg_p->resp.result = sterc_result_failed_unspecified;
        goto error_free_and_return;
      }
    }
  }

  //Then we start to connect the interfaces. Set state to connecting.
  //Note: The state changes triggers actions in runscript.
  //This is why the state is changed immediately to connecting
  sterc_handler_change_state(obj_p,sterc_handler_connecting);

  //Add the handle to the response message
  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, sterc_paramid_handle, &obj_p->handle) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed to add handle to message\n");
    resp_msg_p->resp.result = sterc_result_failed_unspecified;
    goto error_free_and_return;
  }

  if (sterc_mode_usb_eth_nap == mode) {
    // This statechange is only valid when not using the pscc beause
    // we will not statechange later for the connected event
    sterc_handler_change_state(obj_p,sterc_handler_connected);
  }

  return resp_msg_p;

  error_free_and_return:
  sterc_handler_disconnect(obj_p);
  sterc_handler_destroy(obj_p);
  sterc_handler_free(obj_p);
  return resp_msg_p;
}

/**
 * handle_req_destroy()
 **/
static mpl_msg_t *handle_req_destroy(mpl_msg_t *req_destroy_p)
{
  sterc_handler_t *obj_p;
  mpl_msg_t *resp_msg_p;
  int handle;

  assert(req_destroy_p != NULL);
  assert(req_destroy_p->req.type == mpl_msg_type_req);
  assert(req_destroy_p->req.id == sterc_destroy);

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp.id = req_destroy_p->req.id;
  resp_msg_p->resp.result = sterc_result_ok;

  if (STERC_CT_PRESENT(req_destroy_p))
  {
    uint32_t ct;

    ct = STERC_GET_CT(req_destroy_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, sterc_paramid_ct, &ct) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
      return resp_msg_p;
    }
  }

  if (!HANDLE_PRESENT_IN_LIST(req_destroy_p->req.param_list_p))
  {
    STERC_DBG_TRACE(LOG_ERR, "Handle not present in list\n");
    resp_msg_p->resp.result = sterc_result_failed_object_not_found;
    return resp_msg_p;
  }
  handle = GET_HANDLE_FROM_LIST(req_destroy_p->req.param_list_p);

  //Find object
  obj_p = sterc_handler_find(sterc_paramid_handle,&handle);
  if (NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Sterc object not found\n");
    resp_msg_p->resp.result = sterc_result_failed_object_not_found;
    return resp_msg_p;
  }

  sterc_handler_disconnect(obj_p);
  sterc_handler_destroy(obj_p);
  sterc_handler_free(obj_p);

  STERC_DBG_TRACE(LOG_DEBUG, "STERC DESTROY received handle=%d\n",handle);

  //Send response
  return resp_msg_p;
}

/**
 * handle_req_get()
 **/
static mpl_msg_t *handle_req_get(mpl_msg_t *req_get_p)
{
  mpl_msg_t *resp_msg_p;
  sterc_handler_t *obj_p;
  mpl_param_element_t *param_elem_p;
  mpl_param_element_t *old_param_elem_p;
  mpl_list_t**param_list_pp;
  mpl_list_t *list_p;
  int identifier;
  sterc_paramid_t paramid;

  assert(req_get_p != NULL);
  assert(req_get_p->common.type == sterc_msgtype_req);
  assert(req_get_p->req.id == sterc_get);

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp.id = req_get_p->req.id;
  resp_msg_p->resp.result = sterc_result_ok;

  if (STERC_CT_PRESENT(req_get_p))
  {
    uint32_t ct;

    ct = STERC_GET_CT(req_get_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, sterc_paramid_ct, &ct) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
      return resp_msg_p;
    }
  }

  //Could be pppd_pid or handle that identifies the object
  if (HANDLE_PRESENT_IN_LIST(req_get_p->req.param_list_p)) {
    identifier = GET_HANDLE_FROM_LIST(req_get_p->req.param_list_p);
    paramid = sterc_paramid_handle;
  }
  else if (PPPD_PID_PRESENT_IN_LIST(req_get_p->req.param_list_p)) {
    identifier = GET_PPPD_PID_FROM_LIST(req_get_p->req.param_list_p);
    paramid = sterc_paramid_pppd_pid;
  }
  else {
    STERC_DBG_TRACE(LOG_ERR, "Handle or PPP Pid not in request message\n");
    resp_msg_p->resp.result = sterc_result_failed_object_not_found;
    goto cleanup_and_return;
  }


  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, paramid, &identifier) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed to add handle/ppp pid to message\n");
    resp_msg_p->resp.result = sterc_result_failed_unspecified;
    goto cleanup_and_return;
  }


  //Find object
  obj_p = sterc_handler_find(paramid,&identifier);
  if (NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Sterc object not found\n");
    resp_msg_p->resp.result = sterc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  param_list_pp = sterc_get_param_list_ref(obj_p);

  MPL_LIST_FOR_EACH(req_get_p->req.param_list_p, list_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (paramid == (sterc_paramid_t) param_elem_p->id)
      continue;

    /* CT is in the parameter list but it should be ignored here since we handle that in the response above */
    if (sterc_paramid_ct == param_elem_p->id)
      continue;

    /* Check if the parameter is allowed to get */
    if (!mpl_param_allow_get(param_elem_p->id))
    {
      STERC_DBG_TRACE(LOG_ERR, "Trying to get ungettable parameter: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      resp_msg_p->resp.result = sterc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }

    /* Somewhat strange, but we allow it ... */
    if (NULL != param_elem_p->value_p)
    {
      STERC_DBG_TRACE(LOG_WARNING, "Parameter contains value in get command: %s (ignoring)\n",
                     mpl_param_id_get_string(param_elem_p->id));
    }

    /* Check if the parameter exists */
    old_param_elem_p = mpl_param_list_find(param_elem_p->id, *param_list_pp);
    if (NULL != old_param_elem_p)
    {
      STERC_DBG_TRACE(LOG_DEBUG, "parameter exist: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      do
      {
        /* ...yes copy all into the response parameter list */
        param_elem_p = mpl_param_element_clone(old_param_elem_p);
        if (NULL == param_elem_p)
        {
          STERC_DBG_TRACE(LOG_ERR, "Failed cloning parameter: %s\n",
                         mpl_param_id_get_string(old_param_elem_p->id));
          resp_msg_p->resp.result = sterc_result_failed_unspecified;
          goto cleanup_and_return;
        }
        mpl_list_add(&resp_msg_p->resp.param_list_p, &param_elem_p->list_entry);
      } while(NULL != (old_param_elem_p = mpl_param_list_find_next(param_elem_p->id, old_param_elem_p)));
    }
  }

  cleanup_and_return:
  return (resp_msg_p);

}

/**
 * handle_req_set()
 **/
static mpl_msg_t *handle_req_set(mpl_msg_t *req_set_p)
{
  sterc_handler_t *obj_p;
  mpl_msg_t *resp_msg_p;
  mpl_list_t**param_list_pp;
  mpl_list_t *list_p,*tmp_p;
  mpl_param_element_t *param_elem_p;
  mpl_param_element_t *old_param_elem_p;
  int identifier;
  sterc_paramid_t paramid;

  assert(req_set_p != NULL);
  assert(req_set_p->req.type == mpl_msg_type_req);
  assert(req_set_p->req.id == sterc_set);

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    return NULL;
  }

  resp_msg_p->resp.id = req_set_p->req.id;
  resp_msg_p->resp.result = sterc_result_ok;

  if (STERC_CT_PRESENT(req_set_p))
  {
    uint32_t ct;

    ct = STERC_GET_CT(req_set_p);
    if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, sterc_paramid_ct, &ct) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR, "Failed to add client tag to message\n");
      resp_msg_p->resp.result = sterc_result_failed_unspecified;
      return resp_msg_p;
    }
  }

  //Could be pppd_pid or handle that identifies the object
  if (HANDLE_PRESENT_IN_LIST(req_set_p->req.param_list_p)) {
    identifier = GET_HANDLE_FROM_LIST(req_set_p->req.param_list_p);
    paramid = sterc_paramid_handle;
  }
  else if (PPPD_PID_PRESENT_IN_LIST(req_set_p->req.param_list_p)) {
    identifier = GET_PPPD_PID_FROM_LIST(req_set_p->req.param_list_p);
    paramid = sterc_paramid_pppd_pid;
  }
  else {
    STERC_DBG_TRACE(LOG_ERR, "Handle or PPP Pid not in request message\n");
    resp_msg_p->resp.result = sterc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  if (mpl_add_param_to_list(&resp_msg_p->common.param_list_p, paramid, &identifier) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed to add handle/ppp pid to message\n");
    resp_msg_p->resp.result = sterc_result_failed_unspecified;
    goto cleanup_and_return;
  }

  //Find object
  obj_p = sterc_handler_find(paramid,&identifier);
  if (NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Sterc object not found\n");
    resp_msg_p->resp.result = sterc_result_failed_object_not_found;
    goto cleanup_and_return;
  }

  param_list_pp = sterc_get_param_list_ref(obj_p);

  /* First loop through all parameters and check if they are OK */
  MPL_LIST_FOR_EACH(req_set_p->req.param_list_p, list_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (paramid == (sterc_paramid_t) param_elem_p->id)
      continue;

    /* CT is in the parameter list but it should be ignored here since we handle that in the response above */
    if (sterc_paramid_ct == param_elem_p->id)
      continue;

    /* Check if the parameter is allowed to set */
    if (!mpl_param_allow_set(param_elem_p->id))
    {
      STERC_DBG_TRACE(LOG_ERR,
                     "Trying to set unsettable parameter: %s\n",
                     mpl_param_id_get_string(param_elem_p->id));
      resp_msg_p->resp.result = sterc_result_failed_operation_not_allowed;
      goto cleanup_and_return;
    }
    STERC_DBG_TRACE(LOG_DEBUG, "STERC parameter set %s\n",mpl_param_id_get_string(param_elem_p->id) );
  }

  /* ... then set them all on the object parameter list */
  MPL_LIST_FOR_EACH_SAFE(req_set_p->req.param_list_p, list_p, tmp_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (paramid == (sterc_paramid_t) param_elem_p->id)
      continue;

    /* CT is in the parameter list but it should be ignored here since we handle that in the response above */
    if (sterc_paramid_ct == param_elem_p->id)
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
      (void)mpl_list_remove(&req_set_p->req.param_list_p, &param_elem_p->list_entry);
      mpl_list_add(param_list_pp, &param_elem_p->list_entry);
    }
  }

  cleanup_and_return:
  return (resp_msg_p);
}



/**
 *  sterc_context_find()
 **/
static sterc_handler_t* sterc_handler_find(sterc_paramid_t param_id, void* value)
{
  mpl_list_t *obj_p;
  sterc_handler_t *sterc_handler_p;

  assert(value != NULL);

  MPL_LIST_FOR_EACH(sterc_handler_list_p, obj_p)
  {
    sterc_handler_p = MPL_LIST_CONTAINER(obj_p, sterc_handler_t, list_entry);
    switch(param_id)
    {
    case sterc_paramid_handle:
      if (*(int *)value == sterc_handler_p->handle)
        return sterc_handler_p;
      break;
    case sterc_paramid_cid:
      if (CID_PRESENT_IN_LIST(sterc_handler_p->sterc_params_p)) {
        int cid = GET_CID_FROM_LIST(sterc_handler_p->sterc_params_p);
        if (*(int *)value == cid)
          return sterc_handler_p;
      }
      break;
    case sterc_paramid_pppd_pid:
      if (PPPD_PID_PRESENT_IN_LIST(sterc_handler_p->sterc_params_p)) {
        int pppd_pid = GET_PPPD_PID_FROM_LIST(sterc_handler_p->sterc_params_p);
        if (*(int *)value == pppd_pid)
          return sterc_handler_p;
      }
      break;
    default:
      STERC_DBG_TRACE(LOG_ERR, "Unsupported param_id for search\n");
    }
  }
  return NULL;
}


/**
 * sterc_get_param()
 **/
static mpl_param_element_t* sterc_get_param(int handle,sterc_paramid_t  paramid, char *key)
{
  sterc_handler_t *sterc_p;
  mpl_list_t **param_list_pp;
  mpl_param_element_t *param_elem_p;

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle,&handle);

  if(NULL == sterc_p)
  {
    /* a sterc object does not exist for this handle */
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n",handle);
    return (NULL);
  }

  param_list_pp = sterc_get_param_list_ref(sterc_p);

  if (NULL == key)
  {
    param_elem_p = mpl_param_list_find(paramid, *param_list_pp);
    if (NULL == param_elem_p)
    {
      /* ...no get it from the default configuration instead */
      param_elem_p = mpl_config_get_para(paramid, &stercd_config);
      if (NULL == param_elem_p)
      {
        /* No value found for parameter! */
        return (NULL);
      }
    }
  }
  else
  {
    char *wildcard = "*";

    param_elem_p = mpl_param_list_tuple_key_find_wildcard(paramid, key, wildcard, *param_list_pp);
    if (NULL == param_elem_p)
    {
      /* ...no get it from the default configuration instead */
      param_elem_p = mpl_config_tuple_key_get_para(paramid,key, wildcard, &stercd_config);
      if (NULL == param_elem_p)
      {
        /* No value found for parameter! */
        return (NULL);
      }
    }
  }

  return param_elem_p;
}

/**
 * sterc_set_param()
 **/
static int sterc_set_param(int handle, mpl_param_element_t *param)
{
  sterc_handler_t *sterc_p;
  mpl_list_t **param_list_pp;

  assert(param != NULL);

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle, &handle);

  if(NULL == sterc_p) {
    /* a PS connection object does not exist for this handle*/
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n",handle);
    return (-1);
  }

  param_list_pp = sterc_get_param_list_ref(sterc_p);
  mpl_list_add(param_list_pp, &param->list_entry);

  return (0);
}


/**
 * sterc_delete_param()
 **/
static void sterc_delete_param(int handle, sterc_paramid_t paramid)
{
  sterc_handler_t *sterc_p;
  mpl_list_t **param_list_pp;
  mpl_param_element_t *param_p;

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle, &handle);
  if (NULL == sterc_p) {
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n", handle);
    return;
  }

  param_list_pp = sterc_get_param_list_ref(sterc_p);

  while(NULL != (param_p = mpl_param_list_find(paramid, *param_list_pp)))
  {
    (void)mpl_list_remove(param_list_pp, &param_p->list_entry);
    mpl_param_element_destroy(param_p);
  }
}

/**
 * sterc_pscc_connected()
 **/
static void sterc_pscc_connected(int handle)
{
  sterc_handler_t *sterc_p;
  mpl_list_t **param_list_pp,*list_p;
  mpl_param_element_t *param_p,*cloned_param_p;

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle, &handle);

  if(NULL == sterc_p) {
    /* a sterc object does not exist for this handle */
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n", handle);
    return;
  }

  /* convert WAN parameters to LAN parameters */
  param_list_pp = sterc_get_param_list_ref(sterc_p);

  MPL_LIST_FOR_EACH(*param_list_pp, list_p) {
    param_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (param_p != NULL) {
      if(param_p->id == sterc_paramid_wan_ip) {
        cloned_param_p = mpl_param_element_clone(param_p);
        if (NULL != cloned_param_p)  {
          cloned_param_p->id = sterc_paramid_lan_peer_ip;
          mpl_list_add(param_list_pp, &cloned_param_p->list_entry);
        }
      }
      if(param_p->id == sterc_paramid_wan_dns) {
        cloned_param_p = mpl_param_element_clone(param_p);
        if (NULL != cloned_param_p) {
          cloned_param_p->id = sterc_paramid_lan_dns;
          mpl_list_add(param_list_pp, &cloned_param_p->list_entry);
        }
      }
    }
  }

  if (sterc_p->lan_if_funcs.resume) {
    sterc_p->lan_if_funcs.resume(sterc_p->lan_if_handle,sterc_result_ok);
  }

  sterc_p->wan_connected = true;

  if (true == sterc_p->lan_connected)
    sterc_handler_change_state(sterc_p, sterc_handler_connected);

}

/**
 * sterc_pscc_disconnected()
 **/
static void sterc_pscc_disconnected(int handle)
{
  sterc_handler_t *sterc_p;

  STERC_DBG_TRACE(LOG_INFO, "pscc disconnected\n");

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle, &handle);

  if(NULL == sterc_p) {
    /* a sterc object does not exist for this handle */
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n", handle);
    return;
  }

  sterc_p->wan_connected = false;

  switch(sterc_p->state)
  {
  case sterc_handler_disconnecting:
    if (false == sterc_p->lan_connected) {
      sterc_send_disconnect_event(sterc_p);
      sterc_handler_change_state(sterc_p, sterc_handler_created);
    }
  break;
  case sterc_handler_connecting:
    if (sterc_p->lan_if_funcs.resume) {
      sterc_p->lan_if_funcs.resume(sterc_p->lan_if_handle,sterc_result_failed_unspecified);
    }
    /* fallthrough */
  case sterc_handler_connected:
    sterc_handler_disconnect(sterc_p);
    break;
  default:
    break;
  }
}


/**
 * sterc_if_connected()
 **/
static void sterc_if_connected(int handle)
{
  sterc_handler_t *sterc_p;

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle, &handle);

  if(NULL == sterc_p) {
    /* a sterc object does not exist for this handle */
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n", handle);
    return;
  }

  sterc_p->lan_connected = true;

  //check wan connection if we need
  if (true == sterc_p->wan_connected) {
    sterc_handler_change_state(sterc_p, sterc_handler_connected);
  }

}

/**
 * sterc_if_disconnected()
 **/
static void sterc_if_disconnected(int handle)
{
  sterc_handler_t *sterc_p;

  /* find parent object */
  sterc_p = sterc_handler_find(sterc_paramid_handle, &handle);

  if(NULL == sterc_p) {
    /* a sterc object does not exist for this handle */
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n", handle);
    return;
  }

  sterc_p->lan_connected = false;

  switch(sterc_p->state)
  {
  case sterc_handler_connecting:
  case sterc_handler_connected:
    sterc_handler_disconnect(sterc_p);
    break;
  case sterc_handler_disconnecting:
    if (false == sterc_p->wan_connected) {
      sterc_send_disconnect_event(sterc_p);
      sterc_handler_change_state(sterc_p, sterc_handler_created);
    }
    break;
  default:
    break;
  }
}

/**
 * sterc_send_disconnect_event()
 **/
static void sterc_send_disconnect_event(sterc_handler_t *obj_p)
{
  char *eventbuf_p=NULL;
  size_t len=0;
  mpl_msg_t *msg_p;

  msg_p = mpl_event_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == msg_p) {
    STERC_DBG_TRACE(LOG_ERR, "Failed to alloc memory\n");
    return;
  }

  msg_p->event.id = sterc_event_disconnected;
  msg_p->event.param_list_p = NULL;
  (void) mpl_add_param_to_list(&msg_p->event.param_list_p, sterc_paramid_handle, &obj_p->handle);
  (void) mpl_add_param_to_list(&msg_p->event.param_list_p, sterc_paramid_reason, &obj_p->reason);

  if (0 > mpl_msg_pack(msg_p, &eventbuf_p, &len)) {
    STERC_DBG_TRACE(LOG_ERR, "failed to pack event message\n");
    goto error_return;
  }

  STERC_DBG_TRACE(LOG_INFO, "Sending event: %s\n", eventbuf_p);
  (void)stercd_send_event(eventbuf_p, len);

error_return:
  if(NULL != eventbuf_p)
    free(eventbuf_p);
  mpl_msg_free(msg_p);
}
