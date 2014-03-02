/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   table8_16_low_latency.h
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#define N_8_16_low_latency 60 
 #define M_8_16_low_latency 2 
 #define L_8_16_low_latency 1 
 #define decimno_8_16_low_latency 1 
 #define DELAY_8_16_low_latency (30)  
 #define SHIFT_8_16_low_latency -1 
 #define SCALE_8_16_low_latency FORMAT_FLOAT(1.000000000000000,MAXVAL)
extern Float const YMEM resample_8_16_low_latency[61];
extern int const RESAMPLE_MEM src_incr_offset_2_1[3];
#define NZCOEFS_8_16_low_latency (0)
