/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* This file is intended for trigger level handling for various events. It
 * has been made generic to make it easy to extend.
 */

#include <stdio.h>

#include "cn_data_types.h"
#include "cn_log.h"
#include "cn_event_trigger_level.h"
#include "message_handler.h"
#include "request_handling.h"

/* table definition */
etl_table_entry_t etl_reg_status_table[ETL_MAX_NR_OF_TABLE_ENTRIES];

/****************************** TABLE HANDLING ************************************************/

void etl_clear_table(etl_table_t table)
{
    int i = 0;

    for (i = 0; i < ETL_MAX_NR_OF_TABLE_ENTRIES; i++) {
        etl_clear_table_entry(table, i);
    }
}

void etl_print_table(etl_table_t table)
{
    int i = 0;

    for (i = 0; i < ETL_MAX_NR_OF_TABLE_ENTRIES; i++) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            CN_LOG_D("reg_status_trigger_level_table[%d]: %d %d", i,
                     etl_reg_status_table[i].client_id,
                     etl_reg_status_table[i].trigger_level);
            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            break;
        } /* end of switch-statement */
    } /* end of if-statement */
}

void etl_write_entry_to_table(etl_table_t table, int index, int client_id, int trigger_level)
{
    if (ETL_MAX_NR_OF_TABLE_ENTRIES > index && 0 <= index) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            etl_reg_status_table[index].client_id = client_id;
            etl_reg_status_table[index].trigger_level = trigger_level;
            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            break;
        } /* end of switch-statement */
    } /* end of if-statement */
}

etl_table_entry_t etl_read_entry_from_table(etl_table_t table, int index)
{
    etl_table_entry_t entry = { -1, -1};

    if (ETL_MAX_NR_OF_TABLE_ENTRIES > index && 0 <= index) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            entry = etl_reg_status_table[index];
            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            break;
        } /* end of switch-statement */
    } /* end of if-statement */

    return entry;
}

int etl_find_entry_in_table(etl_table_t table, int client_id)
{
    int i = 0;

    /* remove client entry in table if it exists */
    for (i = 0; i < ETL_MAX_NR_OF_TABLE_ENTRIES; i++) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            if (client_id == etl_reg_status_table[i].client_id) {
                return i;
            }

            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            goto exit_loop;
        } /* end of switch-statement */
    } /* end of loop */

exit_loop:

    return -1;
}

int etl_get_max_trigger_level_in_table(etl_table_t table)
{
    int i = 0;
    int max = -1;

    for (i = 0; i < ETL_MAX_NR_OF_TABLE_ENTRIES; i++) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            if (max < etl_reg_status_table[i].trigger_level) {
                max = etl_reg_status_table[i].trigger_level;
            }

            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            goto error;
        } /* end of switch-statement */
    } /* end of loop */

    return max;

error:
    return -1;
}

int etl_find_first_empty_row(etl_table_t table)
{
    int i = 0;

    for (i = 0; i < ETL_MAX_NR_OF_TABLE_ENTRIES; i++) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            if (ETL_NO_CLIENT_ID == etl_reg_status_table[i].client_id) {
                return i;
            }

            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            goto exit_loop;
        } /* end of switch-statement */
    } /* end of loop */

exit_loop:

    return -1;
}

void etl_clear_table_entry(etl_table_t table, int index)
{
    if (ETL_MAX_NR_OF_TABLE_ENTRIES > index && 0 <= index) {
        switch (table) {
        case ETL_TABLE_REG_STATUS:
            etl_reg_status_table[index].client_id = ETL_NO_CLIENT_ID;
            etl_reg_status_table[index].trigger_level = CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM; /* default level */
            break;

        default:
            CN_LOG_W("unknown table! (%d)", table);
            break;
        } /* end of switch-statement */
    } /* end of if-statement */
}


/****************************** TRIGGER LEVEL HANDLING ************************************************/
cn_bool_t etl_update_trigger_level_table(etl_table_t table, int client_id, int trigger_level)
{
    int index = 0;

    /* find existing entry */
    index = etl_find_entry_in_table(table, client_id);

    if (-1 != index) { /* entry found */
        CN_LOG_D("existing entry found on index=%d", index);
        goto write_entry;
    }

    /* new client */
    index = etl_find_first_empty_row(table);

    if (-1 == index) {
        CN_LOG_E("trigger level table exhausted!");
        etl_print_table(table);
        goto error;
    }

    CN_LOG_D("new entry will be written on index=%d", index);

write_entry:
    CN_LOG_D("writing entry %d %d on index %d", client_id, trigger_level, index);
    etl_write_entry_to_table(table, index, client_id, trigger_level);

    return TRUE;

error:
    return FALSE;
}


cn_reg_status_trigger_level_t etl_determine_reg_status_trigger_level()
{
    /* determine the current level (client disconnects may have occured) */
    return etl_get_max_trigger_level_in_table(ETL_TABLE_REG_STATUS);
}

static void etl_update_modem_setting()
{
    request_record_t *record_p = NULL;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_request_reg_status_event_config_t reg_status_event_config;

    CN_LOG_D("enter");

    /* The table has been altered. Update configuration with the help of the
     * normal request handler. This approach has been chosen to have common code
     * between the handler for external clients and this event handler.
     */
    record_p = request_record_create(CN_REQUEST_REG_STATUS_EVENT_CONFIG, 0, -1); /* no client id */

    if (!record_p) {
        CN_LOG_E("ERROR, not possible to create request record!");
        goto exit;
    }

    record_p->request_handler_p = handle_request_reg_status_event_config;

    /* use CN_REG_STATUS_TRIGGER_LEVEL_UNDEFINED to specify that the trigger table should not be updated. */
    reg_status_event_config.trigger_level = CN_REG_STATUS_TRIGGER_LEVEL_UNDEFINED;

    status = handle_request_reg_status_event_config(&reg_status_event_config, record_p);

    if (REQUEST_STATUS_PENDING != status) {
        request_record_free(record_p);
    }

exit:
    return;
}

void etl_init()
{
    CN_LOG_D("enter");

    etl_clear_table(ETL_TABLE_REG_STATUS);

    etl_update_modem_setting(); /* with a cleared table the trigger level will be CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM */
}

void etl_client_disconnected_cb(int client_id) /* this callback is invoked by the cnserver */
{
    int index = 0;

    /* clear possible entry in available tables (remove client entry) */
    index = etl_find_entry_in_table(ETL_TABLE_REG_STATUS, client_id);

    if (-1 != index) {
        CN_LOG_D("removing client_id %d from ETL_TABLE_REG_STATUS due to client disconnection", client_id);
        etl_clear_table_entry(ETL_TABLE_REG_STATUS, index);
    } else {
        CN_LOG_D("client_id %d has not been registered in ETL_TABLE_REG_STATUS, skipping update.", client_id);
        goto exit;
    }

    etl_update_modem_setting();

exit:
    return;
}

