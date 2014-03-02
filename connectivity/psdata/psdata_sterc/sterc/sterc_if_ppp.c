/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Sterc PPP handler
 */


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "sterc_if_ppp.h"
#include "sterc_if.h"
#include "sterc_handler.h"
#include "sterc_msg.h"
#include "stercd.h"
#include "sterc_runscript.h"
#include "mpl_param.h"
#include "mpl_msg.h"
#include "mpl_list.h"
#include "libstecom.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <fcntl.h>
/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define PPPD_PLUGIN_NAME "/system/lib/ppp_sterc.so"

#define PPPD_PID_PRESENT_IN_LIST(param_list_p) MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_pppd_pid, (param_list_p))
#define GET_PPPD_PID_FROM_LIST(param_list_p) MPL_GET_PARAM_VALUE_FROM_LIST(int,sterc_paramid_pppd_pid,(param_list_p))


#define STERC_PPP_STATES\
  STERC_PPP_STATE_ELEMENT(ppp_created) \
  STERC_PPP_STATE_ELEMENT(ppp_connecting) \
  STERC_PPP_STATE_ELEMENT(ppp_connected) \
  STERC_PPP_STATE_ELEMENT(ppp_disconnecting)

#define STERC_PPP_STATE_ELEMENT(STATE) sterc_##STATE,
typedef enum
{
  STERC_PPP_STATES
  sterc_ppp_number_of_states
} sterc_ppp_state_t;
#undef STERC_PPP_STATE_ELEMENT

#define STERC_PPP_STATE_ELEMENT(STATE) #STATE,
static const char* sterc_ppp_state_names[] =
{
  STERC_PPP_STATES
};
#undef STERC_PPP_STATE_ELEMENT

/**
 * please note that the debug trace in this macro is
 * used in the test scripts. If the printout is changed
 * please update the test pattern in the unix.exp file
**/
#define sterc_ppp_change_state(obj_p,new_state)                         \
  do                                                                    \
  {                                                                     \
    assert((new_state) < sterc_ppp_number_of_states);                   \
    assert((obj_p)->state < sterc_ppp_number_of_states);                \
    assert((obj_p) != NULL);                                            \
    STERC_DBG_TRACE(LOG_DEBUG, "ppp state change: %s->%s\n",            \
                    sterc_ppp_state_names[(obj_p)->state],              \
                    sterc_ppp_state_names[(new_state)]);                \
    (obj_p)->state = new_state;                                         \
  } while(0)


typedef struct
{
  sterc_ppp_state_t state;
  int handle;
  int parentid;
  sterc_get_param_fp_t get_param_cb;
  sterc_set_param_fp_t set_param_cb;
  sterc_delete_param_fp_t delete_param_cb;
  sterc_if_connected_fp_t connected_cb;
  sterc_if_disconnected_fp_t disconnected_cb;
  mpl_list_t list_entry;
} sterc_ppp_t;

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static mpl_list_t *sterc_ppp_list_p=NULL;
static struct stec_ctrl *ctrl_handle_p=NULL; //Used for delayed response to ppp plugin
static int sterc_ppp_identifier = 0;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static int sterc_ppp_create(int parentid,
                            sterc_get_param_fp_t get_param_cb,
                            sterc_set_param_fp_t set_param_cb,
                            sterc_delete_param_fp_t delete_param_cb);
static void sterc_ppp_destroy(int handle);
static int sterc_ppp_connect(int handle,
                             sterc_if_connected_fp_t connected_cb,
                             sterc_if_disconnected_fp_t disconnected_cb);
static void sterc_ppp_disconnect(int handle);
static void sterc_ppp_resume(int handle, sterc_result_t result);

static void handle_event_ppp_up(mpl_msg_t *event_ppp_up_p);
static void handle_event_ppp_down(mpl_msg_t *event_ppp_down_p);
static mpl_msg_t * handle_req_connect_wan(mpl_msg_t *req_connect_wan_p);

static sterc_ppp_t* sterc_ppp_find(sterc_paramid_t param_id, void* value);

static void sterc_ppp_disconnect_int(sterc_ppp_t *obj_p, bool report);

