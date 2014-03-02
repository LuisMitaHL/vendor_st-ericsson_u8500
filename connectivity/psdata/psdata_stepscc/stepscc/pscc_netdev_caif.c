/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control Net Device - Caif
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
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include "pscc_msg.h"
#include "mpl_list.h"
#include "pscc_object.h"
#include "pscc_netdev.h"
#include "psccd.h"
#include "nlcom.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/


typedef struct netlink_entry_s
{
  mpl_list_t list_entry;

  /* pscc handler specific context data */
  int                          connid;
  pscc_get_param_fp_t        param_get_cb;
  pscc_set_param_fp_t        param_set_cb;
  pscc_delete_param_fp_t     param_delete_cb;
  pscc_netdev_destroyed_fp_t netdev_destroyed_cb;

  /* Caif net link instance specific context data */
  int                        if_index;
}netlink_entry_t;

typedef struct
{
  mpl_list_t* list_p;

  /* Net link generic context data */
  struct nlcom_handle *rth;
}pscc_netdev_context_t;

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static pscc_netdev_context_t *pscc_netdev_ctx_p = NULL;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static netlink_entry_t* pscc_netdev_find(void* context_p, int dev_handle);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * pscc_netdev_init()
 **/
int pscc_netdev_init(void)
{
  int subscription = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

  if (NULL != pscc_netdev_ctx_p)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "pscc netdev already initialized\n");
    return -1;
  }

  nlcom_init(NULL,&psccd_log_func);

  pscc_netdev_ctx_p = malloc(sizeof(pscc_netdev_context_t)) ;
  if (NULL == pscc_netdev_ctx_p)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "could not allocate pscc_netdev_ctx\n");
    return -1;
  }

  memset(pscc_netdev_ctx_p,0, sizeof(pscc_netdev_context_t));
  
  pscc_netdev_ctx_p->rth = nlcom_open(subscription);
  if(NULL ==  pscc_netdev_ctx_p->rth)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "nlcom_open failed\n");
    return -1;
  }
  //Return file descriptor
  return nlcom_get_fd(pscc_netdev_ctx_p->rth);
}

/**
 * pscc_netdev_shutdown()
 **/
void pscc_netdev_shutdown(void)
{
  mpl_list_t *obj_p, *tmp_p;
  netlink_entry_t *nl_entry_p;

  if (NULL == pscc_netdev_ctx_p)
    return;
  

  // Tear down any remaining netlink instances (destroy)
  MPL_LIST_FOR_EACH_SAFE(pscc_netdev_ctx_p->list_p, obj_p, tmp_p)
  {
    nl_entry_p = MPL_LIST_CONTAINER(obj_p, netlink_entry_t, list_entry);
    if (nlcom_destroy_if(pscc_netdev_ctx_p->rth, nl_entry_p->if_index) < 0)
    {
      PSCC_DBG_TRACE(LOG_WARNING, "failed destroying netlink instance, continues...\n");
    }
    mpl_list_remove(&pscc_netdev_ctx_p->list_p,&nl_entry_p->list_entry);
    free(nl_entry_p);
  }
  // Close the netlink handler
  nlcom_close(pscc_netdev_ctx_p->rth);
  nlcom_deinit();
  // Free generic net device context
  free(pscc_netdev_ctx_p);
  pscc_netdev_ctx_p = NULL;
}


/**
 * pscc_netdev_handle_message()
 **/
