/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/common/src/misc.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
/*
 * 
 */
#include "rtos/common.nmf"

/*
 * Void...
 */
void Void() {
    // Nothing to do
}

void Unbinded(void *returnAddr) {
    Panic(INTERFACE_NOT_BINDED, (t_uint24) returnAddr - 1);
}
