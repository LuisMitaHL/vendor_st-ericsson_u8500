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
#include "cn_event_trigger_level.h"

tc_result_t etl_reg_status_table_handling()
{
    tc_result_t tc_result = TC_RESULT_OK;
    etl_table_entry_t etl_entry;
    int index = 0;
    int value = 0;

    etl_clear_table(ETL_TABLE_REG_STATUS);

    /* test write and read function */
    CN_LOG_D("");
    etl_write_entry_to_table(ETL_TABLE_REG_STATUS, 0, 12, 0);
    etl_write_entry_to_table(ETL_TABLE_REG_STATUS, 9, 67, 36);
    etl_write_entry_to_table(ETL_TABLE_REG_STATUS, 1, 14, 1);
    etl_write_entry_to_table(ETL_TABLE_REG_STATUS, ETL_MAX_NR_OF_TABLE_ENTRIES-1, 16, 2);

    etl_entry = etl_read_entry_from_table(ETL_TABLE_REG_STATUS, 0);
    TC_ASSERT(12 == etl_entry.client_id);
    TC_ASSERT(0 == etl_entry.trigger_level);

    etl_entry = etl_read_entry_from_table(ETL_TABLE_REG_STATUS, 1);
    TC_ASSERT(14 == etl_entry.client_id);
    TC_ASSERT(1 == etl_entry.trigger_level);

    etl_entry = etl_read_entry_from_table(ETL_TABLE_REG_STATUS, 9);
    TC_ASSERT(67 == etl_entry.client_id);
    TC_ASSERT(36 == etl_entry.trigger_level);

    etl_entry = etl_read_entry_from_table(ETL_TABLE_REG_STATUS, ETL_MAX_NR_OF_TABLE_ENTRIES-1);
    TC_ASSERT(16 == etl_entry.client_id);
    TC_ASSERT(2 == etl_entry.trigger_level);

    CN_LOG_D("");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    /* test find function */
    index = etl_find_entry_in_table(ETL_TABLE_REG_STATUS, 67);
    TC_ASSERT(9 == index);

    /* test max function */
    value = etl_get_max_trigger_level_in_table(ETL_TABLE_REG_STATUS);
    TC_ASSERT(36 == value);

    /* test clear function */
    etl_clear_table_entry(ETL_TABLE_REG_STATUS,9);
    etl_entry = etl_read_entry_from_table(ETL_TABLE_REG_STATUS, 9);
    TC_ASSERT(-1 == etl_entry.client_id);
    TC_ASSERT(0 == etl_entry.trigger_level);

    /* test first empty row function */
    index = etl_find_first_empty_row(ETL_TABLE_REG_STATUS);
    TC_ASSERT(2 == index);

    /* cleanup */
    etl_clear_table(ETL_TABLE_REG_STATUS);

exit:
    return tc_result;
}


tc_result_t etl_trigger_level_handling()
{
    tc_result_t tc_result = TC_RESULT_OK;
    cn_bool_t result = FALSE;
    cn_reg_status_trigger_level_t trigger_level = 0;

    CN_LOG_D("");
    CN_LOG_D("define table data with the update table function - CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM (0):");
    etl_clear_table(ETL_TABLE_REG_STATUS);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 13, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 14, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 15, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 16, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM == trigger_level);

    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    CN_LOG_D("");
    CN_LOG_D("test update functionality. use a single CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE (1):");
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 13, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 14, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 15, CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 16, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE == trigger_level);

    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    CN_LOG_D("");
    CN_LOG_D("add a new entry with CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC (2):");
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 17, CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC);
    TC_ASSERT(result);

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC == trigger_level);

    CN_LOG_D("");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    CN_LOG_D("");
    CN_LOG_D("update table with two CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS (3):");
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 15, CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 16, CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS);

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS == trigger_level);

    CN_LOG_D("");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    CN_LOG_D("");
    CN_LOG_D("\"remove\" the two entries with CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS (3):");
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 15, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 16, CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM);
    TC_ASSERT(result);

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC == trigger_level);

    CN_LOG_D("");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    CN_LOG_D("");
    CN_LOG_D("update entry with CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE (1) -> should not change level:");
    result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, 13, CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE);
    TC_ASSERT(result);

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC == trigger_level);

    CN_LOG_D("");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    CN_LOG_D("");
    CN_LOG_D("fake table entry clearing that comes when a client disconnects - remove entry with client id 17:");
    etl_clear_table_entry(ETL_TABLE_REG_STATUS, 4); /* client id 17 - removing CN_REG_STATUS_TRIGGER_LEVEL_CID_OR_LAC from the table */

    /* check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_REGISTRATION_STATE == trigger_level);

    CN_LOG_D("");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */

    /* cleanup */
    etl_clear_table(ETL_TABLE_REG_STATUS);
    CN_LOG_D("");
    CN_LOG_D("check that the trigger level is CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM (0) for an empty list:");
    etl_print_table(ETL_TABLE_REG_STATUS); /* visual inspection */
    trigger_level =  etl_determine_reg_status_trigger_level();
    TC_ASSERT(CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM == trigger_level);

exit:
    return tc_result;
}


