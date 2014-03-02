/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cli_cmd.c
 * \brief    This module implements the native commande of the CLI
 *           Each command is associated to a function defined as follows :
 *           Commands functions name must be in the format C_group_command
 * \author   ST-Ericsson
 */
/*****************************************************************************/
/*-----------------------------------------*/
/* INCLUDES                                */
/*-----------------------------------------*/

#include "cli_p.h"

#ifdef __ARM_LINUX
#include <unistd.h>
#endif // __ARM_LINUX

#include "los/api/file.h"

/*-----------------------------------------*/
/* Constant                                */
/*-----------------------------------------*/

/**
 * @constant _MAX_LOOP
 * @description
 *  Maximum number of loop that can fit together.
 */
#define _MAX_LOOP ((int) 16)

/**
 * @constant _MAX_FILE_HANDLE
 * @description
 *  Maximum number of file handle that can be opened with 
 *  C_cli_open function.
 */
#define _MAX_FILE_HANDLE  ((t_uint16) 32)

/**
 * @constant _MAX_MESSAGE_SIZE
 * @description
 *  Maximum number of byte that can be printed 
 *  in one line into a file handle.
 */
#define _MAX_MESSAGE_SIZE  ((t_uint16) 256)


/*-----------------------------------------*/
/* Type                                    */
/*-----------------------------------------*/


/**
 * @type loop_struct
 * @description
 *   Structure  that defines the index of the loop start and
 *  indicates the remaining number of loop to be done.
 *
 * @fields
 *  - int loop_start
 *   - Pointer on the line after the loop declaration
 *  - int nb
 *   - Number of remaining loop.
 */

typedef struct 
{
    char * loop_start;
    int nb;
}  loop_struct ;


/**
 * @type file_desc
 * @description
 *   Structure  that makes the correspondance between a file alias and
 *  a file handle.
 *
 * @fields
 *  - int loop_start
 *   - Pointer on the line after the loop declaration
 *  - int nb
 *   - Number of remaining loop.
 */

typedef struct
{
    t_los_file * file_handle;
    char file_alias[256];
}  file_desc;


/*-----------------------------------------*/
/* Global variables                        */
/*-----------------------------------------*/
extern int    alias_cur_size;
extern int    nbitfs;
extern t_itf  itflist[MAX_ITFS];
extern char   aliastab[MAX_NESTED_FILE][MAX_ATE_CMD_ARGS][MAX_LINE];
extern int script_nb_argument[MAX_NESTED_SCRIPT];
extern t_alias *list_alias;
extern int    script_idx;
extern unsigned char current_input;
extern int    myexit;

extern t_historic list_historic[];
extern int hist_enable; 
extern int hist_size;
extern int hist_index;


/* CR FIDO 374781
   Disabling command printing
*/
extern t_bool bPushedCommandPrinting;


/*-----------------------------------------*/
/* Global variables for error management   */
/*-----------------------------------------*/
extern int gErrArgs;
extern int gErrNoMemory;
extern int gErrGeneral;
extern int gErrOmx;
extern int gErrUnknown;
extern int gErrTotal;


/* Array that make the correspondance between  file_aliases and the
   file handles .*/
file_desc _file_desc_array[_MAX_FILE_HANDLE];

/* Indicates if the file_desc_array was initialized. */
t_bool _init_file_desc_array = 0; 

/* Array of loop struct. The size of the array defines
 * The max number of loop that fit together*/
loop_struct  _loop_tab[_MAX_LOOP];

/* Loop index */
int _loop_idx = 0;

