/*
 * Test app
 *
 * Generic framework to provide an IDBG test app
 *
 * main.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include "thermal.h"

#define MAX_ARGV_LEN (128)
#define MAX_CMD_LEN (128)

typedef int (*builtin_cmd_t)(int argc, char **argv);

typedef struct {
    char *name;
    builtin_cmd_t func;
} builtin_cmd_list_t;

static void usage(int argc, char **argv);
static int run_test_script(char *file);
static int interactive(void);
static void str_to_argv(char *str, int *argc, char **argv);
static int execute_builtin(int argc, char **argv);
static int execute_line(char *cmd);
static int ls(int argc, char **argv);
static int sleep_for(int argc, char **argv);
static int cmd_exit(int argc, char **argv);

// built in "shell" functions
static builtin_cmd_list_t builtin_cmds[] = {
    {"ls", ls},
    {"sleep", sleep_for},
    {"exit", cmd_exit},
    {NULL, NULL},
};

static int opt_v = 0;

/*
 * main
 */
int
main(int argc, char **argv) {
	int ch;
	char *opt_f = NULL;

	// deal with cmd line args
	while ((ch = getopt(argc, argv, "hivf:")) != -1) {
		switch (ch) {
		case 'v':
			opt_v = 1;
			break;

		case 'f':
			opt_f = strdup(optarg);
			break;

		case 'h':
		default:
			usage(argc, argv);
		}
	}

	argc -= optind;
	argv += optind;

	// register all test tables
	TEST_TABLE_REGISTER(THERMAL);

	// check cmd line args are valid
	if (opt_f && argc)
		fprintf(stderr, "invalid cmd line args\n");

	// if the cmd was passed in as arguments
	if (argc)
		return TEST_execute_cmd(argc, argv);

	// if we want to run a test script
	if (opt_f)
		return run_test_script(opt_f);

	// by default go into interactive mode
	return interactive();
}

/*
 * prints out usage example
 */
static void usage(int argc, char **argv) {
	printf("%s: [-hv] TABLE cmd\n", argv[0]);
	printf("%s: [-v] -f <FILENAME>\n", argv[0]);
	exit(-1);
}

/*
 * read a file passed in 1 line at a time and
 * fire through the test framework
 */
static int run_test_script(char *file) {
	FILE *f;
	char cmd[MAX_CMD_LEN];
	f = fopen(file, "r");

	if (f == NULL)
		err(errno, "unable to open %s", file);

	while (fgets(cmd, (MAX_CMD_LEN - 1), f) != NULL) {
		if (opt_v)
			printf("> %s", cmd);

		execute_line(cmd);
	}

	fclose(f);
	return 0;
}

/*
 * present an interactive prompt to the user
 */
static int interactive(void) {
	char cmd[MAX_CMD_LEN];

	while (1) {
		printf("> ");
		fflush(stdin);
		fgets(cmd, (MAX_CMD_LEN - 1), stdin);
		execute_line(cmd);
	}

	return 0;
}

/*
 * handle an input line
 */
static int execute_line(char *cmd) {
	char *argv[MAX_ARGV_LEN];
	int argc, i, err;

	// remove any new line chars & check for duff strings
	for (i = 0; i < MAX_CMD_LEN && cmd[i] != '\0'; i++) {
		if (cmd[i] == '\n')
			cmd[i] = '\0';
	}

	if (cmd[0] == '\0')
		return -1;

	// convert the string into an argc/argv vector
	str_to_argv(cmd, &argc, argv);

	// run any built in cmds (like ls..)
	if (!execute_builtin(argc, argv))
		return 0;

	// try and push it through the test engine
	err = TEST_execute_cmd(argc, argv);

	if (err)
		fprintf(stderr, "failed\n");

	return err;
}

/*
 * tries to execute a "built in" command
 */
static int execute_builtin(int argc, char **argv) {
	int i = 0;

	if (argc < 0 || argv[0] == NULL)
		return -1;

	while (builtin_cmds[i].name != NULL && builtin_cmds[i].func != NULL) {
		if (strcmp(builtin_cmds[i].name, argv[0]) == 0)
			return builtin_cmds[i].func(argc, argv);

		i++;
	}

	return -1;
}

/*
 * converts a string with spaces into an array split
 * by the space
 */
static void str_to_argv(char *str, int *argc, char **argv) {
	char **ap;
	*argc = 0;

	for (ap = argv;
	     ap < &argv[MAX_ARGV_LEN] && (*ap = strsep(&str, " \t")) != NULL;) {
		if (**ap != '\0')
			ap++;

		(*argc)++;
	}

	*ap = NULL;
}

/*
 * the built in ls command, to list all available commands
 */
static int ls(int argc, char **argv) {
	TEST_table_t *nxt = TEST_registered_tables;
	TEST_cmd_t *cmd;

	while (nxt != NULL) {
		printf("%s\n", nxt->name);
		cmd = nxt->cmds;

		while (cmd->name != NULL) {
			printf("* %s:\t%s\n", cmd->name, cmd->description);
			cmd++;
		}

		nxt = nxt->next;
	}

	return 0;
}

/*
 * Sleeps for n seconds
 */
static int sleep_for(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: sleep <seconds>\n");
		return -1;
	}

	sleep(atoi(argv[1]));

	return 0;
}

/*
 * Exit the test app
 */
static int cmd_exit(int argc, char **argv) {
	exit(0);
}