static pid_t spawn_pppd(char* tty_name, char* ip_address_option);

/********************************************************************************
 *
 * Global variables
 *
 ********************************************************************************/
sterc_if_ctrl_block_t sterc_ppp_if =
{
  sterc_ppp_create,
  sterc_ppp_destroy,
  sterc_ppp_connect,
  sterc_ppp_disconnect,
  sterc_ppp_resume
};


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
int sterc_ppp_handler(struct stec_ctrl *ctrl, mpl_msg_t *msg_p, mpl_msg_t **rsp_msg)
{
  assert(msg_p != NULL);
  assert(rsp_msg != NULL);

  if (sterc_msgtype_req == msg_p->common.type)
  {
    switch(msg_p->common.id)
    {

    case sterc_connect_wan:
      *rsp_msg = handle_req_connect_wan(msg_p);
       ctrl_handle_p = ctrl;
      break;
    default:
      return 0; //Request not for ppp
    }
  }
  else if (sterc_msgtype_event == msg_p->common.type)
  {
    switch(msg_p->common.id)
    {
    case sterc_event_ppp_up:
      handle_event_ppp_up(msg_p);
      break;
    case sterc_event_ppp_down:
      handle_event_ppp_down(msg_p);
      break;
    default:
      return 0; //Event not for ppp
    }
    //No response, set rsp_msg to NULL
    *rsp_msg=NULL;
  }

  return 1; //Message handeled by PPP
}



/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
static int sterc_ppp_create(int parentid,
                            sterc_get_param_fp_t get_param_cb,
                            sterc_set_param_fp_t set_param_cb,
                            sterc_delete_param_fp_t delete_param_cb)
{
  sterc_ppp_t *obj_p = NULL;

  assert(get_param_cb != NULL);
  assert(set_param_cb != NULL);
  assert(delete_param_cb != NULL);

  obj_p = malloc(sizeof(sterc_ppp_t));
  if(NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return -1;
  }

  /*Check that tty param exist in the parameter list */
  if (NULL == get_param_cb(parentid, sterc_paramid_tty,NULL))
  {
    STERC_DBG_TRACE(LOG_ERR, "Missing TTY param\n");
    free(obj_p);
    return -1;
  }

  memset(obj_p,0,sizeof(sterc_ppp_t));
  obj_p->handle = ++sterc_ppp_identifier;
  obj_p->parentid = parentid;
  obj_p->get_param_cb = get_param_cb;
  obj_p->set_param_cb = set_param_cb;
  obj_p->delete_param_cb = delete_param_cb;
  mpl_list_add(&sterc_ppp_list_p,&obj_p->list_entry);

  sterc_ppp_change_state(obj_p,sterc_ppp_created);
  STERC_DBG_TRACE(LOG_DEBUG, "sterc ppp created\n");

  return (obj_p->handle);
}


static void sterc_ppp_destroy(int handle)
{
  sterc_ppp_t *obj_p;
  int pppd_pid, status;
  mpl_param_element_t *param_p;

  obj_p = sterc_ppp_find(sterc_paramid_handle, &handle);
  if (NULL == obj_p)
    return;

  //Kill pppd
  param_p = obj_p->get_param_cb(obj_p->parentid,sterc_paramid_pppd_pid,NULL);
  if (NULL != param_p)
  {
    //Kill pppd
    pppd_pid =  MPL_GET_VALUE_FROM_PARAM_ELEMENT(int, param_p);
    if (0 != pppd_pid)
    {
      STERC_DBG_TRACE(LOG_DEBUG, "sterc ppp: pppd killed\n");
      kill(pppd_pid, SIGTERM);
      waitpid(pppd_pid, &status, 0);
    }
  }

  mpl_list_remove(&sterc_ppp_list_p,&obj_p->list_entry);
  free(obj_p);
  STERC_DBG_TRACE(LOG_DEBUG, "sterc ppp freed\n");
  return;
}

