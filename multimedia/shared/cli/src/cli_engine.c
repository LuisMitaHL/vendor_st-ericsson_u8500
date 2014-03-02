/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     cli_engine.c
 * \brief    Command Line Interface engine
 * \author   ST-Ericsson
 */
/*****************************************************************************/
/*-----------------------------------------*/
/* INCLUDES                                */
/*-----------------------------------------*/

#ifdef __ARM_SYMBIAN
#include "cli_symbian.h"
#endif
#include "cli_p.h"

// for traces (!!!!! NOT SUPPORTED YET FOR SYMBIAN !!!!!)
#ifndef __ARM_SYMBIAN
#define  OMXCOMPONENT "MMTE_CLI" 
#undef   OMX_TRACE_UID
#define  OMX_TRACE_UID 0x20
#ifdef PLATFORME_NAME_UX600
//#include "osi_trace.h"
#else
#include "osi_trace.h"
#endif
#include "los/api/los_api.h"
#endif

//#define CLI_TRACE_MALLOC

void CLI_read_script(char *buffer);
t_uint64 CLI_atol_msg(char* name, unsigned char display_message);

/*-----------------------------------------*/
/* Global variables                        */
/*-----------------------------------------*/
#ifndef __ARM_SYMBIAN
static int skip_state = 0 ;
#endif

static CmdPtr restart_cmd_func = NULL;
int CLI_displayAfterAliasReplacement = 0;

EXPORT_C void CLI_SkipOnError_Until(CmdPtr func){
    restart_cmd_func = func;
}

extern int ena_display_msg;

// Condition for skipping unknown command in ..else condition.
t_bool CLI_enable_skip_unknown_command = FALSE;

EXPORT_C char* CLI_strdup(char* str)
{
    char* string;
    if (!str) return NULL;
    string = CLI_malloc(strlen(str) + 1);
    if (!string) return NULL;
    strcpy(string, str);
    return string;
}

int  force_eof=0;
unsigned char   current_input = CONSOLE_INPUT;
int    crtlc;
int    myexit;
char  *pscript;
int    script_idx = 0;
int    fileidx = 0;
int script_nb_argument[MAX_NESTED_SCRIPT];
static char *script_name[MAX_NESTED_SCRIPT];    // Name of script is saved
static char *script_start[MAX_NESTED_SCRIPT];   // Handle of script is saved to free memory on completion.
static char *script_loc[MAX_NESTED_SCRIPT];     // pointer to current line in script.

static t_malloc_table mallocTable[MALLOC_TABLE_SIZE];

//static t_los_file *filetab[MAX_NESTED_FILE];

char aliastab[MAX_NESTED_SCRIPT][MAX_ATE_CMD_ARGS][MAX_LINE];

t_itf itflist[MAX_ITFS];
int   nbitfs = 0;

t_alias  *list_alias     = NULL;
int      alias_max_size  = 0;
int      alias_cur_size  = 0;

t_alias *static_alias_tab[]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

t_historic  list_historic[MAX_HISTORIC];
int hist_enable = 0;	//default should be to disable
int hist_size = 0;
int hist_index = 0;

// Global Variables used for error management
int      gErrArgs       = 0;
int      gErrNoMemory   = 0;
int      gErrGeneral    = 0;
int      gErrOmx        = 0;
int      gErrUnknown    = 0;
int      gErrTotal      = 0;

#ifdef __ARM_SYMBIAN
static TCliSymbianInitParams   gCliSymbianInitParams;
#endif


// Global varibale used for displaying command on terminal
t_bool bPushedCommandPrinting = 1;


/** ------------------------------------------------------------------
 * function CLI_getpscript
 * \param   void
 * \return  char *
 *     Returns the pscript pointer.
 * Description:
 *    Returns the pscript pointer.
 * -----------------------------------------------------------------*/

char * CLI_getpscript(void)
 {
   return pscript;
   
 }/* char * CLI_getpscript(void) */


/** ------------------------------------------------------------------
 * function CLI_setpscript
 * \param   
 *    - IN char * pscript
 *      - New value of the pscript pointer.
 *    
 * \return  void
 *
 * Description:
 *    Set the pscript pointer.
 * -----------------------------------------------------------------*/

  void CLI_setpscript(char * arg_pscript)
  {
    pscript = arg_pscript;
  }/*   void CLI_setpscript(char * arg_pscript) */

/** ------------------------------------------------------------------
 * function CLI_atol_msg
 * \param   name string to convert
 * \return  name converted into long
 * Description:
 *   convert string into long.
 *   String should looks like : -?[0-9]+ or 0[xX][0-9a-fA-F]+
 *   in other cases it return 0 and a warning is printed
 * -----------------------------------------------------------------*/
#define DEC_MODE 10
#define HEX_MODE 16
#define MINUS_DEC_MODE 10

t_uint64 CLI_atol_msg(char* name, unsigned char display_message)
{
    t_uint64 value = 0;
    t_uint64 dec = 1;
    int mode = DEC_MODE;
    int minus = 0;
    int index  = 0;
    int offset = 0;
    char c;

    /* find string size and check contents */
    if((!strncmp(name,"0x",2)) ||
       (!strncmp(name,"0X",2)))
    {
        offset = 2;
        mode = HEX_MODE;
    }
    else if(!strncmp(name,"-",1))
    {
        offset = 1;
        minus = 1;
    }

    index = offset;
    if(mode == DEC_MODE)
    {
        while(name[index] != '\0')
        {
            c = name[index];
            if(c<'0' || c>'9')
            {
			  if(display_message) CLI_disp_warning("Could not convert index %d to an int\n",name);
			  return 0;
            }
            index ++;
        }
    }
    else
    {
        while(name[index] != '\0')
        {
            c = name[index];
            if((c<'0' || c>'9') &&
               (c<'a' || c>'f') &&
               (c<'A' || c>'F'))
            {
			  if(display_message) CLI_disp_warning("Could not convert index %d to an int\n",name);
			  return 0;
            }
            index ++;
        }
    }
    index --;

    for( ; index>=offset; index--)
    {
        c = name[index];
        if((c >= 'a' && c <= 'f'))
            value += (c - 'a' + 10) * dec;
        else if ((c >= 'A' && c <= 'F'))
            value += (c - 'A' + 10) * dec;
        else value += (c - '0') * dec;

        dec = dec * mode;
    }

    if(minus)
        return -value;
    else
        return value;
}


/** ------------------------------------------------------------------
 * function CLI_atol
 * \param   name string to convert
 * \return  name converted into long
 * Description:
 *   convert string into long.
 *   String should looks like : -?[0-9]+ or 0[xX][0-9a-fA-F]+
 *   in other cases it return 0 and a warning is printed
 * -----------------------------------------------------------------*/
EXPORT_C t_uint64 CLI_atol(char* name)
{
	return CLI_atol_msg(name, 1);
}

/** ------------------------------------------------------------------
 * function CLI_atoi
 * \param   name string to convert
 * \return  name converted into int
 * Description:
 *   convert string into int.
 *   String should looks like : -?[0-9]+ or 0[xX][0-9a-fA-F]+
 *   in other cases it return 0 and a warning is printed
 * -----------------------------------------------------------------*/
#define DEC_MODE 10
#define HEX_MODE 16
#define MINUS_DEC_MODE 10

EXPORT_C unsigned int CLI_atoi(char* name)
{
    return((unsigned int)CLI_atol_msg(name,1));
}


