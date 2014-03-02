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
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/un.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "stercd.h"
#include "libstecom.h"
#include "sterc_pscc.h"
#include "pscc_msg.h"
#include "mpl_list.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define STERC_PSCC_MAX_MSG_LEN 1000
#define STERC_PSCC_STATES                        \
  STERC_PSCC_STATE_ELEMENT(idle)                 \
  STERC_PSCC_STATE_ELEMENT(connecting)           \
  STERC_PSCC_STATE_ELEMENT(connected)            \
  STERC_PSCC_STATE_ELEMENT(disconnecting)        \

#define STERC_PSCC_STATE_ELEMENT(STATE) sterc_pscc_state_##STATE,
typedef enum
{
  STERC_PSCC_STATES
  sterc_pscc_number_of_states
} sterc_pscc_state_t;
#undef STERC_PSCC_STATE_ELEMENT

#define STERC_PSCC_STATE_ELEMENT(STATE) #STATE,
static const char* sterc_pscc_state_names[] =
{
  STERC_PSCC_STATES
};
#undef STERC_PSCC_STATE_ELEMENT

#define sterc_pscc_change_state(obj_p,new_state)                             \
  do                                                                         \
  {                                                                          \
    assert((sterc_pscc_state_##new_state) < sterc_pscc_number_of_states);    \
    assert((obj_p)->state < sterc_pscc_number_of_states);                    \
    assert((obj_p) != NULL);                                                 \
    STERC_DBG_TRACE(LOG_DEBUG, "pscc state change: %s->%s\n",                \
                    sterc_pscc_state_names[(obj_p)->state],                  \
                    sterc_pscc_state_names[(sterc_pscc_state_##new_state)]); \
    (obj_p)->state = sterc_pscc_state_##new_state;                           \
  } while(0)

typedef struct
{
  sterc_pscc_state_t state;
  int handle;
  int parentid;
  int connid;
  sterc_get_param_fp_t get_param_cb;
  sterc_set_param_fp_t set_param_cb;
  sterc_delete_param_fp_t delete_param_cb;
  sterc_pscc_connected_fp_t connected_cb;
  sterc_pscc_disconnected_fp_t disconnected_cb;
  bool created_by_me;
  mpl_list_t list_entry;
} sterc_pscc_t;

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static struct stec_ctrl *sterc_pscc_ctrl_p=NULL;
static struct stec_ctrl *sterc_pscc_event_p=NULL;
static char *sterc_pscc_msg_packed_p = NULL;
static pscc_msg_t *sterc_pscc_msg_p=NULL;
static mpl_list_t *sterc_pscc_list_p=NULL;

static const pscc_paramid_t sterc_pscc_req_param[] = {pscc_paramid_own_ip_address,
                                                      pscc_paramid_dns_address,
                                                      pscc_paramid_netdev_name};
static int sterc_pscc_num_req_param = (sizeof(sterc_pscc_req_param)/sizeof(pscc_paramid_t));

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static sterc_pscc_t *find_pscc_obj_by_handle(int handle);
static sterc_pscc_t *find_pscc_obj_by_connid(int connid);
static int create_pscc_connection(sterc_pscc_t *obj_p);
static int get_connection_status(sterc_pscc_t *obj_p,pscc_connection_status_t *status_p);
static int connect_pscc(sterc_pscc_t *obj_p);
static int disconnect_pscc(sterc_pscc_t *obj_p);
static int destroy_pscc(sterc_pscc_t *obj_p);
static void handle_event_connected(sterc_pscc_t *obj_p);
static void handle_event_disconnected(sterc_pscc_t *obj_p);
static mpl_list_t* get_pscc_parameters(sterc_pscc_t *obj_p,const pscc_paramid_t *reqparam_p, int num_param);
static void set_pscc_parameters(sterc_pscc_t *obj_p,mpl_list_t *param_list_p);
static int pack_send_receive_unpack(mpl_msg_t *msg_p, mpl_msg_resp_t *resp_p);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * sterc_pscc_init()
 **/
int sterc_pscc_init(sterc_transport_type_t tt, char *ctrl_addr_p,char *event_addr_p)
{
  assert(tt!=tt_unknown);
  assert(ctrl_addr_p != NULL);
  assert(event_addr_p != NULL);

  pscc_init(NULL, &stercd_log_func);

  sterc_pscc_msg_packed_p = malloc(STERC_PSCC_MAX_MSG_LEN);
  if(NULL == sterc_pscc_msg_packed_p)
  {
    STERC_DBG_TRACE( LOG_ERR, "malloc failed\n");
    goto error_free_return;
  }

  sterc_pscc_msg_p = mpl_msg_alloc(PSCC_PARAM_SET_ID);
  if (NULL == sterc_pscc_msg_p)
  {
    STERC_DBG_TRACE( LOG_ERR,"Failed allocating message\n");
    goto error_free_return;
  }

  if (tt == tt_unix)
  {
    struct sockaddr_un addr_un;
    memset(&addr_un, 0, sizeof(addr_un));
    addr_un.sun_family = AF_UNIX;
    strncpy(addr_un.sun_path, ctrl_addr_p, UNIX_PATH_MAX);
    sterc_pscc_ctrl_p = stec_open((struct sockaddr*) &addr_un, sizeof(addr_un));
    if (sterc_pscc_ctrl_p == NULL)
    {
      STERC_DBG_TRACE( LOG_DEBUG, "stec_open failed\n");
      goto error_free_return;
    }

    memset(&addr_un, 0, sizeof(addr_un));
    addr_un.sun_family = AF_UNIX;
    strncpy(addr_un.sun_path, event_addr_p, UNIX_PATH_MAX);
    sterc_pscc_event_p = stec_open((struct sockaddr*) &addr_un, sizeof(addr_un));
    if (sterc_pscc_event_p == NULL)
    {
      STERC_DBG_TRACE( LOG_DEBUG, "stec_open failed\n");
      goto error_free_return;
    }
  }

  if (tt == tt_ip)
  {
    struct sockaddr_in addr_in;
    int port;
    static const int loopback = 0x7f000001;

    port = strtol(ctrl_addr_p, NULL, 10);
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = htonl(loopback);
    sterc_pscc_ctrl_p = stec_open((struct sockaddr*) &addr_in, sizeof(addr_in));
    if (NULL == sterc_pscc_ctrl_p)
    {
      STERC_DBG_TRACE( LOG_ERR, "stec_open failed\n");
      goto error_free_return;
    }

    port = strtol(event_addr_p, NULL, 10);
    memset(&addr_in, 0, sizeof(addr_in));
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = htonl(loopback);
    sterc_pscc_event_p = stec_open((struct sockaddr*) &addr_in, sizeof(addr_in));
    if (sterc_pscc_event_p == NULL)
    {
      STERC_DBG_TRACE( LOG_ERR, "stec_open failed\n");
      goto error_free_return;
    }
  }

  if(stec_subscribe(sterc_pscc_event_p)<0)
  {
    STERC_DBG_TRACE( LOG_ERR, "stec_subscribe failed\n");
    goto error_free_return;
  }

  return stec_get_fd(sterc_pscc_event_p);

  error_free_return:
  if(sterc_pscc_ctrl_p != NULL)
  {
    stec_close(sterc_pscc_ctrl_p);
    sterc_pscc_ctrl_p = NULL;
  }
  if(sterc_pscc_event_p != NULL)
  {
    stec_close(sterc_pscc_event_p);
    sterc_pscc_event_p = NULL;
  }
  if(sterc_pscc_msg_packed_p != NULL)
  {
    free(sterc_pscc_msg_packed_p);
    sterc_pscc_msg_packed_p = NULL;
  }
  if(sterc_pscc_msg_p != NULL)
  {
    free(sterc_pscc_msg_p);
    sterc_pscc_msg_p = NULL;
  }
  return(-1);
}

/**
 * sterc_pscc_deinit()
 **/
void sterc_pscc_deinit(void)
{
  mpl_list_t *obj_p, *tmp_p;
  sterc_pscc_t *pscc_p;

  /*  disconnect and destroy all pscc instances */
  MPL_LIST_FOR_EACH_SAFE(sterc_pscc_list_p, obj_p, tmp_p)
  {
    pscc_p = MPL_LIST_CONTAINER(obj_p, sterc_pscc_t, list_entry);
    assert(pscc_p != NULL);
    (void)mpl_list_remove(&sterc_pscc_list_p, &pscc_p->list_entry);
    switch(pscc_p->state)
    {
    case sterc_pscc_state_connecting:
    case sterc_pscc_state_connected:
      disconnect_pscc(pscc_p);
      /* fallthrough, go to idle, do not wait for disconnection to complete */
    case sterc_pscc_state_disconnecting:
      sterc_pscc_change_state(pscc_p,idle);
      /* fallthrough, destroy the device */
    case sterc_pscc_state_idle:
      destroy_pscc(pscc_p);
      break;
    default: /* unexpected states */
      STERC_DBG_TRACE(LOG_ERR, "unexpected state %d\n",pscc_p->state);
      free(pscc_p);
      break;
    }
  }

  if(sterc_pscc_ctrl_p != NULL)
    stec_close(sterc_pscc_ctrl_p);
  if(sterc_pscc_event_p != NULL)
    stec_close(sterc_pscc_event_p);
  if(sterc_pscc_msg_p != NULL)
    free(sterc_pscc_msg_p);
  if(sterc_pscc_msg_packed_p != NULL)
    free(sterc_pscc_msg_packed_p);
}

/**
 * sterc_pscc_handler()
 **/
int sterc_pscc_handler(int fd)
{
  int msg_len;
  sterc_pscc_t *obj_p;

  STERC_DBG_TRACE(LOG_DEBUG, "got event\n");

  assert(sterc_pscc_event_p != NULL);
  assert(fd == stec_get_fd(sterc_pscc_event_p));
  STERC_IDENTIFIER_NOT_USED(fd);

  msg_len = stec_recv(sterc_pscc_event_p,sterc_pscc_msg_packed_p,STERC_PSCC_MAX_MSG_LEN,-1);
  if(msg_len<=0)
  {
    STERC_DBG_TRACE(LOG_ERR, "stec_recv failed\n");
    return(-1);
  }
  STERC_DBG_TRACE(LOG_DEBUG, "%s\n",sterc_pscc_msg_packed_p);

  /* unpack message received */
  if (mpl_msg_unpack(sterc_pscc_msg_packed_p,msg_len,(mpl_msg_t *)sterc_pscc_msg_p) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Command unpacking failed\n");
    return(-1);
  }

  if(sterc_pscc_msg_p->common.type != mpl_msg_type_event)
  {
    STERC_DBG_TRACE(LOG_ERR,"not an event\n");
    mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
    return(-1);
  }

  if(!MPL_PARAM_PRESENT_IN_LIST(pscc_paramid_connid, (sterc_pscc_msg_p->common.param_list_p)))
  {
    STERC_DBG_TRACE(LOG_DEBUG,"connid missing in event message\n");
    mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
    return(0);
  }

  obj_p = find_pscc_obj_by_connid(MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(sterc_pscc_msg_p->common.param_list_p)));
  if(NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_DEBUG,"not for me\n");
    mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
    return(0);
  }

  mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
  switch(sterc_pscc_msg_p->common.id)
  {
  case pscc_event_connecting:
    STERC_DBG_TRACE(LOG_DEBUG,"connecting\n");
    break;
  case pscc_event_connected:
    STERC_DBG_TRACE(LOG_DEBUG,"connected\n");
    handle_event_connected(obj_p);
    break;
  case pscc_event_disconnecting:
    STERC_DBG_TRACE(LOG_DEBUG,"disconnecting\n");
    break;
  case pscc_event_disconnected:
    STERC_DBG_TRACE(LOG_DEBUG,"disconnected\n");
    handle_event_disconnected(obj_p);
    break;
  default:
    STERC_DBG_TRACE(LOG_DEBUG,"unexpected event\n");
    break;
  }

  return (0);
}

/**
 * sterc_pscc_create
 **/
int sterc_pscc_create(int parentid,
                      sterc_get_param_fp_t get_param_cb,
                      sterc_set_param_fp_t set_param_cb,
                      sterc_delete_param_fp_t delete_param_cb)
{
  static int sterc_pscc_handle=0;
  sterc_pscc_t *obj_p;
  assert(get_param_cb != NULL);
  assert(set_param_cb != NULL);
  assert(delete_param_cb != NULL);

  obj_p = malloc(sizeof(sterc_pscc_t));
  if(NULL == obj_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return (-1);
  }
  memset(obj_p,0,sizeof(sterc_pscc_t));
  obj_p->handle = ++sterc_pscc_handle;
  obj_p->parentid = parentid;
  obj_p->get_param_cb = get_param_cb;
  obj_p->set_param_cb = set_param_cb;
  obj_p->delete_param_cb = delete_param_cb;
  obj_p->connected_cb = NULL;
  obj_p->disconnected_cb = NULL;

  /* create ps connection object */
  if(create_pscc_connection(obj_p) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR, "failed create pscc object\n");
    free(obj_p);
    return (-1);
  }
  STERC_DBG_TRACE(LOG_DEBUG, "sterc_pscc allocated\n");
  sterc_pscc_change_state(obj_p,idle);
  mpl_list_add(&sterc_pscc_list_p,&obj_p->list_entry);
  return (obj_p->handle);
}

/*
 * sterc_pscc_connect
 **/
int sterc_pscc_connect(int handle,
                       sterc_pscc_connected_fp_t connected_cb,
                       sterc_pscc_disconnected_fp_t disconnected_cb)
{
  sterc_pscc_t *pscc_p;
  mpl_list_t *param_list_p=NULL;
  mpl_param_element_t *param_p,*tmp_param_p;
  pscc_auth_method_t auth_method = pscc_auth_method_none;

  assert(connected_cb != NULL);
  assert(disconnected_cb != NULL);

  pscc_p = find_pscc_obj_by_handle(handle);
  if(NULL == pscc_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "obj not found\n");
    return(-1);
  }

  /* store callbacks */
  pscc_p->connected_cb = connected_cb;
  pscc_p->disconnected_cb = disconnected_cb;

  /* fetch parameters from handler and convert to stepscc parameters */
  param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_static_ip,NULL);
  if((param_p != NULL) && (param_p->value_p != NULL))
  {
    tmp_param_p = mpl_param_element_clone(param_p);
    if(tmp_param_p == NULL)
      goto error_return;

    STERC_DBG_TRACE(LOG_DEBUG,"set static ip address\n");
    tmp_param_p->id = pscc_paramid_static_ip_address;
    mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
  }

  /* we dont need to set auth method if it is set to none */
  param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_auth_method,NULL);
  if((param_p != NULL) && (param_p->value_p != NULL) &&
    (*(pscc_auth_method_t*)param_p->value_p != pscc_auth_method_none))
  {
    auth_method = *(pscc_auth_method_t*)param_p->value_p;
    tmp_param_p = mpl_param_element_clone(param_p);
    if(tmp_param_p == NULL)
      goto error_return;

    STERC_DBG_TRACE(LOG_DEBUG,"set auth method\n");
    tmp_param_p->id = pscc_paramid_auth_method;
    mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
  }

  /* no need to set these parameters for auth method none */
  if(auth_method != pscc_auth_method_none)
  {
    param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_auth_uid,NULL);
    if((param_p != NULL) && (param_p->value_p != NULL))
    {
      tmp_param_p = mpl_param_element_clone(param_p);
      if(tmp_param_p == NULL)
        goto error_return;

      STERC_DBG_TRACE(LOG_DEBUG,"set auth username\n");
      tmp_param_p->id = pscc_paramid_auth_uid;
      mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
    }
    param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_auth_pwd,NULL);
    if((param_p != NULL) && (param_p->value_p != NULL))
    {
      tmp_param_p = mpl_param_element_clone(param_p);
      if(tmp_param_p == NULL)
        goto error_return;

      STERC_DBG_TRACE(LOG_DEBUG,"set auth password\n");
      tmp_param_p->id = pscc_paramid_auth_pwd;
      mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
    }
  }

  /* only need to set these parameters for auth method chap */
  if(auth_method == pscc_auth_method_chap)
  {
    param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_auth_chap_id,NULL);
    if((param_p != NULL) && (param_p->value_p != NULL))
    {
      tmp_param_p = mpl_param_element_clone(param_p);
      if(tmp_param_p == NULL)
        goto error_return;

      STERC_DBG_TRACE(LOG_DEBUG,"set auth chap id\n");
      tmp_param_p->id = pscc_paramid_auth_chap_id;
      mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
    }
    param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_auth_chap_challenge,NULL);
    if((param_p != NULL) && (param_p->value_p != NULL))
    {
      tmp_param_p = mpl_param_element_clone(param_p);
      if(tmp_param_p == NULL)
        goto error_return;

      STERC_DBG_TRACE(LOG_DEBUG,"set auth chap challenge\n");
      tmp_param_p->id = pscc_paramid_auth_chap_challenge;
      mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
    }
    param_p = pscc_p->get_param_cb(pscc_p->parentid,sterc_paramid_wan_auth_chap_response,NULL);
    if((param_p != NULL) && (param_p->value_p != NULL))
    {
      tmp_param_p = mpl_param_element_clone(param_p);
      if(tmp_param_p == NULL)
        goto error_return;

      STERC_DBG_TRACE(LOG_DEBUG,"set auth chap response\n");
      tmp_param_p->id = pscc_paramid_auth_chap_response;
      mpl_list_add(&param_list_p,&tmp_param_p->list_entry);
    }
  }

  set_pscc_parameters(pscc_p,param_list_p);
  mpl_param_list_destroy(&param_list_p);
  return connect_pscc(pscc_p);

  error_return:
  STERC_DBG_TRACE(LOG_ERR, "failed to set a parameter\n");
  mpl_param_list_destroy(&param_list_p);
  return(-1);
}

