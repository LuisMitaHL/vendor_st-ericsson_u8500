/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "testcases.h"
#include "module_test_main.h"
#include "cn_client.h"
#include "cn_log.h"

/**
 * 'Normal' test case. Add two differnt numbers and a third that is already present, read them back and assert that the correct numbers (2) have been returned. Then clear the list and assert that no numbers can be retrieved.
 */
tc_result_t cn_request_get_emergency_number_list_positive_1()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Create the number struct to set */
    char *number1 = "123456";
    char *number2 = "789";

    char *mcc1 = "240";
    char mcc_range = '2';

    cn_emergency_number_config_t number;
    memset(&number, 0x00, sizeof(cn_emergency_number_config_t));
    number.service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void)strncpy(number.emergency_number, (const char *)number1, sizeof(number.emergency_number) - 1);
    (void)strncpy(number.mcc, (const char *)mcc1, sizeof(number.mcc) - 1);
    number.mcc_range[0] = mcc_range;

    /* Add first number*/
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_ADD, &number, (cn_client_tag_t) 2055);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the add-operation was successful: */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("1 cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2055 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Make and set number 2 */
    (void)strncpy(number.emergency_number, (const char *)number2, sizeof(number.emergency_number) - 1);
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_ADD, &number, (cn_client_tag_t) 2056);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the second add-operation was successful: */

    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2056 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Add a third number, identical to the first one. Call should execute correctly, but not increase the number of numbers on the file . */
    (void)strncpy(number.emergency_number, (const char *)number1, sizeof(number.emergency_number) - 1);
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_ADD, &number, (cn_client_tag_t) 2058);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the second add-operation was successful: */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2058 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Read back the list and verify that the numbers are the same:*/
    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("b cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_emergency_number_list_t *cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;

    TC_ASSERT(2 == cn_emergency_number_list_p->num_of_emergency_numbers);
    TC_ASSERT(0 == strcmp(cn_emergency_number_list_p->emergency_number[0].emergency_number, "123456"));
    TC_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED == cn_emergency_number_list_p->emergency_number[0].service_type)
    TC_ASSERT(0 == strcmp(cn_emergency_number_list_p->emergency_number[1].emergency_number, "789"));
    TC_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED == cn_emergency_number_list_p->emergency_number[1].service_type)
    free(msg_p);
    msg_p = NULL;

    /* Clear the list and verify that there are no entries */
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST, NULL, (cn_client_tag_t) 2059);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_modify_emergency_number_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* wait for request to finish (synchronous operation internally) */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2059 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Wait for stub to be invoked */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;
    TC_ASSERT(0 == cn_emergency_number_list_p->num_of_emergency_numbers);
    free(msg_p);
    msg_p = NULL;

exit:
    free(msg_p);

    return tc_result;
}

/**
 * Case with no defined emergency numbers. Assert that no numbers are returned.
 * */
tc_result_t cn_request_get_emergency_number_list_positive_2()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;
    cn_emergency_number_list_t *cn_emergency_number_list_p = NULL;

    /* Send CN_REQUEST_GET_EMERGENCY_NUM_LIST */
    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Receive CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;

    TC_ASSERT(0 == cn_emergency_number_list_p->num_of_emergency_numbers);

exit:
    free(msg_p);
    return tc_result;
}

/**
 * 'Normal' test case. Add two differnt numbers and a third that is already present, read them back and assert that the correct numbers (2) have been returned. Then clear the list and assert that no numbers can be retrieved.
 */
tc_result_t cn_request_get_emergency_number_list_positive_3()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Create the number struct to set */
    char *number1 = "123456";
    char *number2 = "789";

    char *mcc1 = "240";
    char mcc_range = '2';

    cn_emergency_number_config_t number;
    memset(&number, 0x00, sizeof(cn_emergency_number_config_t));
    number.service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void)strncpy(number.emergency_number, (const char *)number1, sizeof(number.emergency_number) - 1);
    (void)strncpy(number.mcc, (const char *)mcc1, sizeof(number.mcc) - 1);
    number.mcc_range[0] = mcc_range;

    /* Add first number*/
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_REMOVE, &number, (cn_client_tag_t) 2055);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the add-operation was successful: */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("1 cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2055 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Make and set number 2 */
    (void)strncpy(number.emergency_number, (const char *)number2, sizeof(number.emergency_number) - 1);
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_REMOVE, &number, (cn_client_tag_t) 2056);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the second add-operation was successful: */

    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2056 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Add a third number, identical to the first one. Call should execute correctly, but not increase the number of numbers on the file . */
    (void)strncpy(number.emergency_number, (const char *)number1, sizeof(number.emergency_number) - 1);
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_REMOVE, &number, (cn_client_tag_t) 2058);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the second add-operation was successful: */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2058 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Read back the list and verify that the numbers are the same:*/
    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("b cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_emergency_number_list_t *cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;

    TC_ASSERT(2 == cn_emergency_number_list_p->num_of_emergency_numbers);
    TC_ASSERT(0 == strcmp(cn_emergency_number_list_p->emergency_number[0].emergency_number, "123456"));
    TC_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED == cn_emergency_number_list_p->emergency_number[0].service_type)
    TC_ASSERT(0 == strcmp(cn_emergency_number_list_p->emergency_number[1].emergency_number, "789"));
    TC_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED == cn_emergency_number_list_p->emergency_number[1].service_type)
    free(msg_p);
    msg_p = NULL;

    /* Clear the list and verify that there are no entries */
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST, NULL, (cn_client_tag_t) 2059);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_modify_emergency_number_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* wait for request to finish (synchronous operation internally) */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2059 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Wait for stub to be invoked */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;
    TC_ASSERT(0 == cn_emergency_number_list_p->num_of_emergency_numbers);
    free(msg_p);
    msg_p = NULL;