static int sterc_ppp_connect(int handle,
                             sterc_if_connected_fp_t connected_cb,
                             sterc_if_disconnected_fp_t disconnected_cb)
{
  sterc_ppp_t *obj_p;
  mpl_param_element_t *param_p;
  pid_t pid = -1;
  char* tmp_p = NULL;
  char* tty_name_p = NULL;
  char* ip_address_option_p = NULL;
  size_t elem_size;
  int res = 0;

  assert(connected_cb != NULL);
  assert(disconnected_cb != NULL);

  obj_p = sterc_ppp_find(sterc_paramid_handle, &handle);
  if (NULL == obj_p) {
    res = -1;
    goto free_and_return;
  }

  if (sterc_ppp_created != obj_p->state)  {
    STERC_DBG_TRACE(LOG_ERR, "sterc ppp in wrong state %s\n",sterc_ppp_state_names[(obj_p)->state]);
    res = -1;
    goto free_and_return;
  }

  if ((NULL == obj_p->get_param_cb) || (NULL == obj_p->set_param_cb)) {
    STERC_DBG_TRACE(LOG_ERR, "missing set and/or get callback");
    res = -1;
    goto free_and_return;
  }

  //Store callbacks
  obj_p->connected_cb = connected_cb;
  obj_p->disconnected_cb = disconnected_cb;


  //First fetch the tty name
  param_p = obj_p->get_param_cb(obj_p->parentid, sterc_paramid_tty, NULL);
  if ((NULL == param_p) || (NULL == param_p->value_p)) {
    res = -1;
    goto free_and_return;
  }

  tty_name_p = (char*)param_p->value_p;

  //Fetch the own ip address
  param_p = obj_p->get_param_cb(obj_p->parentid, sterc_paramid_lan_own_ip, NULL);
  if ((NULL == param_p) || (NULL == param_p->value_p)) {
    res = -1;
    goto free_and_return;
  }

  /* in case the local ip address has a '*:* prefix... */
  tmp_p = strstr((char*)param_p->value_p, "*:");
  if (param_p->value_p == tmp_p)
    tmp_p += strlen("*:");
  else
    tmp_p = (char*)param_p->value_p;

  elem_size = snprintf(NULL, 0, "%s:", tmp_p);
  elem_size++;

  ip_address_option_p = malloc(elem_size);
  if(NULL == ip_address_option_p) {
    STERC_DBG_TRACE(LOG_ERR, "malloc failed");
    res = -1;
    goto free_and_return;
  }

  elem_size = snprintf(ip_address_option_p, elem_size, "%s:", tmp_p);
  ip_address_option_p[elem_size]='\0';

  pid = spawn_pppd(tty_name_p, ip_address_option_p);

  STERC_DBG_TRACE(LOG_INFO, "pppd pid=%d", pid);

  if (pid != -1) {
    //Store the pppd pid
    param_p = mpl_param_element_create(sterc_paramid_pppd_pid, &pid);
    if (obj_p->set_param_cb(obj_p->parentid, param_p) < 0) {
      STERC_DBG_TRACE(LOG_ERR, "not able to store pppd pid");
      mpl_param_element_destroy(param_p);
    }
  }
  else {
    STERC_DBG_TRACE(LOG_ERR, "failed to start pppd");
    res = -1;
    goto free_and_return;
  }

  //Set state to CONNECTING, Runscript will do the rest....
  sterc_ppp_change_state(obj_p,sterc_ppp_connecting);

free_and_return:
  free(ip_address_option_p);
  return res;
}

static void sterc_ppp_disconnect(int handle)
{
  sterc_ppp_t *obj_p;

  obj_p = sterc_ppp_find(sterc_paramid_handle, &handle);
  if (NULL == obj_p)
    return;

  sterc_ppp_disconnect_int(obj_p, false);
}

