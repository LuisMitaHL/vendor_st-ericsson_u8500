/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PPP plugin for STERC
 */


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include <stddef.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "pppd.h"
#include "libstecom.h"
#include "mpl_msg.h"
#include "sterc_msg.h"
#include "chap-new.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define STERC_PPP_MAX_MSG_LEN 1000
#define STERC_CTRL_PATH "/dev/socket/sterc_ctrl_server"

#define send_receive(handle,request,request_len,resp_p,max_resp_len) \
  stec_sendsync((handle),(request),(request_len),(resp_p),(max_resp_len),100)

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static struct stec_ctrl *ctrl_handle_p = NULL;
static char *sterc_ppp_msg_packed_p = NULL;

char pppd_version[] = VERSION;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static void sterc_exit(void *opaque, int arg);
static void sterc_ip_up_notifier(void *opaque, int arg);
static void sterc_ip_down_notifier(void *opaque, int arg);
static void sterc_ip_choose(u_int32_t *addrp);
static int sterc_set_dns(u_int32_t *dns);
static int sterc_chap_verify(char *name, char *ourname, int id,
                             struct chap_digest_type *digest,
                             unsigned char *challenge, unsigned char *response,
                             char *message, int message_space);
static int sterc_pap_auth(char *user, char *passwd, char **msgp,
                          struct wordlist **paddrs,
                          struct wordlist **popts);

static int start_pdp_context(void);
static void send_ppp_sterc_event(sterc_msgid_t msgid);
/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * plugin_init()
 **/
