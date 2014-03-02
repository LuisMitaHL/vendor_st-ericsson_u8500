/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _iir_h_
#define _iir_h_

#include "audiolibs_common.h"

void
iir_first_order(Float *buffer,     /* address of the buffer */
		int len,           /* length of the buffer  */
		Float *coef,       /* order a0, a1, b1      */
		Float *context,    /* order x(n-1) y(n-1)   */
		int left_shift);   /* shift before rounding */

#endif /* Do not edit below this line */
