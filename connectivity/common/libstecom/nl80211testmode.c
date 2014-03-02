/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include "linux/nl80211.h"

#include "libstecom_nl80211testmode.h"
#include "nl80211testmode_i.h"


struct stec_nl {
  struct nl_sock *nl_sock;
  struct nl_sock *nl_sock_event;
  struct nl_cache *nl_cache;
  struct nl_cache *nl_cache_event;
  struct genl_family *nl80211;

  char ifname[STEC_NL_IFNAME_SIZE];
  int ifindex;
  int phyindex;
};



/* nl80211 code */
static int ack_handler(struct nl_msg *msg, void *arg)
{
  int *err = arg;
  *err = 0;
  return NL_STOP;
}

static int finish_handler(struct nl_msg *msg, void *arg)
{
  int *ret = arg;
  *ret = 0;
  return NL_SKIP;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
  int *ret = arg;
  *ret = err->error;
  return NL_SKIP;
}

static int no_seq_handler(struct nl_msg *msg, void *arg)
{
  return NL_OK;
}


static int phy_lookup(char *name)
{
  char buf[200];
  int fd, pos;

  snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name);

  fd = open(buf, O_RDONLY);
  if (fd < 0)
    return -1;

  pos = read(fd, buf, sizeof(buf) - 1);

  if (pos < 0)
    return -1;

  buf[pos] = '\0';
  return atoi(buf);
}

#ifndef CONFIG_LIBNL20
/* libnl 2.0 compatibility code */

static inline struct nl_handle *nl_socket_alloc(void)
{
  return nl_handle_alloc();
}

static inline void nl_socket_free(struct nl_sock *h)
{
  nl_handle_destroy(h);
}

static inline int __genl_ctrl_alloc_cache(struct nl_sock *h, struct nl_cache **cache)
{
  struct nl_cache *tmp = genl_ctrl_alloc_cache(h);

  if (!tmp)
    return -ENOMEM;

  *cache = tmp;

  return 0;
}

#define genl_ctrl_alloc_cache __genl_ctrl_alloc_cache
#endif /* CONFIG_LIBNL20 */


struct family_data {
  const char *group;
  int id;
};

static int _nl_get_multicast_id(struct stec_nl *nl,
                               const char *family, const char *group)
{
  struct nl_msg *msg;
  int res = -1;
  struct family_data f_res = { group, -ENOENT };

  msg = nlmsg_alloc();
  if (!msg)
    return -ENOMEM;

  genlmsg_put(msg, 0, 0, genl_ctrl_resolve(nl->nl_sock, "nlctrl"),
              0, 0, CTRL_CMD_GETFAMILY, 0);
  NLA_PUT_STRING(msg, CTRL_ATTR_FAMILY_NAME, family);

  res = nl_send_auto_complete(nl->nl_sock, msg);
  if (res < 0)
  {
    perror("nl80211: nl_send_auto_complete");
    goto THIS_FAILED;
  }

  res = _stec_nl80211testmode_recv(nl, (char *)&f_res, sizeof(f_res));
  if (res == 0)
    res = f_res.id;

THIS_FAILED:
nla_put_failure:
  nlmsg_free(msg);
  return res;
}



struct stec_nl * _stec_nl80211testmode_init()
{
  return calloc(1, sizeof(struct stec_nl));
}

void _stec_nl80211testmode_deinit(struct stec_nl * const nl)
{
  if (nl != NULL)
    free(nl);
}


int _stec_nl80211testmode_open(struct stec_nl * const nl, const struct sockaddr *addr, socklen_t addr_len)
{
  nl->nl_sock = nl_socket_alloc();

  memset(nl->ifname, 0, STEC_NL_IFNAME_SIZE);
  strncpy(nl->ifname, ((struct sockaddr_nl80211tm *)addr)->nltm_path, strlen(((struct sockaddr_nl80211tm *)addr)->nltm_path));

  if (!nl->nl_sock)
  {
    perror("nl80211: Failed to allocate netlink socket");
    goto THIS_FAILED;
  }

  if (genl_connect(nl->nl_sock))
  {
    perror("nl80211: Failed to connect to generic netlink");
    goto THIS_FAILED;
  }

  if (genl_ctrl_alloc_cache(nl->nl_sock, &nl->nl_cache) < 0)
  {
    perror("nl80211: Failed to allocate generic netlink cache");
    goto THIS_FAILED;
  }

  nl->nl80211 = genl_ctrl_search_by_name(nl->nl_cache, "nl80211");
  if (!nl->nl80211)
  {
    perror("nl80211: 'nl80211' generic netlink not found");
    goto THIS_FAILED;
  }

  nl->phyindex = 0;
  nl->ifindex = if_nametoindex(nl->ifname);
  if (!nl->ifindex)
  {
    nl->phyindex = phy_lookup(nl->ifname);
    if (nl->phyindex < 0)
    {
      perror("nl80211: phy_lookup");
      goto THIS_FAILED;
    }
  }

  nl_socket_enable_msg_peek(nl->nl_sock);

  return 0;

THIS_FAILED:
  nl_cache_free(nl->nl_cache);
  nl->nl_cache = NULL;

  nl_close(nl->nl_sock);
  nl_socket_free(nl->nl_sock);
  nl->nl_sock = NULL;

  return -1;
}