void plugin_init(void)
{
  struct sockaddr_un ctrl_addr_un;

  //initiate message and parameter library
  sterc_init(NULL, NULL);

  //Register notifications
  add_notifier(&exitnotify,sterc_exit,NULL);
  add_notifier(&ip_up_notifier,sterc_ip_up_notifier,NULL);
  add_notifier(&ip_down_notifier,sterc_ip_down_notifier,NULL);

  //register hooks
  ip_choose_hook = sterc_ip_choose;
  dns_set_hook = sterc_set_dns;
  chap_verify_hook = sterc_chap_verify;
  //pap_auth_hook = sterc_pap_auth;

//create socket towards stercd
  memset(&ctrl_addr_un, 0, sizeof(ctrl_addr_un));
  ctrl_addr_un.sun_family = AF_UNIX;
  strncpy(ctrl_addr_un.sun_path, STERC_CTRL_PATH, UNIX_PATH_MAX) ;
  ctrl_handle_p = stec_open((struct sockaddr*) &ctrl_addr_un, sizeof(ctrl_addr_un));
  if (ctrl_handle_p == NULL) {
    dbglog("\nstec_open failed\n");
    return;
  }

  //Allocate default receive buffer
  sterc_ppp_msg_packed_p = malloc(STERC_PPP_MAX_MSG_LEN);
  if(NULL == sterc_ppp_msg_packed_p)
  {
    dbglog("malloc failed\n");
    return;
  }
  info("ppp_sterc initilized.");
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
static void sterc_exit(void *opaque, int arg)
{
  dbglog("***PPP Plugin notify sterc_exit %d\n",arg);

  //Close socket
  if (ctrl_handle_p)
    stec_close(ctrl_handle_p);

  if (sterc_ppp_msg_packed_p)
    free(sterc_ppp_msg_packed_p);

}


static void sterc_ip_up_notifier(void *opaque, int arg)
{
  dbglog("***PPP Plugin notify sterc_ip_up %d\n",arg);
  send_ppp_sterc_event(sterc_event_ppp_up);
}


static void sterc_ip_down_notifier(void *opaque, int arg)
{
  dbglog("***PPP Plugin notify sterc_ip_down %d\n",arg);
  send_ppp_sterc_event(sterc_event_ppp_down);
}


static void sterc_ip_choose(u_int32_t *addrp)
{
  mpl_msg_t *sterc_msg_p;
  char *packed_request_p = NULL;
  size_t packed_request_len;
  int resp_len;
  mpl_msg_t *resp_msg_p;
  char *ip_addr_p;
  u_int32_t ip_addr;
  int pid = getpid();

  dbglog("***PPP hook ip choose with address %d\n",*addrp);

  //First send signal to sterc to start the pdp context. Wait for response
  if (!start_pdp_context())
  {
    dbglog("Failed starting the pdp context\n");
    return;
  }

  //pdp context finished. Fetch IP address
  sterc_msg_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == sterc_msg_p)
  {
    dbglog("Failed allocating message\n");
    return;
  }

  sterc_msg_p->req.id = sterc_get;

  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_pppd_pid,&pid);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_lan_peer_ip,NULL);

  if (mpl_msg_pack(sterc_msg_p, &packed_request_p, &packed_request_len) != 0)
  {
    dbglog("Command packing failed");
    goto free_req_param_list_and_return;
  }

  resp_len = send_receive(ctrl_handle_p,
                          packed_request_p,
                          packed_request_len,
                          sterc_ppp_msg_packed_p,
                          STERC_PPP_MAX_MSG_LEN);

  if (resp_len <= 0)
  {
    dbglog("Response from STERC handler failed (%d)\n",resp_len);
    goto free_req_buf_and_return;
  }

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == resp_msg_p)
  {
    dbglog("Failed allocating message\n");
    goto free_req_param_list_and_return;
  }

  /* unpack response */
  if (mpl_msg_unpack(sterc_ppp_msg_packed_p,resp_len,resp_msg_p) < 0)
  {
    dbglog("Command unpacking failed\n");
    goto free_resp_msg_and_return;
  }

  if (resp_msg_p->common.type != sterc_msgtype_resp)
  {
    dbglog("Unexpected message type received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.id != sterc_get)
  {
    dbglog("Unexpected response message received\n");
    goto free_resp_param_list_and_return;
  }

  if(!MPL_PARAM_PRESENT_IN_LIST(sterc_paramid_lan_peer_ip, (resp_msg_p->resp.param_list_p)))
  {
    dbglog("ip address missing in response message\n");
    goto free_resp_param_list_and_return;
  }

  ip_addr_p = MPL_GET_PARAM_VALUE_REF_FROM_LIST(char*,sterc_paramid_lan_peer_ip,(resp_msg_p->resp.param_list_p));

  if (NULL == ip_addr_p)
  {
    dbglog("No ip address value in param list\n");
    goto free_resp_param_list_and_return;
  }

  if(inet_pton(AF_INET,ip_addr_p,&ip_addr) <= 0)
  {
    dbglog("inet_pton failed\n");
    goto free_resp_param_list_and_return;
  }

  //Set the address
  *addrp = ip_addr;
  dbglog("***PPP hook address set:%s\n",ip_addr_p);


  free_resp_msg_and_return:
  mpl_msg_free(resp_msg_p);
  free_resp_param_list_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp.param_list_p);
  free_req_buf_and_return:
  mpl_msg_buf_free(packed_request_p);
  free_req_param_list_and_return:
  mpl_param_list_destroy(&sterc_msg_p->req.param_list_p);
  mpl_msg_free(sterc_msg_p);

  return ;
}



