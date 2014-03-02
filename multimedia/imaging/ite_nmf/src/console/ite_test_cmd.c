/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#include "ate_common.h"
#include "cli.h"
#include <string.h>
#include <stdio.h>
#include "test/api/test.h"
#include "ite_main.h"


t_uint32 LOS_AllocatedChunk_start;
static int TestStarted = 0;
static int memory_check_enabled = 0;
t_uint16 underflow_counter;
t_uint16 overflow_counter;



/* ---------------------------------------------------------------------------- */
/**
 * \defgroup test_cmd Test commands
 * \brief This function is used in test_start to store the initial memory status 
 */
/* ---------------------------------------------------------------------------- */
static void memory_check_start (void)
{
#ifndef __ARM_SYMBIAN
    // initial allocations status at DSPs side is managed in init_ate.c (afm_memory_status_init(void))
    // storing initial allocations status at arm side
    LOS_AllocatedChunk_start = LOS_GetAllocatedChunk();
#else
    CLI_disp_msg("memory_check_start not implemented yet in SYMBIAN\n");
#endif

} 

/* -------------------------------------------------------------------------------------------------------- */
/**
 * \defgroup test_cmd Test commands
 * \brief This function is used in test_end to check the memory status and manage the memory leakage if any 
 */
/* -------------------------------------------------------------------------------------------------------- */
static void memory_check_end (void)
{

#ifndef __ARM_SYMBIAN

    t_uint32 LOS_AllocatedChunk_end;

    LOS_AllocatedChunk_end = LOS_GetAllocatedChunk();
    CLI_disp_msg("Memory Checking...\n");
    
    /* ITE not implemented
    if (!afm_memory_status_check()){
        CLI_disp_msg("\tERROR: AFM Memory leakage\n");
    }   
    else{
        CLI_disp_msg("\tAFM Memory Check Passed\n");
    } 
    */
    
    if (LOS_AllocatedChunk_end!=LOS_AllocatedChunk_start){
        CLI_disp_msg("\tERROR: ARM Memory Leakage\n");
        CLI_disp_msg("\t\tLOS_alloc start: %d\n",LOS_AllocatedChunk_start);
        CLI_disp_msg("\t\tLOS_alloc end: %d\n",LOS_AllocatedChunk_end);
        CLI_disp_msg("\t\tFreeing ARM allocated memory....\n"); 
    /* ITE not implemented
        mem_check_free();
    */
        LOS_AllocatedChunk_end = LOS_GetAllocatedChunk();
        if (LOS_AllocatedChunk_end!=LOS_AllocatedChunk_start){
            CLI_disp_msg("Freeing ARM allocated memory Failed\n");
        }
        else{
            CLI_disp_msg("Freeing ARM allocated memory Passed\n");
        }
    }
    else{
        CLI_disp_msg("\tARM Memory Check Passed\n");  
    }

#else
    CLI_disp_msg("memory_check_end not implemented yet in SYMBIAN\n");
#endif

} 

static void getStringFromArgv(char * desc, int argc, char **argv, int offset){
    int i;
    if(offset<argc){
        strcpy(desc,argv[offset]);
        for(i = offset+1; i< argc;i++){
            strcat(desc, " ");
            strcat(desc, argv[i]);
        }
    }
}


static CMD_COMPLETION C_disable_mem_check(int argc, char **argv)
{
 UNUSED(argv);
	if(argc < 1)
	{
		CLI_disp_usage(C_disable_mem_check);
		return CMD_ERR_ARGS;
	} 
    memory_check_enabled =0;
    CLI_disp_msg("memory check on test_end disabled\n");
	return CMD_COMPLETE;
}

static CMD_COMPLETION C_enable_mem_check(int argc, char **argv)
{
 UNUSED(argv);
	if(argc < 1)
	{
		CLI_disp_usage(C_enable_mem_check);
		return CMD_ERR_ARGS;
	} 
    memory_check_enabled =1;
    CLI_disp_msg("memory check on test_end enabled\n");
	return CMD_COMPLETE;
}