static void sterc_ppp_disconnect_int(sterc_ppp_t *obj_p, bool report)
{
  int pppd_pid, status;
  mpl_param_element_t *param_p;

  assert(obj_p != NULL);
  assert(obj_p->get_param_cb != NULL);

  if(sterc_ppp_created == obj_p->state) {
    STERC_DBG_TRACE(LOG_INFO, "ppp disconnect already initiated, returning..");
    return;
  }

  if (!report)
    obj_p->disconnected_cb = NULL; //To make sure that the disconnected_cb is not called
  obj_p->connected_cb = NULL;

  sterc_ppp_change_state(obj_p,sterc_ppp_disconnecting);

  param_p = obj_p->get_param_cb(obj_p->parentid,sterc_paramid_pppd_pid,NULL);
  if (NULL != param_p)
  {
    //Kill pppd
    pppd_pid =  MPL_GET_VALUE_FROM_PARAM_ELEMENT(int, param_p);
    if (0 != pppd_pid)
    {
     STERC_DBG_TRACE(LOG_INFO, "Sending SIGTERM to pid=%d", pppd_pid);
     kill(pppd_pid, SIGTERM);
     waitpid(pppd_pid, &status, 0);
    }
  }


  sterc_ppp_change_state(obj_p,sterc_ppp_created);

  //Report to handler
  if (report && NULL != obj_p->disconnected_cb)
  {
    obj_p->disconnected_cb(obj_p->parentid);
    obj_p->disconnected_cb = NULL;
  }

}


static void sterc_ppp_resume(int handle, sterc_result_t result)
{
  mpl_msg_t *resp_msg_p = NULL;
  size_t resp_len;
  char *resp_p=NULL;
  struct sockaddr_un sock_addr;
  sterc_ppp_t *obj_p;
  char * addr;
  mpl_param_element_t *param_p;

  /*Check if response already sent*/
  if(NULL == ctrl_handle_p)
    return;

  /* find parent object */
  obj_p = sterc_ppp_find(sterc_paramid_handle,(void*)&handle);
  if(NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "object with id %d not found\n",handle);
    return;
  }

  resp_msg_p = mpl_msg_alloc(STERC_PARAM_SET_ID);
  if(NULL == resp_msg_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Failed allocating memory\n");
    goto cleanup_and_return;
  }

  resp_msg_p->resp.type = mpl_msg_type_resp;
  resp_msg_p->resp.id = sterc_connect_wan;
  resp_msg_p->resp.result = result;

  resp_p = malloc(STERCD_BUFSIZ);
  resp_len = STERCD_BUFSIZ;

  //Pack the message
  if (NULL != resp_msg_p)
  {
    if(0 > mpl_msg_pack((mpl_msg_t*) resp_msg_p, &resp_p, &resp_len))
    {
      STERC_DBG_TRACE(LOG_ERR, "Failed packing response message: %s\n",
                      mpl_param_value_get_string(sterc_paramid_message, &(resp_msg_p->common.id)));
      goto cleanup_and_return;

    }
  }

  if (NULL == obj_p->get_param_cb)
  {
    STERC_DBG_TRACE(LOG_ERR, "get_param_cb is NULL\n");
    goto cleanup_and_return;
  }

  param_p = obj_p->get_param_cb(obj_p->parentid,sterc_paramid_socket_addr,NULL);
  if (NULL == param_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "param socket_addr not found\n");
    goto cleanup_and_return;
  }

  addr = MPL_GET_VALUE_REF_FROM_PARAM_ELEMENT(char*, param_p);

  //Fill in socket address
  memset(&sock_addr,0,sizeof(sock_addr));
  sock_addr.sun_family = AF_UNIX;
  strncpy(sock_addr.sun_path,addr, strlen(addr));

  if (stec_sendto(ctrl_handle_p, resp_p, resp_len, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
    STERC_DBG_TRACE(LOG_ERR, "stec_send: %s\n", strerror(errno));
    goto cleanup_and_return;
  }

  ctrl_handle_p = NULL;
  mpl_msg_free((mpl_msg_t*) resp_msg_p);
  free(resp_p);
  resp_msg_p = NULL;

  return;

  cleanup_and_return:
  if (resp_msg_p)
    mpl_msg_free((mpl_msg_t*) resp_msg_p);
  if (resp_p)
    free(resp_p);
  resp_msg_p = NULL;
  if (NULL != obj_p->get_param_cb)
  {
    sterc_ppp_disconnect_int(obj_p,true);
  }
}


