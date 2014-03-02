/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   24 to 16 bit mode transform facilities
*/

#ifndef _GRAB_ADAPTER_H_
#define _GRAB_ADAPTER_H_

asm long convLong24to16(long value) {
   dmvl @{value}, @{}
   L_msli @{}, #8, @{}
   asri @{}.0, #8, @{}.0
}

asm long convLong16to24(long value) {
   dmvl @{value}, @{}
   asli @{}.0, #8, @{}.0
   L_lsri @{}, #8, @{}
}

#endif /*_GRAB_ADAPTER_H_*/

