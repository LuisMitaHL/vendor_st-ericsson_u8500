/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util
 * Description     : util for mfl
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#ifndef UTIL_MFL_H_
#define UTIL_MFL_H_

#include "tc_mfl_basic.h"
#include "sim.h"
#include "sim_internal.h"
#include <stdint.h>
/**********************************************/
/********* Test framework payload structs *****/
/**********************************************/


/* one socket for for sim communication stub -> test */
static const char psccd_test_socket_name_sim_st[] = "/tmp/pscc_test_sim_socket_st";
/* one socket for for sim communication test -> stub */
static const char psccd_test_socket_name_sim_ts[] = "/tmp/pscc_test_sim_socket_ts";


/*************** sim *********************/

typedef struct {
  ste_sim_closure_t closure;
} PSCC_test_ste_sim_new_st_req_t;

typedef struct {
  ste_sim_t ste_sim;
} PSCC_test_ste_sim_new_st_resp_t;

typedef struct {
  ste_sim_t sim;
  uintptr_t client_tag;
} PSCC_test_ste_sim_connect_req_t;

typedef struct {
  int retval;
} PSCC_test_ste_sim_connect_resp_t;

typedef struct {
  ste_sim_t sim;
} PSCC_test_ste_sim_fd_req_t;

typedef struct {
  int retval;
} PSCC_test_ste_sim_fd_resp_t;

typedef struct {
  ste_sim_t sim;
  uintptr_t client_tag;
} PSCC_test_ste_sim_disconnect_req_t;

typedef struct {
  int retval;
} PSCC_test_ste_sim_disconnect_resp_t;

typedef struct {
  ste_sim_t sim;
  uintptr_t client_tag;
} PSCC_test_ste_sim_delete_req_t;

typedef struct {
} PSCC_test_ste_sim_delete_resp_t;

typedef struct {
  ste_sim_t cat;
  uintptr_t client_tag;
  ste_cat_call_control_t cc_p;
  char str_p[100];
  uint32_t strlen;
} PSCC_test_ste_cat_call_control_req_t;

typedef struct {
  int retval;
} PSCC_test_ste_cat_call_control_resp_t;

typedef struct {
  ste_sim_t sim;
} PSCC_test_ste_sim_read_req_t;

typedef struct {
  int retval;
} PSCC_test_ste_sim_read_resp_t;

typedef struct {
  ste_sim_t uicc;
  uintptr_t client_tag;
  sim_service_type_t service_type;
} PSCC_test_ste_uicc_get_service_availability_req_t;

typedef struct {
  uicc_request_status_t retval;
} PSCC_test_ste_uicc_get_service_availability_resp_t;

typedef struct {
  int cause;
  uintptr_t client_tag;
  char data[100];
  char user_data[100];
} PSCC_test_ste_sim_cb_t;

/**********************************************/
/********* methods used in test fw for mfl ****/
/**********************************************/

/*
 * create the sockets
 */
int createSockets(int isServer) ;

/*
 * close sockets
 */
void closeSockets();

/*
 * get sim fds
 */
void getSimFd(int* sim_fd);

/**
 *  Method used to wait for and fetch an event on the sim fd
 */
int waitAndReadSimEvent(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void** data_pp);

/**
 *  Method used to send to sim fd
 */
int sendToSim(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void* data_p, int datasize);

#endif /* UTIL_MFL_H_ */
