/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HAMAC_TRANSFORMS_H
#define HAMAC_TRANSFORMS_H

#include "types.h"


void IDCT4x4(t_sint16 *p_b);
void Hadamard4x4(t_sint16 *p_b);
void Hadamard2x2(t_sint16 *p_b);

#endif
