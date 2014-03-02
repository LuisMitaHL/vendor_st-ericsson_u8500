/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : common.h
 * Description     : Common test fw functions
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "mpl_list.h"
#include "mpl_param.h"
#include "mpl_msg.h"


/**********************************************/
/********* Test framework macros and defines **/
/**********************************************/

/* 0 = no extra, 1 = function eval, 2 = bool eval */
static const int DEBUGLEVEL = 0;

#define PRINTDEBUGENTER(a) \
  if(DEBUGLEVEL == 1)\
    printf("Enter:%s\n", #a);

#define PRINTDEBUGEXIT(a) \
  if(DEBUGLEVEL == 1)\
    printf("Exit:%s\n", #a);

/* should be used to check positive returns */
#define TESTNOTNEG(a) \
  if(DEBUGLEVEL > 0) \
    printf("[%s:%d] TEST: " #a " called \n", __FILE__, __LINE__);\
  if((a) < 0) {\
    printf("[%s:%d] ERROR: " #a " failed \n", __FILE__, __LINE__);\
  goto error;\
  };

/* should be used to check negative returns */
#define TESTNOTPOS(a) \
  if(DEBUGLEVEL > 0) \
    printf("[%s:%d] TEST: " #a " called \n", __FILE__, __LINE__);\
  if((a) > -1) {\
    printf("[%s:%d] ERROR: " #a " failed \n", __FILE__, __LINE__);\
  goto error;\
  };

/* should be used to check boolean evaluations */
#define TESTBOOL(a,b)\
  {\
    int _tempa = (int) (a); \
    int _tempb = (int) (b); \
    if(DEBUGLEVEL > 1) \
      printf("[%s:%d] TESTBOOL: " #a " (%d) vs " #b " (%d)\n", __FILE__, __LINE__, _tempa, _tempb);\
    if(!((_tempa) == (_tempb))) {\
      printf("[%s:%d] ERROR: " #a " (%d) does not match " #b " (%d)\n", __FILE__, __LINE__, _tempa, _tempb);\
    goto error;\
    };\
  }

/* should be used to check boolean evaluations */
#define INITREQANDRESP_TEST(a)\
  PSCC_test_##a##_req_t* request_p;\
  PSCC_test_##a##_resp_t response;\
  memset(&response, 0, sizeof(response));

/* used to ignore values sent from pscc to stub and vice versa */
#define DONT_CARE (-1)

static const char pscc_ctrl[] = "/tmp/pscc_ctrl_server_socket";
static const char pscc_event[] = "/tmp/pscc_event_server_socket";

/* max size of read buffer between sockets */
static const int MAX_READ_BUFFER_SIZE = 3000;
/* timeout time to wait for an event */
static const int TIMEOUTTIME = 1000;

/*
 * Direction used when sending between test and stubs
 */
typedef enum {
  PSCCD_TEST_TEST_STUB,
  PSCCD_TEST_STUB_TEST
} psccd_test_direction_t;

typedef enum {
  PSCCD_TEST_SUCCESS,
  PSCCD_TEST_FAIL
} psccd_test_state_type_t;

/*
 * Enum with all requests from pscc to stubs
 */
typedef enum {
  /* common */
  PSCC_STE_SIM_NEW_ST,
  PSCC_STE_SIM_CONNECT,
  PSCC_STE_SIM_FD,
  PSCC_STE_SIM_DISCONNECT,
  PSCC_STE_SIM_DELETE,
  PSCC_STE_SIM_CALL_CONTROL,
  PSCC_STE_SIM_READ,
  PSCC_STE_UICC_GET_SERVICE_AVAILABILITY,
  PSCC_STE_SIM_CB,
  /* mal specific  */
  PSCC_MAL_GPDS_INIT,
  PSCC_MAL_GPDS_DEINIT,
  PSCC_MAL_GPDS_REGISTER_CALLBACK,
  PSCC_MAL_GPDS_REQUEST,
  PSCC_MAL_GPDS_SET_PARAM,
  PSCC_MAL_GPDS_GET_PARAM,
  PSCC_MAL_GPDS_RESPONSE_HANDLER,
  PSCC_NETLNK_SOCKET_RECV,
  PSCC_MAL_GPDS_CALLBACK,
  PSCC_NETLNK_CALLBACK,
  /* mfl specific */
} psccd_test_request_id_t;

/*
 * Function pointer to a test
 */
typedef psccd_test_state_type_t (*psccd_test_fp_t)(int i);

/*
 * Definition of a testcase
 */
typedef struct {
  char *test_string;
  psccd_test_fp_t psccd_test_fp;
} testcase_t;

/*
 * Struct used as header in messaging
 */
typedef struct {
  psccd_test_request_id_t request_id; /* request id. Determines which payload struct to use                    */
} psccd_test_msghdr_t;


/**********************************************/
/********* Test framework functions ***********/
/**********************************************/

/*
 * Method used to setup stecom (psccd) communication
 */
int initStecom();

/*
 * Method used to close stecom communication
 */
void closeStecom();

/*
 * Method used to send messages to stecom (to psccd)
 */
int sendToStecomCtrl(mpl_msg_t *pscc_msg_p);

/**
 *  Method used to wait for and fetch a response from stecom
 */
int waitAndReadStecomResp(mpl_msg_t* resp_p);

/**
 *  Method used to wait for and fetch an event from stecom
 */
int waitAndReadStecomEvent(mpl_msg_t* resp_p);

/*
 * Logging function to register to stecom
 */
void my_log_func(void* user_p, int level, const char* file, int line, const char *format, ...);


/*******************/
/* general methods */
/*******************/
/*
 * Method for creating a socket
 */
int createsocket(int isServer, char* socketname);

/*
 * Method for sending data to a fd
 */
int sendToFd(psccd_test_request_id_t request_id, void* data_p, int datasize,
    int fd, char* socketname);

/*
 * Method for listening to a fd
 */
int waitForEvent(int fd);

/**
 * Method used for reading data from a fd
 * if successful return data_p must be freed
 */
int readFromFd(int* readsize_p, void** data_pp, int fd, char* socketname);


#endif /* CONFIG_H_ */