/* ----------------------------------------------------------------- */
/**
  \defgroup test_logDir test_logDir 
  \ingroup test_cmd
  \b Usage:
  \code test_logDir <LogDirectory> \endcode
  \param path for log files. If not set,logs are written where the mmte is started. 
  \param ShortDescription brief description of the test
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_logDir(int argc, char **argv)
{
    if(argc > 2)
	{
		CLI_disp_usage(C_test_logDir);
		return CMD_ERR_ARGS;
	} 
   
    if (argc < 2)
    {
        // Reinit to 0 array for logDirectory
        memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );
    }
    else
    {
        strcpy(g_out_path,argv[1]);
    }
	
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_set_out_path_cmd
   PURPOSE  : initialise global g_out_path for .out file directory 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_set_out_path_cmd(int a_nb_args, char ** ap_args)
{
	if (a_nb_args == 2)
	{
		if (0==strcmp(ap_args[1],"help")) 
		{
			CLI_disp_error("syntax: setoutpath ~      : empty g_out_path string\n");
			CLI_disp_error("syntax: setoutpath ..     : remove last directory from string g_out_path (cd ..)\n");
			CLI_disp_error("syntax: setoutpath <path> : add path to g_out_path (do not forget /)\n");
		}
		else if (0==strcmp(ap_args[1],"..")) 
			*(strrchr(g_out_path,47)) = 0x00;
		else if (0==strcmp(ap_args[1],"~")) 
			memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );
		else
			strcat(g_out_path,ap_args[1]);

		CLI_disp_msg("Current path: >%s< \n",(char *)g_out_path);
	}
	else
		CLI_disp_usage(C_set_out_path_cmd);

	return CMD_COMPLETE;
}
	


/* ----------------------------------------------------------------- */
/**
  \defgroup test_start test_start 
  \ingroup test_cmd
  \b Usage:
  \code test_start <TestName> <ShortDescription> \endcode
  \param TestName Name of the test
  \param ShortDescription brief description of the test
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_start(int argc, char **argv)
{
	char desc[2048] = "description no provided";
    char filename[256];
    char* plogDirectory = g_out_path;

    if(argc < 2)
	{
		CLI_disp_usage(C_test_start);
		return CMD_ERR_ARGS;
	}
    if(argc > 2){
        getStringFromArgv(desc, argc, argv, 2);
    }
    if(TestStarted == 1){
        CLI_disp_msg("test_start cannot be called twice, please call test_end before recalling test_start\n");
        return CMD_ERR_GENERAL;
    }
		
    strcpy(filename,argv[1]);
    // strcat(filename, "_");
    // strcat(filename, MMTE_PLATFORM);
	if(*plogDirectory != 0 )
        mmte_testStart(filename, desc,plogDirectory);
    else
        mmte_testStart(filename, desc,NULL);
	
    TestStarted =1;
    if(memory_check_enabled){
        memory_check_start();	
    }
	return CMD_COMPLETE;
}



/* ----------------------------------------------------------------- */
/**
  \defgroup test_comment test_comment
  \ingroup test_cmd
  \b Usage:
  \code test_comment <Comment> \endcode
  \param Comment a comment
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_comment(int argc, char **argv)
{
	char desc[2048] = "description no provided";
	if(argc < 2)
	{
		CLI_disp_usage(C_test_comment);
		return CMD_ERR_ARGS;
	}
    if(TestStarted == 0){
        CLI_disp_msg("test_start must be called before calling test_comment\n");
        return CMD_ERR_GENERAL;
    }

    getStringFromArgv(desc, argc, argv, 1);
		
	mmte_testComment(desc);
	
	return CMD_COMPLETE;
}



/* ----------------------------------------------------------------- */
/**
  \defgroup test_property test_property 
  \ingroup test_cmd
  \b Usage:
  \code test_property <Key> <Value>\endcode
  \param Key property name
  \param Value property value
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_property(int argc, char **argv)
{
	char desc[2048] = "description no provided";
	if(argc < 3)
	{
		CLI_disp_usage(C_test_property);
		return CMD_ERR_ARGS;
	}
    if(TestStarted == 0){
        CLI_disp_msg("test_start must be called before calling test_property\n");
        return CMD_ERR_GENERAL;
    }
		
    getStringFromArgv(desc, argc, argv, 2);
		
	mmte_testProperty(argv[1], desc);
	
	return CMD_COMPLETE;
}



/* ----------------------------------------------------------------- */
/**
  \defgroup test_next test_next
  \ingroup test_cmd
  \b Usage:
  \code test_next <StepDescription>\endcode
  \param StepDescription ???
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_next(int argc, char **argv)
{
	char desc[2048] = "description no provided";
	if(argc < 2)
	{
		CLI_disp_usage(C_test_next);
		return CMD_ERR_ARGS;
	}
    if(TestStarted == 0){
        CLI_disp_msg("test_start must be called before calling test_next\n");
        return CMD_ERR_GENERAL;
    }
    getStringFromArgv(desc, argc, argv, 1);
	
    mmte_testNext(desc);
	
	return CMD_COMPLETE;
}



/* ----------------------------------------------------------------- */
/**
  \defgroup test_result test_result
  \ingroup test_cmd
  \b Usage:
  \code test_result <Passed|Failed>\endcode
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_result(int argc, char **argv)
{
	if(argc < 2)
	{
		CLI_disp_usage(C_test_result);
		return CMD_ERR_ARGS;
	}
    if(TestStarted == 0){
        CLI_disp_msg("test_start must be called before calling test_result\n");
        return CMD_ERR_GENERAL;
    }
	if (*argv[1] == 'P'){
		mmte_testResult(TEST_PASSED);
    }
    else { 
		mmte_testResult(TEST_FAILED);
    }
		
	return CMD_COMPLETE;
}



/* ----------------------------------------------------------------- */
/**
  \defgroup test_end test_end
  \ingroup test_cmd
  \b Usage:
  \code test_end \endcode
  */
