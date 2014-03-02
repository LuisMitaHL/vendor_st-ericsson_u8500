/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _analysis_tables_h_
#define _analysis_tables_h_

#include "audiolibs_common.h"

#define ENWINDOW_LENGTH (512+1)

#ifdef REDUCE_MIPS_AND_TABLES
#define MODULATION_LENGTH 496
#else
#define MODULATION_LENGTH 544
#endif

extern Float const POLYANALYSIS_MEM
polyanalysis_enwindow[ENWINDOW_LENGTH];

extern Float const POLYANALYSIS_MEM
polyanalysis_subband_modulation[MODULATION_LENGTH]; /* size 496 or 544 */

extern Float const POLYANALYSIS_MEM * AUDIOLIBS_UPLOAD_MEM polyanalysis_p_enwindow;
extern Float const POLYANALYSIS_MEM * AUDIOLIBS_UPLOAD_MEM polyanalysis_p_subband_modulation;

#endif /* Do not edit below this line */

