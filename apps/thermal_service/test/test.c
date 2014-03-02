/*
 * Test app
 *
 * Generic framework to provide an IDBG test app
 *
 * test.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

TEST_table_t *TEST_registered_tables = NULL;

static TEST_cmd_t *find_test_cmd(char *table, char *cmd);

/*
 * registers a table with the framework
 */
int TEST_register_table(char *name, TEST_cmd_t *cmds) {
	TEST_table_t *table;
	table = malloc(sizeof(TEST_table_t));

	if (table == NULL)
		return -1;

	// set it all up
	table->name = name;
	table->next = NULL;
	table->cmds = cmds;

	// add it to the start
	table->next = TEST_registered_tables;
	TEST_registered_tables = table;
	return 0;
}

/*
 * executes a command registered with the test
 * framework
 */
int TEST_execute_cmd(int argc, char **argv) {
	TEST_cmd_t *cmd;

	if (argc < 2)
		return -1;

	cmd = find_test_cmd(argv[0], argv[1]);

	if (cmd == NULL)
		return -1;

	if (cmd->test == NULL)
		return -1;

	return cmd->test(argc, argv);
}

/*
 * searches the tables for the test command
 */
static TEST_cmd_t *find_test_cmd(char *table, char *cmd) {
	TEST_table_t *t;
	TEST_cmd_t *c;

	// find test table
	t = TEST_registered_tables;

	while (t != NULL) {
		if (strcmp(t->name, table) == 0)
			break;

		t = t->next;
	}

	if (t == NULL)
		return NULL;

	// find test cmd
	c = t->cmds;

	while (c->name != NULL) {
		if (strcmp(c->name, cmd) == 0)
			break;

		c++;
	}

	return c;
}
