/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _dBconv_include_h_
#define _dBconv_include_h_


#define NB_STEP 64
#define DBCONV_MEM EXTERN


#ifdef __flexcc2__
#ifndef _wtype_
#define _wtype_
typedef long MMlong;
typedef int MMshort;
typedef long long MMlonglong;
#endif
#endif

#ifdef __flexcc2__
#define MINIMUM_GAIN 0xff8000U
#else
#define MINIMUM_GAIN 0xffff8000L
#endif

#define Q8_ONE (1<<8) /* 1.0 in Q8 representation */
#define sixdB 0x605 /* 6.02dB = 0x605 in Q8 representation  */

#define STEP_FACT FORMAT_FLOAT(((float)(NB_STEP-1)/(float)sixdB),MAXVAL)
#define STEP_FACT_INV FORMAT_FLOAT(((float)6.02/(float)(NB_STEP-1)),Q8_ONE)


#endif