int _stec_nl80211testmode_close(struct stec_nl * const nl)
{
  genl_family_put(nl->nl80211);

  nl_cache_free(nl->nl_cache);
  nl->nl_cache = NULL;

  nl_close(nl->nl_sock);
  nl_socket_free(nl->nl_sock);
  nl->nl_sock = NULL;

  return 0;
}


int _stec_nl80211testmode_send(struct stec_nl * const nl, const char * const buf, size_t buf_len)
{
  struct nl_msg *msg = NULL;
  int res = -1;

  msg = nlmsg_alloc();
  if (!msg)
  {
    perror("nl80211: msg alloc");
    goto THIS_FAILED;
  }

  genlmsg_put(msg, 0, 0, genl_family_get_id(nl->nl80211), 0,
              0, NL80211_CMD_TESTMODE, 0);

  if (nl->ifindex)
  {
    NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, nl->ifindex);
  } else {
    NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, nl->phyindex);
  }

  res = nla_put(msg, NL80211_ATTR_TESTDATA, buf_len, buf);
  if (res < 0)
  {
    perror("nl80211: nla_put");
    goto THIS_FAILED;
  }

  res = nl_send_auto_complete(nl->nl_sock, msg);
  if (res < 0)
  {
    perror("nl80211: nl_send_auto_complete");
    goto THIS_FAILED;
  }

  return 0;

THIS_FAILED:
nla_put_failure:
  nlmsg_free(msg);
  return res;
}


struct _recv_data_t
{
  char * recv_buf;
  int * recv_len;
  struct stec_nl * nl;
};

static int _stec_nl80211testmode_recv_handler(struct nl_msg *msg, void *arg)
{
  struct nlattr *tb[NL80211_ATTR_MAX + 1];
  struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
  struct nlattr *mcgrp;
  struct _recv_data_t *recv_data = (struct _recv_data_t *)arg;
  int i;

  nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
    genlmsg_attrlen(gnlh, 0), NULL);

  if(tb[NL80211_ATTR_TESTDATA] && tb[NL80211_ATTR_WIPHY])
  {
    memcpy(recv_data->recv_buf, nla_data(tb[NL80211_ATTR_TESTDATA]), nla_len(tb[NL80211_ATTR_TESTDATA]));
    *recv_data->recv_len = nla_len(tb[NL80211_ATTR_TESTDATA]);
  }
  else if(tb[CTRL_ATTR_MCAST_GROUPS])
  {
    struct family_data *res = (struct family_data *)recv_data->recv_buf;

    nla_for_each_nested(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], i) 
    {
      struct nlattr *tb2[CTRL_ATTR_MCAST_GRP_MAX + 1];

      nla_parse(tb2, CTRL_ATTR_MCAST_GRP_MAX, nla_data(mcgrp), nla_len(mcgrp), NULL);
      if (!tb2[CTRL_ATTR_MCAST_GRP_NAME] ||
          !tb2[CTRL_ATTR_MCAST_GRP_ID] ||
          strncmp(nla_data(tb2[CTRL_ATTR_MCAST_GRP_NAME]), res->group, nla_len(tb2[CTRL_ATTR_MCAST_GRP_NAME])) != 0)
      {
        continue;
      }

      // Found multicast ID for TestMode CMD
      res->id = nla_get_u32(tb2[CTRL_ATTR_MCAST_GRP_ID]);
      *recv_data->recv_len = 0;
      break;
    };
  }
  else
  {
    perror("no data!");
    *recv_data->recv_len = 0;
  }

  return NL_SKIP;
}

