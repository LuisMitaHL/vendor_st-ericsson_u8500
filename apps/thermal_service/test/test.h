/*
 * Test app
 *
 * Generic framework to provide an IDBG test app
 *
 * test.h
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef __TEST_H__
#define __TEST_H__

typedef int (*TEST_function_t)(int argc, char **argv);

typedef struct {
	char *name;
	char *description;
	TEST_function_t test;
} TEST_cmd_t;

typedef struct TEST_table_s {
	char *name;
	TEST_cmd_t *cmds;
	struct TEST_table_s *next;
} TEST_table_t;

#define TEST_OK (0)
#define TEST_FAILED (-1)

#define TEST_TABLE_BEGIN(_x) TEST_cmd_t TEST_TABLE_##_x##_CMDS[] = {
#define TEST_TABLE_CMD(name, descr, func) {name, descr, func},
#define TEST_TABLE_END {NULL, NULL, NULL}};

#define TEST_TABLE_REGISTER(_x) TEST_register_table(#_x, TEST_TABLE_##_x##_CMDS)
#define TEST_TABLE_PROTOTYPE(_x) extern TEST_cmd_t TEST_TABLE_##_x##_CMDS[];

extern TEST_table_t *TEST_registered_tables;

extern int TEST_register_table(char *name, TEST_cmd_t *cmds);
extern int TEST_execute_cmd(int argc, char **argv);
extern int TEST_execute_sub_cmd(int argc, char **argv);

#endif