/** ------------------------------------------------------------------
 * function CLI_atoi_without_msg
 * \param   name string to convert
 * \return  name converted into int
 * Description:
 * same as CLI_atoi but does not display warnings
 * -----------------------------------------------------------------*/
EXPORT_C unsigned int CLI_atoi_without_msg(char* name)
{
    return((unsigned int)CLI_atol_msg(name,0));
}

/** ------------------------------------------------------------------
 * function : CLI_change_slash_to_backslash
 * \param   :
 * \return  :
 * Description: Does what it says   
 * -----------------------------------------------------------------*/
void CLI_change_slash_to_backslash(char *string)
{
    while(*string != '\0')
    {
        if (*string == '\\')
            *string = '/';
    
        string++;
    }
}


/** ------------------------------------------------------------------
 * function : CLI_set_parameter64
 * \param   : Name string to set and previous value 
 * \return  : Name string converted into unsigned long
 * Description:
 *    if the value is of the form +(offset) or -(offset) add/remove
 * offset form previous_value. In other case simply call CLI_atol
 * -----------------------------------------------------------------*/
EXPORT_C t_uint64 CLI_set_parameter64(char *value, t_uint64 previous_value)
{
    char    *end_value;
    t_uint64 tmp_value;
    t_uint64 result;

    if(!strncmp(value,"+(",2) ||
       !strncmp(value,"-(",2) ||
       !strncmp(value,"*(",2) ||
       !strncmp(value,"/(",2) )
    {
        end_value = strstr(value,")");
        if(end_value == NULL)
        {
            CLI_disp_error("Syntax error in parameter\n");
        }
        else
        {
            *end_value = '\0';
        }
        tmp_value = CLI_atoi(&value[2]);

        switch(value[0])
        {
            case '+': result = (unsigned long)(previous_value + tmp_value); break;
            case '-': result = (unsigned long)(previous_value - tmp_value); break;
            case '*': result = (unsigned long)(previous_value * tmp_value); break;
            case '/': result = (unsigned long)(previous_value / tmp_value); break;
            default:  result = 0; break;
        }
    }
    else
    {
        result = CLI_atol_msg(value,1);
    }

    return result;
}


/** ------------------------------------------------------------------
 * function : CLI_set_parameter
 * \param   : Name string to set and previous value 
 * \return  : Name string converted into unsigned int
 * Description:
 *    if the value is of the form +(offset) or -(offset) add/remove
 * offset form previous_value. In other case simply call CLI_atoi
 * -----------------------------------------------------------------*/
EXPORT_C unsigned int CLI_set_parameter(char *value, unsigned int previous_value)
{
    return ( (unsigned int)CLI_set_parameter64(value,(unsigned int)previous_value) );
}




/** ------------------------------------------------------------------
 * function : CLI_lltostr
 * \param   :
 * \return  :
 * Description:
 *   convert unsigned int into string
 * -----------------------------------------------------------------*/
#define LLTOSTR_MAX_VALUE 18
static char lltostr_tab[LLTOSTR_MAX_VALUE];
EXPORT_C char *CLI_lltostr(unsigned int value)
{
    long local_value = value;
    int flag_neg = 0;
    int index = LLTOSTR_MAX_VALUE - 1;

    if(local_value < 0)
    {
        flag_neg = 1;
        local_value = -local_value;
    }

    lltostr_tab[index] = '\0';
    index--;

    while(local_value >= 10)
    {
        /* 48 is the ascii value of character 0 */
        lltostr_tab[index] = (char)(48+(local_value % 10));
        local_value = local_value/10;
        index--;
    }

    lltostr_tab[index] = (char)(48+(local_value % 10));

    if(flag_neg)
    {
        index--;
        lltostr_tab[index] = '-';
    }

    return &lltostr_tab[index];
}


/** ------------------------------------------------------------------
 * function : CLI_malloc
 * \param   :
 * \return  :
 * Description: invokes LOS_Alloc and maintain a table of corresponding
 *              buffid and ptr. 
 * -----------------------------------------------------------------*/
#ifndef __ARM_SYMBIAN
static t_los_mutex_id cli_malloc_mutex;
#endif // __ARM_SYMBIAN

EXPORT_C void* CLI_malloc(int size)
{
	int i=0;

#ifndef __ARM_SYMBIAN
    LOS_MutexLock(cli_malloc_mutex);
#endif // __ARM_SYMBIAN

	// search table for first free slot
	while ((mallocTable[i].ptr != NULL) && (i < MALLOC_TABLE_SIZE))
	{	i++;
	}
	
	if (i < MALLOC_TABLE_SIZE)
	{
		mallocTable[i].buffid  = LOS_Alloc(size, 4, LOS_MAPPING_DEFAULT);
		if (0 == mallocTable[i].buffid)
			{
			CLI_disp_error("Not enough memory to allocate in CLI_malloc\n");
			return NULL;	
			}

        mallocTable[i].ptr = (char *)LOS_GetLogicalAddress(mallocTable[i].buffid);
		//CLI_disp_msg("\t CLI allocation asked : %d bytes, ptr = 0x%x \n", size, mallocTable[i].ptr);
#ifdef DEBUG_MALLOC
        CLI_disp_error("Allocate(%d) buffer %x with size == %d\n",i,mallocTable[i].ptr,size);
#endif

#ifdef CLI_TRACE_MALLOC
		LOS_Log("CLI_trace Allocate(%d/%d) buffer %x with size %d\n",i,MALLOC_TABLE_SIZE, mallocTable[i].ptr,size);
		CLI_Check_MallocTable();
#endif
#ifndef __ARM_SYMBIAN
        LOS_MutexUnlock(cli_malloc_mutex);
#endif // __ARM_SYMBIAN
		return mallocTable[i].ptr;
	}

#ifdef CLI_TRACE_MALLOC
	LOS_Log("CLI_trace Allocate error\n");
#endif
    CLI_disp_error("Malloc Table is full! You should redefine MALLOC_TABLE_SIZE in cli_p.h\n");
#ifndef __ARM_SYMBIAN
    LOS_MutexUnlock(cli_malloc_mutex);
#endif // __ARM_SYMBIAN
    return NULL;
}