/* ----------------------------------------------------------------- */
static CMD_COMPLETION C_test_end(int argc, char **argv)
{
 UNUSED(argv);
	if(argc < 1)
	{
		CLI_disp_usage(C_test_end);
		return CMD_ERR_ARGS;
	}
    if(memory_check_enabled){
        memory_check_end(); 
    }
	mmte_testEnd();
    TestStarted =0;    
		return CMD_COMPLETE;
}
 

static CMD_COMPLETION C_goto_end_on_error(int argc, char **argv)
{
 UNUSED(argv);
	if(argc < 1)
	{
		CLI_disp_usage(C_goto_end_on_error);
		return CMD_ERR_ARGS;
	} 
	CLI_SkipOnError_Until(C_test_end);
    CLI_disp_msg("Goto test_end on error enabled\n");
	return CMD_COMPLETE;
}


/* ----------------------------------------------------------------- */
/**
  \defgroup mmte_check_test mmte_check_test
  \ingroup mmte_cmd
  \b Usage:
  \code mmte_check_test \endcode

  This function prompts "was test ok [y/n]" to user and wait for answer
  to continue. If answer is \b y it displays "TEST PASSED" else it
  displays "TEST FAILED". THis function is usefull for test report generation
  */
/* ----------------------------------------------------------------- */
CMD_COMPLETION C_check_test(int argc, char **argv)
{
    UNUSED(argv);
    UNUSED(argc);
    char buffer[80], *Answer=buffer;
    UNUSED(Answer);

    if(TestStarted == 0){
        CLI_disp_msg("test_start must be called before calling check_test\n");
        return CMD_ERR_GENERAL;
    }

    mmte_testNext("Underflows");
    if(underflow_counter)
    {   
        CLI_disp_msg("Detected %u underflow error(s)\n", underflow_counter);
        CLI_disp_msg("UNDERFLOW STATUS: FAILED\n");
        mmte_testResult(TEST_FAILED);
    }
    else
    {  
        CLI_disp_msg("UNDERFLOW STATUS: PASSED\n");
        mmte_testResult(TEST_PASSED);
    }

    mmte_testNext("Overflows");
    if (overflow_counter)
    {   
        CLI_disp_msg("Detected %u overflow error(s)\n", overflow_counter);
        CLI_disp_msg("OVERFLOW STATUS: FAILED\n");
        mmte_testResult(TEST_FAILED);
    }
    else
    {   
        CLI_disp_msg("OVERFLOW STATUS: PASSED\n");
        mmte_testResult(TEST_PASSED);
    }

    CLI_disp_msg("\nWas Test ok ? [y/n]");

#ifndef __ARM_SYMBIAN
    gets(Answer);

    mmte_testNext("Human criteria");

    if((Answer[0]=='y') || (Answer[0]=='Y')){
        CLI_disp_msg("TEST STATUS: PASSED\n");
        mmte_testResult(TEST_PASSED);
    }
    else{
        CLI_disp_msg("TEST STATUS: FAILED\n");
        mmte_testResult(TEST_FAILED);
    }
#else
    CLI_disp_msg("!!!! FIX ME NOT IMPLEMENTED YET IN SYMBIAN !!!! \n");
#endif

    //Reset underflow and overflow counters
    underflow_counter = 0;
    overflow_counter = 0;

    return CMD_COMPLETE;
}