int pscc_netdev_handle_message(int fd)
{
  int ifindex=0;
  int status=0;
  netlink_entry_t* netlink_entry_p;

  assert(pscc_netdev_ctx_p != NULL);

  PSCC_IDENTIFIER_NOT_USED(fd);

  if (nlcom_receive_event(pscc_netdev_ctx_p->rth, NULL, &ifindex, &status) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "handle_netlink_event failed\n");
    return (-1);
  }
  // find list entry
  netlink_entry_p =  pscc_netdev_find(pscc_netdev_ctx_p,ifindex);

  /*We are only interested in caif interfaces and the deleted event */
  if (NULL != netlink_entry_p && NULL != netlink_entry_p->netdev_destroyed_cb)
  {
    switch (status)
    {
    case NLCOM_STATUS_DELETED:
      PSCC_DBG_TRACE(LOG_DEBUG, "sending netdev_destroyed_cb for connid %d\n", netlink_entry_p->connid);
      netlink_entry_p->netdev_destroyed_cb(netlink_entry_p->connid);
      break;
    case NLCOM_STATUS_CREATED:
      PSCC_DBG_TRACE(LOG_DEBUG, "current status of caif for connid %d is CREATED\n", netlink_entry_p->connid);
      break;
    case NLCOM_STATUS_IF_UP:
      PSCC_DBG_TRACE(LOG_DEBUG, "current status of caif for connid %d is IF_UP\n", netlink_entry_p->connid);
      break;
    case NLCOM_STATUS_NEW_ADDR:
      PSCC_DBG_TRACE(LOG_DEBUG, "current status of caif for connid %d is NEW_ADDR\n", netlink_entry_p->connid);
      break;
    case NLCOM_STATUS_DEL_ADDR:
      PSCC_DBG_TRACE(LOG_DEBUG, "current status of caif for connid %d is DEL_ADDR\n", netlink_entry_p->connid);
      break;
    default:
      PSCC_DBG_TRACE(LOG_DEBUG, "Unknown status of caif for connid %d\n", netlink_entry_p->connid);
      break;
    }
  }

  return 0;
}


/**
 * pscc_netdev_create()
 **/
int pscc_netdev_create(
      int                         connid,
      pscc_get_param_fp_t         param_get_cb,
      pscc_set_param_fp_t         param_set_cb,
      pscc_delete_param_fp_t      param_delete_cb,
      pscc_netdev_destroyed_fp_t  netdev_destroyed_cb)
{
  netlink_entry_t* nl_entry_p = NULL; // New configuration for this nl instance
  mpl_param_element_t* param_p = NULL; // parameter fetch from bearer
  enum ifla_caif ifla;
  bool loop_enabled=false;
  int nsapi;
  char* if_name_prefix_p = NULL;
  char  if_name[IFNAMSIZ];
  pscc_pdp_type_t pdp_type;

  assert(pscc_netdev_ctx_p != NULL);
  assert(param_get_cb!= NULL);
  assert(param_set_cb != NULL);
  assert(param_delete_cb != NULL);
  assert(netdev_destroyed_cb != NULL);

// create instance data
  nl_entry_p = malloc(sizeof(netlink_entry_t));
  if (NULL == nl_entry_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    return (-1);
  }
  memset(nl_entry_p,0,sizeof(netlink_entry_t));

  // Store parameters:
  nl_entry_p->if_index            = -1;
  nl_entry_p->connid              = connid;
  nl_entry_p->param_get_cb        = param_get_cb;
  nl_entry_p->param_set_cb        = param_set_cb;
  nl_entry_p->param_delete_cb     = param_delete_cb;
  nl_entry_p->netdev_destroyed_cb = netdev_destroyed_cb;

  // Check if what kind of interface is requested
  // Fetch < loopback >

  param_p = param_get_cb(connid, pscc_paramid_loopback);
  if((NULL != param_p))
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "will create caif loopback interface\n");
    loop_enabled = MPL_GET_VALUE_FROM_PARAM_ELEMENT(bool,param_p);
  }
  // Fetch < PDP type >
  param_p = param_get_cb(connid, pscc_paramid_pdp_type);
  if(NULL == param_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not find pdp type parameter\n");
    goto error;
  }
  pdp_type = MPL_GET_VALUE_FROM_PARAM_ELEMENT(pscc_pdp_type_t,param_p);

  switch(pdp_type)
  {
  case pscc_pdp_type_ipv4:
    ifla = IFLA_CAIF_IPV4_CONNID;
    break;
  case pscc_pdp_type_ipv6:
    ifla = IFLA_CAIF_IPV6_CONNID;
    break;
  default:
    PSCC_DBG_TRACE(LOG_WARNING, "unsupported pdp type, using IPv4\n");
    ifla = IFLA_CAIF_IPV4_CONNID;
    break;
  }

  // Fetch < nsapi >
  param_p = param_get_cb(connid, pscc_paramid_nsapi);
  if(NULL == param_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not find nsapi parameter\n");
    goto error;
  }
  nsapi = MPL_GET_VALUE_FROM_PARAM_ELEMENT(int,param_p);

  // Fetch the prefix
  param_p = param_get_cb(connid, pscc_paramid_netdev_name_prefix);
  if(NULL == param_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "could not find netdev name prefix parameter\n");
    goto error;
  }
  if_name_prefix_p = MPL_GET_VALUE_REF_FROM_PARAM_ELEMENT(char*,param_p);
  PSCC_DBG_TRACE(LOG_INFO, "if_name_prefix_p %s\n",if_name_prefix_p);

  // Create netlink interface instance
  if (nlcom_create_if(pscc_netdev_ctx_p->rth, ifla, nsapi, loop_enabled, if_name_prefix_p, if_name, IFNAMSIZ, &nl_entry_p->if_index) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed creating caif interface\n");
    goto error;
  }

  // Set name in parameter list for connection
  param_p = mpl_param_element_create_stringn(pscc_paramid_netdev_name,if_name,strlen(if_name));
  if (NULL == param_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed allocating memory\n");
    goto error;
  }

  if (param_set_cb(connid, param_p) < 0)
  {
    PSCC_DBG_TRACE(LOG_ERR, "failed to set the interface name to pscc\n");
    free(param_p);
    goto error;
  }

  // add entry to list and return interface handle
  mpl_list_add(&pscc_netdev_ctx_p->list_p, &nl_entry_p->list_entry);
  return (nl_entry_p->if_index);