/** ------------------------------------------------------------------
 * function : CLI_free
 * \param   :
 * \return  :
 * Description: invokes LOS_Free based on id and ptr in mallocTable  
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_free(void *freeMe)
{
	int i=0;
	
#ifndef __ARM_SYMBIAN
    LOS_MutexLock(cli_malloc_mutex);
#endif // __ARM_SYMBIAN
	// find index of freeMe
	while ((mallocTable[i].ptr != freeMe) && (i < MALLOC_TABLE_SIZE))
	{	i++;
	}
	
	if (i < MALLOC_TABLE_SIZE)
	{
#ifdef DEBUG_MALLOC
        CLI_disp_error("Free buffer %x\n",mallocTable[i].ptr);
#endif
#ifdef CLI_TRACE_MALLOC
		LOS_Log("CLI_trace Free(%d/%d) buffer %x\n",i,MALLOC_TABLE_SIZE, mallocTable[i].ptr);
		CLI_Check_MallocTable();
#endif
		LOS_Free(mallocTable[i].buffid);
		mallocTable[i].ptr = NULL;
    }
	else
	{
#ifdef CLI_TRACE_MALLOC
		LOS_Log("CLI_trace Free error Could not find the LOS_Alloc id for this pointer (0x%x)!\n", freeMe);
#endif
		CLI_disp_error("Could not find the LOS_Alloc id for this pointer (0x%x)!\n", freeMe);
	}
   
#ifndef __ARM_SYMBIAN
    LOS_MutexUnlock(cli_malloc_mutex);
#endif // __ARM_SYMBIAN

}

void CLI_Check_InitMallocTable(void)
{
    int i=0;
	while (i < MALLOC_TABLE_SIZE)
	{	
        mallocTable[i].ptr = NULL;
        i++;
	}

#ifndef __ARM_SYMBIAN
    cli_malloc_mutex = LOS_MutexCreate();
#endif // __ARM_SYMBIAN
}

void CLI_Check_MallocTable(void)
{
	int i=0;
	int counter = 0;
    

	while (i < MALLOC_TABLE_SIZE)
	{	
        if(mallocTable[i].ptr != NULL){
            
            counter++;
        }
        i++;
	}
#ifdef CLI_TRACE_MALLOC
	LOS_Log("CLI_trace Check_MallocTable counter = %d/%d\n", counter, MALLOC_TABLE_SIZE);
#else
    CLI_disp_error("Malloc Table is not empty counter = %d fields fulled\n",counter);
#endif
}


void CLI_Free_MallocTable(void)
{
	int i=0;
	
	while (i < MALLOC_TABLE_SIZE)
    {
        if(mallocTable[i].ptr)
        {
            LOS_Free(mallocTable[i].buffid);
            mallocTable[i].ptr = NULL;
        }
        i++;
    }

#ifndef __ARM_SYMBIAN
    LOS_MutexDestroy(cli_malloc_mutex);
#endif // __ARM_SYMBIAN
}


/** ------------------------------------------------------------------
 * function : CLI_find_static_alias
 * \param   : Alias name
 * \return  : Pointer to alias description structure
 * Description:
 *
 * -----------------------------------------------------------------*/
t_alias* CLI_find_static_alias(char* name)
{
    int i,j;
    for(i = 0; (i < STATIC_ALIAS_TABS_MAX_NB) && (static_alias_tab[i]); i++) 
    {
        j=0;
        while(strcmp(static_alias_tab[i][j].name, "LAST_ALIAS"))
        {
            if (!strcmp(static_alias_tab[i][j].name, name))
                return &static_alias_tab[i][j];
            else
                j++;
        }
    }
    return NULL;
}


/** ------------------------------------------------------------------
 * function : CLI_find_alias
 * \param   : Alias name
 * \return  : Pointer to alias description structure
 * Description:
 *
 * -----------------------------------------------------------------*/
EXPORT_C t_alias* CLI_find_alias(char* name)
{  
    int i;
    for(i = 0; i < alias_cur_size; i++) 
    {
        if (list_alias[i].name && name) 
        {
            if (!strcmp(list_alias[i].name, name))
                return &list_alias[i];
        }
    }
    return CLI_find_static_alias (name);
}


/** ------------------------------------------------------------------
 * function : CLI_find_free_index
 * \param   : none
 * \return  : alias index
 * Description:
 *
 * -----------------------------------------------------------------*/
int CLI_find_free_index()
{
    int i;
    for(i = 0; i < alias_cur_size; i++)
    {   if (list_alias[i].name == NULL)
            break;
    }
    return i;
}


int isValueInHexa(char* value, char **decimalString)
{
	if( ((value[0] == '0') && (value[1] == 'x')) ||
		((value[0] == '0') && (value[1] == 'X')) )
	{
		int decVal = 0;
		*decimalString = CLI_malloc(64);
		sscanf((char*)(value+2),"%x",&decVal);
		//printf("decVal = %d\n", decVal);
		sprintf(*decimalString, "%d", decVal);
		//printf("*decimalString = %s\n", *decimalString);
		return 1;
	}
	else
	{
		//printf("isvalueInHexa => NO\n");
		*decimalString = (char*)NULL;
		return 0;
	}
}

/** ------------------------------------------------------------------
 * function : CLI_add_alias
 * \param   :
 * \return  : none
 * Description:
 *
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_add_alias(char* name, char* value, unsigned char persistance)
{
    t_alias        *al, *al2;
    int i, index;
    
   // check if the alias is defined in static aliases structure
   if(CLI_find_static_alias(name))
   {
       CLI_disp_error("%s is an existing static alias\n", name);
       return;
   }

    
    al = CLI_find_alias(name);
    if (al)
    {
        al2 = CLI_find_alias(value);
        if(al2){
			if(0 != strcmp (al->value, al2->value)){
				CLI_free(al->value);
				al->value = CLI_strdup(al2->value);
			}
        }
        else
		{
			int isValHexa = 0;
			char *decimalString = NULL;
			isValHexa = isValueInHexa(value, &decimalString);

			if(0 == isValHexa)
			{
				if(0 != strcmp (al->value, value))
				{
					CLI_free(al->value);
					al->value = CLI_strdup(value);
				}
			}
			else
			{
				CLI_free(al->value);
				al->value = decimalString;
			}
        }
        
        al->persistance= persistance;
    }
    else
    {
        index = CLI_find_free_index();
        if (index == alias_max_size)
        {
            t_alias* list;
            alias_max_size *= 2;
            if (alias_max_size < 16)
                alias_max_size = 16;
            list = CLI_malloc(sizeof(t_alias) * alias_max_size);
            if(list == NULL)
            {
                CLI_disp_error("Too Many Alias defined\n");
                return;
            }
            if (list_alias)
            {
                /* recopy only the pointer not the name and value */
                memcpy(list, list_alias, sizeof(t_alias) * alias_cur_size);
                CLI_free(list_alias);
            }
            list_alias = list;
            /* init */
            for(i=index; i<alias_max_size; i++)
            {
                list_alias[i].name=NULL;
            }
        }

        list_alias[index].name = CLI_strdup(name);
        
        al = CLI_find_alias(value);
        if(al){
            list_alias[index].value = CLI_strdup(al->value);
        }
        else
		{
			int isValHexa = 0;
			char *decimalString = NULL;
			isValHexa = isValueInHexa(value, &decimalString);
			if(0 == isValHexa)
			{
				list_alias[index].value = CLI_strdup(value);
			}
			else
			{
				list_alias[index].value = decimalString;
			}
        }
        
        list_alias[index].persistance = persistance;
        
        if(index == alias_cur_size){
            alias_cur_size++;
        }
    }
}


/** ------------------------------------------------------------------
 * function : CLI_modify_alias
 * \param   :
 * \return  : none
 * Description:
 *
 * -----------------------------------------------------------------*/
