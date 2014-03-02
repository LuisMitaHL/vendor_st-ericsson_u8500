/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#include <stdio.h>

#include <bass_app.h>
#include <bass_app_test.h>
#include <debug.h>
#include <tee_client_api.h>

/**
 * Simply retrieve the anti-roll back table info.
 */
uint32_t bass_app_test_get_arb_table_info(void)
{
    bass_return_code result = BASS_RC_ERROR_UNKNOWN;
    struct bass_app_arb_table_info table_info;

    char toc_id1[100];
    char toc_id2[100];
    uint8_t table1[256];
    uint8_t table2[256];

    /* Sizes not set to 0, this to check the lengths are updated */
    table_info.toc_id1_size = 111;
    table_info.toc_id2_size = 222;
    table_info.table1_size = 333;
    table_info.table2_size = 444;

    dprintf(INFO, "Calling get_arb_table_info to get the sizes\n");
    result = bass_get_arb_table_info(true, &table_info);

    if (BASS_RC_SUCCESS != result) {
        dprintf(ERROR, "Failed to get ARB table sizes! Error code = %d\n",
                result);
    } else {
        dprintf(INFO, "ARB table sizes successfully returned\n");
        dprintf(INFO, "toc_id1_size: %d, toc_id2_size: %d\n",
                table_info.toc_id1_size, table_info.toc_id2_size);
        dprintf(INFO, "table1_size: %d, table2_size: %d\n",
                table_info.table1_size, table_info.table2_size);
    }

    if ((BASS_RC_SUCCESS == result) &&
        (table_info.table1_size > 0 || table_info.table2_size > 0)) {
        dprintf(INFO, "Calling get_arb_table_info to get the ARB tables\n");
        table_info.toc_id1 = (char *)&toc_id1[0];
        table_info.toc_id2 = (char *)&toc_id2[0];
        table_info.table1 = (uint8_t *)&table1[0];
        table_info.table2 = (uint8_t *)&table2[1];

        result = bass_get_arb_table_info(false, &table_info);

        if (BASS_RC_SUCCESS != result) {
            dprintf(ERROR, "Failed to get ARB tables! Error code = %d\n",
                    result);
        } else {
            dprintf(INFO, "Successful call to bass_get_arb_table_info\n");
            dprintf(INFO, "toc_id1_size: %d, toc_id2_size: %d\n",
                    table_info.toc_id1_size, table_info.toc_id2_size);
            dprintf(INFO, "table1_size: %d, table2_size: %d\n",
                    table_info.table1_size, table_info.table2_size);

            dprintf(INFO, "toc_id1: %s, toc_id2: %s\n", table_info.toc_id1,
                    table_info.toc_id2);

            dump_buffer("table1", table_info.table1, table_info.table1_size);
            dump_buffer("table2", table_info.table2, table_info.table2_size);
        }
    } else {
        dprintf(INFO, "No arb data to be updated\n");
    }

    return 0;
}
