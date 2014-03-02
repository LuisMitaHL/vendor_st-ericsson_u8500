/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : mh_mce.c 
 * Description     : Sim modem initialization.
 * 
 * Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
 * 
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// For select
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "sim_unused.h"
#include "mal_mce.h"
#include "sim.h"

#define LOG_TAG "RIL MHMCEL"
#include <cutils/log.h>
#if 0
#ifndef LOGI
#define LOGI(format, ...) do { printf("Info: [" LOG_TAG "] " format "\n", ## __VA_ARGS__); } while(0)
#endif
#endif

static void sim_event_callback(int cause, uintptr_t client_tag, void *data, void *vuser_data) {
  LOGI("mhmcel got cause %s\n", ste_sim_cause_name[cause]);
}

extern void sim_maluicc_indication_response_trace(int message_id, void* data, int mal_error, void* client_tag);

static void mce_event_callback(int event_id, void *data, mal_mce_error_type error_code, void *client_tag)
{
  switch(event_id) {
  case MAL_MCE_RF_STATE_RESP:
  case MAL_MCE_RF_STATE_IND:
  case MAL_MCE_RF_STATE_QUERY_RESP:
  default: 
    LOGI("malclient cb: event 0x%x\n", event_id);
    break;
  }
}

int main(int argc, char **argv)
{
  int res, i;
  int fd_mce, fd_sim;
  fd_set rfds;
  struct timeval tv;
  ste_sim_t *uicc;
  ste_sim_closure_t pc;
  int maxfd;

  LOGI("start\n");
  FD_ZERO(&rfds);
  res = mal_mce_init(&fd_mce);
  if (res != 0) {
      LOGI("mal_mce_init failed!\n"); goto Done;
  }
  LOGI("mal_mce_init success!\n");
  FD_SET(fd_mce, &rfds);
  maxfd = fd_mce;
  res = mal_mce_register_callback(mce_event_callback);
  if (res != 0) {
      LOGI("mal_mce_register_callback failed!\n"); goto Done;
  }
  LOGI("mal_mce_register_callback success!\n");
/*   res = mal_mce_rf_on(&maxfd); // Just any non-null use for client tag */
/*   if (res != 0) { */
/*       LOGI("mal_mce_rf_on failed!\n"); goto Done; */
/*   } */
/*   LOGI("mal_mce_rf_on success!\n"); */
  res = mal_mce_config();
  if (res != 0) {
      LOGI("mal_mce_config failed!\n"); goto Done;
  }
  LOGI("mal_mce_config success!\n");
  LOGI("Modem init done.!\n");
  pc.func = sim_event_callback;
  pc.user_data = NULL;
  uicc = ste_sim_new_st(&pc);
  // If simd is not yet up and running we retry connect a few times
  for (i=0; i<10; i++) {
    res = ste_sim_connect(uicc, 1);
    LOGI("Connecting to simd...\n");
    if (res == STE_SIM_SUCCESS) {
      LOGI("connect to simd ok\n");
      break;
    } else if (res == STE_SIM_ERROR_SOCKET) {
      LOGI("simd socket error\n");
      sleep(1);
    } else {
      LOGI("simd connect error %d\n", res);
      sleep(1);
    }
  }
  if (res != 0) {
      LOGI("Connect failed\n"); goto Done;
  }
  res = ste_sim_ping(uicc, 1);
  if (res != 0) {
      LOGI("Ping failed\n"); goto Done;
  }
  fd_sim = ste_sim_fd(uicc);
  FD_SET(fd_sim, &rfds);
  if ( fd_sim > maxfd ) maxfd = fd_sim;
  res = ste_sim_startup(uicc, 1);
  if (res != 0) {
      LOGI("Startup failed\n"); goto Done;
  }
  if (res != 0) goto Done;
  do {
    fd_set temp = rfds;
    LOGI("mhmcel: Waiting on select!\n");
    res = select(maxfd+1, &temp, NULL, NULL, NULL);
    if (res == -1) {
      perror("select()");
      goto Done;
    } else if (FD_ISSET(fd_mce, &temp)) {
      LOGI("Select on mce\n");
      mal_mce_response_handler();
    } else if (FD_ISSET(fd_sim, &temp)) {
      LOGI("Select on sim\n");
      res = ste_sim_read(uicc);
      if (res != 0){
	LOGI("ste_sim_read FAILED\n");
        FD_CLR(fd_sim,&rfds);
      }
    }
  } while(1);

 Done:
  for (i = 0; i < 30; i++) {
    sleep(1);
    LOGI("mhmcel: Exiting main!\n");
  }
  exit(EXIT_SUCCESS);
}