static CMD_COMPLETION C_file_comp(int argc, char **argv)
{
    t_los_file          *file[2]={NULL,NULL};
    char                *zone[2]={NULL,NULL};
    t_uint32             size[2]={0,0};
    t_uint32             size_asked=0;
    int                  i;
    int                  swap = 0;
    int                  first_error = 0;
    t_bool ate_comp_check = FALSE;
  
    if(argc < 3)
    {
        CLI_disp_usage(C_file_comp);
        return CMD_ERR_ARGS;
    }
   
    if(argc > 3) {
        size_asked = CLI_atoi(argv[3]);
    }
  
    /* read command line */
    for(i=0; i<2; i++) {
        file[i] = LOS_fopen(argv[i+1],"rb");
        if(file[i] == NULL) {
            CLI_disp_error("Cannot open file %s \n",argv[i+1]);
            goto end;
        }
        else {
            LOS_fseek(file[i], 0, LOS_SEEK_END);
            size[i] = LOS_ftell(file[i]);
            LOS_fseek(file[i], 0, LOS_SEEK_SET);
            if(size_asked == 0){
                size_asked = size[i];
            }
            else if(size[i] < size_asked){
                size_asked = size[i];
            }
        }
	}

    for(i=0; i<2; i++) {
        t_uint32 read_size = 0;
        zone[i] = CLI_malloc(size_asked);
        if(!zone[i])
        {   
            CLI_disp_error("Not enough memory available!! \n");
            return CMD_ERR_NOMEMORY;
        }
        read_size = LOS_fread(zone[i], 1, size_asked, file[i]) ;
        if(read_size != size_asked){
            CLI_disp_error("Error when reading file %d, read %d bytes instead of %d\n", i+1, read_size , size_asked);
            return CMD_ERR_GENERAL;            
        }
    }
  
  
    /* Compare */
compare:  
    for(i=0;(t_uint32)i<size_asked-1;i++) {
        if(zone[1][i] != zone[0][i+swap]) {
            if(swap) {
                if(i>first_error) {
                    first_error=i;
                }
                CLI_disp_error("Files differ at byte %d \n",first_error);
                goto end;
            }
            else {
                swap = 1;
                first_error = i;
                goto compare;
            }
        }

        i++;
	
        if(zone[1][i] != zone[0][i-swap]) {
            if(swap) {
                if(i>first_error) {
                    first_error=i;
                }
                CLI_disp_error("Files differ at byte %d \n",first_error);
                goto end;
            }
            else {
                swap = 1;
                first_error = i;
                goto compare;
            }
        }
    }

    if(swap) CLI_disp_msg("Byte swap needed \n");
    CLI_disp_msg("%d Bytes compared \n",size_asked);
    
    if(size_asked != 0){
        ate_comp_check = TRUE;
    }
    
    if(TestStarted){
        mmte_testNext("File Comparision");
        if(ate_comp_check){
            mmte_testResult(TEST_PASSED);
        }
        else{
            mmte_testResult(TEST_FAILED);
        }
    }
  
end:
    for(i=0; i<2; i++) {
        if(file[i]) {
            if(zone[i]) {
                CLI_free(zone[i]);
            }
            LOS_fclose(file[i]);
        }
    }
	
	return CMD_COMPLETE;
}


/*-----------------------------------------*/
/* Test commands list         			   */
/*-----------------------------------------*/
t_cmd_list  test_cmd_list[] = {
    { "check_test",			C_check_test,  "check_test: \n"},
    { "ate_check_test",		C_check_test,  "ate_check_test: \n"}, //maintained for backwards compatibility
    { "test_logDir",		C_test_logDir,      "test_logDir :   [<LogDirectory>]\n"},
	{ "setoutpath",			C_set_out_path_cmd, "setoutpath: setoutpath <~> or <..> or <path> \n"},
	{ "test_start",			C_test_start,    "test_start :    <TestName> <ShortDescription>\n"},
	{ "test_comment",		C_test_comment,  "test_comment :  <Comment>\n"},
	{ "test_property",		C_test_property, "test_property : <Key> <Value>\n"},
	{ "test_next",			C_test_next,     "test_next :     <StepDescription>\n"},
	{ "test_result",		C_test_result,   "test_result :   <Passed|Failed>\n"},
    { "test_end",			C_test_end,         "test_end :      no param\n"},
    { "disable_mem_check",	C_disable_mem_check,"disable_mem_check:      no param\n"},
    { "enable_mem_check",	C_enable_mem_check, "enable_mem_check:      no param\n"},
    { "file_comp",			C_file_comp,        "file_comp :  <filename1> <filenname2> [<byte_to_compare>]\n"},
    { "goto_end_on_error",	C_goto_end_on_error,"goto_end_on_error :      no param\n"}
};

/**
 * \defgroup test_cmd Test commands
 * \brief This interface gathers all commands related to tests
 */
void Init_Test_CmdList(void)
{
	CLI_register_interface("Tests", sizeof(test_cmd_list)/(sizeof(test_cmd_list[0])), test_cmd_list, 1);
}

