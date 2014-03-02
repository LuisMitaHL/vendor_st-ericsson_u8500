/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* system includes */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* atc includes */
#include "atc_command_table.h"
#include "atc_connection.h"
#include "atc_log.h"
#include "atc_context.h"
#include "atc_parser.h"

/* test includes */
#include "whitebox.h"
#include "wb_test.h"  /* Not needed... */

/* Global variables */
atc_context_t *CONTEXT_p = NULL;
AT_ParserState_s *PARSER_p = NULL;
wb_executor_t EXECUTOR;
wb_testcase_t CURRENT_TEST_CASE;
bool PASS;

char *conn_p = "/dev/ttyGS0";

extern char *atc_revision_identification_path_p;
extern char *atc_model_identification_path_p;
extern char *atc_manufacturer_identification_path_p;

bool wb_test_check(bool a, char *file, int line)
{
    PASS = PASS ? a : PASS; /* Do not set pass to true if it already is false. */

    if (!a) {
        printf("FAIL:  file: %s \t line: %d\n", file, line);
    }

    return PASS;
}

void clean_buffer(char *buffer_p)
{
    char *pos_p = buffer_p;

    while (*pos_p) {
        *pos_p = '_';
        pos_p++;
    }
}

bool leftovers(char *buffer_p)
{
    bool result = false;
    unsigned int pos;
    char *pos_p;

    for (pos = 0; pos < RESPONSE_BUFFER_LENGTH; pos++) {
        pos_p = buffer_p + pos;

        if (!result && ('_' == *pos_p || PARSER_p->S3 == *pos_p || PARSER_p->S4 == *pos_p)) {
            *pos_p = '_';
        } else {
            if (!result) {
                printf("%s: case=%d, expecting nothing, got=(", __FUNCTION__, CURRENT_TEST_CASE);
                result = true;
            }

            if (PARSER_p->S3 == *pos_p) {
                printf("<S3>");
            } else if (PARSER_p->S4 == *pos_p) {
                printf("<S4>");
            } else {
                printf("%c", *pos_p);
            }
        }

        pos_p++;
    }

    if (result) {
        printf(")\n");
    }

    return result;
}


bool check_for_string(char *buffer_p, const char *string_p)
{
    bool result = false;
    char *pos_p = NULL;
    char *response_p = buffer_p;
    unsigned int length = RESPONSE_BUFFER_LENGTH;
    unsigned int pos;

    /* Reverse any non-standard S3 or S4 output formatting */
    if (PARSER_p->S3 != '\r' || PARSER_p->S4 != '\n') {
        response_p = (char *)malloc(length);
        if (!response_p) {
            printf("%s: Unrecoverable error, malloc failed!\n", __FUNCTION__);
            exit(EXIT_FAILURE);
        }

        for (pos = 0; pos < length; pos++) {
            if (PARSER_p->S3 == *(buffer_p + pos)) {
                *(response_p + pos) = '\r';
            } else if (PARSER_p->S4 == *(buffer_p + pos)) {
                *(response_p + pos) = '\n';
            } else {
                *(response_p + pos) = *(buffer_p + pos);
            }
        }
    }

    pos_p = strstr(response_p, string_p);

    if (pos_p != NULL) {
        result = true;
        pos = pos_p - response_p;
        memset(buffer_p + pos, '_', strlen(string_p));
    } else {
        printf("%s: case=%d, expecting=(%s), got=(", __FUNCTION__, CURRENT_TEST_CASE, string_p);

        for (pos = 0; pos < length; pos++) {
            if (PARSER_p->S3 == *(buffer_p + pos)) {
                printf("<S3>");
            } else if (PARSER_p->S4 == *(buffer_p + pos)) {
                printf("<S4>");
            } else {
                printf("%c", *(buffer_p + pos));
            }
        }

        printf(")\n");
    }

    if (response_p != buffer_p) {
        free(response_p);
    }

    return result;
}

bool check_for_string_no_print(char *buffer_p, const char *string_p)
{
    bool result = false;
    char *pos_p = NULL;
    char *response_p = buffer_p;
    unsigned int length = RESPONSE_BUFFER_LENGTH;
    unsigned int pos;

    /* Reverse any non-standard S3 or S4 output formatting */
    if (PARSER_p->S3 != '\r' || PARSER_p->S4 != '\n') {
        response_p = (char *)malloc(length);
        if (!response_p) {
            printf("%s: Unrecoverable error, malloc failed!\n", __FUNCTION__);
            exit(EXIT_FAILURE);
        }

        for (pos = 0; pos < length; pos++) {
            if (PARSER_p->S3 == *(buffer_p + pos)) {
                *(response_p + pos) = '\r';
            } else if (PARSER_p->S4 == *(buffer_p + pos)) {
                *(response_p + pos) = '\n';
            } else {
                *(response_p + pos) = *(buffer_p + pos);
            }
        }
    }

    pos_p = strstr(response_p, string_p);

    if (pos_p != NULL) {
        result = true;
        pos = pos_p - response_p;
        memset(buffer_p + pos, '_', strlen(string_p));
    }

    if (response_p != buffer_p) {
        free(response_p);
    }

    return result;
}