/**
 * sterc_pscc_disconnect
 **/
int sterc_pscc_disconnect(int handle)
{
  sterc_pscc_t *pscc_p;

  pscc_p = find_pscc_obj_by_handle(handle);
  if(NULL == pscc_p)
  {
    STERC_DBG_TRACE(LOG_ERR, "obj not found\n");
    return(-1);
  }
  return disconnect_pscc(pscc_p);
}

/**
 * sterc_pscc_destroy
 **/
int sterc_pscc_destroy(int handle)
{
  sterc_pscc_t *pscc_p;

  pscc_p = find_pscc_obj_by_handle(handle);
  if(NULL == pscc_p)
  {
    STERC_DBG_TRACE(LOG_DEBUG, "obj not found\n");
    return(-1);
  }
  return destroy_pscc(pscc_p);
}


/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * find_pscc_obj_by_handle()
 **/
static sterc_pscc_t *find_pscc_obj_by_handle(int handle)
{
  mpl_list_t *obj_p;
  sterc_pscc_t *pscc_p;

  MPL_LIST_FOR_EACH(sterc_pscc_list_p, obj_p)
  {
    pscc_p = MPL_LIST_CONTAINER(obj_p, sterc_pscc_t, list_entry);
    if(pscc_p->handle == handle)
      return (pscc_p);
  }
  return NULL;
}
/**
 * find_pscc_obj_by_connid()
 **/
