/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  netlink communication
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
#include "nlcom.h"


/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define  NLCOM_IDENTIFIER_NOT_USED(param) (void)(param)

/* So far we only use level LOG_DEBUG from this library.
     * The arguments are the following:
     * - It is caller of the lib that has to find out if a failure is really critical or not
     *   I.e. has ta add another 'level' of logging
     * - We do not want to populate the log file with lots of errors/warnings
     *   that is really unimportant.
     */
#define DBG_TRACE(format, ...) \
  if (NULL != nlcom_log_func) \
    (*nlcom_log_func)(nlcom_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

/*
  FIXME: these defines are missing when compiling for android,
  remove when available!!
 */
#ifndef IFLA_LINKINFO
#warning "IFLA_LINKINFO not defined, temporarily defining my own. please check the if_link.h file!" 
#define IFLA_LINKINFO  18
#define IFLA_INFO_KIND 1
#define IFLA_INFO_DATA 2
#endif



#define NLCOM_MAX_BUF_SIZE 16384


#define NLMSG_TAIL(nmsg) \
  ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

struct netlink_handle
{
  int                   fd;
  struct sockaddr_nl    local;
  __u32                 seq;
};

struct iplink_req {
  struct nlmsghdr       n;
  struct ifinfomsg      i;
  char                  buf[1024];
};

struct dump_req{
  struct nlmsghdr n;
  struct rtgenmsg g;
};

struct nlcom_handle
{
  struct netlink_handle event_handle;
  struct netlink_handle ctrl_handle;
};

  


/*Function prototype used when parsing the received data from the netlink socket*/
typedef int (*netlink_parse_fp_t)(struct nlmsghdr *n, void *param1, void *param2, void *param3);


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_log_fp nlcom_log_func = NULL;
static void* nlcom_log_user_p = NULL;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static int netlink_open(struct netlink_handle *rth, unsigned int subscriptions);
static int netlink_close(struct netlink_handle *rth);
static int send_netlink_request(struct netlink_handle *handler, struct nlmsghdr *hdr);
static int netlink_send_dump_request(struct netlink_handle *nh);
static int netlink_recv_message(struct netlink_handle *nh,netlink_parse_fp_t parsefunc_cb,void *param1, void *param2, void *param3, bool event);
static int add_attribute(struct nlmsghdr *n, int maxlen, int type, const void *data, int datalen);

/*CAIF spesific functions (To be updated/renamed)*/
static int create_caif_interface(struct netlink_handle *nh, int connection_type, int nsapi, bool loop_enabled, char *ifname_set, char *ifname,size_t ifname_len, int *ifindex);
static int destroy_caif_interface(struct netlink_handle *nh, int ifindex);
static int get_caif_interface_id(struct netlink_handle *nh, char *ifname, int *ifindex);
static int set_caif_interface_id(struct netlink_handle *nh, int ifindex, char *ifname);
static int get_caif_interface_suffix(struct netlink_handle *nh, char *iftype, int *ifsuffix);


/*parsing functions*/
static int parse_data_get_last_created_if(struct nlmsghdr *n, void *param1, void *param2, void *param3);
static int parse_data_get_suffix(struct nlmsghdr *n, void *param1, void *param2, void *param3);
static int parse_data_get_status_from_ifindex(struct nlmsghdr *n, void *param1, void *param2, void *param3);
static int parse_data_get_status_from_devid(struct nlmsghdr *n, void *param1, void *param2, void *param3);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * nlcom_init()
 **/
void nlcom_init(void* user_p, mpl_log_fp log_fp)
{
  nlcom_log_user_p = user_p;
  nlcom_log_func = log_fp;
}
 

/**
 * nlcom_deinit()
 **/
void nlcom_deinit(void)
{
  nlcom_log_user_p = NULL;
  nlcom_log_func = NULL; 
}


/**
 * nlcom_open()
 **/
struct nlcom_handle * nlcom_open(int subscription)
{
  struct nlcom_handle * handle = NULL;
  
  // Allocate Context Data for the generic netlink interface
  handle = malloc(sizeof(struct nlcom_handle));
  if(NULL == handle)
  {
    DBG_TRACE("failed allocating memory for Net Device context\n");
    return NULL;
  }
  memset(handle,0,sizeof(struct nlcom_handle));

  if (0 == subscription)
    DBG_TRACE("nothing has been subscribed for nlcomlib\n");

  //DBG_TRACE("subscription is %d\n",subscription);
  
  // Netlink initialization
  if (netlink_open(&handle->event_handle, subscription) < 0)
  {
    DBG_TRACE("rtnl_open failed\n");
    free(handle);
    return NULL;
  }
  if (netlink_open(&handle->ctrl_handle, 0) < 0)
  {
    DBG_TRACE("rtnl_open failed\n");
    free(handle);
    return NULL;
  }

  //Return file descriptor
  return handle;
}


/**
 * nlcom_close()
 **/
void nlcom_close(struct nlcom_handle *handle)
{
  assert(handle != NULL);
  
  if(0 != netlink_close(&handle->event_handle))
  {
    DBG_TRACE("fail to close the net link file descriptor\n");
  }
  if(0 != netlink_close(&handle->ctrl_handle))
  {
    DBG_TRACE("fail to close the net link file descriptor\n");
  }
  free(handle);
}


/**
 * nlcom_get_fd()
 **/
int nlcom_get_fd(struct nlcom_handle *handle)
{
  assert(handle != NULL);

  return handle->event_handle.fd;
}


/**
 * nlcom_receive_event()
 **/
int nlcom_receive_event(struct nlcom_handle *handle, char * devid, int *ifindex, int *status)
{
  assert(handle != NULL);
  assert(status != NULL);
  
  *ifindex = -1;
  *status = -1;

  if (netlink_recv_message(&handle->event_handle, parse_data_get_status_from_ifindex, (void*)ifindex, (void*)status, (void*)devid,true) < 0) 
  {
    DBG_TRACE("Netlink receive failed\n");
    return -1;
  }
/*
  if (NULL != devid)
  {
    if (netlink_send_dump_request(&handle->ctrl_handle) < 0)
    {
      DBG_TRACE("Netlink send dump message failed\n");
      return -1;
    }

  
    if (netlink_recv_message(&handle->ctrl_handle, parse_data_get_devid, (void*)ifindex, (void*)devid, false) < 0) 
    {
      DBG_TRACE("Netlink receive failed\n");
      return -1;
    }
  }
*/
  return 0;
}


/**
 * nlcom_get_if_status()
 **/
int nlcom_get_if_status(struct nlcom_handle *handle, char *devid, int *status)
{
  assert(handle != NULL);
  assert(devid != NULL);
  assert(status != NULL);

  if (netlink_send_dump_request(&handle->ctrl_handle) < 0)
  {
    DBG_TRACE("Netlink send dump message failed\n");
    return -1;
  }

  *status = -1;
  
  if (netlink_recv_message(&handle->ctrl_handle, parse_data_get_status_from_devid, (void*)devid, (void*)status,NULL, false) < 0) 
  {
    DBG_TRACE("Netlink receive failed\n");
    return -1;
  }

  if (*status < 0)
  {
    DBG_TRACE("Could not get status on interface %s\n",devid);
    return -1;
  }
    
  return 0;
}


/**
 * nlcom_create_if()
 **/
int nlcom_create_if(struct nlcom_handle *handle, int connection_type, int nsapi, bool loop_enabled, char *ifname_set, char *ifname,size_t ifname_len, int *ifindex)
{
  return create_caif_interface(&handle->ctrl_handle, connection_type, nsapi, loop_enabled, ifname_set, ifname, ifname_len, ifindex);
}


/**
 * nlcom_destroy_if()
 **/
int nlcom_destroy_if(struct nlcom_handle *handle, int ifindex)
{
  return destroy_caif_interface(&handle->ctrl_handle, ifindex);
}


/*******************************************************************************
 *
 * Private/Static Functions
 *
 *******************************************************************************/
/**
 *  netlink_open()
 **/
static int netlink_open(struct netlink_handle *rth, unsigned int subscriptions)
{
  socklen_t addr_len;

  assert(rth != NULL);

  memset(rth, 0, sizeof(*rth));

  rth->fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (rth->fd < 0) {
    DBG_TRACE("Create Netlink socket failed\n");
    return -1;
  }

  memset(&rth->local, 0, sizeof(rth->local));
  rth->local.nl_family = PF_NETLINK;
  rth->local.nl_groups = subscriptions;

  if (bind(rth->fd, (struct sockaddr*)&rth->local, sizeof(rth->local)) < 0) {
    DBG_TRACE("Bind Netlink socket failed\n");
    netlink_close(rth);
    return -1;
  }
  /*fetch local info and store */
  addr_len = sizeof(rth->local);
  if (getsockname(rth->fd, (struct sockaddr*)&rth->local, &addr_len) < 0) {
    DBG_TRACE("getsockname failed\n");
    netlink_close(rth);
    return -1;
  }
  if (addr_len != sizeof(rth->local)) {
    DBG_TRACE("illegal address length\n");
    netlink_close(rth);
    return -1;
  }
  if (rth->local.nl_family != AF_NETLINK) {
    DBG_TRACE("illegal family\n");
    netlink_close(rth);
    return -1;
  }
  rth->seq = time(NULL);
  return 0;
}


/**
 *  netlink_close()
 **/
static int netlink_close(struct netlink_handle *rth)
{
  assert(rth != NULL);

  if (rth->fd < 0)
    return 0;

  return close(rth->fd);
}


/**
 *  netlink_recv_message()
 **/
static int netlink_recv_message(struct netlink_handle *nh,
                                netlink_parse_fp_t parsefunc_cb,
                                void *param1, void *param2, void *param3, bool event)
{
  struct sockaddr_nl nladdr;
  struct iovec iov;
  struct msghdr msg;
  int bytes_received;
  char *buf_p = NULL;
  struct nlmsghdr *hdr;

  memset(&msg,0,sizeof(msg));
  msg.msg_name = &nladdr;
  msg.msg_namelen = sizeof(nladdr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  memset(&nladdr, 0, sizeof(nladdr));
  nladdr.nl_family = AF_NETLINK;

  buf_p = malloc(NLCOM_MAX_BUF_SIZE);
  if (NULL == buf_p)
  {
    DBG_TRACE("malloc failed\n");
    return -1;
  }
  memset(buf_p,0,NLCOM_MAX_BUF_SIZE);
  iov.iov_base = buf_p;

  while (1)
  {
    iov.iov_len = NLCOM_MAX_BUF_SIZE;

    bytes_received = recvmsg(nh->fd, &msg, 0);
    if (bytes_received <= 0)
    {
      if (EINTR == errno || EAGAIN == errno)
        continue;
      DBG_TRACE("recvmsg failed failed status %d, errno %d\n",bytes_received,errno);
      free(buf_p);
      return -1;
    }

    /* We have received a message from the kernel. Check and decode*/
    /* There could be several nl packets within the message. Check until we receive a valid answer */
    for (hdr = (struct nlmsghdr*)buf_p; bytes_received >= (int)sizeof(*hdr);)
    {
      //Check if the message is corrupted
      if (sizeof(*hdr) > hdr->nlmsg_len || (int)hdr->nlmsg_len > bytes_received)
      {
        DBG_TRACE("corrupted message: len=%d, sizeof(h)=%d\n", hdr->nlmsg_len,sizeof(*hdr));
        free(buf_p);
        return -1;
      }
      /*Check if the message is sent from the kernel to us*/
      if (event)
      {
        //multicast message expected, pids, and sequence number should be 0
        // if ((nladdr.nl_pid != 0) || (hdr->nlmsg_pid != 0) || (hdr->nlmsg_seq != 0))
        if (!((nladdr.nl_pid == 0) && (hdr->nlmsg_pid == 0) && (hdr->nlmsg_seq == 0)))
        {
          DBG_TRACE("No event from kernel. %d, %d, %d, %d, %d Continue \n",nladdr.nl_pid, hdr->nlmsg_pid,hdr->nlmsg_seq,hdr->nlmsg_type,hdr->nlmsg_flags);
          // check if it's the newaddr message from dhcpcd
          if(hdr->nlmsg_type != RTM_NEWADDR)
          {
            hdr = NLMSG_NEXT(hdr, bytes_received);
            continue;
          }
        }
      }
      else
      {
        if (nladdr.nl_pid != 0 || hdr->nlmsg_pid != nh->local.nl_pid || hdr->nlmsg_seq != nh->seq)
        {
          hdr = NLMSG_NEXT(hdr, bytes_received);
          DBG_TRACE("Message not for us. Continue %d,  %d, %d, seq %d, %d\n",nladdr.nl_pid, hdr->nlmsg_pid,nh->local.nl_pid,hdr->nlmsg_seq,nh->seq);
          continue;
        }
      }
      if (NLMSG_DONE == hdr->nlmsg_type)
      {
        /*Finished. ok*/
        free(buf_p);
        return 0;
      }

      if (NLMSG_ERROR == hdr->nlmsg_type)
      {
        /*Could still be a valid answer. Check errno */
        struct nlmsgerr *msgerr = (struct nlmsgerr*)NLMSG_DATA(hdr);
        if (0 == msgerr->error)
        {
          /*ok answer. ACK*/
          free(buf_p);
          return 0;
        }
        DBG_TRACE("NLMSG_ERROR, error %d\n",-msgerr->error);
        free(buf_p);
        return -1;
      }

      /*call parse function*/
      if (parsefunc_cb)
      {
        int res = parsefunc_cb(hdr,param1,param2,param3);
        if (res < 0)
        {
          DBG_TRACE("parsefunc_cb failed.\n");
          free(buf_p);
          return -1;
        }
        else if (res > 0)
        {
          //Finished ok
          free(buf_p);
          return 0;
        }
      }
      /*Go to next*/
      hdr = NLMSG_NEXT(hdr, bytes_received);
    } //for
  } //while
  if (buf_p)
    free(buf_p);

  return 0;
}


/**
 *  create_caif_interface()
 **/
static int create_caif_interface(struct netlink_handle *nh, int connection_type, int nsapi, bool loop_enabled, char *ifname_set, char *ifname, size_t ifname_len, int *ifindex)
{
  char type[] = "caif";
  struct iplink_req *req;

  assert(ifname != NULL);
  assert(ifindex != NULL);

  DBG_TRACE("Inside create_caif_interfac\n");

  req = malloc(sizeof(struct iplink_req));
  if (NULL == req) {
    DBG_TRACE("malloc failed\n");
    return -1;
  }
  memset(req, 0, sizeof(struct iplink_req));

  req->n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  req->n.nlmsg_flags = NLM_F_REQUEST|NLM_F_CREATE|NLM_F_EXCL|NLM_F_ACK;
  req->n.nlmsg_type = RTM_NEWLINK;
  req->i.ifi_family = AF_UNSPEC;

  {
    struct rtattr *linkinfo = NLMSG_TAIL(&req->n);
    add_attribute(&req->n, sizeof(struct iplink_req), IFLA_LINKINFO, NULL, 0);
    add_attribute(&req->n, sizeof(struct iplink_req), IFLA_INFO_KIND, type, strlen(type));

    {
      struct rtattr * data = NLMSG_TAIL(&req->n);
      add_attribute(&req->n, sizeof(struct iplink_req), IFLA_INFO_DATA, NULL, 0);
      if (connection_type == IFLA_CAIF_IPV4_CONNID)
        add_attribute(&req->n, sizeof(struct iplink_req), IFLA_CAIF_IPV4_CONNID, &nsapi,4);
      else if (connection_type == IFLA_CAIF_IPV6_CONNID)
        add_attribute(&req->n, sizeof(struct iplink_req), IFLA_CAIF_IPV6_CONNID, &nsapi,4);
      else {
        DBG_TRACE("unsupported linktype\n");
        free(req);
        return -1;
      }
      if (loop_enabled) {
        nsapi = 1; //1 - means loop enabled
        add_attribute(&req->n, sizeof(struct iplink_req), IFLA_CAIF_LOOPBACK, &nsapi,4);
      }
      data->rta_len = (void *)NLMSG_TAIL(&req->n) - (void *)data;
    }

    linkinfo->rta_len = (void *)NLMSG_TAIL(&req->n) - (void *)linkinfo;
  }

  /* Send Netlink packet. Response ok/not ok*/
  if (send_netlink_request(nh, &req->n) < 0) {
    DBG_TRACE("Netlink sendmessage failed\n");
    free(req);
    return -1;
  }

  /*Then get the id*/
  if (get_caif_interface_id(nh, ifname, ifindex) < 0) {
    DBG_TRACE("failed to get the id\n");
    free(req);
    return -1;
  }

  //Check the ifname_set. If this is different than the type ("caif"), we will have to change it
  if (NULL != ifname_set && strcmp(ifname_set,type) != 0)
  {
    int suffix=0;

    //Get available index
    if (get_caif_interface_suffix(nh, ifname_set,&suffix) < 0)
      DBG_TRACE("failed to get the suffix, try 0\n");

    snprintf(ifname,ifname_len,"%s%d",ifname_set,suffix);
    DBG_TRACE("Change Interface name to %s\n",ifname);

    //set new name
    set_caif_interface_id(nh, *ifindex,ifname);
  }

  free(req);
  return 0;
}


/**
 *  destroy_caif_interface()
 **/
static int destroy_caif_interface(struct netlink_handle *nh, int ifindex)
{
  struct iplink_req *req;

  req = malloc(sizeof(struct iplink_req));
  if (NULL == req) {
    DBG_TRACE("malloc failed\n");
    return -1;
  }
  memset(req, 0, sizeof(struct iplink_req));

  req->n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  req->n.nlmsg_flags = NLM_F_REQUEST|NLM_F_CREATE|NLM_F_EXCL|NLM_F_ACK;
  req->n.nlmsg_type = RTM_DELLINK;
  req->i.ifi_family = AF_UNSPEC;
  req->i.ifi_index = ifindex;

  if (send_netlink_request(nh, &req->n) < 0) {
    DBG_TRACE("Netlink sendmessage failed\n");
    free(req);
    return -1;
  }

  free(req);
  return 0;
}


/**
 *  get_caif_interface_id()
 **/
static int get_caif_interface_id(struct netlink_handle *nh, char *ifname, int *ifindex)
{
  assert(ifname != NULL);
  assert(ifindex != NULL);

  if (netlink_send_dump_request(nh) < 0)
  {
    DBG_TRACE("Netlink send dump message failed\n");
    return -1;
  }


  if (netlink_recv_message(nh, parse_data_get_last_created_if, (void*)ifname, (void*)ifindex,NULL, false) < 0) 
  {
    DBG_TRACE("Netlink receive failed\n");
    return -1;
  }

  return 0;
}


/**
 *  get_caif_interface_suffix()
 **/
static int get_caif_interface_suffix(struct netlink_handle *nh, char *iftype, int *suffix)
{
  assert(iftype != NULL);
  assert(suffix != NULL);

  if (netlink_send_dump_request(nh) < 0)
  {
    DBG_TRACE("Netlink send dump message failed\n");
    return -1;
  }


  if (netlink_recv_message(nh, parse_data_get_suffix, (void*)iftype, (void*)suffix, NULL, false) < 0) 
  {
    DBG_TRACE("Netlink receive failed\n");
    return -1;
  }

  return 0;
}


/**
 *  set_caif_interface_id()
 **/
static int set_caif_interface_id(struct netlink_handle *nh, int ifindex, char *ifname)
{
  struct iplink_req *req;

  assert(ifname != NULL);

  req = malloc(sizeof(struct iplink_req));
  if (NULL == req) {
    DBG_TRACE("malloc failed\n");
    return -1;
  }
  memset(req, 0, sizeof(struct iplink_req));

  req->n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  req->n.nlmsg_flags = NLM_F_REQUEST|NLM_F_CREATE|NLM_F_EXCL|NLM_F_ACK;
  req->n.nlmsg_type = RTM_SETLINK;
  req->i.ifi_family = AF_UNSPEC;
  req->i.ifi_index = ifindex;

  {
    struct rtattr *linkinfo = NLMSG_TAIL(&req->n);
    add_attribute(&req->n, sizeof(struct iplink_req), IFLA_IFNAME, ifname, strlen(ifname));

    linkinfo->rta_len = (void *)NLMSG_TAIL(&req->n) - (void *)linkinfo;
  }

  if (send_netlink_request(nh, &req->n) < 0) {
    DBG_TRACE("Netlink sendmessage failed\n");
    free(req);
    return -1;
  }

  free(req);
  return 0;
}


/**
 *   send_netlink_request()
 **/
static int send_netlink_request(struct netlink_handle *handle, struct nlmsghdr *hdr)
{
  struct sockaddr_nl nladdr;
  struct iovec iov;
  struct msghdr msg;
  int res;

  assert(handle != NULL);
  assert(hdr != NULL);

  iov.iov_base = (void*)hdr;
  iov.iov_len = hdr->nlmsg_len;

  memset(&msg,0,sizeof(msg));
  msg.msg_name = &nladdr;
  msg.msg_namelen = sizeof(nladdr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  memset(&nladdr, 0, sizeof(nladdr));
  nladdr.nl_family = AF_NETLINK;

  hdr->nlmsg_seq = ++handle->seq;

  if ((res = sendmsg(handle->fd, &msg, 0)) < 0)
  {
    DBG_TRACE("sendmsg failed\n");
    return -1;
  }

  if (netlink_recv_message(handle,NULL,NULL,NULL,NULL,false) < 0)
  {
    DBG_TRACE("recvmsg failed\n");
    return -1;
  }
  return 0;
}


/**
 *  netlink_send_dump_request()
 **/
static int netlink_send_dump_request(struct netlink_handle *nh)
{
  struct dump_req req;

  assert(nh != NULL);

  memset(&req, 0, sizeof(req));
  req.n.nlmsg_len = sizeof(req);
  req.n.nlmsg_type = RTM_GETLINK;
  req.n.nlmsg_flags = NLM_F_ROOT|NLM_F_MATCH|NLM_F_REQUEST;
  req.n.nlmsg_pid = 0;
  req.n.nlmsg_seq = ++nh->seq;
  req.g.rtgen_family = AF_UNSPEC;

  if (send(nh->fd, (void*)&req, sizeof(req), 0) < 0)
  {
    DBG_TRACE("send failed\n");
    return -1;
  }

  return 0;
}


/**
 *   add_attribute()
 **/
static int add_attribute(struct nlmsghdr *n, int maxlen, int type, const void *data, int datalen)
{
  int len = RTA_LENGTH(datalen);
  struct rtattr *rta;

  assert(n != NULL);

  if ((int)(NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len)) > maxlen)
  {
    DBG_TRACE("attribute to large for message\n");
    return -1;
  }
  rta = NLMSG_TAIL(n);
  rta->rta_type = type;
  rta->rta_len = len;
  memcpy(RTA_DATA(rta), data, datalen);
  n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
  return 0;
}


/**
 *   parse_dump_data_get_if_id()
 **/
static int parse_data_get_last_created_if(struct nlmsghdr *n, void *param1, void *param2, void *param3)
{
  struct ifinfomsg *ifi = NLMSG_DATA(n);
  struct rtattr *tb[IFLA_MAX+1];
  struct rtattr *rta = IFLA_RTA(ifi);
  int len = IFLA_PAYLOAD(n);
  char *ifname = (char*)param1;
  int *ifindex = (int *)param2;
  NLCOM_IDENTIFIER_NOT_USED(param3);

  assert(n != NULL);
  assert(param1 != NULL);
  assert(param2 != NULL);

  if (n->nlmsg_len < NLMSG_LENGTH(sizeof(ifi)))
    return -1;

  memset(tb, 0, sizeof(tb));

  while (RTA_OK(rta, len))
  {
    if (rta->rta_type <= IFLA_MAX)
      tb[rta->rta_type] = rta;
    rta = RTA_NEXT(rta,len);
  }
  if (len > 0)
    DBG_TRACE("Error during parsing. %d\n",len);

  if (tb[IFLA_IFNAME] == NULL)
    return 0;

  *ifindex = ifi->ifi_index;
  strncpy(ifname,(char *)RTA_DATA(tb[IFLA_IFNAME]),IFNAMSIZ);

  return 0;
}


/**
 *   parse_dump_data_get_suffix()
 **/
static int parse_data_get_suffix(struct nlmsghdr *n, void *param1, void *param2, void *param3)
{
  struct ifinfomsg *ifi = NLMSG_DATA(n);
  struct rtattr *tb[IFLA_MAX+1];
  struct rtattr *rta = IFLA_RTA(ifi);
  int len = IFLA_PAYLOAD(n);
  char tmp[IFNAMSIZ];
  char *iftype = (char*)param1;
  int *ifsuffix = (int *)param2;
  int suffix = -1;
  NLCOM_IDENTIFIER_NOT_USED(param3);

  assert(n != NULL);
  assert(param1 != NULL);
  assert(param2 != NULL);

  if (n->nlmsg_len < NLMSG_LENGTH(sizeof(ifi)))
    return -1;

  memset(tb, 0, sizeof(tb));

  while (RTA_OK(rta, len))
  {
    if (rta->rta_type <= IFLA_MAX)
      tb[rta->rta_type] = rta;
    rta = RTA_NEXT(rta,len);
  }
  if (len > 0)
    DBG_TRACE("Error during parsing. %d\n",len);

  if (tb[IFLA_IFNAME] == NULL)
    return 0;

  strncpy(tmp,(char *)RTA_DATA(tb[IFLA_IFNAME]),IFNAMSIZ);

  if (NULL != strstr(tmp,iftype))
  {
    char keys[] = "1234567890";
    int i;
    i = strcspn (tmp,keys);
    if (i < 0 || i == (int)strlen(tmp))
    {
      DBG_TRACE("No numeric value found..\n");
      suffix = -1;
    }
    else
      suffix = strtol(&tmp[i],NULL,10);
  }

  if (suffix >= *ifsuffix)
    *ifsuffix = suffix+1;

  return 0;
}


/**
 *   parse_data_get_status()
 **/
static int parse_data_get_status_from_ifindex(struct nlmsghdr *n, void *param1, void *param2, void *param3)
{
  struct ifinfomsg *ifi = NLMSG_DATA(n);
  struct rtattr *tb[IFLA_MAX+1];
  struct rtattr *rta = IFLA_RTA(ifi);
  int len = IFLA_PAYLOAD(n);
  int *ifindex = (int *)param1;
  int *status  = (int *)param2;
  char *devid = (char *)param3;
  

  assert(n != NULL);
  assert(param1 != NULL);
  assert(param2 != NULL);

  ifi = NLMSG_DATA(n);

  if (n->nlmsg_len < (int)NLMSG_LENGTH(sizeof(ifi))) {
    DBG_TRACE("Error during parsing. msg too small %d\n",len);
    return -1;
  }

  *ifindex = ifi->ifi_index;

  if(NULL != devid)
  {
    memset(tb, 0, sizeof(tb));

    while (RTA_OK(rta, len))
    {
      if (rta->rta_type <= IFLA_MAX)
        tb[rta->rta_type] = rta;
      rta = RTA_NEXT(rta,len);
    }
    if (len > 0)
    {
      DBG_TRACE("Error during parsing. %d, msgtyp %d\n",len,n->nlmsg_type);
      return 0;
    }
  

    if (tb[IFLA_IFNAME] == NULL)
    {
      DBG_TRACE("Error during parsing. No devicename found\n");
      return 0;
    }
  
    strncpy(devid,(char *)RTA_DATA(tb[IFLA_IFNAME]),IFNAMSIZ);
  }
  
  
  switch (n->nlmsg_type)
  {
  case  RTM_NEWLINK:
    if (ifi->ifi_flags & IFF_LOWER_UP)
      *status = NLCOM_STATUS_IF_UP;
    else
      *status = NLCOM_STATUS_CREATED; //NOTE: Could aslo indicate IF_down
    break;
  case  RTM_DELLINK:
    *status = NLCOM_STATUS_DELETED;
    break;
  case  RTM_NEWADDR:
    *status = NLCOM_STATUS_NEW_ADDR;
    break;
  case  RTM_DELADDR:
    *status = NLCOM_STATUS_DEL_ADDR;
    break;
  default:
    DBG_TRACE("Received netlink event, interface=%d, msg=%d, flags=%d\n",ifi->ifi_index, n->nlmsg_type, ifi->ifi_flags);
  }

  //Return 1 to indicate that we are finished.
  return 1;
}


/**
 *   parse_dump_data_get_status()
 **/
static int parse_data_get_status_from_devid(struct nlmsghdr *n, void *param1, void *param2, void *param3)
{
  struct ifinfomsg *ifi = NLMSG_DATA(n);
  struct rtattr *tb[IFLA_MAX+1];
  struct rtattr *rta = IFLA_RTA(ifi);
  int len = IFLA_PAYLOAD(n);
  char tmp[IFNAMSIZ];
  char *devid = (char*)param1;
  int *status = (int *)param2;
  NLCOM_IDENTIFIER_NOT_USED(param3);

  assert(n != NULL);
  assert(param1 != NULL);
  assert(param2 != NULL);

  if (n->nlmsg_len < NLMSG_LENGTH(sizeof(ifi)))
    return -1;

  memset(tb, 0, sizeof(tb));

  while (RTA_OK(rta, len))
  {
    if (rta->rta_type <= IFLA_MAX)
      tb[rta->rta_type] = rta;
    rta = RTA_NEXT(rta,len);
  }
  if (len > 0)
    DBG_TRACE("Error during parsing. %d\n",len);

  if (tb[IFLA_IFNAME] == NULL)
    return 0;

  strncpy(tmp,(char *)RTA_DATA(tb[IFLA_IFNAME]),IFNAMSIZ);

  if (0 == strncmp(tmp,devid,IFNAMSIZ))
  {
    if (ifi->ifi_flags & IFF_LOWER_UP)
      *status = NLCOM_STATUS_IF_UP;
    else
      *status = NLCOM_STATUS_CREATED; //NOTE: Could aslo indicate IF_down

    return 1;
  }

  return 0;
}


