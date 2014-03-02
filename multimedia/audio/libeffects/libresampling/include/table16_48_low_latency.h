/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   table16_48_low_latency.h
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#define N_16_48_low_latency 90 
 #define M_16_48_low_latency 3 
 #define L_16_48_low_latency 1 
 #define decimno_16_48_low_latency 1 
 #define DELAY_16_48_low_latency (30)  
 #define SHIFT_16_48_low_latency -2 
 #define SCALE_16_48_low_latency FORMAT_FLOAT(0.750000000000000,MAXVAL)
extern Float const YMEM resample_16_48_low_latency[91];
extern int const RESAMPLE_MEM src_incr_offset_3_1[4];
#define NZCOEFS_16_48_low_latency (0)
