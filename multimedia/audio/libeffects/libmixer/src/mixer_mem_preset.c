/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mixer_mem_preset.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "mixer.h"

/* This file contains all the look up table for memory allocation */

/* first a global table for the effect must be defined. This table gather the memory preset
   supported by the effect .
   In our case we support 5 memory preset.
   Note that a extern reference to this table is defined in mixer.h because
   this table will be reused by the interface library
*/

MEMORY_PRESET_T const EXTERN mixer_mem_preset_table[MIXER_NB_MEM_PRESET]={
  MEM_ALL_TCM,
  MEM_ALL_DDR,
#ifdef __flexcc2__
  MEM_ALL_ESRAM,
#endif
};


