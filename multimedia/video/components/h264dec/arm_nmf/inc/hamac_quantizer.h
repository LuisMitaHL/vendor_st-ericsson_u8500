/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef HAMAC_QUANTIZER_H
#define HAMAC_QUANTIZER_H

#include "types.h"

#define QLUMADC     0
#define QLUMA4x4    1
#define QLUMA16x16  2
#define QCHROMADC   3
#define QCHROMA     4


void InvQuant(t_sint16 *p_b, t_uint16 qp, t_uint16 block_type);

#endif
