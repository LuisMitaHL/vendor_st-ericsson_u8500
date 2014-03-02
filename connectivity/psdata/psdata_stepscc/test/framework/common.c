/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : common.c
 * Description     : Common test fw functions
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>

#include "common.h"
#include <linux/un.h>
#include "stecom_i.h"
#include "libstecom.h"
#include "string.h"

static struct stec_ctrl *ctrl_handle_p = NULL;
static struct stec_ctrl *event_handle_p = NULL;

/**
 *  Method used to initalize stecom
 */
int initStecom() {
  struct sockaddr_un addr_un;
  if (ctrl_handle_p != NULL)
  {
    printf("initStecom(): ctrl_handle_p already initialized \n");
    return -1;
  }
  memset(&addr_un, 0, sizeof(addr_un));
  addr_un.sun_family = AF_UNIX;
  strncpy(addr_un.sun_path, pscc_ctrl, UNIX_PATH_MAX);
  ctrl_handle_p = stec_open((struct sockaddr*) &addr_un, sizeof(addr_un));
  if (ctrl_handle_p == NULL) {
    printf("initStecom(): ctrl_handle_p == NULL\n");
    goto error;
  }

  if (event_handle_p != NULL)
  {
    printf("initStecom(): event_handle_p already initialized \n");
    goto error;
  }
  memset(&addr_un, 0, sizeof(addr_un));
  addr_un.sun_family = AF_UNIX;
  strncpy(addr_un.sun_path, pscc_event, UNIX_PATH_MAX);
  event_handle_p = stec_open((struct sockaddr*) &addr_un, sizeof(addr_un));
  if (event_handle_p == NULL) {
    printf("initStecom(): event_handle_p == NULL\n");
    goto error;
  }
  if (stec_subscribe(event_handle_p) != 0) {
      printf("initStecom(): failed to activate event subscription");
      goto error;
  }

  return 0;

error:
  return -1;
}

/**
 *  Method used to close stecom
 */
void closeStecom() {
  stec_close(ctrl_handle_p);
  stec_close(event_handle_p);
  ctrl_handle_p = NULL;
  event_handle_p = NULL;
}

/**
 *  Method used to send to stecom
 */
int sendToStecomCtrl(mpl_msg_t *pscc_msg_p) {

  char *packed_request = NULL;
  size_t packed_request_len;
  int ret;
  mpl_msg_pack((mpl_msg_t*) pscc_msg_p, &packed_request, &packed_request_len);

  ret = stec_send(ctrl_handle_p, packed_request, packed_request_len);
  if (ret < 0) {
    printf("sendToStecom(): stec_send returned <0\n");
    goto error;
  }
  mpl_msg_buf_free((mpl_msg_t*) packed_request);

  return 0;

  error: return -1;
}

/**
 *  Method used to wait and read stecom response
 */
int waitAndReadStecomResp(mpl_msg_t* resp_p) {
  char buf[MAX_READ_BUFFER_SIZE];
  int size;

  if (waitForEvent((int) ctrl_handle_p->sock) < 0) {
    printf("waitAndReadStecomEvent : waitForEvent failed\n");
    goto error;
  }
  size = stec_recv(ctrl_handle_p, buf, 1000, TIMEOUTTIME);
  if (size < 0) {
    printf("waitAndReadStecomEvent : stec_recv failed\n");
    goto error;
  }
  if (mpl_msg_unpack(buf, size, resp_p) < 0) {
    printf("waitAndReadStecomEvent : mpl_msg_unpack failed\n");
    goto error;
  }

  return 0;
  error: return -1;
}

/**
 *  Method used to wait and read stecom event
 */
int waitAndReadStecomEvent(mpl_msg_t* resp_p) {
  char buf[MAX_READ_BUFFER_SIZE];
  int size;

  if (waitForEvent((int) event_handle_p->sock) < 0) {
    printf("waitAndReadStecomEvent : waitForEvent failed\n");
    goto error;
  }
  size = stec_recv(event_handle_p, buf, 1000, TIMEOUTTIME);
  if (size < 0) {
    printf("waitAndReadStecomEvent : stec_recv failed\n");
    goto error;
  }
  if (mpl_msg_unpack(buf, size, resp_p) < 0) {
    printf("waitAndReadStecomEvent : mpl_msg_unpack failed\n");
    goto error;
  }

  return 0;
  error: return -1;
}