int _stec_nl80211testmode_recv(struct stec_nl * const nl, char * const buf, size_t buf_len)
{
  struct nl_cb *cb = NULL;
  int err = 1;
  int res = -1;
  struct _recv_data_t recv_data;

  cb = nl_cb_alloc(NL_CB_DEFAULT);
  if (!cb)
  {
    perror("nl80211: nl_cb_alloc");
    res = -1;
    goto THIS_FAILED;
  }

  recv_data.recv_buf = buf;
  recv_data.recv_len = &res;
  recv_data.nl = nl;

  nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
  nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
  nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

  if(buf)
  {
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, _stec_nl80211testmode_recv_handler, &recv_data);
  }

  while (err > 0)
    nl_recvmsgs(nl->nl_sock, cb);

  if(err == 0 && res == -1 && recv_data.recv_buf == NULL) {
    res = 0;
  }

THIS_FAILED:
  nl_cb_put(cb);
  return res;
}



int _stec_nl80211testmode_event_subscribe(struct stec_nl * const nl)
{
  int ret = -1;

  nl->nl_sock_event = nl_socket_alloc();

  if (!nl->nl_sock_event)
  {
    perror("nl80211: Failed to allocate netlink event socket");
    goto THIS_FAILED;
  }

  if (genl_connect(nl->nl_sock_event))
  {
    perror("nl80211: Failed to connect to generic netlink (event)");
    goto THIS_FAILED;
  }

  if (genl_ctrl_alloc_cache(nl->nl_sock_event, &nl->nl_cache_event) < 0)
  {
    perror("nl80211: Failed to allocate generic netlink cache (event)");
    goto THIS_FAILED;
  }

  ret = _nl_get_multicast_id(nl, "nl80211", "testmode");
  if (ret >= 0)
  {
    ret = nl_socket_add_membership(nl->nl_sock_event, ret);
  }

  if (ret < 0) {
    perror("nl80211: Could not add multicast membership for testmode events");
    goto THIS_FAILED;
  }

  ret = nl_socket_get_fd(nl->nl_sock_event);

  return ret;

THIS_FAILED:
  nl_cache_free(nl->nl_cache_event);
  nl->nl_cache_event = NULL;

  nl_socket_free(nl->nl_sock_event);
  nl->nl_sock_event = NULL;

  return ret;
}

int _stec_nl80211testmode_event_unsubscribe(struct stec_nl * const nl)
{
  if (nl->nl_cache_event)
  {
    nl_cache_free(nl->nl_cache_event);
    nl->nl_cache_event = NULL;
  }

  if (nl->nl_sock_event)
  {
    nl_socket_free(nl->nl_sock_event);
    nl->nl_sock_event = NULL;
  }

  return 0;
}

static int _stec_nl80211testmode_event_recv_handler(struct nl_msg *msg, void *arg)
{
  struct nlattr *tb[NL80211_ATTR_MAX + 1];
  struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
  struct _recv_data_t *recv_data = (struct _recv_data_t *)arg;

  *recv_data->recv_len = 0;

  nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
    genlmsg_attrlen(gnlh, 0), NULL);

  if (tb[NL80211_ATTR_IFINDEX])
  {
    int ifindex = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);

    if (ifindex != recv_data->nl->ifindex)
    {
      printf("libstecom nl80211 testmode: Ignored event (cmd=%d)"
             " for foreign interface (ifindex %d)\n",
             gnlh->cmd, recv_data->nl->ifindex);
      return NL_SKIP;
    }
  }

  if (tb[NL80211_ATTR_TESTDATA])
  {
    memcpy(recv_data->recv_buf, nla_data(tb[NL80211_ATTR_TESTDATA]), nla_len(tb[NL80211_ATTR_TESTDATA]));
    *recv_data->recv_len = nla_len(tb[NL80211_ATTR_TESTDATA]);
  }

  // Unknown events are ignored

  return NL_SKIP;
}

int _stec_nl80211testmode_event_recv(struct stec_nl * const nl, char * const buf, size_t buf_len)
{
  struct nl_cb *cb = NULL;
  int res = -1;
  struct _recv_data_t recv_data;

  cb = nl_cb_alloc(NL_CB_DEFAULT);
  if (!cb)
  {
    perror("nl80211: nl_cb_alloc");
    res = -1;
    goto THIS_FAILED;
  }

  recv_data.recv_buf = buf;
  recv_data.recv_len = &res;
  recv_data.nl = nl;

  nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_handler, NULL);
  nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, _stec_nl80211testmode_event_recv_handler, &recv_data);

  nl_recvmsgs(nl->nl_sock_event, cb);

  nl_cb_put(cb);

THIS_FAILED:
  return res;
}

