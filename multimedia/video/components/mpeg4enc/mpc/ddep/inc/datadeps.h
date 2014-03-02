/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ADAPTERS_H_
#define _ADAPTERS_H_

asm long convLong24to16(long value) {
   mv @{value}, @{}
   L_msli @{}, #8, @{}
   asri @{}.0, #8, @{}.0
} 

asm long convLong16to24(long value) {
   mv @{value}, @{}
   asli @{}.0, #8, @{}.0
   L_lsri @{}, #8, @{}
} 

#endif /*_ADAPTERS_H_*/