static int sterc_chap_verify(char *name, char *ourname, int id,
                             struct chap_digest_type *digest,
                             unsigned char *challenge, unsigned char *response,
                             char *message, int message_space)
{
  mpl_msg_t *sterc_msg_p;
  char *packed_request_p = NULL;

  size_t packed_request_len;
  int resp_len;
  mpl_msg_t *resp_msg_p;
  int pid = getpid();
  int result = 0; //default failed
  sterc_wan_auth_method_t auth_method = sterc_wan_auth_method_chap;

  dbglog("========sterc_chap_verify=========\n");

  sterc_msg_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == sterc_msg_p)
  {
    dbglog("Failed allocating message\n");
    return 0;
  }

  sterc_msg_p->req.id = sterc_set;

  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_pppd_pid, &pid);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_method, &auth_method);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_uid, name);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_chap_id, &id);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_chap_challenge, challenge);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_chap_response, response);

  if (mpl_msg_pack(sterc_msg_p, &packed_request_p, &packed_request_len) != 0)
  {
    dbglog("Command packing failed");
    goto free_req_param_list_and_return;
  }
  resp_len = send_receive(ctrl_handle_p,
                          packed_request_p,
                          packed_request_len,
                          sterc_ppp_msg_packed_p,
                          STERC_PPP_MAX_MSG_LEN);

  if (resp_len <= 0)
  {
    dbglog("Response from STERC handler failed (%d)\n",resp_len);
    goto free_req_buf_and_return;
  }

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == resp_msg_p)
  {
    dbglog("Failed allocating message\n");
    goto free_req_param_list_and_return;
  }

  /* unpack response */
  if (mpl_msg_unpack(sterc_ppp_msg_packed_p,resp_len,resp_msg_p) < 0)
  {
    dbglog("Command unpacking failed\n");
    goto free_resp_msg_and_return;
  }

  if (resp_msg_p->common.type != sterc_msgtype_resp)
  {
    dbglog("Unexpected message type received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.id != sterc_set)
  {
    dbglog("Unexpected response message received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.result != sterc_result_ok)
  {
    dbglog("Setting PAP authentication parameters failed\n");
    goto free_resp_param_list_and_return;
  }

  //Chap challenge is valid
  result = 1;

  free_resp_msg_and_return:
  mpl_msg_free(resp_msg_p);
  free_resp_param_list_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp.param_list_p);
  free_req_buf_and_return:
  mpl_msg_buf_free(packed_request_p);
  free_req_param_list_and_return:
  mpl_param_list_destroy(&sterc_msg_p->req.param_list_p);
  mpl_msg_free(sterc_msg_p);

  //1 Valid
  //0 Not valid
  return result;
}



static int sterc_pap_auth(char *user, char *passwd, char **msgp,
                          struct wordlist **paddrs,
                          struct wordlist **popts)
{
  mpl_msg_t *sterc_msg_p;
  char *packed_request_p = NULL;
  size_t packed_request_len;
  int resp_len;
  mpl_msg_t *resp_msg_p;
  int pid = getpid();
  int result = 0; //default failed
  sterc_wan_auth_method_t auth_method = sterc_wan_auth_method_pap;

  dbglog("========sterc_pap_auth=========\n");

  sterc_msg_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == sterc_msg_p)
  {
    dbglog("Failed allocating message\n");
    return 0;
  }

  sterc_msg_p->req.id = sterc_set;

  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_pppd_pid,&pid);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_method,&auth_method);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_uid,user);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_wan_auth_pwd,passwd);

  if (mpl_msg_pack(sterc_msg_p, &packed_request_p, &packed_request_len) != 0)
  {
    dbglog("Command packing failed");
    goto free_req_param_list_and_return;
  }

  resp_len = send_receive(ctrl_handle_p,
                          packed_request_p,
                          packed_request_len,
                          sterc_ppp_msg_packed_p,
                          STERC_PPP_MAX_MSG_LEN);

  if (resp_len <= 0)
  {
    dbglog("Response from STERC handler failed (%d)\n",resp_len);
    goto free_req_buf_and_return;
  }

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == resp_msg_p)
  {
    dbglog("Failed allocating message\n");
    goto free_req_param_list_and_return;
  }

  /* unpack response */
  if (mpl_msg_unpack(sterc_ppp_msg_packed_p,resp_len,resp_msg_p) < 0)
  {
    dbglog("Command unpacking failed\n");
    goto free_resp_msg_and_return;
  }

  if (resp_msg_p->common.type != sterc_msgtype_resp)
  {
    dbglog("Unexpected message type received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.id != sterc_set)
  {
    dbglog("Unexpected response message received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.result != sterc_result_ok)
  {
    dbglog("Setting PAP authentication parameters failed\n");
    goto free_resp_param_list_and_return;
  }

  //username/password ok
  result = 1;

  free_resp_msg_and_return:
  mpl_msg_free(resp_msg_p);
  free_resp_param_list_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp.param_list_p);
  free_req_buf_and_return:
  mpl_msg_buf_free(packed_request_p);
  free_req_param_list_and_return:
  mpl_param_list_destroy(&sterc_msg_p->req.param_list_p);
  mpl_msg_free(sterc_msg_p);

  //1 username password ok
  //0 username/password NOT ok
  //-1 ppp will check the pap-secrets files as normally
  return result;
}




static void send_ppp_sterc_event(sterc_msgid_t msgid)
{
  int pid = getpid();
  mpl_msg_t *ppp_event_p;
  char *packed_ppp_event=NULL;
  size_t packed_ppp_event_len;


  ppp_event_p = mpl_event_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == ppp_event_p)
  {
    dbglog("Failed allocating message\n");
    return;
  }

  ppp_event_p->event.id = msgid;

  (void) mpl_add_param_to_list(&ppp_event_p->event.param_list_p, sterc_paramid_pppd_pid,&pid);

  switch (msgid)
  {
  case sterc_event_ppp_up:
    (void) mpl_add_param_to_list(&ppp_event_p->event.param_list_p, sterc_paramid_lan_device,ifname);
    break;
  default:
    break;
  }

  if (mpl_msg_pack(ppp_event_p, &packed_ppp_event, &packed_ppp_event_len) != 0)
  {
    dbglog("Command packing failed");
    goto free_event_list_and_return;
  }

  if (stec_send(ctrl_handle_p,packed_ppp_event,packed_ppp_event_len) < 0)
  {
    dbglog("Sending ppp event failed");
    goto free_event_buf_and_return;
  }

  free_event_buf_and_return:
  mpl_msg_buf_free(packed_ppp_event);
  free_event_list_and_return:
  mpl_param_list_destroy(&ppp_event_p->event.param_list_p);
  mpl_msg_free(ppp_event_p);

}

static int sterc_set_dns(u_int32_t *dns)
{
	int value_set = 0;
  mpl_msg_t *sterc_msg_p;
  char *packed_request_p = NULL;
  size_t packed_request_len;
  int resp_len;
  mpl_msg_t *resp_msg_p;
  u_int32_t dns_addr;
  int pid = getpid();
  mpl_list_t *list_p,*tmp_p;
  mpl_param_element_t *param_elem_p;

  dbglog("***PPP hook set dns addresses\n");

  sterc_msg_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == sterc_msg_p)
  {
    dbglog("Failed allocating message\n");
    return 0;
  }

  sterc_msg_p->req.id = sterc_get;

  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_pppd_pid,&pid);
  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_lan_dns,NULL);

  if (mpl_msg_pack(sterc_msg_p, &packed_request_p, &packed_request_len) != 0)
  {
    dbglog("Command packing failed");
    goto free_req_param_list_and_return;
  }

  resp_len = send_receive(ctrl_handle_p,
                          packed_request_p,
                          packed_request_len,
                          sterc_ppp_msg_packed_p,
                          STERC_PPP_MAX_MSG_LEN);

  if (resp_len <= 0)
  {
    dbglog("Response from STERC handler failed (%d)\n",resp_len);
    goto free_req_buf_and_return;
  }

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == resp_msg_p)
  {
    dbglog("Failed allocating message\n");
    goto free_req_param_list_and_return;
  }

  /* unpack response */
  if (mpl_msg_unpack(sterc_ppp_msg_packed_p,resp_len,resp_msg_p) < 0)
  {
    dbglog("Command unpacking failed\n");
    goto free_resp_msg_and_return;
  }

  if (resp_msg_p->common.type != sterc_msgtype_resp)
  {
    dbglog("Unexpected message type received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.id != sterc_get)
  {
    dbglog("Unexpected response message received\n");
    goto free_resp_param_list_and_return;
  }



  //Loop through params
  MPL_LIST_FOR_EACH_SAFE(resp_msg_p->resp.param_list_p, list_p, tmp_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);

    if (NULL != param_elem_p && NULL != param_elem_p->value_p && sterc_paramid_lan_dns == param_elem_p->id)
    {
      dbglog("**** adding dns address %s\n",(char*)param_elem_p->value_p);
      if(inet_pton(AF_INET,param_elem_p->value_p,&dns_addr) <= 0)
      {
        dbglog("inet_pton failed\n");
        goto free_resp_param_list_and_return;
      }
      value_set = 1;
      *dns = dns_addr;
      dns++;
    }
  }

  free_resp_msg_and_return:
  mpl_msg_free(resp_msg_p);
  free_resp_param_list_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp.param_list_p);
  free_req_buf_and_return:
  mpl_msg_buf_free(packed_request_p);
  free_req_param_list_and_return:
  mpl_param_list_destroy(&sterc_msg_p->req.param_list_p);
  mpl_msg_free(sterc_msg_p);


  return value_set;
}


