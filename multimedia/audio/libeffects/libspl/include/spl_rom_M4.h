/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/**
 * \file   spl_rom_M4.h
 * \brief  
 * \author ST-Ericsson
 */
#ifndef _spl_rom_M4_h_
#define _spl_rom_M4_h_
#define SPL_EXTERN 
#define SPL_BASE_SIZE_PEAKTAB 4
#define SPL_SIZE_PEAKTAB (1+(1<<SPL_BASE_SIZE_PEAKTAB))
#define SPL_PEAK_DIFF_SHIFT 3
extern const SPL_EXTERN  long long PeakTab[SPL_SIZE_PEAKTAB];
extern const SPL_EXTERN  int PeakDiffTab[SPL_SIZE_PEAKTAB];
#endif

