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
#include "message_handler.h"

tc_result_t message_strings()
{
    tc_result_t tc_result = TC_RESULT_OK;
    int i = 0;
    int j = 0;
    char* msg_p = NULL;

    /* check that the table lookup function works as expected */
    for(i=CN_REQUEST_READ_OPL_FROM_SIM; i<=CN_REQUEST_SET_MANDATED_NETWORK_TYPE_FROM_SIM; i++) {
        msg_p = lookup_message_table(i);
        TC_ASSERT(NULL != msg_p);
        CN_LOG_V("%s", message_string_table[j].msg_string_p);
        TC_ASSERT(0 == memcmp(msg_p, message_string_table[j].msg_string_p, strlen(msg_p)+1));

        j++;
    }

    /* check that an unknown request is handled correctly */
    msg_p = lookup_message_table(0x300);
    TC_ASSERT(NULL == msg_p);

exit:
    return tc_result;
}

tc_result_t request_table_print()
{
    tc_result_t tc_result = TC_RESULT_OK;

    /* This test case requires visual inspection. Besides for covering the print function it
     * is also used to give clear information about the request table status of the CNS server
     * at different stages of execution. Most notably, the state before shutdown.
     */
    print_request_table();

exit:
    return tc_result;

}
