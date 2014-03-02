/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define BaseAddress 0xF0002800

#include "IPPhwReg.h"

volatile IPP_IP_ts *p_IPP_IP = (IPP_IP_ts *)(BaseAddress + 0x000);    //IPP

