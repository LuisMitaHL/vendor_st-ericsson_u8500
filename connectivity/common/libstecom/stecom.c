/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "libstecom.h"

#include "subscribe_i.h"

#ifdef CONFIG_NL80211_TESTMODE
#include "nl80211testmode_i.h"
#endif

#include "stecom_i.h"

#ifdef SW_VARIANT_ANDROID
#define LOG_TAG "RIL_STECOM"
#include <utils/Log.h>
#else
#define ALOGI(format, arg...) fprintf(stdout, format, ##arg)
#define ALOGE(format, arg...) fprintf(stderr, format, ##arg)
#endif

/*
 * NL80211 is only supported on client side
 */


static const char *cmd_subscribe = "SUBSCRIBE";
static const char *cmd_unsubscribe = "UNSUBSCRIBE";
static const char *cmdresp_ok = "OK";
static const char *cmdresp_fail = "FAIL";


static void _stec_ctrl_init(struct stec_ctrl *ctrl)
{
  ctrl->serverside = 0;

  ctrl->local_addr = NULL;
  ctrl->local_addr_len = 0;

  ctrl->remote_addr = NULL;
  ctrl->remote_addr_len = 0;

  ctrl->last_client_addr = NULL;
  ctrl->last_client_addr_len = 0;

  ctrl->subscribe = NULL;

#ifdef CONFIG_NL80211_TESTMODE
  ctrl->nl = NULL;
#endif
}


struct stec_ctrl * stec_open(const struct sockaddr *addr, socklen_t addr_len)
{
  struct stec_ctrl *ctrl = calloc(1, sizeof(struct stec_ctrl));

  _stec_ctrl_init(ctrl);

  ctrl->remote_addr_len = addr_len;
  ctrl->remote_addr = calloc(1, addr_len);
  memcpy(ctrl->remote_addr, addr, addr_len);

  if (ctrl->remote_addr->sa_family == AF_UNIX)
  {
    int temp_fd;
    struct sockaddr_un *local_addr_un;

    // We need a local addr for recvfrom to work
    ctrl->local_addr_len = sizeof(local_addr_un->sun_path);
    ctrl->local_addr = calloc(1, ctrl->local_addr_len);
    local_addr_un = (struct sockaddr_un *)ctrl->local_addr;
    local_addr_un->sun_family = AF_UNIX;

#ifdef SW_VARIANT_ANDROID
    snprintf(local_addr_un->sun_path, ctrl->local_addr_len, "/data/local/tmp/stecom-XXXXXX");
#else
    snprintf(local_addr_un->sun_path, ctrl->local_addr_len, "/tmp/stecom-XXXXXX");
#endif

    /*
     * Silence mktemp() / tmpnam() unsecure warning.
     * Also we don't unlink as socket seems to not care.
     */
    temp_fd = mkstemp(local_addr_un->sun_path);
    if (temp_fd < 0)
    {
      ALOGE("mkstemp failed: %s", strerror(errno));
    }
    else
    {
      close(temp_fd);
    }

    ctrl->sock = socket(PF_UNIX, SOCK_DGRAM, 0);
  }
  else if (ctrl->remote_addr->sa_family == AF_INET)
  {
    ctrl->sock = socket(PF_INET, SOCK_DGRAM, 0);
  }
#ifdef CONFIG_NL80211_TESTMODE
  else if (ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    ctrl->nl = _stec_nl80211testmode_init();

    ctrl->sock = _stec_nl80211testmode_open(ctrl->nl, addr, addr_len);

    if (ctrl->sock >= 0)
    {
      // All is OK, stop here
      return ctrl;
    }
  }
#endif
  else
  {
    ALOGE("socket: %s", strerror(errno));
    goto THIS_FAILED;
  }

  if (ctrl->sock < 0)
  {
    ALOGE("create client socket: %s", strerror(errno));
    goto THIS_FAILED;
  }

  // Connect to local addr if we've got one
  if (ctrl->local_addr != NULL)
  {
    // Unlinking just in case
    unlink(((struct sockaddr_un *)ctrl->local_addr)->sun_path);

    if (bind(ctrl->sock, (struct sockaddr *)ctrl->local_addr, ctrl->local_addr_len) < 0)
    {
      ALOGE("bind: %s", strerror(errno));
      goto THIS_FAILED;
    }
  }

  if (connect(ctrl->sock, ctrl->remote_addr, ctrl->remote_addr_len) < 0)
  {
    ALOGE("connect: %s", strerror(errno));
    goto THIS_FAILED;
  }

  return ctrl;

THIS_FAILED:
  stec_close(ctrl);

  return NULL;
}