static void handle_event_ppp_up(mpl_msg_t *event_ppp_up_p)
{
  sterc_ppp_t *obj_p;
  int pppd_pid;
  mpl_param_element_t *param_p, *tmp_param_p;

  assert(event_ppp_up_p != NULL);
  assert(event_ppp_up_p->req.type == mpl_msg_type_event);
  assert(event_ppp_up_p->req.id == sterc_event_ppp_up);

  if (!PPPD_PID_PRESENT_IN_LIST(event_ppp_up_p->event.param_list_p))
  {
    STERC_DBG_TRACE(LOG_ERR, "PPPd PID not present in list\n");
    return;
  }
  pppd_pid = GET_PPPD_PID_FROM_LIST(event_ppp_up_p->event.param_list_p);

  STERC_DBG_TRACE(LOG_DEBUG, "STERC PPP_UP received PPP_id=%d\n",pppd_pid);

  //Find object
  obj_p = sterc_ppp_find(sterc_paramid_pppd_pid,(void*)&pppd_pid);
  if (NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Sterc ppp object not found\n");
    return;
  }

  //We should also get the devicename
  param_p = mpl_param_list_find(sterc_paramid_lan_device,event_ppp_up_p->event.param_list_p);
  if (NULL == param_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Device name not present in list\n");
    goto error_cleanup_and_return;
  }

  tmp_param_p = mpl_param_element_clone(param_p);
  if(NULL == tmp_param_p)
  {
    STERC_DBG_TRACE(LOG_ERR,"param clone failed\n");
    goto error_cleanup_and_return;
  }

  if (NULL != obj_p->set_param_cb)
  {
    if (obj_p->set_param_cb(obj_p->parentid,tmp_param_p) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR, "Failed to add devicename to list\n");
      mpl_param_element_destroy(tmp_param_p);
      goto error_cleanup_and_return;
    }
  }
  else
  {
    STERC_DBG_TRACE(LOG_ERR,"set param failed, invalid set_param_cb\n");
    mpl_param_element_destroy(tmp_param_p);
    goto error_cleanup_and_return;
  }

  //set state
  sterc_ppp_change_state(obj_p,sterc_ppp_connected);

  //Call connected_cb
  obj_p->connected_cb(obj_p->parentid);

  return;

  error_cleanup_and_return:
  sterc_ppp_disconnect_int(obj_p, true);

  return;
}


static void handle_event_ppp_down(mpl_msg_t *event_ppp_down_p)
{
  sterc_ppp_t *obj_p;
  int pppd_pid;

  assert(event_ppp_down_p != NULL);
  assert(event_ppp_down_p->req.type == mpl_msg_type_event);
  assert(event_ppp_down_p->req.id == sterc_event_ppp_down);

  if (!PPPD_PID_PRESENT_IN_LIST(event_ppp_down_p->event.param_list_p))
  {
    STERC_DBG_TRACE(LOG_ERR, "PPPd PID not present in list\n");
    return;
  }
  pppd_pid = GET_PPPD_PID_FROM_LIST(event_ppp_down_p->event.param_list_p);

  STERC_DBG_TRACE(LOG_DEBUG, "STERC PPP_DOWN received PPP_id=%d\n",pppd_pid);

  //Find object
  obj_p = sterc_ppp_find(sterc_paramid_pppd_pid,(void*)&pppd_pid);
  if (NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Sterc object not found\n");
    return;
  }

  sterc_ppp_disconnect_int(obj_p, true);

  return;
}


/**
 * handle_req_connect_wan()
 **/