/**
 *  Logfunction that is being registered to stecom
 */
void my_log_func(void* user_p, int level, const char* file, int line,
    const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vsyslog(level, format, ap);
  vprintf(format, ap);
  va_end(ap);
}

/*
 * Method for creating a socket
 */
int createsocket(int isServer, char* socketname) {
  int fd;
  fd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if (fd < 0) {
    printf("Failed to create socket\n");
    return -1;
  }

  if (isServer) {
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketname, sizeof(addr.sun_path));
    /* Bind the socket */
    unlink(addr.sun_path);
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
      printf("bind() returns error(%d)=\"%s\"\n", errno, strerror(errno));
      close(fd);
      return -1;
    }
  }

  return fd;
}

/*
 * Method for sending data to a fd
 */
int sendToFd(psccd_test_request_id_t request_id, void* data_p, int datasize,
    int fd, char* socketname) {

  psccd_test_msghdr_t* psccd_test_message_p;
  void* message_data_p = NULL;
  int n = 0;
  struct sockaddr_un addr;

  if (fd < 0)
  {
    printf("sendToFd: fd < 0\n");
    return -1;
  }

  message_data_p = malloc(datasize + sizeof(psccd_test_msghdr_t));
  memset(message_data_p, 0, sizeof(*message_data_p));
  psccd_test_message_p = message_data_p;
  psccd_test_message_p->request_id = request_id;
  memcpy(message_data_p + sizeof(psccd_test_msghdr_t), data_p, datasize);

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socketname, sizeof(addr.sun_path));

  n = sendto(fd, message_data_p, datasize + sizeof(psccd_test_msghdr_t), 0,
      (struct sockaddr *) &addr, sizeof(addr));
  if (n < 0) {
    printf("sendto returns error(%d)=\"%s\"\n", errno, strerror(errno));
    free(message_data_p);
    return -1;
  }
  free(message_data_p);
  return 0;
}

/*
 * Method for listening to a fd
 */
int waitForEvent(int fd) {
  fd_set rfds;
  struct timeval tv, *tvp;
  int result = 0;

  if (fd < 0) {
    printf("waitForEvent: fd < 0 \n");
    return -1;
  }

  /* Set up timeout structure */
  if (TIMEOUTTIME < 0) {
    tvp = NULL;
  } else {
    tv.tv_sec = TIMEOUTTIME / 1000;
    tv.tv_usec = (TIMEOUTTIME % 1000) * 1000;
    tvp = &tv;
  }

  FD_ZERO(&rfds);
  FD_SET(fd, &rfds );

  result = select(fd + 1, &rfds, NULL, NULL, tvp);

  if (result == -EINTR) {
    printf("waitForEvent: select returned because of a signal\n");
    return -1;
  } else if (result == 0) {
    printf("waitForEvent: Timeout waiting for event\n");
    return -1;
  } else if (FD_ISSET(fd, &rfds)) {
    /* we got an event before timeout */
    return 0;
  } else {
    printf("waitForEvent: Error, select returned without fd set\n");
    return -1;
  }
}

/**
 * Method used for reading data from a fd
 * if successful return data_p must be freed
 */
int readFromFd(int* readsize_p, void** data_pp, int fd, char* socketname) {
  int n = 0;
  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;

  if (fd < 0)
  {
    printf("readFromFd: fd < 0 \n");
    return -1;
  }

  if (data_pp == NULL) {
    printf("readFromFd: data_pp == NULL\n");
    return -1;
  }
  *data_pp = malloc(MAX_READ_BUFFER_SIZE);
  memset(*data_pp, 0, MAX_READ_BUFFER_SIZE);
  memset(&addr, 0, sizeof(addr));

  strncpy(addr.sun_path, socketname, sizeof(addr.sun_path));
  int addrlen = sizeof(addr);

  n = recvfrom(fd, *data_pp, MAX_READ_BUFFER_SIZE, 0,
      (struct sockaddr *) &addr, (socklen_t *) &addrlen);
  if (n < 0) {
    printf("readFromFd: recvfrom returns error(%d)=\"%s\"\n", errno, strerror(errno));
    free(*data_pp);
    return -1;
  }
  if (n == MAX_READ_BUFFER_SIZE) {
    printf("readFromFd: Warning, read full buffer. Extend buffersize.\n");
  }
  *readsize_p = n;
  return 0;
}
