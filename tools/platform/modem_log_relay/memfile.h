/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MEMFILE_H
#define MEMFILE_H

#include <stdio.h> // For EOF.

typedef struct {
    const char *data;
    unsigned int length;
    unsigned int offset;
} MFILE;

MFILE *memfopen(const char *, unsigned int);
void memfclose(MFILE *);
int memfreadline(MFILE *, char*, size_t);
void memfreset(MFILE *);

#endif

