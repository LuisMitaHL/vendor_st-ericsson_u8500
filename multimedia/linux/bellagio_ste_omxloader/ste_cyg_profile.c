/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file    ste_cyg_profile.c
* \brief   Provide functions called when -finstrument-functions gcc option used
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define __CYG_PROFILE_FILENAME "/profile.log"
#define __CYG_PROFILE_NB_MAX_THREAD 128
#define __CYG_PROFILE_RECORD_BUFFER_SIZE 128

typedef struct {
    int thread_index;
    int profile_depth;
    unsigned long addr;
} profile_record_t;

typedef struct {
    pthread_t thread_id;
    int profile_depth;
    int record_index;
    profile_record_t record_buffer[__CYG_PROFILE_RECORD_BUFFER_SIZE];
} cyg_profile_entry_t;

/* C ensures init to 0 */
static cyg_profile_entry_t cyg_profile_entries[__CYG_PROFILE_NB_MAX_THREAD];
static int cyg_profile_initialized = 0;
static FILE * __cyg_profile_file;
static int __cyg_profile_fileno;

#ifdef ANDROID
#define PRINTF LOGI
#define LOG_TAG "STE_OMX_cyg_profile"
#include <cutils/log.h>
#else
#define PRINTF(...) printf(__VA_ARGS__)
#endif

#ifndef CYG_PROFILE_TEXT
/* Default to binary trace based */
#define CYG_PROFILE_TEXT 0
#endif 

void ste_cyg_profile_init()
{
    /* FIXME: add some Linux specific args to filter here */
    __cyg_profile_file = fopen(__CYG_PROFILE_FILENAME, "w");
    if(__cyg_profile_file == NULL) {
	PRINTF("Failed to open __cyg_profile_file(%s)\n", __CYG_PROFILE_FILENAME);
    } else {
	PRINTF("__cyg_profile_file : %s opened\n", __CYG_PROFILE_FILENAME);
    }
    cyg_profile_initialized = 1;
    /* Register the initial thread */
    cyg_profile_entries[0].thread_id = pthread_self();
    __cyg_profile_fileno = fileno(__cyg_profile_file);
    
    PRINTF("registered thread %d\n", (int)cyg_profile_entries[0].thread_id);
}

void __cyg_profile_func_enter( void *func_address, void *call_site )
                                __attribute__ ((no_instrument_function));

void __cyg_profile_func_exit ( void *func_address, void *call_site )
                                __attribute__ ((no_instrument_function));

static inline int __cyg_profile_find_index() __attribute__ ((no_instrument_function));

static inline int __cyg_profile_find_index()
{
    pthread_t thread_id = pthread_self();
    int i = 0;	
    while(cyg_profile_entries[i].thread_id != thread_id) {
	/* FIXME - not thread safe  !!!!! use atomic instruction instead */
	if (cyg_profile_entries[i].thread_id == 0) {
	    cyg_profile_entries[i].thread_id = thread_id;
	    PRINTF("Registered thread %d : 0x%08x\n", i, (int)thread_id);
	    return i;
	}
	i++;
    }
    return i;
}

void __cyg_profile_func_enter(void *this, void *callsite)
{
    if(__cyg_profile_file != NULL) {
	int index = __cyg_profile_find_index();
	cyg_profile_entry_t * thread_entry = &cyg_profile_entries[index];
	profile_record_t * record = &thread_entry->record_buffer[thread_entry->record_index++];
	record->thread_index = index;
	record->profile_depth = thread_entry->profile_depth++;
	record->addr = (unsigned long) this;
	
	if(thread_entry->record_index == __CYG_PROFILE_RECORD_BUFFER_SIZE) {
#if CYG_PROFILE_TEXT
	    int i, nbchar;
	    char buffer[1024];
	    for(i=0;i<__CYG_PROFILE_RECORD_BUFFER_SIZE;i++) {
		record = &thread_entry->record_buffer[i];
		/* We write to a buffer and then use write as printf does not
		   guaranty that entire string will be written at once in case of 
		   multiple threads */
		nbchar = sprintf(buffer, "++++++++ %d %d 0x%08x\n",
				 record->thread_index, record->profile_depth, record->addr);
		write(__cyg_profile_fileno, buffer, nbchar);
	    }
#else
	    write(__cyg_profile_fileno, thread_entry->record_buffer,
		  __CYG_PROFILE_RECORD_BUFFER_SIZE*sizeof(profile_record_t));
#endif
	    thread_entry->record_index = 0;
	}
    }
}

void __cyg_profile_func_exit(void *this, void *callsite)
{
    if(__cyg_profile_file != NULL) {
	cyg_profile_entries[__cyg_profile_find_index()].profile_depth--;
    }
}
