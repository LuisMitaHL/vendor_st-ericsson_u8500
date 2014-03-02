/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util.h
 * Description     : util for mal
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#ifndef UTIL_MAL_H_
#define UTIL_MAL_H_

#include "tc_mal_basic.h"
#include "sim.h"
#include "sim_internal.h"
#include "mal_gpds.h"
#include "shm_netlnk.h"
#include <stdint.h>

/**********************************************/
/********* Sockets used in test fw for mal ****/
/**********************************************/

/* one socket for mal_gpds communication stub -> test */
static const char psccd_test_socket_name_gpds_st[] = "/tmp/pscc_test_gpds_socket_st";
/* and one socket for netlnk communication stub -> test */
static const char psccd_test_socket_name_netlnk_st[] = "/tmp/pscc_test_netlnk_socket_st";
/* one socket for for sim communication stub -> test */
static const char psccd_test_socket_name_sim_st[] = "/tmp/pscc_test_sim_socket_st";
/* one socket for mal_gpds communication test -> stub */
static const char psccd_test_socket_name_gpds_ts[] = "/tmp/pscc_test_gpds_socket_ts";
/* and one socket for netlnk communication test -> stub */
static const char psccd_test_socket_name_netlnk_ts[] = "/tmp/pscc_test_netlnk_socket_ts";
/* one socket for for sim communication test -> stub */
static const char psccd_test_socket_name_sim_ts[] = "/tmp/pscc_test_sim_socket_ts";


#define IPV4_MAP   (MAL_GPDS_IPV4_ADDRESS_PRESENT | MAL_GPDS_IPV4_PDNS_PRESENT | MAL_GPDS_IPV4_SDNS_PRESENT)
#define IPV6_MAP   (MAL_GPDS_IPV6_ADDRESS_PRESENT | MAL_GPDS_IPV6_PDNS_PRESENT | MAL_GPDS_IPV6_SDNS_PRESENT)
#define IPV4V6_MAP (IPV4_MAP | IPV6_MAP)

/**********************************************/
/********* Test framework payload structs *****/
/**********************************************/

/*************** mal *********************/
typedef struct {
} PSCC_test_mal_gpds_init_req_t;

typedef struct {
  int retval;
} PSCC_test_mal_gpds_init_resp_t;

typedef struct {
} PSCC_test_mal_gpds_deinit_req_t;

typedef struct {
} PSCC_test_mal_gpds_deinit_resp_t;

typedef struct {
  mal_gpds_event_cb_t event_cb;
} PSCC_test_mal_gpds_register_callback_req_t;

typedef struct {
} PSCC_test_mal_gpds_register_callback_resp_t;

typedef struct {
  uint8_t conn_id;
  mal_gpds_req_ids_t req_id;
  char data[100];
} PSCC_test_mal_gpds_request_req_t;

typedef struct {
  int retval;
} PSCC_test_mal_gpds_request_resp_t;

typedef struct {
  uint8_t conn_id;
  mal_gpds_params_id_t param_id;
  int data[50];
} PSCC_test_mal_gpds_set_param_req_t;

typedef struct {
  int retval;
} PSCC_test_mal_gpds_set_param_resp_t;

typedef struct {
  uint8_t conn_id;
  mal_gpds_params_id_t param_id;
} PSCC_test_mal_gpds_get_param_req_t;

typedef struct {
  int retval;
  int data[50];
  int datasize;
} PSCC_test_mal_gpds_get_param_resp_t;

typedef struct {
  int fd;
} PSCC_test_mal_gpds_response_handler_req_t;

typedef struct {
} PSCC_test_mal_gpds_response_handler_resp_t;

typedef struct {
  int32_t netlnkfd;
} PSCC_test_netlnk_socket_recv_req_t;

typedef struct {
  int msg[200];
} PSCC_test_netlnk_socket_recv_resp_t;

typedef struct {
  uint8_t conn_id;
  mal_gpds_event_ids_t event_id;
  char data[100];
  char extradata[100];
} PSCC_test_mal_gpds_cb_t;

typedef struct {
  int32_t netlnkfd;
  int msg;
} PSCC_test_netlnk_cb_t;

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
/********* methods used in test fw for mal ****/
/**********************************************/

/*
 * create the sockets
 */
int createSockets(int isServer) ;

/*
 * get gpds fds
 */
void getGPDSFd(int* gpds_fd, int* netlnk_fd);

/*
 * get sim fds
 */
void getSimFd(int* sim_fd);

/*
 * close sockets
 */
void closeSockets();

/**
 *  Method used to wait for and fetch an event on the gpds fd
 */
int waitAndReadGPDSEvent(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void** data_pp);

/**
 *  Method used to wait for and fetch an event on the netlnk fd
 */
int waitAndReadNetlnkEvent(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void** data_pp);

/**
 *  Method used to wait for and fetch an event on the sim fd
 */
int waitAndReadSimEvent(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void** data_pp);

/**
 *  Method used to send to gpds fd
 */
int sendToGPDS(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void* data_p, int datasize);

/**
 *  Method used to send to netlnk fd
 */
int sendToNetlnk(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void* data_p, int datasize);

/**
 *  Method used to send to sim fd
 */
int sendToSim(psccd_test_direction_t dir, psccd_test_request_id_t request_id, void* data_p, int datasize);

#endif /* UTIL_MAL_H_ */
