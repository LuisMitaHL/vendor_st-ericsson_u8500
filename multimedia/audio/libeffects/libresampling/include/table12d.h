/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   table12d.h
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#define N_12d 720 
 #define M_12d 1 
 #define L_12d 12 
 #define decimno_12d 1 
 #define DELAY_12d (720)  
 #define SHIFT_12d 3 
 #define SCALE_12d FORMAT_FLOAT(1.000000000000000,MAXVAL)
extern Float const YMEM resample_12x[361];
extern int const RESAMPLE_MEM src_incr_offset_1_12[13];
#define NZCOEFS_12d (0) 