static sterc_pscc_t *find_pscc_obj_by_connid(int connid)
{
  mpl_list_t *obj_p;
  sterc_pscc_t *pscc_p;

  MPL_LIST_FOR_EACH(sterc_pscc_list_p, obj_p)
  {
    pscc_p = MPL_LIST_CONTAINER(obj_p, sterc_pscc_t, list_entry);
    if(pscc_p->connid == connid)
      return (pscc_p);
  }
  return NULL;
}

/**
 * create_pscc_connection()
 **/
static int create_pscc_connection(sterc_pscc_t *obj_p)
{
  pscc_resp_create_t resp_create;
  int res=0;
  mpl_param_element_t *param_p;
  int cid=0;
  pscc_connection_status_t connection_status;

  assert(obj_p != NULL);
  assert(obj_p->get_param_cb != NULL);
  assert(obj_p->set_param_cb != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);
  assert(sterc_pscc_msg_p != NULL);

  sterc_pscc_msg_p->common.type = pscc_msgtype_req;
  sterc_pscc_msg_p->req_create.id = pscc_create;
  //sterc_pscc_msg_p->req_create.param_list_p = NULL;

  /* fetch cid parameter */
  param_p = obj_p->get_param_cb(obj_p->parentid,sterc_paramid_cid,NULL);
  if(param_p != NULL)
    cid = *((int*)param_p->value_p);

  /* add cid to req_create parameter list (using cid as connection id) */
  (void) mpl_add_param_to_list(&sterc_pscc_msg_p->req_create.param_list_p, pscc_paramid_connid,&cid);

  STERC_DBG_TRACE(LOG_DEBUG,"Sending create to server\n");
  if(pack_send_receive_unpack((mpl_msg_t *)sterc_pscc_msg_p,(mpl_msg_resp_t*)&resp_create) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Sending create to server failed..\n");
    mpl_param_list_destroy(&sterc_pscc_msg_p->req_create.param_list_p);
    return (-1);
  }

  mpl_param_list_destroy(&sterc_pscc_msg_p->req_create.param_list_p);

  switch(resp_create.result)
  {
  case pscc_result_ok:
    if(!MPL_PARAM_PRESENT_IN_LIST(pscc_paramid_connid, (resp_create.param_list_p)))
    {
      STERC_DBG_TRACE(LOG_ERR,"connid missing in response message\n");
      res = -1;
      goto free_resp_list_and_return;
    }

    obj_p->connid = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connid,(resp_create.param_list_p));

    /* created by my, remember cid */
    obj_p->created_by_me = true;
    /* store/update cid? */
    if(0 == cid)
    {
      cid = obj_p->connid;

      param_p = mpl_param_element_create(sterc_paramid_cid, &cid);
      if(NULL == param_p)
      {
        STERC_DBG_TRACE(LOG_ERR,"create param\n");
        res = -1;
        goto free_resp_list_and_return;
      }

      /* delete exisiting cid parameter */
      obj_p->delete_param_cb(obj_p->parentid,sterc_paramid_cid);
      /* set new cid */
      obj_p->set_param_cb(obj_p->parentid,param_p);
    }
    break;
  case pscc_result_failed_object_exists:
    /*not created by me cid ok */
    obj_p->connid = cid;
    /* check status of the connection object, it should be in the state disconnected */
    if(get_connection_status(obj_p,&connection_status) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR,"get connection status\n");
      res=-1;
      goto free_resp_list_and_return;
    }

    if(connection_status != pscc_connection_status_disconnected)
    {
      STERC_DBG_TRACE(LOG_ERR,"trying to use a connection that is not disconnected\n");
      res=-1;
      goto free_resp_list_and_return;
    }
    break;

  case pscc_result_failed_operation_not_allowed:
  case pscc_result_failed_object_not_found:
  case pscc_result_failed_parameter_not_found:
  case pscc_result_failed_unspecified:
  default:
    res = -1;
    break;
  }

  free_resp_list_and_return:
  mpl_param_list_destroy(&resp_create.param_list_p);
  return (res);
}