static int start_pdp_context(void)
{
  int result = 0;
  mpl_msg_t *sterc_msg_p;
  char *packed_request_p = NULL;
  size_t packed_request_len;
  int resp_len;
  mpl_msg_t *resp_msg_p;
  int pid = getpid();
  struct sockaddr_un sock_addr;

  sterc_msg_p = mpl_req_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == sterc_msg_p)
  {
    dbglog("Failed allocating message\n");
    return 0;
  }

  sterc_msg_p->req.id = sterc_connect_wan;

  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_pppd_pid,&pid);


  if (stec_get_local_addr(ctrl_handle_p, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
  {
    dbglog("Failed fetching socket address\n");
    goto free_req_param_list_and_return;
  }

  (void) mpl_add_param_to_list(&sterc_msg_p->req.param_list_p, sterc_paramid_socket_addr,sock_addr.sun_path);


  if (mpl_msg_pack(sterc_msg_p, &packed_request_p, &packed_request_len) != 0)
  {
    dbglog("Command packing failed");
    goto free_req_param_list_and_return;
  }

  resp_len = send_receive(ctrl_handle_p,
                          packed_request_p,
                          packed_request_len,
                          sterc_ppp_msg_packed_p,
                          STERC_PPP_MAX_MSG_LEN);

  if (resp_len <= 0)
  {
    dbglog("Response from STERC handler failed (%d)\n",resp_len);
    goto free_req_buf_and_return;
  }

  resp_msg_p = mpl_resp_msg_alloc(STERC_PARAM_SET_ID);
  if (NULL == resp_msg_p)
  {
    dbglog("Failed allocating message\n");
    goto free_req_param_list_and_return;
  }

  /* unpack response */
  if (mpl_msg_unpack(sterc_ppp_msg_packed_p,resp_len,resp_msg_p) < 0)
  {
    dbglog("Command unpacking failed\n");
    goto free_resp_msg_and_return;
  }

  if (resp_msg_p->common.type != sterc_msgtype_resp)
  {
    dbglog("Unexpected message type received\n");
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.id != sterc_connect_wan)
  {
    dbglog("Unexpected response message received, %d\n",resp_msg_p->resp.id);
    goto free_resp_param_list_and_return;
  }

  if (resp_msg_p->resp.result != sterc_result_ok)
  {
    dbglog("Unexpected result %d\n",resp_msg_p->resp.result);
    goto free_resp_param_list_and_return;
  }

  //Alles in ordnung
  result = 1;

  free_resp_msg_and_return:
  mpl_msg_free(resp_msg_p);
  free_resp_param_list_and_return:
  mpl_param_list_destroy(&resp_msg_p->resp.param_list_p);
  free_req_buf_and_return:
  mpl_msg_buf_free(packed_request_p);
  free_req_param_list_and_return:
  mpl_param_list_destroy(&sterc_msg_p->req.param_list_p);
  mpl_msg_free(sterc_msg_p);

  return result;
}
