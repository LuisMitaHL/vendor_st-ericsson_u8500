/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : testframework file
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 */

#include "sim.h"
#include "sim_internal.h"
#include "simpb.h"

/* global variable for all test code */
//static ste_simpb_t *ste_simpb_p = NULL;

typedef enum {
    SIMPBD_TEST_STE_UICC_GET_APP_INFO = 0x00,
    SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD,
    SIMPBD_TEST_STE_UICC_SIM_FILE_UPDATE_RECORD,
    SIMPBD_TEST_STE_UICC_SIM_GET_STATE,
    SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS,
    SIMPBD_TEST_STE_UICC_SIM_FILE_GET_FORMAT,
    SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET,
    SIMPBD_TEST_STE_UICC_SIM_FILE_READ_BINARY,
    SIMPBD_TEST_STE_SIM_NEW_ST,
    SIMPBD_TEST_STE_SIM_DELETE,
    SIMPBD_TEST_STE_SIM_CONNECT,
    SIMPBD_TEST_STE_SIM_FD,
    SIMPBD_TEST_STE_SIM_DISCONNECT,
    SIMPBD_TEST_STE_SIM_READ,
    SIMPBD_TEST_STE_SIM_URC,
    SIMPBD_TEST_CB = 0xff // callback id
} simpbd_test_request_id_t;

int sendStubResponse(simpbd_test_request_id_t request_id, void *data_p,
                     int datasize);
int sendCallback(simpbd_test_request_id_t request_id, void *data_p,
                 int datasize);
int getStubEvent(int timeout, simpbd_test_request_id_t request_id,
                 void **data_pp, int *size_p);
int waitForEvent(int timeout, int fd); //Currently only used for callbacks
void ste_simpb_cb(ste_simpb_cause_t     cause,
                  uintptr_t             client_tag, /* client_tag == 0 is UNSOLICITED EVENT */
                  simpbd_request_id_t   request_id,
                  void                 *data_p,
                  ste_simpb_result_t    result,
                  void                 *client_context);

