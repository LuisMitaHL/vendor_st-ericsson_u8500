/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <signal.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/time.h>

#define ctrlc SIGINT
#define segfault SIGSEGV

extern void signal_handle(void (*signal_handler)(int signo),int signum);
extern void ite_nmf_exit();
