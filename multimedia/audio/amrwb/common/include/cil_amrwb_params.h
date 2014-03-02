/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _cil_amrwb_params_h_
#define _cil_amrwb_params_h_
#include "audiolibs_types.h"

/* amr-wb dec */
typedef struct {
  t_uint16        iNoHeader;	// STATIC, range [0,1], default 0, magic number present
  t_uint16        iMemoryPreset;	// STATIC, 
									// 0(MEM_DEFAULT=MEM_ALL_TCM)  1(MEM_ALL_DDR) 2(MEM_ALL_TCM) 3(MEM_MIX_DDR_TCM_1) 8(MEM_ALL_ESRAM) 11(MEM_MIX_ESRAM_OTHER_1)
  t_uint16        iErrorConcealment;	// STATIC, range [0,1] default is 0 (off)
} t_dec_amrwb_params;

/* amr-wb enc */
typedef struct {
  t_uint16        iRateEnum;	//  DYNAMIC, range [0,9], default 8 (23.85kb/s)
  t_uint16        iDtxEnable;	//  DYNAMIC, range [0,1], default 1, dtx active
  t_uint16        iNoHeader;	//  STATIC, range [0,1], default 0, magic number present
  t_uint16        iMemoryPreset; // STATIC,
  // 0(MEM_DEFAULT=MEM_ALL_TCM)  1(MEM_ALL_DDR) 2(MEM_ALL_TCM) 3(MEM_MIX_DDR_TCM_1) 8(MEM_ALL_ESRAM) 11(MEM_MIX_ESRAM_OTHER_1)
} t_enc_amrwb_params;


#define ESAA_AMRWB_MAX_FRAME_SIZE_IN_BIT                    	(560)  // 72 bits magic number + 8bit header + 480 bits
#define ESAA_AMRWB_SAMPLE_FRAME_SIZE_IN_WORD                    320

#endif
