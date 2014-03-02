/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <uuid.h>

bass_return_code bass_get_arb_table_info(
    bool get_sizes, struct bass_app_arb_table_info *table_info)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Operation operation;

    if (NULL == table_info) {
        dprintf(ERROR, "NULL pointer error\n");
        goto function_exit;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    if (get_sizes) {
        dprintf(INFO, "Get table info sizes\n");

        operation.memRefs[0].buffer = &table_info->toc_id1_size;
        operation.memRefs[0].size = sizeof(size_t);
        operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

        operation.memRefs[1].buffer = &table_info->toc_id2_size;
        operation.memRefs[1].size = sizeof(size_t);
        operation.memRefs[1].flags = TEEC_MEM_OUTPUT;

        operation.memRefs[2].buffer = &table_info->table1_size;
        operation.memRefs[2].size = sizeof(size_t);
        operation.memRefs[2].flags = TEEC_MEM_OUTPUT;

        operation.memRefs[3].buffer = &table_info->table2_size;
        operation.memRefs[3].size = sizeof(size_t);
        operation.memRefs[3].flags = TEEC_MEM_OUTPUT;

    } else {
        if (table_info->toc_id1_size > 0 && table_info->toc_id1 != NULL &&
            table_info->toc_id2_size > 0 && table_info->toc_id2 != NULL &&
            table_info->table1_size > 0  && table_info->table1  != NULL &&
            table_info->table2_size > 0  && table_info->table2  != NULL) {
            dprintf(INFO, "Get table info\n");

            operation.memRefs[0].buffer = table_info->toc_id1;
            operation.memRefs[0].size = table_info->toc_id1_size;
            operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

            operation.memRefs[1].buffer = table_info->toc_id2;
            operation.memRefs[1].size = table_info->toc_id2_size;
            operation.memRefs[1].flags = TEEC_MEM_OUTPUT;

            operation.memRefs[2].buffer = table_info->table1;
            operation.memRefs[2].size = table_info->table1_size;
            operation.memRefs[2].flags = TEEC_MEM_OUTPUT;

            operation.memRefs[3].buffer = table_info->table2;
            operation.memRefs[3].size = table_info->table2_size;
            operation.memRefs[3].flags = TEEC_MEM_OUTPUT;

        } else {
            dprintf(ERROR, "At least one parameter invalid\n");
            goto function_exit;
        }
    }
    operation.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED |
                        TEEC_MEMREF_2_USED | TEEC_MEMREF_3_USED;

    if (get_sizes) {
        ret_code = teec_invoke_secworld(
                        &operation, BASS_APP_GET_ARB_INFO_SIZES);
    } else {
        ret_code = teec_invoke_secworld(
                        &operation, BASS_APP_GET_ARB_INFO);
    }

function_exit:
    return ret_code;
}
