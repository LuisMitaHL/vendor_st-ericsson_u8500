/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cli.h
 * \brief    CLI API 
 * \author   ST-Ericsson
 */
/*****************************************************************************/

#ifndef __cli_h
#define __cli_h


#ifndef __SYMBIAN32__
    #define IMPORT_C
    #define EXPORT_C
#endif



#ifdef __cplusplus
extern "C"
{
#endif

#include "los/api/memory.h"

/*-----------------------------------------*/
/* Defines           					   */
/*-----------------------------------------*/
#define MASK_ERROR_DISPLAY          0x01
#define MASK_MSG_DISPLAY            0x02
#define MASK_WARNING_DISPLAY        0x04
#define MASK_DEBUG_DISPLAY          0x08
#define STATIC_ALIAS_TABS_MAX_NB    16

/*-----------------------------------------*/
/* typedef, enum      					   */
/*-----------------------------------------*/
enum CMD_COMPLETION{
    CMD_ERR_NONE        , // No error
    CMD_RETURN_FALSE=CMD_ERR_NONE, // No error	
	CMD_RETURN_TRUE		,
    CMD_ERR_ARGS        , // an argument is out of range 
    CMD_ERR_GENERAL     , // an error that has no specific categorisation (resource pb,file access,OMX...)
    CMD_ERR_NOMEMORY    , // an attempt to allocate memory has failed
    CMD_ERR_OMX         , // an OMX error. Refer to OMX error type.
    CMD_RUNNING         , // command still running (used for asynchronous operation)       
    CMD_COMPLETE = 0xff , // command complete. Can process next command
};
typedef enum CMD_COMPLETION CMD_COMPLETION;

typedef CMD_COMPLETION (* CmdPtr)( int, char ** );

typedef struct
{	char name[33];     // Command name
	CmdPtr command;    // Pointer to the command function
	char* helpstring;  // Help message
} t_cmd_list ;

typedef struct t_itf
{	char* group;
	int nb_funcs;
	t_cmd_list* list;
	int enabled;
} t_itf;

typedef struct
{	char*         name;
	char*         value;
    unsigned char persistance;
} t_alias;

typedef struct
{	CmdPtr  func;
    int     argc;
	char ** argv;
} t_cmd_function;

typedef struct
{	void *ptr;
	t_los_memory_handle buffid;
} t_malloc_table;

#define MAX_HISTORIC 20
typedef struct
{	int           HId;
	char*         cmd;
} t_historic;

enum {
	ERROR_DISPLAY,
	MSG_DISPLAY,
	WARNING_DISPLAY,
	DEBUG_DISPLAY,
    ERROR_IT_DISPLAY,
    MSG_IT_DISPLAY
};

/*-----------------------------------------*/
/* Exported functions prototypes           */
/*-----------------------------------------*/
IMPORT_C void         CLI_init(void);
IMPORT_C void         CLI_term(void);

IMPORT_C int          CLI_register_interface(char*, int, t_cmd_list*, int);
IMPORT_C void         CLI_register_static_aliases(t_alias alias_tab[]);

IMPORT_C unsigned int  CLI_set_parameter(char *, unsigned int);
IMPORT_C t_uint64      CLI_set_parameter64(char *, t_uint64);

IMPORT_C void           CLI_set_display_filter(int);
IMPORT_C int            CLI_get_display_filter(void);

IMPORT_C CMD_COMPLETION CLI_do_command(CmdPtr cmd, int, char **);

IMPORT_C t_cmd_function CLI_getNextFunction(void);
IMPORT_C void           CLI_addCommand(char * new_command);


IMPORT_C t_bool CLI_pushCommand(char * new_command);
IMPORT_C char*  CLI_pullCommand(void);
IMPORT_C t_bool CLI_PrintCommandPushed(t_bool bState);
IMPORT_C void   CLI_add_alias(char* name, char* value, unsigned char persistance);


// OSI/Linux implementation of command loop
IMPORT_C void           CLI_command_loop(char * commandfile);

IMPORT_C unsigned char  CLI_getCurrentInput(void);
IMPORT_C char*          CLI_get_current_script(void);

// Set of usefull function for command implementation
IMPORT_C void         CLI_disp_usage(CmdPtr command);
IMPORT_C unsigned int CLI_atoi(char *name);
IMPORT_C unsigned int CLI_atoi_without_msg(char *name);
IMPORT_C t_uint64     CLI_atol(char *name);
IMPORT_C char*        CLI_lltostr(unsigned int value);
IMPORT_C void*        CLI_malloc(int);
IMPORT_C void         CLI_free(void *);

void            CLI_Check_MallocTable(void);
void            CLI_Check_InitMallocTable(void);
void            CLI_Free_MallocTable(void);
IMPORT_C void   CLI_SkipOnError_Until(CmdPtr func);
    
// As strdup STD impl performs some malloc we have to use a LOS alloc based version
IMPORT_C char*  CLI_strdup(char* str);


// Display function
// TODO: prefer handler comming from the appli, passed in CLI_init
IMPORT_C void CLI_disp_error(char *format, ...);
IMPORT_C void CLI_disp_debug(char *format, ...);
IMPORT_C void CLI_disp(char *format, ...);
IMPORT_C void CLI_disp_msg(char *format, ...);
IMPORT_C void CLI_disp_warning(char *format, ...);
IMPORT_C void CLI_disp_it_error(char *format, ...);
IMPORT_C void CLI_disp_it_msg(char *format, ...);

// Error function utilities
IMPORT_C int    CLI_process_error (CMD_COMPLETION err);

IMPORT_C void   CLI_free_dynamic_aliases(void);
IMPORT_C t_alias*  CLI_find_alias(char* name);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