/* Fix imbricated If */
int _if_counter = 0;
int _if_index[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Condition for skipping unknown command in ..else condition.
extern t_bool CLI_enable_skip_unknown_command;

/*-----------------------------------------*/
/* Static functions prototypes             */
/*-----------------------------------------*/

static CMD_COMPLETION C_cli_alias(int, char **);
static CMD_COMPLETION C_cli_modifyalias(int, char **);
static CMD_COMPLETION C_cli_free_alias(int, char **);
static CMD_COMPLETION C_cli_unalias(int, char **);
static CMD_COMPLETION C_cli_do(int, char **);
static CMD_COMPLETION C_cli_help(int, char **);
static CMD_COMPLETION C_cli_ate_ena_itf(int, char **);
static CMD_COMPLETION C_cli_echo(int, char **);
#ifndef __ARM_SYMBIAN
static CMD_COMPLETION C_cli_quit(int, char **);
#endif
static CMD_COMPLETION C_cli_check_malloc(int, char **);
static CMD_COMPLETION C_cli_loop(int, char **);
static CMD_COMPLETION C_cli_endloop(int, char **);
static CMD_COMPLETION C_cli_print_file(int,char **);
static CMD_COMPLETION C_cli_open_file(int,char **);
static CMD_COMPLETION C_cli_close_file(int,char **);
static CMD_COMPLETION C_cli_if(int,char **);
static CMD_COMPLETION C_cli_else(int,char **);
static CMD_COMPLETION C_cli_endif(int,char **);
static CMD_COMPLETION C_cli_check_error(int,char **);
static CMD_COMPLETION C_cli_enable_disp_alias_processing(int argc, char **argv);
static CMD_COMPLETION C_cli_disable_disp_alias_processing(int argc, char **argv);
static CMD_COMPLETION C_cli_historic(int,char **);
static CMD_COMPLETION C_cli_enable_skip_unknown_command(int,char **);
static CMD_COMPLETION C_cli_disable_skip_unknown_command(int,char **);


static CMD_COMPLETION C_cli_existalias(int, char **);

/*-----------------------------------------*/
/* ate commands list                       */
/*-----------------------------------------*/

t_cmd_list native_cmd_list[] = {
    { "alias", C_cli_alias, "alias: set an alias\n"},
    { "arithmeticalias", C_cli_modifyalias, "modify alias: arithmeticalias <myalias><+ - / * % > < ! ^> <param>\n"},
    { "free_alias", C_cli_free_alias, "free_alias: free every alias\n"},
    { "unalias", C_cli_unalias, "unalias: remove one alias\n"},
    { "exist_alias", C_cli_existalias, "exist_alias name result: ckeck if given alias exist (result= no=0, static=1, regular=2)\n"},
    { "do", C_cli_do, "do: Load a command file \n"},
    { "ate_ena_itf", C_cli_ate_ena_itf, "ate_ena_itf : activate or deactivate interface (group of commands)\n"},
    { "help", C_cli_help, "help: display this message\n"},
    { "?", C_cli_help, "?: display this message\n"},
    { "echo", C_cli_echo, "echo: display a string (with alias expansion)\n"},
#ifndef __ARM_SYMBIAN
    { "quit", C_cli_quit, "quit: quit the monitor\n"},
#endif
    { "check_malloc", C_cli_check_malloc, "check_malloc Check if the MallocTable array is empty\n"},
    { "loop", C_cli_loop, "loop : Make a n times loop of the code between the loop instruction and the endloop key word\n"},
    { "endloop", C_cli_endloop, "endloop : Defines the end of a loop\n"},
    { "print_file", C_cli_print_file,"print_file\t<file_handle_name><message> \n" },
    { "open_file", C_cli_open_file,"open_file\t<file_handle_name><file_path>\n" },
    { "close_file", C_cli_close_file,"close_file\t<file_handle_name>\n"},
    { "if", C_cli_if,"if\t<var1> <test> <var2>\n"},
    { "else", C_cli_else,"else\n"},
    { "endif", C_cli_endif,"endif\n"},
    { "enable_disp_alias_process", C_cli_enable_disp_alias_processing,"enable_disp_alias_process\n"},
    { "disable_disp_alias_process", C_cli_disable_disp_alias_processing,"disable_disp_alias_process\n"},
    { "check_error",C_cli_check_error,"check_error : prompt number of error returned by commands \n" },
    { "hist", C_cli_historic,"hist\ton|off|list|<id>\n"},
    { "enable_skip_unknown_command", C_cli_enable_skip_unknown_command,"enable_skip_unknown_command (useful in if..else consition)\n"},
    { "disable_skip_unknown_command", C_cli_disable_skip_unknown_command,"disable_skip_unknown_command (useful in if..else consition)\n"},
};


/** ------------------------------------------------------------------
 * function : CLI_init
 * \param   :
 * \return  : none
 * Description: runs a script
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_init(void) {
    int nb_of_native_cmd = sizeof(native_cmd_list) / sizeof(native_cmd_list[0]);

    CLI_register_interface("native", nb_of_native_cmd, native_cmd_list, 1);
#ifdef __ARM_SYMBIAN
#else
    CLI_Check_InitMallocTable();
#endif
/*  Define CLI_PLATFORM versus the platform where it is used */
#ifdef __SYMBIAN32__
	CLI_add_alias("CLI_PLATFORM", "symbian", 1);
#elif defined(WIN32)
	CLI_add_alias("CLI_PLATFORM", "win32", 1);
#elif defined(ANDROID)
	CLI_add_alias("CLI_PLATFORM", "android", 1);
#elif defined(WORKSTATION)
	CLI_add_alias("CLI_PLATFORM", "x86", 1);
#else
	CLI_add_alias("CLI_PLATFORM", "linux", 1);
#endif

}


/** ------------------------------------------------------------------
 * function : CLI_term
 * \param   :
 * \return  : none
 * Description: Terminates properly CLI. Free ressources
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_term(void) {
    
    CLI_free_dynamic_aliases();
    CLI_Free_MallocTable();
}

/* ----------------------------------------------------------------- */
/**
  \defgroup check_malloc check_malloc
  \ingroup cli_cmd
  \b Usage:
  \code check_malloc \endcode

  tis functions checks if the MallocTable array is empty
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_check_malloc(int argc, char **argv)
{
    CLI_Check_MallocTable();

    return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup disp_alias_processing disp_alias_processing
  \ingroup cli_cmd
  \b Usage:
  \code disp_alias_processing \endcode

  this functions eable display of commandf after alias substitution
  */
/* ----------------------------------------------------------------- */
extern int CLI_displayAfterAliasReplacement;
CMD_COMPLETION C_cli_enable_disp_alias_processing(int argc, char **argv){
    CLI_displayAfterAliasReplacement = 1;
    return CMD_COMPLETE;
}

