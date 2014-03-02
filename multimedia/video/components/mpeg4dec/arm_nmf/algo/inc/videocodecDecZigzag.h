/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECDECZIGZAG_H
#define INCLUSION_GUARD_VIDEOCODECDECZIGZAG_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
* Includes
**************************************************************************/
#include "t_basicdefinitions.h"

/*************************************************************************
* Types, constants and external variables
**************************************************************************/
extern const uint8 InvZigZagTable[3][64];  // Located in vidSymbolDecH263.c
extern const uint8 ZigZagTable[64];        // Located in vidSymbolDecH263.c

/*************************************************************************
* Declarations of functions
**************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDECZIGZAG_H
