/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   comxilstspl.h
 * \brief  
 * \author ST-Ericsson
 */
#ifndef _spl_rom_h_
#define _spl_rom_h_


#ifndef __flexcc2__
#define SPL_EXTERN 
// #define SPL_EXTERN YMEM
#else
#define SPL_EXTERN EXTMEM
#endif

#define SPL_BASE_SIZE_PEAKTAB 4

#define SPL_SIZE_PEAKTAB (1+(1<<SPL_BASE_SIZE_PEAKTAB))

#define SPL_PEAK_DIFF_SHIFT 3

extern const SPL_EXTERN  MMlong PeakTab[SPL_SIZE_PEAKTAB];

extern const SPL_EXTERN  MMshort PeakDiffTab[SPL_SIZE_PEAKTAB];

#endif

