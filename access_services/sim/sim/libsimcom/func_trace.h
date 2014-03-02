/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : func_trace.h
 * Description     : Function tracing interface file
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */
#ifndef __func_trace_h__
#define __func_trace_h__

__attribute__((no_instrument_function)) void setProcName ( const char *name );
__attribute__((no_instrument_function)) void setThreadName ( const char *name );
__attribute__((no_instrument_function)) const char *getThreadName ( void );
__attribute__((no_instrument_function)) const char *getProcName ( void );

#endif
