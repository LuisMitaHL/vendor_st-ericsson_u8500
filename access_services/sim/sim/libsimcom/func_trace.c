/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : func_trace.c
 * Description     : Function tracing implementation file
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "func_trace.h"

#define FILENAME    "func_ee.log"

static int firsttime = 1;
static FILE *fp = NULL;
static unsigned long basetime = 0;
static const char *procName = NULL;
#define MAX_THREAD_NAMES    100
#define MAX_THREAD_NAMELEN  20
static struct {
    pthread_t   threadID;
    char        threadName[MAX_THREAD_NAMELEN];
} threadNameTable[MAX_THREAD_NAMES];
static int numThreadNames = 0;
static pthread_mutex_t tnMux;

static __attribute__((no_instrument_function)) void
tnInit ( void ) {
    pthread_mutex_init( &tnMux, 0 );
}
static __attribute__((no_instrument_function)) void
tnFini ( void ) {
    pthread_mutex_destroy( &tnMux );
}

static __attribute__((no_instrument_function)) void
getBaseTime ( void )
{
    time_t      now,base;
    struct tm   tm_now;
    now = time(NULL);
    tm_now = *localtime(&now);
    tm_now.tm_min = 0;
    tm_now.tm_sec = 0;
    base = mktime(&tm_now);
    basetime = base;
}

static __attribute__((no_instrument_function)) unsigned long
getOffsetMsec ( void )
{
    struct timeval  now;
    unsigned long result = 0;

    if ( basetime == 0 ) {
        getBaseTime();
    }

    if ( gettimeofday(&now, NULL) == 0 ) {
        now.tv_sec -= basetime;
        now.tv_usec /= 1000;
        result = now.tv_sec * 1000 + now.tv_usec;
    }

    return result;
}

static __attribute__((no_instrument_function)) void
cleanup ( void )
{
    if ( fp != NULL ) {
        fclose(fp);
        fp = NULL;
        firsttime = 1;
    }
}

static __attribute__((no_instrument_function)) void
setup ( void )
{
    char    fn[1000];
    pid_t   pid = getpid();
    firsttime = 0;
    atexit( cleanup );
    sprintf(fn,"%s_%d_%s",procName,(int)pid,FILENAME);
    fp = fopen(fn,"w");
    if ( fp == NULL ) {
        fprintf(stderr,"Cannot create function enter/exit profile (%s)\n",fn);
    }
}

static __attribute__((no_instrument_function)) void
trace ( const char *state, void *this_fn, void *call_site )
{
    unsigned long now = getOffsetMsec();
    const char *p = getProcName();
    const char *t = getThreadName();
    if ( firsttime ) {
        setup();
        if ( fp != NULL ) {
            fprintf(fp,"!:state:process:thread:mSecTime:this_fn:call_site\n");
        }
    }
    if ( fp != NULL ) {
        fprintf(fp,"@:%s:%s:%s"
                   ":%lu:%08lx:%08lx\n",
                   state,p,t,
                   now,(unsigned long)this_fn,(unsigned long)call_site);
        fflush(fp);
    }
}

void __attribute__((no_instrument_function))
__cyg_profile_func_enter (void *this_fn, void *call_site)
{
    trace("Enter",this_fn,call_site);
}

void __attribute__((no_instrument_function))
__cyg_profile_func_exit (void *this_fn, void *call_site)
{
    trace("Leave",this_fn,call_site);
}

__attribute__((no_instrument_function)) void
setThreadName ( const char *name )
{
    pthread_mutex_lock( &tnMux );
    if ( numThreadNames < MAX_THREAD_NAMES ) {
        pthread_t me = pthread_self();
        threadNameTable[numThreadNames].threadID = me;
        strncpy( threadNameTable[numThreadNames].threadName, name, MAX_THREAD_NAMELEN );
        threadNameTable[numThreadNames].threadName[MAX_THREAD_NAMELEN-1] = '\0';
        numThreadNames++;
    } else {
        static int firstReport = 1;
        if ( firstReport ) {
            fprintf( stderr, "setThreadName - max entries reached = %d\n", MAX_THREAD_NAMES );
            firstReport = 0;
        }
    }
    pthread_mutex_unlock( &tnMux );
}

__attribute__((no_instrument_function)) void
setProcName ( const char *name )
{
    const char *p;
    if ( name != NULL && (p=strrchr(name,'/')) != NULL ) {
        procName = p+1;
    } else {
        procName = name;
    }
    tnInit();
    atexit(tnFini);
    setThreadName("(main)");
}

__attribute__((no_instrument_function)) const char *
getThreadName ( void )
{
    const char *t = "?";
    if ( numThreadNames > 0 ) {
        pthread_mutex_lock( &tnMux );
        pthread_t me = pthread_self();
        int i;
        for ( i = 0 ; i < numThreadNames ; i++ ) {
            if ( threadNameTable[i].threadID == me ) {
                t = threadNameTable[i].threadName;
                break;
            }
        }
        pthread_mutex_unlock( &tnMux );
    }
    return t;
}

__attribute__((no_instrument_function)) const char *
getProcName ( void )
{
    return procName ? procName : "(noProc)";
}
