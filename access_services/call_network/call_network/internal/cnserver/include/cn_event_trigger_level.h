/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _CN_EVENT_TRIGGER_LEVEL_H_
#define _CN_EVENT_TRIGGER_LEVEL_H_

typedef enum {
    ETL_TABLE_UNKNOWN,
    ETL_TABLE_REG_STATUS
} etl_table_t;

typedef struct {
    int client_id;
    int trigger_level;
} etl_table_entry_t;

#define ETL_NO_CLIENT_ID -1
#define ETL_MAX_NR_OF_TABLE_ENTRIES 16
#define ETL_MAX_TRIGGER_LEVEL 255

void etl_init();
cn_bool_t etl_update_trigger_level_table(etl_table_t table, int client_id, int trigger_level);
cn_reg_status_trigger_level_t etl_determine_reg_status_trigger_level();
void etl_client_disconnected_cb(int client_id);

/* functions exposed below for unit testing purposes */
void etl_print_table(etl_table_t table);
void etl_write_entry_to_table(etl_table_t table, int index, int client_id, int trigger_level);
etl_table_entry_t etl_read_entry_from_table(etl_table_t table, int index);
int  etl_find_entry_in_table(etl_table_t table, int client_id);
int  etl_get_max_trigger_level_in_table(etl_table_t table);
int  etl_find_first_empty_row(etl_table_t table);
void etl_clear_table_entry(etl_table_t table, int index);
void etl_clear_table(etl_table_t table);

#endif /* _CN_EVENT_TRIGGER_LEVEL_H_ */
