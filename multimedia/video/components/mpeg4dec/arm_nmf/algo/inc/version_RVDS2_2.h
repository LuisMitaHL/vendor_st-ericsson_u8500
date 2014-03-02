/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 //This file is used when we have RVDS version 2.2 as the intrinsics listed below are not avaiable
 //in RVCT 2.2 version

#ifndef VERSION_RVDS2_2_H
#define VERSION_RVDS2_2_H

#ifdef RVDS_2_2
#define __ssat(a,b) MIN(MAX(a, -(1 << ((b) - 1))), (1 << ((b) - 1)) - 1)


static int CountLeadingZeros(int Value) { \
            int foo; \
            foo = 0; \
            while ((0x80000000 & Value) == 0) \
            { \
                Value <<= 1; \
                Value  |= 1; \
                foo++; \
            } \
            return foo; \
}

#define __builtin_expect(a,b) a
#define __builtin_clz(a) CountLeadingZeros(a)
#define __clz(a) CountLeadingZeros(a)

#endif //RVDS_2_2

#endif // VERSION_RVDS2_2_H