/**
 * get_connection_status()
 **/
static int get_connection_status(sterc_pscc_t *obj_p,pscc_connection_status_t *status_p)
{
  static mpl_list_t* param_list_p=NULL;
  const pscc_paramid_t reqparam[] = {pscc_paramid_connection_status};

  assert(obj_p != NULL);
  assert(status_p != NULL);

  param_list_p = get_pscc_parameters(obj_p,reqparam,1);   /* get only 1 parameter */
  if(NULL == param_list_p)
    return(-1);

  if(!MPL_PARAM_PRESENT_IN_LIST(pscc_paramid_connection_status, param_list_p))
  {
    STERC_DBG_TRACE(LOG_ERR,"connection status missing\n");
    return(-1);
  }
  *status_p = MPL_GET_PARAM_VALUE_FROM_LIST(int,pscc_paramid_connection_status,param_list_p);
  mpl_param_list_destroy(&param_list_p);
  return(0);
}

/**
 * connect_pscc
 **/
static int connect_pscc(sterc_pscc_t *obj_p)
{
  pscc_resp_connect_t resp_connect;
  int res=0;
  mpl_param_element_t *param_p;

  assert(obj_p != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);
  assert(sterc_pscc_msg_p != NULL);

  if(obj_p->state != sterc_pscc_state_idle)
  {
    STERC_DBG_TRACE(LOG_DEBUG, "unexpected state\n");
    return (-1);
  }

  sterc_pscc_msg_p->common.type = pscc_msgtype_req;
  sterc_pscc_msg_p->req_connect.id = pscc_connect;
  sterc_pscc_msg_p->req_connect.param_list_p = NULL;

  /* add connection id to req_connect parameter list */
  param_p = mpl_param_element_create(pscc_paramid_connid, &obj_p->connid);
  if(NULL == param_p)
  {
    STERC_DBG_TRACE(LOG_ERR,"create param\n");
    return (-1);
  }
  mpl_list_add(&sterc_pscc_msg_p->req_connect.param_list_p,
               &param_p->list_entry);

  STERC_DBG_TRACE(LOG_DEBUG,"Sending connect to server\n");
  if(pack_send_receive_unpack((mpl_msg_t *)sterc_pscc_msg_p,(mpl_msg_resp_t*)&resp_connect) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Sending connect to server failed..\n");
    goto free_req_list_and_return;
  }

  switch(resp_connect.result)
  {
  case pscc_result_ok:
    sterc_pscc_change_state(obj_p,connecting);
    break;
  default:
    res = -1;
    break;
  }

  mpl_param_list_destroy(&resp_connect.param_list_p);
  free_req_list_and_return:
  mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
  return (res);
}

