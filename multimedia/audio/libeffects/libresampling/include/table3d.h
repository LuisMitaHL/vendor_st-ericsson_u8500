/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   table3d.h
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#define N_3d 720 
 #define M_3d 1 
 #define L_3d 3 
 #define decimno_3d 4 
 #define DELAY_3d (180)  
 #define SHIFT_3d 1 
 #define SCALE_3d FORMAT_FLOAT(1.000000000000000,MAXVAL)
extern Float const YMEM resample_12x[361];
extern int const RESAMPLE_MEM src_incr_offset_1_3[4];
#define NZCOEFS_3d (0)
