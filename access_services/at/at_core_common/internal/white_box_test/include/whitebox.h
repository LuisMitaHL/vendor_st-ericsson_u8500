/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef WB_H
#define WB_H 1

#include <stdbool.h>
#include "atc_parser.h"
#include "atc_context.h"
#include "atc_selector.h"
#include "exe.h"
#ifdef EXE_USE_CN_SERVICE
#include "cn_client.h"
#endif
#include "mpl_list.h"
#include "mpl_msg.h"
#include "wb_test.h"

#ifdef EXE_USE_SIMPB_SERVICE
#include "simpb.h"
#endif


#define FAIL() do {PASS = false; printf("FAIL  case: %d \t file: %s \t line: %d\n", CURRENT_TEST_CASE, __FILE__, __LINE__); } while(0)
#define WB_CHECK(a) wb_test_check(a, __FILE__, __LINE__)
#define SIZEOF_WB_TEST_USSD_STRING 400
#define WB_LOG_E(...) do {(void)printf(__VA_ARGS__); (void)printf("\n"); } while (0)
#define WB_LOG_D(...) do { (void)printf(__VA_ARGS__); (void)printf("\n"); } while (0)
#define WB_ENABLE_DEBUGGING(...) do { WB_LOG_D(__VA_ARGS__); } while(0)
#define WB_PRINT_BUFFER()    do{ printf("buffer: %s\n", RESPONSE_BUFFER); } while(0)

bool wb_test_check(bool a, char *file, int line);
void clean_buffer(char *buffer_p);

typedef enum {
    SIM_WAITING_FOR_GET_STATE,
    SIM_WAITING_FOR_VERIFY_OK,
    SIM_WAITING_FOR_VERIFY_FAIL
} wb_sim_waiting_state;

typedef struct {
    exe_t *exe_p;

    /* SIM */
    void *simclient_p;
    callback_t sim_selector_callback_request;
    callback_t sim_selector_callback_event;
    wb_sim_waiting_state sim_state;

#ifdef EXE_USE_SIMPB_SERVICE
    void *simpbclient_p;
    ste_simpb_cb_t simpb_client_callback;
    callback_t simpb_selector_callback;
#endif

    /* SMS */
    void *smsclient_p;
    callback_t sms_selector_callback_request;
    callback_t sms_selector_callback_event;

#ifdef EXE_USE_CN_SERVICE
    callback_t cn_selector_callback_request;
    callback_t cn_selector_callback_event;
#endif

    /* PSCC */
    void *psccclient_p;
    bool sterc;
    mpl_list_t *pscc_param_list_p;
    mpl_msg_type_t pscc_msg_type;
    int pscc_event_id;
    int pscc_request_id;
    mpl_msg_type_t sterc_msg_type;
    mpl_list_t *sterc_param_list_p;
    int sterc_event_id;
    int sterc_request_id;
    callback_t pscc_selector_callback_request;
    callback_t pscc_selector_callback_event;
    callback_t sterc_selector_callback_request;
    callback_t sterc_selector_callback_event;

    /* request/test case specific data */
    void *client_tag_p;

    /* used for controlling the result in the pscc response */
    int pscc_result;

    /* pointer to specific test scenario data */
    void *wb_testdata_p;
    int  wb_testdata_size;
    char wb_test_ussd_str[SIZEOF_WB_TEST_USSD_STRING];

} wb_executor_t;

typedef void (*wb_testfcn_t)(char *at_string);

typedef struct {
    wb_testcase_t testcase;
    char *test_string;
    wb_testfcn_t function;
    char *description;
} wb_testcase_table_s;

/* Global variables */
extern const wb_testcase_table_s wb_testcase_table[];
extern atc_context_t *CONTEXT_p;
extern AT_ParserState_s *PARSER_p;
extern bool PASS;
extern wb_testcase_t CURRENT_TEST_CASE;
extern wb_executor_t EXECUTOR;
extern unsigned int RESPONSE_BUFFER_LENGTH;
extern char RESPONSE_BUFFER[2100];


/*
typedef enum {
    wb_testcase_setup,
    wb_testcase_mal_client_tag,
    wb_testcase_cfun_1,
    wb_testcase_cfun_1_failure,
    wb_testcase_clcc_1,
    wb_testcase_clcc_2,
    wb_testcase_cpin_test,
    wb_testcase_cleanup,
} wb_testcase_t;
*/
/* Test functions for MAL */
/* void wb_testfcn_exe_clcc_0(void); */
/* void wb_testfcn_exe_clcc_1(void); */
/* void wb_testfcn_exe_clcc_2(void); */

/* Test functions for SMS */
/* void exe_testfcn_cnmi(void); */

/* Test functions for SIM */
/* void wb_testfcn_cpin_test(char* at_string); */
/* void exe_testfcn_cpin_read(void); */

/* Test functions for SMS */
/* void exe_testfcn_cnmi(void); */

bool check_for_string(char *buffer_p, const char *string_p);
bool check_for_string_no_print(char *buffer_p, const char *string_p);
bool leftovers(char *buffer_p);
#endif
