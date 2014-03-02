/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define BaseAddress_v1 0xF0000000

#include "PictorhwReg_v1.h"

volatile ISP_SMIARX_V1_IP_ts *p_ISP_SMIARX_V1_IP = (ISP_SMIARX_V1_IP_ts *)(BaseAddress_v1 + 0x500);