static mpl_msg_t * handle_req_connect_wan(mpl_msg_t *req_connect_wan_p)
{
  sterc_ppp_t *obj_p;
  int pppd_pid;
  mpl_list_t *list_p,*tmp_p;
  mpl_param_element_t *param_p, *tmp_param_p;

  assert(req_connect_wan_p != NULL);
  assert(req_connect_wan_p->req.type == mpl_msg_type_req);
  assert(req_connect_wan_p->req.id == sterc_connect_wan);

  if (!PPPD_PID_PRESENT_IN_LIST(req_connect_wan_p->req.param_list_p))
  {
    STERC_DBG_TRACE(LOG_ERR, "PPPd PID not present in list\n");
    return NULL;
  }
  pppd_pid = GET_PPPD_PID_FROM_LIST(req_connect_wan_p->req.param_list_p);

  //Find object
  obj_p = sterc_ppp_find(sterc_paramid_pppd_pid,(void*)&pppd_pid);
  if (NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "Sterc object not found\n");
    return NULL;
  }

  //Store address. It should be there
  MPL_LIST_FOR_EACH_SAFE(req_connect_wan_p->req.param_list_p, list_p, tmp_p)
  {
    param_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (sterc_paramid_socket_addr != param_p->id)
      continue;

    //In case it already exist in the handler, delete it
    if (obj_p->delete_param_cb)
      obj_p->delete_param_cb(obj_p->parentid,sterc_paramid_socket_addr);

    //Add parameter
    tmp_param_p = mpl_param_element_clone(param_p);
    if(NULL == tmp_param_p)
    {
      STERC_DBG_TRACE(LOG_ERR,"param clone failed\n");
      sterc_ppp_disconnect_int(obj_p,true);
      return NULL;
    }
    if (obj_p->set_param_cb)
    {
      obj_p->set_param_cb(obj_p->parentid,tmp_param_p);
    }
    else
    {
      STERC_DBG_TRACE(LOG_ERR,"set param failed, invalid set_param_cb\n");
      mpl_param_element_destroy(tmp_param_p);
      sterc_ppp_disconnect_int(obj_p,true);
      return NULL;
    }
  }

  //Notify the handler that PPP is auth parameters are ready, and that the pdp context can be started
  sterc_handler_connect_wan(obj_p->parentid);

  //The handler will call "resume" when the address is available

  //Set state to connecting
  sterc_ppp_change_state(obj_p,sterc_ppp_connecting);

  return NULL;
}


/**
 *  sterc_context_find()
 **/
static sterc_ppp_t* sterc_ppp_find(sterc_paramid_t param_id, void* value)
{
  mpl_list_t *obj_p;
  sterc_ppp_t *sterc_ppp_p;
  mpl_param_element_t *param_p;

  assert(value != NULL);

  MPL_LIST_FOR_EACH(sterc_ppp_list_p, obj_p)
  {
    sterc_ppp_p = MPL_LIST_CONTAINER(obj_p, sterc_ppp_t, list_entry);
    switch(param_id)
    {
    case sterc_paramid_handle:
      if (*(int *)value == sterc_ppp_p->handle)
        return sterc_ppp_p;
      break;
    case sterc_paramid_pppd_pid:
      if (sterc_ppp_p->get_param_cb)
      {
        param_p = sterc_ppp_p->get_param_cb(sterc_ppp_p->parentid,sterc_paramid_pppd_pid,NULL);
        if (NULL == param_p)
        {
          STERC_DBG_TRACE(LOG_ERR, "param socket_addr not found\n");
          return NULL;
        }

        if (*(int *)value == MPL_GET_VALUE_FROM_PARAM_ELEMENT(int, param_p))
          return sterc_ppp_p;
      }
      break;
    default:
      STERC_DBG_TRACE(LOG_ERR, "Unsupported param_id for search\n");
    }
  }
  return NULL;
}

/**
 * spawn_pppd - Spawn a new pppd process
 **/
static pid_t spawn_pppd(char* tty_name, char* ip_address_option)
{
  pid_t pid;
  char *argv[] = {"pppd",
                  tty_name,
                  ip_address_option,
                  "nodetach",
                  "passive",
                  "debug",
                  "plugin", PPPD_PLUGIN_NAME,
                  NULL};


  STERC_DBG_TRACE(LOG_INFO, "spawn_pppd, tty_name = %s, ip_address_option = %s", tty_name, ip_address_option);


  pid = fork();
  if (pid < 0) {
    return -1;
  }

  if (0 == pid)
  {
    /* Child */
    int fd;
    fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
      STERC_DBG_TRACE(LOG_INFO, "open dev/null failed.");
    }
    else {
      dup2(fd, 0);
      dup2(fd, 1);
      dup2(fd, 2);
      close(fd);
    }

    execve("/system/bin/pppd", argv, NULL);
    exit(EXIT_SUCCESS);
  }

  return pid;
}
