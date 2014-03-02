/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "types.h"
#include "host_types.h"


void PrintBanner(void);
void PrintHelp(void);
int ParseArgs(int argc, char **argv, t_host_args *user_args);

#if FULL_GUI == 1
int CallPP(int argc, char **argv, t_host_args *user_args, t_uint32 nframes, t_uint16 width, t_uint16 height);
#endif

#endif
