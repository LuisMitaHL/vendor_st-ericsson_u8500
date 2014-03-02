/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_VIDEOCODECTABLESANNEXT_H
#define INCLUSION_GUARD_VIDEOCODECTABLESANNEXT_H

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
extern const uint8 ChrominanceQuantTable[32];  // Located in vidSymbolDecH263.c
	/*
const uint8 ChrominanceQuantTable[32] =
{
  0, 1, 2, 3, 4, 5, 6,
  6, 7, 8,
  9, 9,
  10, 10,
  11, 11,
  12, 12, 12,
  13, 13, 13,
  14, 14, 14, 14, 14,
  15, 15, 15, 15, 15
};
*/
/*************************************************************************
* Declarations of functions
**************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECTABLESANNEXT_H
