/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _local_polyanalysis_test_h_
#define _local_polyanalysis_test_h_


/* defines for tests */
#ifdef __flexcc2__
#define TEST_MEM __EXTERN
#define SIZE_TEST 4096
#else
#define TEST_MEM
#define SIZE_TEST 16384
#endif


extern TEST_MEM Float const     in[SIZE_TEST];
extern TEST_MEM Float const     res_sb[SIZE_TEST];
extern TEST_MEM Float const     res_sb_scaling[SIZE_TEST];
extern TEST_MEM Float const     res_sb_bt[SIZE_TEST];
extern TEST_MEM Float const     res_sb_scaling_bt[SIZE_TEST];

#endif /* Do not edit below this line */

