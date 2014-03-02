/*
 * STELP Test App
 *
 * main.c
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <agpsosa.h>

#include <test_engine_process.h>
#include <test_engine_int.h>
#include <test_engine_util.h>

//#include <stelp_locationprovider.h>
#include <strings.h>

#include <stelp_tables.h>
#include <test_stelp_socket.h>
extern int vg_test_result;

#ifdef USE_LTP_FRAMEWORK
#include <test.h>
#include <usctest.h>
#endif

#ifdef USE_LTP_FRAMEWORK
extern int  Tst_count;
extern char *TESTDIR;

char *TCID = "POSMAN";
int  TST_TOTAL = 1;

static FILE *T_out = NULL;
static char fname[512];

#endif

void usage(void);
void interactive(void);

#ifdef USE_LTP_FRAMEWORK
extern void cleanup(void)
{
    // clean up functions
    tst_exit();
}
#endif

TSTENG_TEST_TABLE_PROTOTYPE(DATA)

#ifdef AGPS_LINUX_BARE_FTR
#define ALOGD   printf
#endif


int main(int argc, char *argv[])
{
    char str[TSTENG_MAX_CMD_STR_SIZE];
    int result;
    int opt_v = 0;
    int i;
    int ch;

    extern char *optarg;
    extern int optind, optopt;

#ifndef LBS_LINUX_UBUNTU_FTR
    LBSSTELP_socket_init();
#endif


    while ((ch = getopt(argc, argv, "hvp:")) != -1) {
      switch (ch) {
      case 'h':
      usage();
      break;
      case 'p':
      //STELP_set_port(atoi(optarg));
      break;
      case 'v':
      opt_v++;
      break;
      default:
      break;
      }
    }

    //STELP_set_debug_level(opt_v);

    argc -= optind;
    argv += optind;

    TSTENG_REGISTER_TABLE(NAF);
#ifndef LBS_LINUX_UBUNTU_FTR
    TSTENG_REGISTER_TABLE(SBEE);
    TSTENG_REGISTER_TABLE(AMILAT);
#endif
    //TSTENG_REGISTER_TABLE(MOLR);
    //TSTENG_REGISTER_TABLE(MTLR);
    #ifdef AGPS_UNIT_TEST_FTR
    TSTENG_REGISTER_TABLE(SUPL);
    #endif
    //TSTENG_REGISTER_TABLE(DATA);
    TSTENG_REGISTER_TABLE(TREQ);

    if (argc == 0) {
    interactive();
    } else {
    bzero(&str, TSTENG_MAX_CMD_STR_SIZE);

    for (i = 0; i < argc; i++) {
        strcat(str, argv[i]);
        strcat(str, " ");
    }

    result = TSTENG_execute_command(-1, str, NULL);
    TSTENG_test_result_cb(-1, result);

    ALOGD("END\n");
    }

    return 0;
}

void usage(void) {
  ALOGD("stelp_test_app [-p <portno>] MODULE cmd\n");
  exit(0);
}

/*
 * Test engine callbacks
 */
void TSTENG_test_result_cb(TSTENG_handle_t handle, TSTENG_result_t result) {
   handle=handle; /*Parameter not used.This is to avoid warning for th known fact*/

#ifdef USE_LTP_FRAMEWORK
    if (result == TSTENG_RESULT_OK)
    tst_resm(TPASS, "PASS");
    else
    tst_resm(TFAIL, "FAIL");
#else
    ALOGD("%s\n", (result == TSTENG_RESULT_OK ? "OK" : "FAIL"));
#endif
}

void TSTENG_test_info_cb(TSTENG_handle_t handle, TSTENG_result_t result, const char *str) {
    
    handle=handle; /*Parameter not used.This is to avoid warning for th known fact*/
#ifdef USE_LTP_FRAMEWORK
    if (result == TSTENG_RESULT_OK)
    tst_resm(TPASS, "%s\n",str);
    else
    tst_resm(TFAIL, "%s\n",str);
#else
    ALOGD("%s %s\n", (result == TSTENG_RESULT_OK ? "OK" : "FAIL"), str);
#endif
if(result == TSTENG_RESULT_OK)
    vg_test_result = TSTENG_RESULT_OK;
else
    vg_test_result = TSTENG_RESULT_FAIL;
}

#define MAX_CMD_LEN (255)


void list_subdirs(void) {
    TSTENG_table_t *table = TSTENG_registered_tables;

    while (table != NULL) {
    ALOGD("%s\n", table->name);
    table = table->next;
    }

}

TSTENG_table_t *cd(char *dir) {
    TSTENG_table_t *table = TSTENG_registered_tables;
    while (table != NULL) {
    if (strcmp(dir, table->name) == 0)
        return table;

    table = table->next;
    }

    return NULL;
}

void list_cmds(TSTENG_table_t *table) {
    TSTENG_command_item_t *cmd;

    cmd = table->table;
    while (cmd->command_p != NULL) {
    ALOGD("%s - %s\n", cmd->command_p, cmd->description);
    cmd++;
    }
}

void interactive(void) {
    TSTENG_table_t *curdir = NULL;
    char cmd[MAX_CMD_LEN];
    int quit = 0;
    int i = 0;

    while (!quit) {
    ALOGD("> ");
    fflush(stdin);
    fgets(cmd, MAX_CMD_LEN, stdin);

    for (i = 0; i < (MAX_CMD_LEN - 1) && cmd[++i] != '\n'; );
    cmd[i] = '\0';

    if (strcmp(cmd, "quit") == 0) {
        quit = 1;
        continue;
    }

    if (strcmp(cmd, "ls") == 0) {
        if (curdir == NULL) {
        list_subdirs();
        } else {
        list_cmds(curdir);
        }
        continue;
    }

    if (strncmp(cmd, "cd ", 3) == 0) {
        char dir[MAX_CMD_LEN];
        int j = 0;

        for (i = 3; cmd[i] != '\0'; dir[j++] = cmd[i++]);
        dir[j] = '\0';

        curdir = cd(dir);
        continue;
    }

    if (strcmp(cmd, "pwd") == 0) {
        if (curdir == NULL) {
        ALOGD(". \n");
        } else {
        ALOGD("%s\n", curdir->name);
        }
        continue;
    }


    {
        char exe[MAX_CMD_LEN*2];
        bzero(exe, MAX_CMD_LEN*2);

        strcpy(exe, cmd);

        TSTENG_execute_command(-1,
                   exe,
                   NULL);
    }
    }
}