/**
 * disconnect_pscc
 **/
static int disconnect_pscc(sterc_pscc_t *obj_p)
{
  pscc_resp_disconnect_t resp_disconnect;
  int res=0;
  mpl_param_element_t *param_p;

  assert(obj_p != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);
  assert(sterc_pscc_msg_p != NULL);

  switch(obj_p->state)
  {
  case sterc_pscc_state_connecting:
  case sterc_pscc_state_connected:
    break;
  default:
    STERC_DBG_TRACE(LOG_WARNING, "unexpected state\n");
    return(0);
  }

  sterc_pscc_msg_p->common.type = pscc_msgtype_req;
  sterc_pscc_msg_p->req_disconnect.id = pscc_disconnect;
  sterc_pscc_msg_p->req_disconnect.param_list_p = NULL;

  /* add connection id to req_disconnect parameter list */
  param_p = mpl_param_element_create(pscc_paramid_connid, &obj_p->connid);
  if(NULL == param_p)
  {
    STERC_DBG_TRACE(LOG_ERR,"create param\n");
    return (-1);
  }
  mpl_list_add(&sterc_pscc_msg_p->req_disconnect.param_list_p,
               &param_p->list_entry);

  STERC_DBG_TRACE(LOG_DEBUG,"Sending disconnect to server\n");
  if(pack_send_receive_unpack((mpl_msg_t *)sterc_pscc_msg_p,(mpl_msg_resp_t*)&resp_disconnect) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Sending disconnect to server failed..\n");
    goto free_req_list_and_return;
  }

  switch(resp_disconnect.result)
  {
  case pscc_result_ok:
    sterc_pscc_change_state(obj_p,disconnecting);
    break;
  default:
    res = -1;
    break;
  }

  mpl_param_list_destroy(&resp_disconnect.param_list_p);
  free_req_list_and_return:
  mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
  return (res);
}

