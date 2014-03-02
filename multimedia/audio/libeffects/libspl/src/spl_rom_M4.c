/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_rom_M4.c
 * \brief  
 * \author ST-Ericsson
 */
#include "spl_defines.h"
#ifdef M4
#include "audiolibs_common.h"
#include "spl_algo_M4.h"
#include "spl_rom_M4.h"
const SPL_EXTERN  long long PeakTab[] = {
	0, 17588033683456LL, 35065148997632LL, 52025752551424LL, 
	67927462117376LL, 82519827415040LL, 95950248869888LL, 108396636274688LL, 
	119646581686272LL, 129042007195648LL, 135829280260096LL, 139635116212224LL, 
	140737488355327LL, 140737488355327LL, 140737488355327LL, 140737488355327LL, 
	140737488355327LL
};
const SPL_EXTERN  int PeakDiffTab[] = {
	8386628, 8333738, 8087446, 7582526, 
	6958182, 6404124, 5934900, 5364392, 
	4480088, 3236424, 1814764, 525652, 
	0, 0, 0, 0, 
	0
};
#endif //if def M4