/********** Macros used in test **********/
/* should be used in testapp when sending/receiving to stub */
#define TESTCHECK(a) \
  printf("[%d] [%s] [%s] TESTCHECK: " #a " called \n", getpid(), __FILE__, __FUNCTION__);\
  if(a < 0) {\
  printf("[%d] [%s] [%s]  TESTCHECK: ERROR! " #a " failed \n", getpid(), __FILE__, __FUNCTION__);\
  goto error;\
  };

/* should be used in calls to limbsimpb */
#define TESTSIMPB(a)\
  printf("[%d] [%s] [%s] TESTSIMPB: " #a " called \n", getpid(), __FILE__, __FUNCTION__);\
  if(a != STE_SIMPB_SUCCESS) {\
  printf("[%d] [%s] [%s] TESTSIMPB: ERROR! " #a " failed \n", getpid(), __FILE__, __FUNCTION__);\
  goto error;\
  };

#define TESTSTUB(a)\
  printf("[%d] [%s] [%s] TESTSTUB:" #a " called \n", getpid(), __FILE__, __FUNCTION__);\
  if(a != 0) {\
  printf("[%d] [%s] [%s] TESTSTUB: ERROR! " #a " failed \n", getpid(), __FILE__, __FUNCTION__);\
  goto error;\
  };

#define TESTSIMPB_PRINT(a) \
        printf("[%d] [%s] [%s] TESTSTUB: %s", getpid(), __FILE__, __FUNCTION__, a);

/*****************************************/

/* one socket for test to stub communication */
static const char simpbd_test_socket_name_ts[] = "/tmp/socket/simpbd_ts";
/* and one socket for stub to test communication */
static const char simpbd_test_socket_name_st[] = "/tmp/socket/simpbd_st";

/* This needs to be increased if the packets sent on the test-stub filedescriptors are bigger */
static const int MAX_READ_BUFFER_SIZE = 3000;

typedef enum {
    SIMPB_TEST_SUCCESS, SIMPB_TEST_FAIL
} simpb_test_state_type_t;

typedef simpb_test_state_type_t (*simpb_test_fp_t)(int i, ste_simpb_t *ste_simpb_p);

/*
 * Definition of a testcase
 */
typedef struct {
    char *test_string;
    simpb_test_fp_t simpb_test_fp;
} testcase_t;

/*
 * Struct used as header in all stub - test communication
 */
typedef struct {
    simpbd_test_request_id_t request_id; /* request id. Determines which payload struct to use                    */
} simpb_test_message_t;

/****** sim callback struct (used to send from sim to simpb) *****/
typedef struct {
    int cause;
    uintptr_t client_tag;
    char data[1000]; // reference
    char user_data[1000]; //
} ste_sim_callback_t;

/*** simpb callback struct (used to send from simpb to test) *****/
typedef struct {
    ste_simpb_cause_t       cb_cause;
    uintptr_t               cb_client_tag;
    simpbd_request_id_t     cb_request_id;
    void                    *cb_data_p;
    ste_simpb_result_t      cb_result;
    void                    *cb_client_context;
} ste_simpb_callback_t;

/************** request and response structs ***************/
typedef struct {
    ste_sim_t uicc;
    ste_sim_t *uicc_p;
    uintptr_t client_tag;
} simpbd_test_ste_uicc_get_app_info_req_t;

typedef struct {
    int status;
    sim_app_type_t  app_type;
    int retval;
    uintptr_t client_tag;
} simpbd_test_ste_uicc_get_app_info_resp_t;

typedef struct {
    ste_sim_t uicc;
    uintptr_t client_tag;
    int file_id;
    int record_id;
    int length;
    char *file_path;
    uint8_t *data;
} simpbd_test_ste_uicc_sim_file_update_record_req_t;

typedef struct {
    int retval;
} simpbd_test_ste_uicc_sim_file_update_record_resp_t;

typedef struct {
    ste_sim_t uicc;
    uintptr_t client_tag;
} simpbd_test_ste_uicc_sim_get_state_req_t;

typedef struct {
    int retval;
} simpbd_test_ste_uicc_sim_get_state_resp_t;

typedef struct {
    ste_sim_t uicc;
    uintptr_t client_tag;
} simpbd_test_ste_uicc_sim_card_status_req_t;

typedef struct {
    sim_card_type_t card_type;
    int retval;
    uintptr_t client_tag;
} simpbd_test_ste_uicc_sim_card_status_resp_t;

typedef struct {
    ste_sim_t uicc;
    uintptr_t client_tag;
    int file_id;
    char *file_path;
    ste_uicc_sim_get_file_info_type_t type;
} simpbd_test_ste_uicc_sim_file_get_information_req_t;

typedef struct {
    int retval;
} simpbd_test_ste_uicc_sim_file_get_information_resp_t;

typedef struct {
    ste_sim_t *uicc;
    uintptr_t client_tag;
    int file_id;
    int record_id;
    int length;
    const char *file_path;
} simpbd_test_ste_uicc_sim_file_read_record_req_t;

typedef struct {
    int                                 retval;
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    uint8_t *                           data;                          /**< data. record retrieved. */
    uint8_t                             length;
} simpbd_test_ste_uicc_sim_file_read_record_resp_t;

typedef struct {
    ste_sim_closure_t closure;
    ste_sim_callback *cb_func_p;
} simpbd_test_ste_sim_new_st_req_t;

typedef struct {
    ste_sim_t ste_sim;
    int retval;
} simpbd_test_ste_sim_new_st_resp_t;

typedef struct {
    ste_sim_t ste_sim;
    uintptr_t client_tag;
} simpbd_test_ste_sim_delete_req_t;

typedef struct {
    ste_sim_t ste_sim;
    uintptr_t client_tag;
} simpbd_test_ste_sim_delete_resp_t;

typedef struct {
    ste_sim_t *sim_p;
    ste_sim_t sim;
    uintptr_t client_tag;
} simpbd_test_ste_sim_connect_req_t;

typedef struct {
    ste_sim_t sim;
    int retval;
} simpbd_test_ste_sim_connect_resp_t;

typedef struct {
    ste_sim_t sim;
    int fd;
} simpbd_test_ste_sim_fd_req_t;

typedef struct {
    int fd;
    int retval;
} simpbd_test_ste_sim_fd_resp_t;

typedef struct {
    ste_sim_t cat;
    uintptr_t client_tag;
} simpbd_test_ste_sim_startup_req_t;

typedef struct {
    int retval;
} simpbd_test_ste_sim_startup_resp_t;

typedef struct {
    ste_sim_t sim;
    uintptr_t client_tag;
} simpbd_test_ste_sim_disconnect_req_t;

typedef struct {
    int retval;
    uintptr_t client_tag;
} simpbd_test_ste_sim_disconnect_resp_t;

typedef struct {
    ste_sim_t sim;
} simpbd_test_ste_sim_read_req_t;

typedef struct {
    int retval;
    int size;
    int cause;
    uintptr_t client_tag;
    uint8_t data[1];
} simpbd_test_ste_sim_read_resp_t;

