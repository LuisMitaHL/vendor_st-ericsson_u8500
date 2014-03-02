/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __ITE_CONVF900_H
#define __ITE_CONVF900_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inc/type.h>


#ifdef __cplusplus
extern "C"
{
#endif

void ITE_ConvertTo16BitFloat(volatile float , unsigned short* );
void ITE_ConvertTo32BitFloat(unsigned short, float*);

#ifdef __cplusplus
}
#endif

#endif