/**
 * destroy_pscc
 **/
static int destroy_pscc(sterc_pscc_t *obj_p)
{
  int res=0;

  assert(obj_p != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);
  assert(sterc_pscc_msg_p != NULL);

  if (obj_p->state == sterc_pscc_state_disconnecting)
     sterc_pscc_change_state(obj_p,idle);

  switch(obj_p->state != sterc_pscc_state_idle)
  {
    STERC_DBG_TRACE(LOG_DEBUG, "unexpected state\n");
    return (-1);
  }

  /* pscc object created by me, go ahead and destroy it*/
  if(obj_p->created_by_me)
  {
    pscc_resp_destroy_t resp_destroy;
    mpl_param_element_t *param_p;

    sterc_pscc_msg_p->common.type = pscc_msgtype_req;
    sterc_pscc_msg_p->req_destroy.id = pscc_destroy;
    sterc_pscc_msg_p->req_destroy.param_list_p = NULL;

    /* add connection id to req_disconnect parameter list */
    param_p = mpl_param_element_create(pscc_paramid_connid, &obj_p->connid);
    if(NULL == param_p)
    {
      STERC_DBG_TRACE(LOG_ERR,"create param\n");
      res = -1;
      goto free_and_return;
    }
    mpl_list_add(&sterc_pscc_msg_p->req_destroy.param_list_p,
                 &param_p->list_entry);

    STERC_DBG_TRACE(LOG_ERR,"Sending destroy to server\n");
    if(pack_send_receive_unpack((mpl_msg_t *)sterc_pscc_msg_p,(mpl_msg_resp_t*)&resp_destroy) < 0)
    {
      STERC_DBG_TRACE(LOG_ERR,"Sending destroy to server failed..\n");
      goto free_req_list_and_return;
    }

    switch(resp_destroy.result)
    {
    case pscc_result_ok:
      break;
    default:
      res = -1;
      break;
    }

    mpl_param_list_destroy(&resp_destroy.param_list_p);
    free_req_list_and_return:
    mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
  }

  free_and_return:
  mpl_list_remove(&sterc_pscc_list_p,&obj_p->list_entry);
  free(obj_p);
  return (res);
}