CMD_COMPLETION C_cli_disable_disp_alias_processing(int argc, char **argv){
    CLI_displayAfterAliasReplacement = 0;
    return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup alias alias
  \ingroup cli_cmd
  \b Usage:
  \code alias [name [value [persitence]]] \endcode
  \param name       : name of the alias
  \param value      : value of the alias
  \param persistance: persitance of the alias. 
  0=non persistant
  1=persistant (ex: needed for regression)   

  This function set or display an alias.\n
  Call whitout parameter, it displays all defined aliases.\n
  Call with only name parameter, it displays its value if name is defined as an alias\n
  Call with name and value, it creates an alias \b name with value \b value and with persistance 0.
  Call with name, value and persistance , it creates an alias \b name with value \b value and with persitance \b persistance.
  If \b name was already defined as an alias then it previous value is overwritten.
  For more details on alias mechanism see \ref aliases
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_alias(int argc, char **argv)
{
    unsigned char persistance = 0;
    if (argc == 1)
    {   
        int i;
        for(i = 0; i < alias_cur_size; i++)
        {   if ( list_alias[i].name != NULL && list_alias[i].name[0] != '$')
            CLI_disp_msg("%s\t\t%s\n", list_alias[i].name, list_alias[i].value);
        }
    
    }
    
    else if (argc == 2)
    {
        int i;
        for(i = 0; i < alias_cur_size; i++)
        {
            // check that list_alias[i].name not NULL before making comparaison. Causes a Kernel fault otherwise.
            if (list_alias[i].name != NULL)
                {
                    if (!strcmp(list_alias[i].name, argv[1]))
                        CLI_disp_msg("[%s] => [%s]\n",list_alias[i].name, list_alias[i].value);
                }
        }
    }
    
    else if (argc == 4)
    {
        persistance = (unsigned char)CLI_atoi(argv[3]) ;
    }

    if (argc >=3 )
        {
        CLI_add_alias(argv[1], argv[2],persistance);
        }

    return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup arithmeticalias arithmeticalias 
  \ingroup cli_cmd
  \b Usage:
  \code  arithmeticalias [name][operation type][value] \endcode
  \param name          : name of the alias
  \param operatio type : + - / * 
  \param value         : value of arithmetic operation

  This function modifies an alias.\n
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_modifyalias(int argc, char **argv)
{
    if (argc == 3)
    {   
		CLI_modify_alias(argv[1], argv[2]);
    }

    return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup free_alias free_alias
  \ingroup cli_cmd
  \b Usage:
  \code free_alias \endcode
  \param none

  This function deletes every alias created by user \ref alias
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_free_alias(int argc, char **argv)
{
    CLI_free_dynamic_aliases();
    return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup unalias unalias
  \ingroup cli_cmd
  \b Usage:
  \code unalias <name> \endcode
  \param name Name of the alias to delete

  This function deletes one alias created with \ref alias
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_unalias(int argc, char **argv)
{
    t_alias * al;
    if(argc == 2)
    {
        if (CLI_find_static_alias(argv[1]))
        {
            CLI_disp_error("%s is a static alias\n",argv[1]);
            return CMD_ERR_GENERAL;
        }


        al = CLI_find_alias(argv[1]);
        if(al)
        {   CLI_free(al->name);
            CLI_free(al->value);
            al->name = NULL;
            al->value = NULL;
        }
        else
        {   
            CLI_disp_warning("Unknown alias [%s]\n",argv[1]);
            return CMD_ERR_GENERAL;
        }
    }
    return CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
  \defgroup do do
  \ingroup cli_cmd
  \b Usage:
  \code do <ate_script> \endcode
  \param ate_script Name of the ATE script to execute

  This function execute an ATE script. After this command, the script
  will become the current ATE input until EOF is reached or another script is
  opened by this one. Up to 5 scripts can be nested.
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_do(int cargc, char *cargv[])
{
    if (cargc >= 2)
    {
        unsigned short        i;
        t_los_file            *p_File;
        unsigned long         FileSize;
        char*                 pScriptAdd;
        char *                script_name = cargv[1];

        // get size of script
        p_File = LOS_fopen(script_name, "rb");
        if (p_File == NULL)
        {   CLI_disp_error("Could NOT open command file '%s'\n",script_name);
            return CMD_ERR_GENERAL;
        }
        LOS_fseek(p_File, 0, LOS_SEEK_END);
        FileSize = LOS_ftell(p_File);
        LOS_fseek(p_File, 0, LOS_SEEK_SET);

        CLI_disp_msg("Loading command file : %s  (%d bytes)\n", script_name, FileSize);

        // Allocate memory for script
        pScriptAdd = CLI_malloc(FileSize+1);
        if (pScriptAdd == NULL)
        {   CLI_disp_error("Could NOT allocate memory for command file!\n");
            return CMD_ERR_NOMEMORY;
        }

        // load script in allocated memory
        FileSize = LOS_fread(pScriptAdd, 1, FileSize, p_File);

        LOS_fclose(p_File);
        if (FileSize==0)
        {   CLI_disp_error("Could not load the script!\n");
            return CMD_ERR_GENERAL;
        }
        *(pScriptAdd+FileSize) = '\0';  // add NULL at the end of the script.

#if (defined (TLM_PLATFORM) || defined(__ARM_SYMBIAN))
        CLI_change_slash_to_backslash(pScriptAdd);
#endif //TLM_PLATFORM

        if (CLI_script_push(pScriptAdd,script_name))
        {
            current_input = SCRIPT_INPUT;

            script_nb_argument[script_idx]=(cargc-1);


            for (i = 0; i < (cargc-1); i++)
            {
                char name[100];
                t_alias* palias;
                sprintf(name, "$%d", i);
                palias = CLI_find_alias(name);
                strcpy(aliastab[script_idx-1][i], palias ? palias->value : "");

                CLI_add_alias(name,cargv[i+1],1);
            }
        }

    }
    else
    {
        CLI_disp_error("do <filename> \n");
        return CMD_ERR_ARGS;
    }
    return CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
  \defgroup help help
  \ingroup cli_cmd
  \b Usage:
  \code help [interface_name / function_name] \endcode
  \code ? [interface_name / function_name] \endcode

  print help of the specified interface or function.
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_help( int cargc, char ** cargv)
{
    int i, j;
    char help_cmd[20];

    if (cargc == 2)
    {
        sscanf(cargv[1],"%s",help_cmd);
        if (!strcmp(help_cmd, "groups"))
        {
            for (i = 0; i < nbitfs; i++)
            {
                CLI_disp("%s\t\t%s\n", itflist[i].group, itflist[i].enabled ? "enabled" : "disabled");
            }
        }

        /* List help of all functions of a specified interface (if any) */
        for (i = 0; i < nbitfs; i++)
        {
            if (!strcmp(itflist[i].group, help_cmd))
            {
                if ( itflist[i].enabled)
                {
                    for (j = 0; j < itflist[i].nb_funcs; j++)
                    {
                        CLI_disp("+ %s",itflist[i].list[j].helpstring);
                    }
                }
                else
                {
                    CLI_disp("Class [%s] disabled!\n",itflist[i].group);
                }
                return CMD_COMPLETE;
            }
        }

        /* List help of a specified function */
        for (i = 0; i < nbitfs; i++)
        {
            if ( itflist[i].enabled)
            {
                for (j = 0; j < itflist[i].nb_funcs; j++)
                {
                    if (!strcmp(itflist[i].list[j].name, help_cmd))
                    {
                        CLI_disp(itflist[i].list[j].helpstring);
                    }
                }
            }
        }
    }
    else
    {
        /* List help of all functions */
        for (i = 0; i < nbitfs; i++)
        {
            if ( itflist[i].enabled)
            {
                CLI_disp("\n-- Available function in interface [%s]\n",
                        itflist[i].group);
                CLI_disp("-----------------------------------------------\n");
                for (j = 0; j < itflist[i].nb_funcs; j++)
                {
                    CLI_disp("+ %s",itflist[i].list[j].helpstring);
                }
            }
        }
    }
    return CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
  \defgroup ate_ena_itf ate_ena_itf
  \ingroup cli_cmd
  \b Usage:
  \code ate_ena_itf <interface_name> \endcode

  Enable command interface
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_ate_ena_itf( int cargc, char ** cargv)
{
    int ena;
    char group[50];

    if (cargc == 3)
    {
        sscanf(cargv[1],"%s",group);
        sscanf(cargv[2],"%d",&ena);
        CLI_set_interface(group, ena);
    }
    return CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
  \defgroup echo echo
  \ingroup cli_cmd
  \b Usage:
  \code alias <string> \endcode
  \param string to display

  Display a string with alias expansion
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_echo(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc; i++)
    {
        CLI_disp( "%s ", argv[i]);
    }
    CLI_disp("\n");
    return CMD_COMPLETE;
}

#ifndef __ARM_SYMBIAN
/* ----------------------------------------------------------------- */
/**
  \defgroup quit quit
  \ingroup cli_cmd
  \b Usage:
  \code quit \endcode

  Exit ATE
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_quit( int cargc, char ** cargv )
{
    myexit = 1;
    return CMD_COMPLETE;
}
#endif


/* ----------------------------------------------------------------- */
/**
  \defgroup loop loop 
  \ingroup cli_cmd
  \b Usage:
  \code loop <loop_number>
  \param
  - loop_number
  - Number of executed loop.
  \description
  Makes a loop of loop_number times of the code placed between 
  the loop key word and the endloop keyword.
  This key word works only in an mmte script and not in the 
  command line directly.
  */
/* ----------------------------------------------------------------- */

CMD_COMPLETION C_cli_loop(int argc, char **argv)
{
    _loop_idx++;
    if(_loop_idx < _MAX_LOOP )
    {
        /* We register the loop */
        _loop_tab[_loop_idx].nb = CLI_atoi(argv[1]);
        _loop_tab[_loop_idx].loop_start =  CLI_getpscript();
    }
    else
    {

        /* The maximum number of loop is reached, don't take care of this new loop */
        _loop_idx--; 
        CLI_disp_warning("loop: You already reached the maximum number of loop which fit together \n");

    }/* !if(loop_idx < (_MAX_LOOP-1)) */
    return CMD_COMPLETE;

}/* C_cli_loop(int argc, char **argv) */


/* ----------------------------------------------------------------- */
/**
  \defgroup endloop  endloop
  \ingroup cli_cmd
  \b Usage: 
  \code endloop  \endcode
  \description
  Marks the end of the loop.
  */
/* ----------------------------------------------------------------- */

CMD_COMPLETION C_cli_endloop(int argc, char **argv)
{

    /* The endloop key word is reached, so the number of loop to be done is 
     * decreased.*/
    _loop_tab[_loop_idx].nb--;

    /* tests that it was not the last loop */
    if(_loop_tab[_loop_idx].nb > 0)
    {
        /* Another loop must be done, set the script pointer on the line
         * after the loop key word.
         */
		CLI_disp_msg("\n Another loop must be done : index = %d\n\n", _loop_tab[_loop_idx].nb);
        CLI_setpscript(_loop_tab[_loop_idx].loop_start);
    }/* if(loop_tab[loop_idx].nb > 0)  */
    else
    {
        /* The loop is entirely finished, release ressources associated to this loop. */
        _loop_idx--;
    }/* ! if(loop_tab[loop_idx].nb > 0)*/

    return CMD_COMPLETE;  

}/* C_cli_endloop(int argc, char **argv) */


/* ----------------------------------------------------------------- */
/*
 * @description   _search_file_alias
 * 
 *  @arguments
 *      - IN char * alias_name
 *        - Alias name searshed in the _file_desc_array
 *  @returns
 *    - t_uint16
 *      - Returns the _file_desc_array index that match the alias_name.
 *
 * @description
 *   Finds the index of _file_desc_array where alias_name matches with
 *   file_alias field.
 */

/* ----------------------------------------------------------------- */

static t_uint16 _search_file_alias(char * alias_name)
{
    t_uint16 i =0;

    while(strcmp( alias_name,_file_desc_array[i].file_alias) && i< _MAX_FILE_HANDLE)
    {
        i++;
    }/* while(_file_desc_array[i].file_handle != NULL && i< _MAX_FILE_HANDLE) */

    if(strcmp( alias_name,_file_desc_array[i].file_alias))
    {
        /* The given file is not yet opened */
        return _MAX_FILE_HANDLE;
    }/* if(strcmp( alias_name,_file_desc_array[i].file_alias)) */
    else 
        return i;

} /*static t_uint16 _search_file_alias(char * alias_name) */


/* ----------------------------------------------------------------- */
/**
 *  \defgroup open_file open_file 
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code open_file <file_handle_name> <File_path> \endcode
 *  \param
 *      - file_handle_name
 *       - File handle that will be associated to the opened file
 *      - file_path
 *       - Path to the file that will be opened.
 *
 * \description
 *   Opens the file with the given path.
 */

/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_open_file(int argc,char ** argv)
{
    t_uint16 i =0;
    if(_init_file_desc_array == 0)
    {
        for ( i=0; i< _MAX_FILE_HANDLE ; i++)
        {
            _file_desc_array[i].file_handle = NULL;
            strcpy(_file_desc_array[i].file_alias,"\0");
        }/* for ( i=0; i<32; i++) */
        _init_file_desc_array = 1;
    }/*  if(_init_file_desc_array == 0) */
    i=0;


    /* Checks that the file was not already opened */
    if( _search_file_alias(argv[1]) != _MAX_FILE_HANDLE )
    {
        /* The file was already opened, do not reopened a new handle */
        CLI_disp_warning("You already opened the file_alias %s  \n",argv[1]);
        return CMD_COMPLETE;
    }

    while(_file_desc_array[i].file_handle != NULL && i< _MAX_FILE_HANDLE)
    {
        i++;
    }/* while(_file_desc_array[i].file_handle != NULL && i<32) */

    if( _file_desc_array[i].file_handle != NULL)
    {
        return CMD_COMPLETE;
    }/* if( _file_desc_array[i].file_handle != NULL) */


    _file_desc_array[i].file_handle = LOS_fopen( argv[2], "w" );


    /* The file was correctly opened, we can registered the correponding alias. */
    if(  _file_desc_array[i].file_handle )
        strcpy(_file_desc_array[i].file_alias,argv[1]);

    return CMD_COMPLETE;
}/*CMD_COMPLETION C_cli_open(int,char **) */

/* ----------------------------------------------------------------- */
/**
 *  \defgroup close_file close_file 
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code close_file <file_handle_name> \endcode
 *  \param
 *      - file_handle_name
 *        - File handle name
 *
 * \description
 *   Closes the given file.
 */

/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_close_file(int argc,char ** argv)
{
    t_uint16 i =0;

    i =  _search_file_alias(argv[1]);
    if( i >=  _MAX_FILE_HANDLE )
    {
        /* no handle with this name is found */ 
        CLI_disp_warning("No file with alias %s is found. Cannot close the file",argv[1]);
        return  CMD_ERR_GENERAL;
    }
    LOS_fclose( _file_desc_array[i].file_handle );
    _file_desc_array[i].file_handle = NULL;
    strcpy(_file_desc_array[i].file_alias,"\0");
    return  CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
 *  \defgroup print_file
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code print_file <file_handle_name> <text> \endcode
 *  \param
 *      - file_handle_name
 *        - File handle name
 *      - text
 *        - text printed into the file.
 *
 * \description
 *   Prints the given text.
 */

/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_print_file(int argc ,char ** argv) {
    t_uint16 i =0;
    int argc_count;
    char message[_MAX_MESSAGE_SIZE] = "";

    for(argc_count = 2 ; argc_count < argc; argc_count++) {
        sprintf(message,"%s %s",message, argv[argc_count]);
    }
    sprintf(message,"%s \n",message);

    i =  _search_file_alias(argv[1]);
    if( i >=  _MAX_FILE_HANDLE ) {
        /* no handle with this name is found */ 
        CLI_disp_warning("No file with alias %s is found. Cannot close the file\n",argv[1]);
        return  CMD_ERR_GENERAL;
    }
    else {
        LOS_fwrite( message, strlen(message), 1, _file_desc_array[i].file_handle);
        return  CMD_COMPLETE;
    }
}

/* ----------------------------------------------------------------- */
/**
 *  \defgroup if if
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code if <var1> <test> <var2> \endcode
 *  \param
 *    - var1
 *      - First operand
 *    - test
 *      - Test applied to the two operands.
 *    - var2
 *      - Second operand
 *
 * \description
 *   Makes the comparaison of var1 and var2 with test comparator.
 */

/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_if(int argc ,char ** argv) {

    char *arg1=argv[1], *arg2=argv[3];
    int compare;
    t_cmd_function next_cmd;
    int skipFirstStatement = 0;
	unsigned int ui_val_1 = 0, ui_val_2 = 0;
	int i_val_1 = 0, i_val_2 = 0;
	char *argHandle = NULL;

    char _goimbricated;

    t_bool save_skipUnknowCommand = CLI_enable_skip_unknown_command;

    /* CR FIDO 374781
       Disabling command printing
    */
    t_bool save_bPushedCommandPrinting = bPushedCommandPrinting;


    _if_index[_if_counter] = 1;
    _if_counter++;
    if (_if_counter > sizeof(_if_index)/sizeof(_if_index[0])) {
        CLI_disp_error("Too many nested ifs (max %d levels)\n", sizeof(_if_index)/sizeof(_if_index[0])); 
        return CMD_ERR_GENERAL;
    }

    if(argc == 4) {
		// CLI_atoi() returns an unsigned int
		if(arg1[0] == '-')
		{
			// skipped '-' character
			argHandle = arg1 + 1;
			i_val_1 = CLI_atoi_without_msg(argHandle);
			i_val_1 = -i_val_1;
		    CLI_disp_debug("using i_val_1 = %d\n", i_val_1);
		}
		else
		{
			ui_val_1 = CLI_atoi_without_msg(arg1);
		    CLI_disp_debug("using ui_val_1 = %d\n", ui_val_1);
		}
		if(arg2[0] == '-')
		{
			// skipped '-' character
			argHandle = arg2 + 1;
			i_val_2 = CLI_atoi_without_msg(argHandle);
			i_val_2 = -i_val_2;
		    CLI_disp_debug("using i_val_2 = %d\n", i_val_2);
		}
		else
		{
			ui_val_2 = CLI_atoi_without_msg(arg2);
		    CLI_disp_debug("using ui_val_2 = %d\n", ui_val_2);
		}


        compare = strcmp(arg1, arg2);

        /* Checks the comparator */
        if( !strcmp(argv[2],"==")) {
            if(compare!=0) {
                skipFirstStatement = 1;
            }
        }
        else if( !strcmp(argv[2],"!=")) {
            if(compare == 0) {
                skipFirstStatement = 1;
            }
        }
		else if( !strcmp(argv[2],"<") ) {
			if(arg1[0] == '-')
			{
				if(arg2[0] == '-')
				{
					if( i_val_1 >= i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( i_val_1 >= (int)ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
			else
			{
				if(arg2[0] == '-')
				{
					if( (int)ui_val_1 >= i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( ui_val_1 >= ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
		}
		else if( !strcmp(argv[2],"<=") ) {
			if(arg1[0] == '-')
			{
				if(arg2[0] == '-')
				{
					if( i_val_1 > i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( i_val_1 > (int)ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
			else
			{
				if(arg2[0] == '-')
				{
					if( (int)ui_val_1 > i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( ui_val_1 > ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
		}
		else if( !strcmp(argv[2],">") ) {
			if(arg1[0] == '-')
			{
				if(arg2[0] == '-')
				{
					if( i_val_1 <= i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( i_val_1 <= (int)ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
			else
			{
				if(arg2[0] == '-')
				{
					if( (int)ui_val_1 <= i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( ui_val_1 <= ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
		}
		else if( !strcmp(argv[2],">=") ) {
			if(arg1[0] == '-')
			{
				if(arg2[0] == '-')
				{
					if( i_val_1 < i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( i_val_1 < (int)ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
			else
			{
				if(arg2[0] == '-')
				{
					if( (int)ui_val_1 < i_val_2) {
						skipFirstStatement = 1;
					}
				}
				else
				{
					if( ui_val_1 < ui_val_2) {
						skipFirstStatement = 1;
					}
				}
			}
		}
    }
    else if(argc == 2) {
        if( ! arg1 ) {
            skipFirstStatement = 1;
        }
    }
    else {
        CLI_disp_error("Malformed IF.\n usage: IF <arg1> <test> <arg2> \n");   
    }

    if(skipFirstStatement)
		CLI_disp_msg("IF boolean result is FALSE\n");   
	else		
		CLI_disp_msg("IF boolean result is TRUE\n");   	

    if(skipFirstStatement){
        CLI_enable_skip_unknown_command = TRUE;

        /* Jumps to the else key word */
        do {
	    //printf("Ct:%d-", _if_counter);	// imbricated if
            /* Do nothing just jump functions 
             * until the else key word is detected.  
             */

            /* CR FIDO 374781
               Disabling command printing
            */
            bPushedCommandPrinting=FALSE;

        	next_cmd = CLI_getNextFunction();
            if (next_cmd.func != NULL){
                CLI_free (next_cmd.argv);
            }

	    /* Fix imbricated If */
	    _goimbricated=0;
            if (next_cmd.func == C_cli_if) {
                _if_counter++;
                if (_if_counter > sizeof(_if_index)/sizeof(_if_index[0])) {
                    CLI_disp_error("Too many nested ifs (max %d levels)\n", sizeof(_if_index)/sizeof(_if_index[0])); 
                    return CMD_ERR_GENERAL;
                }
            }
            if (next_cmd.func == C_cli_else && _if_index[_if_counter-1] == 0) {
	    	_goimbricated=1;
	    }
            if (next_cmd.func == C_cli_endif && _if_index[_if_counter-1] == 0) {
		_if_counter--;
	    	_goimbricated=1;
	    }

        }  while( _goimbricated == 1 || (next_cmd.func != C_cli_else  &&  next_cmd.func != C_cli_endif) ); 

        CLI_enable_skip_unknown_command = save_skipUnknowCommand;

        /* CR FIDO 374781
           Disabling command printing
        */
        if (next_cmd.func == C_cli_else)
            CLI_disp_msg("> else\n");

        if (next_cmd.func == C_cli_endif)
            CLI_disp_msg("> endif\n");

        bPushedCommandPrinting = save_bPushedCommandPrinting;

        if (next_cmd.func == C_cli_endif ) {
	    _if_counter--;
	    _if_index[_if_counter] = 0;
    	}
    }

    return  CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
 *  \defgroup else else
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code else \endcode
 *  \param
 *
 * \description
 *   Else function associated to IF
 */

/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_else(int argc ,char ** argv)
{

    t_cmd_function next_cmd;        
    char _goimbricated;

    t_bool save_skipUnknowCommand = CLI_enable_skip_unknown_command;

    /* CR FIDO 374781
       Disabling command printing
    */
    t_bool save_bPushedCommandPrinting = bPushedCommandPrinting;


    /* If we are in this function, it means that the 
       The previous if was correct.
       So we jump to the endif key word.
       */
    CLI_enable_skip_unknown_command = TRUE;
    do{


	//printf("Ct:%d-", _if_counter);	// imbricated if
        /* Do nothing just jump functions 
         * until the endif key word is detected  
         */  

        /* CR FIDO 374781
           Disabling command printing
        */
        bPushedCommandPrinting=FALSE;

        next_cmd = CLI_getNextFunction();
        if (next_cmd.func != NULL){
            CLI_free (next_cmd.argv);
        }

	/* Fix imbricated If */
	_goimbricated=0;
        if (next_cmd.func == C_cli_if) {
            _if_counter++;
            if (_if_counter > sizeof(_if_index)/sizeof(_if_index[0])) {
                CLI_disp_error("Too many nested ifs (max %d levels)\n", sizeof(_if_index)/sizeof(_if_index[0])); 
                return CMD_ERR_GENERAL;
            }
        }
        if (next_cmd.func == C_cli_endif && _if_index[_if_counter-1] == 0) {
	    _if_counter--;
	    _goimbricated=1;
	}
    }while(_goimbricated == 1 || next_cmd.func != C_cli_endif );

    /* Fix imbricated If */
    if (next_cmd.func == C_cli_endif) {
	_if_counter--;
	_if_index[_if_counter] = 0;
    CLI_disp_msg("> endif\n");
    }

    CLI_enable_skip_unknown_command = save_skipUnknowCommand;

    /* CR FIDO 374781
       Disabling command printing
    */
    bPushedCommandPrinting = save_bPushedCommandPrinting;

    return  CMD_COMPLETE;

}/* CMD_COMPLETION C_cli_else(int argc ,char ** argv) */



/* ----------------------------------------------------------------- */
/**
 *  \defgroup endif endif
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code endif \endcode
 *  \param
 *
 *
 * \description
 *   Endif function associated to IF
 */

/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_endif(int argc ,char ** argv)
{
    /* Simply do nothing, BUT can not be removed because the function pointer 
       is used in C_cli_else function!!
       */
    /* Fix imbricated If */
    _if_counter--;
    _if_index[_if_counter] = 0;

    return  CMD_COMPLETE;

}/* CMD_COMPLETION C_cli_endif(int argc ,char ** argv) */



/* ----------------------------------------------------------------- */
/**
  \defgroup cli_check_error cli_check_error
  \ingroup cli_cmd
  \b Usage:
  \code cli_check_erro \endcode

  This function prompts number of error returned by commands
  This function is usefull for test report generation
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_check_error(int argc, char **argv)
{

    if(gErrArgs)
    {   CLI_disp_error("Detected %u ARGS error(s)\n", gErrArgs);
        CLI_disp_error("ERROR ARGUMENTS STATUS: FAILED\n");
    }
    else
    {   CLI_disp("ERROR ARGUMENTS STATUS: PASSED\n");
    }

    if(gErrNoMemory)
    {   CLI_disp_error("Detected %u NOMEMORY error(s)\n", gErrNoMemory);
        CLI_disp_error("ERROR NOMEMORY STATUS: FAILED\n");
    }
    else
    {   CLI_disp("ERROR NOMEMORY STATUS: PASSED\n");
    }

    if(gErrGeneral)
    {   CLI_disp_error("Detected %u GENERAL error(s)\n", gErrGeneral);
        CLI_disp_error("ERROR GENERAL STATUS: FAILED\n");
    }
    else
    {   CLI_disp("ERROR GENERAL STATUS: PASSED\n");
    }

    if(gErrOmx)
    {   CLI_disp_error("Detected %u OMX error(s)\n", gErrOmx);
        CLI_disp_error("ERROR OMX STATUS: FAILED\n");
    }
    else
    {   CLI_disp("ERROR OMX STATUS: PASSED\n");
    }

    if(gErrUnknown)
    {   CLI_disp_error("Detected %u Unknown error(s)\n", gErrUnknown);
        CLI_disp_error("ERROR Unknown STATUS: FAILED\n");
    }
    else
    {   CLI_disp("ERROR Unknown STATUS: PASSED\n");
    }


    if(gErrTotal)
    {   CLI_disp_error("Total error = %u error(s)\n", gErrTotal);
        CLI_disp_error("ERROR TOTAL STATUS: FAILED\n");
    }
    else
    {   CLI_disp("ERROR TOTAL STATUS: PASSED\n");
    }

    //Reset error counters
    gErrArgs        = 0;
    gErrNoMemory    = 0;
    gErrGeneral     = 0;
    gErrOmx         = 0;
    gErrUnknown     = 0;
    gErrTotal       = 0;

    return CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
 *  \defgroup historic
 * \ingroup cli_cmd
 *  \b Usage:
 *  \code hist list|print|<id> \endcode
 *  \param
 *    - list : to get historic listed on screen
 *    - print: to have historic printed to a file
 *    - id : to recall cmd identified by id
 *
 * \description
 *   Historic on cli
 */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_historic(int argc ,char ** argv) {
    int histId;
    int i, minlist, maxlist, found;

    minlist = MAX_HISTORIC;
    maxlist = hist_index;
    if (hist_size > MAX_HISTORIC) minlist=hist_index ;

    if(argc == 2) {
        /* Action depends on argument */
	if( !strcmp(argv[1],"off")) {
	    hist_enable = 0;
            CLI_disp_msg( "Historic disabled \n" );   
        }
	else if( !strcmp(argv[1],"on")) {
	    hist_enable = 1;
            CLI_disp_msg( "Historic enabled \n" );   
        }

        else if( !strcmp(argv[1],"list")) {
            //CLI_disp_msg( "Historic list (size=%d)\n", hist_size );   
	    for (i=minlist; i<MAX_HISTORIC; i++) {
                CLI_disp_msg( "\t%3d :\t%s \n", list_historic[i].HId, list_historic[i].cmd );   
	    }
	    for (i=0; i<maxlist; i++) {
                CLI_disp_msg( "\t%3d :\t%s \n", list_historic[i].HId, list_historic[i].cmd );   
	    }
        }
	else if( !strcmp(argv[1],"print")) {
            CLI_disp_warning( "Historic print to file _not yet implemented_\n" );   
        }

	/* Replay command */
        else {
	    histId = CLI_atoi(argv[1]);
	    found = 0;
	    for (i=0; i<MAX_HISTORIC; i++) {
		if (list_historic[i].HId == histId) {
		    found = 1;
		    break;
		}
	    }
            if ((found==1) && (list_historic[i].cmd != NULL)) {
        	t_cmd_function cmd ;

		CLI_disp(">");
		CLI_disp(list_historic[i].cmd);
		CLI_disp("\n");
    		hist_index--; hist_size--;
            	CLI_addCommand(list_historic[i].cmd);
        	cmd = CLI_getNextFunction();
                CLI_do_command(cmd.func, cmd.argc, cmd.argv);
            	CLI_free(cmd.argv);
            }
	    else 
              CLI_disp_warning("Unknown historic id=%d \n", histId);
        }
    }
    else {
        CLI_disp_usage( C_cli_historic );   
    }
    return  CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
  \defgroup C_cli_enable_skip_unknown_command C_cli_enable_skip_unknown_command
  \ingroup cli_cmd
  \b Usage:
  \code C_cli_enable_skip_unknown_command \endcode

  this function enables skipping of unknown command. Useful in if..else condition
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_enable_skip_unknown_command(int argc, char **argv){
    CLI_enable_skip_unknown_command = TRUE;
    return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup C_cli_disable_disp_unknown_command C_cli_disable_disp_unknown_command
  \ingroup cli_cmd
  \b Usage:
  \code C_cli_disable_disp_unknown_command \endcode

  this function disables skipping of unknown command. Useful in if..else condition
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_disable_skip_unknown_command(int argc, char **argv){
    CLI_enable_skip_unknown_command = FALSE;
    return CMD_COMPLETE;
}


extern t_alias *static_alias_tab[];
/* ----------------------------------------------------------------- */
/**
  \defgroup eixstalias
  \ingroup cli_cmd
  \b Usage:
  \code exist_alias <name> <result> \endcode
  \param name Name of the alias to check

  This function put in result if alias exist or not \ref alias
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_existalias(int argc, char **argv)
{
	int result;
	char cRes[2];
	char *pVar;
	char *pName;
	if (argc==1)
	{ //If no parameter display the whole list
		int index=0;
		while(index < alias_cur_size)
		{
			if (list_alias[index].name==NULL )
				break;
			CLI_disp_msg("\n%03d : %10s  -> %s", index , list_alias[index].name, list_alias[index].value);
			index++;
		}
		CLI_disp_msg("\n");
#if 0 // Don't print static alias
		int i,j;
		for(i = 0; (i < STATIC_ALIAS_TABS_MAX_NB) && (static_alias_tab[i]); i++) 
		{
			j=0;
			while(strcmp(static_alias_tab[i][j].name, "LAST_ALIAS"))
			{
				CLI_disp_msg("static[%03d, %03d] : %s  -> %s", i, j , static_alias_tab[i][j].name, static_alias_tab[i][j].value);
					j++;
			}
		}
#endif
	}
	if(argc != 3)
		return CMD_COMPLETE;
	pName=argv[1];
	if (*pName=='&')
	{
		++pName;
	}
	result=CLI_exist_alias(pName);
	cRes[0]='0'+result;
	cRes[1]='\0';
	pVar=argv[2];
	if (*pVar=='&')
	{
		++pVar;
	}
	CLI_add_alias(pVar, cRes, 0);
	return CMD_COMPLETE;
}


//********************************************************************************
//********************************************************************************
//********************************************************************************

#if 0
#include <stdarg.h>
FILE *EnsTraceRecord_fp=NULL;

int EnsTraceRecord(const char *fmt, ...)
{
	if (EnsTraceRecord_fp)
	{
		va_list list;
		va_start(list, fmt);
		vfprintf(EnsTraceRecord_fp, fmt, list);
		va_end(list);
	}
	return(0);

}

/* ----------------------------------------------------------------- */
/**
  \defgroup EnableLogTrace
  \ingroup cli_cmd
  \b Usage:
  \code EnableLogTrace <filename>\endcode
  \param name Name of the alias to delete

  This function open a file a record ENS IL trace \ref alias
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_cli_EnableLogTrace(int argc, char **argv)
{
	if (argc==2)
	{ 
		ENS_Core *pEns_core = ENS_Core::getInstance();
		if (pEns_core==NULL)
		{
		}
		FILE *fp;
		fp=fopen(argv[1], "w");
		if (fp==NULL)
		{
		}
		if (fp!=NULL)
		{
			pEns_core->mTraceLevel=2;
			pEns_core->mLogFunction=EnsTraceRecord;
		}
		else
		{
			pEns_core->mTraceLevel=0;
			pEns_core->mLogFunction=NULL;
			if (EnsTraceRecord_fp!=NULL)
			{
				fclose(EnsTraceRecord_fp);
				EnsTraceRecord_fp=NULL.
			}

		}
	}
	return CMD_COMPLETE;
}


#endif