void CLI_modify_alias(char* name, char* value)
{
    t_alias *al = NULL;
	char aliasName[MAX_PARAM_LENGTH];
	char *tmp = NULL;
	char operatorToUse = 0;
    
   // check if the alias is defined in static aliases structure
   if(CLI_find_static_alias(name))
   {
       CLI_disp_error("%s is an existing static alias\n", name);
       return;
   }
   
   memcpy(aliasName, name, strlen(name)+1);
   tmp = aliasName;
   tmp += strlen(name) - 1;
   while( !(  ((*tmp) == '+') || ((*tmp) == '-') || ((*tmp) == '*') || ((*tmp) == '/') || ((*tmp) == '&')
	   || ((*tmp) == '>') || ((*tmp) == '<') || ((*tmp) == '^') || ((*tmp) == '%') || ((*tmp) == '!')) )
   {
		tmp = tmp - 1;
		if(tmp == aliasName)
		{
			CLI_disp_warning("unknown operation found in %s\n", name);
			return;
		}
   }
   operatorToUse = *tmp;
   *tmp = 0;  
    
    al = CLI_find_alias(aliasName);
    if (al)
    {
		char *newVal = NULL;
		int val = CLI_atoi(al->value);
		//CLI_disp_msg("previsous value of alias %s was %d\n", name, val);
		CLI_free(al->value);
		if(operatorToUse == '+')
			val += CLI_atoi(value);
		else if(operatorToUse == '-')
			val -= CLI_atoi(value);
		else if(operatorToUse == '*')
			val = val * CLI_atoi(value);
		else if(operatorToUse == '/')
			val = val / CLI_atoi(value);
		else if(operatorToUse == '&')
			val = val & CLI_atoi(value);
		else if(operatorToUse == '>')
			val = val >> CLI_atoi(value);
		else if(operatorToUse == '<')
			val = val << CLI_atoi(value);
		else if(operatorToUse == '^')
			val = val ^ CLI_atoi(value);
		else if(operatorToUse == '%')
			val = val % CLI_atoi(value);
		else if(operatorToUse == '!')
			val = val | CLI_atoi(value);
		else
			CLI_disp_warning("unknown operation to apply\n");
		newVal = CLI_lltostr(val);
		al->value = CLI_strdup(newVal);
		CLI_disp_msg("new value of %s is %s\n", aliasName, al->value);
    }
    else
    {
        CLI_disp_error("%s is an non existing alias\n", aliasName);
    }
}

/** ------------------------------------------------------------------
 * function : CLI_free_dynamic_aliases
 * \param   : none
 * \return  : none
 * Description:
 *
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_free_dynamic_aliases(void)
{
    int index;
    for(index = 0; index < alias_max_size; index++){
        if ((list_alias[index].name != NULL)&(!list_alias[index].persistance)){
            CLI_free (list_alias[index].name);
            CLI_free (list_alias[index].value);
            list_alias[index].name=NULL;
            list_alias[index].value=NULL;
        }
    }    
}

/** ------------------------------------------------------------------
 * function : register_interface
 * \param   :
 * \return  : none
 * Description:
 *   Register a group of functions in the monitor
 * -----------------------------------------------------------------*/
EXPORT_C int CLI_register_interface(char* group, int nb_funcs, t_cmd_list* list, int enabled)
{
    int i;

    if (nbitfs == MAX_ITFS)
        return 0;
        
    for(i = 0; i < nbitfs; i++)
    {
        if (!strcmp(group, itflist[i].group))
            return 0;
    }
    itflist[i].group = group;
    itflist[i].nb_funcs = nb_funcs;
    itflist[i].list = list;
    itflist[i].enabled = enabled;
    nbitfs++;
    
    return 1;
}

/** ------------------------------------------------------------------
 * function : CLI_set_interface
 * \param   :
 * \return  :
 * Description:
 *   Enable/Disable a group
 * -----------------------------------------------------------------*/
int CLI_set_interface(char* group, int enable)
{
    int i;
    
    for(i = 0; i < nbitfs; i++)
    {
        if (!strcmp(group, itflist[i].group))
        {
            itflist[i].enabled = enable;
            return 1;
        }
    }
    return 0;
}


/** --------------------------------------------------------------------------------
 * function   : register_static_aliases
 * \param     : table of t_static_alias. must end with an alias named "LAST_ALIAS"
 * \return    :  
 * Description: stores the address of the input table in a global table of pointers
 *
 * --------------------------------------------------------------------------------*/
EXPORT_C void CLI_register_static_aliases(t_alias tab[])
{
    int i, pos=0;
      
    //find a free position in the static_alias_tab 
    for (i=0;(i<STATIC_ALIAS_TABS_MAX_NB) && (static_alias_tab[i]);i++){
        pos++;
    }

    if (pos < STATIC_ALIAS_TABS_MAX_NB){
        static_alias_tab[pos] = tab;
    }
    else
    {
        CLI_disp_error("static_alias_tab is full\n");
        return;
    }
    
}


/** ------------------------------------------------------------------
 * function : CLI_io_write_channel
 * \param   : console output
 * \return  : none
 * Description:
 *   Write console or UART
 * -----------------------------------------------------------------*/
void CLI_io_write_channel( int channel, char *format, ...)
{
    va_list ap;
    char outBuffer[MAX_PARAM_LENGTH];
    
    va_start(ap, format);
    vsprintf(outBuffer, format, ap);
    CLI_write_console(channel, outBuffer);
    va_end(ap);
}


/** ------------------------------------------------------------------
 * function : CLI_io_write
 * \param   : message to display
 * \return  : none
 * Description:
 *   Write console or UART
 * -----------------------------------------------------------------*/
void CLI_io_write( char *format, ...)
{
    va_list ap;
    
    va_start(ap, format);
    CLI_io_write_channel(MSG_DISPLAY, format, ap);
    va_end(ap);
}


/** ------------------------------------------------------------------
 * function : CLI_write_console
 * \param   : message to display
 * \return  : none
 * Description:
 *   Write to console output using JTAG link
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_write_console(int channel, char *l)
{
#ifdef __ARM_SYMBIAN
    CLI_write_console_symbian(channel, l);
#else
    if ((channel == ERROR_DISPLAY) || (channel == ERROR_IT_DISPLAY)) { 
	fprintf(stderr, l); fflush(stderr);
    } else {   
	fprintf(stdout, l); fflush(stdout);
    }
#endif
}



/** ------------------------------------------------------------------
 * function : CLI_addCommand
 * \param   : message to display
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/
static char command_from_cli [MAX_LINE];

EXPORT_C void CLI_addCommand(char * new_command)
{
     CLI_addHistoric (new_command);
     strcpy(command_from_cli, new_command);
}


/** ------------------------------------------------------------------
 * function : CLI_pushCommand
 * \param   : command to enqueue in the commandbuffer
 * \return  : none
 * Description: push the command place in parameter into a "command" buffer
 *              the command is placed at the end of the queue. (FIFO scheme)
 *   
 * -----------------------------------------------------------------*/
static char command_from_push [MAX_PUSH_CMD][MAX_LINE];
static int index_buffer_push =-1;

EXPORT_C t_bool CLI_pushCommand(char * new_command)
{
     if( (index_buffer_push < MAX_PUSH_CMD) 
             && (new_command != NULL)
             && strlen(new_command) ){
         index_buffer_push++;
         strcpy(command_from_push[index_buffer_push], new_command);
         return 1;
     }
     
     return 0;
     
}


/** ------------------------------------------------------------------
 * function : CLI_pullCommand
 * \param   : command to get back from the commandbuffer
 * \return  : 1st element of the buffer : a command
 * Description: extract the 1st element of the "command" buffer and update it
 *   
 * -----------------------------------------------------------------*/

