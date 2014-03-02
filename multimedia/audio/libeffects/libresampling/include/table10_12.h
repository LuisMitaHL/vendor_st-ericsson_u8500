/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   table10_12.h
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#define N_10_12 720 
 #define M_10_12 5 
 #define L_10_12 6 
 #define decimno_10_12 2 
 #define DELAY_10_12 (72) 
 #define SHIFT_10_12 (-1) 
#define SCALE_10_12 FORMAT_FLOAT(0.625000000000000,MAXVAL) 
extern Float const YMEM resample_12x[361];
extern int const RESAMPLE_MEM src_incr_offset_5_6[11];
#define NZCOEFS_10_12 (0)