void wb_testfcn_setup(char *at_string)
{
    /* Set up context */
    CONTEXT_p = atc_context_get_free();
    CONTEXT_p->at_connection_type = ATC_CONNECTION_TYPE_INTERNAL;
    CONTEXT_p->conn = &conn_p;

    /* Set up parser state */
    PARSER_p = ParserStateTable_GetParserState_ByEntry(0);
    ParserStateTable_ResetEntry(0);

    /* Connect context and parser state */
    atc_context_connect_to_parser(CONTEXT_p);
    Parser_SetConnectionType(PARSER_p, ATC_CONNECTION_TYPE_INTERNAL);
}

void wb_testfcn_cleanup(char *at_string)
{
    ParserStateTable_ResetEntry(0);
    atc_context_init(CONTEXT_p);
}

char *wb_add_top_to_path(char *initial_path_p)
{

    char *path_p = NULL;
    char *env_p = NULL;
    uint32_t top_path_length;
    uint32_t tot_path_length;

    if (NULL == initial_path_p) {
        ATC_LOG_E("wb_add_top_to_path(): initial_path_p is NULL");
        assert(0);
    }

    env_p = getenv("TOP");

    if (NULL == env_p) {
        ATC_LOG_E("wb_add_top_to_path(): TOP is not set");
        assert(0);
    }

    top_path_length = strlen(env_p);
    tot_path_length = top_path_length + strlen(initial_path_p);

    path_p = (char *) malloc(tot_path_length + 1);

    if (NULL == path_p) {
        ATC_LOG_E("wb_add_top_to_path(): could not allocate path_p");
        assert(0);
    }

    (void) memmove(path_p,
                   env_p,
                   top_path_length);
    (void) memmove(path_p + top_path_length,
                   initial_path_p,
                   tot_path_length - top_path_length);

    path_p[tot_path_length] = '\0';

    return path_p;
}


static void wb_test_init(void)
{
    /* Initialize parameters that are test case specific. */
    EXECUTOR.client_tag_p = NULL;
    EXECUTOR.pscc_result = 0; /* pscc_result_ok == 0 */
    EXECUTOR.pscc_request_id = -1; /* No request pending */
    EXECUTOR.pscc_event_id = -1; /* No request pending */
    EXECUTOR.sterc_request_id = -1; /* No request pending */
    EXECUTOR.sterc_event_id = -1; /* No request pending */
    EXECUTOR.sterc = 0;
    EXECUTOR.wb_testdata_p = NULL;
    EXECUTOR.wb_testdata_size = 0;
    memset(EXECUTOR.wb_test_ussd_str, 0, sizeof(EXECUTOR.wb_test_ussd_str));

    if (PARSER_p) {
        /* Reset some parser state properties */
        PARSER_p->S3 = 13;      /* According to 3GPP. 13 = CR */
        PARSER_p->S4 = 10;      /* According to 3GPP. 10 = LF */
        PARSER_p->S5 = 8;       /* According to 3GPP. 8 = BS */
    }

    if (CONTEXT_p) {
        /* Reset some context properties */
        CONTEXT_p->next_input_mode = ATC_INPUT_MODE_NEW_LINE;
        CONTEXT_p->input_mode = ATC_INPUT_MODE_NEW_LINE;
    }

    RESPONSE_BUFFER_LENGTH = 0;
}


static bool run_all(void)
{
    int passed_test_cases = 0;
    int failed_test_cases = 0;
    char *test_case_string;
    int i = 0;
    printf("\n");

    atc_revision_identification_path_p = wb_add_top_to_path("/vendor/st-ericsson/access_services/at/at_core_common/internal/white_box_test/system_id.cfg");
    atc_model_identification_path_p = wb_add_top_to_path("/vendor/st-ericsson/access_services/at/at_core_common/internal/white_box_test/model_id.cfg");
    atc_manufacturer_identification_path_p = wb_add_top_to_path("/vendor/st-ericsson/access_services/at/at_core_common/internal/white_box_test/manuf_id.cfg");

    do {
        PASS = true;
        wb_test_init();
        CURRENT_TEST_CASE = wb_testcase_table[i].testcase;
        test_case_string = wb_testcase_table[i].test_string;
        wb_testcase_table[i].function(test_case_string);

        if (PASS) {
            printf("PASS: %s\n", wb_testcase_table[i].description);
            passed_test_cases++;
        } else {
            printf("\n   --> FAIL: %s\n\n", wb_testcase_table[i].description);
            failed_test_cases++;
        }

        i++;
    } while (CURRENT_TEST_CASE < wb_testcase_cleanup);

    free(atc_revision_identification_path_p);
    free(atc_model_identification_path_p);
    free(atc_manufacturer_identification_path_p);

    printf("\nSum -- Passed = %d \t  Failed = %d \n\n", passed_test_cases, failed_test_cases);
}

int main(int argc, const char *argv[])
{
    exe_t *exe_p;
    setbuf(stdout, 0);
    exe_p = exe_create();
    atc_context_init_all_contexts();
    init_parser_states();
    atc_setup_exe_glue(exe_p);
    exe_start_open_sessions(exe_p);

    run_all();

    exe_destroy(exe_p);
    return EXIT_SUCCESS;
}