struct stec_ctrl * stec_opensrv(const struct sockaddr *addr, socklen_t addr_len)
{
  struct stec_ctrl *ctrl = calloc(1, sizeof(struct stec_ctrl));

  _stec_ctrl_init(ctrl);

  ctrl->serverside = 1;

  ctrl->last_client_addr_len = addr_len;
  ctrl->last_client_addr = calloc(1, addr_len);

  ctrl->local_addr_len = addr_len;
  ctrl->local_addr = calloc(1, addr_len);
  memcpy(ctrl->local_addr, addr, addr_len);

  if (ctrl->local_addr->sa_family == AF_UNIX)
  {
    ctrl->sock = socket(PF_UNIX, SOCK_DGRAM, 0);
  }
  else if (ctrl->local_addr->sa_family == AF_INET)
  {
    ctrl->sock = socket(PF_INET, SOCK_DGRAM, 0);
  }
  else
  {
    ALOGE("unknown addr family when creating server socket: %s", strerror(errno));
    goto THIS_FAILED;
  }

  if (ctrl->sock < 0) {
    ALOGE("create server socket: %s", strerror(errno));
    goto THIS_FAILED;
  }

  if (bind(ctrl->sock, addr, addr_len) < 0)
  {
    ALOGE("server socket bind: %s", strerror(errno));
    goto THIS_FAILED;
  }

  ctrl->subscribe = subscribe_init();

  return ctrl;

THIS_FAILED:
  stec_close(ctrl);

  return NULL;
}

void stec_close(struct stec_ctrl *ctrl)
{
  if(!ctrl)
  {
    return;
  }

  if (ctrl->local_addr)
  {
    if (ctrl->local_addr->sa_family == AF_UNIX)
    {
      unlink(((struct sockaddr_un *)ctrl->local_addr)->sun_path);
    }

    free(ctrl->local_addr);
  }

#ifdef CONFIG_NL80211_TESTMODE
  if (ctrl->remote_addr && ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    ctrl->sock = _stec_nl80211testmode_close(ctrl->nl);
    _stec_nl80211testmode_deinit(ctrl->nl);
    ctrl->nl = NULL;
  }
#endif

  if (ctrl->remote_addr)
  {
    free(ctrl->remote_addr);
  }

  if (ctrl->last_client_addr)
  {
    free(ctrl->last_client_addr);
  }

  if (ctrl->sock >= 0)
  {
    close(ctrl->sock);
  }

  subscribe_deinit(ctrl->subscribe);

  free(ctrl);
}


int stec_get_fd(struct stec_ctrl *ctrl)
{
  if(!ctrl)
  {
    return -1;
  }

  return ctrl->sock;
}


int stec_get_local_addr(struct stec_ctrl *ctrl, struct sockaddr *addr, socklen_t addr_len)
{
  if(!ctrl)
  {
    return -1;
  }

  if (ctrl->local_addr && (addr_len >= ctrl->local_addr_len))
  {
    memcpy(addr, ctrl->local_addr, ctrl->local_addr_len);
    return ctrl->local_addr_len;
  }

  return -1;
}



int _stec_recvfrom(struct stec_ctrl *ctrl, char *buf, size_t buf_len, struct sockaddr *addr, socklen_t *addr_len)
{
  return recvfrom(ctrl->sock, buf, buf_len, 0, addr, addr_len);
}


int __stec_recv(struct stec_ctrl *ctrl, char *buf, size_t buf_len)
{
  int res = -1;

  if (ctrl->serverside == 1)
  {
    socklen_t fromlen = ctrl->last_client_addr_len;

    memset(ctrl->last_client_addr, 0, ctrl->last_client_addr_len);
    res = _stec_recvfrom(ctrl, buf, buf_len, ctrl->last_client_addr, &fromlen);

    if (fromlen == 0)
    {
      ALOGE("ERR no from address in recvfrom...\n");
    }
  } else {
    res = recv(ctrl->sock, buf, buf_len, 0);
  }

  return res;
}

int _stec_recv_timeout(struct stec_ctrl *ctrl, char *buf, size_t buf_len, int sec_timeout)
{
  fd_set fds;
  struct timeval tv;
  int quit = 0;
  int res = -1;

  while (!quit)
  {
    FD_ZERO(&fds);
    FD_SET(ctrl->sock, &fds);

    tv.tv_sec = sec_timeout;
    tv.tv_usec = 0;

    if (select(sizeof(fds)*8, &fds, NULL, NULL, &tv))
    {
      if (FD_ISSET(ctrl->sock, &fds))
      {
        res = __stec_recv(ctrl, buf, buf_len);
        quit = 1;
      } else {
        // No data within timeout
        res = 0;
        quit = 1;
      }
    }
  }

  return res;
}