error:
  // If instance is created remove it..
  if (nl_entry_p->if_index > -1)
  {
    if (nlcom_destroy_if(pscc_netdev_ctx_p->rth, nl_entry_p->if_index) < 0)
    {
      PSCC_DBG_TRACE(LOG_WARNING, "failed to clean up interface during error handling\n");
    }
  }

  free(nl_entry_p);
  return (-1);
}


/**
 *  pscc_netdev_destroy()
 **/
int pscc_netdev_destroy(int dev_handle)
{
  netlink_entry_t *nl_entry_p;

  if (NULL == pscc_netdev_ctx_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "netdev_destroy called without any initialized netlink\n");
    return (-1);
  }

  // Find the right netlink instance
  nl_entry_p = pscc_netdev_find(pscc_netdev_ctx_p, dev_handle);
  if(NULL == nl_entry_p)
  {
    PSCC_DBG_TRACE(LOG_DEBUG, "Could not find instance to destroy, just return ok\n");
    return 0;
  }

  // Reset destroy cb before destroying the if, just to be sure...
  nl_entry_p->netdev_destroyed_cb = NULL;

  // Destroy Caif interface
  if(nlcom_destroy_if(pscc_netdev_ctx_p->rth, nl_entry_p->if_index) < 0)
  {
    PSCC_DBG_TRACE(LOG_WARNING, "Can't destroy CAIF interface with id %d, %d continues..\n",nl_entry_p->if_index,dev_handle);
  }

  nl_entry_p->param_delete_cb(nl_entry_p->connid, pscc_paramid_netdev_name);

  // Remove entry from interface list
  mpl_list_remove(&pscc_netdev_ctx_p->list_p,&nl_entry_p->list_entry);
  free(nl_entry_p);

  return 0;
}

/*******************************************************************************
 *
 * Private/Static Functions
 *
 *******************************************************************************/
/**
 *  pscc_netdev_find()
 **/
static netlink_entry_t* pscc_netdev_find(void* context_p, int dev_handle)
{
  mpl_list_t *obj_p;
  netlink_entry_t *nl_entry_p;
  pscc_netdev_context_t* ctx_p = (pscc_netdev_context_t*)context_p;

  if (NULL == context_p)
  {
    PSCC_DBG_TRACE(LOG_ERR, "called with invalid parameters\n");
    return NULL;
  }

  MPL_LIST_FOR_EACH(ctx_p->list_p, obj_p)
  {
    nl_entry_p = MPL_LIST_CONTAINER(obj_p, netlink_entry_t, list_entry);
    if (nl_entry_p->if_index == dev_handle)
      return nl_entry_p;
  }
  return NULL;
}