/**
 * handle_event_connected
 **/
static void handle_event_connected(sterc_pscc_t *obj_p)
{
  mpl_list_t *param_list_p=NULL,*list_p,*tmp_p;
  mpl_param_element_t *param_p;

  assert(obj_p != NULL);
  assert(obj_p->connected_cb != NULL);
  assert(obj_p->set_param_cb != NULL);

  switch(obj_p->state)
  {
  case sterc_pscc_state_connecting:
    sterc_pscc_change_state(obj_p,connected);

    param_list_p = get_pscc_parameters(obj_p,sterc_pscc_req_param,sterc_pscc_num_req_param);
    MPL_LIST_FOR_EACH_SAFE(param_list_p, list_p, tmp_p)
    {
      param_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

      /* Move the new parameter from the returned list and convert to sterc parameters */
      if ((param_p != NULL) && (param_p->value_p != NULL))
      {
        (void)mpl_list_remove(&param_list_p, &param_p->list_entry);
        if(pscc_paramid_own_ip_address == param_p->id)
          param_p->id = sterc_paramid_wan_ip;
        else if (pscc_paramid_dns_address == param_p->id)
          param_p->id = sterc_paramid_wan_dns;
        else if (pscc_paramid_netdev_name == param_p->id)
          param_p->id = sterc_paramid_wan_device;
        else
        {
          mpl_param_element_destroy(param_p);
          continue;
        }

        if(obj_p->set_param_cb(obj_p->parentid, param_p) < 0)
        {
          mpl_param_element_destroy(param_p);
        }
      }
    }
    mpl_param_list_destroy(&param_list_p);
    obj_p->connected_cb(obj_p->parentid);
    break;
  default:
    break;
  }
  mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
}

static void handle_event_disconnected(sterc_pscc_t *obj_p)
{
  int i;
  assert(obj_p != NULL);
  assert(obj_p->disconnected_cb != NULL);
  assert(obj_p->delete_param_cb != NULL);

  switch(obj_p->state)
  {
  case sterc_pscc_state_connected:
  case sterc_pscc_state_disconnecting:
    /* delete parameters */
    for(i=0;i<sterc_pscc_num_req_param;i++)
      obj_p->delete_param_cb(obj_p->parentid,sterc_pscc_req_param[i]);
    /* fallthrough */
  case sterc_pscc_state_connecting:
    sterc_pscc_change_state(obj_p,idle);
    obj_p->disconnected_cb(obj_p->parentid);
    break;
  default:
    break;
  }
}

/**
 *set_pscc_parameters
 **/
static void set_pscc_parameters(sterc_pscc_t *obj_p,mpl_list_t *param_list_p)
{
  pscc_resp_set_t resp_set;
  mpl_param_element_t *param_p;
  mpl_list_t *list_p;

  assert(obj_p != NULL);
  assert(sterc_pscc_msg_p != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);

  /* anthing to set? */
  if(NULL == param_list_p)
    return;

  sterc_pscc_msg_p->common.type          = pscc_msgtype_req;
  sterc_pscc_msg_p->req_get.id           = pscc_set;
  sterc_pscc_msg_p->req_get.param_list_p = NULL;

  /* add connection id to req_get parameter list */
  param_p = mpl_param_element_create(pscc_paramid_connid, &obj_p->connid);
  if(NULL == param_p)
  {
    STERC_DBG_TRACE(LOG_ERR,"create param\n");
    goto free_req_list_and_return;
  }
  mpl_list_add(&sterc_pscc_msg_p->req_set.param_list_p,
               &param_p->list_entry);

  /* add parameters */
  MPL_LIST_FOR_EACH(param_list_p, list_p)
  {
    param_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    /* Move the parameter to the request list */
    if ((param_p != NULL) && (param_p->value_p != NULL))
    {
      mpl_param_element_t *tmp_param_p;
      tmp_param_p = mpl_param_element_clone(param_p);
      if(tmp_param_p != NULL)
        mpl_list_add(&sterc_pscc_msg_p->req_set.param_list_p,&tmp_param_p->list_entry);
    }
  }

  STERC_DBG_TRACE(LOG_DEBUG,"Send set to server\n");
  if(pack_send_receive_unpack((mpl_msg_t *)sterc_pscc_msg_p,(mpl_msg_resp_t*)&resp_set) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Sending set to server failed..\n");
    goto free_req_list_and_return;
  }

  switch(resp_set.result)
  {
  case pscc_result_ok:
    break;
  default:
    break;
  }

  mpl_param_list_destroy(&resp_set.param_list_p);
  free_req_list_and_return:
  mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
}

