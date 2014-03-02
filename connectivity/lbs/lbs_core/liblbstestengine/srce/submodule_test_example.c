/*
 * Positioning Manager
 *
 * submodule_test_example.c.bak
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <agpsosa.h>

#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

#include <test_engine_int.h>

/*
 * Test cases
 */

static int SUBMOD_testcase1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);
static int SUBMOD_testcase2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found);

TSTENG_TEST_TABLE_BEGIN(SUBMOD)
    TSTENG_TEST_TABLE_CMD("tc1", SUBMOD_testcase1)
    TSTENG_TEST_TABLE_CMD("tc2", SUBMOD_testcase2)
    TSTENG_TEST_TABLE_END
/*
 * SUBMOD_init
 *
 * Init submodule tests
 */
void SUBMOD_init(void)
{
    INF("registering SUBMOD testcases with TSTENG...");

    TSTENG_REGISTER_TABLE(SUBMOD);
}


/*************************************************************
* SUBMOD_testcase1
**************************************************************/
static int SUBMOD_testcase1(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_result_t result = TSTENG_RESULT_OK;

    if (args_found > 0) {
    if (args_found == 1) {
        if (TSTENG_CaseInsensitiveStrCompare(cmd_buf + arg_index[0], "?")) {
        TSTENG_info_result_cb(handle, TSTENG_RESULT_OK, "SUBMOD testcase1 <count> - run tc1 <count> times\n");
        } else {
        int count = TSTENG_StrToSint32(cmd_buf + arg_index[0]);
        if (count > 0) {
            // XXX call test count times
            for (; count > 0; --count) {
            TSTENG_info_result_cb(handle, result, "info");
            }
        }
        }
    }
    } else {
    result = TSTENG_RESULT_FAIL;
    }

    return result;
}

/*************************************************************
* SUBMOD_testcase2
**************************************************************/
static int SUBMOD_testcase2(TSTENG_handle_t handle, char *cmd_buf, int arg_index[], int args_found)
{
    TSTENG_result_t result = TSTENG_RESULT_FAIL;

    result = TSTENG_execute_command(handle, "SUBMOD tc1 300");

    return result;
}
