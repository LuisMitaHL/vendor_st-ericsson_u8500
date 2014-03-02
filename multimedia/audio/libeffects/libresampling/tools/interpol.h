/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   interpol.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cfuncts.h"
#define degree 7
#define NTAPS 89
#define MAXPHASE 16
#define NTAPSsinx 51
#define passratiopoly .455
#define MINDB 120.0
#define SRCBULKSIZ (2*NTAPS*(MAXPHASE+1))+NTAPSsinx+(((NTAPSsinx/2)+1)*(degree+1))
#define BLOCKSIZ 80
#define xmult 2147483647.0