int _stec_recv(struct stec_ctrl *ctrl, char *buf, size_t buf_len, int sec_timeout)
{
  int res = -1;
  int flags;

#ifdef CONFIG_NL80211_TESTMODE
  if (ctrl->serverside == 0 &&
     ctrl->remote_addr &&
     ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    res = _stec_nl80211testmode_recv(ctrl->nl, buf, buf_len);
  } else
#endif
  {
    flags = fcntl(ctrl->sock, F_GETFL);

    // Set non-blocking
    fcntl(ctrl->sock, F_SETFL, (flags | O_NONBLOCK));

    if (sec_timeout == -1)
    {
      res = __stec_recv(ctrl, buf, buf_len);
    } else {
      res = _stec_recv_timeout(ctrl, buf, buf_len, sec_timeout);
    }

    // Unset non-blocking
    fcntl(ctrl->sock, F_SETFL, flags);
  }

  return res;
}

int stec_recv(struct stec_ctrl *ctrl, char *buf, size_t buf_len, int sec_timeout)
{
  if(!ctrl)
  {
    return -1;
  }

  return _stec_recv(ctrl, buf, buf_len, sec_timeout);
}



int _stec_sendto(struct stec_ctrl *ctrl, const char *buf, size_t buf_len, const struct sockaddr *addr, socklen_t addr_len)
{
  return sendto(ctrl->sock, buf, buf_len, 0, addr, addr_len);
}

int stec_sendto(struct stec_ctrl *ctrl, const char *buf, size_t buf_len, const struct sockaddr *addr, socklen_t addr_len)
{
  if (_stec_sendto(ctrl, buf, buf_len, addr, addr_len) < 0)
  {
    return -1;
  }

  return 0;
}

int _stec_send(struct stec_ctrl *ctrl, const char *buf, size_t buf_len)
{
  int res = -1;

#ifdef CONFIG_NL80211_TESTMODE
  if (ctrl->serverside == 0 &&
     ctrl->remote_addr &&
     ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    res = _stec_nl80211testmode_send(ctrl->nl, buf, buf_len);
  } else
#endif
  {
    if (ctrl->serverside == 1)
    {
      res = _stec_sendto(ctrl, buf, buf_len, ctrl->last_client_addr, ctrl->last_client_addr_len);
    } else {
      res = send(ctrl->sock, buf, buf_len, 0);
    }
  }

  return res;
}


int stec_send(struct stec_ctrl *ctrl, const char *buf, size_t buf_len)
{
  int res = -1;

  if(!ctrl)
  {
    return -1;
  }

  res = _stec_send(ctrl, buf, buf_len);

#ifdef CONFIG_NL80211_TESTMODE
  // We need to receive the ACK
  if (ctrl->serverside == 0 &&
     ctrl->remote_addr &&
     ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    // We send in buf = NULL indicating that there should
    // be no handler when running receive for TESTMODE.
    // We just want the ACK for the send.
    res = _stec_recv(ctrl, NULL, buf_len, -1);
  }
#endif

  return res;
}


int stec_sendsync(struct stec_ctrl *ctrl, const char *req, size_t req_len, char *resp, size_t resp_len, int sec_timeout)
{
  int res = 0;

  if(!ctrl)
  {
    return -1;
  }

  res = _stec_send(ctrl, req, req_len);

  if(res < 0)
  {
    return -1;
  }

  res = _stec_recv(ctrl, resp, resp_len, sec_timeout);

  return res;
}


int stec_get_last_recvfrom(struct stec_ctrl *ctrl, struct sockaddr *addr, socklen_t addr_len)
{
  if(!ctrl)
  {
    return -1;
  }

  if (ctrl->last_client_addr && (addr_len >= ctrl->last_client_addr_len))
  {
    memcpy(addr, ctrl->last_client_addr, ctrl->last_client_addr_len);
    return ctrl->last_client_addr_len;
  }

  return -1;
}


/*******
 * CLIENT SIDE
 * Event related
 *******/

int stec_subscribe(struct stec_ctrl *ctrl)
{
  size_t reply_len = 20;
  char reply[20];
  int res = 0;

  if(!ctrl)
  {
    return -1;
  }

#ifdef CONFIG_NL80211_TESTMODE
  if (ctrl->remote_addr &&
      ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    res = _stec_nl80211testmode_event_subscribe(ctrl->nl);

    if (res >= 0)
    {
      // Point to nl event socket as main sock (for get fd)
      ctrl->sock = res;
      res = 0;
    }
  }
  else
#endif
  {
    res = stec_sendsync(ctrl, cmd_subscribe, strlen(cmd_subscribe)+1, reply, reply_len, 3);

    if (res == strlen(cmdresp_ok)+1 && !strncmp(reply, cmdresp_ok, res))
    {
      // OK
      res = 0;
    } else {
      // Failed
     res = -1;
    }
  }

  return res;
}

