/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util
 * Description     : util for mal
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "util.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int fd_gpds_ts = -1;
static int fd_netlnk_ts = -1;
static int fd_gpds_st = -1;
static int fd_netlnk_st = -1;
static int fd_sim_st = -1;
static int fd_sim_ts = -1;

/*
 * Create sockets that are specific for MAL testing
 */
int createSockets(int isServer) {

  printf("Setting up sockets (isServer=%d)\n", isServer);
  if (fd_gpds_ts < 0) {
    fd_gpds_ts = createsocket(isServer ? 0 : 1,
        (char*) psccd_test_socket_name_gpds_ts);
    if (fd_gpds_ts < 0)
      goto error_socket;
  }

  if (fd_gpds_st < 0) {
    fd_gpds_st = createsocket(isServer ? 1 : 0,
        (char*) psccd_test_socket_name_gpds_st);
    if (fd_gpds_st < 0)
      goto error_socket;
  }

  if (fd_netlnk_ts < 0) {
    fd_netlnk_ts = createsocket(isServer ? 0 : 1,
        (char*) psccd_test_socket_name_netlnk_ts);
    if (fd_netlnk_ts < 0)
      goto error_socket;
  }

  if (fd_netlnk_st < 0) {
    fd_netlnk_st = createsocket(isServer ? 1 : 0,
        (char*) psccd_test_socket_name_netlnk_st);
    if (fd_netlnk_st < 0)
      goto error_socket;
  }

  if (fd_sim_ts < 0) {
    fd_sim_ts = createsocket(isServer ? 0 : 1,
        (char*) psccd_test_socket_name_sim_ts);
    if (fd_sim_ts < 0)
      goto error_socket;
  }

  if (fd_sim_st < 0) {
    fd_sim_st = createsocket(isServer ? 1 : 0,
        (char*) psccd_test_socket_name_sim_st);
    if (fd_sim_st < 0)
      goto error_socket;
  }

  /* here all sockets are setup */

  return 0;

  error_socket:
  /* if the sockets havnt been created closeSockets will handle that too */
  closeSockets();
  return -1;
}

/*
 * get gpds fds
 */
void getGPDSFd(int* gpds_fd, int* netlnk_fd)
{
  /* the returned values should be the ones pointing to the stubs */
  *gpds_fd = fd_gpds_ts;
  *netlnk_fd = fd_netlnk_ts;
}

/*
 * get sim fd
 */
void getSimFd(int* sim_fd)
{
  /* the returned values should be the ones pointing to the stubs */
  *sim_fd = fd_sim_ts;
}

/*
 * Close sockets
 */
void closeSockets() {
  printf("Closing sockets\n");

  if (fd_gpds_ts > -1) {
    close(fd_gpds_ts);
  }
  if (fd_netlnk_ts > -1) {
    close(fd_netlnk_ts);
  }
  if (fd_gpds_st > -1) {
    close(fd_gpds_st);
  }
  if (fd_netlnk_st > -1) {
    close(fd_netlnk_st);
  }
  fd_gpds_ts = -1;
  fd_netlnk_ts = -1;
  fd_gpds_st = -1;
  fd_netlnk_st = -1;
  return;
}

/**
 *  Method used to wait for and fetch an event on the gpds fd
 */
int waitAndReadGPDSEvent(psccd_test_direction_t dir,
    psccd_test_request_id_t request_id, void** data_pp) {
  void* tempdata_p;
  psccd_test_msghdr_t message;
  int size;
  int fd = (dir == PSCCD_TEST_TEST_STUB ? fd_gpds_ts : fd_gpds_st);
  char* name =
      (dir == PSCCD_TEST_TEST_STUB ? (char*) psccd_test_socket_name_gpds_ts
          : (char*) psccd_test_socket_name_gpds_st);

  if (waitForEvent(fd) < 0){
    return -1;
  }
  if (readFromFd(&size, &tempdata_p, fd, (char*) name) < 0)
  {
    return -1;
  }

  /* extract message header */
  memcpy(&message, tempdata_p, sizeof(psccd_test_msghdr_t));
  /* check if it was the correct event */
  if (message.request_id != request_id) {
    printf("waitAndReadGPDSEvent(): Wrong response received, expected %d, got %d\n", request_id,
        message.request_id);
    free(tempdata_p);
    return -1;
  }
  /* move the memory pointer to point to the response data */
  *data_pp = malloc(size - sizeof(psccd_test_msghdr_t));
  memcpy(*data_pp, tempdata_p + sizeof(psccd_test_msghdr_t), size
      - sizeof(psccd_test_msghdr_t));
  free(tempdata_p);

  return 0;
}