EXPORT_C char* CLI_pullCommand(void)
{
    if(index_buffer_push > -1)
    {
        int i=0;
        int size = strlen(command_from_push[0]) + 1;
        char * temp = (char *)CLI_malloc(size*sizeof(char));
		if (NULL == temp)
		{	
			CLI_disp_error( "Not enough memory in CLI_pullCommand" ) ;
			return NULL ;
		}

        strcpy(temp, command_from_push[0]);
        
        
        for(i =0;i< MAX_PUSH_CMD;i++) // update the buffer by reforming the list
        {
          if(i<index_buffer_push)
              strcpy(command_from_push[i], command_from_push[i+1]);
          else
              strcpy(command_from_push[i], "");
    
        }
        index_buffer_push--;
        
        return temp;
    }
    else
    {
        return '\0';
    }
    
}

/** ------------------------------------------------------------------
 * function : CLI_PrintCommandPushed
 * \param   : command to allow or not theprint of the command pulled from the command_from_push buffer
 * \return  : none
 * Description: Set a flag to enable/disable the print of trace
 *   
 * -----------------------------------------------------------------*/


EXPORT_C t_bool CLI_PrintCommandPushed(t_bool bState)
{
    bPushedCommandPrinting = bState;
    return  bPushedCommandPrinting;
}

/** ------------------------------------------------------------------
 * function : CLI_getNextFunction
 * \param   : message to display
 * \return  : none
 * Description:
 *   
 * -----------------------------------------------------------------*/
EXPORT_C t_cmd_function CLI_getNextFunction(void)
{
    t_cmd_function ret = {0,0,0};
    static char tmp [MAX_LINE];
    //IN0("");

    tmp[0] = '\0';

    if(index_buffer_push >-1)        // to run in priority command(s) place in a buffer (via dispatchState)
    {
        char* ptrPullCommand = CLI_pullCommand();
        strcpy(tmp, ptrPullCommand);
        CLI_free(ptrPullCommand);
        
        if(bPushedCommandPrinting == 1)
        {
            //CLI_disp_msg("Command pulled from buffer and executed : \n");
        CLI_disp_msg(tmp);
        CLI_disp_msg("\n");
        }
     }

    else if((command_from_cli[0] != '\0')&&((tmp[0] == '\0') || (tmp[0] == '#')))
    {
        strcpy(tmp, command_from_cli);
        command_from_cli[0] = '\0';
    } 
    
    
    else if(current_input == SCRIPT_INPUT)
    {
        CLI_read_script(tmp);
    }
    
    

    if((tmp[0] != '\0') && (tmp[0] != '#'))
    {
	#ifndef __ARM_SYMBIAN
    //printf("%s\n", tmp);
    #endif
    
        
        ret = CLI_inter(tmp);
    }

    //OUT0("");
    return ret;
}


/** ------------------------------------------------------------------
 * function : CLI_read_console
 * \param   : command typed in the console + size of string
 * \return  : none
 * Description:
 *   Read command line from the console input using the JTAG link
 * -----------------------------------------------------------------*/
int CLI_read_console(char *buffer, int stringSize)
{
    unsigned char c = ' ';
    int  cnt = 0;
	int isEOF = 0;

   char c_0=0,c_1=0,c_2=0,c_3=0;
   int nb_hist=0;

    while ((c!='\n') && (cnt<MAX_MONITOR_LINE_LENGTH))
    {
        c = fgetc(stdin);       // get char from console

        switch(c)
        {   
			case 255 : isEOF = 1 ; break;
			case '\n' : /* do nothing */    break;  // LineFeed = 0x0A
            case '\r' :
#ifdef ATE_USB                                      // Change <CR> to <LF> when USB console
                    c = '\n';
#endif
                break;
            case '\b' : if (cnt>0)  cnt--;break; // Backspace
            case '\t' :
				if(cnt >= stringSize)
				{
			        CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
			        return 0;
				}
            	buffer[cnt++] = 32;
				break; // Replace Tab by space
			case 27:
				if(cnt >= stringSize)
				{
			        CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
			        return 0;
				}
				//CLI_disp_msg("espace detected ");
				c_0=c;
				c_1=0;
				c_2=0;
				c_3=0;
				buffer[cnt++] = c;
				break;
			case 91:
				if(cnt >= stringSize)
				{
			        CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
			        return 0;
				}
				//CLI_disp_msg("left opening bracket detected ");
				if(c_0==27)
				{
					c_1=c;
					c_2=0;
					c_3=0;
				}
				else
				{
					c_0=0;
					c_1=0;
					c_2=0;
					c_3=0;
				}
				buffer[cnt++] = c;
				break;
			case 65:
				if(cnt >= stringSize)
				{
			        CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
			        return 0;
				}
				//CLI_disp_msg("A detected ");
				if(c_0==27)
				{
					if(c_1==91)
					{
						c_2=c;
						nb_hist++;
					}
					else
					{
						c_0=0;
						c_1=0;
						c_2=0;
						c_3=0;
						buffer[cnt++] = c;
					}
				}
				else
				{
					c_0=0;
					c_1=0;
					c_2=0;
					c_3=0;
					buffer[cnt++] = c;
				}
				break;
			case 66:
				if(cnt >= stringSize)
				{
			        CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
			        return 0;
				}
				//CLI_disp_msg("B detected ");
				if(c_0==27)
				{
					if(c_1==91)
					{
						c_2=c;
						nb_hist--;
					}
					else
					{
						c_0=0;
						c_1=0;
						c_2=0;
						c_3=0;
						buffer[cnt++] = c;
					}
				}
				else
				{
					c_0=0;
					c_1=0;
					c_2=0;
					c_3=0;
					buffer[cnt++] = c;
				}
				break;

			default   :
				if(cnt >= stringSize)
				{
					CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
					return 0;
				}
				buffer[cnt++] = c;
				//CLI_disp_msg("%d is detected ", c);
				break;
        }
		if (isEOF) break;
    }

	if(cnt >= stringSize)
	{
		CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
		return 0;
	}
    buffer[cnt++] = '\0';

#ifndef __ARM_LINUX    
    CLI_io_write("\n");
#endif

    if (cnt >= MAX_MONITOR_LINE_LENGTH)
    {
        cnt = MAX_MONITOR_LINE_LENGTH - 2;
		if(cnt >= stringSize)
		{
			CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
			return 0;
		}
        buffer[cnt++] = '\n';
        CLI_write_console(ERROR_DISPLAY, "Command line is too long!\n");
    }
	else
	{
	 //CLI_disp_msg("exiting fct %s\n", __FUNCTION__);
	}

	return nb_hist;

}


/** ------------------------------------------------------------------
 * function : CLI_read_script
 * \param   : command typed in the console
 * \return  : none
 * Description:
 *   Read command line from the console input using the JTAG link
 * -----------------------------------------------------------------*/
