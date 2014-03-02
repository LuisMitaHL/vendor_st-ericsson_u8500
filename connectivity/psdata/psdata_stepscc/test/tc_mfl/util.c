/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util
 * Description     : util for mfl
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include "util.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int createSockets(int isServer) {

  printf("Setting up sockets (isServer=%d)\n", isServer);

  return 0;
}

/*
 * Close sockets
 */
void closeSockets() {
  printf("Closing test and stub sockets\n");

  return;
}


/**
 *  Method used to send data to netlnk fd
 */
int sendToSim(psccd_test_direction_t dir,
    psccd_test_request_id_t request_id, void* data_p, int datasize) {
  return 0;
}

void getSimFd(int* sim_fd)
{
  return;
}

/**
 *  Method used to wait for and fetch an event on the netlnk fd
 */
int waitAndReadSimEvent(psccd_test_direction_t to,
    psccd_test_request_id_t request_id, void** data_pp) {
  return 0;
}
