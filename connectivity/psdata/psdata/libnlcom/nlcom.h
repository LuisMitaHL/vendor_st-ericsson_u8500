/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  netlink communication API
 */

#ifndef NLCOM_H
#define NLCOM_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include "mpl_param.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

struct nlcom_handle;


enum nlcom_status{
  NLCOM_STATUS_CREATED,  //Created or if_down
  NLCOM_STATUS_DELETED,
  NLCOM_STATUS_IF_UP,
  NLCOM_STATUS_NEW_ADDR,
  NLCOM_STATUS_DEL_ADDR
};

/*
  TODO: This should be fetched from if_caif.h when its available
*/
enum ifla_caif {
  __IFLA_CAIF_UNSPEC,
  IFLA_CAIF_IPV4_CONNID,
  IFLA_CAIF_IPV6_CONNID,
  IFLA_CAIF_LOOPBACK,
  __IFLA_CAIF_MAX
};
#define IFLA_CAIF_MAX (__IFLA_CAIF_MAX-1)



/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * nlcom_init
 *
 * @user_p -
 * @log_fp - Logging function to be used
 *
 * Returns: none
 *
 **/
void  nlcom_init(void* user_p, mpl_log_fp log_fp);

/**
 * nlcom_deinit
 *
 * Returns: none
 *
 **/
void  nlcom_deinit(void);


/**
 * nlcom_open
 *
 * @subscription - Netlink events to subscribe for
 *
 * Returns: handler to a netlink struct
 *
 **/
struct nlcom_handle * nlcom_open(int subscription);


/**
 * nlcom_close
 *
 * @handle - handler
 *
 * Returns: none
 *
 **/
void nlcom_close(struct nlcom_handle *handle);


/**
 * nlcom_get_fd
 *
 * @handle - handler
 *
 * Returns: netlink file descriptor
 *
 **/
int nlcom_get_fd(struct nlcom_handle *handle);


/**
 * nlcom_receive_event
 *
 * @handle - handler
 * @devid - device id. Must be allocated (out). Optional (NULL if not needed)
 * @ifindex - interface index (out)
 * @status - status
 *
 * Returns: (-1) if error, (0) if ok
 *
 **/
int nlcom_receive_event(struct nlcom_handle *handle, char *devid, int* ifindex, int *status);


/**
 * nlcom_get_if_status
 *
 * @handle - handler
 * @devid - device id. 
 * @status - status (CREATED or IF_UP)
 *
 * Returns: (-1) if error or not found, (0) if ok
 *
 **/
int nlcom_get_if_status(struct nlcom_handle *handle, char *devid, int *status);


/**
 * nlcom_create_if (currently only supports CAIF IF)
 *
 * @handle - handler
 *
 * Returns: (-1) if error (0) if ok
 *
 **/
int nlcom_create_if(struct nlcom_handle *handle, int connection_type, int nsapi, bool loop_enabled, char *ifname_set, char *ifname,size_t ifname_len, int *ifindex);

/**
 * nlcom_destroy_if
 *
 * @handle - handler
 * @ifindex - if to destroy
 *
 * Returns: (-1) if error (0) if ok
 *
 **/
int nlcom_destroy_if(struct nlcom_handle *handle, int ifindex);

#endif //NLCOM_H