void CLI_read_script(char *buffer)
{
    unsigned char c = ' ';
    int  cnt = 0;
    int  eof = 0;
    t_alias * al;
    
    if(force_eof == 1)
    {
        int i;
        // close current script, and pop the new pscript
        pscript = CLI_script_pop();
        CLI_disp_msg("Current script has been closed\n");

        for (i = 0; i < script_nb_argument[script_idx+1]; i++)
        {
            char name[100];
            sprintf(name, "$%d", i);
            
            
            al = CLI_find_alias(name);
            if(al)
            {   
                CLI_free(al->name);
                CLI_free(al->value);

                al->name = NULL;
                al->value = NULL;
            }
            else
            {   CLI_disp_warning("Unknown alias [%s]\n",name);
            }
        }
        
        for (i = 0; i < script_nb_argument[script_idx]; i++)
        {
            
            char name[100];
            sprintf(name, "$%d", i);
            if(strlen(aliastab[script_idx][i]) != 0){    
                CLI_add_alias(name, aliastab[script_idx][i],1);
            }
        }
        force_eof = 0;
    }

    while ((c!='\n') && (cnt<MAX_MONITOR_LINE_LENGTH))
    {
        c = (char)*pscript++;   // get char from current script

        switch(c)
        {
            case 255  : eof = 1; break;
            case '\0' : eof = 1;             break;  // End of file is reached
            case '\n' : /* do nothing */     break;  // LineFeed = 0x0A
            case '\r' : /* do nothing */     break;  
            case '\t' : buffer[cnt++] = 32;  break;  // Replace Tab by space
            default   : buffer[cnt++] = c;   break;
        }
        if (eof) break;
    }

    buffer[cnt++] = '\0';

    if(bPushedCommandPrinting == 1)
    {
    CLI_disp_debug("evaluate : ");	
    CLI_disp_msg(buffer);
    CLI_disp_msg("\n");
    }

    if (cnt >= MAX_MONITOR_LINE_LENGTH)
    {
        cnt = MAX_MONITOR_LINE_LENGTH - 2;
        buffer[cnt++] = '\n';
        CLI_disp_error("Command line is too long!\n");
    }

    if(eof)
    {
        int i;
        pscript = CLI_script_pop();

        if (pscript == NULL)
        {   current_input = CONSOLE_INPUT;
        }

        for (i = 0; i < script_nb_argument[script_idx+1]; i++)
        {
            char name[100];
            sprintf(name, "$%d", i);
            
            
            al = CLI_find_alias(name);
            if(al)
            {   
                CLI_free(al->name);
                CLI_free(al->value);

                al->name = NULL;
                al->value = NULL;
            }
            else
            {   CLI_disp_warning("Unknown alias [%s]\n",name);
            }
            
            strcpy(aliastab[script_idx+1][i], "");
        }
        
        for (i = 0; i < script_nb_argument[script_idx]; i++)
        {
            
            char name[100];
            sprintf(name, "$%d", i);
            if(strlen(aliastab[script_idx][i]) != 0){    
                CLI_add_alias(name, aliastab[script_idx][i],1);
            }
        }
    }

}


/** ------------------------------------------------------------------
 * function : CLI_script_pop
 * \param   : none
 * \return  : file handler
 * Description:
 *   pop file handler from the stack
 * -----------------------------------------------------------------*/
char * CLI_script_pop(void)
{
    CLI_free(script_start[script_idx]);
    CLI_free(script_name[script_idx]);

    if (script_idx==0)
        return NULL;
    else
        return (script_loc[--script_idx]);

}

/** ------------------------------------------------------------------
 * function : CLI_get_current_script
 * \param   : none
 * \return  : Return a pointer to complete path with test name
 * Description:
 *   Return a pointer to complete path with test name
 * -----------------------------------------------------------------*/
EXPORT_C char* CLI_get_current_script(void)
{
    return script_name[script_idx]; // Return a pointer to complete path with test name
}

/** ------------------------------------------------------------------
 * function : CLI_script_push
 * \param   : none
 * \return  : file handler
 * Description:
 *   push file handler to the stack
 * -----------------------------------------------------------------*/
int CLI_script_push(char * pnew_script, char * pscript_name)
{
    if (script_idx == MAX_NESTED_SCRIPT)
    {
        CLI_disp_error("Too many command files nested!\n");
        return 0;
    }

    if (script_idx > 0){
        script_loc[script_idx] = pscript;
    }
    script_idx++;
    script_nb_argument[script_idx] = 0;
    script_start[script_idx] = pnew_script;
    script_name[script_idx] = CLI_strdup(pscript_name);
    pscript = pnew_script;
    return 0xff;
}


/** ------------------------------------------------------------------
 * function : CLI_domatch
 * \param   : char*, char*
 * \return  : none
 * Description:
 *   command match
 * -----------------------------------------------------------------*/
int CLI_domatch(char *a, char *b)
{
    // VI ID:25452
    return(strcmp(a,b) ? 0:1 );
    //return (strncmp(a, b, strlen(b)) ? 0 : 1);
}


/** ------------------------------------------------------------------
 * function : CLI_command_match
 * \param   : char*
 * \return  : Pointer to the command function
 * Description:
 *   Check if the command match
 * -----------------------------------------------------------------*/
CmdPtr CLI_command_match(char *s)
{
    
    int i, j, itf = -1, idx = -1, match = 0;
    
    if(s == NULL)
        return NULL;
    
	for (j = 0; j < nbitfs; j++)
    {
        if (itflist[j].enabled)
        {
            for (i = 0; i < itflist[j].nb_funcs; i++)
            {
                match += CLI_domatch( itflist[j].list[i].name, s );
                if (idx == -1 && match != 0)
                {
                    idx = i;
                    itf = j;
                }
            }
        }
    }
    
    if (match == 1)
        return itflist[itf].list[idx].command;
    
    if (match == 0 && crtlc != 1)
        {
        if (CLI_enable_skip_unknown_command == FALSE)
            CLI_disp_error("Unknown command: %s\n", s);
        }
        
    
    if (match > 1)
        CLI_disp_warning("Multiple matches for abbreviation: %s\n", s);
    
    return NULL;
}


/** ------------------------------------------------------------------
 * function : CLI_do_command
 * \param   : cmd    = Pointer to command function
 * \param   : largc  = number of arguments to be passed
 * \param   : lcargv = arguments list
 * \return  : command completion/status 
 * Description:
 *   Execute the command
 * -----------------------------------------------------------------*/
EXPORT_C CMD_COMPLETION CLI_do_command(CmdPtr cmd, int lcargc, char *lcargv[])
{
    CMD_COMPLETION ret = CMD_COMPLETE;

    if ( (lcargc != 0) && (cmd != NULL) )
        {
			CLI_disp_debug( "-> in\n" ) ;
            ret = cmd( lcargc, lcargv);
            CLI_process_error(ret);
			CLI_disp_debug( "-< out\n" ) ;			
    }
    return ret;
}


/** ------------------------------------------------------------------
 * function : CLI_extract_token
 * \param   :
 * \return  : none
 * Description:
 *   Extract command fields
 * -----------------------------------------------------------------*/
