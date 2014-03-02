/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include "cops.h"
#include "atc_log.h"
#include "whitebox.h"
#include "cspsa.h"

CSPSA_Result_t CSPSA_Open(const char* const ParameterAreaName_p, CSPSA_Handle_t* const Handle_p)
{
    char *storage_name_p = "CSPSA1";

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecspsar_storage:
    case wb_testcase_ecspsaw_storage:
        if(strcmp(ParameterAreaName_p, storage_name_p) != 0){
            return T_CSPSA_RESULT_E_INTERNAL_ERROR;
        }
        break;
    }
    return T_CSPSA_RESULT_OK;
}

CSPSA_Result_t CSPSA_Close(CSPSA_Handle_t* const Handle_p)
{
    return T_CSPSA_RESULT_OK;
}

CSPSA_Result_t CSPSA_ReadValue(const CSPSA_Handle_t Handle,
                               const CSPSA_Key_t Key,
                               const CSPSA_Size_t Size,
                               CSPSA_Data_t* const Data_p)
{
    CSPSA_Data_t read_data[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 };

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecspsar_fail:
        return T_CSPSA_RESULT_E_INTERNAL_ERROR;
        break;
    case wb_testcase_ecspsar:
    default:
        memcpy(Data_p, read_data, 10);
        break;
    }
    return T_CSPSA_RESULT_OK;
}

CSPSA_Result_t CSPSA_WriteValue(const CSPSA_Handle_t Handle,
                                const CSPSA_Key_t Key,
                                const CSPSA_Size_t Size,
                                const CSPSA_Data_t* const Data_p)
{
    CSPSA_Data_t cmp_data[] = { 0xBE, 0xEF };
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecspsaw:
        if (memcmp(Data_p, cmp_data, 2) != 0) {
            return T_CSPSA_RESULT_E_INTERNAL_ERROR;
        }
        break;
    case wb_testcase_ecspsaw_fail2:
        return T_CSPSA_RESULT_E_INTERNAL_ERROR;
        break;
    }
    return T_CSPSA_RESULT_OK;
}

CSPSA_Result_t CSPSA_GetSizeOfValue(const CSPSA_Handle_t Handle,
                                    const CSPSA_Key_t Key,
                                    CSPSA_Size_t* const Size_p)
{
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_ecspsar:
    default:
        *Size_p = 10;
        break;
    }
    return T_CSPSA_RESULT_OK;
}
