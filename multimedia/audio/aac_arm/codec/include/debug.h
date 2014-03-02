/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _debug_h_
#define _debug_h_

extern void debuglog_short(int frame_select_ON, unsigned long fm, char *name, int *t, int n);
extern void debuglog(int frame_select_ON, unsigned long fm, char *name, char *t, int n);
extern void debuglog_float(int frame_select_ON, unsigned long fm, char *name,  Float *t, int time_nb,  int n, int scale);
extern void debuglog_pseudofloat(int frame_select_ON, unsigned long fm, char *name,  Float *m, Float *e, int n, int rescale);

#endif           
