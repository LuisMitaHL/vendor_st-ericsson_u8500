/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   table6d.h
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#define N_6d 720 
 #define M_6d 1 
 #define L_6d 6 
 #define decimno_6d 2 
 #define DELAY_6d (360)  
 #define SHIFT_6d 2 
 #define SCALE_6d FORMAT_FLOAT(1.000000000000000,MAXVAL)
extern Float const YMEM resample_12x[361];
extern int const RESAMPLE_MEM src_incr_offset_1_6[7];
#define NZCOEFS_6d (0)