exit:
    free(msg_p);

    return tc_result;
}

/**
 * 'Normal' test case. Add two differnt numbers and a third that is already present, read them back and assert that the correct numbers (2) have been returned. Then clear the list and assert that no numbers can be retrieved.
 */
tc_result_t cn_request_get_emergency_number_list_positive_4()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_error_code_t result = 0;
    cn_uint32_t size = 0;
    cn_message_t *msg_p = NULL;

    /* Create the number struct to set */
    char *number1 = "123456";
    char *number2 = "789";

    char *mcc1 = "240";
    char mcc_range = '2';

    cn_emergency_number_config_t number;
    memset(&number, 0x00, sizeof(cn_emergency_number_config_t));
    number.service_type = CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED;
    (void)strncpy(number.emergency_number, (const char *)number1, sizeof(number.emergency_number) - 1);
    (void)strncpy(number.mcc, (const char *)mcc1, sizeof(number.mcc) - 1);
    number.mcc_range[0] = mcc_range;

    /* Add first number*/
    result = cn_request_modify_emergency_number_list(g_context_p, 5, &number, (cn_client_tag_t) 2055);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the add-operation was successful: */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("1 cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2055 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Make and set number 2 */
    (void)strncpy(number.emergency_number, (const char *)number2, sizeof(number.emergency_number) - 1);
    result = cn_request_modify_emergency_number_list(g_context_p, 5, &number, (cn_client_tag_t) 2056);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the second add-operation was successful: */

    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2056 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Add a third number, identical to the first one. Call should execute correctly, but not increase the number of numbers on the file . */
    (void)strncpy(number.emergency_number, (const char *)number1, sizeof(number.emergency_number) - 1);
    result = cn_request_modify_emergency_number_list(g_context_p, 5, &number, (cn_client_tag_t) 2058);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_add_emergency_number failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    /* Check that the second add-operation was successful: */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2058 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    /* Read back the list and verify that the numbers are the same:*/
    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    WAIT_FOR_SELECT_TO_FINISH();

    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("b cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);

    cn_emergency_number_list_t *cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;

    TC_ASSERT(2 == cn_emergency_number_list_p->num_of_emergency_numbers);
    TC_ASSERT(0 == strcmp(cn_emergency_number_list_p->emergency_number[0].emergency_number, "123456"));
    TC_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED == cn_emergency_number_list_p->emergency_number[0].service_type)
    TC_ASSERT(0 == strcmp(cn_emergency_number_list_p->emergency_number[1].emergency_number, "789"));
    TC_ASSERT(CN_EMERGENCY_CALL_SERVICE_UNSPECIFIED == cn_emergency_number_list_p->emergency_number[1].service_type)
    free(msg_p);
    msg_p = NULL;

    /* Clear the list and verify that there are no entries */
    result = cn_request_modify_emergency_number_list(g_context_p, CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST, NULL, (cn_client_tag_t) 2059);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_modify_emergency_number_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* wait for request to finish (synchronous operation internally) */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2059 == msg_p->client_tag);
    free(msg_p);
    msg_p = NULL;

    result = cn_request_get_emergency_number_list(g_context_p, (cn_client_tag_t) 2057);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_request_get_emerg_num_list failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    /* Wait for stub to be invoked */
    WAIT_FOR_MESSAGE(g_request_fd);
    result = cn_message_receive(g_request_fd, &size, &msg_p);

    if (CN_SUCCESS != result) {
        CN_LOG_E("cn_message_receive failed!");
        SET_TC_RESULT_AND_GOTO_EXIT(TC_RESULT_FAILED);
    }

    TC_ASSERT(CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST == msg_p->type);
    TC_ASSERT(CN_SUCCESS == msg_p->error_code);
    TC_ASSERT(2057 == msg_p->client_tag);
    TC_ASSERT(NULL != msg_p->payload);
    cn_emergency_number_list_p = (cn_emergency_number_list_t *)msg_p->payload;
    TC_ASSERT(0 == cn_emergency_number_list_p->num_of_emergency_numbers);
    free(msg_p);
    msg_p = NULL;

exit:
    free(msg_p);

    return tc_result;
}