int stec_unsubscribe(struct stec_ctrl *ctrl)
{
  size_t reply_len = 20;
  char reply[20];
  int res = 0;

  if(!ctrl)
  {
    return -1;
  }

#ifdef CONFIG_NL80211_TESTMODE
  if (ctrl->remote_addr &&
      ctrl->remote_addr->sa_family == AF_NETLINK)
  {
    res = _stec_nl80211testmode_event_unsubscribe(ctrl->nl);
  }
  else
#endif
  {
    res = stec_sendsync(ctrl, cmd_unsubscribe, strlen(cmd_unsubscribe)+1, reply, reply_len, 3);

    if (res == strlen(cmdresp_ok)+1 && !strncmp(reply, cmdresp_ok, res))
    {
      // OK
      res = 0;
    } else {
      // Failed
      res = -1;
    }
  }

  return res;
}

#ifdef CONFIG_NL80211_TESTMODE
int stec_nl80211testmode_event_recv(struct stec_ctrl *ctrl, char *buf, size_t buf_len)
{
  return _stec_nl80211testmode_event_recv(ctrl->nl, buf, buf_len);
}
#endif

/*******
 * SERVER SIDE
 * Event related
 *******/

/*
 * Checks all incoming packets for the
 * SUBSCRIBE or UNSUBSCRIBE texts. All
 * unknown packets are thrown.
 */
int stec_process_recv(struct stec_ctrl *ctrl)
{
  char buf[20];
  int res;
  socklen_t fromlen = ctrl->last_client_addr_len;

  memset(ctrl->last_client_addr, 0, ctrl->last_client_addr_len);

  // Receive message, discarding excess data in message (that doesn't fit in buffer)
  res = _stec_recvfrom(ctrl, buf, 20, ctrl->last_client_addr, &fromlen);

  if (!ctrl->subscribe)
    return -1;

  if (res < 0)
  {
    // Error occured
    return res;
  }

  if (res == strlen(cmd_subscribe)+1 && !strncmp(buf, cmd_subscribe, res))
  {
    // SUBSCRIBE
    if (subscribe_add(ctrl->subscribe, ctrl->last_client_addr, fromlen) == 0)
    {
      res = _stec_sendto(ctrl, cmdresp_ok, strlen(cmdresp_ok)+1, ctrl->last_client_addr, fromlen);
    } else {
      res = _stec_sendto(ctrl, cmdresp_fail, strlen(cmdresp_fail)+1, ctrl->last_client_addr, fromlen);
    }
  }

  else if (res == strlen(cmd_unsubscribe)+1 && !strncmp(buf, cmd_unsubscribe, res))
  {
    // UNSUBSCRIBE
    if (subscribe_remove(ctrl->subscribe, ctrl->last_client_addr, fromlen) == 0)
    {
      res = _stec_sendto(ctrl, cmdresp_ok, strlen(cmdresp_ok)+1, ctrl->last_client_addr, fromlen);
    } else {
      res = _stec_sendto(ctrl, cmdresp_fail, strlen(cmdresp_fail)+1, ctrl->last_client_addr, fromlen);
    }
  }

  if (res < 0)
  {
    ALOGE("Subscribe / unsubscribe response failed\n");
  }

  return res;
}


int stec_sendevent(struct stec_ctrl *ctrl, const char *buf, size_t buf_len)
{
  struct subscribe_client_t *c;

  if (!ctrl->subscribe)
    return -1;

  for (c = subscribe_get_first(ctrl->subscribe); c != NULL; c = subscribe_get_next(c))
  {
    if (stec_sendeventto(ctrl, buf, buf_len, subscribe_get_addr(c), subscribe_get_addr_len(c)) != 0)
    {
      // Remove subscriber since we failed sending to it
      subscribe_remove(ctrl->subscribe, subscribe_get_addr(c), subscribe_get_addr_len(c));
    }
  }

  return 0;
}


int stec_sendeventto(struct stec_ctrl *ctrl, const char *buf, size_t buf_len, const struct sockaddr *addr, socklen_t addr_len)
{
  if (!ctrl->subscribe)
    return -1;

  if (_stec_sendto(ctrl, buf, buf_len, addr, addr_len) < 0)
  {
    return -1;
  }

  return 0;
}

