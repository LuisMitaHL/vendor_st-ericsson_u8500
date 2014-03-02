/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cli_p.h
 * \brief    CLI internal function declaration
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef __cli_p_h
#define __cli_p_h


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*-----------------------------------------*/
/* INCLUDES           					   */
/*-----------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#include "cli.h"
#include "los/api/file.h"

/*-----------------------------------------*/
/* DEFINES           					   */
/*-----------------------------------------*/

#define PROMPT "MMTE> "
#define MAX_NESTED_FILE     30

#define MAX_NESTED_SCRIPT   30

#ifdef __ARM_SYMBIAN
#define MAX_LINE          4096
#define MAX_PUSH_CMD      30
#else
#define MAX_LINE          4096
#define MAX_PUSH_CMD      10
#endif

#define MAX_ATE_CMD_ARGS  40

#define MAX_ITFS                 40
#define MAX_MONITOR_LINE_LENGTH  4096
#define MAX_PARAM_LENGTH         4096

#define CONSOLE_INPUT   0
#define RS232_INPUT     1
#define SCRIPT_INPUT    2

#define MALLOC_TABLE_SIZE  2048

/*-----------------------------------------*/
/*  Function prototypes                    */
/*-----------------------------------------*/
int             CLI_set_interface(char*, int);
void            CLI_change_slash_to_backslash(char *string);

int             CLI_domatch(char*, char*);
CmdPtr          CLI_command_match(char *);
void            CLI_extract_token(char *, char *, int*, char**);

// Following 2 are redundant ?? no ?
int            CLI_read_console( char *, int );
void            CLI_write_console(int, char *);

int             CLI_filepush(t_los_file*);
t_los_file*     CLI_filepop(void);
t_alias*        CLI_find_static_alias(char* name);
void            CLI_register_static_aliases(t_alias tab[]);

void            CLI_modify_alias(char* name, char* value);
char*           CLI_replace_alias(char* str_in);
t_cmd_function  CLI_inter(char* line);
char*           CLI_script_pop(void);
int             CLI_script_push(char * memory, char * name);
void            CLI_io_read(char *);
void            CLI_io_write(char *format, ...);
void            CLI_io_write_channel(int channel, char *format, ...);


char *          CLI_getpscript(void);
void            CLI_setpscript(char *);

void            CLI_addHistoric(char * new_command);








int CLI_exist_alias(const char* name);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