void CLI_extract_token(char *l, char* cli_expand_alias, int *my_argc, char **my_argv)
{
    int offset = 0;
    int i = 1;
    t_alias* al;
    char *arg_ptr;
    char *alias_start;
    char *alias_end;
    *my_argc = 0;
    my_argv[*my_argc] = strtok(l, " ");
    
    if(my_argv[0] == NULL)  return;
    
    if(CLI_domatch("alias", my_argv[0]) ||
       CLI_domatch("unalias", my_argv[0]))
    {
        i = 2;
    }

    if(CLI_domatch("alias", my_argv[0])   ||
       CLI_domatch("unalias", my_argv[0]) ||
       CLI_domatch("do", my_argv[0])      ||
       CLI_domatch("load_", my_argv[0])   ||
       CLI_domatch("echo", my_argv[0]))
    {
        while (my_argv[*my_argc])
        {
            *my_argc = *my_argc + 1;
            my_argv[*my_argc] = strtok(0, " \t");
        }
    }
    else
    {
        while (my_argv[*my_argc])
        {
            *my_argc = *my_argc + 1;
            my_argv[*my_argc] = strtok(0, " \t|");
        }
    }

    /* test each parameter to see if alias expansion is needed */
    while(i < *my_argc)
    {
        arg_ptr = my_argv[i];
        
        /* test in parameter is a simple alias */
        al = CLI_find_alias(my_argv[i]);
        if (al)
        {
            strcpy(cli_expand_alias + offset, al->value);
            my_argv[i] = cli_expand_alias + offset;
            offset += strlen(al->value) + 1;
            if(offset >= MAX_MONITOR_LINE_LENGTH)
            {
                CLI_disp_error("Not enough memory for command line parsing\nAbort...");
                exit(1);
            }
        }
        else
        {
            /* test syntax param=value */
            alias_start = strstr(my_argv[i],"=");
            if(alias_start)
            {
                alias_start++;
                al = CLI_find_alias(alias_start);
            }
            
            if(al)
            {
                alias_start--;
                *alias_start='\0'; /* overwrite = so that we can call strcpy */
                strcpy(cli_expand_alias + offset, my_argv[i]);
                my_argv[i] = cli_expand_alias + offset;
                offset += strlen(my_argv[i]);
                
                *(cli_expand_alias + offset)='='; /* reset the = previously overwritten */
                offset++;
                
                strcpy(cli_expand_alias + offset, al->value);
                offset += strlen(al->value) + 1;
                if(offset >= MAX_MONITOR_LINE_LENGTH)
                {
                    CLI_disp_error("Not enough memory for command line parsing\nAbort...");
                    exit(1);
                }
            }
            else
            {
                /* test if parameter is composed on one or many alias */
                alias_start = strstr(my_argv[i],"$");
                if(alias_start) my_argv[i] = cli_expand_alias + offset;
                
                while(alias_start)
                {
                    alias_end = alias_start;
                    
                    /* recopy non-alias part */
                    *alias_start++ = '\0';
                    strcpy(cli_expand_alias + offset, arg_ptr);
                    offset += strlen(arg_ptr);
                    if(offset >= MAX_MONITOR_LINE_LENGTH)
                    {
                    CLI_disp_error("Not enough memory for command line parsing\nexiting...");
                    exit(1);
                    }
                    
                    /* find alias_end */
                    if(*alias_start == '(')
                    {
                        /* it is like $(alias) */
                        alias_end = strstr(alias_start,")");
                        if(alias_end == NULL)
                        {
                            CLI_disp_error("syntax error in alias expansion\n");
                            arg_ptr     = alias_start;
                            alias_start = NULL;
                        }
                        else
                        {
                            *alias_end = '\0';
                            alias_start++;
                            
                            al = CLI_find_alias(alias_start);
                            if(al)
                            {
                                strcpy(cli_expand_alias + offset, al->value);
                                offset += strlen(al->value);
                                if(offset >= MAX_MONITOR_LINE_LENGTH)
                                {
                                    CLI_disp_error("Not enough memory for command line parsing\nexiting...");
                                    exit(1);
                                }
                            }
                            arg_ptr = alias_end + 1;
                            alias_start = strstr(arg_ptr,"$");
                        }
                    }
                    else
                    {
                        CLI_disp_error("syntax error in alias expansion\n");
                        arg_ptr     = alias_start;
                        alias_start = NULL;
                    }
                }
                
                /* recopy last non-alias part */
                strcpy(cli_expand_alias + offset, arg_ptr);
                offset += strlen(arg_ptr) + 1;
                if(offset >= MAX_MONITOR_LINE_LENGTH)
                {
                    CLI_disp_error("Not enough memory for command line parsing\nAbort...");
                    exit(1);
                }
            }
        }
        i++;
    }
}



/** ------------------------------------------------------------------
 * function : CLI_inter
 * \param   : command line
 * \return  : none
 * Description:
 *   Interpret the command
 * -----------------------------------------------------------------*/
t_cmd_function CLI_inter(char* line)
{
    t_cmd_function ret = {0,0,0};
    int  size;
    int  i;
    int  offset = 0;
    static char new_cmdline[MAX_MONITOR_LINE_LENGTH];
    static char cli_expand_alias[MAX_MONITOR_LINE_LENGTH];
    int  my_argc = 0;
    char ** my_argv;

    new_cmdline[0] = '\0' ;
    cli_expand_alias[0] = '\0';

    my_argv = CLI_malloc(MAX_PARAM_LENGTH * sizeof (char*));

    if ( my_argv == NULL){
        CLI_disp_error("Not enough memory to allocate in CLI_inter\n");
        return ret;
    }
    crtlc  = 0;
    myexit = 0;
    
    size = strlen(line);
    
    // Remove CR or LF
    if ((line[size - 1] == '\n') || (line[size - 1] == '\r'))
    {
        line[size - 1] = 0;
    }
    
    CLI_extract_token(line, cli_expand_alias, &my_argc, my_argv);
    
    // Check if argument contain several parameters separated by a '|'
    for(i=0; i<my_argc; i++)
    {
        strcpy(new_cmdline + offset, my_argv[i]);
        offset += strlen(my_argv[i]);
        new_cmdline[offset]=32; //Add Space between each argument
        offset ++;
    }
    new_cmdline[offset]=0;
    if(strstr(new_cmdline,"|")){
        CLI_extract_token(new_cmdline, cli_expand_alias, &my_argc, my_argv);
    }

    if(CLI_displayAfterAliasReplacement){
        int i;
        CLI_disp("after alias replacement: ");
        for(i=0;i<my_argc;i++){
            CLI_disp(" %s",my_argv[i]);
        }
        CLI_disp("\n");
    }
    ret.func = CLI_command_match(my_argv[0]);
    if(ret.func != NULL)
    {
        ret.argc = my_argc;
        ret.argv = my_argv;
    }
    else
    {
        CLI_free(my_argv);
    }

    return ret;
}

