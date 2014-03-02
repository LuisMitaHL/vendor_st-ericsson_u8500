/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _local_fft_h_
#define _local_fft_h_

#include "audiolibs_common.h"
#include "fft.h"
#include "fft_table.h"

#define CHECK_RANGE(a,b)  assert((a)<(b) && (a)>=-(b))

#ifdef PROJ_OP
/* growth detection is broken on op9 core */
#define SOFTWARE_GROWTH_DETECTION
#endif /* PROJ_OP */

/* defines for tests */
#ifdef __flexcc2__
#define TEST_MEM __EXTERN
#else // __flexcc2__
#define TEST_MEM 
#endif // __flexcc2__


#endif // _local_fft_h_
