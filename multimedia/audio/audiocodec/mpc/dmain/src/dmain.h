/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmain.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef  _DMAIN_H_
#define  _DMAIN_H_

#include <audiocodec/mpc/dmain.nmf>
#include "dma_common.h"
#include "dma_registers.h"
#include "msp_registers.h"
#include "samplesplayed.idt"


#define TOTAL_NB_PORTS            3

#define AVSOURCE_MSPSLOT          0

#define MULTIMIC_MSPSLOT          0

#define FMRX_MSPSLOT              6

#endif   // _DMAIN_H_
