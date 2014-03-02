/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _driver_readfile_h_
#define _driver_readfile_h_

#include <stdio.h>
#include <assert.h>

extern void
dfile_read_set_input(FILE *f);

extern void
dfile_read_restart(void);

extern int
dfile_read_getbyte(void);

extern int
dfile_read_eof(int);

#endif /* Do not edit below this line */