/**
 * get_pscc_parameters
 **/
static mpl_list_t* get_pscc_parameters(sterc_pscc_t *obj_p,const pscc_paramid_t *reqparam_p, int num_param)
{
  pscc_resp_get_t resp_get;
  mpl_param_element_t *param_p;
  int i;
  mpl_list_t *param_list_p=NULL,*list_p,*tmp_p;

  assert(obj_p != NULL);
  assert(obj_p->set_param_cb != NULL);
  assert(sterc_pscc_msg_p != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);
  assert(reqparam_p != NULL);

  sterc_pscc_msg_p->common.type          = pscc_msgtype_req;
  sterc_pscc_msg_p->req_get.id           = pscc_get;
  sterc_pscc_msg_p->req_get.param_list_p = NULL;

  /* add connection id to req_get parameter list */
  param_p = mpl_param_element_create(pscc_paramid_connid, &obj_p->connid);
  if(NULL == param_p)
  {
    STERC_DBG_TRACE(LOG_ERR,"create param\n");
    return NULL;
  }
  mpl_list_add(&sterc_pscc_msg_p->req_get.param_list_p,
               &param_p->list_entry);

  for(i=0;i<num_param;i++)
  {
    param_p = mpl_param_element_create_empty(reqparam_p[i]);
    if(NULL == param_p)
    {
      STERC_DBG_TRACE(LOG_ERR,"create param\n");
      goto free_req_list_and_return;
    }
    mpl_list_add(&sterc_pscc_msg_p->req_get.param_list_p,
                 &param_p->list_entry);
  }

  STERC_DBG_TRACE(LOG_DEBUG,"Sending get to server\n");
  if(pack_send_receive_unpack((mpl_msg_t *)sterc_pscc_msg_p,(mpl_msg_resp_t*)&resp_get) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Sending get to server failed..\n");
    goto free_req_list_and_return;
  }

  switch(resp_get.result)
  {
  case pscc_result_ok:
    /* move parameters from response parameter list to the list to be returned */
    MPL_LIST_FOR_EACH_SAFE(resp_get.param_list_p, list_p, tmp_p)
    {
      param_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

      /* Move the parameter from the response list and add to the new list */
      if ((param_p != NULL) && (param_p->value_p != NULL))
      {
        (void)mpl_list_remove(&resp_get.param_list_p, &param_p->list_entry);
        mpl_list_add(&param_list_p,
                     &param_p->list_entry);
      }
    }
    break;
  default:
    STERC_DBG_TRACE(LOG_ERR,"faild to get parameters\n");
    break;
  }

  mpl_param_list_destroy(&resp_get.param_list_p);
  free_req_list_and_return:
  mpl_param_list_destroy(&sterc_pscc_msg_p->common.param_list_p);
  return param_list_p;
}

static int pack_send_receive_unpack(mpl_msg_t *msg_p, mpl_msg_resp_t *resp_p)
{

  char *packed_request=NULL;
  size_t packed_request_len;
  int resp_len;

  assert(msg_p != NULL);
  assert(resp_p != NULL);
  assert(sterc_pscc_msg_packed_p != NULL);

  if (mpl_msg_pack(msg_p, &packed_request, &packed_request_len) != 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Command packing failed\n");
    return (-1);
  }

  resp_len = stec_sendsync(sterc_pscc_ctrl_p,
                           packed_request,
                           packed_request_len,
                           sterc_pscc_msg_packed_p,
                           STERC_PSCC_MAX_MSG_LEN,
                           3); /* wait max 3 seconds */

  if (resp_len <= 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Response from PS server failed (%d)\n",resp_len);
    goto error_free_req_buf_and_return;
  }

  /* unpack response */
  if (mpl_msg_unpack(sterc_pscc_msg_packed_p,resp_len,(mpl_msg_t *)resp_p) < 0)
  {
    STERC_DBG_TRACE(LOG_ERR,"Command unpacking failed\n");
    goto error_free_req_buf_and_return;
  }

  if (resp_p->type != pscc_msgtype_resp)
  {
    STERC_DBG_TRACE(LOG_ERR,"Unexpected message type received\n");
    goto error_free_resp_list_and_return;
  }

  STERC_DBG_TRACE(LOG_DEBUG,"received response: %s\n", pscc_msg_id_get_string(resp_p->id));
  if (resp_p->id != msg_p->common.id)
  {
    STERC_DBG_TRACE(LOG_ERR,"Unexpected response message received\n");
    goto error_free_resp_list_and_return;
  }

  mpl_msg_buf_free((mpl_msg_t*) packed_request);
  return (0);

  error_free_resp_list_and_return:
  mpl_param_list_destroy(&resp_p->param_list_p);
  error_free_req_buf_and_return:
  mpl_msg_buf_free((mpl_msg_t*) packed_request);
  return (-1);
}
