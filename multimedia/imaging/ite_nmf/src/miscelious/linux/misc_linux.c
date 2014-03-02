/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "misc_linux.h"

struct sigaction signalhandle;

void signal_handle(void (*signal_handler)(int signo),int signum)
{
    signalhandle.sa_handler = signal_handler;
    sigemptyset(&signalhandle.sa_mask);
    signalhandle.sa_flags = 0;
    sigaction(signum, &signalhandle, NULL);
}

void ite_nmf_exit()
{
exit(0);
}