/**
 *  Method used to wait for and fetch an event on the netlnk fd
 */
int waitAndReadNetlnkEvent(psccd_test_direction_t to,
    psccd_test_request_id_t request_id, void** data_pp) {
  void* tempdata_p;
  psccd_test_msghdr_t message;
  int size;
  int fd = (to == PSCCD_TEST_TEST_STUB ? fd_netlnk_ts : fd_netlnk_st);
  char* name =
      (to == PSCCD_TEST_TEST_STUB ? (char*) psccd_test_socket_name_netlnk_ts
          : (char*) psccd_test_socket_name_netlnk_st);

  if (waitForEvent(fd) < 0)
    return -1;
  if (readFromFd(&size, &tempdata_p, fd, (char*) name) < 0)
    return -1;

  /* extract message header */
  memcpy(&message, tempdata_p, sizeof(psccd_test_msghdr_t));
  /* check if it was the correct event */
  if (message.request_id != request_id) {
    printf("waitAndReadNetlnkEvent: Wrong response received, expected %d, got %d\n", request_id,
        message.request_id);
    free(tempdata_p);
    return -1;
  }
  /* move the memory pointer to point to the response data */
  *data_pp = malloc(size - sizeof(psccd_test_msghdr_t));
  memcpy(*data_pp, tempdata_p + sizeof(psccd_test_msghdr_t), size
      - sizeof(psccd_test_msghdr_t));
  free(tempdata_p);
  return 0;
}

/**
 *  Method used to wait for and fetch an event on the netlnk fd
 */
int waitAndReadSimEvent(psccd_test_direction_t to,
    psccd_test_request_id_t request_id, void** data_pp) {
  void* tempdata_p;
  psccd_test_msghdr_t message;
  int size;
  int fd = (to == PSCCD_TEST_TEST_STUB ? fd_sim_ts : fd_sim_st);
  char* name =
      (to == PSCCD_TEST_TEST_STUB ? (char*) psccd_test_socket_name_sim_ts
          : (char*) psccd_test_socket_name_sim_st);

  if (waitForEvent(fd) < 0)
    return -1;
  if (readFromFd(&size, &tempdata_p, fd, (char*) name) < 0)
    return -1;

  /* extract message header */
  memcpy(&message, tempdata_p, sizeof(psccd_test_msghdr_t));
  /* check if it was the correct event */
  if (message.request_id != request_id) {
    printf("waitAndReadSIMEvent: Wrong response received, expected %d, got %d\n", request_id,
        message.request_id);
    free(tempdata_p);
    return -1;
  }
  /* move the memory pointer to point to the response data */
  *data_pp = malloc(size - sizeof(psccd_test_msghdr_t));
  memcpy(*data_pp, tempdata_p + sizeof(psccd_test_msghdr_t), size
      - sizeof(psccd_test_msghdr_t));
  free(tempdata_p);
  return 0;
}

/**
 *  Method used to send data to gpds fd
 */
int sendToGPDS(psccd_test_direction_t dir, psccd_test_request_id_t request_id,
    void* data_p, int datasize) {
  return sendToFd(request_id, data_p, datasize,
      dir == PSCCD_TEST_TEST_STUB ? fd_gpds_ts : fd_gpds_st, dir
          == PSCCD_TEST_TEST_STUB ? (char*) psccd_test_socket_name_gpds_ts
          : (char*) psccd_test_socket_name_gpds_st);
}

/**
 *  Method used to send data to netlnk fd
 */
int sendToNetlnk(psccd_test_direction_t dir,
    psccd_test_request_id_t request_id, void* data_p, int datasize) {
  return sendToFd(request_id, data_p, datasize,
      dir == PSCCD_TEST_TEST_STUB ? fd_netlnk_ts : fd_netlnk_st, dir
          == PSCCD_TEST_TEST_STUB ? (char*) psccd_test_socket_name_netlnk_ts
          : (char*) psccd_test_socket_name_netlnk_st);
}

/**
 *  Method used to send data to netlnk fd
 */
int sendToSim(psccd_test_direction_t dir,
    psccd_test_request_id_t request_id, void* data_p, int datasize) {
  return sendToFd(request_id, data_p, datasize,
      dir == PSCCD_TEST_TEST_STUB ? fd_sim_ts : fd_sim_st, dir
          == PSCCD_TEST_TEST_STUB ? (char*) psccd_test_socket_name_sim_ts
          : (char*) psccd_test_socket_name_sim_st);
}
