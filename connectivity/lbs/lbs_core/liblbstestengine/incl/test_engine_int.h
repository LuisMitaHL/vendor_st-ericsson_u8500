/*
 * Positioning Manager
 *
 * test_engine_int.h
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef __TEST_ENGINE_INT_H__
#define __TEST_ENGINE_INT_H__

#define TSTENG_PORT                     5679
#define TSTENG_MAX_CLIENT               20

#define TSTENG_MAX_PREFIX_SIZE          20    // Max length of a module prefix (including zero termination)

#define TSTENG_MAXLEN_ENTRYNAME         200
#define TSTENG_MAX_ARGS                 40
#define TSTENG_MAXLEN_CMDBUF            4096

#define TSTENG_TBL_TYPE_END             0
#define TSTENG_TBL_TYPE_CMD             ('C')

#define TSTENG_MAX_CMD_STR_SIZE         40

#define TSTENG_SPACE_CHAR               0x20
#define TSTENG_BACKSLASH_CHAR           0x5c

#define TSTENG_TEST_TABLE_BEGIN(_x)      TSTENG_command_item_t TSTENG_##_x##_table[] = {
#define TSTENG_TEST_TABLE_CMD(Str,Func,Desc) {Str, (TSTENG_function_p)Func, Desc, TSTENG_TBL_TYPE_CMD},
#define TSTENG_TEST_TABLE_END           {NULL, NULL, NULL, TSTENG_TBL_TYPE_END}};

#define TSTENG_REGISTER_TABLE(_x) {TSTENG_register_table(#_x, TSTENG_##_x##_table);}

#define TSTENG_TEST_TABLE_PROTOTYPE(_x) extern TSTENG_command_item_t TSTENG_##_x##_table[];

typedef int TSTENG_handle_t;

typedef struct {
    char testcase_str[TSTENG_MAX_CMD_STR_SIZE];
} TSTENG_socket_msg_t;

typedef enum {
    TSTENG_RESULT_OK,
    TSTENG_RESULT_FAIL
} TSTENG_result_t;

typedef enum {
    TSTENG_RTN_RESULT,
    TSTENG_RTN_INFO,
    TSTENG_RTN_END,
} TSTENG_rtn_type_t;

typedef struct {
    TSTENG_rtn_type_t type;
    unsigned long result;
    unsigned long len;
    char *string;
} TSTENG_socket_rtn_msg_t;

// Type of a command function
typedef int (*TSTENG_function_p) (TSTENG_handle_t handle,...);

typedef struct {
    char *command_p;
    TSTENG_function_p function_p;
    char *description;
    unsigned char type;
} TSTENG_command_item_t;

// This type contains a parsed command line
typedef struct {
    int num_args;        // The number of arguments found
    int arg_offsets[TSTENG_MAX_ARGS];    // The offset in CmdBuf for the arguments
    char command_buf[TSTENG_MAXLEN_CMDBUF];    // The entire command line
} TSTENG_command_line_t;

typedef struct TSTENG_table_s {
    char *name;
    TSTENG_command_item_t *table;
    struct TSTENG_table_s *next;
} TSTENG_table_t;

extern TSTENG_table_t *TSTENG_registered_tables;

void TSTENG_test_result_cb(TSTENG_handle_t handle, TSTENG_result_t result);
void TSTENG_test_info_cb(TSTENG_handle_t handle, TSTENG_result_t result, const char *str);
int TSTENG_execute_command(TSTENG_handle_t handle, const char *str_p, TSTENG_command_item_t ** cmd_pp);

void TSTENG_register_table(char *name, TSTENG_command_item_t * table);

#endif