/** ------------------------------------------------------------------
 * function : CLI_disp_usage
 * \param   : command pointer
 * \return  : none
 * Description:
 *   Display usage of the command
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_disp_usage(CmdPtr command){
    int i,j;
    for (i = 0; i < nbitfs; i++)
    {
        for (j = 0; j < itflist[i].nb_funcs; j++)
        {
            if(itflist[i].list[j].command == command){
                CLI_disp("usage: %s",itflist[i].list[j].helpstring);
            }
        }
    }
}
/** ------------------------------------------------------------------
 * function : CLI_command_loop
 * \param   :
 * \return  : none
 * Description:
 *   Command interpreter loop
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_command_loop(char * initfile)
{
    t_los_file *p_File;
    char tmp[MAX_LINE] = "do ";
    strcat(tmp, initfile);

#ifdef __ARM_SYMBIAN	
    // Check if a first file is specified
    p_File = LOS_fopen(initfile, "rb");
    if(p_File != NULL)
    {
        LOS_fclose(p_File);
        CLI_addCommand(tmp);
    }
#else    

    // Check if a first file is specified
    p_File = LOS_fopen(initfile, "rb");
    if(p_File != NULL)
    {
        CLI_disp(">");
        LOS_fclose(p_File);
    }
    else
    {
        CLI_disp(PROMPT);
        CLI_read_console(tmp, MAX_LINE);
    }
    
    CLI_addCommand(tmp);
    
    while (1) {
        t_cmd_function cmd ;

        cmd = CLI_getNextFunction();

        if(cmd.func != NULL) {
            if(skip_state){
                if(cmd.func == restart_cmd_func){
                    skip_state = 0;
                    restart_cmd_func = NULL;
                }
            }
            if(skip_state){
                CLI_disp_debug("Command not executed\n"); 
            }
            else{
                CLI_do_command(cmd.func, cmd.argc, cmd.argv);
                if(current_input != SCRIPT_INPUT)
                {
                  CLI_disp_msg("Command executed\n"); 
                }
            }
            CLI_free(cmd.argv);
        }

        if(myexit)
            break;

        if(current_input != SCRIPT_INPUT) {
			int nb_hist_found = 1, nb_hist_to_apply=0, isHistoricAsked=0;
            CLI_disp(PROMPT);

			 while(nb_hist_found !=0)
			 {
				 //CLI_disp_msg("nb_hist_to_apply before CLI_read_console = %d\n", nb_hist_to_apply);
				 nb_hist_found = CLI_read_console(tmp, MAX_LINE);
				 //CLI_disp_msg("nb_hist_to_apply before adding = %d\n", nb_hist_to_apply);
				 nb_hist_to_apply = nb_hist_to_apply + nb_hist_found;
				 //CLI_disp_msg("nb_hist_found = %d\n", nb_hist_found);
				 //CLI_disp_msg("nb_hist_to_apply = %d\n", nb_hist_to_apply);
				 if(hist_size>0)
				 {
					//CLI_disp_msg("hist_size = %d\n", hist_size);
					 if(nb_hist_found != 0)
					 {
						if( (hist_size - nb_hist_to_apply ) >= 0)
						{
							//CLI_disp_msg("hist_size = %d\n", hist_size);
							//CLI_disp_msg("nb_hist_found = %d\n", nb_hist_found);
							//CLI_disp_msg("nb_hist_to_apply = %d\n", nb_hist_to_apply);
							//CLI_disp_msg("list_historic[hist_size - nb_hist_to_apply -= %d].cmd = %s\n", hist_size - nb_hist_to_apply , list_historic[hist_size - nb_hist_to_apply ].cmd);
							 CLI_disp_msg("continue navigating in historic (high or low arrow) or type enter to execute: %s\n", list_historic[hist_size - nb_hist_to_apply].cmd);

							 isHistoricAsked=1;
						}
						else
						{
							//CLI_disp_msg("hist_size - nb_hist_to_apply - 1 is NOT >= 0\n");
							CLI_disp_msg("no more historic, please go back in historic.\n");
						}
					 }
				 }
				 else
				 {
					//CLI_disp_msg("hist_size is NOT > 0\n");
				 }
				 //CLI_disp_msg("nb_hist_to_apply end = %d\n", nb_hist_to_apply);
			 }

			//CLI_disp_msg("tmp retrieve %s\n", tmp);
			if(isHistoricAsked)
			{

				if(tmp[0]==0)
				{
					if(NULL != list_historic[hist_size - nb_hist_to_apply].cmd)
					{
						CLI_disp_msg("use cmd history : %s\n", list_historic[hist_size - nb_hist_to_apply].cmd);
						strncpy(tmp, list_historic[hist_size - nb_hist_to_apply].cmd, sizeof(tmp)/sizeof(char));
					}
					else
					{
						CLI_disp_msg("use cmd history : impossible as NULL\n");
					}
				}
				else
				{
					CLI_disp_msg("use cmd retrieve tmp\n");
				}

			}
		   //CLI_disp_msg("CLI_addCommand call with : %s\n", tmp);
           CLI_addCommand(tmp);
       }
        else{
            CLI_disp(">");
        }

        crtlc = 0 ;

    }
#endif
}

/** ------------------------------------------------------------------
 * function : CLI_getCurrentInput
 * \param   :
 * \return  : none
 * Description:
 *   return current script input (file or console)
 * -----------------------------------------------------------------*/
EXPORT_C unsigned char  CLI_getCurrentInput()
{
    return(current_input);
}

/** ------------------------------------------------------------------
 * function : CLI_process_error
 \param err error type
 * \return 0 if no error, != 0 in other cases
 *
 * Process one error 
 * -----------------------------------------------------------------*/
EXPORT_C int  CLI_process_error (CMD_COMPLETION err)
{
    if( (err == CMD_COMPLETE) || (err == CMD_RUNNING) || (err == CMD_ERR_NONE)){
        CLI_add_alias ("$?", CLI_lltostr((unsigned int)0),1);
        return 0;
    }
    
    CLI_add_alias ("$?", CLI_lltostr((unsigned int) err),1);

    if( err == CMD_RETURN_TRUE){
        return 0;
    }
	
    
    if(restart_cmd_func){
        #ifndef __ARM_SYMBIAN
        skip_state = 1;
        #endif
    }
    

    switch(err) 
    {
        case CMD_ERR_ARGS:
            CLI_disp_it_error("error : CMD_ERR_ARGS \n");
            gErrArgs++;
            break;
        case CMD_ERR_GENERAL:
            CLI_disp_it_error("error : CMD_ERR_GENERAL \n");
            gErrGeneral++;
            break;
        case CMD_ERR_NOMEMORY:
            CLI_disp_it_error("error : CMD_ERR_NOMERORY \n");
            gErrNoMemory++;
            break;
        case CMD_ERR_OMX:
            CLI_disp_it_error("error : CMD_ERR_OMX \n");
            gErrOmx++;
            break;
        default :
            CLI_disp_it_error("error : Unknown error %x \n",err);
            gErrUnknown++;
            break;
    }

    // update number of total errors.
    gErrTotal++;
    return 1;
}

/** ------------------------------------------------------------------
 * function : CLI_addHistoric
 * \param   : command line to register
 * \return  : none
 *
 * Register line to historic table
 * -----------------------------------------------------------------*/
void CLI_addHistoric (char * line)
{
    if ((hist_enable == 1) && (strlen(line) > 1))
    {
	if (hist_index == MAX_HISTORIC) hist_index = 0;
	list_historic[hist_index].cmd = CLI_strdup(line);
	list_historic[hist_index].HId = hist_size;
	hist_size++;
	hist_index++;
    }
}

#ifdef __ARM_SYMBIAN
/** ------------------------------------------------------------------
 * function : CLI_write_console_symbian
 * \param   : Pointer to function handling console writing
 * \return  : none
 * Description:
 *   Initialize global variable pointer for writing into symbian console
 * -----------------------------------------------------------------*/
EXPORT_C void CLI_Init_Symbian(TCliSymbianInitParams* aCliSymbianInitParams)
{
    gCliSymbianInitParams.iCliWriteConsoleSymbianPtr = aCliSymbianInitParams->iCliWriteConsoleSymbianPtr;
}


void CLI_write_console_symbian(int channel, char *l)
{
    gCliSymbianInitParams.iCliWriteConsoleSymbianPtr(channel,l);
}

#endif


//*******************************************************************************************************
/** ------------------------------------------------------------------
 * function : CLI_exist_alias
 * \param   : Alias name
 * \return  : 0 if alias not defined, 1 if static alias, 2 if alias
 * Description: Return 0 if the name is not an alias, 
 * -----------------------------------------------------------------*/
int CLI_exist_alias(const char* name)
{
	if (CLI_find_static_alias((char*)name) != NULL)
		return(1);
	else if (CLI_find_alias((char*)name) != NULL)
		return(2);
	return 0;
}

