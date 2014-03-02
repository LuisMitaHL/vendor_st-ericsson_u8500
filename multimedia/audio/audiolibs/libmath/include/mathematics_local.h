/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _MATHEMATICS_LOCAL_H_
#define _MATHEMATICS_LOCAL_H_

#include "mathematics.h"
#include "mathematics_local.h"


#define ONE_HALF_W16            0x4000      // 1/2      in Q15
#define SQRT_0_5_W16            0x5A82      // 0.5^0.5  in Q15
#define SQRT_SQRT_2_DIV_2_W16   0x4C1C      // 2^0.25/2 in Q15
#define SQRT_SQRT_0_5_W16       0x6BA2      // 0.5^0.25 in Q15

#define ONE_HALF_W32            0x40000000L // 1/2      in Q31
#define SQRT_0_5_W32            0x5A82799AL // 0.5^0.5  in Q31
#define SQRT_SQRT_2_DIV_2_W32   0x4C1BF829L // 2^0.25/2 in Q31
#define SQRT_SQRT_0_5_W32       0x6BA27E65L // 0.5^0.25 in Q31

#define IND_ONE_HALF            0
#define IND_SQRT_0_5            1
#define IND_SQRT_SQRT_2_DIV_2   2
#define IND_SQRT_SQRT_0_5       3

extern const int16_t tab_coef_sqrt16[8];
extern const int32_t tab_coef_sqrt32[20];
extern const int16_t tab1_sqrt16[256 * 2];
extern const int32_t tab1_sqrt32[256 * 2];
extern const int16_t tab2_sqrt16[129];
extern const int32_t tab2_sqrt32[1025];

#endif /* _MATHEMATICS_LOCAL_H_ */
