/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
 * Memory Transfert FIFO commands using hardware loop => limited to transfer
 * size < 1023 16bit words.
 */
/*****************************************************************************/

/**
 * Includes							       
 */
#include "mtf_hwloop.h"
#ifndef T1XHV_SIA
#include "dma_api.h"   /* def of t_mtf_incr */
#else
#include "dma_sia_api.h"
#endif
#include "mmdsp_api.h"
#include "macros.h"

/* END of mtf_hwloop.c */
